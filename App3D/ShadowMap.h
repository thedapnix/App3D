#pragma once

#include "Drawable.h"
#include "Spotlight.h"

class ShadowMap
{
public:
	ShadowMap() = default;
	ShadowMap(ID3D11Device* device, std::vector<Drawable>* drawables, std::vector<SpotLight>* spotlights);
	~ShadowMap() = default;
private:
	std::vector<Drawable>* m_drawables = NULL;
	std::vector<SpotLight>* m_spotlights = NULL;

	void InitDepthBuffer(ID3D11Device* device, UINT resolution, UINT arraySize);
	Microsoft::WRL::ComPtr<ID3D11Texture2D> DST;
	std::vector<Microsoft::WRL::ComPtr<ID3D11DepthStencilView>> DSVs;
	D3D11_VIEWPORT viewport = {};

	void InitShaderResourceView(ID3D11Device* device, UINT arraySize);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;

	void InitShaderAndInputLayout(ID3D11Device* device);
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
};