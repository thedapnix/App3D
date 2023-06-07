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
	srtData.SysMemPitch = width * channels;
	//srtData.SysMemSlicePitch = 0;

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

	/*Setup sampler, allowing us to access texture data in shader*/
	//D3D11_SAMPLER_DESC samplerDesc;
	//ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	//samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;		//Anisotropic filtering (crazy that D3D does this for us, thank you <3)
	//samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;	//Method to use for resolving texture coordinates outside the 0 to 1 range (WRAP is basically repeat)
	//samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	//samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	//samplerDesc.MipLODBias = 0;							//Offset from the calculated mipmap level
	//samplerDesc.MaxAnisotropy = 16;						//Clamping for the anisotropic filtering (Valid values are between 1 and 16)
	//samplerDesc.MinLOD = 0;
	//samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//hr = device->CreateSamplerState(&samplerDesc, sampler.GetAddressOf());

	//if (FAILED(hr)) {
	//	MessageBox(NULL, L"Failed to create sampler for drawable!", L"Error", MB_OK);
	//	return;
	//}
}

ID3D11ShaderResourceView* ShaderResource::GetSRV() const
{
	return srv.Get();
}