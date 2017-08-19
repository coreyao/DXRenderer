#pragma once

#include "Utils/Util.h"

class Texture
{
public:
	enum TextureFormat
	{
		TF_R8G8B8,
		TF_R8G8B8A8,
	};

	Texture(const std::string& str, TextureFormat format = TF_R8G8B8);

	std::string m_path;
	unsigned int m_width;
	unsigned int m_height;
	TextureFormat m_format;
	IDirect3DTexture9* m_pDeviceTexture;

	static Texture* GetTexture(const std::string& str);
	static std::map<std::string, Texture*> m_vLoadedTextures;

private:
	void LoadData();
};