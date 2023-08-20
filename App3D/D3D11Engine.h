#pragma once

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <fstream>
#include <sstream>
#include <unordered_map>

#include "ConstantBuffer.h"
#include "Drawable.h"
#include "Camera.h"
#include "CubeMap.h"
#include "ParticleSystem.h"
#include "ShadowMap.h"
#include "QuadTree.h"
#include "ShaderResource.h" //new

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

private:
	//Deferred Renderer
	struct GBuffer
	{
		//G-buffers are essentially just textures for us to render to. We also supply them with an RTV and SRV to enable reading and writing (RTV write, SRV read) of the geometry data
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
	};

	/*PRIVATE FUNCTIONS*/
	/*Render functions*/
	void Render(float dt, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv, D3D11_VIEWPORT* viewport, Camera* cam, const float clear[4]);
	void RenderParticles(Camera* cam);
	void RenderReflectiveObject(float dt);
	void RenderDepth(float dt);
	void DefPassOne(Camera* cam);
	void DefPassTwo(Camera* cam);

	/*Initializers because this constructor would be HUGE otherwise*/
	//"Regular" stuff
	void InitInterfaces(const HWND& window);
	void InitViewport();
	void InitRTV();
	void InitDepthStencil();
	void InitShadersAndInputLayout();
	bool InitDrawableFromFile(std::string objFileName, std::vector<Drawable>& vecToFill, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotate, DirectX::XMFLOAT3 translate);
	void InitSampler();
	void InitCamera();

	//Deferred
	void InitUAV();
	void InitGraphicsBuffer(GBuffer(&gbuf)[4]);

	//LOD
	void InitRasterizerStates();

	//Shadow
	void InitSpotlights();

	//Internal function called by the obj-parser to now also parse materials
	bool ParseMaterial(std::string mtlFileName, std::string& ambient, std::string& diffuse, std::string& specular, float& shininess);
	std::unordered_map<std::string, ShaderResource> m_textures;
	
	/*VARIABLES*/
	UINT m_windowWidth;
	UINT m_windowHeight;
	static constexpr float CLEAR_COLOR[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	static constexpr float TEST_COLOR[4] = { 0.1f, 0.5f, 0.1f, 1.0f };
	std::vector<Drawable> m_drawables;
	std::vector<Drawable> m_reflectiveDrawables;
	SpotLights m_spotlights;
	std::unique_ptr<Camera> m_camera;
	int m_drawablesBeingRendered = 0;
	GBuffer m_gBuffers[4];
	float particleVar = 0.0f;

	/*ImGui variables*/
	int m_fpsCounter = 0;
	std::string m_fpsString = "";
	bool deferredIsEnabled = false;
	bool cullingIsEnabled = false;
	bool billboardingIsEnabled = false;
	bool cubemapIsEnabled = false;
	bool lodIsEnabled = false;

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

	//Shaders + input layout
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

	//Deferred rendering
	Microsoft::WRL::ComPtr<ID3D11PixelShader> deferredPixelShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> computeShader;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> uav;

	//Tessellation
	Microsoft::WRL::ComPtr<ID3D11HullShader> hullShader;
	Microsoft::WRL::ComPtr<ID3D11DomainShader> domainShader;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> regularRS;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> wireframeRS;

	//Clearly there came a time where I started making actual classes instead of bloating this D3D11Engine class. Will I rewrite things?
	//Guhhh...?
	ParticleSystem m_particles;
	CubeMap m_cubeMap;
	ShadowMap m_shadowMap;
	QuadTree<Drawable> m_quadTree;
};
