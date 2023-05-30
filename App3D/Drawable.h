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

	std::string textureFileName;
};

class Drawable
{
public:
	Drawable(ID3D11Device* device, const BufferData& data, DirectX::XMFLOAT3 scaling, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 translation);
	~Drawable() = default;

	void InitTexture(ID3D11Device* device, const char* textureFileName);

	void Bind(ID3D11DeviceContext* context, ID3D11ShaderResourceView* inputSRV);
	void Draw(ID3D11DeviceContext* context) const;
	void Unbind(ID3D11DeviceContext* context);
	void UpdateConstantBuffer(ID3D11DeviceContext* context);
	void CreateBoundingBoxFromPoints(DirectX::XMVECTOR min, DirectX::XMVECTOR max);

	//Movement stuff
	void EditTranslation(float x, float y, float z);
	void RotateY(float dt);

	/*Getters*/
	DirectX::XMMATRIX World();
	DirectX::BoundingBox GetBoundingBox();
	const VertexBuffer& GetVertexBuffer() const;
	void* GetVertexVectorData();

private:
	struct WorldTransform
	{
		DirectX::XMFLOAT4X4 scale;
		DirectX::XMFLOAT4X4 rotate;
		DirectX::XMFLOAT4X4 translate;
	};
	DirectX::XMFLOAT3 m_scale;
	DirectX::XMFLOAT3 m_rotate;
	DirectX::XMFLOAT3 m_translate;
	WorldTransform m_transform;
	ConstantBuffer m_constantBuffer;

	/*Buffers*/
	VertexBuffer m_vertexBuffer;
	IndexBuffer m_indexBuffer;

	/*To-do, shader resource views allowing for all drawables to be textured*/
	Microsoft::WRL::ComPtr<ID3D11Texture2D> srt;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;

	DirectX::BoundingBox m_aabb;

	void* m_vertexVectorData;
};