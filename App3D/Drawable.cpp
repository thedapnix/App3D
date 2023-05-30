#include "Drawable.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Drawable::Drawable(ID3D11Device* device, const BufferData& data, DirectX::XMFLOAT3 scaling, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 translation)
{
	//Every drawable has a vertex buffer, an index buffer, and a constant buffer
	m_vertexVectorData = (void*)(data.vData.vector.data());
	m_vertexBuffer.Init(
		device,
		data.vData.size,
		data.vData.count,
		m_vertexVectorData
	);

	m_indexBuffer.Init(
		device,
		data.iData.size,
		data.iData.count,
		(uint32_t*)(data.iData.vector.data()));

	//Quoted stackoverflow answer to the question on which order we do world matrix multiplications: "Usually it is scale, then rotation and lastly translation."
	m_scale = scaling;
	m_rotate = rotation;
	m_translate = translation;
	DirectX::XMStoreFloat4x4(&m_transform.scale, DirectX::XMMatrixTranspose(
		DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z)
	));
	DirectX::XMStoreFloat4x4(&m_transform.rotate, DirectX::XMMatrixTranspose(
		DirectX::XMMatrixRotationX(m_rotate.x) * DirectX::XMMatrixRotationY(m_rotate.y) * DirectX::XMMatrixRotationZ(m_rotate.z)
	));
	DirectX::XMStoreFloat4x4(&m_transform.translate, DirectX::XMMatrixTranspose(
		DirectX::XMMatrixTranslation(m_translate.x, m_translate.y, m_translate.z)
	));
	m_constantBuffer.Init(device, &m_transform, sizeof(m_transform));

	//They can also have shader resource view+texture (realistically everything will be textured but hey, we have the option not to)
	InitTexture(device, data.textureFileName.c_str());
}

void Drawable::InitTexture(ID3D11Device* device, const char* textureFileName)
{
	/*Read from texture file*/
	//int width, height;
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
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;		//Anisotropic filtering (crazy that D3D does this for us, thank you <3)
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;	//Method to use for resolving texture coordinates outside the 0 to 1 range (WRAP is basically repeat)
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0;							//Offset from the calculated mipmap level
	samplerDesc.MaxAnisotropy = 16;						//Clamping for the anisotropic filtering (Valid values are between 1 and 16)
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = device->CreateSamplerState(&samplerDesc, sampler.GetAddressOf());

	if (FAILED(hr)) {
		MessageBox(NULL, L"Failed to create sampler for drawable!", L"Error", MB_OK);
		return;
	}
}

void Drawable::Bind(ID3D11DeviceContext* context, ID3D11ShaderResourceView* inputSRV)
{
	//Vertex Buffer
	ID3D11Buffer* buffer[] = { m_vertexBuffer.GetBuffer() };
	UINT stride = m_vertexBuffer.GetVertexSize();
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, buffer, &stride, &offset);

	//Index Buffer
	context->IASetIndexBuffer(m_indexBuffer.GetBuffer(), DXGI_FORMAT_R32_UINT, 0);

	//Texture
	//ID3D11ShaderResourceView* view = { srv.Get() };
	if (inputSRV != NULL)	context->PSSetShaderResources(0, 1, &inputSRV);
	else					context->PSSetShaderResources(0, 1, srv.GetAddressOf());
	context->PSSetSamplers(0, 1, sampler.GetAddressOf());
}

void Drawable::Draw(ID3D11DeviceContext* context) const
{
	context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetBufferAddress());
	context->Draw(m_vertexBuffer.GetVertexCount(), 0);
	//context->DrawIndexed(m_indexBuffer.GetIndexCount(), 0, 0);
}

void Drawable::Unbind(ID3D11DeviceContext* context)
{
	ID3D11ShaderResourceView* nullSRV = NULL;
	context->PSSetShaderResources(0, 1, &nullSRV);
}

void Drawable::UpdateConstantBuffer(ID3D11DeviceContext* context)
{
	D3D11_MAPPED_SUBRESOURCE mapped = {};												//Set up the new srtData for the resource, zero the memory
	context->Map(m_constantBuffer.GetBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);	//Disable GPU access to the srtData we want to change, and get a pointer to its memory
	memcpy(mapped.pData, &m_transform, sizeof(m_transform));							//Write the new srtData to memory
	context->Unmap(m_constantBuffer.GetBuffer(), 0);									//Re-enable GPU access to the srtData
}

void Drawable::CreateBoundingBoxFromPoints(DirectX::XMVECTOR min, DirectX::XMVECTOR max)
{
	DirectX::BoundingBox::CreateFromPoints(m_aabb, min, max);
}

void Drawable::EditTranslation(float x, float y, float z)
{
	m_translate.x += x;
	m_translate.y += y;
	m_translate.z += z;
	DirectX::XMStoreFloat4x4(&m_transform.translate, DirectX::XMMatrixTranspose(
		DirectX::XMMatrixTranslation(m_translate.x, m_translate.y, m_translate.z)
	));
}

void Drawable::RotateY(float dt)
{
	m_rotate.y += 0.025;
	DirectX::XMStoreFloat4x4(&m_transform.rotate, DirectX::XMMatrixTranspose(
		DirectX::XMMatrixRotationX(m_rotate.x) * DirectX::XMMatrixRotationY(m_rotate.y) * DirectX::XMMatrixRotationZ(m_rotate.z)
	));
}

DirectX::XMMATRIX Drawable::World()
{
	//This feels like a horribly expensive function. I might be wrong though. But if I'm not, consider storing a XMFLOAT4X4 m_world
	return DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z) * 
		DirectX::XMMatrixRotationX(m_rotate.x) * DirectX::XMMatrixRotationY(m_rotate.y) * DirectX::XMMatrixRotationZ(m_rotate.z) * 
		DirectX::XMMatrixTranslation(m_translate.x, m_translate.y, m_translate.z);
}

DirectX::BoundingBox Drawable::GetBoundingBox()
{
	return m_aabb;
}

const VertexBuffer& Drawable::GetVertexBuffer() const
{
	return m_vertexBuffer;
}

void* Drawable::GetVertexVectorData()
{
	return m_vertexVectorData;
}
