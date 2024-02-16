#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
#include <crtdbg.h>
// Выше для утечек

/**/
//#include <list>
//#include <iostream>
#include <windows.h>
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")
#include <Tchar.h>

//#include <atlstr.h>

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
#include "..//..//Iincludes//Class.h"
#include "..//..//Iincludes//WinCom.h"
#include "..//..//Iincludes//Rect_Point.h"
//#include "..//..//Bpl//BrowseDialog.h"
#include "resource.h"
//#include "afxres.h" // если используются формы в ресурсах

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

ContSpliter* SpliterP;

ContSpliter* retsplit(HWND hwnd) {return SpliterP; }

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

	HWND hwnd = CreateWindow(Cl.lpszClassName, _T("for SE"), WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX | WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hwnd) { MessageBox(NULL, _T("Не создано окно!"), _T("Ошибка"), MB_OK); return 1; }

	auto defstyle = WS_CLIPSIBLINGS | WS_BORDER | WS_CLIPCHILDREN | WS_VISIBLE | WS_CHILD;
	auto styleEx = WS_EX_STATICEDGE | WS_EX_CLIENTEDGE;
	auto style = defstyle | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | ES_WANTRETURN | ES_MULTILINE;

	ContSpliter::ProcGet = retsplit;
	ContSpliter::Reg_Class(hInstance);
	
	HWND item = CreateWindow(WC_CONTSPLITER, NULL, defstyle, 0, 0, 400, 400, hwnd, (HMENU)01, hInstance, NULL),
		item2 = CreateWindow(WC_STATIC, NULL, defstyle, 5, 5, 250, 150, item, (HMENU)02, hInstance, NULL);
	CreateWindow(WC_STATIC, NULL, defstyle, 0, 200, 250, 150, item, (HMENU)03, hInstance, NULL);

	if (item == nullptr)
		ShowMessage(TextError(GetLastError()).c_str(), 0, 0, hwnd);

	//CreateWindowEx(styleEx, WC_EDIT, NULL, style, 5, 5, 180, 80, item2, (HMENU)101, hInst, NULL);
	//CreateWindowEx(styleEx, WC_EDIT, NULL, style, 5, 100, 180, 80, item2, (HMENU)102, hInst, NULL);


	//ContSpliter(item, CreateWindowEx(styleEx, WC_EDIT, NULL, style, 100, 0, 80, 80, item, (HMENU)102, hInst, NULL),
	//	CreateWindowEx(styleEx, WC_EDIT, NULL, style, 0, 300, 80, 80, item, (HMENU)104, hInst, NULL), true, 0);

	auto wp = GetWindowLong(item, GWLP_WNDPROC);

	auto Spliter = ContSpliter_vert(item, item2,
		CreateWindow(WC_STATIC, NULL, defstyle, 0, 200, 250, 150, item, (HMENU)03, hInstance, NULL));
	SpliterP = &Spliter;

	wp = GetWindowLong(item, GWLP_WNDPROC);
	auto wp2 = SetWindowLong(item, GWLP_WNDPROC, (LONG)&WindowProc);
	wp = GetWindowLong(item, GWLP_WNDPROC);

	//SetSplitter(item, CreateWindowEx(styleEx, WC_EDIT, NULL, style, 0, 100, 80, 80, item, (HMENU)101, hInst, NULL), item2, false);
	//SetSplitter(item2, CreateWindowEx(styleEx, WC_EDIT, NULL, style, 100, 0, 80, 80, item2, (HMENU)102, hInst, NULL),
	//	CreateWindowEx(styleEx, WC_EDIT, NULL, style, 0, 300, 80, 80, item2, (HMENU)104, hInst, NULL), true);*/

	//CreateWindow(WC_BUTTON, _T("Go"), defstyle, 160, 500, 150, 30, hwnd, (HMENU)501, hInst, nullptr);
	//CreateWindow(WC_BUTTON, _T("quick_exit"), defstyle, 5, 500, 150, 30, hwnd, (HMENU)502, hInst, nullptr);

	UnregisterClass(Cl.lpszClassName, hInstance);
	ShowWindow(hwnd, nCmdShow);

	MSG Mes;
	while (GetMessage(&Mes, NULL, 0, 0)) {
		TranslateMessage(&Mes);
		DispatchMessage(&Mes);
	}

	return Mes.wParam;
};//--------------------------------------------------------------------------------------------------

void OnCreate(HWND hwnd, HINSTANCE hInst) {
};//--------------------------------------------------------------------------------------------------

void OnResize(HWND hwnd, int Width, int Height) {
	MyRECT L, tmp, R(0, 0, Width, Height);

	//R.Grop_top(26).Grop_left(5).Grop_right(5).Grop_bottom(5);

	/*L = R.Cut_bottom(35).Grop_top(5);// низ
	SetWindowPos(GetDlgItem(hwnd, 01), NULL, 0, 0, R.GetWidth(), R.GetHeight() * 0.85, SWP_NOMOVE);//Верх

	auto el = GetDlgItem(hwnd, 501);
	GetClientRect(el, &tmp);
	tmp = L.Cut_right(tmp.GetWidth());
	SetWindowPos(el, HWND_TOP, tmp.left, tmp.top, 0, 0, SWP_NOSIZE);*/
	return;

	HDWP hdwp = BeginDeferWindowPos(3);
	
	L = R.Cut_bottom(40).Grop(5,5,5,5);// низ
	hdwp = DeferWindowPos(hdwp, GetDlgItem(hwnd, 01), NULL, 0, 0, R.GetWidth(), R.GetHeight(), SWP_NOMOVE); _ASSERTE(hdwp != 0);//Верх
	R.Grop(5, 5, 5, 6);
	
	auto el = GetDlgItem(hwnd, 501);
	GetClientRect(el, &tmp);
	tmp = L.Cut_right(tmp.GetWidth());
	hdwp = DeferWindowPos(hdwp, el, HWND_TOP, tmp.left, tmp.top, 0, 0, SWP_NOSIZE); _ASSERTE(hdwp != 0);
	tmp.ShiftH(-tmp.GetWidth() - 5);
	hdwp = DeferWindowPos(hdwp, GetDlgItem(hwnd, 502), HWND_TOP, tmp.left, tmp.top, 0, 0, SWP_NOSIZE); _ASSERTE(hdwp != 0);
	if (hdwp) EndDeferWindowPos(hdwp);

	//R.Moveto(0, 0);
	//L = R.Cut_left(R.GetWidth() / 2).Grop_right(3);//левая половина
	//R.Grop_left(6);
	L = R;
		
	tmp = L.Cut_bottom(L.GetHeight() / 2 - 3, 5);// .Grop_top(2);//Низ
	//L.Grop_bottom(5);//ВерхЛев
	
	hdwp = BeginDeferWindowPos(2);
	hdwp = DeferWindowPos(hdwp, el = GetDlgItem_fromDlg(hwnd, 01, 02), HWND_TOP, L.left, L.top, L.GetWidth(), L.GetHeight(), 0); _ASSERTE(hdwp != 0);
	hdwp = DeferWindowPos(hdwp, GetDlgItem_fromDlg(hwnd, 01, 03), HWND_TOP, tmp.left, tmp.top, tmp.GetWidth(), tmp.GetHeight(), 0); _ASSERTE(hdwp != 0);
	if (hdwp) EndDeferWindowPos(hdwp);

	
	
	L.Moveto(0, 0);
	R = L.Cut_right(L.GetWidth() / 2 - 3, 5);//Верх
	R.Grop_right(6);//Низ
	
	hdwp = BeginDeferWindowPos(2);
	hdwp = DeferWindowPos(hdwp, GetDlgItem(el, 101), HWND_TOP, L.left, L.top, L.GetWidth(), L.GetHeight(), 0); _ASSERTE(hdwp != 0);
	hdwp = DeferWindowPos(hdwp, GetDlgItem(el, 102), HWND_TOP, R.left, R.top, R.GetWidth(), R.GetHeight(), 0); _ASSERTE(hdwp != 0);
	if (hdwp) EndDeferWindowPos(hdwp);
};//--------------------------------------------------------------------------------------------------

void KeyPressed(HWND hwnd, WORD key, WORD p2) {
}//--------------------------------------------------------------------------------------------------

#define elseif	else if
void OnClick1(HWND hwnd) {
	SmartStr f = sizeof(ContSpliter);
};//--------------------------------------------------------------------------------------------------

void OnClick2(HWND hwnd) {
	quick_exit(1);
}//--------------------------------------------------------------------------------------------------

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_WINDOWPOSCHANGING: 
	case WM_WINDOWPOSCHANGED:
	{
		auto wp = (WINDOWPOS*)lParam;
		if (hwnd != wp->hwnd) {
			auto s = GetTextS(wp->hwnd);
		}
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	
	}
		break;
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