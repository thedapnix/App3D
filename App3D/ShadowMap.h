#pragma once

#include "Drawable.h"

class ShadowMap
{
public:
	ShadowMap() = default;
	ShadowMap(ID3D11Device* device, std::vector<Drawable>* drawables);
	~ShadowMap() = default;
private:
	std::vector<Drawable>* m_drawables = NULL;
	//make some class for placing directional spotlights (origin, direction, fov, color?) and let this class have a vector of pointers to them?

	void InitShaderAndInputLayout(ID3D11Device* device);
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	D3D11_VIEWPORT viewport;
};