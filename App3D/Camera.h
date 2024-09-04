#pragma once

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <vector>
#include <d3d11.h>
#include <DirectXCollision.h>

#include "ConstantBuffer.h"
#include "D3D11MathHelper.h"

class Camera
{
public:
	Camera();
	~Camera() = default;
	//Camera& operator=(const Camera&) = delete;
	//Camera(const Camera&) = delete;

	/*GETTERS*/
	//Camera position
	DirectX::XMVECTOR GetPositionVec() const;
	DirectX::XMFLOAT3 GetPosition() const;

	//Bounding Sphere
	DirectX::BoundingSphere GetBoundingSphere() const;
	void SetBoundingSphereCenter(DirectX::XMFLOAT3 pos);

	//X-, Y- and Z-vectors
	DirectX::XMVECTOR GetRight() const;
	//DirectX::XMVECTOR GetUp() const;
	DirectX::XMFLOAT3 GetUp() const;
	//DirectX::XMVECTOR GetLook() const;
	DirectX::XMFLOAT3 GetLook() const;

	//Frustum (projection matrix requires near- and far-z, aspect ratio and fov)
	float GetNearZ() const;
	float GetFarZ() const;
	float GetAspect() const;
	float GetFovY() const;
	float GetFovX() const;

	float GetNearWindowWidth() const;
	float GetNearWindowHeight() const;
	float GetFarWindowWidth() const;
	float GetFarWindowHeight() const;

	//new
	float GetYaw() const;
	DirectX::XMVECTOR GetDefaultLook() const;

	//View- and Projection-matrices
	DirectX::XMMATRIX View() const;
	DirectX::XMMATRIX Proj() const;

	/*SETTERS*/
	//Camera position
	void SetPosition(float x, float y, float z);
	void SetPosition(const DirectX::XMFLOAT3& v);

	//Z-vector, needs the Y-vector (aka the up-vector)
	void LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp);
	void LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);

	//Frustum
	void SetLens(float fovY, float aspect, float zn, float zf);

	/*COMMANDS*/
	//Strafe and walk a distance d
	DirectX::XMFLOAT3 Strafe(float d);
	void Walk(float d);

	//Temp
	DirectX::XMFLOAT3 PlayerWalk(float d);

	//Rotate
	void Pitch(float angle);
	void PitchFPC(float angle); //new: first person camera version
	void RotateY(float angle);

	//Update view matrix after modification of position/orientation
	void UpdateViewMatrix();

	/*New functionality since moving stuff from D3D11Engine to here*/
	void UpdateConstantBuffer(ID3D11DeviceContext* context);
	const ConstantBuffer& GetConstantBuffer() const;
	const DirectX::BoundingFrustum& GetFrustum() const;
	void InitConstantBufferAndFrustum(ID3D11Device* device);

private:
	//Coordinate system of the camera relative to world space (starting position)
	DirectX::XMFLOAT3 m_position = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 m_right = { 1.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 m_up = { 0.0f, 1.0f, 0.0f };
	DirectX::XMFLOAT3 m_look = { 0.0f, 0.0f, 1.0f };

	/*DEFAULTS*/
	DirectX::XMVECTOR m_defaultLook = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	DirectX::XMVECTOR m_defaultRight = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	/*ROTATIONS*/
	float m_pitch = 0.0f;
	float m_yaw = 0.0f;

	//Frustum properties
	float m_nearZ = 0.0f;
	float m_farZ = 0.0f;
	float m_aspect = 0.0f;
	float m_fovY = 0.0f;
	float m_nearWindowHeight = 0.0f;
	float m_farWindowHeight = 0.0f;

	//boolean to check if we've made changes to the camera
	bool m_viewDirty = true;

	//View/Proj matrices
	DirectX::XMFLOAT4X4 m_view = Identity4x4();
	DirectX::XMFLOAT4X4 m_proj = Identity4x4();

	//Constant buffer stuff moved from D3D11Engine
	__declspec(align(16)) struct CameraData
	{
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 proj;
		DirectX::XMFLOAT3 pos;
	};
	CameraData m_cameraData;
	ConstantBuffer m_cameraCB;

	//Bounding volumes
	DirectX::BoundingFrustum m_frustum;
	DirectX::BoundingBox m_aabb; //Obsolete now because we're using sphere but I'm keeping it just in case
	DirectX::BoundingSphere m_sphere;
};