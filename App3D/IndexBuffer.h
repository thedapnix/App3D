#pragma once

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <cstdint>
#include <iostream>
#include <d3d11_4.h>
#include <wrl.h>

class IndexBuffer
{
public:
	IndexBuffer() = default;
	~IndexBuffer() = default;

	void Init(ID3D11Device* device, UINT size, UINT indexCount, uint32_t* indexData);

	UINT GetIndexCount() const;
	ID3D11Buffer* GetBuffer() const;
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
	UINT m_count = 0;
};