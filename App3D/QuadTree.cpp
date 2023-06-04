#include "QuadTree.h"

template<typename T>
void QuadTree<T>::AddElement(const T* element, const DirectX::BoundingBox& aabb)
{
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
	if (node->children[0].get() == NULL) //Doesn't have leaves = is a leaf
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
			DirectX::XMFLOAT2 center = node->aabb.Center;
			DirectX::XMFLOAT2 extents = node->aabb.Extents;

			//"For a quadtree, one of the axes(often the Y-axis if it represents the "height" in a scene) covers the whole scene and no partitioning is done for it"
			//So we subdivide space into 4 quadrants along x- and z-axes (this part hurts my brain)
			DirectX::BoundingBox nearLeft = DirectX::BoundingBox::CreateFromPoints(nearLeft, 
				{center.x, minHeight, center.z},
				{ center.x - extents.x, maxHeight, center.z - extents.z });

			DirectX::BoundingBox nearRight = DirectX::BoundingBox::CreateFromPoints(nearRight,
				{ center.x, minHeight, center.z },
				{ center.x + extents.x, maxHeight, center.z - extents.z });

			DirectX::BoundingBox farLeft = DirectX::BoundingBox::CreateFromPoints(nearRight,
				{ center.x, minHeight, center.z },
				{ center.x - extents.x, maxHeight, center.z + extents.z });

			DirectX::BoundingBox farRight = DirectX::BoundingBox::CreateFromPoints(nearRight,
				{ center.x, minHeight, center.z },
				{ center.x + extents.x, maxHeight, center.z + extents.z });

			node->children[0] = std::make_unique<Node>();
			node->children[0]->aabb = nearLeft;
			node->children[1] = std::make_unique<Node>();
			node->children[1]->aabb = nearRight;
			node->children[2] = std::make_unique<Node>();
			node->children[2]->aabb = farLeft;
			node->children[3] = std::make_unique<Node>();
			node->children[3]->aabb = farRight;

			//"For each of the currently stored elements in this node, attempt to add them to the new child nodes"
			for (auto& element : node->elements)
			{
				for (int i = 0; i < 4; i++)
				{
					if (node->children[i].aabb.Intersects(element->GetBoundingBox()))
					{
						node->children[i].elements.push_back(element);
					}
				}
			}
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
		for (auto& element : node->elements) 
		{
			if (frustum.Intersects(element->GetBoundingBox()))
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
