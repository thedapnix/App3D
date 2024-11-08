#include "Camera.h"

using namespace DirectX;

#define CAMERA_Y_OFFSET 9.0f //Temp: Let the camera collider be offset so technically we're checking for collisions by our "feet", not by our "face"

//camera hitbox is at 3.0f, and reached up and down between 2.0f and 4.0f

Camera::Camera()
{
    SetPosition(m_position);

    //Fov set to a fourth of pi is standard (where the fuck did I get that from, radians means pi/4 is 45 degrees which is pisslow. Currently more like 60 fov, way better)
    //Aspect ratio set to window width divided by window height
    //Near- and far-z set to arbitrary standard values of 1 and 1000 (near could be 0.1 instead, we'll see what happens)
    SetLens(0.33f * XM_PI, 800.0f / 600.0f, 1.0f, 175.0f); //Mess around with farZ values, I think 175.0f is good but in different setting another might be more appropriate

    //Smaller fov (4 degrees lol) and clips at a distance of 15 Z (Previously 10)
    SetSelectionLens(0.022f * XM_PI, 800.0f / 600.0f, 1.0f, 15.0f);

    //Bounding Box
    XMVECTOR vMin = XMVectorSet(-1.0f, -1.0f, -1.0f, 0.0f);
    XMVECTOR vMax = XMVectorSet( 1.0f,  1.0f,  1.0f, 0.0f);
    BoundingBox::CreateFromPoints(m_aabb, vMin, vMax);

    //Update position of bounding box (also called any time the camera gets edited, aka m_viewDirty gets set to true and we call UpdateViewMatrix())
    m_aabb.Center = m_position;

    //Bounding Sphere (alternatively I could just have my own struct with a center point and a radius, the library feels like it's been made obsolete by basic maths)
    BoundingSphere::CreateFromBoundingBox(m_sphere, m_aabb); //The CreateFromPoints and CreateFromFrustum are super confusing so I'm just going to do this ehe
    m_sphere.Center = m_position;
    m_sphere.Center.y -= CAMERA_Y_OFFSET;
}

DirectX::XMVECTOR Camera::GetPositionVec() const
{
    return XMLoadFloat3(&m_position);
}

DirectX::XMFLOAT3 Camera::GetPosition() const
{
    return m_position;
}

DirectX::BoundingSphere Camera::GetBoundingSphere() const
{
    return m_sphere;
}

void Camera::SetBoundingSphereCenter(DirectX::XMFLOAT3 pos)
{
    m_sphere.Center = pos;
    m_sphere.Center.y -= CAMERA_Y_OFFSET;
}

DirectX::XMVECTOR Camera::GetRight() const
{
    return XMLoadFloat3(&m_right);
}

DirectX::XMFLOAT3 Camera::GetUp() const
{
    return m_up;
}

//DirectX::XMVECTOR Camera::GetUp() const
//{
//    return XMLoadFloat3(&m_up);
//}

//DirectX::XMVECTOR Camera::GetLook() const
//{
//    return XMLoadFloat3(&m_look);
//}

DirectX::XMFLOAT3 Camera::GetLook() const
{
    return m_look;
}

float Camera::GetNearZ() const
{
    return m_nearZ;
}

float Camera::GetFarZ() const
{
    return m_farZ;
}

float Camera::GetAspect() const
{
    return m_aspect;
}

float Camera::GetFovY() const
{
    return m_fovY;
}

float Camera::GetFovX() const
{
    float half = 0.5f * GetNearWindowWidth();
    return 2.0f * atan(half / m_nearZ);
}

float Camera::GetNearWindowWidth() const
{
    return m_aspect * m_nearWindowHeight;
}

float Camera::GetNearWindowHeight() const
{
    return m_nearWindowHeight;
}

float Camera::GetFarWindowWidth() const
{
    return m_aspect * m_farWindowHeight;
}

float Camera::GetFarWindowHeight() const
{
    return m_farWindowHeight;
}

float Camera::GetPitch() const
{
    return m_pitch;
}

float Camera::GetYaw() const
{
    return m_yaw;
}

DirectX::XMVECTOR Camera::GetDefaultLook() const
{
    return m_defaultLook;
}

XMMATRIX Camera::View() const
{
    return XMLoadFloat4x4(&m_view);
}

XMMATRIX Camera::Proj() const
{
    return XMLoadFloat4x4(&m_proj);
}

void Camera::SetPosition(float x, float y, float z)
{
    m_position = XMFLOAT3(x, y, z);

    m_viewDirty = true;
}

void Camera::SetPosition(const XMFLOAT3& v)
{
    m_position = v;
    m_viewDirty = true;
}

void Camera::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
{
    XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, pos));
    XMVECTOR R = XMVector3Normalize(XMVector3Cross(worldUp, L));
    XMVECTOR U = XMVector3Cross(L, R);

    XMStoreFloat3(&m_position, pos);
    XMStoreFloat3(&m_look, L);
    XMStoreFloat3(&m_right, R);
    XMStoreFloat3(&m_up, U);

    m_viewDirty = true;
}

void Camera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
{
    XMVECTOR P = XMLoadFloat3(&pos);
    XMVECTOR T = XMLoadFloat3(&target);
    XMVECTOR U = XMLoadFloat3(&up);

    LookAt(P, T, U);

    m_viewDirty = true;
}

void Camera::SetLens(float fovY, float aspect, float zn, float zf)
{
    if (fovY != 0.0f) //Safety check
    {
        m_fovY = fovY;
    }
    m_aspect = aspect;

    m_nearZ = zn;
    m_farZ = zf;

    m_nearWindowHeight = 2.0f * m_nearZ * tanf(0.5f * m_fovY);
    m_farWindowHeight = 2.0f * m_farZ * tanf(0.5f * m_fovY);

    DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(m_fovY, m_aspect, m_nearZ, m_farZ);
    DirectX::XMStoreFloat4x4(&m_proj, P);
}

void Camera::SetSelectionLens(float fovY, float aspect, float zn, float zf)
{
    if (fovY != 0.0f) //Safety check
    {
        m_selectionFovY = fovY;
    }
    m_selectionAspect = aspect;

    m_selectionNearZ = zn;
    m_selectionFarZ = zf;

    m_selectionNearWindowHeight = 2.0f * m_nearZ * tanf(0.5f * m_fovY);
    m_selectionFarWindowHeight = 2.0f * m_farZ * tanf(0.5f * m_fovY);

    DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(m_selectionFovY, m_selectionAspect, m_selectionNearZ, m_selectionFarZ);
    DirectX::XMStoreFloat4x4(&m_selectionProj, P);
}

DirectX::XMFLOAT3 Camera::Strafe(float d)
{
    // mPosition += d*mRight
    XMVECTOR s = XMVectorReplicate(d);
    XMVECTOR r = XMLoadFloat3(&m_right);
    XMVECTOR p = XMLoadFloat3(&m_position);
    XMStoreFloat3(&m_position, XMVectorMultiplyAdd(s, r, p));

    m_viewDirty = true;

    //new: return directional vector that represents what way the camera is moving
    XMFLOAT3 toReturn;
    XMStoreFloat3(&toReturn, r);
    return toReturn;
}

void Camera::Walk(float d)
{
    XMVECTOR s = XMVectorReplicate(d);
    XMVECTOR l = XMLoadFloat3(&m_look);
    XMVECTOR p = XMLoadFloat3(&m_position);
    XMStoreFloat3(&m_position, XMVectorMultiplyAdd(s, l, p));

    m_viewDirty = true;
}

DirectX::XMFLOAT3 Camera::PlayerWalk(float d)
{
    //Maybe we should just take the default vector instead and just multiply by the yaw (TOTAL ANGLE, ACCUMULATE VALUE EVERY PITCH AND YAW), ignoring the pitch, and moving forward?
    XMMATRIX temp = XMMatrixRotationY(m_yaw);
    XMVECTOR newLook = XMVector3TransformNormal(m_defaultLook, temp);
    XMVECTOR pos = XMLoadFloat3(&m_position);
    pos += d * newLook;
    XMStoreFloat3(&m_position, pos);

    m_viewDirty = true;

    XMFLOAT3 toReturn;
    XMStoreFloat3(&toReturn, newLook); //do I need to get the actual look-vector? probably not
    return toReturn;
}

void Camera::Pitch(float angle)
{
    m_pitch += angle;
    // Rotate up and look vector about the right vector.
    //(This is for rotating up and down)
    XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_right), angle);

    XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), R));
    XMStoreFloat3(&m_look, XMVector3TransformNormal(XMLoadFloat3(&m_look), R));

    m_viewDirty = true;
}

void Camera::PitchFPC(float angle)
{
    // Rotate up and look vector about the right vector (Rotate up and down)
    XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_right), angle);

    //Clamp????? (god i hate xmvectors and float3s)
    XMVECTOR tempVec = XMVector3TransformNormal(XMLoadFloat3(&m_look), R);
    XMFLOAT3 tempF3;
    XMStoreFloat3(&tempF3, tempVec);
    if (tempF3.y >= 0.95f && tempF3.y >= m_look.y)
        return;
    else if (tempF3.y <= -0.95f && tempF3.y <= m_look.y)
        return;
    tempVec = XMLoadFloat3(&tempF3);

    XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), R));
    //XMStoreFloat3(&m_look, XMVector3TransformNormal(XMLoadFloat3(&m_look), R));
    XMStoreFloat3(&m_look, tempVec);

    m_pitch += angle; //Only increment the pitch if we actually did pitch (aka after the clamping up above)

    m_viewDirty = true;
}

void Camera::RotateY(float angle)
{
    m_yaw += angle;
    // Rotate the basis vectors about the world y-axis (Rotate left and right)

    XMMATRIX R = XMMatrixRotationY(angle);

    XMStoreFloat3(&m_right, XMVector3TransformNormal(XMLoadFloat3(&m_right), R));
    XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), R));
    XMStoreFloat3(&m_look, XMVector3TransformNormal(XMLoadFloat3(&m_look), R));

    m_viewDirty = true;
}

void Camera::UpdateViewMatrix()
{
    if (m_viewDirty)
    {
        //
        XMVECTOR R = XMLoadFloat3(&m_right);
        XMVECTOR U = XMLoadFloat3(&m_up);
        XMVECTOR L = XMLoadFloat3(&m_look);
        XMVECTOR P = XMLoadFloat3(&m_position);

        // Keep camera's axes orthogonal to each other and of unit length.
        L = XMVector3Normalize(L);
        U = XMVector3Normalize(XMVector3Cross(L, R));

        // U, L already ortho-normal, so no need to normalize cross product.
        R = XMVector3Cross(U, L);

        // Fill in the view matrix entries.
        float x = -XMVectorGetX(XMVector3Dot(P, R));
        float y = -XMVectorGetX(XMVector3Dot(P, U));
        float z = -XMVectorGetX(XMVector3Dot(P, L));

        XMStoreFloat3(&m_right, R);
        XMStoreFloat3(&m_up, U);
        XMStoreFloat3(&m_look, L);

        //Alternatively, this part can be done using an initializer-list
        m_view(0, 0) = m_right.x;
        m_view(1, 0) = m_right.y;
        m_view(2, 0) = m_right.z;
        m_view(3, 0) = x;

        m_view(0, 1) = m_up.x;
        m_view(1, 1) = m_up.y;
        m_view(2, 1) = m_up.z;
        m_view(3, 1) = y;
        
        m_view(0, 2) = m_look.x;
        m_view(1, 2) = m_look.y;
        m_view(2, 2) = m_look.z;
        m_view(3, 2) = z;

        m_view(0, 3) = 0.0f;
        m_view(1, 3) = 0.0f;
        m_view(2, 3) = 0.0f;
        m_view(3, 3) = 1.0f;

        //Recreate frustum
        DirectX::BoundingFrustum::CreateFromMatrix(m_frustum, Proj());
        DirectX::XMMATRIX invView = DirectX::XMMatrixInverse(NULL, View());
        m_frustum.Transform(m_frustum, invView);

        //Recreate selection frustum
        DirectX::BoundingFrustum::CreateFromMatrix(m_selectionFrustum, XMLoadFloat4x4(&m_selectionProj));
        m_selectionFrustum.Transform(m_selectionFrustum, invView);

        //Update position of bounding sphere
        //m_aabb.Center = m_position;
        m_sphere.Center = m_position;
        m_sphere.Center.y -= CAMERA_Y_OFFSET;

        m_viewDirty = false;
    }
}

void Camera::UpdateConstantBuffer(ID3D11DeviceContext* context)
{
    if (m_currentFrustum == 0)
    {
        DirectX::XMStoreFloat4x4(&m_cameraData.view, XMMatrixTranspose(View()));
        DirectX::XMStoreFloat4x4(&m_cameraData.proj, XMMatrixTranspose(Proj()));
        DirectX::XMStoreFloat3(&m_cameraData.pos, GetPositionVec());
    }
    else
    {
        DirectX::XMStoreFloat4x4(&m_cameraData.view, XMMatrixTranspose(View()));
        DirectX::XMStoreFloat4x4(&m_cameraData.proj, XMMatrixTranspose(XMLoadFloat4x4(&m_selectionProj)));
        DirectX::XMStoreFloat3(&m_cameraData.pos, GetPositionVec());
    }

    D3D11_MAPPED_SUBRESOURCE mapped = {};						                    //Set up the new data for the resource, zero the memory
    context->Map(m_cameraCB.GetBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);   //Disable GPU access to the data we want to change, and get a pointer to the memory containing said data
    memcpy(mapped.pData, &m_cameraData, sizeof(m_cameraData));						//Write the new data to memory
    context->Unmap(m_cameraCB.GetBuffer(), 0);										//Re-enable GPU access to the data
}

const ConstantBuffer& Camera::GetConstantBuffer() const
{
    return m_cameraCB;
}

const DirectX::BoundingFrustum& Camera::GetFrustum() const
{
    return m_frustum;
}

const DirectX::BoundingFrustum& Camera::GetSelectionFrustum() const
{
    return m_selectionFrustum;
}

void Camera::InitConstantBufferAndFrustum(ID3D11Device* device)
{
    XMStoreFloat4x4(&m_cameraData.view, XMMatrixTranspose(View()));
    XMStoreFloat4x4(&m_cameraData.proj, XMMatrixTranspose(Proj()));
    DirectX::XMStoreFloat3(&m_cameraData.pos, GetPositionVec());
    m_cameraCB.Init(device, &m_cameraData, sizeof(m_cameraData));
}

void Camera::ChangeFrustum()
{
    if (m_currentFrustum == 0) //Regular, change to selection
    {
        m_currentFrustum = 1;
    }
    else //Selection, change to regular
    {
        m_currentFrustum = 0;
    }
}
