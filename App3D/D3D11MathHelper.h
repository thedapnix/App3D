#pragma once

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <DirectXMath.h>

#include <cmath>

//Helper functions (idk if i'll need this rn but this class will get worked on)
//void UpdateWorldMatrix(DirectX::XMMATRIX& world, float dt);

static DirectX::XMFLOAT4X4 Identity4x4()
{
	static DirectX::XMFLOAT4X4 I(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	return I;
}

/*VERY BASIC XMFLOAT3 (VECTOR) CALCULATION HELPER FUNCTIONS*/

//Add two vectors together
DirectX::XMFLOAT3& XmFloat3Add(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs);

//Subtract one vector by another
DirectX::XMFLOAT3& XmFloat3Subtract(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs);

//Multiply every value in a vector by some float value
DirectX::XMFLOAT3& XmFloat3Multiply(const DirectX::XMFLOAT3& lhs, const float& rhs);

//Divide every value in a vector by some float value
DirectX::XMFLOAT3& XmFloat3Divide(const DirectX::XMFLOAT3& lhs, const float& rhs);

//Raise every value in a vector to the power of some given number
DirectX::XMFLOAT3& XmFloat3Pow(const DirectX::XMFLOAT3& in, int pow);

//Get the length of a vector
float& XmFloat3Length(const DirectX::XMFLOAT3& in);

//Normalize a vector
DirectX::XMFLOAT3& XmFloat3Norm(const DirectX::XMFLOAT3& in);

//Returns a vector with the greatest x-, y-, and z-values between two vectors
DirectX::XMFLOAT3& XmFloat3Max(const DirectX::XMFLOAT3& in1, const DirectX::XMFLOAT3& in2);

//Gets the greatest x-, y-, or z-component from a vector
float XmFloat3MaxValue(const DirectX::XMFLOAT3& in);

float XmFloat3Dot(const DirectX::XMFLOAT3& in1, const DirectX::XMFLOAT3& in2);

//Returns true if the two inputs are the same
bool XmFloat3Compare(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs);