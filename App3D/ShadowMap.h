#pragma once

#include "Drawable.h"
#include "Spotlight.h"

class ShadowMap
{
public:
	ShadowMap() = default;
	ShadowMap(ID3D11Device* device, std::vector<Drawable>* drawables, std::vector<SpotLight>* spotlights);
	~ShadowMap() = default;

	ID3D11VertexShader* GetVertexShader();
	const D3D11_VIEWPORT* GetViewport() const;
	ID3D11DepthStencilView* GetDepthStencilViewAt(UINT index);
private:
	std::vector<Drawable>* m_drawables = NULL;
	std::vector<SpotLight>* m_spotlights = NULL;

	void InitStructuredBuffer(ID3D11Device* device, bool isDynamic, bool hasUAV, UINT elementSize, UINT elementCount, void* bufferData);
	Microsoft::WRL::ComPtr<ID3D11Buffer> structuredBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;

	void InitDepthBuffer(ID3D11Device* device, UINT resolution, UINT arraySize);
	Microsoft::WRL::ComPtr<ID3D11Texture2D> DST;
	std::vector<Microsoft::WRL::ComPtr<ID3D11DepthStencilView>> DSVs;
	D3D11_VIEWPORT viewport = {};

	void InitShaderAndInputLayout(ID3D11Device* device);
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
};