#pragma once

#include "Camera.h"
#include <wrl.h>
#include <memory>

struct LightData
{
	//Think camera view
	DirectX::XMFLOAT3 pos{ 0.0f, 0.0f, 0.0f };
	float rotX = 0.0f;
	float rotY = 0.0f;

	//Think camera proj
	float fovY = 0.0f;
	//float aspect = 0.0f;
	//float zn = 0.1f;
	//float zf = 100.0f;

	DirectX::XMFLOAT3 col{ 1.0f, 1.0f, 1.0f };	//Default white light color
};

class SpotLight
{
public:
	SpotLight() = default;
	SpotLight(ID3D11Device* device, const LightData& lightData);
	~SpotLight() = default;

private:
	Camera m_camera;
};