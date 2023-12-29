//--------------------------------------------------------------------------------------
// ���� 2. ��������� ������������. ������� �� ������� �� DX SDK (c) Microsoft Corp.
//--------------------------------------------------------------------------------------
#include <windows.h>
#include <d3d11.h>
#include <D3DX11.h>
#include <d3dcompiler.h>	// ���������� ����� ���������
#include <xnamath.h>
#include "resource.h"
#include <string>
#include <list>
#include <vector>


//���������: --------------------------------------------------------------------------------------
// ��������� �������
struct SimpleVertex
{
    XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

// ��������� ������������ ������ (��������� �� ���������� � �������)
struct ConstantBuffer
{
	XMMATRIX mWorld;       // ������� ����
	XMMATRIX mView;        // ������� ����
	XMMATRIX mProjection;  // ������� ��������
};

struct ColorBuffer
{
	DWORD Color[10];
};

bool onIndex = true;

struct object
{
	std::string name;
	UINT cou_ver, cou_ind;
	UINT beg_v, beg_i;
	UINT getCou() { 
		return onIndex ? cou_ind : cou_ver; }
	UINT getBeg() { 
		return onIndex ? beg_i : beg_i; }
};

//**���������--------------------------------------------------------------------------------------

std::list<object> objects;

auto to = objects.begin();

int cur_top = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
_XMFLOAT2 curAngel = { 0,0 };
UINT cou_vert;



//--------------------------------------------------------------------------------------
// ���������� ����������
//--------------------------------------------------------------------------------------
HINSTANCE               g_hInst = NULL;
HWND                    g_hWnd = NULL;
D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*           g_pd3dDevice = NULL;		// ���������� (��� �������� ��������)
ID3D11DeviceContext*    g_pImmediateContext = NULL;	// �������� ���������� (���������)
IDXGISwapChain*         g_pSwapChain = NULL;		// ���� ����� (������ � �������)
ID3D11RenderTargetView* g_pRenderTargetView = NULL;	// ������ ������� ������
ID3D11Texture2D*		g_pDepthStencil = NULL;		// �������� ������ ������
ID3D11DepthStencilView* g_pDepthStencilView = NULL;	// ������ ����, ����� ������

ID3D11VertexShader*     g_pVertexShader = NULL;		// ��������� ������
ID3D11PixelShader*      g_pPixelShader = NULL;		// ���������� ������
ID3D11InputLayout*      g_pVertexLayout = NULL;		// �������� ������� ������
ID3D11Buffer*           g_pVertexBuffer = NULL;		// ����� ������
ID3D11Buffer*			g_pIndexBuffer = NULL;		// ����� �������� ������
ID3D11Buffer*			g_pConstantBuffer = NULL;	// ����������� �����

XMMATRIX                g_World;					// ������� ����
XMMATRIX                g_View;						// ������� ����
XMMATRIX                g_Projection;				// ������� ��������


//--------------------------------------------------------------------------------------
// ��������������� ���������� �������
//--------------------------------------------------------------------------------------
HRESULT InitDevice();			// ������������� ��������� DirectX
HRESULT InitGeometry();			// ������������� ������� ����� � ������ ������
HRESULT InitMatrixes();			// ������������� ������
void Render();					// ������� ���������

void SetMatrixes(float fYaw = 0, float fPitch = 0, float fRoll = 0);
void SetMatrixes2(float fAngel);

LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );	  // ������� ����

//--------------------------------------------------------------------------------------
// ����������� ������ � �������� ����
//--------------------------------------------------------------------------------------
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)// �������� ����
{
	// ����������� ������
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_ICON1);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"Urok2WindowClass";
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_ICON1);
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	// �������� ����
	g_hInst = hInstance;
	RECT rc = { 0, 0, 600, 600 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	g_hWnd = CreateWindow(L"Urok2WindowClass", L"���� 2: ��������� ������������",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
		NULL);
	if (!g_hWnd)
		return E_FAIL;

	ShowWindow(g_hWnd, nCmdShow);

	return S_OK;
}

//--------------------------------------------------------------------------------------
// ������������ ���� ��������� ��������
//--------------------------------------------------------------------------------------
void CleanupDevice()// �������� ���������� ��������� DirectX
{
	// ������� �������� �������� ����������
	if (g_pImmediateContext) g_pImmediateContext->ClearState();
	// ����� ������ �������
	if (g_pConstantBuffer) g_pConstantBuffer->Release();
	if (g_pVertexBuffer) g_pVertexBuffer->Release();
	if (g_pIndexBuffer) g_pIndexBuffer->Release();
	if (g_pVertexLayout) g_pVertexLayout->Release();
	if (g_pVertexShader) g_pVertexShader->Release();
	if (g_pPixelShader) g_pPixelShader->Release();
	if (g_pDepthStencil) g_pDepthStencil->Release();
	if (g_pDepthStencilView) g_pDepthStencilView->Release();
	if (g_pRenderTargetView) g_pRenderTargetView->Release();
	if (g_pSwapChain) g_pSwapChain->Release();
	if (g_pImmediateContext) g_pImmediateContext->Release();
	if (g_pd3dDevice) g_pd3dDevice->Release();
}

//--------------------------------------------------------------------------------------
// ����� ����� � ���������. ������������� ���� �������� � ���� � ���� ���������.
// ��������� ����� ������������ ��� ��������� �����.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );

	// �������� ���� ����������
    if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
        return 0;

	// �������� �������� DirectX
    if( FAILED( InitDevice() ) )
    {
        CleanupDevice();
        return 0;
    }

	// �������� �������� � ������ ������
    if( FAILED( InitGeometry() ) )
    {
        CleanupDevice();
        return 0;
    }

	// ������������� ������
	if (FAILED(InitMatrixes()))
	{
		CleanupDevice();
		return 0;
	}
	SetMatrixes2(0);

    // ������� ���� ���������
    MSG msg = {0};
    while( WM_QUIT != msg.message )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else	// ���� ��������� ���
        {
            Render();	// ������
        }
    }

    CleanupDevice();

    return (int)msg.wParam;
}


//--------------------------------------------------------------------------------------
// ���������� ������ ���, ����� ���������� �������� ��������� ���������
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch( message )
    {
        case WM_PAINT:
            hdc = BeginPaint( hWnd, &ps );
            EndPaint( hWnd, &ps );
            break;

        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;

		case WM_SIZE:
			CleanupDevice();
			if (FAILED(InitDevice()))
			{
				CleanupDevice();
				return 0;
			}
			// �������� �������� � ������ ������
			if (FAILED(InitGeometry()))
			{
				CleanupDevice();
				return 0;
			}
			if (FAILED(InitMatrixes()))
			{
				CleanupDevice();
				return 0;
			}
			break;

		case WM_RBUTTONDOWN:
		case WM_LBUTTONDOWN:
		{
			static POINT pm;
			pm.y = HIWORD(lParam);
			pm.x = LOWORD(lParam);
			break; 
		}/*
		case WM_RBUTTONUP:
		case WM_LBUTTONUP:
		{
			md = false;
			break;
		}*/
		case WM_MOUSEMOVE:
		{
			if (LOWORD(wParam) == 0) return 0;
			static POINT pm;
			POINT tm;

			tm.y = HIWORD(lParam);
			tm.x = LOWORD(lParam);
			curAngel.x = (pm.y - tm.y) * XM_PI / 100;
			curAngel.y = (pm.x - tm.x) * XM_PI / 100;
			SetMatrixes(curAngel.x, curAngel.y);

			//SetMatrixes((pm.x - tm.x) * XM_PI/ 100 , (pm.x - tm.x) * XM_PI/ 100 );

			return 0;
			break;
		}

		case WM_KEYDOWN:
		{
			switch (LOWORD(wParam)) {
			case 37: curAngel.y -= 0.025f; break;
			case 38: curAngel.x -= 0.025f; break;
			case 39: curAngel.y += 0.025f; break;
			case 40: curAngel.x += 0.025f; break;
			default:
				SetWindowTextA(hWnd, std::to_string(LOWORD(wParam)).c_str());
				return 0;
			}
			SetMatrixes(curAngel.x, curAngel.y);
			break;
		}

		case WM_CHAR:
		{
			switch (LOWORD(wParam)) {
			case 32:
			{static float t = 0.0f;
			t += 0.125f;
			SetMatrixes2(t);
			break; }
			case 46:
			{
				if(objects.size() == 1) break;
				if (++to == objects.end())
					to = objects.begin();
				cou_vert = to->getCou();
				break; }
			case 43:
			{
				if (++cou_vert > to->getCou())
					cou_vert = 1;
				break; }
			case 45:
			{
				if (cou_vert-- == 0)
					cou_vert = to->getCou();
				break; }
			case 47:
			{
				if (--cur_top == 0)
					cur_top = 5;
				g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY(cur_top));
				break; }
			case 105: 
				onIndex = !onIndex;
				cou_vert = to->getCou();
				break;
			case 122:
				curAngel.y = 0.0f;
				curAngel.x = 0.0f;
				SetMatrixes(curAngel.x, curAngel.y);
				break;
			case 42:
			{
				if (++cur_top > 5)
					cur_top = 1;
				g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY(cur_top));
				break; }
			default:
				SetWindowTextA(hWnd, std::to_string(LOWORD(wParam)).c_str());
				return 0;
			}
			SetWindowTextA(hWnd, (to->name + std::to_string(cou_vert) + "  /  " + std::to_string(cur_top) + " : " + std::to_string(onIndex)).c_str());
			break;
		}

        default:
            return DefWindowProc( hWnd, message, wParam, lParam );
    }

    return 0;
}


//--------------------------------------------------------------------------------------
// ��������������� ������� ��� ���������� �������� � D3DX11
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
{
    HRESULT hr = S_OK;
    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
    ID3DBlob* pErrorBlob;
    hr = D3DX11CompileFromFile( szFileName, NULL, NULL, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL );
    if( FAILED(hr) )
    {
        if( pErrorBlob != NULL )
            OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
        if( pErrorBlob ) pErrorBlob->Release();
        return hr;
    }
    if( pErrorBlob ) pErrorBlob->Release();

    return S_OK;
}


//--------------------------------------------------------------------------------------
// �������� ���������� Direct3D (D3D Device), ��������� ���� (Swap Chain) �
// ��������� ���������� (Immediate Context).
//--------------------------------------------------------------------------------------
HRESULT InitDevice()
{
    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect( g_hWnd, &rc );
    UINT width = rc.right - rc.left;	// �������� ������
    UINT height = rc.bottom - rc.top;	// � ������ ����

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE( driverTypes );

    // ��� �� ������� ������ �������������� ������ DirectX
    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
	UINT numFeatureLevels = ARRAYSIZE( featureLevels );

	// ������ �� �������� ���������� DirectX. ��� ������ �������� ���������,
	// ������� ��������� �������� ��������� ������ � ����������� ��� � ������ ����.
    DXGI_SWAP_CHAIN_DESC sd;			// ���������, ����������� ���� ����� (Swap Chain)
    ZeroMemory( &sd, sizeof( sd ) );	// ������� ��
	sd.BufferCount = 1;					// � ��� ���� �����
    sd.BufferDesc.Width = width;		// ������ ������
    sd.BufferDesc.Height = height;		// ������ ������
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// ������ ������� � ������
    sd.BufferDesc.RefreshRate.Numerator = 75;			// ������� ���������� ������
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// ���������� ������ - ������ �����
    sd.OutputWindow = g_hWnd;							// ����������� � ������ ����
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;						// �� ������������� �����

    for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
    {
        g_driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain( NULL, g_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
                                            D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext );
        if (SUCCEEDED(hr))  // ���� ���������� ������� �������, �� ������� �� �����
            break;
    }
    if (FAILED(hr))
        return hr;

    // ������ ������� ������ �����. �������� ��������, � SDK
    // RenderTargetOutput - ��� �������� �����, � RenderTargetView - ������.

	// ��������� �������� ������� ������
    ID3D11Texture2D* pBackBuffer = NULL;
    hr = g_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
    if (FAILED(hr))	return hr;

	// �� ����������� �������� ������� ����������� ���������
    hr = g_pd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &g_pRenderTargetView );
    pBackBuffer->Release();
    if (FAILED(hr))	return hr;

	// ��������� � �������� ������ ������
	// ������� ��������-�������� ������ ������
	D3D11_TEXTURE2D_DESC descDepth;     // ��������� � �����������
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = width;            // ������ �
	descDepth.Height = height;    // ������ ��������
	descDepth.MipLevels = 1;            // ������� ������������
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // ������ (������ �������)
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;         // ��� - ����� ������
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	// ��� ������ ����������� ���������-�������� ������� ������ ��������
	hr = g_pd3dDevice->CreateTexture2D(&descDepth, NULL, &g_pDepthStencil);
	if (FAILED(hr)) return hr;

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;            // ��������� � �����������
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;         // ������ ��� � ��������
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	// ��� ������ ����������� ���������-�������� � �������� ������� ������ ������ ������
	hr = g_pd3dDevice->CreateDepthStencilView(g_pDepthStencil, &descDSV, &g_pDepthStencilView);
	if (FAILED(hr)) return hr;

    // ���������� ������ ������� ������ � ��������� ����������
	g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);
    //g_pImmediateContext->OMSetRenderTargets( 1, &g_pRenderTargetView, NULL );

    // ��������� �������� (������� � ������� ���������). � ���������� ������� �� ����������
	// �������������, ���� �� ��� ����� ����.
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pImmediateContext->RSSetViewports( 1, &vp );

    return S_OK;
}

//--------------------------------------------------------------------------------------
// ������������� ������
//--------------------------------------------------------------------------------------
HRESULT InitMatrixes()
{
	RECT rc;
	GetClientRect(g_hWnd, &rc);
	UINT width = rc.right - rc.left;    // �������� ������
	UINT height = rc.bottom - rc.top;   // � ������ ����

	// ������������� ������� ����
	g_World = XMMatrixIdentity();

	// ������������� ������� ����
	XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, -3.0f, 0.0f);  // ������ �������
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);    // ���� �������
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);    // ����������� �����
	g_View = XMMatrixLookAtLH(Eye, At, Up);
	// ������������� ������� ��������
	g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.01f, 100.0f);
	return S_OK;
}

void DrawShar(LPSTR name, SimpleVertex *vertices, WORD *indices, UINT& tv, UINT& ti, UINT numElements = 6)
{
	object obj;
	obj.beg_v = tv;
	vertices[tv++] = { XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 1.0, 0.0f, 1.0f) };

	//���
	for (int z = 1; z < numElements; z++) {
		float a1 = XM_PI / numElements * z;
		for (int x = 0; x < numElements; x++) {
			float a2 = XM_2PI / numElements * x;
			auto& tp = vertices[tv].Pos;
			tp.x = sinf(a1) * cosf(a2);
			tp.y = sinf(a1) * sinf(a2);
			tp.z = cosf(a1);
			vertices[tv++].Color = XMFLOAT4(1.0f / numElements * z, 1.0f / numElements * (numElements - z), 0.0f, 1.0f);
		}
	}
	
	vertices[tv++] = { XMFLOAT3(sinf(XM_PI), 0, cosf(XM_PI)), XMFLOAT4(1.0, 0.0f, 0.0f, 1.0f) };
	obj.cou_ver = tv - obj.beg_v;

	obj.beg_i = ti;
	for (int x = 1; x <= numElements; x++)
	{
		indices[ti++] = obj.beg_v;
		indices[ti++] = obj.beg_v + x;
		indices[ti++] = obj.beg_v + x + 1;
	}
	indices[ti - 1] = obj.beg_v + 1;


	for (int z = 0; z < numElements - 2; z++) {
		for (int x = 0; x < numElements; x++)
		{
			int ind = obj.beg_v + numElements * z + x + 1;
			indices[ti++] = ind + numElements - 1;
			indices[ti++] = ind + numElements;
			indices[ti++] = ind;

			indices[ti++] = ind;
			indices[ti++] = ind + numElements;
			indices[ti++] = ind + 1;
		}
	}

	for (int x = 0; x <= numElements-1; x++)
	{
		indices[ti++] = tv - 1;
		indices[ti++] = tv - 2 - x;
		indices[ti++] = tv - 3 - x;
	}
	indices[ti - 1] = tv - 2;

	obj.cou_ind = ti- obj.beg_i;
	obj.name = std::string(name) + " - " + std::to_string(obj.cou_ver) + " (" + std::to_string(obj.cou_ind) + ")";

	objects.emplace_back(obj);
}

void DrawBuble(LPSTR name, SimpleVertex* vertices, WORD* indices, UINT& tv, UINT& ti, 
	POINT numElements = { 10 , 6 }, XMFLOAT4 rad = { 1.f, 1.f, 0.3f, 0.3f}, float sd = 0.f)
{
	object obj;
	obj.beg_v = tv;
	obj.beg_i = ti;

	auto m = XMMatrixTranslation(rad.x - rad.z, 0.f, 0.f);
	if (sd != 0) m = XMMatrixRotationX(sd) * m;
	//auto m = XMMatrixRotationX(XM_PI / 4) * XMMatrixTranslation(r1 - r2, 0.f, 0.f);
	   
	for (int x = 0; x < numElements.y; x++) {
		float a = XM_2PI / numElements.y * x;
		auto mr = XMVector3Transform(XMLoadFloat3(&XMFLOAT3(cosf(a) * rad.z, 0.0f, sinf(a) * rad.w)), m).m128_f32;
		vertices[tv++] = { XMFLOAT3(mr[0], mr[1] * rad.x / rad.y, mr[2]), XMFLOAT4(0.0f, 1.0f, 1.0f / numElements.y * (x + 1), 1.0f) };
	}

	WORD delta;
	for (int z = 1; z <= numElements.x -1; z++) {
		m = XMMatrixRotationZ(XM_2PI / numElements.x * z);
		delta =  obj.beg_v + (z * numElements.y);
		for (int x = 0; x < numElements.y; x++) {
			auto mr = XMVector3Transform(XMLoadFloat3(&(vertices[obj.beg_v + x].Pos)), m).m128_f32;
			vertices[tv++] = { XMFLOAT3(mr[0], mr[1], mr[2]), 
				XMFLOAT4(1.0f / numElements.x * z, 1.0f / numElements.y * (x+1), 1.0f / numElements.x * (numElements.x - z), 1.0f) };

			indices[ti++] = delta + x - 1;
			indices[ti++] = indices[ti - 1] + 1;
			indices[ti++] = indices[ti - 1] - numElements.y;

			indices[ti++] = indices[ti - 1];
			indices[ti++] = indices[ti - 3];
			indices[ti++] = indices[ti - 2] + 1;
		}
	}

	delta = tv;
	obj.cou_ver = tv - obj.beg_v;

	#define circle(x, p, d)	((x)<(p)?(x):(x-d))
	
	for (int x = 0; x < numElements.y; x++) {
		indices[ti++] = circle(delta + x - 1, delta, obj.cou_ver);
		indices[ti++] = circle(delta + x, delta, obj.cou_ver);
		indices[ti++] = circle(delta + x - numElements.y, delta, obj.cou_ver);

		indices[ti++] = indices[ti - 1];
		indices[ti++] = indices[ti - 3];
		indices[ti++] = circle(indices[ti - 2] + 1, delta, obj.cou_ver);
	}


	//vertices[tv] = { XMFLOAT3(sinf(XM_PI), 0, cosf(XM_PI)), XMFLOAT4(1.0, 0.0f, 0.0f, 1.0f) };
	obj.cou_ind = ti - obj.beg_i;
	obj.name = std::string(name) + " - " + std::to_string(obj.cou_ver) + " (" + std::to_string(obj.cou_ind) + ")";
	
	objects.emplace_back(obj);
}



//--------------------------------------------------------------------------------------
// �������� ������ ������, �������� (shaders) � �������� ������� ������ (input layout)
//--------------------------------------------------------------------------------------
HRESULT InitGeometry()
{
	HRESULT hr = S_OK;

	// ���������� ���������� ������� �� �����
    ID3DBlob* pVSBlob = NULL; // ��������������� ������ - ������ ����� � ����������� ������
    hr = CompileShaderFromFile( L"Urok2.fx", "VS", "vs_4_0", &pVSBlob );
    if (FAILED(hr))
    {
        MessageBox( NULL, L"���������� �������������� ���� FX. ����������, ��������� ������ ��������� �� �����, ���������� ���� FX.", L"������", MB_OK );
        return hr;
    }

	// �������� ���������� �������
	hr = g_pd3dDevice->CreateVertexShader( pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_pVertexShader );
	if(FAILED( hr ))
	{	
		pVSBlob->Release();
        return hr;
	}

    // ����������� ������� ������
	// ������� ����� ����� ��������� ��������� - ���������� � ������������, �������, ����, ����������
	// ��������. ������ ������ ���������, ����� ������ ��������� �������� �������, ������� �� ����������
	// ������������. ���� ������� (SimpleVertex) �������� ������ ���������� � ����������� � ������������.
	// ����� �� �� ��������� ��������� ������, ������� ����� �������������� ��� ��������� ���������� �
	// ����� ��������.
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		/* ������������� ���, ������������� ������, ������, �������� ���� (0-15), ����� ������ ������
		   � ������ ������, ����� ��������� ����� (�� �����), InstanceDataStepRate (�� �����) */
    };
	UINT numElements = ARRAYSIZE( layout );

    // �������� ������� ������
	hr = g_pd3dDevice->CreateInputLayout( layout, numElements, pVSBlob->GetBufferPointer(),
                                          pVSBlob->GetBufferSize(), &g_pVertexLayout );
	pVSBlob->Release();
	if (FAILED(hr)) return hr;

    // ����������� ������� ������
    g_pImmediateContext->IASetInputLayout( g_pVertexLayout );

	// ���������� ����������� ������� �� �����
	ID3DBlob* pPSBlob = NULL;
    hr = CompileShaderFromFile( L"Urok2.fx", "PS", "ps_4_0", &pPSBlob );
    if( FAILED( hr ) )
    {
        MessageBox( NULL, L"���������� �������������� ���� FX. ����������, ��������� ������ ��������� �� �����, ���������� ���� FX.", L"������", MB_OK );
        return hr;
    }

	// �������� ����������� �������
	hr = g_pd3dDevice->CreatePixelShader( pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_pPixelShader );
	pPSBlob->Release();
	if (FAILED(hr)) return hr;

    // �������� ������ ������ (��� ������� ������������)

	objects.clear();
	SimpleVertex vertices[2000];
	WORD indices[10000];

	UINT v(0), i(0);
	DrawShar("��� ", vertices, indices, v, i, 25);
	//DrawShar("��� ", vertices, indices, v, i, 15);
	DrawBuble("Buble8 ", vertices, indices, v, i, { 7, 3 });
	/*DrawBuble("������ ����� ", vertices, indices, v, i, { 15, 15 }, { 1.f, 0.5f, 0.3f, 0.1f });
	DrawBuble("������ ", vertices, indices, v, i, { 15, 15 });*/
	//DrawBuble(vertices, indices, v, i, 50, 20);
	//DrawShar(vertices, indices, v, i, 25);

	if (v > 2000) { MessageBox(NULL, (L"����� �� ������� ������� ������:" + std::to_wstring(v)).c_str(), L"������", MB_OK); return 5; }
	if (i > 10000) { MessageBox(NULL, (L"����� �� ������� ������� ��������:" + std::to_wstring(i)).c_str(), L"������", MB_OK); return 5; }
	
	D3D11_BUFFER_DESC bd;	// ���������, ����������� ����������� �����
	ZeroMemory( &bd, sizeof(bd) );				// ������� ��
    bd.Usage = D3D11_USAGE_DEFAULT;	
    bd.ByteWidth = sizeof(SimpleVertex) * v;	// ������ ������
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// ��� ������ - ����� ������
	bd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA InitData; // ���������, ���������� ������ ������
	ZeroMemory( &InitData, sizeof(InitData) );	// ������� ��
    InitData.pSysMem = vertices;				// ��������� �� ���� 3 �������
	// ����� ������ g_pd3dDevice ������� ������ ������ ������
    hr = g_pd3dDevice->CreateBuffer( &bd, &InitData, &g_pVertexBuffer );
	if (FAILED(hr)) return hr;

	bd.Usage = D3D11_USAGE_DEFAULT;            // ���������, ����������� ����������� �����
	bd.ByteWidth = sizeof(WORD) * i; // ��� 6 ������������� ���������� 18 ������
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER; // ��� - ����� ��������
	bd.CPUAccessFlags = 0;
	InitData.pSysMem = indices;         // ��������� �� ��� ������ ��������
	// ����� ������ g_pd3dDevice ������� ������ ������ ��������
	hr = g_pd3dDevice->CreateBuffer(&bd, &InitData, &g_pIndexBuffer);
	if (FAILED(hr)) return hr;

    // ��������� ������ ������
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    g_pImmediateContext->IASetVertexBuffers( 0, 1, &g_pVertexBuffer, &stride, &offset );
	g_pImmediateContext->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	ZeroMemory(&bd, sizeof(bd));				// ������� ��
	// �������� ������������ ������
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);            // ������ ������ = ������� ���������
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // ��� - ����������� �����
	bd.CPUAccessFlags = 0;
	hr = g_pd3dDevice->CreateBuffer(&bd, NULL, &g_pConstantBuffer);
	if (FAILED(hr)) return hr;

	// ��������� ������� ��������� ������ � ������ (� ������ ������ - TRIANGLE LIST,
	// �. �. ����� 1-3 - ������ �����������, 4-6 - ������ � �. �. ������ ������ - TRIANGLE STRIP.
	// � ���� ������ ����� 1-3 - ������ �����������, 2-4 - ������, 3-5 - ������ � �. �.
	// � ���� ������� ���� ������ ���� �����������, ������� ������ ��������� �� ����� ��������.
	//g_pImmediateContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY(cur_top));

	to = objects.begin();
	cou_vert = to->getCou();

	return S_OK;
}


//void Get

//--------------------------------------------------------------------------------------
// ���������� ������
//--------------------------------------------------------------------------------------
void SetMatrixes_1(float fYaw = 0, float fPitch = 0, float fRoll = 0)
{
	// ���������� ����������-�������

	static float t = 0.0f;
	if (g_driverType == D3D_DRIVER_TYPE_REFERENCE)
	{
		t += (float)XM_PI * 0.0125f;
	}
	else
	{
		static DWORD dwTimeStart = 0;
		DWORD dwTimeCur = GetTickCount();
		if (dwTimeStart == 0)
			dwTimeStart = dwTimeCur;
		t = (dwTimeCur - dwTimeStart) / 500.0f;
	}

	// �������-������: ������� �������
	XMMATRIX mOrbit = XMMatrixRotationY(-t + fYaw);
	// �������-����: �������� ������� ������ ����� ���
	XMMATRIX mSpin = XMMatrixRotationY(t * 2);
	// �������-�������: ����������� �� ��� ������� ����� �� ������ ���������
	XMMATRIX mTranslate = XMMatrixTranslation(-2.0f, 0.0f, 0.0f);
	// �������-�������: ������ ������� � 2 ����
	XMMATRIX mScale = XMMatrixScaling(0.5f, 0.5f, 0.5f);

	// �������������� �������
	//  --������� �� � ������, � �������� 1:1:1, ��������� �� ���� ���� �� 0.0f.
	//  --������� -> ������������ ������ Y (���� �� ��� � ������) -> ��������� ����� ->
	//  --����� ������������ ������ Y.
	g_World =  mScale * mSpin * mTranslate *mOrbit;

	// �������� ����������� �����
	// ������� ��������� ��������� � ��������� � ��� �������
	ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose(g_World);
	cb.mView = XMMatrixTranspose(g_View);
	cb.mProjection = XMMatrixTranspose(g_Projection);
	// ��������� ��������� ��������� � ����������� ����� g_pConstantBuffer
	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cb, 0, 0);
}


//--------------------------------------------------------------------------------------
// ���������� ������
//--------------------------------------------------------------------------------------
void SetMatrixes(float fYaw, float fPitch, float fRoll)
{
	// ���������� ����������-�������

	static float t = 0.0f;
	if (g_driverType == D3D_DRIVER_TYPE_REFERENCE)
	{
		t += (float)XM_PI * 0.0125f;
	}
	else
	{
		static DWORD dwTimeStart = 0;
		DWORD dwTimeCur = GetTickCount();
		if (dwTimeStart == 0)
			dwTimeStart = dwTimeCur;
		t = (dwTimeCur - dwTimeStart) / 500.0f;
	}

	// �������-������: ������� �������

	XMMATRIX g_World = XMMatrixRotationRollPitchYaw(fYaw, fPitch, fRoll);

	// �������� ����������� �����
	// ������� ��������� ��������� � ��������� � ��� �������
	ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose(g_World);
	cb.mView = XMMatrixTranspose(g_View);
	cb.mProjection = XMMatrixTranspose(g_Projection);
	// ��������� ��������� ��������� � ����������� ����� g_pConstantBuffer
	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cb, 0, 0);
}

//--------------------------------------------------------------------------------------
// ���������� ������
//--------------------------------------------------------------------------------------
void SetMatrixes2(float fAngel)
{
	// ���������� ����������-�������
	static float t = 0.0f;
	/*if (g_driverType == D3D_DRIVER_TYPE_REFERENCE)
	{
		t += (float)XM_PI * 0.0125f;
	}
	else
	{
		static DWORD dwTimeStart = 0;
		DWORD dwTimeCur = GetTickCount();
		if (dwTimeStart == 0)
			dwTimeStart = dwTimeCur;
		t = (dwTimeCur - dwTimeStart) / 500.0f;
	}*/

	// �������-������: ������� �������
	XMMATRIX g_World = XMMatrixRotationY(-t + fAngel);
	// �������-����: �������� ������� ������ ����� ���
	/*XMMATRIX mSpin = XMMatrixRotationY(t * 2);
	// �������-�������: ����������� �� ��� ������� ����� �� ������ ���������
	XMMATRIX mTranslate = XMMatrixTranslation(-2.0f, 0.0f, 0.0f);
	// �������-�������: ������ ������� � 2 ����
	XMMATRIX mScale = XMMatrixScaling(0.5f, 0.5f, 0.5f);

	// �������������� �������
	//  --������� �� � ������, � �������� 1:1:1, ��������� �� ���� ���� �� 0.0f.
	//  --������� -> ������������ ������ Y (���� �� ��� � ������) -> ��������� ����� ->
	//  --����� ������������ ������ Y.
	g_World = mScale * mSpin * mTranslate * mOrbit;*/

	// �������� ����������� �����
	// ������� ��������� ��������� � ��������� � ��� �������
	ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose(g_World);
	cb.mView = XMMatrixTranspose(g_View);
	cb.mProjection = XMMatrixTranspose(g_Projection);
	// ��������� ��������� ��������� � ����������� ����� g_pConstantBuffer
	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer, 0, NULL, &cb, 0, 0);
}

//--------------------------------------------------------------------------------------
// ��������� �����
//--------------------------------------------------------------------------------------
void Render()
{
	// �������� ����� ������ �� 1.0 (������������ ��������)
	g_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    // �������� ������ �����
    float ClearColor[4] = { 0.15f, 0.15f, 0.3f, 1.0f }; // �������, �������, �����, �����-�����
    g_pImmediateContext->ClearRenderTargetView( g_pRenderTargetView, ClearColor );

	// ���������� � ���������� ��������� �������
	g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
	g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
	// ��� ����� ���������

	//if(to)
		if(onIndex)
			g_pImmediateContext->DrawIndexed(cou_vert, to->beg_i, 0);
		else
			g_pImmediateContext->Draw(cou_vert, to->beg_v);

	/*for (int i = 0; i < 6; i++) {
		// ������������� �������, �������� - ��������� ������������ ��� Y � ��������
		SetMatrixes2(i * (XM_PI * 2) / 6);
		g_pImmediateContext->DrawIndexed(cou_vert, 0, 0);
	}*/

    // ������� � �������� ����� (�� �����) ����������, ������������ � ������ ������.
    g_pSwapChain->Present( 1, 0 );
}
