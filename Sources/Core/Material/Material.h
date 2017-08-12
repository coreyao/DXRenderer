#pragma once 

#include "Utils/Util.h"
#include "Core/Texture/Texture.h"

class Material
{
public:
	Material();

	void CreateVertexShader(const std::string& vertexShaderFile, const std::string& mainFunctionName);
	void CreatePixelShader(const std::string& pixelShaderFile, const std::string& mainFunctionName);

	void SetVertexShaderUniform(const std::string& uniformName, float val);
	void SetVertexShaderUniform(const std::string& uniformName, D3DXMATRIX val);

	void SetPixelShaderUniform(const std::string& uniformName, float val);


	Texture* m_diffuseTex;

	LPDIRECT3DVERTEXSHADER9 m_pVertexShader;
	LPD3DXCONSTANTTABLE     m_pConstantTableVertex;
	LPDIRECT3DPIXELSHADER9	m_pPixelShader;
	LPD3DXCONSTANTTABLE     m_pConstantTablePixel;
};