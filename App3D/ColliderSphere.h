#pragma once

#include "CollisionDataStruct.h"

//Buh
class ColliderSphere
{
public:
	ColliderSphere(const DirectX::XMFLOAT3& center, const float& radius) :
		m_center(center), m_radius(radius) {}

	//Going to disconnect functions like these later but guhh
	IntersectData IntersectSphere(const ColliderSphere& other);

	inline const DirectX::XMFLOAT3& GetCenter() const { return m_center; }
	inline const float& GetRadius() const { return m_radius; }

private:
	DirectX::XMFLOAT3 m_center;
	float m_radius;
};