#pragma once

#include "Camera.h"
#include <d3d11.h>
#include <wrl.h>
#include <vector>

/*Quoted from cookbook:
As the technique aims to render the environment onto the different sides of a texture cube, there is a need for virtual "cameras" that are appropriately set up.
There will be one camera per side, each with a 90 degree field of view in both their "X" and "Y" axis's. 
This will result in everything in the surrounding environment being visible (but potentially obscured of course) for at least one of the cameras, without either holes or overlap. 
The cameras are often positioned in the centre of the object that will have the texture cube applied to it. 
This is not strictly necessary however, and adjustments to get a more desirable result can be done.
*/

class CubeMap
{
private: //Will this need to be public? We'll find out
	class CubeMapView //The view of every virtual camera
	{
	public:
		CubeMapView() = default;
		CubeMapView(ID3D11Device* device, UINT width, UINT height, bool hasSRV);
		~CubeMapView() = default;

		ID3D11RenderTargetView* GetRenderTargetViewAt(int index) { return rtvs.at(index).Get(); }
	private:
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
		std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> rtvs;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
	};
public:
	CubeMap() = default;
	//CubeMap(ID3D11Device* device, bool hasSRV);
	~CubeMap() = default;

	void Init(ID3D11Device* device, bool hasSRV);

	ID3D11RenderTargetView* GetRenderTargetViewAt(int index);
	ID3D11DepthStencilView* GetDepthStencilView();
	const D3D11_VIEWPORT& GetViewport() const;
	Camera* GetCameraAt(int index);

private:
	//Note the comment in CubeMapView constructor: "Texture cube is 6 textures combined in a cubic pattern, where we let indices 0-1 be positive and negative X, 2-3 be Y, 4-5 be Z"
	static constexpr UINT m_sides = 6;
	enum class CubeFace
	{
		POSITIVE_X, NEGATIVE_X,
		POSITIVE_Y, NEGATIVE_Y,
		POSITIVE_Z, NEGATIVE_Z
	};
	Camera m_cameras[m_sides];
	CubeMapView m_cubeMapView;

	//Other required resources, as stated by the cookbook, include a depth buffer and a viewport, matching the dimensions of the texture cube sides
	void InitDepthBuffer(ID3D11Device* device, UINT width, UINT height);
	Microsoft::WRL::ComPtr<ID3D11Texture2D> dst;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv;
	D3D11_VIEWPORT viewport;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
};