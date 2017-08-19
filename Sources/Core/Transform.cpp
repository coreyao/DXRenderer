#include "Transform.h"

Transform::Transform()
{
	memset(&m_position, 0, sizeof(m_position));
	m_scale = D3DXVECTOR3(1, 1, 1);
	memset(&m_eulerRotation, 0, sizeof(m_eulerRotation));
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
		D3DXMATRIX tmp;
		D3DXMatrixIdentity(&m);
		m *= *D3DXMatrixScaling(&tmp, m_scale.x, m_scale.y, m_scale.z);
 		m *= *D3DXMatrixRotationX(&tmp, DEGREES_TO_RADIANS(m_eulerRotation.x));
		m *= *D3DXMatrixRotationY(&tmp, DEGREES_TO_RADIANS(m_eulerRotation.y));
		m *= *D3DXMatrixRotationZ(&tmp, DEGREES_TO_RADIANS(m_eulerRotation.z));
		m *= *D3DXMatrixTranslation(&tmp, m_position.x, m_position.y, m_position.z);

		m_localToParent = m;
	}

	return m_localToParent;
}
