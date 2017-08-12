#pragma once

#include "Utils/Util.h"
#include "Core/Material/Material.h"

class Camera;
class Mesh
{
public:
	Mesh();
	~Mesh();

	void Render(Camera* pCamera);

	LPDIRECT3DVERTEXBUFFER9	m_pVB;
	LPDIRECT3DINDEXBUFFER9 m_pIB;

	Material m_mat;
};


class Model
{
public:
	Model(const std::string& fileName);

	void Render(Camera* pCamera);

	std::vector<Mesh*> m_vMesh;
	LPDIRECT3DVERTEXDECLARATION9 m_pVertexDeclaration;
};