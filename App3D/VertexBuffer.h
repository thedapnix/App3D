#pragma once

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <iostream>
#include <d3d11.h>
#include <wrl.h>

class VertexBuffer {
public:
	VertexBuffer() = default;
	~VertexBuffer() = default;
	//VertexBuffer& operator=(const VertexBuffer&) = delete;
	//VertexBuffer(const VertexBuffer&) = delete;

	void Init(ID3D11Device* device, UINT vertexSize, UINT vertexCount, void* vertexData);

	UINT GetVertexCount() const;
	UINT GetVertexSize() const;
	ID3D11Buffer* GetBuffer() const;
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;

	UINT m_size = 0;
	UINT m_count = 0;
};