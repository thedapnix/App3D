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

	void Init(ID3D11Device* device, const char* textureFileName);
	void InitNormalMap(ID3D11Device* device, const char* textureFileName);

	ID3D11ShaderResourceView* GetSRV() const;

private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> srt;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
};