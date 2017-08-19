#include "Core/Application.h"
#include "Core/Model/Model.h"
#include "Core/Camera.h"

Application::Application()
	: m_bDragging(false)
	, m_bWireframe(false)
{
}

Application::~Application()
{
}

HRESULT Application::Init(HWND hWnd)
{
	if (NULL == (m_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;

	// Set up the structure used to create the D3DDevice
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	// Create the D3DDevice
	if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &m_pd3dDevice)))
	{
		return E_FAIL;
	}

	// Device state would normally be set here

	return S_OK;
}

void Application::Update(float deltaTime)
{
	POINT pot;
	BOOL ret = GetCursorPos(&pot);
	if (ret)
	{
		
	}
}

VOID Application::Render()
{
	// Clear the backbuffer to a blue color
	m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	// Begin the scene
	if (SUCCEEDED(m_pd3dDevice->BeginScene()))
	{
		m_model->Render(m_camera);

		// End the scene
		m_pd3dDevice->EndScene();
	}

	// Present the backbuffer contents to the display
	m_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

void Application::Cleanup()
{
	if (m_pd3dDevice != NULL)
		m_pd3dDevice->Release();

	if (m_pD3D != NULL)
		m_pD3D->Release();
}

void Application::InitScene()
{
	m_model = new Model("../Resources/sponza_crytek_max_obj/sponza.obj");
	m_camera = new Camera();
	m_camera->GetTransform().SetPosition(D3DXVECTOR3(0, 100, -500));
}

void Application::HandleInput(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_LBUTTONDOWN)
	{
		m_bDragging = true;
	}
	else if (uMsg == WM_MOUSEMOVE)
	{
		if (m_bDragging)
		{
			int iMouseX = (short)LOWORD(lParam);
			int iMouseY = (short)HIWORD(lParam);

			if (m_lastMousePos == D3DXVECTOR2(0, 0))
			{
				m_lastMousePos = D3DXVECTOR2(iMouseX, iMouseY);
			}
			else
			{
				float yaw = (iMouseX - m_lastMousePos.x) * 0.05f;
				float pitch = (iMouseY - m_lastMousePos.y) * 0.05f;
				D3DXVECTOR3 rotation = m_camera->GetTransform().GetEulerRotation();
				m_camera->GetTransform().SetEulerRotation(D3DXVECTOR3(rotation.x + pitch, rotation.y + yaw, rotation.z));

				m_lastMousePos = D3DXVECTOR2(iMouseX, iMouseY);
			}
		}
	}
	else if (uMsg == WM_LBUTTONUP)
	{
		m_bDragging = false;
		m_lastMousePos = D3DXVECTOR2(0, 0);
	}
	else if (uMsg == WM_KEYDOWN)
	{
		const int moveOffset = 10;
		int keyCode = wParam;
		D3DXVECTOR3 pos = m_camera->GetTransform().GetPosition();
		if (keyCode == 0x41)
		{
			m_camera->GetTransform().SetPosition(D3DXVECTOR3(pos.x - moveOffset, pos.y, pos.z));
		}
		else if (keyCode == 0x44)
		{
			m_camera->GetTransform().SetPosition(D3DXVECTOR3(pos.x + moveOffset, pos.y, pos.z));
		}
		else if (keyCode == 0x57)
		{
			m_camera->GetTransform().SetPosition(D3DXVECTOR3(pos.x, pos.y, pos.z + moveOffset));
		}
		else if (keyCode == 0x53)
		{
			m_camera->GetTransform().SetPosition(D3DXVECTOR3(pos.x, pos.y, pos.z - moveOffset));
		}
		else if (keyCode == 0x51)
		{
			m_camera->GetTransform().SetPosition(D3DXVECTOR3(pos.x, pos.y - moveOffset, pos.z));
		}
		else if (keyCode == 0x45)
		{
			m_camera->GetTransform().SetPosition(D3DXVECTOR3(pos.x, pos.y + moveOffset, pos.z));
		}
		else if (keyCode == 0x48)
		{
			m_bWireframe = !m_bWireframe;
			Application::m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, m_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID);
		}

	}

}

LPDIRECT3D9 Application::m_pD3D;
LPDIRECT3DDEVICE9 Application::m_pd3dDevice;

