#pragma once

#include <memory>
#include <vector>

#include <DirectXMath.h>
#include <DirectXCollision.h>

//Liberal use of quoting the cookbook here because I'm not a big fan of trees
//Crashes if I try having function definitions in a cpp file. Love templates.

static constexpr int maxElements = 16;
static constexpr int maxHeight = 80;
static constexpr int minHeight = -80;

template<typename T>
class QuadTree
{
private:
	struct Node
	{
		std::vector<const T*> elements; //Cookbook shows this as just a single "T* element" but that won't do
		DirectX::BoundingBox aabb; //"Each node, both parent and leaf, will need to know the spatial volume in the scene that they cover"
		std::unique_ptr<Node> children[4];
	};
public:
	void AddElement(const T* element, const DirectX::BoundingBox& aabb);
	std::vector<const T*> CheckTree(const DirectX::BoundingFrustum& frustum);

private:
	void AddToNode(const T* element, const DirectX::BoundingBox& aabb, std::unique_ptr<Node>& node);
	void CheckNode(const DirectX::BoundingFrustum& frustum, const std::unique_ptr<Node>& node, std::vector<const T*>& foundObjects);

	std::unique_ptr<Node> m_root;
};

template<typename T>
void QuadTree<T>::AddElement(const T* element, const DirectX::BoundingBox& aabb)
{
	//Can't add to root if root is nullptr so
	if (m_root == nullptr)
	{
		m_root = std::make_unique<Node>();
		DirectX::BoundingBox rootAABB = { { minHeight, minHeight, minHeight }, { maxHeight, maxHeight, maxHeight } };
		m_root->aabb = rootAABB;
	}
	AddToNode(element, aabb, m_root);
}

template<typename T>
std::vector<const T*> QuadTree<T>::CheckTree(const DirectX::BoundingFrustum& frustum)
{
	std::vector<const T*> toReturn;
	CheckNode(frustum, m_root, toReturn);

	return toReturn;
}

template<typename T>
void QuadTree<T>::AddToNode(const T* element, const DirectX::BoundingBox& aabb, std::unique_ptr<Node>& node)
{
	//"When a new object is added to the tree, a bounding volume associated with it needs to be present
	//The process then starts with the root node, checking if the node's bounding volume and the object's bounding volume collide in some way
	//If they do not, then this node and all of its child nodes are of no interest, and can be ignored/discarded. If a collision is detected, then further processing of that node is needed"
	if (!node->aabb.Intersects(aabb))
	{
		return;
	}

	//"If a collided with node is not a leaf node, then it should not contain any information about objects"
	if (node->children[0].get() == nullptr) //Doesn't have leaves = is a leaf
	{
		//"In the case that there is room in a collided with leaf node, then the object and any required information can be inserted"
		if (node->elements.size() < maxElements)
		{
			node->elements.push_back(element);
			return;
		}
		else
		{
			//"However, if the node is already "full", then that requires special logic
			//In this case, the "full" leaf node should be turned into an empty parent node by adding children to it, and then moving its currently stored objects to its new child nodes"
			DirectX::XMFLOAT3 center = node->aabb.Center;
			DirectX::XMFLOAT3 extents = node->aabb.Extents;

			//"For a quadtree, one of the axes(often the Y-axis if it represents the "height" in a scene) covers the whole scene and no partitioning is done for it"
			//So we subdivide space into 4 quadrants along x- and z-axes (this part hurts my brain)
			float minX = center.x - extents.x;
			float maxX = center.x + extents.x;
			float minZ = center.z - extents.z;
			float maxZ = center.z + extents.z;
			DirectX::BoundingBox nearLeft;
			nearLeft.CreateFromPoints(nearLeft,
				{ center.x, maxHeight, center.z },
				{ minX, minHeight, minZ });

			DirectX::BoundingBox nearRight;
			nearRight.CreateFromPoints(nearRight,
				{ center.x, maxHeight, center.z },
				{ maxX, minHeight, minZ });

			DirectX::BoundingBox farLeft;
			farLeft.CreateFromPoints(farLeft,
				{ center.x, maxHeight, center.z },
				{ minX, minHeight, maxZ });

			DirectX::BoundingBox farRight;
			farRight.CreateFromPoints(farRight,
				{ center.x, maxHeight, center.z },
				{ maxX, minHeight, maxZ });

			node->children[0] = std::make_unique<Node>();
			node->children[0]->aabb = nearLeft;
			node->children[1] = std::make_unique<Node>();
			node->children[1]->aabb = nearRight;
			node->children[2] = std::make_unique<Node>();
			node->children[2]->aabb = farLeft;
			node->children[3] = std::make_unique<Node>();
			node->children[3]->aabb = farRight;

			//"For each of the currently stored elements in this node, attempt to add them to the new child nodes"
			for (const auto& element : node->elements)
			{
				for (int i = 0; i < 4; i++)
				{
					if (node->children[i]->aabb.Intersects(element->GetBoundingBox()))
					{
						node->children[i]->elements.push_back(element);
					}
				}
			}
			node->elements.clear(); //I'VE FORGOTTEN TO CLEAR
		}
	}
	//"Once each child node has been correctly created and the previously stored elements have been inserted into them correctly, the recursive process can simply be done with the new element and each of the new child nodes"
	for (int i = 0; i < 4; i++)
	{
		AddToNode(element, aabb, node->children[i]);
	}
}

template<typename T>
void QuadTree<T>::CheckNode(const DirectX::BoundingFrustum& frustum, const std::unique_ptr<Node>& node, std::vector<const T*>& foundObjects)
{
	//Check if there is a collision between the frustum and the node's bounding volume
	if (!frustum.Intersects(node->aabb)) //No collision found, node and potential children not relevant
	{
		return;
	}

	if (node->children[0].get() == NULL) //Check if node is a leaf
	{
		for (const auto& element : node->elements)
		{
			//Transform frustum to object local space
			DirectX::XMVECTOR detWorld = DirectX::XMMatrixDeterminant(element->World());
			DirectX::XMMATRIX invWorld = DirectX::XMMatrixInverse(&detWorld, element->World());
			DirectX::BoundingFrustum localFrustum = frustum;
			localFrustum.Transform(localFrustum, invWorld);
			if (localFrustum.Intersects(element->GetBoundingBox()))
			{
				//Check if the object is already present in the return vector, add if not
				//https://stackoverflow.com/questions/3450860/check-if-a-stdvector-contains-a-certain-object thank you stackoverflow
				if (std::find(foundObjects.begin(), foundObjects.end(), element) == foundObjects.end())
				{
					foundObjects.push_back(element);
				}
			}
		}
	}
	else
	{
		//Recursively run this function for each of the child nodes of this node
		for (int i = 0; i < 4; i++)
		{
			CheckNode(frustum, node->children[i], foundObjects);
		}
	}
}