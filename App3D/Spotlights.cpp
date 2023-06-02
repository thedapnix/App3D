#include "Spotlights.h"

SpotLights::SpotLights(ID3D11Device* device, const std::vector<LightData>& lights)
{
	for (int i = 0; i < lights.size(); i++)
	{
		//Create virtual camera just like in cubemap
		float fovY = lights.at(i).fovY;
		float aspect = 1.0f;
		float zn = 0.1f;
		float zf = 100.0f;

		//Local rotations to align the cameras to look in the proper direction
		float upRotation = lights.at(i).rotY;
		float rightRotation = lights.at(i).rotX;

		//Create virtual camera with the data we have
		m_cameras.push_back(Camera());
		m_cameras.at(i).SetPosition(lights.at(i).pos);
		m_cameras.at(i).RotateY(rightRotation);
		m_cameras.at(i).Pitch(upRotation);
		m_cameras.at(i).SetLens(fovY, aspect, zn, zf);
		m_cameras.at(i).UpdateViewMatrix();
		m_cameras.at(i).InitConstantBufferAndFrustum(device);

		//Setup buffer passed to shader (structured buffer, not constant buffer)
		LightBuffer buf = {};
		buf.col = lights.at(i).col;
		buf.origin = m_cameras.at(i).GetPosition();
		buf.direction = m_cameras.at(i).GetLook();
		DirectX::XMStoreFloat4x4(&buf.view, m_cameras.at(i).View()); //Alternatively I make other getters for view- and projection-matrices in camera class
		DirectX::XMStoreFloat4x4(&buf.proj, m_cameras.at(i).Proj());
		buf.rotation.x = lights.at(i).rotX;
		buf.rotation.y = lights.at(i).rotY;
		buf.angle = fovY;
		m_lightBuffers.push_back(buf);
	}
	

	//Create buffers in here (previously things went the other way around)
	InitStructuredBuffer(device, true, false, sizeof(LightBuffer), m_lightBuffers.size(), m_lightBuffers.data());
	InitDepthBuffer(device, 512, lights.size());
}

const ConstantBuffer& SpotLights::GetCameraConstantBufferAt(UINT index) const
{
	return m_cameras.at(index).GetConstantBuffer();
}

const UINT& SpotLights::GetLightCount() const
{
	return m_lightBuffers.size(); //Little hack where we can assume that the amount of buffers will equal the amount of lights (cameras would work too)
}

ID3D11ShaderResourceView* SpotLights::GetStructuredBufferSRV() const
{
	return structuredSRV.Get();
}

ID3D11ShaderResourceView* SpotLights::GetDepthBufferSRV() const
{
	return depthSRV.Get();
}

ID3D11DepthStencilView* SpotLights::GetDepthStencilViewAt(UINT index) const
{
	return DSVs.at(index).Get();
}

void SpotLights::InitStructuredBuffer(ID3D11Device* device, bool isDynamic, bool hasUAV, UINT elementSize, UINT elementCount, void* bufferData)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ByteWidth = elementSize * elementCount;
	desc.Usage = isDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.BindFlags |= hasUAV ? D3D11_BIND_UNORDERED_ACCESS : 0u;
	desc.CPUAccessFlags = isDynamic ? D3D11_CPU_ACCESS_WRITE : 0u;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = elementSize;

	HRESULT hr;
	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = bufferData;
	data.SysMemPitch = data.SysMemSlicePitch = 0;
	hr = device->CreateBuffer(&desc, &data, structuredBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create structured buffer for spotlight!", L"Error", MB_OK);
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = elementCount;

	hr = device->CreateShaderResourceView(structuredBuffer.Get(), &srvDesc, &structuredSRV);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create shader resource view for spotlight structured buffer!", L"Error", MB_OK);
		return;
	}
}

void SpotLights::InitDepthBuffer(ID3D11Device* device, UINT resolution, UINT arraySize)
{
	//Cookbook says that only the depth stencil view is necessary, so I'm skipping the depth stencil state
	HRESULT hr;

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = resolution;
	desc.Height = resolution;
	desc.MipLevels = 1;
	desc.ArraySize = arraySize; //"ArraySize member of the description will be the number of textures in the array" uhh
	desc.Format = DXGI_FORMAT_R32_TYPELESS; //DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	hr = device->CreateTexture2D(&desc, NULL, &DST);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create depth stencil texture for spotlightlight depth buffer!", L"Error", MB_OK);
		return;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
	dsvd.Format = DXGI_FORMAT_D32_FLOAT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	dsvd.Texture2DArray.MipSlice = 0;
	dsvd.Flags = 0;
	dsvd.Texture2DArray.ArraySize = 1;
	for (UINT i = 0; i < arraySize; i++)
	{
		dsvd.Texture2DArray.FirstArraySlice = i;
		ID3D11DepthStencilView* dsv = NULL; //ComPtr?
		hr = device->CreateDepthStencilView(DST.Get(), &dsvd, &dsv);
		if (FAILED(hr))
		{
			MessageBox(NULL, L"Failed to create depth stencil view for spotlight depth buffer!", L"Error", MB_OK);
			return;
		}
		DSVs.push_back(dsv);
	}

	//Do I need a depth buffer srv too? Edit: Yes. Note the descriptor differences, since this works more like a cubemap rtv
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.ArraySize = arraySize;
	srvDesc.Texture2DArray.FirstArraySlice = 0;

	hr = device->CreateShaderResourceView(DST.Get(), &srvDesc, &depthSRV);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create shader resource view for spotlight depth buffer!", L"Error", MB_OK);
		return;
	}
}
