#include "Core/Model/Model.h"
#include "Core/Application.h"
#include "Core/Camera.h"

Mesh::Mesh()
{
	m_mat.CreateVertexShader("../Sources/Shaders/simple_unlit.hlsl", "VSMain");
	m_mat.CreatePixelShader("../Sources/Shaders/simple_unlit.hlsl", "PSMain");
}

Mesh::~Mesh()
{
	if (m_pVB != NULL)
		m_pVB->Release();

	if (m_pIB != NULL)
		m_pIB->Release();
}

void Mesh::Render(Camera* pCamera)
{
	D3DXMATRIX m = pCamera->GetViewMatrix() * pCamera->GetProjectionMatrix();

	D3DXMATRIX mViewProj;
	memcpy((void*)&mViewProj, (void*)&m, sizeof(D3DXMATRIX));
	m_mat.m_pConstantTableVertex->SetMatrix(Application::m_pd3dDevice, "mViewProj", &mViewProj);
	m_mat.m_pConstantTablePixel->SetFloat(Application::m_pd3dDevice, "fIntensity", 1.0f);

	if (m_mat.m_diffuseTex)
		Application::m_pd3dDevice->SetTexture(m_mat.DiffuseSampDesc.RegisterIndex, m_mat.m_diffuseTex->m_pDeviceTexture);

	Application::m_pd3dDevice->SetVertexShader(m_mat.m_pVertexShader);
	Application::m_pd3dDevice->SetPixelShader(m_mat.m_pPixelShader);
	Application::m_pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(VertexData));
	Application::m_pd3dDevice->SetIndices(m_pIB);
	//Application::m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	Application::m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);    // turn on the z-buffer
	Application::m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_vertexData.size(), 0, m_indices.size() / 3);
}

void Mesh::SetupHWResource()
{
	Application::m_pd3dDevice->CreateVertexBuffer(m_vertexData.size() * sizeof(VertexData), 0, 0, D3DPOOL_DEFAULT, &m_pVB, NULL);
	VertexData* pVertices = NULL;
	m_pVB->Lock(0, 0, (void**)&pVertices, 0);
	memcpy(pVertices, &m_vertexData[0], m_vertexData.size() * sizeof(VertexData));
	m_pVB->Unlock();

	Application::m_pd3dDevice->CreateIndexBuffer(m_indices.size() * sizeof(unsigned int), 0, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &m_pIB, NULL);
	unsigned int* pIndices = NULL;
	m_pIB->Lock(0, 0, (void**)&pIndices, 0);
	memcpy(pIndices, &m_indices[0], m_indices.size() * sizeof(unsigned int));
	m_pIB->Unlock();
}

Model::Model(const std::string& fileName)
{
	if (fileName.empty())
		return;

	LoadFromFile(fileName);

	D3DVERTEXELEMENT9 decl[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};
	Application::m_pd3dDevice->CreateVertexDeclaration(decl, &m_pVertexDeclaration);

}

Model::~Model()
{
	for (unsigned int i = 0; i < m_vMesh.size(); ++i)
	{
		delete m_vMesh[i];
	}

	m_vMesh.clear();
}

void Model::LoadFromFile(const std::string& path)
{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	m_sDirectory = path.substr(0, path.find_last_of('/') + 1);

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		m_vMesh.push_back(processMesh(mesh, scene));
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh* Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
	Mesh* pNewMesh = new Mesh;

	std::vector<VertexData> vertices;
	std::vector<unsigned int> indices;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		VertexData vertex;
		// process vertex positions, normals and texture coordinates
		D3DXVECTOR3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.pos = vector;

		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.normal = vector;

		if (mesh->mTextureCoords[0])
		{
			D3DXVECTOR2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.textCoord = vec;
		}
		else
		{
			vertex.textCoord = D3DXVECTOR2(0.0f, 0.0f);
		}
		
		vertices.push_back(vertex);
	}

	// process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	pNewMesh->m_vertexData = vertices;
	pNewMesh->m_indices = indices;

	// process material
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
		const std::string& diffuseName = loadMaterialTexture(material, aiTextureType_DIFFUSE);
		if (!diffuseName.empty())
			pNewMesh->m_mat.SetDiffuseTexture(m_sDirectory + diffuseName);
		//loadMaterialTexture(material, aiTextureType_SPECULAR);
	}

	pNewMesh->SetupHWResource();

	return pNewMesh;
}

std::string Model::loadMaterialTexture(aiMaterial *mat, aiTextureType type)
{
	unsigned int iCount = mat->GetTextureCount(type);
	if (iCount > 0)
	{
		aiString str;
		mat->GetTexture(type, 0, &str);
		return str.C_Str();
	}

	return "";
}

void Model::Render(Camera* pCamera)
{
	Application::m_pd3dDevice->SetVertexDeclaration(m_pVertexDeclaration);
	for (unsigned int i = 0; i < m_vMesh.size(); ++i)
	{
		m_vMesh[i]->Render(pCamera);
	}
}
