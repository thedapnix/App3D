#pragma once

#include "Drawable.h"
#include <string>

int InitDrawableFromFile(std::string objFileName, std::vector<Drawable>& vecToFill, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotate, DirectX::XMFLOAT3 translate, std::unordered_map<std::string, ShaderResource>& textures, ID3D11Device* device, int interact, std::vector<int> interactsWith);


//Stores indices
struct InstanceInfo
{
	int original;
	std::vector<int> instanced;
};

int InitDrawableFromFileInstanced(std::string objFileName, std::string normalMapFileName,
	UINT& nDrawables,
	std::vector<UINT>& instancedDrawableCounts,
	std::unordered_map<std::string, InstanceInfo>& instances,
	std::unordered_map<int, std::vector<DirectX::XMFLOAT4X4>>& transforms,
	std::vector<Drawable>& vecToFill,
	DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotate, DirectX::XMFLOAT3 translate,
	std::unordered_map<std::string, ShaderResource>& textures, ID3D11Device* device,
	int interact, std::vector<int> interactsWith);