
#include "stdafx.h"
#include "resource.h"
#include "macros.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


HWND	g_hrightwnd	= NULL;
HWND	g_hleftwnd	= NULL;


HWND	hText		= NULL;
extern	HINSTANCE	hInst;
extern	HWND		g_hmain;


extern INT		main_Wnd_clr[];
extern INT		left_Wnd_clr[]; 
extern INT		right_Wnd_clr[];


// Function to handle the messages releated to popup window
INT_PTR CALLBACK PopUpProc(HWND , UINT, WPARAM, LPARAM);

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	PAINTSTRUCT ps;
	HDC hdc;
	static HBRUSH	hmain_Wnd_brush	= NULL;
	static HWND		hclose_button	= NULL;
	static HWND		hcolor_button	= NULL;
	static HCURSOR	hcSizeEW		= NULL;
	static HWND	focus = NULL;

	// Varibles used to calculate the width and height of the 
    // left window while increased or decreased using splitter
    static  int         nleftWnd_width = 0;

	 // Variable which informs whether the splitter is dragged
    // or not
    static  BOOL        xSizing;
	static UINT			nwnd_resizing = CLEAR;

	static HBRUSH	hBrush;
	static HICON	hIcon;

	switch (message)
	{
	case WM_CREATE:

		hcSizeEW = LoadCursor(NULL, IDC_SIZEWE);

		nleftWnd_width = LEFT_WINDOW_WIDTH;		

		GetClientRect(hWnd, &rect);
		// Creates the left window using the width and height read from the XML 
		// files
		g_hleftwnd = CreateWindowEx(WS_EX_CLIENTEDGE, LEFT_WINDOW_CLASS, 
										"", 
										WS_CHILD | WS_VISIBLE | SS_SUNKEN,
										rect.left, 
										rect.top + TOP_POS,
										LEFT_WINDOW_WIDTH, 
										(rect.bottom - rect.top) - (TOP_POS + BOTTOM_POS),
										hWnd, NULL, hInst, NULL);
		if(NULL != g_hleftwnd)
		{
			ShowWindow(g_hleftwnd, SW_SHOW);
			UpdateWindow(g_hleftwnd);
		}

		hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));

		if (hIcon != NULL)
		{
				SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
		}

		// Creates the right window using the width and height read from the XML
		// files
		g_hrightwnd = CreateWindowEx(WS_EX_CLIENTEDGE, RIGHT_WINDOW_CLASS,
									 "", 
									 WS_CHILD | WS_VISIBLE | SS_SUNKEN,
									 rect.left + LEFT_WINDOW_WIDTH + SPLITTER_BAR_WIDTH,
									 rect.top + TOP_POS,
									 rect.right - (rect.left + LEFT_WINDOW_WIDTH + SPLITTER_BAR_WIDTH),
									 (rect.bottom - rect.top) - (TOP_POS + BOTTOM_POS),
									 hWnd,
									 NULL, hInst, NULL);

		if(NULL != g_hrightwnd)
		{
			ShowWindow(g_hrightwnd, SW_SHOW);
			UpdateWindow(g_hrightwnd);
		}
		char name[50];
		GetClassName(g_hrightwnd, name, 50);
		SetWindowText(hWnd, name);

		hText = CreateWindow("Static", "",
							WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE | SS_ENDELLIPSIS, 
							rect.left, rect.top, rect.right, STATIC_TEXT_HEIGHT, 
							hWnd, (HMENU)NULL, hInst, NULL);

		//Create the buttons in the main window
		hclose_button = CreateWindow(BUTTON_CLASS, "",
                                    WS_CHILD | WS_VISIBLE | 
                                    WS_TABSTOP | BS_PUSHBUTTON,
									rect.right - SPACE_BUTTON_RIGHT, 
									rect.bottom - SPACE_BUTTON_BOTTOM,
                                    CLOSE_BUTTON_WIDTH,
                                    BUTTON_HEIGHT,  
                                    hWnd, NULL,hInst, NULL);
		if(NULL != hclose_button)
		{
			SetWindowText(hclose_button, "Close");
			ShowWindow(hclose_button, SW_SHOW);
			UpdateWindow(hclose_button);
		}

		hcolor_button = CreateWindow(BUTTON_CLASS, "",
                                    WS_CHILD | WS_VISIBLE | 
                                    WS_TABSTOP | BS_PUSHBUTTON,
									rect.right - (SPACE_BUTTON_RIGHT + COLOR_BUTTON_WIDTH + BUTTON_ADJUST), 
									rect.bottom - SPACE_BUTTON_BOTTOM,
                                    COLOR_BUTTON_WIDTH,
                                    BUTTON_HEIGHT,  
                                    hWnd, NULL,hInst, NULL);
		if(NULL != hcolor_button)
		{
				SetWindowText(hcolor_button, "Click To Change Window Color");
				ShowWindow(hcolor_button, SW_SHOW);
				UpdateWindow(hcolor_button);

				SendMessage(hcolor_button, WM_SETFOCUS, 0, 0);
				focus = hcolor_button;
		}

		return FALSE;
		break;


	case WM_CTLCOLORSTATIC :
		
			//Set the text color inverse of the window color
			SetTextColor((HDC)wParam, RGB(255 - main_Wnd_clr[0], 
										255 - main_Wnd_clr[1],
										255 - main_Wnd_clr[2]));

			SetBkColor((HDC)wParam, RGB(main_Wnd_clr[0], 
									main_Wnd_clr[1],
									main_Wnd_clr[2]));

			if (hBrush != NULL)
			{
					DeleteObject(hBrush);
			}
			hBrush = CreateSolidBrush(RGB(main_Wnd_clr[0], 
										main_Wnd_clr[1],
										main_Wnd_clr[2]));
			return (BOOL)hBrush;
		
		break;


	case WM_SIZE:

		GetClientRect(hWnd, &rect);
		  
		// Call Api to move and adjust the left window postion and its
        // height and width

        MoveWindow(g_hleftwnd, rect.left,
				   rect.top + TOP_POS,
                   rect.left + (nleftWnd_width - WIDTH_ADJUST), 
				   (rect.bottom - (TOP_POS + BOTTOM_POS)),
                   FALSE);
        
        // Call Api to move and adjust the right window postion and its
        // height and width
        MoveWindow(g_hrightwnd, rect.left + nleftWnd_width + WIDTH_ADJUST, 
                      rect.top + TOP_POS,
                      rect.right - (nleftWnd_width + WIDTH_ADJUST),
                      rect.bottom - (TOP_POS + BOTTOM_POS),
                      FALSE);	

		MoveWindow(hclose_button, rect.right - SPACE_BUTTON_RIGHT,
				   rect.bottom - SPACE_BUTTON_BOTTOM, 
				   CLOSE_BUTTON_WIDTH, BUTTON_HEIGHT, FALSE);


		MoveWindow(hcolor_button, 
					rect.right - (SPACE_BUTTON_RIGHT + COLOR_BUTTON_WIDTH + BUTTON_ADJUST), 
					rect.bottom - SPACE_BUTTON_BOTTOM, 
					COLOR_BUTTON_WIDTH, BUTTON_HEIGHT, FALSE);


		MoveWindow(hText, rect.left, rect.top, rect.right, STATIC_TEXT_HEIGHT, FALSE);

		InvalidateRect(hWnd, &rect, TRUE);

		break;

	case WM_COMMAND:

		if(hclose_button == (HWND)lParam)
		{
			DeleteObject(hBrush);
			DestroyWindow(hWnd);
			DeleteObject(hcSizeEW);
		}
		else if(hcolor_button == (HWND)lParam)
		{
			DialogBox(hInst, MAKEINTRESOURCE(IDD_POPUP), hWnd,
									(DLGPROC)PopUpProc); 
		}

		break;

	case WM_KEYDOWN :

		switch(wParam)
		{
			case VK_TAB :
				if (focus == hcolor_button) 
				{					
					SendMessage(focus, WM_KILLFOCUS, 0, 0);
					SendMessage(hclose_button, WM_SETFOCUS, 0, 0);

					focus = hclose_button;
				}
				else if (focus == hclose_button)
				{
					SendMessage(focus, WM_KILLFOCUS, 0, 0);
					SendMessage(hcolor_button, WM_SETFOCUS, 0, 0);


					focus = hcolor_button;
				}


			break;
			
			case VK_RETURN:

				if (IsWindowEnabled(focus))
					SendMessage(hWnd, WM_COMMAND, 0, (LPARAM)focus);

			break;	
		}
		break;


	case WM_PAINT:

		SetWindowText(hText, "To change the color of the window click on \"Click To Change Window Color\" button");

		hdc = BeginPaint(hWnd, &ps);

		// Painting 
		GetClientRect(hWnd, &rect);
		
		hmain_Wnd_brush = CreateSolidBrush(RGB(main_Wnd_clr[0], 
												   main_Wnd_clr[1],
												   main_Wnd_clr[2]));

		FillRect(hdc, &rect, hmain_Wnd_brush);

		DeleteObject(hmain_Wnd_brush);

		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);

		break;

	// Case statement to handle the left mouse button down message
    // received while the mouse left button is down
   case WM_LBUTTONDOWN :
        {
            int                 xPos;
            int                 yPos;
            
            // Varible used to get the mouse cursor x and y co-ordinates
            xPos = (int)LOWORD(lParam);
            yPos = (int)HIWORD(lParam);

            // Checks whether the mouse is over the splitter window
            xSizing = (xPos > nleftWnd_width - SPLITTER_BAR_WIDTH && xPos < nleftWnd_width + SPLITTER_BAR_WIDTH );

            // If the mouse is over the splitter then set mouse cursor 
            // image to IDC_SIZEWE which helps the user to drag the window
            if(xSizing)
            {
                // Api to capture mouse input
                SetCapture(hWnd);
                if(xSizing)
                {
                    SetCursor(hcSizeEW);
                }
             }
        }
     break;

    case WM_LBUTTONUP :
        if(xSizing)
        {
            RECT    focusrect;
            HDC     hdc;

            // Releases the captured mouse input
            ReleaseCapture();
            // Get the main window dc to draw a focus rectangle
            hdc = GetDC(hWnd);
            GetClientRect(hWnd, &rect);
            if(xSizing)
            {
                SetRect(&focusrect, nleftWnd_width - (WIDTH_ADJUST * 2), rect.top + TOP_POS, 
					    nleftWnd_width + WIDTH_ADJUST, 
						rect.bottom - 80);

                // Call api to vanish the dragging rectangle 
                DrawFocusRect(hdc, &focusrect);
				
				xSizing = FALSE;

            }
            // Release the dc once done 
            ReleaseDC(hWnd, hdc);
        }
        // Post a WM_SIZE message to redraw the windows
        PostMessage(hWnd, WM_SIZE, 0, 0);

    break;

    case WM_MOUSEMOVE :
    {
		int   xPos;
		int   yPos;

		// Get the x and y co-ordinates of the mouse
		xPos = (int)LOWORD(lParam);
		yPos = (int)HIWORD(lParam);

		if (xPos < LEFT_MINIMUM_SPACE || xPos > RIGHT_MINIMUM_SPACE)
		{
			return 0;
		}

        // Checks if the left button is pressed during dragging the splitter
        if(wParam == MK_LBUTTON)   
        {
            // If the window is d`agged using the splitter, get the
            // cursors current postion and draws a focus rectangle 
            if(xSizing)
            {
                RECT    focusrect;
                HDC     hdc;

                hdc = GetDC(hWnd);
                GetClientRect(hWnd, &rect);

                if(xSizing)
                {
                   SetRect(&focusrect, nleftWnd_width - (WIDTH_ADJUST * 2), rect.top + TOP_POS,
                               nleftWnd_width +  WIDTH_ADJUST, 
							   rect.bottom - BOTTOM_POS);

                   DrawFocusRect(hdc, &focusrect);

                   // Get the size of the left window to increase
                   nleftWnd_width = xPos;

                   // Draws a focus rectangle
                   SetRect(&focusrect, nleftWnd_width - (SPLITTER_BAR_WIDTH * 2), rect.top + 80,
                              nleftWnd_width + SPLITTER_BAR_WIDTH, 
							  rect.bottom - BOTTOM_POS);

                   DrawFocusRect(hdc, &focusrect);

                }
                ReleaseDC(hWnd, hdc);
            }
        }
        // Set the cursor image to east west direction when the mouse is over 
        // the splitter window
        if( (xPos > nleftWnd_width - SPLITTER_BAR_WIDTH && xPos < nleftWnd_width + 
               SPLITTER_BAR_WIDTH))
        {
             SetCursor(hcSizeEW); 
        }
    }
    break;

	case WM_DESTROY:

		if(NULL != hmain_Wnd_brush)
		{
			DeleteObject(hmain_Wnd_brush);
		}
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}



LRESULT CALLBACK LeftWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rect;
	static HBRUSH	hleft_Wnd_brush = NULL;
	
	switch (message)
	{

	case WM_CREATE :

		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		GetClientRect(hWnd, &rect);

		//Paint the window with the color specified by the user
		hleft_Wnd_brush = CreateSolidBrush(RGB(left_Wnd_clr[0], 
												   left_Wnd_clr[1],
												   left_Wnd_clr[2]));

		FillRect(hdc, &rect, hleft_Wnd_brush);

		DeleteObject(hleft_Wnd_brush);
		
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK RightWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rect;
	static HBRUSH	hright_Wnd_brush = NULL; 
	
	HINSTANCE	hInst;
	static HCURSOR 	hCursor;
	static BOOL	bSplitterMoving;
	static DWORD	dwSplitterPos;
	static HWND	hWnd1, hWnd2;

	switch (message)
	{
		case WM_CREATE:
			hInst = ((LPCREATESTRUCT)lParam)->hInstance;

			hWnd1 = CreateWindowEx(WS_EX_CLIENTEDGE,
				"edit", NULL,
				WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_VSCROLL| SS_SUNKEN | WS_CLIPCHILDREN,
				0, 0, 0, 0,
				hWnd, (HMENU)1,
				hInst, NULL);

			hWnd2 = CreateWindowEx(WS_EX_CLIENTEDGE,
				"edit", NULL,
				WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_VSCROLL| SS_SUNKEN | WS_CLIPCHILDREN,
				0, 0, 0, 0,
				hWnd, (HMENU)2,
				hInst, NULL);


			hCursor = LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS));
			bSplitterMoving = FALSE;

			dwSplitterPos = 130;
			return 0;


		case WM_SIZE:
			/*
			If window is shrunk so that splitter now lies outside the
			window boundaries, move the splitter within the window.
			*/
			if ((wParam != SIZE_MINIMIZED) && (HIWORD(lParam) < dwSplitterPos))
				dwSplitterPos = HIWORD(lParam) - 10;

			/* Adjust the children's size and position */
			MoveWindow(hWnd1, 0, 0, LOWORD(lParam), dwSplitterPos - 1, TRUE);
			MoveWindow(hWnd2, 0, dwSplitterPos + 2, LOWORD(lParam), HIWORD(lParam) - dwSplitterPos - 2, TRUE);
			return 0;


		case WM_MOUSEMOVE:
			if (HIWORD(lParam) > 10) // do not allow above this mark
			{
				SetCursor(hCursor);
				if ((wParam == MK_LBUTTON) && bSplitterMoving)
				{
					GetClientRect(hWnd, &rect);
					if (HIWORD(lParam) > rect.bottom)
						return 0;

					dwSplitterPos = HIWORD(lParam);
					auto wlp = MAKELPARAM(rect.right, rect.bottom);
					SendMessage(hWnd, WM_SIZE, 0, MAKELPARAM(rect.right, rect.bottom));
				}
			}
			return 0;
		case WM_LBUTTONDOWN:
			SetCursor(hCursor);
			bSplitterMoving = TRUE;
			SetCapture(hWnd);
			return 0;


		case WM_LBUTTONUP:
			ReleaseCapture();
			bSplitterMoving = FALSE;
			return 0;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		GetClientRect(hWnd, &rect);

		//Paint the window with the color specified by the user
		hright_Wnd_brush = CreateSolidBrush(RGB(right_Wnd_clr[0], 
													right_Wnd_clr[1],
													right_Wnd_clr[2]));

		FillRect(hdc, &rect, hright_Wnd_brush);

		DeleteObject(hright_Wnd_brush);

		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


INT_PTR CALLBACK PopUpProc(HWND hpopup, 
                           UINT message, 
                           WPARAM wParam, 
                           LPARAM lParam)
{
	WORD wmId;
	RECT rect;
	static HWND hComboBox[9];
	char str[5];
	static int j;

    switch(message)
    {
        case WM_INITDIALOG:

			SetWindowText(hText, "Changing the RGB values will change the color of the windows");

			UINT iLoop, i;

			//Create the handler for all the combo boxes
			for(i = 0; i < 9; i++)
			{
				hComboBox[i] = GetDlgItem(hpopup, IDC_COMBO1 + i);
			}

			//Set the range for the combo boxes(in this case 0 - 255)
			if(NULL != hComboBox)
			{
				for(i = 0 ; i < 9 ; i++)
				{
					for (iLoop = 0; iLoop < 256; iLoop++)
					{
					  sprintf(str, "%d", iLoop);
					  SendMessage(hComboBox[i], CB_ADDSTRING, 0, (LPARAM) str); 
					}
				}
			}

			//Set the combo box text with the previous value for window and child windows
			for (i = 0, j = 0; i < 3; i++)
			{
					sprintf(str, "%d", main_Wnd_clr[i]);
					SendMessage(hComboBox[j++], WM_SETTEXT, 0, (LPARAM) str); 
			}

			for (i = 0; i < 3; i++)
			{
					sprintf(str, "%d", left_Wnd_clr[i]);
					SendMessage(hComboBox[j++], WM_SETTEXT, 0, (LPARAM) str); 
			}

			for (i = 0; i < 3; i++)
			{
					sprintf(str, "%d", right_Wnd_clr[i]);
					SendMessage(hComboBox[j++], WM_SETTEXT, 0, (LPARAM) str); 
			}


			SendMessage(GetDlgItem(hpopup, IDC_COMBO1), WM_SETFOCUS, 0, 0);

			ShowWindow(hpopup, SW_SHOW);
			UpdateWindow(hpopup);
			break;


		case WM_COMMAND:
			wmId = LOWORD(wParam);

			//When submit button is pressed take the value and paint the 
			//window with the user given value

			if(IDOK == wmId)
			{
				UINT j = 0;
				for(UINT i = 0; i < 9; i++)
				{
					char buff[5];

					GetWindowText(hComboBox[i], buff, 5);


					if(i < 3)
					{
						main_Wnd_clr[j++] = atoi(buff);
					}
					else if(i >= 3 && i < 6)
					{
						if(i == 3)
						{
							j = 0;
						}
						left_Wnd_clr[j++] = atoi(buff);
					}
					else if(i >= 6 && i < 9)
					{
						if(i == 6)
						{
							j = 0;
						}
						right_Wnd_clr[j++] = atoi(buff);
					}
				}

				GetClientRect(g_hmain, &rect);
				InvalidateRect(g_hmain, &rect, TRUE);

				GetClientRect(g_hrightwnd, &rect);
				InvalidateRect(g_hrightwnd, &rect, TRUE);

				GetClientRect(g_hleftwnd, &rect);
				InvalidateRect(g_hleftwnd, &rect, TRUE);

				EndDialog(hpopup, 0);
			}

			break;

		case WM_CLOSE :
		case WM_DESTROY :
					EndDialog(hpopup, 0);

			break;

	}

	return FALSE;
}