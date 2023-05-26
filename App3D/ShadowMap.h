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

	void InitShaderAndInputLayout(ID3D11Device* device);
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	D3D11_VIEWPORT viewport;
};