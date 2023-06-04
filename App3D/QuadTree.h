#pragma once

#include <memory>
#include <vector>
#include <DirectXCollision.h>

//Liberal use of quoting the cookbook here because I'm not a big fan of trees

template<typename T>
class QuadTree
{
private:
	struct Node
	{
		T element;
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

	static constexpr int maxElements = 8;
	static constexpr int maxHeight = 80;
	static constexpr int minHeight = -80;
};