#pragma once

#include "Utils/Util.h"

class Transform
{
public:
	Transform();
	virtual ~Transform();

	void SetPosition(const D3DXVECTOR3& position);
	const D3DXVECTOR3& GetPosition();

	void SetEulerRotation(const D3DXVECTOR3& euler);
	const D3DXVECTOR3& GetEulerRotation();

	void SetScale(const D3DXVECTOR3& scale);
	const D3DXVECTOR3& GetScale();

	const D3DXMATRIX& GetLocalToParentMatrix();

protected:
	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_eulerRotation;
	D3DXVECTOR3 m_scale;
	D3DXMATRIX m_localToParent;

	bool m_bDirty;
};