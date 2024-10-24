#include "D3D11MathHelper.h"

DirectX::XMFLOAT3& XmFloat3Add(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
{
	DirectX::XMFLOAT3 vec(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
	return vec;
}

DirectX::XMFLOAT3& XmFloat3Subtract(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
{
	DirectX::XMFLOAT3 vec(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
	return vec;
}

//DirectX::XMFLOAT3& XmFloat3Multiply(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
//{
//	DirectX::XMFLOAT3 vec(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
//	return vec;
//}

DirectX::XMFLOAT3& XmFloat3Multiply(const DirectX::XMFLOAT3& lhs, const float& rhs)
{
	DirectX::XMFLOAT3 vec(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
	return vec;
}

//DirectX::XMFLOAT3& XmFloat3Divide(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
//{
//	DirectX::XMFLOAT3 vec(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
//	return vec;
//}

DirectX::XMFLOAT3& XmFloat3Divide(const DirectX::XMFLOAT3& lhs, const float& rhs)
{
	DirectX::XMFLOAT3 vec(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
	return vec;
}

DirectX::XMFLOAT3& XmFloat3Pow(const DirectX::XMFLOAT3& in, int pow)
{
	DirectX::XMFLOAT3 vec = in;
	for (int i = 0; i < pow - 1; i++) //oops, forgot to -1
	{
		vec.x *= vec.x;
		vec.y *= vec.y;
		vec.z *= vec.z;
	}
	return vec;
}

float& XmFloat3Length(const DirectX::XMFLOAT3& in)
{
	DirectX::XMFLOAT3 vec = in;
	float mag = sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);

	return mag;
}

DirectX::XMFLOAT3& XmFloat3Norm(const DirectX::XMFLOAT3& in)
{
	//Don't mess with the original data
	DirectX::XMFLOAT3 vec = in;

	//Raise to the power of 2 then take the square root to get magnitude (absolute value, length of the vector)
	//vec = XmFloat3Pow(vec, 2);
	//float mag = sqrt(vec.x + vec.y + vec.z);
	float mag = XmFloat3Length(vec);

	//Divide by magnitude to get normalized vector (smol)
	vec = XmFloat3Divide(vec, mag);

	return vec;
}

DirectX::XMFLOAT3& XmFloat3Max(const DirectX::XMFLOAT3& in1, const DirectX::XMFLOAT3& in2)
{
	DirectX::XMFLOAT3 vec;

	//Get the greatest values from x, y, and z-components
	vec.x = in1.x > in2.x ? in1.x : in2.x;
	vec.y = in1.y > in2.y ? in1.y : in2.y;
	vec.z = in1.z > in2.z ? in1.z : in2.z;

	return vec;
}

float XmFloat3MaxValue(const DirectX::XMFLOAT3& in)
{
	float result = in.x;

	if (result < in.y)
		result = in.y;

	if (result < in.z)
		result = in.z;

	return result;
}

float XmFloat3Dot(const DirectX::XMFLOAT3& in1, const DirectX::XMFLOAT3& in2)
{
	return in1.x * in2.x + in1.y * in2.y + in1.z * in2.z;
}

bool XmFloat3Compare(const DirectX::XMFLOAT3& lhs, const DirectX::XMFLOAT3& rhs)
{
	return (lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z) ? true : false;
}
