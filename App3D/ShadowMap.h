#pragma once

#include "Drawable.h"
#include "Spotlights.h"

class ShadowMap
{
public:
	ShadowMap() = default;
	ShadowMap(ID3D11Device* device, std::vector<Drawable>* drawables, SpotLights* spotlights);
	~ShadowMap() = default;

	ID3D11VertexShader* GetVertexShader();
	ID3D11VertexShader* GetVertexShaderPov(); //new
	const D3D11_VIEWPORT* GetViewport() const;
	ID3D11SamplerState* GetSampler();
private:
	std::vector<Drawable>* m_drawables = NULL;
	SpotLights* m_spotlights = NULL;

	D3D11_VIEWPORT viewport = {};

	void InitShaderAndInputLayout(ID3D11Device* device);
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShaderPov;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	void InitSampler(ID3D11Device* device);
	Microsoft::WRL::ComPtr<ID3D11SamplerState> shadowSampler; //used to sample the depth
};