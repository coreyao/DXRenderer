#include "Core/Material/Material.h"
#include "Core/Application.h"

Material::Material()
	: m_diffuseTex(NULL)
{
}

void Material::CreateVertexShader(const std::string& vertexShaderFile, const std::string& mainFunctionName)
{
	LPD3DXBUFFER pCode = NULL;
	LPD3DXBUFFER pError = NULL;

	D3DXCompileShaderFromFileA(vertexShaderFile.c_str(), NULL, NULL, mainFunctionName.c_str(),
		D3DXGetVertexShaderProfile(Application::m_pd3dDevice), 0, &pCode,
		&pError, &m_pConstantTableVertex);
	if (pError != NULL)
	{
		::MessageBoxA(0, (LPCSTR)pError->GetBufferPointer(), 0, 0);
	}

	HRESULT hr = Application::m_pd3dDevice->CreateVertexShader((DWORD*)pCode->GetBufferPointer(), &m_pVertexShader);
	pCode->Release();
}

void Material::CreatePixelShader(const std::string& pixelShaderFile, const std::string& mainFunctionName)
{
	LPD3DXBUFFER pCode = NULL;
	LPD3DXBUFFER pError = NULL;

	D3DXCompileShaderFromFileA(pixelShaderFile.c_str(), NULL, NULL, "PSMain",
		D3DXGetPixelShaderProfile(Application::m_pd3dDevice), 0, &pCode,
		&pError, &m_pConstantTablePixel);
	if (pError != NULL)
	{
		::MessageBoxA(0, (LPCSTR)pError->GetBufferPointer(), 0, 0);
	}

	HRESULT hr = Application::m_pd3dDevice->CreatePixelShader((DWORD*)pCode->GetBufferPointer(), &m_pPixelShader);
	pCode->Release();
}

void Material::SetVertexShaderUniform(const std::string& uniformName, float val)
{
	if (m_pVertexShader && m_pConstantTableVertex)
	{
		m_pConstantTableVertex->SetFloat(Application::m_pd3dDevice, uniformName.c_str(), val);
	}
}

void Material::SetVertexShaderUniform(const std::string& uniformName, D3DXMATRIX val)
{
	if (m_pVertexShader && m_pConstantTableVertex)
	{
		m_pConstantTableVertex->SetMatrix(Application::m_pd3dDevice, uniformName.c_str(), &val);
	}
}

void Material::SetPixelShaderUniform(const std::string& uniformName, float val)
{
	if (m_pPixelShader && m_pConstantTablePixel)
	{
		m_pConstantTablePixel->SetFloat(Application::m_pd3dDevice, uniformName.c_str(), val);
	}
}

void Material::SetDiffuseTexture(const std::string& diffuseTex)
{
	if (m_diffuseTex)
		delete m_diffuseTex;
	m_diffuseTex = Texture::GetTexture(diffuseTex);

	UINT count;
	D3DXHANDLE Samp0Handle = 0;
	Samp0Handle = m_pConstantTablePixel->GetConstantByName(0, "MeshTextureSampler");
	m_pConstantTablePixel->GetConstantDesc(Samp0Handle, &DiffuseSampDesc, &count);
}

