#pragma once

#include <d3d11.h>
#include <wrl.h>
#include "ConstantBuffer.h"


class SubMesh
{
public:
	SubMesh() = default;
	SubMesh(ID3D11Device* device, UINT startIndex, UINT indexCount,
		ID3D11ShaderResourceView* ambient, ID3D11ShaderResourceView* diffuse, ID3D11ShaderResourceView* specular, float shine);
	~SubMesh() = default;

	void AddNormalMap(ID3D11ShaderResourceView* nm);

	void Bind(ID3D11DeviceContext* context, bool isReflective = false, bool hasNormalMap = false) const;
	void Draw(ID3D11DeviceContext* context) const;
	void Unbind(ID3D11DeviceContext* context, bool isReflective = false, bool hasNormalMap = false) const;

private:
	__declspec(align(16)) struct ShininessCB
	{
		float shininess;
	};
	ConstantBuffer m_constantBufferShininess;

	UINT m_startIndex = 0;
	UINT m_indexCount = 0;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ambientSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuseSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> specularSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalmapSRV;
};