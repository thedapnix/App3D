#include "Camera.h"

using namespace DirectX;

Camera::Camera()
{
    SetPosition(m_position);

    //Fov set to a fourth of pi is standard
    //Aspect ratio set to window width divided by window height
    //Near- and far-z set to arbitrary standard values of 1 and 1000 (near could be 0.1 instead, we'll see what happens)
    SetLens(0.25f * XM_PI, 800.0f / 600.0f, 1.0f, 1000.0f);
}

DirectX::XMVECTOR Camera::GetPositionVec() const
{
    return XMLoadFloat3(&m_position);
}

DirectX::XMFLOAT3 Camera::GetPosition() const
{
    return m_position;
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
    if (fovY != 0.0f) //Workaround
    {
        m_fovY = fovY;
    }
    //m_fovY = fovY;
    m_aspect = aspect;

    m_nearZ = zn;
    m_farZ = zf;

    m_nearWindowHeight = 2.0f * m_nearZ * tanf(0.5f * m_fovY);
    m_farWindowHeight = 2.0f * m_farZ * tanf(0.5f * m_fovY);

    DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(m_fovY, m_aspect, m_nearZ, m_farZ);
    DirectX::XMStoreFloat4x4(&m_proj, P);
}

void Camera::Strafe(float d)
{
    // mPosition += d*mRight
    XMVECTOR s = XMVectorReplicate(d);
    XMVECTOR r = XMLoadFloat3(&m_right);
    XMVECTOR p = XMLoadFloat3(&m_position);
    XMStoreFloat3(&m_position, XMVectorMultiplyAdd(s, r, p));

    m_viewDirty = true;
}

void Camera::Walk(float d)
{
    // mPosition += d*mLook
    XMVECTOR s = XMVectorReplicate(d);
    XMVECTOR l = XMLoadFloat3(&m_look);
    XMVECTOR p = XMLoadFloat3(&m_position);
    XMStoreFloat3(&m_position, XMVectorMultiplyAdd(s, l, p));

    m_viewDirty = true;
}

void Camera::Pitch(float angle)
{
    // Rotate up and look vector about the right vector.

    XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_right), angle);

    XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), R));
    XMStoreFloat3(&m_look, XMVector3TransformNormal(XMLoadFloat3(&m_look), R));

    m_viewDirty = true;
}

void Camera::RotateY(float angle)
{
    // Rotate the basis vectors about the world y-axis.

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

        m_viewDirty = false;

        //Recreate the bounding frustum
        DirectX::BoundingFrustum::CreateFromMatrix(m_frustum, Proj());
        DirectX::XMMATRIX invView = DirectX::XMMatrixInverse(NULL, View());
        m_frustum.Transform(m_frustum, invView);
    }
}

void Camera::UpdateConstantBuffer(ID3D11DeviceContext* context)
{
    DirectX::XMStoreFloat4x4(&m_cameraData.view, XMMatrixTranspose(View()));
    DirectX::XMStoreFloat4x4(&m_cameraData.proj, XMMatrixTranspose(Proj()));
    DirectX::XMStoreFloat3(&m_cameraData.pos, GetPositionVec());

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

void Camera::InitConstantBufferAndFrustum(ID3D11Device* device)
{
    XMStoreFloat4x4(&m_cameraData.view, XMMatrixTranspose(View()));
    XMStoreFloat4x4(&m_cameraData.proj, XMMatrixTranspose(Proj()));
    DirectX::XMStoreFloat3(&m_cameraData.pos, GetPositionVec());
    m_cameraCB.Init(device, &m_cameraData, sizeof(m_cameraData));

    DirectX::BoundingFrustum::CreateFromMatrix(m_frustum, Proj());
    DirectX::XMMATRIX invView = DirectX::XMMatrixInverse(NULL, View());
    m_frustum.Transform(m_frustum, invView);
}
