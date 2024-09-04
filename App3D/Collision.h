#pragma once

#define EDGE_LIMIT		8 //Max amount of edges we allow a convex hitbox to have (Not to be confused with the goon limit)
#define HIT_LIMIT		8 //Max amount of things the hitbox can hit
#define HITBOX_LIMIT	4 //Max amount of hitboxes one collider can have


#include <DirectXCollision.h>
#include <vector>

#include "D3D11MathHelper.h"

class CollisionHandler
{
public:
	//CollisionHandler();

	//void TestCollisions();
private:
	//std::vector<Drawable> m_objects;
};

//Cameras will have these
struct SphereCollider
{
	DirectX::XMFLOAT3 center;
	float radius;
};

//Walls will have these (axis-aligned, because we'll never have tilted walls in this simulation, doing a bunch of separating axis theorem for this demonstration is overkill)
struct BoxCollider
{
	DirectX::XMFLOAT3 min, max; //extents
};

//Returns the closest point on the box in comparison to a given point
DirectX::XMFLOAT3& ClosestPointOnBox(const DirectX::XMFLOAT3& point, const BoxCollider& box);

//Check and resolve (either pass in drawable position so we can modify that as well, or have resolve return an XMFLOAT3 for the new position of the object being pushed out)
bool CheckCollisionBoxSphere(const BoxCollider& box, const SphereCollider& sphere);
void ResolveCollisionBoxSphere(const BoxCollider& box, const SphereCollider& sphere);


/*
//Convex Collision stuff using Separated Axis Theorem (SAT)

struct CollisionParams
{
	//EntityID entity1;
	//EntityID entity2;

	//ID's
	int hitbox1;
	int hitbox2;

	//Positions
	DirectX::XMFLOAT3 hitbox1Pos;
	DirectX::XMFLOAT3 hitbox2Pos;

	//Normals
	DirectX::XMFLOAT3 hitbox1Nor;
	DirectX::XMFLOAT3 hitbox2Nor;
};

struct OnCollision
{
	void(*CollisionFunction)(CollisionParams& params);
};

struct CircleHitbox
{
	float radius;
	DirectX::XMFLOAT3 offset;
};

struct ConvexHitbox
{
	//
	float radius; //Make-believe spherical bounding area, because calculations against spheres are better in terms of performance
	int nCorners;

	//(XMFLOAT3's need to be last, uhhhh, byte alignment, uhhhhh????)
	DirectX::XMFLOAT3 center;

	DirectX::XMFLOAT3 corners[EDGE_LIMIT];
	DirectX::XMFLOAT3 normals[EDGE_LIMIT];
};

struct HitboxFlags
{
	//IS THE HITBOX ACTIVE?
	bool isActive = true;
	
	//WHAT TYPE OF HITBOX IS IT?
	bool isWall = true;

	//WHAT HAS THE HITBOX HIT?
	bool hitWall = true;

	//WHAT PROPERTIES DOES THE HITBOX HAVE?
	bool isMoveable = true;

	//void ResetAsActive();
	void Activate();
};

//This is the actual struct we associate with an entity (drawable)
struct Collider
{
	int nMoveableCollisions; //Not currently sure why I need this but trust the process

	//Create flags and hitboxes, initialize all to 0
	//unsigned nCircular; //Bits checking which hitboxes are in use
	/*HitboxFlags circularFlags[HITBOX_LIMIT] = { 0 };
	OnCollision onCircularCollision[HITBOX_LIMIT] = { 0 };
	CircleHitbox circleHitbox[HITBOX_LIMIT] = { 0 };
*/
	
/*
	//unsigned nConvex; //Bits checking which hitboxes are in use
	//HitboxFlags convexFlags[HITBOX_LIMIT] = {0};
	//OnCollision onConvexCollision[HITBOX_LIMIT] = {0};
	//ConvexHitbox convexHitbox[HITBOX_LIMIT] = {0};
	HitboxFlags convexFlags = { 0 };
	OnCollision onConvexCollision = { 0 };
	ConvexHitbox convexHitbox = { 0 }; //I've got my eye on you
	
	DirectX::XMFLOAT3 offsetX = { 1.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 offsetY = { 0.0f, 1.0f, 0.0f };
	DirectX::XMFLOAT3 offsetZ = { 0.0f, 0.0f, 1.0f };

	Collider(); //Constructor (let it set convexhitbox stuff manually because I don't trust like that
};

int InitCollider(Collider& collider, int nCorners, DirectX::XMFLOAT3 cornerPos[]);

//Okay here's the actual functions xd
bool TestConvexCollision(Collider& c1, Collider& c2, DirectX::XMFLOAT3 pos1, DirectX::XMFLOAT3 pos2); //checks collision between 2 convex hitboxes

float RotateOffset(float offsetX, float offsetZ, float xFactor, float zFactor);
*/