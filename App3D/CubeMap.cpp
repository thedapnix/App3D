#include "CubeMap.h"

CubeMap::CubeMapView::CubeMapView(ID3D11Device* device, UINT width, UINT height, bool hasSRV)
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

void CubeMap::Init(ID3D11Device* device, bool hasSRV)
{
	/*Create 6 cameras*/
	//Projection info
	float fovY = DirectX::XM_PIDIV2; //90 degree angle
	float aspect = 1.0f;
	float zn = 0.1f;
	float zf = 100.0f;

	//Local rotations to align the cameras to look in the proper direction?
	float upRotations[6] = { DirectX::XM_PIDIV2, -DirectX::XM_PIDIV2, 0.0f, 0.0f, 0.0f, DirectX::XM_PI };
	float rightRotations[6] = { 0.0f, 0.0f, -DirectX::XM_PIDIV2, DirectX::XM_PIDIV2, 0.0f, 0.0f };

	for (int i = 0; i < 6; i++)
	{
		//First, set position
		m_cameras[i].SetPosition({ 0.0f, 0.0f, 1.0f }); //Pass this in? Set some arbitrary value for now

		//Then rotate along the axes, follow ZYX convention, (changes the look-, up-, and right-vectors)
		m_cameras[i].RotateY(upRotations[i]);
		m_cameras[i].Pitch(rightRotations[i]);

		//Set projection and view matrices
		m_cameras[i].SetLens(fovY, aspect, zn, zf);
		m_cameras[i].UpdateViewMatrix(); //builds view matrix from position, as well as look-, up- and right-vectors
	}

	/*Create depth buffer and set viewport dimensions, both of these are supposed to match the size of a texture cube side. How do I get that? No clue. Set some arbitrary value for now*/
	UINT cubeWidth = 256;
	UINT cubeHeight = 256;
	InitDepthBuffer(device, cubeWidth, cubeHeight);

	viewport.Width = cubeWidth;
	viewport.Height = cubeHeight;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	//Create the thing that stores what the cube side will be reflecting
	m_cubeMapView = CubeMapView(device, cubeWidth, cubeHeight, hasSRV);
}

ID3D11RenderTargetView* CubeMap::GetRenderTargetViewAt(int index)
{
	return m_cubeMapView.GetRenderTargetViewAt(index);
}

ID3D11DepthStencilView* CubeMap::GetDepthStencilView()
{
	return dsv.Get();
}

const D3D11_VIEWPORT& CubeMap::GetViewport() const
{
	return viewport;
}

Camera* CubeMap::GetCameraAt(int index)
{
	return &m_cameras[index];
}

void CubeMap::InitDepthBuffer(ID3D11Device* device, UINT width, UINT height)
{
	//Cookbook says that only the depth stencil view is necessary, so I'm skipping the depth stencil state
	HRESULT hr;

	D3D11_TEXTURE2D_DESC dstd = {};
	dstd.Width = width;
	dstd.Height = height;
	dstd.MipLevels = 1;
	dstd.ArraySize = 1;
	dstd.Format = DXGI_FORMAT_D32_FLOAT; //DXGI_FORMAT_D24_UNORM_S8_UINT;
	dstd.SampleDesc.Count = 1;
	dstd.SampleDesc.Quality = 0;
	dstd.Usage = D3D11_USAGE_DEFAULT;
	dstd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	hr = device->CreateTexture2D(&dstd, NULL, &dst);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create depth stencil texture for cubemap!", L"Error", MB_OK);
		return;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
	dsvd.Format = DXGI_FORMAT_D32_FLOAT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Texture2D.MipSlice = 0;
	hr = device->CreateDepthStencilView(dst.Get(), &dsvd, &dsv);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create depth stencil view for cubemap!", L"Error", MB_OK);
		return;
	}
}