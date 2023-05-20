#pragma once

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include "Drawable.h"

__declspec(align(16)) struct Particle
{
	DirectX::XMFLOAT3 pos;
};

__declspec(align(16)) struct ParticleCB
{
	float angle; //let's do some sort of rotation in the shader
};

class ParticleSystem
{
public:
	ParticleSystem() = default;
	ParticleSystem(ID3D11Device* device);
	~ParticleSystem() = default;

	void Draw(ID3D11DeviceContext* context, UINT width, UINT height, ID3D11Buffer* cameraCB);

private:
	//Structured buffer
	void InitStructuredBuffer(ID3D11Device* device, bool isDynamic, bool hasSRV, bool hasUAV, UINT elementSize, UINT elementCount, void* bufferData);
	static constexpr UINT m_elementCount = 3;
	Microsoft::WRL::ComPtr<ID3D11Buffer> structuredBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> uav;

	//Shaders
	void InitShaders(ID3D11Device* device);
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> computeShader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> geometryShader;

	//Input layout
	void InitInputLayout(ID3D11Device* device, ID3DBlob* vertexData); //called by the InitShaders() function, passes the vBlob
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	ConstantBuffer m_constantBuffer;
};