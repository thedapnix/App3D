#include "ShaderResource.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void ShaderResource::Init(ID3D11Device* device, const char* textureFile)
{
	/*Load texture from file, then set*/
	int width;
	int height;
	int channels;

	std::string path = "Models/" + std::string(textureFile);

	uint8_t* imageData = stbi_load(path.c_str(), &width, &height, &channels, 4);

	if (!imageData) {
		std::string message = "Failed to load the image: " + path;
		MessageBox(NULL, std::wstring(message.begin(), message.end()).c_str(), L"Error", MB_OK);
		return;
	}

	SetTexture(device, width, height, imageData);
}

void ShaderResource::SetTexture(ID3D11Device* device, UINT width, UINT height, void* textureData)
{
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = textureData;
	data.SysMemPitch = desc.Width * 4;
	data.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateTexture2D(&desc, &data, &texture);

	if (FAILED(hr)) {
		MessageBox(NULL, L"Failed to create shader resource texture!", L"Error", MB_OK);
		return;
	}
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Format = desc.Format;

	hr = device->CreateShaderResourceView(texture.Get(), &srvDesc, &srv);

	if (FAILED(hr)) {
		MessageBox(NULL, L"Failed to create shader resource view!", L"Error", MB_OK);
		return;
	}
	stbi_image_free(textureData);
}

ID3D11ShaderResourceView* ShaderResource::GetSRV() const
{
	return srv.Get();
}