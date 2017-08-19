#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Core/Application.h"

Texture::Texture(const std::string& str, TextureFormat format)
	: m_path(str)
	, m_width(0)
	, m_height(0)
	, m_format(format)
	, m_pDeviceTexture(NULL)
{
}

Texture* Texture::GetTexture(const std::string& str)
{
	if (m_vLoadedTextures.find(str) == m_vLoadedTextures.end())
	{
		Texture* pTex = new Texture(str);
		pTex->LoadData();
		m_vLoadedTextures[str] = pTex;
	}

	return m_vLoadedTextures[str];
}

std::map<std::string, Texture*> Texture::m_vLoadedTextures;

void Texture::LoadData()
{
	int width, height, nrChannels;
	unsigned char *data = stbi_load(m_path.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		HRESULT hr = Application::m_pd3dDevice->CreateTexture(width, height, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pDeviceTexture, NULL);
		if (FAILED(hr)) 
		{
			::MessageBoxA(0, (LPCSTR)DXGetErrorDescriptionA(hr), (LPCSTR)DXGetErrorStringA(hr), 0);
			return;
		}

		D3DLOCKED_RECT pLockedRect;
		hr = m_pDeviceTexture->LockRect(0, &pLockedRect, NULL, 0);
		if (FAILED(hr))
		{
			::MessageBoxA(0, (LPCSTR)DXGetErrorDescriptionA(hr), (LPCSTR)DXGetErrorStringA(hr), 0);
			return;
		}

		BYTE* pTextureBuffer = static_cast<BYTE*>(pLockedRect.pBits);
		INT   nTexturePitch = pLockedRect.Pitch;
		for (UINT row = 0; row < height; ++row)
		{
			BYTE* pdwDest = pTextureBuffer;

			for (UINT column = 0; column < width; ++column)
			{
				pdwDest[3] = 255;
				pdwDest[2] = data[row * width * 3 + column * 3];
				pdwDest[1] = data[row * width * 3 + column * 3 + 1];
				pdwDest[0] = data[row * width * 3 + column * 3 + 2];

				pdwDest += 4;
			}
			pTextureBuffer += nTexturePitch;
		}
		m_pDeviceTexture->UnlockRect(0);
	}
}

