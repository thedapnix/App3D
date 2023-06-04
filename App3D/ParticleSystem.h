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

	void UpdateConstantBuffer(ID3D11DeviceContext* context, float angle);

	ID3D11VertexShader* GetVertexShader();
	ID3D11GeometryShader* GetGeometryShader();
	ID3D11PixelShader* GetPixelShader();
	const UINT& GetElementCount() const;
	ID3D11ComputeShader* GetComputeShader();
	const ConstantBuffer& GetConstantBuffer() const;
	ID3D11UnorderedAccessView* const* GetUAVAddress();
	ID3D11ShaderResourceView* GetSRV();

private:
	//Structured buffer
	void InitStructuredBuffer(ID3D11Device* device, bool isDynamic, bool hasSRV, bool hasUAV, UINT elementSize, UINT elementCount, void* bufferData);
	static constexpr UINT m_elementCount = 8;
	Microsoft::WRL::ComPtr<ID3D11Buffer> structuredBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> uav;

	//Shaders
	void InitShaders(ID3D11Device* device);
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> computeShader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> geometryShader;

	ConstantBuffer m_constantBuffer;
};