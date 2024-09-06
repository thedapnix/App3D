#pragma once

#include "Drawable.h"
#include "ShaderResource.h"
#include <string>

bool InitDrawableFromFile(std::string objFileName, std::vector<Drawable>& vecToFill, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotate, DirectX::XMFLOAT3 translate, std::unordered_map<std::string, ShaderResource>& textures, ID3D11Device* device, int interact);