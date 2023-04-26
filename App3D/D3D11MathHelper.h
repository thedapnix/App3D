#pragma once

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <DirectXMath.h>

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

//DirectX::XMMATRIX GetViewMatrix();
//DirectX::XMMATRIX GetProjectionMatrix();
//DirectX::XMMATRIX GetViewProj();