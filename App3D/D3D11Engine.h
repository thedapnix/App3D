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

//New: struct for instanced data (Debating whether or not drawables should have this, or if it should be outside of the class)
//struct InstancedData
//{
//	int originalIndex;
//	std::vector<int> instancedIndices;
//	std::vector <DirectX::XMFLOAT4X4> instancedTransforms;
//};

struct InstancedData
{
	DirectX::XMFLOAT4X4 world;
};

class D3D11Engine
{
public:
	D3D11Engine(const HWND& hWnd, const UINT& width, const UINT& height);
	~D3D11Engine();
	D3D11Engine& operator=(const D3D11Engine&) = delete;
	D3D11Engine(const D3D11Engine&) = delete;

	//Render and ImGui
	void Update(float dt);
	void ImGuiSceneData(D3D11Engine* d3d11engine, bool shouldUpdateFps, int state, int rawX, int rawY);

	//Movement and camera stuff
	void MovePlayerX(float speed);
	void MovePlayerZ(float speed);
	Camera& GetCamera() const noexcept;
	void PlayerInteract();
	void UpdateMovingDrawables(float dt);

	//Drawable and culling stuff
	int CreateDrawable(			std::string objFileName, std::string normalmapFileName = "",
		DirectX::XMFLOAT3 translate = { 0.0f, 0.0f, 0.0f }, DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f }, DirectX::XMFLOAT3 rotate = { 0.0f, 0.0f, 0.0f }, 
		int interact = 0, std::vector<int> interactsWith = {});
	int CreateReflectiveDrawable(	std::string objFileName, std::string normalmapFileName = "", 
		DirectX::XMFLOAT3 translate = { 0.0f, 0.0f, 0.0f }, DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f }, DirectX::XMFLOAT3 rotate = { 0.0f, 0.0f, 0.0f }, 
		int interact = 0, std::vector<int> interactsWith = {});
	int CreatePovDrawable(			std::string objFileName, std::string normalmapFileName = "", 
		DirectX::XMFLOAT3 translate = { 0.0f, 0.0f, 0.0f }, DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f }, DirectX::XMFLOAT3 rotate = { 0.0f, 0.0f, 0.0f }, 
		int interact = 0, std::vector<int> interactsWith = {});
	int CreateConcaveDrawable(std::string objFileName, std::string normalmapFileName = "",
		DirectX::XMFLOAT3 translate = { 0.0f, 0.0f, 0.0f }, DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f }, DirectX::XMFLOAT3 rotate = { 0.0f, 0.0f, 0.0f }, 
		int interact = 0, std::vector<int> interactsWith = {});
	int CreateOrbitDrawable(std::string objFileName, std::string normalmapFileName = "", 
		DirectX::XMFLOAT3 translate = { 0.0f, 0.0f, 0.0f }, DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f }, DirectX::XMFLOAT3 rotate = { 0.0f, 0.0f, 0.0f }, 
		int interact = 0, std::vector<int> interactsWith = {});
	int CreateInstancedDrawable(std::string objFileName, std::string normalmapFileName = "", 
		DirectX::XMFLOAT3 translate = { 0.0f, 0.0f, 0.0f }, DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f }, DirectX::XMFLOAT3 rotate = { 0.0f, 0.0f, 0.0f }, 
		int interact = 0, std::vector<int> interactsWith = {});
	bool MoveDrawable(int i, DirectX::XMFLOAT3 dist);
	bool SetupQT();
	void RemoveDrawableInteraction(int id);
	void DestroyDrawable(int id);
	int GetDrawableIndexFromInteraction(int interactId);
	std::vector<Drawable>& GetDrawables();
	std::vector<Drawable>& GetPovDrawables();

	//Lights stuff
	bool CreateLightSpot(DirectX::XMFLOAT3 position, float fov, float rotX, float rotY, DirectX::XMFLOAT3 color = {1.0f, 1.0f, 1.0f});
	bool CreateLightDir(DirectX::XMFLOAT3 position, float rotX, float rotY, DirectX::XMFLOAT3 color = { 0.25f, 0.25f, 0.25f });
	bool CreateLightPoint(DirectX::XMFLOAT3 position, float rad, DirectX::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f });
	bool SetupLights(); //Because of how my current Spotlights class works, going to change this in the future but I shouldn't procastinate too much by just making the engine cool

	//New: Instanced (Similarly to how we call SetupQT() and SetupLights() after any SetupLevel()-call, we now also SetupInstancedBuffer())
	void ResizeInstanceBuffer(int size);
	void SetupInstancedBuffer(int begin, int end, int index);

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
	//void Render(float dt, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv, D3D11_VIEWPORT* viewport, Camera* cam, const float clear[4]);
	void Render(ID3D11UnorderedAccessView* uav, ID3D11DepthStencilView* dsv, D3D11_VIEWPORT* viewport, Camera* cam, const float clear[4], UINT csX, UINT csY, bool isReflection = false);
	void RenderParticles(Camera* cam);
	void RenderReflectiveObject(Camera* cam, CubeMap* cubeMap, int index);
	void RenderDepth(float dt);
	void DefPassOne(Camera* cam, ID3D11DepthStencilView* dsv, D3D11_VIEWPORT* viewport, bool isReflection = false);
	void DefPassTwo(Camera* cam, ID3D11UnorderedAccessView* uav, UINT csX, UINT csY);
	void DrawInstanced(const Drawable* baseDrawable, bool isDepth = false);

	/*Initializers because this constructor would be HUGE otherwise*/
	//"Regular" stuff
	void InitInterfaces(const HWND& window);
	void InitViewport(D3D11_VIEWPORT& vp);
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
	std::vector<Drawable> m_povDrawables;
	std::vector<LightData> m_lightDataVec;

	//New: Instancing stuff
	//std::unordered_map<std::string, std::vector<int>> m_instanceMap; //Maps the mesh name (obj-file) to a vector of drawable indices (as well as keeps track of the original)
	std::unordered_map<std::string, InstanceInfo> m_instanceMap;
	//std::unordered_map<int, DirectX::XMFLOAT4X4> m_transformMap;
	std::unordered_map<int, std::vector<DirectX::XMFLOAT4X4>> m_transformMap;
	std::vector<InstancedData> m_instancedData;
	//Microsoft::WRL::ComPtr<ID3D11Buffer> m_instancedBuffer;
	std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> m_instancedBuffers; //Maybe I should only have one instanced buffer, and write to parts at a time? (Better yet, maybe I let every drawable have its own instanced buffer)
	UINT m_instancedDrawableCount;
	UINT m_totalDrawableCount = 0; //Trust
	std::vector<UINT> m_instancedDrawableCounts;
	std::vector<int> m_instancedDataSizes;

	SpotLights m_spotlights;
	std::unique_ptr<Camera> m_camera;
	GBuffer m_gBuffers[4];
	float m_particleVar = 0.0f;

	//Dirty temp variables
	bool targetGoingUp = true;
	bool targetGoingForward = true;
	bool targetSwingingOriginal = true;
	DirectX::BoundingBox defaultBox; //New for instancing

	/*ImGui variables*/
	int fpsCounter = 0;
	std::string fpsString = "";
	bool deferredIsEnabled = true;
	bool cullingIsEnabled = false; //Uhhhhhh i literally have better performance without culling
	bool billboardingIsEnabled = false;
	bool cubemapIsEnabled = true;
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

	//New: Render scene to a texture as well, so we can pass it to ImGui and do ImGui::Image() to present instead of doing windows stuff? Perchance? (Not yet done)
	Microsoft::WRL::ComPtr<ID3D11Texture2D> textureToGui;

	//Render Target(s)
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv;

	//Depth Stencil
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> dss;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> dst;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv;

	//Viewport(s)
	D3D11_VIEWPORT viewport;
	D3D11_VIEWPORT povViewport; //Does nothing, but also doesn't break anything so (I want to have pov stuff be its own thing kinda drawn "on top" of the rest of the scene)

	//Shaders + input layout
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

	//Deferred rendering
	Microsoft::WRL::ComPtr<ID3D11PixelShader> deferredPixelShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> computeShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> computeShaderCM; //Cubemap gets its own compute shader
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> uav;

	//Tessellation
	Microsoft::WRL::ComPtr<ID3D11HullShader> hullShader;
	Microsoft::WRL::ComPtr<ID3D11DomainShader> domainShader;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> regularRS;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> wireframeRS;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> nonBackfaceCullRS; //Concave objects shouldn't be backface culled, because then you can't see inside of them which is kinda the point

	//Clearly there came a time where I started making actual classes instead of bloating this D3D11Engine class. Will I rewrite things?
	//Guhhh...?
	ParticleSystem m_particles;
	std::vector<CubeMap> m_cubeMaps;
	ShadowMap m_shadowMap;
	QuadTree<Drawable> m_quadTree;

	/*ECS*/
	Registry m_registry;
};
