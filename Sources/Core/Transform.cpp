#include "Transform.h"

Transform::Transform()
{
	D3DXMatrixIdentity(&m_localToParent);
}

Transform::~Transform()
{
}

void Transform::SetPosition(const D3DXVECTOR3& position)
{
	if (m_position != position)
	{
		m_position = position;
		m_bDirty = true;
	}
}

const D3DXVECTOR3& Transform::GetPosition()
{
	return m_position;
}

void Transform::SetEulerRotation(const D3DXVECTOR3& euler)
{
	if (m_eulerRotation != euler)
	{
		m_eulerRotation = euler;
		m_bDirty = true;
	}
}

const D3DXVECTOR3& Transform::GetEulerRotation()
{
	return m_eulerRotation;
}

void Transform::SetScale(const D3DXVECTOR3& scale)
{
	if (m_scale != scale)
	{
		m_scale = scale;
		m_bDirty = true;
	}
}

const D3DXVECTOR3& Transform::GetScale()
{
	return m_scale;
}

const D3DXMATRIX& Transform::GetLocalToParentMatrix()
{
	if (m_bDirty)
	{
		D3DXMATRIX m;
		D3DXMatrixIdentity(&m);
		m *= *D3DXMatrixScaling(&m, m_scale.x, m_scale.y, m_scale.z);
		m *= *D3DXMatrixRotationX(&m, DEGREES_TO_RADIANS(m_eulerRotation.x));
		m *= *D3DXMatrixRotationY(&m, DEGREES_TO_RADIANS(m_eulerRotation.y));
		m *= *D3DXMatrixRotationZ(&m, DEGREES_TO_RADIANS(m_eulerRotation.z));
		m *= *D3DXMatrixTranslation(&m, m_position.x, m_position.y, m_position.z);

		m_localToParent = m;
	}

	return m_localToParent;
}
