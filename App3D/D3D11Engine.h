#pragma once

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <fstream>
#include <sstream>

#include <DirectXCollision.h>

#include "ConstantBuffer.h"
#include "Drawable.h"
#include "Camera.h"

class D3D11Engine
{
public:
	D3D11Engine(const HWND& hWnd, const UINT& width, const UINT& height);
	~D3D11Engine();

	void Update(float dt);
	//void ImGuiSceneToRender(Scene& sceneToRender, D3D11Engine* d3d11engine, bool shouldUpdate);

	Camera& GetCamera();
	//ID3D11Device* GetDevice();

private:
	/*Structs*/
	struct ViewProj
	{
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 proj;
	};

	/*Functions*/
	void Render(float dt);
	void InitInterfaces(const HWND& window);
	void InitRTV();
	void InitViewport(const UINT& width, const UINT& height);
	void InitVertexShader();
	void InitInputLayout();
	void InitPixelShader();
	void InitCamera();
	//void InitSampler();
	void InitQuad(DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotate, DirectX::XMFLOAT3 translate); // temp
	void InitCube(DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotate, DirectX::XMFLOAT3 translate);
	void UpdateConstantBuffer(ID3D11Buffer* cb, void* data, size_t size); //temp

	/*Variables*/
	static constexpr float CLEAR_COLOR[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	std::vector<Drawable> m_drawables;
	std::unique_ptr<Camera> m_camera;
	ViewProj m_viewProj;
	ConstantBuffer m_cameraCB;
	//int fpsCounter = 0;
	//std::string fpsString = "";

private:
	/*D3D11 Interfaces*/
	//Device
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;

	//Render Targets
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv;
	//Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;

	//Depth Stencil
	/*Microsoft::WRL::ComPtr<ID3D11DepthStencilState> dss;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> dst;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv;*/

	//Viewports
	D3D11_VIEWPORT viewport;

	//AudioManager audioManager;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;

	//Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
};
