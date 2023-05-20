#include "ParticleSystem.h"
#include <d3dcompiler.h>

ParticleSystem::ParticleSystem(ID3D11Device* device)
{
	//Create some amount of particles (manipulate this value through imgui slider later)
	Particle particles[m_elementCount]
	{
		{{-10.0f, -10.0f,  0.0f}},
		{{  5.0f,   5.0f,  5.0f}},
		{{ 25.0f,  25.0f, 10.0f}}
	};
	InitStructuredBuffer(device, false, false, true, sizeof(Particle), m_elementCount, particles);
	
	//Since this is basically its own system of drawables, the particle system also needs its own shaders and a constant buffer
	InitShaders(device);

	ParticleCB pcb;
	m_constantBuffer.Init(device, &pcb, sizeof(pcb));
}

void ParticleSystem::Draw(ID3D11DeviceContext* context, UINT width, UINT height, ID3D11Buffer* cameraCB, D3D11_VIEWPORT viewport)
{
	//Maybe I need the particle system to have a render target view?
	
	context->RSSetViewports(1, &viewport);

	/*Input Assembler Stage*/
	context->IASetInputLayout(NULL); //Done in cookbook, feels weird
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	/*Shader Stage*/
	context->VSSetShader(vertexShader.Get(), NULL, 0);
	context->GSSetShader(geometryShader.Get(), NULL, 0);
	context->GSSetConstantBuffers(0, 1, &cameraCB);
	context->PSSetShader(pixelShader.Get(), NULL, 0);

	//Draw
	context->Draw(m_elementCount, 0);

	//Unbind shaders and constant buffer
	context->VSSetShader(NULL, NULL, 0);
	context->GSSetShader(NULL, NULL, 0);
	context->GSSetConstantBuffers(0, 0, NULL);
	context->PSSetShader(NULL, NULL, 0);

	//Use compute shader and uav to edit stuff
	context->CSSetShader(computeShader.Get(), NULL, 0);
	context->CSSetConstantBuffers(0, 1, m_constantBuffer.GetBufferAddress());
	context->CSSetUnorderedAccessViews(0, 1, uav.GetAddressOf(), NULL);
	context->Dispatch(width / 32, height, 1); //Match the compute shader numthreads, [32, 1, 1]

	//Unbind uav
	ID3D11UnorderedAccessView* nullUAV = NULL;
	context->CSSetUnorderedAccessViews(0, 1, &nullUAV, NULL);

	//Unbind shader and constant buffer
	context->CSSetShader(NULL, NULL, 0);
	context->CSSetConstantBuffers(0, 0, NULL);
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
