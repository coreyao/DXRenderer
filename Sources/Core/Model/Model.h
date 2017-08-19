#pragma once

#include "Utils/Util.h"
#include "Core/Material/Material.h"

class Camera;
class Model;

struct VertexData
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 textCoord;
};

class Mesh
{
	friend class Model;
public:
	Mesh();
	~Mesh();

	void Render(Camera* pCamera);
	void LoadMeshData(const std::vector<VertexData>& vertexData, const std::string& diffuseTexName);

	LPDIRECT3DVERTEXBUFFER9	m_pVB;
	std::vector<VertexData> m_vertexData;
	Material m_mat;
};

class Model
{
public:
	Model(const std::string& fileName);
	~Model();

	void Render(Camera* pCamera);

private:
	void LoadObjFile(const std::string& fileName);

	std::vector<Mesh*> m_vMesh;
	LPDIRECT3DVERTEXDECLARATION9 m_pVertexDeclaration;
};