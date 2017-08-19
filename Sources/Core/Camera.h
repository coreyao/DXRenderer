#pragma once

#include "Transform.h"

class Camera
{
public:
	Camera();

	Transform& GetTransform();

	D3DXMATRIX GetViewMatrix();
	D3DXMATRIX GetProjectionMatrix();

private:
	Transform m_transform;
	float m_fFOV;
	float m_fAspectRatio;
	float m_fNear;
	float m_fFar;
};