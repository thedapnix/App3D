#pragma once

#include "D3D11MathHelper.h"

struct IntersectData
{
	//Things we may want to know when intersections happen
	bool didIntersect = false;								//Did an intersection occur?
	float intersectDist = 0.0f;								//How much of an intersection has occurred?
	DirectX::XMFLOAT3 contactNormal = {0.0f, 0.0f, 0.0f};	//What's the normal of the spot we intersected with?
};