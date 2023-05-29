#include "Spotlight.h"

SpotLight::SpotLight(ID3D11Device* device, const LightData& lightData)
{
	//Create virtual camera just like in cubemap
	float fovY = lightData.fovY;
	float aspect = 1.0f;
	float zn = 0.1f;
	float zf = 100.0f;

	//Local rotations to align the cameras to look in the proper direction
	float upRotation = lightData.rotY;
	float rightRotation = lightData.rotX;

	//Create virtual camera with the data we have
	m_camera.SetPosition(lightData.pos);
	m_camera.RotateY(rightRotation);
	m_camera.Pitch(upRotation);
	m_camera.SetLens(fovY, aspect, zn, zf);
	m_camera.UpdateViewMatrix();
	m_camera.InitConstantBufferAndFrustum(device);
}

const ConstantBuffer& SpotLight::GetCameraConstantBuffer() const
{
	return m_camera.GetConstantBuffer();
}
