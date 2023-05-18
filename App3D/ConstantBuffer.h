#pragma once

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <d3d11.h>
#include <iostream>
#include <wrl.h>

class ConstantBuffer
{
public:
	ConstantBuffer() = default;
	//ConstantBuffer(ID3D11Device* device, size_t byteSize, void* initialData = nullptr);
	~ConstantBuffer() = default;

	void Init(ID3D11Device* device, void* initialData, size_t byteSize);
	void UpdateBuffer(ID3D11DeviceContext* context, void* data);

	ID3D11Buffer* GetBuffer() const;
	ID3D11Buffer* const *GetBufferAddress() const;
	size_t GetSize() const;

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
	size_t m_bufferSize = 0;
};
