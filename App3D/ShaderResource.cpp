#include "ShaderResource.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void ShaderResource::Init(ID3D11Device* device, const char* textureFileName)
{
	/*Read from texture file*/
	int width, height;
	int channels;

	unsigned char* img = stbi_load(textureFileName, &width, &height, &channels, 4);

	if (!img)
	{
		MessageBox(NULL, L"Failed to load texture file!", L"Error", MB_OK);
		return;
	}

	/*Create descriptor and subresource data using info from the file you read*/
	D3D11_TEXTURE2D_DESC srtDesc;
	ZeroMemory(&srtDesc, sizeof(srtDesc));

	srtDesc.Width = width;
	srtDesc.Height = height;
	srtDesc.MipLevels = 1;
	srtDesc.ArraySize = 1;
	srtDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srtDesc.SampleDesc.Count = 1;
	//srtDesc.SampleDesc.Quality = 0;
	srtDesc.Usage = D3D11_USAGE_IMMUTABLE; //GPU read-only
	srtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	srtDesc.CPUAccessFlags = 0;
	srtDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA srtData;
	ZeroMemory(&srtData, sizeof(srtData));

	srtData.pSysMem = img;
	srtData.SysMemPitch = width * 4; //Setting this to be width * channels ruins any image that don't have 32 bits (such as jpegs with no alpha channel hence 24 bits)
	srtData.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateTexture2D(&srtDesc, &srtData, srt.GetAddressOf());

	if (FAILED(hr)) {
		MessageBox(NULL, L"Failed to create shader resource texture for drawable!", L"Error", MB_OK);
		return;
	}

	/*D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));

	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = srtDesc.MipLevels;
	srvDesc.Format = srtDesc.Format;*/

	hr = device->CreateShaderResourceView(srt.Get(), NULL, srv.GetAddressOf());

	if (FAILED(hr)) {
		MessageBox(NULL, L"Failed to create shader resource view for drawable!", L"Error", MB_OK);
		return;
	}

	stbi_image_free(img);
}

ID3D11ShaderResourceView* ShaderResource::GetSRV() const
{
	return srv.Get();
}