#pragma once

#include "Utils/Util.h"

class Model;
class Camera;
class Application
{
public:
	Application();
	~Application();

	HRESULT Init(HWND hWnd);
	void Update(float deltaTime);
	VOID Render();
	void Cleanup();

	void InitScene();

	Camera* m_camera;
	Model* m_model;

	static LPDIRECT3D9 m_pD3D;
	static LPDIRECT3DDEVICE9 m_pd3dDevice;
};