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
	LoadData();
}

void Texture::LoadData()
{
	int width, height, nrChannels;
	unsigned char *data = stbi_load(m_path.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		HRESULT hr = Application::m_pd3dDevice->CreateTexture(width, height, 0, 0, D3DFMT_R8G8B8, D3DPOOL_DEFAULT, &m_pDeviceTexture, NULL);
		assert(hr == S_OK);
		D3DLOCKED_RECT *pLockedRect = NULL;
		m_pDeviceTexture->LockRect(0, pLockedRect, NULL, 0);
		BYTE* pTextureBuffer = static_cast<BYTE*>(pLockedRect->pBits);
		INT   nTexturePitch = pLockedRect->Pitch;
		for (UINT row = 0; row < height; ++row)
		{
			DWORD* pdwDest = (DWORD*)pTextureBuffer;

			for (UINT column = 0; column < width; ++column)
			{
				pdwDest[0] = data[row * width * 3 + column * 3];
				pdwDest[1] = data[row * width * 3 + column * 3 + 1];
				pdwDest[2] = data[row * width * 3 + column * 3 + 2];

				pdwDest += 3;
			}
			pTextureBuffer += nTexturePitch;
		}
		m_pDeviceTexture->UnlockRect(0);
	}
}

