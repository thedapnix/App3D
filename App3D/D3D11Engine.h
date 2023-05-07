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

	//Deferred Renderer
	struct GBuffer
	{
		//G-buffers are essentially just textures for us to render to. We also supply them with an RTV and SRV to enable reading and writing (RTV write, SRV read) of the geometry data
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
	};

	/*Functions*/
	void RenderDef(float dt);
	void Render(float dt);
	void InitInterfaces(const HWND& window);
	void InitViewport();
	void InitRTV();
	void InitDepthStencil();
	void InitShadersAndInputLayout();
	void InitCamera();

	void InitUAV();
	void InitGraphicsBuffer(GBuffer(&gbuf)[3]);

	bool DrawableIsVisible(DirectX::BoundingFrustum frustum, DirectX::BoundingBox aabb, DirectX::XMMATRIX view, DirectX::XMMATRIX world);
	//void InitSampler();

	/*Temp*/
	void InitQuad(DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotate, DirectX::XMFLOAT3 translate);
	void InitCube(DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotate, DirectX::XMFLOAT3 translate);
	void UpdateConstantBuffer(ID3D11Buffer* cb, void* data, size_t size);

	bool InitDrawableFromFile(std::string fileName, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotate, DirectX::XMFLOAT3 translate);

	/*Variables*/
	UINT m_windowWidth;
	UINT m_windowHeight;
	static constexpr float CLEAR_COLOR[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	std::vector<Drawable> m_drawables;
	std::unique_ptr<Camera> m_camera;
	int m_drawablesBeingRendered = 0;
	GBuffer m_gBuffers[3];

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

	//Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

	//Deferred rendering
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> computeShader;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> uav;
};
