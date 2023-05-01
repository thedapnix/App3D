#pragma once

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <iostream>
#include <d3d11.h>
#include <wrl.h>

class ShaderResource {
public:
	ShaderResource() = default;
	~ShaderResource() = default;
	ShaderResource& operator=(const ShaderResource&) = delete;
	ShaderResource(const ShaderResource&) = delete;

	void Init(ID3D11Device* device, const char* textureFile);
	void SetTexture(ID3D11Device* device, UINT width, UINT height, void* textureData);

	ID3D11ShaderResourceView* GetSRV() const;

private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
};