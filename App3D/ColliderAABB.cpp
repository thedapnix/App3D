#include "ColliderAABB.h"

using namespace DirectX;

IntersectData ColliderAABB::IntersectAABB(const ColliderAABB& other)
{
    IntersectData data;

    //Depending on the location of the boxes, just checking for distance in one direction might give us the wrong result
    XMFLOAT3 dist1 = XmFloat3Subtract(other.GetMin(), this->m_max);
    XMFLOAT3 dist2 = XmFloat3Subtract(this->m_min, other.GetMax());

    //Get the greatest values of dist1 and dist2 to determine which distance is the correct one
    XMFLOAT3 maxDist = XmFloat3Max(dist1, dist2);
    float maxDistF = XmFloat3MaxValue(maxDist);

    if (maxDistF < 0.0f)
    {
        data.didIntersect = true;
        data.intersectDist = maxDistF; //Maybe get this as absolute value instead
    }

    return data; //didIntersect is already defaulted to false so we don't bother wasting time doing calculations
}
