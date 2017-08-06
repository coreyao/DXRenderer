#include <d3d9.h>
#include <d3dx9.h>
#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 )

#include "Core/Camera.h"

LPDIRECT3D9             g_pD3D = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // Our rendering device

LPDIRECT3DVERTEXBUFFER9			g_pVB = NULL; // Buffer to hold Vertices
LPDIRECT3DINDEXBUFFER9          g_pIB = NULL;
LPDIRECT3DVERTEXDECLARATION9    g_pVertexDeclaration = NULL;
LPDIRECT3DVERTEXSHADER9         g_pVertexShader = NULL;
LPDIRECT3DPIXELSHADER9			g_pPixelShader = NULL;
LPD3DXCONSTANTTABLE             g_pConstantTableVertex = NULL;
LPD3DXCONSTANTTABLE             g_pConstantTablePixel = NULL;

Camera g_camera;

HRESULT InitD3D(HWND hWnd)
{
	// Create the D3D object.
	if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;

	// Set up the structure used to create the D3DDevice
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	// Create the D3DDevice
	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice)))
	{
		return E_FAIL;
	}

	// Device state would normally be set here

	return S_OK;
}

HRESULT Init()
{
	// Create vertex shader
	LPD3DXBUFFER pCode;
	LPD3DXBUFFER pError = NULL;
	HRESULT hr;
	D3DVERTEXELEMENT9 decl[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		D3DDECL_END()
	};
	g_pd3dDevice->CreateVertexDeclaration(decl, &g_pVertexDeclaration);

	// Create the vertex shader
	WCHAR strPath[512] = L"../Sources/Shaders/simple_unlit.hlsl";
	D3DXCompileShaderFromFile(strPath, NULL, NULL, "VSMain",
		D3DXGetVertexShaderProfile(g_pd3dDevice), 0, &pCode,
		&pError, &g_pConstantTableVertex);
	if (pError != NULL)
	{
		::MessageBoxA(0, (LPCSTR)pError->GetBufferPointer(), 0, 0);
	}

	hr = g_pd3dDevice->CreateVertexShader((DWORD*)pCode->GetBufferPointer(),&g_pVertexShader);
	pCode->Release();

	// Create the pixel shader
	D3DXCompileShaderFromFile(strPath, NULL, NULL, "PSMain",
		D3DXGetPixelShaderProfile(g_pd3dDevice), 0, &pCode,
		&pError, &g_pConstantTablePixel);
	if (pError != NULL)
	{
		::MessageBoxA(0, (LPCSTR)pError->GetBufferPointer(), 0, 0);
	}

	hr = g_pd3dDevice->CreatePixelShader((DWORD*)pCode->GetBufferPointer(), &g_pPixelShader);
	pCode->Release();

	// Create and initialize index buffer
	WORD* pIndices;
	g_pd3dDevice->CreateIndexBuffer(3 * sizeof(WORD), 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &g_pIB, NULL);
	g_pIB->Lock(0, 0, (void**)&pIndices, 0);
	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	g_pIB->Unlock();

	// Create and initialize vertex buffer
	g_pd3dDevice->CreateVertexBuffer(3 * sizeof(D3DXVECTOR3), 0, 0, D3DPOOL_DEFAULT, &g_pVB, NULL);
	D3DXVECTOR3* pVertices;
	g_pVB->Lock(0, 0, (void**)&pVertices, 0); 
	pVertices[0] = D3DXVECTOR3(0, 1.0f, 2.0f);
	pVertices[1] = D3DXVECTOR3(1.0f, -1.0f, 2.0f);
	pVertices[2] = D3DXVECTOR3(-1.0f, -1.0f, 2.0f);
	hr = g_pVB->Unlock();

	return S_OK;
}

VOID Cleanup()
{
	if (g_pVB != NULL)
		g_pVB->Release();

	if (g_pd3dDevice != NULL)
		g_pd3dDevice->Release();

	if (g_pD3D != NULL)
		g_pD3D->Release();
}

void Update(float deltaTime)
{
	D3DXMATRIX m = g_camera.GetViewMatrix() * g_camera.GetProjectionMatrix();

	D3DXMATRIX mViewProj;
	memcpy((void*)&mViewProj, (void*)&m, sizeof(D3DXMATRIX));
	g_pConstantTableVertex->SetMatrix(g_pd3dDevice, "mViewProj", &mViewProj);
	g_pConstantTablePixel->SetFloat(g_pd3dDevice, "fIntensity", 1.0f);
}

VOID Render()
{
	// Clear the backbuffer to a blue color
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	// Begin the scene
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		g_pd3dDevice->SetVertexDeclaration(g_pVertexDeclaration);
		g_pd3dDevice->SetVertexShader(g_pVertexShader);
		g_pd3dDevice->SetPixelShader(g_pPixelShader);
		g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(D3DXVECTOR3));
		g_pd3dDevice->SetIndices(g_pIB);

		g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 3, 0, 1);

		// End the scene
		g_pd3dDevice->EndScene();
	}

	// Present the backbuffer contents to the display
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		Cleanup();
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

INT WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, INT)
{
	UNREFERENCED_PARAMETER(hInst);

	// Register the window class
	WNDCLASSEX wc =
	{
		sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		L"Renderer", NULL
	};
	RegisterClassEx(&wc);

	// Create the application's window
	HWND hWnd = CreateWindow(L"Renderer", L"Renderer",
		WS_OVERLAPPEDWINDOW, 100, 100, 1280, 720,
		NULL, NULL, wc.hInstance, NULL);

	// Initialize Direct3D
	if (SUCCEEDED(InitD3D(hWnd)))
	{
		Init();

		// Show the window
		ShowWindow(hWnd, SW_SHOWDEFAULT);
		UpdateWindow(hWnd);

		// Enter the message loop
		MSG msg;
		ZeroMemory(&msg, sizeof(msg));
		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				Update(1.0f / 60);
				Render();
			}
		}
	}

	UnregisterClass(L"D3D Tutorial", wc.hInstance);
	return 0;
}
