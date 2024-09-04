#pragma once

#include "CollisionDataStruct.h"

//
class ColliderAABB
{
public:
	ColliderAABB(const DirectX::XMFLOAT3& min, const DirectX::XMFLOAT3& max) :
		m_min(min), m_max(max) {}

	//
	IntersectData IntersectAABB(const ColliderAABB& other);

	inline const DirectX::XMFLOAT3& GetMin() const { return m_min; }
	inline const DirectX::XMFLOAT3& GetMax() const { return m_max; }

private:
	DirectX::XMFLOAT3 m_min; //Extents (distances from the center point)
	DirectX::XMFLOAT3 m_max;
};