#include "Core/Model/Model.h"
#include "Core/Application.h"
#include "Core/Camera.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

Mesh::Mesh()
{
	m_mat.CreateVertexShader("../Sources/Shaders/simple_unlit.hlsl", "VSMain");
	m_mat.CreatePixelShader("../Sources/Shaders/simple_unlit.hlsl", "PSMain");
}

Mesh::~Mesh()
{
	if (m_pVB != NULL)
		m_pVB->Release();
}

void Mesh::Render(Camera* pCamera)
{
	D3DXMATRIX m = pCamera->GetViewMatrix() * pCamera->GetProjectionMatrix();

	D3DXMATRIX mViewProj;
	memcpy((void*)&mViewProj, (void*)&m, sizeof(D3DXMATRIX));
	m_mat.m_pConstantTableVertex->SetMatrix(Application::m_pd3dDevice, "mViewProj", &mViewProj);
	m_mat.m_pConstantTablePixel->SetFloat(Application::m_pd3dDevice, "fIntensity", 1.0f);
	//Application::m_pd3dDevice->SetTexture(m_mat.DiffuseSampDesc.RegisterIndex, m_mat.m_diffuseTex->m_pDeviceTexture);

	Application::m_pd3dDevice->SetVertexShader(m_mat.m_pVertexShader);
	Application::m_pd3dDevice->SetPixelShader(m_mat.m_pPixelShader);
	Application::m_pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(VertexData));
	//Application::m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	Application::m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_vertexData.size() / 3);
}

void Mesh::LoadMeshData(const std::vector<VertexData>& vertexData, const std::string& diffuseTexName)
{
	Application::m_pd3dDevice->CreateVertexBuffer(vertexData.size() * sizeof(VertexData), 0, 0, D3DPOOL_DEFAULT, &m_pVB, NULL);
	VertexData* pVertices = NULL;
	m_pVB->Lock(0, 0, (void**)&pVertices, 0);
	memcpy(pVertices, &vertexData[0], vertexData.size() * sizeof(VertexData));
	m_pVB->Unlock();

	m_vertexData = vertexData;
	m_mat.SetDiffuseTexture(diffuseTexName);
}

Model::Model(const std::string& fileName)
{
	if (fileName.empty())
		return;

	LoadObjFile(fileName);

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
	for (int i = 0; i < m_vMesh.size(); ++i)
	{
		delete m_vMesh[i];
	}

	m_vMesh.clear();
}

void Model::LoadObjFile(const std::string& fileName)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	int index = fileName.find_last_of('/');
	std::string basePath = fileName.substr( 0, index + 1);

	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, fileName.c_str(), basePath.c_str(), true);
	if (!err.empty())
	{
		std::cout << err << std::endl;
	}
	if (!ret) 
	{
		printf("Failed to load/parse %s.\n", fileName.c_str());
		return;
	}

	std::vector<VertexData> pVertexData;
	for (size_t i = 0; i < shapes.size(); i++)
	{
		const tinyobj::shape_t& shapeData = shapes[i];
		size_t index_offset = 0;
		Mesh* m = new Mesh();
		for (size_t f = 0; f < shapeData.mesh.num_face_vertices.size(); f++)
		{
			size_t fnum = shapeData.mesh.num_face_vertices[f];
			for (size_t v = 0; v < fnum; v++)
			{
				tinyobj::index_t idx = shapes[i].mesh.indices[index_offset + v];
				VertexData vData;

				vData.pos.x = attrib.vertices[3 * idx.vertex_index + 0];
				vData.pos.y = attrib.vertices[3 * idx.vertex_index + 1];
				vData.pos.z = attrib.vertices[3 * idx.vertex_index + 2];

				vData.normal.x = attrib.normals[3 * idx.normal_index + 0];
				vData.normal.y = attrib.normals[3 * idx.normal_index + 1];
				vData.normal.z = attrib.normals[3 * idx.normal_index + 2];

				vData.textCoord.x = attrib.texcoords[2 * idx.texcoord_index + 0];
				vData.textCoord.y = attrib.texcoords[2 * idx.texcoord_index + 1];

				pVertexData.push_back(vData);
			}

			index_offset += fnum;
		}

		m->LoadMeshData(pVertexData, basePath + materials[shapeData.mesh.material_ids[0]].diffuse_texname);
		m_vMesh.push_back(m);

		pVertexData.clear();
	}
}

void Model::Render(Camera* pCamera)
{
	Application::m_pd3dDevice->SetVertexDeclaration(m_pVertexDeclaration);
	for (int i = 0; i < m_vMesh.size(); ++i)
	{
		m_vMesh[i]->Render(pCamera);
	}
}
