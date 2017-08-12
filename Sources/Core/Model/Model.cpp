#include "Core/Model/Model.h"
#include "Core/Application.h"
#include "Core/Camera.h"

Mesh::Mesh()
{
	Application::m_pd3dDevice->CreateVertexBuffer(3 * sizeof(D3DXVECTOR3), 0, 0, D3DPOOL_DEFAULT, &m_pVB, NULL);
	D3DXVECTOR3* pVertices;
	m_pVB->Lock(0, 0, (void**)&pVertices, 0);
	pVertices[0] = D3DXVECTOR3(0, 1.0f, 2.0f);
	pVertices[1] = D3DXVECTOR3(1.0f, -1.0f, 2.0f);
	pVertices[2] = D3DXVECTOR3(-1.0f, -1.0f, 2.0f);
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

	Application::m_pd3dDevice->SetVertexShader(m_mat.m_pVertexShader);
	Application::m_pd3dDevice->SetPixelShader(m_mat.m_pPixelShader);
	Application::m_pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(D3DXVECTOR3));
	Application::m_pd3dDevice->SetIndices(m_pIB);

	Application::m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 3, 0, 1);
}

Model::Model(const std::string& fileName)
{
	if (fileName.empty())
		return;

	D3DVERTEXELEMENT9 decl[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		D3DDECL_END()
	};
	Application::m_pd3dDevice->CreateVertexDeclaration(decl, &m_pVertexDeclaration);

	m_vMesh.push_back(new Mesh());
}

void Model::Render(Camera* pCamera)
{
	Application::m_pd3dDevice->SetVertexDeclaration(m_pVertexDeclaration);
	for (int i = 0; i < m_vMesh.size(); ++i)
	{
		m_vMesh[i]->Render(pCamera);
	}
}
