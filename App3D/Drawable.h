#pragma once

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <vector>
#include <DirectXMath.h>
#include <DirectXCollision.h>

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "ShaderResource.h"

#include "SubMesh.h"

#include <unordered_map>

#include "ColliderAABB.h"
#include "Camera.h" //Orbit

/*Structs that D3D11Engine needs to know about*/
struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 texcoord;
	DirectX::XMFLOAT3 normal;
};

struct MaterialData
{
	//Default values
	std::string ambientData = "dark_grey.png";
	std::string diffuseData = "grey.png";
	std::string specularData = "light_grey.png";
	float shineData = 1.0f;
};

struct BufferData
{
	struct VertexData
	{
		UINT size;						//sizeof(Vertex)
		UINT count;						//vCount
		std::vector<Vertex> vector;
	} vData;

	struct IndexData
	{
		UINT size;						//sizeof(UINT)
		UINT count;						//iCount
		std::vector<UINT> vector;
	} iData;

	struct MaterialData
	{
		std::string ambient;
		std::string diffuse;
		std::string specular;
		float shininess;
	} mData;

	struct SubMeshData
	{
		UINT startIndex;
		UINT indexCount;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ambientSRV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuseSRV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> specularSRV;
		float shininess;
	};
	std::vector<SubMeshData> subMeshVector;
};

struct ParsedData
{
	std::vector<DirectX::XMFLOAT3> vPos;
	std::vector<DirectX::XMFLOAT2> vTex;
	std::vector<DirectX::XMFLOAT3> vNor;

	std::unordered_map<std::string, MaterialData> materials;

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	/*Material data to pass to drawable, give default values here that we use if the text file doesn't specify a material*/
	std::string currentSubmeshMaterial = "";
	UINT currentSubmeshIndex = 0;
	std::vector<BufferData::SubMeshData> submeshes;

	//faces
	std::unordered_map<std::string, UINT> refs;
};

enum class Interactions
{
	INVALID = 0, //default case
	DESTROY = 1,
	//and just add however many interactions you want (currently just using unsigned int because I know my own interactions but this is better in terms of usability)
};

//Information that the user may like to have access to
struct DrawableInfo
{
	unsigned int interactID = 0;	//An ID that can be referenced in order to know what this thing does (Function pointer magic time?)
	std::vector<int> interactsWith;	//Indices of the drawables that this drawable affects with its interaction
	bool isActive = true;			//Lets the engine know whether or not we bother rendering or collision-checking this drawable
};

struct OrbitData
{
	bool doesOrbit = false;
	DirectX::XMFLOAT3 point = { 0.0f, 0.0f, 0.0f };
};

class Drawable
{
public:
	Drawable(ID3D11Device* device, const BufferData& data, DirectX::XMFLOAT3 scaling, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 translation, int interact, std::vector<int> interactsWith);
	~Drawable() = default;

	void Draw(ID3D11DeviceContext* context) const;
	void DrawInstanced(ID3D11DeviceContext* context) const;
	void UpdateConstantBuffer(ID3D11DeviceContext* context, Camera* camera = nullptr, const DirectX::XMFLOAT3& pos = {0.0f, 0.0f, 0.0f} );
	void CreateBoundingBoxFromPoints(DirectX::XMVECTOR min, DirectX::XMVECTOR max);

	//Movement stuff
	void SetPosition(float x, float y, float z);
	void Move(float x, float y, float z);
	void SetRotation(float angleX, float angleY, float angleZ);
	void Rotate(float angleX, float angleY, float angleZ);

	/*Getters*/
	const DirectX::XMFLOAT3& GetPosition() const;
	const DirectX::XMFLOAT3& GetRotation() const;
	DirectX::XMMATRIX World() const;
	const DirectX::BoundingBox& GetBoundingBox() const;
	const VertexBuffer& GetVertexBuffer() const;
	const IndexBuffer& GetIndexBuffer() const; //New:
	void* GetVertexVectorData();
	const ConstantBuffer& GetConstantBuffer() const; //New:
	const std::vector<SubMesh>& GetSubMeshes() const; //New:

	//New: Pass in a wholeass XMMATRIX and use it to set the world matrix
	void SetWorld(DirectX::XMMATRIX mat);

	/*Interaction stuff*/
	bool IsInteractible() const;
	int GetInteractID() const;
	DrawableInfo& GetDrawableInfo();
	void RemoveInteraction();
	bool IsActive() const;
	void Destroy();

	/*Rendering stuff*/
	void SetReflective(bool set = true);
	bool IsReflective() const;
	void SetConcave(bool set = true);
	bool IsConcave() const;
	bool DoesOrbit() const;
	void SetOrbit(bool set = true);
	void SetNormalMap(ID3D11Device* device, std::string ddsFileName);

	//Magic
	void Interact(int (*funcPtr)(DrawableInfo, std::vector<Drawable>&), std::vector<Drawable>& drawables)
	{
		funcPtr(m_drawableInfo, drawables);
	}

	//New: For instancing
	void SetIndex(int idx);
	const int& GetIndex() const;
	void SetInstanceCount(int n);
	const int& GetInstanceCount() const;

private:
	__declspec(align(16)) struct ConstantBufferData
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4X4 invWorld; //Also transposed?

		bool hasNormalMap = false;
	};
	DirectX::XMFLOAT3 m_scale;
	DirectX::XMFLOAT3 m_rotate;
	DirectX::XMFLOAT3 m_translate;
	ConstantBufferData m_cbd;
	ConstantBuffer m_constantBuffer;
	void CalculateAndTransposeWorld(const DirectX::XMFLOAT3& pos = { 0.0f, 0.0f, 0.0f }, Camera* camera = nullptr);

	//New: For instancing
	int m_index = -1;
	int m_instanceCount = 0;

	std::vector<SubMesh> m_submeshes;

	/*Buffers*/
	VertexBuffer m_vertexBuffer;
	IndexBuffer m_indexBuffer;
	//InstancedBuffer m_instancedBuffer; //New: Instanced data

	Microsoft::WRL::ComPtr<ID3D11Texture2D> ambientSRT;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> diffuseSRT;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> specularSRT;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> srt;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;

	DirectX::BoundingBox m_aabb;

	void* m_vertexVectorData;

	DrawableInfo m_drawableInfo;

	//Variables for drawables with specific rendering qualities
	bool m_isReflective = false;//Need to access this when we do culling with cubemaps
	bool m_isConcave = false;	//Need to access this when we choose which rasterizer state to use when rendering objects, so as to not to perform backface culling on concave objects
	bool m_orbits = false;

	//Normal mapping
	bool m_hasNormalMap = false;
	ShaderResource m_normalMapTexture;

	bool m_isDirty = false;
};