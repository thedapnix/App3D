#pragma once

//Include other colliders and pray that surely we won't get any include rips
#include "ColliderSphere.h"
//#include "ColliderAABB.h"

//
class ColliderPlane
{
public:
	ColliderPlane(const float& dist, const DirectX::XMFLOAT3& nor) :
		m_distance(dist), m_normal(nor) {}

	//
	ColliderPlane Normalized() const;
	IntersectData IntersectSphere(const ColliderSphere& other) const;
	//IntersectData IntersectAABB(const ColliderAABB& other) const;


	inline const float& GetDistance() const { return m_distance; }
	inline const DirectX::XMFLOAT3& GetNormal() const { return m_normal; }

private:
	float m_distance;
	DirectX::XMFLOAT3 m_normal;
};