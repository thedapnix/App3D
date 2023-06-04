#include "ParticleSystem.h"
#include <d3dcompiler.h>

ParticleSystem::ParticleSystem(ID3D11Device* device)
{
	//Create some amount of particles (manipulate this value through imgui slider later)
	Particle particles[m_elementCount]
	{
		{{-8.0f, -11.0f,  -28.0f}},
		{{  -9.0f,   -10.0f,  -27.0f}},
		{{ -10.0f,  -9.0f, -26.0f}},
		{{-11.0f, -8.0f,  -27.0f}},
		{{-8.0f, -8.0f,  -28.0f}},
		{{  -9.0f,   -9.0f,  -27.0f}},
		{{ -10.0f,  -10.0f, -26.0f}},
		{{-11.0f, -11.0f,  -27.0f}},
	};
	InitStructuredBuffer(device, false, true, true, sizeof(Particle), m_elementCount, particles);
	
	//Since this is basically its own system of drawables, the particle system also needs its own shaders and a constant buffer
	InitShaders(device);

	ParticleCB pcb = {};
	m_constantBuffer.Init(device, &pcb, sizeof(pcb));
}

void ParticleSystem::UpdateConstantBuffer(ID3D11DeviceContext* context, float angle)
{
	ParticleCB pcb = {};
	pcb.angle = angle;

	m_constantBuffer.Update(context, &pcb);
}

ID3D11VertexShader* ParticleSystem::GetVertexShader()
{
	return vertexShader.Get();
}

ID3D11GeometryShader* ParticleSystem::GetGeometryShader()
{
	return geometryShader.Get();
}

ID3D11PixelShader* ParticleSystem::GetPixelShader()
{
	return pixelShader.Get();
}

const UINT& ParticleSystem::GetElementCount() const
{
	return m_elementCount;
}

ID3D11ComputeShader* ParticleSystem::GetComputeShader()
{
	return computeShader.Get();
}

const ConstantBuffer& ParticleSystem::GetConstantBuffer() const
{
	return m_constantBuffer;
}

ID3D11UnorderedAccessView* const* ParticleSystem::GetUAVAddress()
{
	return uav.GetAddressOf();
}

ID3D11ShaderResourceView* ParticleSystem::GetSRV()
{
	return srv.Get();
}

void ParticleSystem::InitStructuredBuffer(ID3D11Device* device, bool isDynamic, bool hasSRV, bool hasUAV, UINT elementSize, UINT elementCount, void* bufferData)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ByteWidth = elementSize * elementCount;
	desc.Usage = isDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT; //IMMUTABLE? Refused to build if I had all 3 of these as true in the function call, disable dynamic and do default and we're fine
	desc.BindFlags = hasSRV ? D3D11_BIND_SHADER_RESOURCE : 0u;
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
		MessageBox(NULL, L"Failed to create structured buffer!", L"Error", MB_OK);
	}

	//Bind srv and uav to the structured buffer
	if (hasSRV)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = elementCount;
		hr = device->CreateShaderResourceView(structuredBuffer.Get(), &srvDesc, srv.GetAddressOf());
		if (FAILED(hr))
		{
			MessageBox(NULL, L"Failed to create shader resource view for structured buffer!", L"Error", MB_OK);
		}
	}

	if (hasUAV)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		ZeroMemory(&uavDesc, sizeof(uavDesc));
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = elementCount;
		uavDesc.Buffer.Flags = 0;
		hr = device->CreateUnorderedAccessView(structuredBuffer.Get(), &uavDesc, uav.GetAddressOf());
		if (FAILED(hr))
		{
			MessageBox(NULL, L"Failed to create unordered access view for structured buffer!", L"Error", MB_OK);
		}
	}
}

void ParticleSystem::InitShaders(ID3D11Device* device)
{
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3DBlob>
		vsBlob, psBlob,
		csBlob, gsBlob,
		errorBlob;

	/****************************
	//////READ SHADER FILES//////
	****************************/
	hr = D3DReadFileToBlob(L"../x64/Debug/ParticleVertexShader.cso", &vsBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to read vertex shader for particle system!", L"Error", MB_OK);
	}
	hr = D3DReadFileToBlob(L"../x64/Debug/ParticlePixelShader.cso", &psBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to read pixel shader for particle system!", L"Error", MB_OK);
	}

	hr = D3DCompileFromFile(L"ParticleComputeShader.hlsl", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "cs_5_0", D3DCOMPILE_DEBUG, 0, &csBlob, &errorBlob);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to read compute shader for particle system!", L"Error", MB_OK);
		return;
	}

	hr = D3DReadFileToBlob(L"../x64/Debug/ParticleGeometryShader.cso", &gsBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to read geometry shader for particle system!", L"Error", MB_OK);
	}

	/****************************
	//CREATE SHADERS FROM FILES//
	****************************/
	hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, &vertexShader);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create vertex shader for particle system!", L"Error", MB_OK);
		return;
	}
	hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), NULL, &pixelShader);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create pixel shader for particle system!", L"Error", MB_OK);
		return;
	}
	hr = device->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), NULL, &computeShader);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create compute shader for particle system!", L"Error", MB_OK);
		return;
	}
	hr = device->CreateGeometryShader(gsBlob->GetBufferPointer(), gsBlob->GetBufferSize(), NULL, &geometryShader);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed to create geometry shader for particle system!", L"Error", MB_OK);
		return;
	}
}
