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

#include "SubMesh.h"

/*Structs that D3D11Engine needs to know about*/
struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 texcoord;
	DirectX::XMFLOAT3 normal;
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

class Drawable
{
public:
	Drawable(ID3D11Device* device, const BufferData& data, DirectX::XMFLOAT3 scaling, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 translation);
	~Drawable() = default;

	//void InitTexture(ID3D11Device* device, const char* textureFileName);

	void Bind(ID3D11DeviceContext* context) const;
	void BindSubMesh(ID3D11DeviceContext* context, UINT index) const;
	void Draw(ID3D11DeviceContext* context, UINT index) const;
	void Unbind(ID3D11DeviceContext* context);
	void UpdateConstantBuffer(ID3D11DeviceContext* context);
	void CreateBoundingBoxFromPoints(DirectX::XMVECTOR min, DirectX::XMVECTOR max);

	//Movement stuff
	void EditTranslation(float x, float y, float z);
	void Rotate(float angleX, float angleY, float angleZ);

	/*Getters*/
	DirectX::XMMATRIX World() const;
	const DirectX::BoundingBox GetBoundingBox() const;
	const VertexBuffer& GetVertexBuffer() const;
	void* GetVertexVectorData();

private:
	struct WorldTransform
	{
		DirectX::XMFLOAT4X4 scale;
		DirectX::XMFLOAT4X4 rotate;
		DirectX::XMFLOAT4X4 translate;
	};
	__declspec(align(16)) struct ShininessCB
	{
		float shininess;
	};
	DirectX::XMFLOAT3 m_scale;
	DirectX::XMFLOAT3 m_rotate;
	DirectX::XMFLOAT3 m_translate;
	WorldTransform m_transform;
	ShininessCB m_shineCB;
	ConstantBuffer m_constantBuffer;
	ConstantBuffer m_constantBufferShininess;

	std::vector<SubMesh> m_submeshes;

	/*Buffers*/
	VertexBuffer m_vertexBuffer;
	IndexBuffer m_indexBuffer;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> ambientSRT;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> diffuseSRT;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> specularSRT;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> srt;
	/*Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ambientSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuseSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> specularSRV;*/
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;

	DirectX::BoundingBox m_aabb;

	void* m_vertexVectorData;
};