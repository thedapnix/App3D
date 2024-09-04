#include "ColliderPlane.h"

using namespace DirectX;

ColliderPlane ColliderPlane::Normalized() const
{
    float mag = XmFloat3Length(m_normal);
    
    return ColliderPlane(m_distance / mag, XmFloat3Divide(m_normal, mag));
}

IntersectData ColliderPlane::IntersectSphere(const ColliderSphere& other) const
{
    IntersectData data;

    //Get distance from plane to sphere center
    float dist = fabs(XmFloat3Dot(m_normal, other.GetCenter()) + m_distance);
    dist -= other.GetRadius();

    if (dist < 0.0f)
    {
        data.didIntersect = true;
        data.intersectDist = dist;
    }

    return data; //didIntersect is already defaulted to false so we don't bother wasting time doing calculations
}
