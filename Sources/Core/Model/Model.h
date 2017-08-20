#pragma once

#include "Utils/Util.h"
#include "Core/Material/Material.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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
	void SetupHWResource();

	std::vector<VertexData> m_vertexData;
	std::vector<unsigned int> m_indices;
	Material m_mat;

private:
	LPDIRECT3DVERTEXBUFFER9	m_pVB;
	LPDIRECT3DINDEXBUFFER9 m_pIB;
};

class Model
{
public:
	Model(const std::string& fileName);
	~Model();

	void Render(Camera* pCamera);

	std::vector<Mesh*> m_vMesh;
	std::string m_sDirectory;

private:
	void LoadFromFile(const std::string& fileName);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh* processMesh(aiMesh *mesh, const aiScene *scene);
	std::string loadMaterialTexture(aiMaterial *mat, aiTextureType type);

private:
	LPDIRECT3DVERTEXDECLARATION9 m_pVertexDeclaration;
};