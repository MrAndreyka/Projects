// Splitter_Wnd.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Splitter_Wnd.h"
#include "macros.h"
#include <commctrl.h>

#define MAX_LOADSTRING		100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HWND  g_hmain = NULL;


INT		main_Wnd_clr[3] = {236, 233, 216};
INT		left_Wnd_clr[3] = {255, 255, 255};
INT		right_Wnd_clr[3] = {255, 255, 255};;


// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	LeftWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	RightWndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;

	 InitCommonControls();
	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_SPLITTER_WND, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int) msg.wParam;
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	UINT ret_val;
	HBRUSH	hBrush = NULL;

	 // Resetting the structure members before use
    memset(&wcex, 0, sizeof(WNDCLASSEX));

	wcex.cbSize			= sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SPLITTER_WND));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);

	hBrush = CreateSolidBrush(RGB(main_Wnd_clr[0], 
			  					  main_Wnd_clr[1],
								  main_Wnd_clr[2]));


	wcex.hbrBackground	= (HBRUSH)hBrush;
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_SPLITTER_WND);
	wcex.lpszClassName	= MAIN_WINDOW_CLASS;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	 // Register the Window class with the OS
    ret_val = RegisterClassEx(&wcex);
    if(0 == ret_val)
    {
        return 0;
    }

	 // Intilaizing the class for the lefthand side window
    wcex.lpfnWndProc   = LeftWndProc;
	hBrush = CreateSolidBrush(RGB(left_Wnd_clr[0], 
			  					  left_Wnd_clr[1],
								  left_Wnd_clr[2]));
	wcex.hbrBackground = (HBRUSH)hBrush;
    wcex.lpszClassName = LEFT_WINDOW_CLASS;
    
    // Register the Window class with the OS
    ret_val = RegisterClassEx(&wcex);
    if(0 == ret_val)
    {
        return 0;
    }

    // Intilaizing the class for the righthand side window
    wcex.lpfnWndProc    = RightWndProc;
	hBrush = CreateSolidBrush(RGB(right_Wnd_clr[0], 
			  					  right_Wnd_clr[1],
								  right_Wnd_clr[2]));

    wcex.hbrBackground  = (HBRUSH)hBrush;
    wcex.lpszClassName  = RIGHT_WINDOW_CLASS;
    
     // Register the Window class with the OS
    ret_val = RegisterClassEx(&wcex);

	DeleteObject(hBrush);

    if(0 == ret_val)
    {
        return 0;
    }

	return ret_val;
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

    // Varible used to postion the window, center to the desktop
    UINT nx_size = GetSystemMetrics(SM_CXSCREEN);
    UINT ny_size = GetSystemMetrics(SM_CYSCREEN);
 
    UINT nx_pos = (nx_size - WINDOW_WIDTH) / 2;
    UINT ny_pos = (ny_size - WINDOW_HEIGHT) / 2;

   g_hmain = CreateWindow(MAIN_WINDOW_CLASS, "Splitter window with dynamic color changing",
							WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
							nx_pos, ny_pos, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);

   if (!g_hmain)
   {
	   UINT ret_val;

	   ret_val = GetLastError();
      return FALSE;
   }

   ShowWindow(g_hmain, nCmdShow);
   UpdateWindow(g_hmain);

   return TRUE;
}
