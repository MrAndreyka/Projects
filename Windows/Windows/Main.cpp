#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
// Выше для утечек

#include <list>
#include <iostream>
#include <windows.h>
#include <commctrl.h>
#include <Tchar.h>
#pragma comment(lib, "comctl32.lib")

#include <atlstr.h>

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type = 'win32' name = 'Microsoft.Windows.Common-Controls' version = '6.0.0.0' processorArchitecture = 'x86' publicKeyToken = '6595b64144ccf1df' language = '*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type = 'win32' name = 'Microsoft.Windows.Common-Controls' version = '6.0.0.0' processorArchitecture = 'ia64' publicKeyToken = '6595b64144ccf1df' language = '*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type = 'win32' name = 'Microsoft.Windows.Common-Controls' version = '6.0.0.0' processorArchitecture = 'amd64' publicKeyToken = '6595b64144ccf1df' language = '*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type = 'win32' name = 'Microsoft.Windows.Common-Controls' version = '6.0.0.0' processorArchitecture = '*' publicKeyToken = '6595b64144ccf1df' language = '*'\"")
#endif
#endif

#include "..//..//Bpl//Splitter.h"
#include "..//..//Bpl//Class.h"
#include "..//..//Bpl//WinCom.cpp"
//#include "..//..//Bpl//BrowseDialog.h"
#include "resource.h"
//#include "afxres.h" // если используются формы в ресурсах

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {

	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtDumpMemoryLeaks();
	_CrtSetBreakAlloc(173);
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG );

	InitCommonControls();

	WNDCLASSEX Cl;
	Cl.cbSize = sizeof(Cl);
	Cl.style = CS_BYTEALIGNCLIENT;
	Cl.cbClsExtra = 0;
	Cl.cbWndExtra = 0;
	Cl.hbrBackground = (HBRUSH)GetSysColorBrush(COLOR_3DFACE);
	Cl.hCursor = LoadCursor(NULL, IDC_ARROW);
	Cl.hIcon = (HICON)LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	Cl.hIconSm = Cl.hIcon;
	Cl.hInstance = hInstance;
	Cl.lpfnWndProc = WindowProc;
	Cl.lpszMenuName = NULL;
	Cl.lpszClassName = _T("MyClass");
	
	if (!RegisterClassEx(&Cl)) { MessageBox(NULL, _T("Не зарегестрирован класс окна!"), _T("Ошибка"), MB_OK); return 1; }

	HWND MW = CreateWindow(Cl.lpszClassName, _T("for SE"), WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX | WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!MW) { MessageBox(NULL, _T("Не создано окно!"), _T("Ошибка"), MB_OK); return 1; }

	UnregisterClass(Cl.lpszClassName, hInstance);
	ShowWindow(MW, nCmdShow);

	MSG Mes;
	while (GetMessage(&Mes, NULL, 0, 0)) {
		TranslateMessage(&Mes);
		DispatchMessage(&Mes);
	}

	return Mes.wParam;
};//--------------------------------------------------------------------------------------------------

void OnCreate(HWND hwnd, HINSTANCE hInst) {
	RECT R;
	GetClientRect(hwnd, &R);
	auto styleEx =  WS_EX_STATICEDGE| WS_EX_CLIENTEDGE;
	auto style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | ES_WANTRETURN | ES_MULTILINE;

	HWND item = CreateWindow(WC_STATIC, NULL, WS_CHILD | WS_VISIBLE| WS_CLIPCHILDREN, 0, 0, 600, 450, hwnd, (HMENU)01, hInst, NULL),
		item2 = CreateWindow(WC_STATIC, NULL, WS_CHILD | WS_VISIBLE| WS_CLIPCHILDREN , 0, 0, 0, 0, item, (HMENU)02, hInst, NULL);

	ContSpliter(item, CreateWindowEx(styleEx, WC_EDIT, NULL, style, 100, 0, 80, 80, item, (HMENU)102, hInst, NULL),
		CreateWindowEx(styleEx, WC_EDIT, NULL, style, 0, 300, 80, 80, item, (HMENU)104, hInst, NULL), true, 0);
	
	/*SetSplitter(item, CreateWindowEx(styleEx, WC_EDIT, NULL, style, 0, 100, 80, 80, item, (HMENU)101, hInst, NULL), item2, false);
	SetSplitter(item2, CreateWindowEx(styleEx, WC_EDIT, NULL, style, 100, 0, 80, 80, item2, (HMENU)102, hInst, NULL),
		CreateWindowEx(styleEx, WC_EDIT, NULL, style, 0, 300, 80, 80, item2, (HMENU)104, hInst, NULL), true);*/


	CreateWindow(WC_BUTTON, _T("Go"), WS_CHILD | WS_VISIBLE, 5, 500, 150, 30, hwnd, (HMENU)501, hInst, nullptr);

};//--------------------------------------------------------------------------------------------------

void OnResize(HWND hwnd, int Width, int Height) {
	//struct MyRECT : RECT {
	//	inline MyRECT() {};
	//	inline MyRECT(LONG l, LONG t, LONG r, LONG b) { left = l; right = r; top = t; bottom = b; };
	//	inline MyRECT(RECT ini) { CopyMemory(this, &ini, sizeof(ini)); };

	//	MyRECT Get_left(int size) {
	//		MyRECT res;
	//		res = *this;
	//		res.right = res.left + size;
	//		Grop_left(size);
	//		return res;
	//	};
	//	MyRECT Get_right(int size) {
	//		MyRECT res;
	//		res = *this;
	//		res.left = res.right - size;
	//		Grop_right(size);
	//		return res;
	//	};
	//	MyRECT Get_top(int size) {
	//		MyRECT res;
	//		res = *this;
	//		res.bottom = res.top + size;
	//		Grop_top(size);
	//		return res;
	//	};
	//	MyRECT Get_bottom(int size) {
	//		MyRECT res;
	//		res = *this;
	//		res.top = res.bottom - size;
	//		Grop_bottom(size);
	//		return res;
	//	};

	//	inline MyRECT& Grop_left(int val) { left += val; return *this; };
	//	inline MyRECT& Grop_right(int val) { right -= val; return *this; };
	//	inline MyRECT& Grop_top(int val) { top += val; return *this; };
	//	inline MyRECT& Grop_bottom(int val) { bottom -= val; return *this; };

	//	int width() {
	//		return right - left;
	//	};
	//	int height() {
	//		return bottom - top;
	//	};
	//	operator RECT() { return (RECT)*this; };
	//}R(0,0, Width, Height), L, tmp;

	//R.Grop_top(26).Grop_left(5).Grop_right(5).Grop_bottom(5);

	//HDWP hdwp = BeginDeferWindowPos(9);

	//DeferWindowPos(hdwp, GetDlgItem(hwnd, 01), NULL, 0, 0, R.width(), 21, SWP_NOMOVE);//Верх
	//L = R.Get_bottom(35).Grop_top(5);// низ
	//auto el = GetDlgItem(hwnd, 1013);
	//GetClientRect(el, &tmp);
	//DeferWindowPos(hdwp, el, HWND_TOP, L.Grop_right(tmp.width()+5).right, L.top, 0, 0, SWP_NOSIZE);
	//el = GetDlgItem(hwnd, 501);
	//GetClientRect(el, &tmp);
	//DeferWindowPos(hdwp, el, HWND_TOP, L.Grop_right(tmp.width() + 5).right, L.top, 0, 0, SWP_NOSIZE);
	//el = GetDlgItem(hwnd, 502);
	//GetClientRect(el, &tmp);
	//DeferWindowPos(hdwp, el, HWND_TOP, L.Grop_right(tmp.width() + 5).right, L.top, 0, 0, SWP_NOSIZE);

	//L = R.Get_left(R.width() / 2).Grop_right(5);// левая половина
	//tmp = L.Get_top(L.height() / 3);
	//DeferWindowPos(hdwp, GetDlgItem(hwnd, 101), NULL, 0, 0, tmp.width(), tmp.height(), SWP_NOMOVE);
	//L.Get_top(5);
	//DeferWindowPos(hdwp, GetDlgItem(hwnd, 104), NULL, L.left, L.top, L.width(), L.height(), 0);

	//L = R;// правая половина
	//L= R.Get_top(L.height() / 2).Grop_bottom(5);
	//DeferWindowPos(hdwp, GetDlgItem(hwnd, 102), NULL, L.left, L.top, L.width(), L.height() , 0);
	//DeferWindowPos(hdwp, GetDlgItem(hwnd, 103), NULL, R.left, R.top, R.width(), R.height(), 0);

	//if (hdwp) EndDeferWindowPos(hdwp);
};//--------------------------------------------------------------------------------------------------

void KeyPressed(HWND hwnd, WORD key, WORD p2) {
}//--------------------------------------------------------------------------------------------------

#define elseif	else if
void OnClick1(HWND hwnd) {
	SmartStr f = 224;
};//--------------------------------------------------------------------------------------------------

void OnClick2(HWND hwnd) {
}//--------------------------------------------------------------------------------------------------

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CREATE:
		OnCreate(hwnd, ((LPCREATESTRUCT)lParam)->hInstance);
		break;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
	}break;
	case WM_COMMAND: {
		WORD hw(HIWORD(wParam)), lw(LOWORD(wParam));
		switch (lw) {
		case 1013:PostQuitMessage(0);
			break;
		case 501: OnClick1(hwnd);
			break;
		case 502: OnClick2(hwnd);
			break;
		default: { return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
		}
	}break;
	case WM_KEYDOWN:
		KeyPressed(hwnd, LOWORD(wParam), HIWORD(lParam));
		break;
	case WM_SIZE: 
		OnResize(hwnd, LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_GETMINMAXINFO:{// ограничения окна
		LPMINMAXINFO lpMMI  =  (LPMINMAXINFO)lParam;
		lpMMI->ptMinTrackSize.x  =  500;
		lpMMI->ptMinTrackSize.y  =  300;

		/*lpMMI->ptMaxTrackSize.x  =  0;
		lpMMI->ptMaxTrackSize.y  =  0;*/
	}break;
	case WM_DESTROY:
		PostQuitMessage(0);
	break;
	default: return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
};//--------------------------------------------------------------------------------------------------