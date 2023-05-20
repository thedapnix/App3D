#pragma once

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <d3d11.h>
#include <iostream>
#include <wrl.h>

class ParticleSystem
{
public:
	ParticleSystem() = default;
	ParticleSystem(ID3D11Device* device, bool isDynamic, bool hasSRV, bool hasUAV);
	~ParticleSystem() = default;
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> structuredBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> sbSRV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> sbUAV;
};