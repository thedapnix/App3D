#include "ColliderSphere.h"

IntersectData ColliderSphere::IntersectSphere(const ColliderSphere& other)
{
	IntersectData data;

	float totalRadius = this->m_radius + other.GetRadius();
	float dist = XmFloat3Length(XmFloat3Subtract(other.GetCenter(), this->m_center));

	if (dist < totalRadius)
	{
		data.didIntersect = true;
		data.intersectDist = dist - totalRadius;
	}

	return data; //didIntersect is already defaulted to false so we don't bother wasting time doing calculations

}
