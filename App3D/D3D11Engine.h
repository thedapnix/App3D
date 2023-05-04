#pragma once

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <fstream>
#include <sstream>

//#include <DirectXCollision.h>

#include "ConstantBuffer.h"
#include "Drawable.h"
#include "Camera.h"

/*
* Here's all my deferred rendering stuff
* 
struct GBuffer
{
	//G-buffers are essentially just textures for us to render to. We also supply them with an RTV and SRV to enable reading and writing (RTV write, SRV read) of the geometry data
	ID3D11Texture2D* texture = NULL;
	ID3D11RenderTargetView* rtv = NULL;
	ID3D11ShaderResourceView* srv = NULL;
};



void InitGraphicsBuffer(GBuffer (&gbuf)[3], UINT bufferSize, ID3D11Device*& device)
{
	HRESULT hr;

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = WIDTH;
	textureDesc.Height = HEIGHT;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	for (UINT i = 0; i < bufferSize; i++) //Create texture(s)
	{
		hr = device->CreateTexture2D(&textureDesc, NULL, &gbuf[i].texture);
		assert(SUCCEEDED(hr));
	}

	for (UINT i = 0; i < bufferSize; i++) //Create RTV(s), used to write geometric data to texture
	{
		hr = device->CreateRenderTargetView(gbuf[i].texture, NULL, &gbuf[i].rtv); //hr = device->CreateRenderTargetView(gBuffer[i].texture, &renderTargetViewDesc, &gBuffer[i].rtv);
		assert(SUCCEEDED(hr));
	}

	for (UINT i = 0; i < bufferSize; i++) //Create SRV(s), used to read the data from RTV(s)
	{
		hr = device->CreateShaderResourceView(gbuf[i].texture, NULL, &gbuf[i].srv); //hr = device->CreateShaderResourceView(gBuffer[i].texture, &shaderResourceViewDesc, &gBuffer[i].srv);
		assert(SUCCEEDED(hr));
	}
}




void DeferredRender(UINT width, UINT height, HWND window, UINT vStride, UINT vOffset, UINT iCount,
	ID3D11DeviceContext* context, ID3D11RenderTargetView* rtv, D3D11_VIEWPORT viewport, ID3D11InputLayout* inputLayout,
	ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer, ID3D11Buffer* constantBuffer1, ID3D11Buffer* constantBuffer2,
	ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11ShaderResourceView* srv, ID3D11SamplerState* samplerState,
	IDXGISwapChain* swapChain, ID3D11ComputeShader* computeShader, ID3D11UnorderedAccessView* backBufferUAV, GBuffer gbuf[], ID3D11DepthStencilView* dsv)
{
	//Deferred rendering splits rendering into 3 parts: A geometry pass, a draw pass, and a lighting pass

	//First pass (aka Geometry pass, we fill our gbuffers with data)
	float background_colour[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	context->IASetInputLayout(inputLayout);
	context->RSSetViewports(1, &viewport);

	//Create an array of render target views and fill it with the rtv's from our gbuffers
	ID3D11RenderTargetView* rtvArr[] = { gbuf[0].rtv, gbuf[1].rtv, gbuf[2].rtv };
	context->OMSetRenderTargets(3, rtvArr, dsv); //When render targets are bound to the output merger (if I understand correctly), they are sent to the pixel shader where they get filled with data yes?

	context->ClearRenderTargetView(gbuf[0].rtv, background_colour);
	context->ClearRenderTargetView(gbuf[1].rtv, background_colour);
	context->ClearRenderTargetView(gbuf[2].rtv, background_colour);
	context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, 1.0f, 0.0f);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Draw pass (we draw the scene onto the backbuffer without doing any lighting calculations)
	context->IASetVertexBuffers(
		0,
		1,
		&vertexBuffer,
		&vStride,
		&vOffset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	context->VSSetShader(vertexShader, NULL, 0);
	context->PSSetShader(pixelShader, NULL, 0);

	context->VSSetConstantBuffers(0, 1, &constantBuffer1);
	context->VSSetConstantBuffers(1, 1, &constantBuffer2);
	context->PSSetShaderResources(0, 1, &srv);
	context->PSSetSamplers(0, 1, &samplerState);

	context->DrawIndexed(iCount, 0, 0);

	//Unbind RTVs, since we're done writing data to the RTVs (pixel shader output)
	rtvArr[0] = NULL;
	rtvArr[1] = NULL;
	rtvArr[2] = NULL;

	//Last pass (aka Lighting pass, we use our Compute Shader to edit the backbuffer and do lighting computations once everything else has been done first)
	//So now we set the rendertarget to be the *actual* backbuffer, from what I understand
	//float background_colour_2[4] = { 0.9f, 0.9f, 0.9f, 1.0f };
	ID3D11RenderTargetView* nullRtv = NULL;
	context->OMSetRenderTargets(1, &nullRtv, NULL);

	ID3D11ShaderResourceView* srvArr[] = { gbuf[0].srv, gbuf[1].srv, gbuf[2].srv };
	context->CSSetShaderResources(0, 3, srvArr);

	//Use compute shader to edit the backbuffer
	context->CSSetShader(computeShader, NULL, 0);
	context->CSSetUnorderedAccessViews(0, 1, &backBufferUAV, NULL); //Last value matters in case of "append consume" buffers, but I've only heard of that, I've *no idea* what it means
	context->Dispatch(width / 8, height / 8, 1); //In order to make our Dispatch cover the entire window, we group threads by the width and height of the window divided by 8 (as 8x8 is defined in compute shader)

	//Unbind UAV
	ID3D11UnorderedAccessView* nullUav = NULL;
	context->CSSetUnorderedAccessViews(0, 1, &nullUav, NULL);

	//Unbind SRVs
	srvArr[0] = NULL;
	srvArr[1] = NULL;
	srvArr[2] = NULL;
	context->CSSetShaderResources(0, 3, srvArr);
	////////////////////

	//Finally, present
	swapChain->Present(1, 0);
}




	//header-stuff
	ID3D11ComputeShader* pComputeShader = NULL;
	ID3D11UnorderedAccessView* pBackBufferUAV = NULL; //Read-and-write permissions to the backbuffer when we pass it to the compute shader


	//Will belong in our constructor here
	//Create the diffuse buffer, position buffer, and normal buffer
	GBuffer gbuffers[3];
	InitGraphicsBuffer(gbuffers, 3, pDevice);

	//creating the uav, also belongs in the constructor
	HRESULT hr;
	ID3D11Texture2D* backBuffer = NULL;
	hr = swapChain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),
		(void**)&backBuffer);
	assert(SUCCEEDED(hr));

	hr = device->CreateUnorderedAccessView(
		backBuffer,
		NULL,
		&uav);
	assert(SUCCEEDED(hr));
	backBuffer->Release();

	return !FAILED(hr);
*/

class D3D11Engine
{
public:
	D3D11Engine(const HWND& hWnd, const UINT& width, const UINT& height);
	~D3D11Engine();
	D3D11Engine& operator=(const D3D11Engine&) = delete;
	D3D11Engine(const D3D11Engine&) = delete;

	void Update(float dt);
	void ImGuiSceneData(D3D11Engine* d3d11engine, bool shouldUpdateFps, int state);

	void MovePlayer(float speed);
	Camera& GetCamera() const noexcept;
	//ID3D11Device* GetDevice();

private:
	/*TODO: Move to camera*/
	struct ViewProj
	{
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 proj;
	};
	ViewProj m_viewProj;
	ConstantBuffer m_cameraCB;
	DirectX::BoundingFrustum m_frustum;

	//OBJ-parser
	struct VertexReference
	{
		int v, vt, vn;
	};

	/*Functions*/
	void Render(float dt);
	void InitInterfaces(const HWND& window);
	void InitRTV();
	void InitViewport(const UINT& width, const UINT& height);
	void InitDepthStencil(const UINT& width, const UINT& height);
	void InitVertexShader();
	void InitInputLayout();
	void InitPixelShader();
	void InitCamera();

	bool DrawableIsVisible(DirectX::BoundingFrustum frustum, DirectX::BoundingBox aabb, DirectX::XMMATRIX view, DirectX::XMMATRIX world);
	//void InitSampler();

	/*Temp*/
	void InitQuad(DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotate, DirectX::XMFLOAT3 translate);
	void InitCube(DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotate, DirectX::XMFLOAT3 translate);
	void UpdateConstantBuffer(ID3D11Buffer* cb, void* data, size_t size);

	bool InitDrawableFromFile(std::string fileName, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotate, DirectX::XMFLOAT3 translate);

	/*Variables*/
	static constexpr float CLEAR_COLOR[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	std::vector<Drawable> m_drawables;
	std::unique_ptr<Camera> m_camera;
	int drawablesBeingRendered = 0;

	/*ImGui variables*/
	int m_fpsCounter = 0;
	std::string m_fpsString = "";
	bool objIsEnabled = true;
	bool deferredIsEnabled = false;
	bool cullingIsEnabled = false;
	bool billboardingIsEnabled = false;
	bool cubemapIsEnabled = false;
	bool lodIsEnabled = false;
	bool shadowIsEnabled = false;



private:
	/*D3D11 Interfaces*/
	//Device
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;

	//Render Target(s)
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv;

	//Depth Stencil
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> dss;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> dst;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv;

	//Viewport(s)
	D3D11_VIEWPORT viewport;

	//AudioManager audioManager;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;

	//Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
};
