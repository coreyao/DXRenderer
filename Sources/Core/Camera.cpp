#include "Camera.h"

#include "Utils/Util.h"

Camera::Camera()
	: m_fFOV(90.0f)
	, m_fNear(1.0f)
	, m_fFar(3000.0f)
	, m_fAspectRatio(16.0f / 9.0f)
{
}

Transform& Camera::GetTransform()
{
	return m_transform;
}

D3DXMATRIX Camera::GetViewMatrix()
{
	D3DXMATRIX ret;
	D3DXVECTOR3 cameraPos = m_transform.GetPosition();

	D3DXVECTOR3 lookat = D3DXVECTOR3(0, 0, 1);
	D3DXVec3TransformNormal(&lookat, &lookat, &m_transform.GetLocalToParentMatrix());

	D3DXVECTOR3 lookAtPos = cameraPos + lookat;
	D3DXVECTOR3 upDir = D3DXVECTOR3(0, 1, 0);

	return *D3DXMatrixLookAtLH(&ret, &cameraPos, &lookAtPos, &upDir);
}

D3DXMATRIX Camera::GetProjectionMatrix()
{
	D3DXMATRIX ret;
	return *D3DXMatrixPerspectiveFovLH(&ret, DEGREES_TO_RADIANS(m_fFOV), m_fAspectRatio, m_fNear, m_fFar);
}
