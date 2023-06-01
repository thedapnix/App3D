#pragma once

#include "Camera.h"
#include <wrl.h>
#include <memory>

struct LightData
{
	DirectX::XMFLOAT3 col{ 1.0f, 1.0f, 1.0f };

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
	SpotLights(ID3D11Device* device, const std::vector<LightData>& lights);
	~SpotLights() = default;

	const ConstantBuffer& GetCameraConstantBufferAt(UINT index) const;

private:
	std::vector<LightData> m_lights;
	std::vector<LightBuffer> m_lightBuffers;
	std::vector<Camera> m_cameras; //Previously, one camera. Now a camera for each light

	//Moved from shadowmap class
	void InitStructuredBuffer(ID3D11Device* device, bool isDynamic, bool hasUAV, UINT elementSize, UINT elementCount, void* bufferData);
	Microsoft::WRL::ComPtr<ID3D11Buffer> structuredBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;

	void InitDepthBuffer(ID3D11Device* device, UINT resolution, UINT arraySize);
	Microsoft::WRL::ComPtr<ID3D11Texture2D> DST;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DSV;
	D3D11_VIEWPORT viewport = {};
};