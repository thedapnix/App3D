#pragma once

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <vector>
#include <DirectXMath.h>

#include "ShaderResource.h"
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

	//std::string textureFileName;
};

class Drawable
{
public:
	Drawable(ID3D11Device* device, const BufferData& data, DirectX::XMFLOAT3 scaling, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 translation);
	~Drawable() = default;
	//Drawable& operator=(const Drawable&) = delete;
	//Drawable(const Drawable&) = delete;

	void Bind(ID3D11DeviceContext* context);
	void Draw(ID3D11DeviceContext* context) const;
	void UpdateConstantBuffer(ID3D11DeviceContext* context);

private:
	struct Transform
	{
		DirectX::XMFLOAT4X4 world;
	};

	/*Buffers and srv+srt*/
	VertexBuffer m_vertexBuffer;
	IndexBuffer m_indexBuffer;

	Transform m_transform;
	ConstantBuffer m_constantBuffer;

	//ShaderResource m_shaderResource;
};