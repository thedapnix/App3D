#pragma once

#include "Camera.h"
#include <wrl.h>
#include <memory>

struct LightData
{
	DirectX::XMFLOAT3 col{ 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT3 pos{ 0.0f, 0.0f, 0.0f };
	float rotX = 0.0f;
	float rotY = 0.0f;
	float fovY = 0.0f;
	float rad = 0.0f; //new
};

__declspec(align(16)) struct LightBuffer
{
	//Passing data to shaders
	DirectX::XMFLOAT4X4 view; 
	DirectX::XMFLOAT4X4 proj;

	DirectX::XMFLOAT3 col;
	DirectX::XMFLOAT3 origin;
	DirectX::XMFLOAT3 direction;

	DirectX::XMFLOAT2 rotation;

	float fov; //Check against this in shaders. if fov is greater than 0, it's a spotlight, otherwise it's a directional light
	float rad; //If fov is 0, also check to see if radius is greater than 0. In that case, it's a point light
};

class SpotLights
{
public:
	SpotLights() = default;
	SpotLights(ID3D11Device* device, const std::vector<LightData>& lights);
	~SpotLights() = default;

	Camera* GetCameraAt(UINT index);
	const ConstantBuffer& GetCameraConstantBufferAt(UINT index) const;
	const UINT& GetLightCount() const;
	ID3D11ShaderResourceView* GetStructuredBufferSRV() const;
	ID3D11ShaderResourceView* GetDepthBufferSRV() const;
	ID3D11DepthStencilView* GetDepthStencilViewAt(UINT index) const;

private:
	std::vector<LightBuffer> m_lightBuffers;
	std::vector<Camera> m_cameras; //Previously, one camera. Now a camera for each light

	//Moved from shadowmap class
	void InitStructuredBuffer(ID3D11Device* device, bool isDynamic, bool hasUAV, UINT elementSize, UINT elementCount, void* bufferData);
	Microsoft::WRL::ComPtr<ID3D11Buffer> structuredBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> structuredSRV;

	void InitDepthBuffer(ID3D11Device* device, UINT resolution, UINT arraySize);
	Microsoft::WRL::ComPtr<ID3D11Texture2D> DST;
	std::vector<Microsoft::WRL::ComPtr<ID3D11DepthStencilView>> DSVs;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> depthSRV;
};