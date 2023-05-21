#include "CubeMapView.h"

CubeMapView::CubeMapView(ID3D11Device* device, UINT width, UINT height, bool hasSRV)
{
	//Create texture cube
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 6; //Texture cube is 6 textures combined in a cubic pattern, where we let indices 0-1 be positive and negative X, 2-3 be Y, 4-5 be Z
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; //damn we pass this in too, cringe, default as 4-dimensional 32-bit floats?
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
	texDesc.BindFlags |= hasSRV ? D3D11_BIND_SHADER_RESOURCE : 0; //Prepare to bind an srv if we want one
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	HRESULT hr = device->CreateTexture2D(&texDesc, NULL, texture.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create texture cube!", L"Error", MB_OK);
	}

	//Create render target views for every side of the cube
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(rtvDesc));
	rtvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // grr, same as above
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.ArraySize = 1;
	rtvDesc.Texture2DArray.MipSlice = 0;

	for (int i = 0; i < 6; i++)
	{
		rtvDesc.Texture2DArray.FirstArraySlice = i;
		ID3D11RenderTargetView* rtv = NULL;
		hr = device->CreateRenderTargetView(texture.Get(), &rtvDesc, &rtv);
		if (FAILED(hr))
		{
			MessageBox(NULL, L"Failed to create rtv for cubemap!", L"Error", MB_OK);
		}
		rtvs.push_back(rtv);
	}

	//Create shader resource view if we choose to have one
	if (hasSRV)
	{
		hr = device->CreateShaderResourceView(texture.Get(), NULL, srv.GetAddressOf());
		if (FAILED(hr))
		{
			MessageBox(NULL, L"Failed to create srv for cubemap!", L"Error", MB_OK);
		}
	}
}
