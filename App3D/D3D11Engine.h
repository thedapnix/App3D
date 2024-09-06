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
#include "ObjParser.h"

#include "EntityFramework.h" //ECS baby

class D3D11Engine
{
public:
	D3D11Engine(const HWND& hWnd, const UINT& width, const UINT& height);
	~D3D11Engine();
	D3D11Engine& operator=(const D3D11Engine&) = delete;
	D3D11Engine(const D3D11Engine&) = delete;

	//Render and ImGui
	void Update(float dt);
	void ImGuiSceneData(D3D11Engine* d3d11engine, bool shouldUpdateFps, int state, int rawX, int rawY); //new: raw x and y mouse coordinates for debug purposes

	//Movement and camera stuff
	void MovePlayerX(float speed);
	void MovePlayerZ(float speed);
	Camera& GetCamera() const noexcept;
	void PlayerInteract();

	//Drawable and culling stuff
	bool CreateDrawable(std::string objFileName, DirectX::XMFLOAT3 translate = { 0.0f, 0.0f, 0.0f }, DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f }, DirectX::XMFLOAT3 rotate = { 0.0f, 0.0f, 0.0f }, int interact = 0);
	bool CreateReflectiveDrawable(std::string objFileName, DirectX::XMFLOAT3 translate = { 0.0f, 0.0f, 0.0f }, DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f }, DirectX::XMFLOAT3 rotate = { 0.0f, 0.0f, 0.0f });
	bool MoveDrawable(int i, DirectX::XMFLOAT3 dist);
	bool SetupQT();
	void RemoveDrawableInteraction(int id);
	void DestroyDrawable(int id);
	int GetDrawableIndexFromInteraction(int interactId);

	//Lights stuff
	bool CreateLightSpot(DirectX::XMFLOAT3 position, float fov, float rotX, float rotY, DirectX::XMFLOAT3 color = {1.0f, 1.0f, 1.0f});
	bool CreateLightDir(DirectX::XMFLOAT3 position, float rotX, float rotY, DirectX::XMFLOAT3 color = { 0.25f, 0.25f, 0.25f });
	bool SetupLights(); //Because of how my current Spotlights class works, going to change this in the future but I shouldn't procastinate too much by just making the engine cool

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
	bool InitEntityGraphics(EntityID& entity, DirectX::XMFLOAT3 translate);
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
	std::unordered_map<std::string, ShaderResource> m_textures;
	
	/*VARIABLES*/
	UINT m_windowWidth;
	UINT m_windowHeight;
	static constexpr float CLEAR_COLOR[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	static constexpr float TEST_COLOR[4] = { 0.1f, 0.5f, 0.1f, 1.0f };
	std::vector<Drawable> m_drawables;
	std::vector<Drawable> m_reflectiveDrawables;
	std::vector<Drawable> m_interactibleDrawables; //added a bit late into implementation so i'm not sure that i want to modify the m_drawables too much right now but let me cook
	std::vector<LightData> m_lightDataVec;
	SpotLights m_spotlights;
	std::unique_ptr<Camera> m_camera;
	GBuffer m_gBuffers[4];
	float m_particleVar = 0.0f;

	/*ImGui variables*/
	int fpsCounter = 0;
	std::string fpsString = "";
	bool deferredIsEnabled = false; //Dim the lights baby
	bool cullingIsEnabled = false;
	bool billboardingIsEnabled = false;
	bool cubemapIsEnabled = false;
	bool lodIsEnabled = false;
	int drawablesBeingRendered = 0;
	int selectableDrawables = 0;

	/*Collision stuff*/
	DirectX::XMFLOAT3& ClosestPointOnBox(const DirectX::XMFLOAT3& point, const DirectX::BoundingBox& box); //Returns the closest point on the box in comparison to a given point
	void MovePlayer(const DirectX::XMFLOAT3& toMove);
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

	/*ECS*/
	Registry m_registry;
};
