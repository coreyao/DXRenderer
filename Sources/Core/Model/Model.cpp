#include "Core/Model/Model.h"
#include "Core/Application.h"
#include "Core/Camera.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

struct SVertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 color;
	D3DXVECTOR2 textCoord;
};

Mesh::Mesh()
{
	Application::m_pd3dDevice->CreateVertexBuffer(3 * sizeof(SVertex), 0, 0, D3DPOOL_DEFAULT, &m_pVB, NULL);
	SVertex* pVertices;
	m_pVB->Lock(0, 0, (void**)&pVertices, 0);
	pVertices[0].pos = D3DXVECTOR3(0, 1.0f, 2.0f);
	pVertices[0].color = D3DXVECTOR3(1.0f, 0.0, 0.0);
	pVertices[0].textCoord = D3DXVECTOR2(0.5f, 0);

	pVertices[1].pos = D3DXVECTOR3(1.0f, -1.0f, 2.0f);
	pVertices[1].color = D3DXVECTOR3(0.0, 1.0f, 0.0);
	pVertices[1].textCoord = D3DXVECTOR2(1.0f, 1.0f);

	pVertices[2].pos = D3DXVECTOR3(-1.0f, -1.0f, 2.0f);
	pVertices[2].color = D3DXVECTOR3(0.0, 0.0, 1.0f);
	pVertices[2].textCoord = D3DXVECTOR2(0.0f, 1.0f);
	m_pVB->Unlock();

	WORD* pIndices;
	Application::m_pd3dDevice->CreateIndexBuffer(3 * sizeof(WORD), 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB, NULL);
	m_pIB->Lock(0, 0, (void**)&pIndices, 0);
	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	m_pIB->Unlock();

	m_mat.CreateVertexShader("../Sources/Shaders/simple_unlit.hlsl", "VSMain");
	m_mat.CreatePixelShader("../Sources/Shaders/simple_unlit.hlsl", "PSMain");
	m_mat.SetDiffuseTexture("../Resources/sponza_crytek_max_obj/textures/lion.tga");
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
	Application::m_pd3dDevice->SetTexture(m_mat.DiffuseSampDesc.RegisterIndex, m_mat.m_diffuseTex->m_pDeviceTexture);

	Application::m_pd3dDevice->SetVertexShader(m_mat.m_pVertexShader);
	Application::m_pd3dDevice->SetPixelShader(m_mat.m_pPixelShader);
	Application::m_pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(SVertex));
	Application::m_pd3dDevice->SetIndices(m_pIB);
	//Application::m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	Application::m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 3, 0, 1);
}

Model::Model(const std::string& fileName)
{
	if (fileName.empty())
		return;

	LoadObjFile(fileName);

	D3DVERTEXELEMENT9 decl[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};
	Application::m_pd3dDevice->CreateVertexDeclaration(decl, &m_pVertexDeclaration);

	m_vMesh.push_back(new Mesh());
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

	// For each shape
	for (size_t i = 0; i < shapes.size(); i++)
	{
		printf("shape[%ld].name = %s\n", static_cast<long>(i), shapes[i].name.c_str());
		printf("Size of shape[%ld].indices: %lu\n", static_cast<long>(i), static_cast<unsigned long>(shapes[i].mesh.indices.size()));

		size_t index_offset = 0;

		assert(shapes[i].mesh.num_face_vertices.size() == shapes[i].mesh.material_ids.size());

		printf("shape[%ld].num_faces: %lu\n", static_cast<long>(i), static_cast<unsigned long>(shapes[i].mesh.num_face_vertices.size()));

		// For each face
		for (size_t f = 0; f < shapes[i].mesh.num_face_vertices.size(); f++) 
		{
			size_t fnum = shapes[i].mesh.num_face_vertices[f];

			printf("  face[%ld].fnum = %ld\n", static_cast<long>(f), static_cast<unsigned long>(fnum));

			// For each vertex in the face
			for (size_t v = 0; v < fnum; v++) 
			{
				tinyobj::index_t idx = shapes[i].mesh.indices[index_offset + v];
				printf("    face[%ld].v[%ld].idx = %d/%d/%d\n", static_cast<long>(f), static_cast<long>(v), idx.vertex_index, idx.normal_index, idx.texcoord_index);
			}

			printf("  face[%ld].material_id = %d\n", static_cast<long>(f), shapes[i].mesh.material_ids[f]);

			index_offset += fnum;
		}
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
