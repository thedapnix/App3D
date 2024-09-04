#include "Collision.h"

#include <algorithm> //min and max

DirectX::XMFLOAT3& ClosestPointOnBox(const DirectX::XMFLOAT3& point, const BoxCollider& box)
{
	DirectX::XMFLOAT3 toReturn;

	toReturn.x = std::max(box.min.x, std::min(point.x, box.max.x));
	toReturn.y = std::max(box.min.y, std::min(point.y, box.max.y));
	toReturn.z = std::max(box.min.z, std::min(point.z, box.max.z));

	return toReturn;
}

bool CheckCollisionBoxSphere(const BoxCollider& box, const SphereCollider& sphere)
{
	//Get the closest point on the box, compared against the sphere's centerpoint
	DirectX::XMFLOAT3 closest = ClosestPointOnBox(sphere.center, box);

	//Get the distance between the two points
	DirectX::XMFLOAT3 dist = XmFloat3Subtract(sphere.center, closest);

	//Collision has occurred if the distance between the two points is lesser or equal to the radius of the sphere
	return XmFloat3Length(dist) <= sphere.radius;
}

void ResolveCollisionBoxSphere(const BoxCollider& box, SphereCollider& sphere)
{
	//I refuse to believe that this is enough (obviously we need to consider other things, such as the actual position of the camera being moved instead of just some abstract bounding sphere)
	DirectX::XMFLOAT3 closest = ClosestPointOnBox(sphere.center, box);
	DirectX::XMFLOAT3 dist = XmFloat3Subtract(sphere.center, closest);

	if (XmFloat3Length(dist) < sphere.radius)
	{
		DirectX::XMFLOAT3 push = XmFloat3Norm(dist);
		
		DirectX::XMFLOAT3 pushMultRad = XmFloat3Multiply(push, sphere.radius);
		sphere.center = XmFloat3Add(closest, pushMultRad);
	}
}

/*
using namespace DirectX;

//void HitboxFlags::ResetAsActive()
//{
//	unsigned short* flags = (unsigned short*)this;
//	*flags = 0;
//	this->isActive = true;
//}

void HitboxFlags::Activate()
{
	this->isActive = true;
}

Collider::Collider()
{
	//Zero everything
	//this->nCircular = 0;
	//this->nConvex = 0;
	//long long* flags = (long long*)this->circularFlags;	//Point to the memory of circularFlags
	//*flags = 0;											//Dereference memory and set everything to 0
	this->nMoveableCollisions = 0;

	//No more use of arrays, so long long* casting is no longer valid
	unsigned short* flags = (unsigned short*)&this->convexFlags;
	*flags = 0;
	//this->convexFlags.isActive = false;
	//this->convexFlags.isWall = false;
	//this->convexFlags.hitWall = false;
	//this->convexFlags.isMoveable = false;
}

void SetCollisionEvent(Collider& collider, void* func)
{

}

int InitCollider(Collider& collider, int nCorners, DirectX::XMFLOAT3 cornerPos[])
{
	if (nCorners < 3 || nCorners > EDGE_LIMIT)
		return -1;

	XMFLOAT3 sum = { 0.0f, 0.0f, 0.0f }; //For summing up all the incoming corner positions

	//Set corner positions
	for (int i = 0; i < nCorners; i++)
	{
		collider.convexHitbox.corners[i] = cornerPos[i];

		//
		sum = XmFloat3Add(sum, cornerPos[i]);
		//sum.x += cornerPos[i].x;
		//sum.y += cornerPos[i].y;
		//sum.z += cornerPos[i].z;
	}

	//Set number of corners
	collider.convexHitbox.nCorners = nCorners;

	//Calculate and set center by taking the sum of all corner positions and dividing by the median (which in this case is the amount of corners)
	collider.convexHitbox.center = XmFloat3Divide(sum, (float)nCorners);
	//collider.convexHitbox.center.x = sum.x / (float)nCorners;
	//collider.convexHitbox.center.y = sum.y / (float)nCorners;
	//collider.convexHitbox.center.z = sum.z / (float)nCorners;

	//Calculate and set bounding radius (performance reasons, cheaper to do an initial spherical collision check before we do SAT)
	float longestDist = 0.0f;
	for (int i = 0; i < nCorners; i++)
	{
		XMFLOAT3 d = { 0.0f, 0.0f, 0.0f };
		d = XmFloat3Subtract(cornerPos[i], collider.convexHitbox.center);
		d = XmFloat3Pow(d, 2);
		//d.x = cornerPos[i].x - collider.convexHitbox.center.x;
		//d.x = d.x * d.x;

		//d.y = cornerPos[i].y - collider.convexHitbox.center.y;
		//d.y = d.y * d.y;

		//d.z = cornerPos[i].z - collider.convexHitbox.center.z;
		//d.z = d.z * d.z;

		//float tempDist = d.x * d.x + d.y * d.y + d.z * d.z;
		float tempDist = d.x + d.y + d.z; //This feels more appropriate, no?

		if (tempDist > longestDist)
		{
			longestDist = tempDist;
		}
	}
	collider.convexHitbox.radius = std::sqrt(longestDist);

	//Calculate and set normals
	XMFLOAT3 l = { 0.0f, 0.0f, 0.0f };	//line
	float s = 0.0f;						//scalar
	for (int i = 0; i < nCorners; i++)
	{
		//You'd think I would have made my own header for XMFLOAT3 mathematics at this point huh (Edit: I did)
		//Get line from one corner to another
		l = XmFloat3Subtract(cornerPos[(i + 1) % nCorners], cornerPos[i]);
		//l.x = cornerPos[(i + 1) % nCorners].x - cornerPos[i].x;
		//l.y = cornerPos[(i + 1) % nCorners].y - cornerPos[i].y;
		//l.z = cornerPos[(i + 1) % nCorners].z - cornerPos[i].z;

		//Rotate the line by 90 degrees to get the normal (but why is damned soul backend 2d hardcoded)
		collider.convexHitbox.normals[i].x = -l.z;
		collider.convexHitbox.normals[i].y =  l.y; //I think? Or negative (prev l.x)
		collider.convexHitbox.normals[i].z =  l.x;

		//Get line from center to corner
		l = XmFloat3Subtract(cornerPos[i], collider.convexHitbox.center);

		//Calculate scalar
		s = l.x * collider.convexHitbox.normals[i].x +
			l.y * collider.convexHitbox.normals[i].y +
			l.z * collider.convexHitbox.normals[i].z
			;

		//Invert normal directions if scalar is negative
		if (s < 0.0f)
			collider.convexHitbox.normals[i] = XmFloat3Multiply(collider.convexHitbox.normals[i], -1.0f);

		//Normalize the normals, as you do
		collider.convexHitbox.normals[i] = XmFloat3Norm(collider.convexHitbox.normals[i]); //Silly syntax but hey it's better than the alternative, hate DirectX math
	}

	//Assuming everything was set properly (which it definitely fucking won't), activate the hitbox and consider it ready for use
	collider.convexFlags.Activate();

	//We not doing this rn, all collisions are simple
	//SetCollisionEvent(collider, NoCollision);

	return 1; //Yipee
}

float RotateOffset(float offsetX, float offsetZ, float xFactor, float zFactor)
{
	return offsetX * xFactor +
		offsetZ * zFactor;
}

struct ConvexReturnData
{
	bool hit;
	float line1X, line1Z, line2X, line2Z;
};

ConvexReturnData LineToLineIntersection(float l1c1x, float l1c1z, float l1c2x, float l1c2z, float l2c1x, float l2c1z, float l2c2x, float l2c2z)
{
	//Get denominator to get where on line the intersection is from 0 to 1
	float denominator = (l2c2x - l2c1x) * (l1c1z - l1c2z) - (l1c1x - l1c2x) * (l2c2z - l2c1z);

	//Get where on the lines the intersection happens, use denominator to get to 0 to 1 if on line
	float whereOnLine1 = ((l2c1z - l2c2z) * (l1c1x - l2c1x) + (l2c2x - l2c1x) * (l1c1z - l2c1z)) / denominator;
	float whereOnLine2 = ((l1c1z - l1c2z) * (l1c1x - l2c1x) + (l1c2x - l1c1x) * (l1c1z - l2c1z)) / denominator;
	ConvexReturnData toReturn = { 0 };
	//Check if the intersection happens between the corners of the lines
	//if outside of 0 and 1 the intersection is outside of convex shape
	if (whereOnLine1 >= 0.0f && whereOnLine1 < 1.0f && whereOnLine2 >= 0.0f && whereOnLine2 < 1.0f)
	{
		toReturn.line1X = (1.0f - whereOnLine1) * (l1c2x - l1c1x);
		toReturn.line1Z = (1.0f - whereOnLine1) * (l1c2z - l1c1z);
		toReturn.line2X = (1.0f - whereOnLine2) * (l2c2x - l2c1x);
		toReturn.line2Z = (1.0f - whereOnLine2) * (l2c2z - l2c1z);
		toReturn.hit = true;
	}
	return toReturn;
}

bool TestConvexCollision(Collider& c1, Collider& c2, DirectX::XMFLOAT3 pos1, DirectX::XMFLOAT3 pos2)
{
	XMFLOAT3 p1 = pos1;
	XMFLOAT3 p2 = pos2;

	//Bitmask circle1 is to circle2 hit
	unsigned short iSmask = 0b0000000001111110;// is mask
	unsigned short hitMask = 0b0001111110000000;
	unsigned short* r1 = (unsigned short*)&c1.convexFlags;
	unsigned short* r2 = (unsigned short*)&c2.convexFlags;

	bool iShit1 = ((*r1 & iSmask) << 6) & (*r2 & hitMask);//convex1 hit convex2. is hit
	bool iShit2 = ((*r2 & iSmask) << 6) & (*r1 & hitMask);//convex2 hit convex1. is hit

	//If none can interact return false
	if (!iShit1 && !iShit2)
	{
		return false;
	}

	//First check if bounding radius reach each other (fuck offsets)
	//float posX = pos1x + RotateOffset(center1X, center1Z, convex1->offsetXx, convex1->offsetXz);
	//float posZ = pos1z + RotateOffset(center1Z, center1X, convex1->offsetZz, convex1->offsetZx);

	//float pos2X = pos2x + RotateOffset(center2X, center2Z, convex2->offsetXx, convex2->offsetXz);
	//float pos2Z = pos2z + RotateOffset(center2Z, center2X, convex2->offsetZz, convex2->offsetZx);

	//float dx = posX - pos2X;
	//float dz = posZ - pos2Z;

	float dx = pos1.x - pos2.x;
	float dy = pos1.y - pos2.y;
	float dz = pos1.z - pos2.z;


	//If distance from each other is greater than radi, no circular collision, skip. 
	//(I swear isn't it less costly to just make a vector between both centerpoints and check if THAT is greater than the two radii?)
	if (sqrtf(dx * dx + dy * dy + dz * dz) > (c1.convexHitbox.radius) + (c2.convexHitbox.radius))
	{
		return false;
	}
	ConvexReturnData returnedData = { 0 };
	int whichLine = 0;
	//Now we can check which lines collide
	for (int i = 0; i < c1.convexHitbox.nCorners; i++)
	{
		for (int j = 0; j < c2.convexHitbox.nCorners; j++)
		{
			//Check from center to corner of one convex shape onto the sides of another.
			//This is separated axis theorem no?
			returnedData = LineToLineIntersection(
				pos1.x + RotateOffset(pos1.x, pos1.z, c1.offsetX.x, c1.offsetX.z),
				pos1.z + RotateOffset(pos1.z, pos1.x, c1.offsetZ.z, c1.offsetZ.x),

				pos1.x + RotateOffset(c1.convexHitbox.corners[i].x, c1.convexHitbox.corners[i].z, c1.offsetX.x, c1.offsetX.z),
				pos1.z + RotateOffset(c1.convexHitbox.corners[i].z, c1.convexHitbox.corners[i].x, c1.offsetZ.z, c1.offsetZ.x),

				pos2.x + RotateOffset(c2.convexHitbox.corners[j].x, c2.convexHitbox.corners[j].z, c2.offsetX.x, c2.offsetX.z),
				pos2.z + RotateOffset(c2.convexHitbox.corners[j].z, c2.convexHitbox.corners[j].x, c2.offsetZ.z, c2.offsetZ.x),

				pos2.x + RotateOffset(c2.convexHitbox.corners[(1 + j) % EDGE_LIMIT].x, c2.convexHitbox.corners[(1 + j) % EDGE_LIMIT].z, c2.offsetX.x, c2.offsetX.z),
				pos2.z + RotateOffset(c2.convexHitbox.corners[(1 + j) % EDGE_LIMIT].z, c2.convexHitbox.corners[(1 + j) % EDGE_LIMIT].x, c2.offsetZ.z, c2.offsetZ.x)
			);

			if (returnedData.hit)
			{
				whichLine = j;
				//If we add convex hurtboxes for enemies we can add moving them correctly here.
				break;
			}
		}
		if (returnedData.hit)
		{
			//If we add convex hurtboxes for enemies we can add moving them correctly here.
			break;
		}
	}
	CollisionParams params = {};

	if (iShit2)
	{
		//params.entity1 = entity1;
		//params.entity2 = entity2;
		params.hitbox1 = 1 + HITBOX_LIMIT;
		params.hitbox2 = 2 + HITBOX_LIMIT;
		params.hitbox1Nor = c2.convexHitbox.normals[whichLine];
		params.hitbox2Nor = c2.convexHitbox.normals[whichLine];

		params.hitbox1Nor.x *= -1;
		params.hitbox1Nor.y *= -1;
		params.hitbox1Nor.z *= -1;

		c1.onConvexCollision.CollisionFunction(params);
	}
	if (iShit1)
	{
		//params.entity1 = entity2;
		//params.entity2 = entity1;
		params.hitbox1 = 1 + HITBOX_LIMIT;
		params.hitbox2 = 2 + HITBOX_LIMIT;
		params.hitbox1Nor = c2.convexHitbox.normals[whichLine];
		params.hitbox2Nor = c2.convexHitbox.normals[whichLine];
		
		params.hitbox2Nor.x *= -1;
		params.hitbox2Nor.y *= -1;
		params.hitbox2Nor.z *= -1;

		c2.onConvexCollision.CollisionFunction(params);
	}

	return returnedData.hit;
}
*/
