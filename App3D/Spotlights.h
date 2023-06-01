#pragma once

#include "Camera.h"
#include <wrl.h>
#include <memory>

struct LightData
{
	DirectX::XMFLOAT3 col{ 1.0f, 1.0f, 1.0f };	//Default white light color

	//Think camera view
	DirectX::XMFLOAT3 pos{ 0.0f, 0.0f, 0.0f };
	float rotX = 0.0f;
	float rotY = 0.0f;

	//Think camera proj
	float fovY = 0.0f;
	//float aspect = 0.0f;
	//float zn = 0.1f;
	//float zf = 100.0f;
};

__declspec(align(16)) struct LightBuffer
{
	//Passing data to shaders
	DirectX::XMFLOAT4X4 view; 
	DirectX::XMFLOAT4X4 proj;
	DirectX::XMFLOAT3 col;
	DirectX::XMFLOAT3 origin;
	DirectX::XMFLOAT3 direction;
};

class SpotLights
{
public:
	SpotLights() = default;
	SpotLights(ID3D11Device* device, const LightData& lightData);
	~SpotLights() = default;

	const ConstantBuffer& GetCameraConstantBuffer() const;

private:
	Camera m_camera;
};