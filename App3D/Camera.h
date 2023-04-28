#pragma once

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <vector>

#include "D3D11MathHelper.h"

class Camera
{
public:
	Camera();
	~Camera() = default;

	/*GETTERS*/
	//Camera position
	//DirectX::XMVECTOR GetPosition() const;
	DirectX::XMFLOAT3 GetPosition() const;

	//X-, Y- and Z-vectors
	DirectX::XMVECTOR GetRight() const;
	DirectX::XMVECTOR GetUp() const;
	DirectX::XMVECTOR GetLook() const;

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
	void Strafe(float d);
	void Walk(float d);

	//Rotate
	void Pitch(float angle);
	void RotateY(float angle);

	//Update view matrix after modification of position/orientation
	void UpdateViewMatrix();
private:
	//Coordinate system of the camera relative to world space
	DirectX::XMFLOAT3 m_position = { 0.0f, 0.0f, -3.0f };
	DirectX::XMFLOAT3 m_right = { 1.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 m_up = { 0.0f, 1.0f, 0.0f };
	DirectX::XMFLOAT3 m_look = { 0.0f, 0.0f, 1.0f };

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
};