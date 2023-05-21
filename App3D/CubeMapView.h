#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <vector>

class CubeMapView //The view of every virtual camera
{
public:
	CubeMapView() = default;
	CubeMapView(ID3D11Device* device, UINT width, UINT height, bool hasSRV);
	~CubeMapView() = default;
private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> rtvs;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
};