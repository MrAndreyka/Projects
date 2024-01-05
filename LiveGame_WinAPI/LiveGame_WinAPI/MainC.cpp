#include "mainC.h"
#include <list>
#include <format>

#include "..\..\Iincludes\class.h"
#include "..\..\Iincludes\wincom.h"

#include "..\..\Iincludes\Rect_Point.h"

template <class T>
class world {
private:
	T **field, **copy = nullptr;
	SIZE sz; 

	T** GetArray() {
		T** res = new T * [sz.cx];
		for (auto i = 0; i < sz.cx; i++) {
			res[i] = new T[sz.cy];
			ZeroMemory(res[i], sz.cy);
		}
		return res;
	}
	void freeArrays() {
		if (field != nullptr) {
			for (auto i = 0; i < sz.cx; i++)
				delete[] field[i];
			delete[]field;
		}
		if (copy != nullptr) {
			for (auto i = 0; i < sz.cx; i++)
				delete[] copy[i];
			delete[]copy;
			copy = nullptr;
		}
	}

public:
	inline world() { sz.cx = 0; sz.cy = 0; field = nullptr; };
	world(LONG xsize, LONG ysize) {
		if(!init(xsize, ysize)) world();
	}

	~world() {freeArrays(); };

	bool init(LONG xsize, LONG ysize) {
		if (xsize == 0 || ysize == 0) return false; 
		freeArrays();
		sz.cx = xsize;
		sz.cy = ysize;
		field = GetArray();
		return true;
	}

	pair<u_char, u_char> neighbours(u_int x, u_int y) {
		auto res = pair<u_char, T>(0,0);
		for (int xi = - 1; xi < 2; xi++)
			for (int yi = -1; yi < 2; yi++) {
				auto tmp = GetVal((xi + x) % sz.cx, (yi + y) % sz.cy);
				if (tmp > 0 && !(xi == 0 && yi == 0)) {
					res.first++;
					res.second += tmp;
				}
			}
		return res;
	}

	void NextStep() {
		if (copy == nullptr)
			copy = GetArray();

		for (auto x = 0; x < sz.cx; x++) {
			ZeroMemory(copy[x], sz.cy);
			for (auto y = 0; y < sz.cy; y++) {
				auto cn = neighbours(x, y);
				if (field[x][y] > 0 && (cn.first < 2 || cn.first > 3))
					copy[x][y] = 0;
				else if (field[x][y] == 0 && cn.first == 3)
					copy[x][y] = cn.second / cn.first;
				else
					copy[x][y] = field[x][y];
			}
		}
		
		auto tmp = field;
		field = copy;
		copy = tmp;	
	}

	inline void SetVal(LONG x, LONG y, T Val, bool cicle = false) {
		(cicle? field[x%sz.cx][y % sz.cy] : field[x][y]) = Val; }
	inline T& GetVal(LONG x, LONG y) { return field[x][y]; }

	inline const T ** GetField() const { return (const u_char**)field; }
	inline const LONG GetHeigth() const { return sz.cy; }
	inline const LONG GetWidth() const { return sz.cx; }
};

HDC secondHdc;
HWND hWnd;
POINT ClSize, pos;
BOOL pause = true;
u_char multiple = 10, tecCol = 1;
POINT winSize;
world<u_char> myg;

//auto bRed = CreateSolidBrush(RGB(0xff, 0, 0));
auto pNull = CreatePen(PS_NULL, 0, 0);

COLORREF cols[]{ RGB(0xff, 0, 0), RGB(0, 0xff, 0), RGB(0,0, 0xff), RGB(0, 0xff, 0xff), RGB(0xff, 0xff, 0), RGB(0xff, 0, 0xff), RGB(0xff, 0xff, 0xff) };

//auto ElAct = list<pair<int, wstring>>();
void OnPaint();

void ShowWin()
{
	SelectObject(secondHdc, GetStockObject(NULL_PEN));
	SelectObject(secondHdc, GetStockObject(BLACK_BRUSH));
	Rectangle(secondHdc, 0, 0, winSize.x * multiple+1, winSize.y * multiple+1);

	SelectObject(secondHdc, GetStockObject(DC_PEN));
	SelectObject(secondHdc, GetStockObject(DC_BRUSH));
	//SelectObject(secondHdc, bRed);

	int xc, yc;

	auto f = myg.GetField();
	if (f != nullptr) {
		auto w = myg.GetWidth(), h = myg.GetHeigth();
		for (auto i = 0; i < winSize.x; i++)
			for (auto j = 0; j < winSize.y; j++) {

				xc = i + pos.x;
				yc = j + pos.y;

				auto tv = f[xc % w][yc % h];
				if (tv == 0) continue;
				SetDCBrushColor(secondHdc, cols[tv - 1]);
				Rectangle(secondHdc, (i * multiple), (j * multiple), (i * multiple) + multiple - 1, (j * multiple) + multiple - 1);
			}
	}

	if (pos.x + winSize.x > myg.GetWidth()) {
		xc = (myg.GetWidth() - pos.x) * multiple;
		MoveToEx(secondHdc, xc, 0, NULL);
		LineTo(secondHdc, xc, winSize.y * multiple);
	}

	if (pos.y + winSize.y > myg.GetHeigth()) {
		yc = (myg.GetHeigth() - pos.y) * multiple;
		MoveToEx(secondHdc, 0, yc, NULL);
		LineTo(secondHdc, winSize.x * multiple, yc);
	}

	InvalidateRect(hWnd, NULL, true);
	//UpdateWindow(hWnd);
	//OnPaint();
}

void ShowInfo()
{
	MyRECT r (winSize.x * multiple, ClSize.y - 40, ClSize.x - 10, ClSize.y - 10);
	
	SelectObject(secondHdc, GetStockObject(BLACK_BRUSH));

	Rectangle(secondHdc, r.left, 0, ClSize.x, ClSize.y);

	SelectObject(secondHdc, GetStockObject(DKGRAY_BRUSH));
	SetBkMode(secondHdc, TRANSPARENT);
	SetTextColor(secondHdc, RGB(0xff, 0xff, 0xff));

	r.SET(winSize.x * multiple + 2, 5, ClSize.x - 2, 25);
	SetTextColor(secondHdc, RGB(0x80,0x80,0x80));

	wstring s = L"Текущий цвет:";
	
	SelectObject(secondHdc, GetStockObject(DC_PEN));
	SetDCBrushColor(secondHdc, cols[tecCol - 1]);
	FillRect(secondHdc, &(r.Copy(r.GetWidth() - 30, 0, 0, 0).ShiftH(-5)), (HBRUSH)GetStockObject(DC_BRUSH));
	DrawText(secondHdc, s.c_str(), s.length(), &r, DT_SINGLELINE | DT_VCENTER);

	r.ShiftV(45);
	s = L"1-7: Выбор цвета";
	DrawText(secondHdc, s.c_str(), s.length(), &r, 0);
	
	r.ShiftV(20);
	s = L"Enter: Авто (Вкл/Выкл)";
	DrawText(secondHdc, s.c_str(), s.length(), &r, DT_SINGLELINE);
	
	r.ShiftV(20);
	s = L"Space: След.итерация";
	DrawText(secondHdc, s.c_str(), s.length(), &r, DT_SINGLELINE);

	InvalidateRect(hWnd, NULL, true);
}

void ActionWin(ULONG cou){
	static LONG ct = 0; ct += cou;

	
	while(ct > 50) {
		myg.NextStep(); 
		ct -= 50;
	}

	//printf(" /  %d\n", count);
	//string s = to_string(X) + " :: " + to_string(x) + " : " + to_string(y) + " ( " + to_string(v.x) + " : " + to_string(v.y) + ") / " + to_string(count);
	//SetWindowText(hWnd, s.c_str());
}

void newWorld() {
	myg.init(180, 100);
	pos.y = pos.x = 0;
}

void OutputInfo(HWND hWnd) {
	wstring s = MultiStr(12, L"Pos:", to_wstring(pos.x).c_str(), L"x", to_wstring(pos.y).c_str(),
		L"\tSize:", to_wstring(winSize.x).c_str(), L"x", to_wstring(winSize.y).c_str(),
		L"\tWin:", to_wstring(ClSize.x).c_str(), L"x", to_wstring(ClSize.y).c_str());
	SetWindowText(hWnd, s.c_str());

	ShowInfo();
	ShowWin();
}

void chek_pos(HWND hWnd)
{
	if(pos.x < 0) pos.x = myg.GetWidth() + pos.x;
	else if (pos.x >= myg.GetWidth()) pos.x = 0;
	 
	if (pos.y < 0) pos.y = myg.GetHeigth() + pos.y;
	else if (pos.y >= myg.GetHeigth()) pos.y = 0;

	OutputInfo(hWnd);
}

void resize() {
	winSize.x = (ClSize.x - 150) / multiple;
	if (myg.GetWidth() < winSize.x)
		winSize.x = myg.GetWidth();
	winSize.y = ClSize.y / multiple;
	if (myg.GetHeigth() < winSize.y)
		winSize.y = myg.GetHeigth();

	HBITMAP bMap = CreateCompatibleBitmap(GetDC(hWnd), ClSize.x, ClSize.y);
	DeleteObject(SelectObject(secondHdc, bMap));

	OutputInfo(hWnd);

	SetWindowPos(GetDlgItem(hWnd, 101), NULL, ClSize.x - 140, ClSize.y - 30, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE);
	SetWindowPos(GetDlgItem(hWnd, 102), NULL, ClSize.x - 140, ClSize.y - 60, 0, 0, SWP_NOOWNERZORDER | SWP_NOSIZE);
}


void OnPaint() {
	PAINTSTRUCT ps;
	auto hdc = BeginPaint(hWnd, &ps);
	BitBlt(hdc, 0, 0, ClSize.x, ClSize.y, secondHdc, 0, 0, SRCCOPY);
	EndPaint(hWnd, &ps);
}

LRESULT WINAPI WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	//printf("%d	%d	%d\n", Msg, wParam, lParam);

	static POINT prevPosMouse;

	switch (Msg)
	{
	case WM_CREATE: // вызов сообщения при создании окна
		newWorld();
		break;
	case WM_COMMAND:
		switch (wParam)
		{
		case 101:
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		case 102:
			if (MessageBox(hWnd, L"Текущие данные будут потеряны, продолжить?", L"Новое поле", MB_YESNO | MB_APPLMODAL) == IDYES)
				newWorld();
			break;
		}
		break;

		printf("%lld (%d,%d)	%lld (%d,%d)\n", wParam, LOWORD(wParam), HIWORD(wParam), lParam, LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_KEYDOWN:
	{
		auto isShift = GetKeyState(VK_SHIFT) < 0;

		switch (wParam)
		{
		case 107:
			multiple++;
			resize();
			break;
		case 109:
			multiple--;
			if (multiple < 5) multiple = 5;
			resize();
			break;
		case 13:
			pause = !pause;
			break;
		case 37:case 38:case 39:case 40://left,up,rigth,down
		{
			switch (wParam - 37)
			{
			case 0:
				pos.x--;
				break;
			case 1:
				pos.y--;
				break;
			case 2:
				pos.x++;
				break;
			case 3:
				pos.y++;
				break;
			}
			chek_pos(hWnd);
		}
		break;
		case 32:
		{
			ActionWin(75);
			ShowWin();
		}	break;
		default:
			if (wParam >= 49 and wParam <= 55) {
				tecCol = wParam - 48;
				ShowInfo();
			}
			else
				printf("WM_KEYDOWN: %lld\n", wParam);
			break;
		}
	}
	break;
	/*case WM_LBUTTONDOWN:
	{
		MyPOINT tmp(LOWORD(lParam), HIWORD(lParam));
		if (tmp.x > winSize.x * multiple) {
			MyRECT r(winSize.x * multiple + 10, ClSize.y - 40, ClSize.x - 10, ClSize.y - 10);
			for (auto& a : ElAct) {
				if (PtInRect(&r, tmp))
				{
					switch (a.first)
					{
					case 0:
						SendMessage(hWnd, WM_CLOSE, 0, 0);
						break;
					case 1:
						if (MessageBox(hWnd, L"Текущие данные будут потеряны, продолжить?", L"Новое поле", MB_YESNO | MB_APPLMODAL) == IDYES)
							newWorld();
						break;
					}
					break;
				}
				r.ShiftV(-35);
			}
		}
	}
	break;*/
	case WM_RBUTTONDOWN:
		MyPOINT_(prevPosMouse).SET(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_MOUSEMOVE: {
		MyPOINT tmp(LOWORD(lParam), HIWORD(lParam));
		if ((wParam & MK_RBUTTON) == MK_RBUTTON) {
			tmp -= prevPosMouse;
			tmp /= multiple;
			if (tmp.x == 0 && tmp.y == 0) break;
			pos += tmp;
			chek_pos(hWnd);
			MyPOINT_(prevPosMouse).SET(LOWORD(lParam), HIWORD(lParam));
		}
		else if ((wParam & MK_LBUTTON) == MK_LBUTTON) {
			tmp /= multiple;
			myg.SetVal(pos.x + tmp.x, pos.y + tmp.y, tecCol, true);
			ShowWin();
		}/*
		else if (tmp.x > winSize.x * multiple) {
			MyRECT r(winSize.x * multiple + 10, ClSize.y - 40, ClSize.x - 10, ClSize.y - 10);
			for (auto& a : ElAct) {
				if (PtInRect(&r, tmp)) {
					SelectObject(secondHdc, GetStockObject(WHITE_BRUSH));
					RoundRect(secondHdc, r.left, r.top, r.right, r.bottom, 5, 5);
					InvalidateRect(hWnd, &r, true);
					break; }
				r.ShiftV(-35);
			}
		}*/
	}
	 break;
	case WM_SIZE: {
		ClSize.x = LOWORD(lParam);
		ClSize.y = HIWORD(lParam);
		resize();
	}
				break;
	case WM_PAINT:
		OnPaint();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
	break; 
	default: 
		return DefWindowProcW(hWnd, Msg, wParam, lParam);
	}

	return 0;
}

int main()
{
	srand(GetTickCount64());

	WNDCLASSW wlc;
	memset(&wlc, 0, sizeof(wlc));
	wlc.lpszClassName = L"MyApiClass";
	wlc.lpfnWndProc = WndProc;
	wlc.hCursor = LoadCursor(NULL, IDC_ARROW);

	if (!RegisterClassW(&wlc))
		return 0;

	hWnd = CreateWindowExW(WS_EX_APPWINDOW, L"MyApiClass", L"My window", WS_OVERLAPPEDWINDOW, 100,  100, 800, 600, NULL, NULL, NULL, NULL);

	secondHdc = CreateCompatibleDC(GetDC(hWnd));
	
	auto b = CreateWindowW(
		L"BUTTON",  // Predefined class; Unicode assumed 
		L"Выход",      // Button text 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
		10,         // x position 
		10,         // y position 
		130,        // Button width
		25,        // Button height
		hWnd,     // Parent window
		(HMENU)101,       // No menu.
		(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
		NULL);      // Pointer not needed.
	if (b == nullptr)
		cout << "Button 'Выход'" << GetLastError() << endl;

	b = CreateWindowW(
		L"BUTTON",  // Predefined class; Unicode assumed 
		L"Заново",      // Button text 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
		10,         // x position 
		10,         // y position 
		130,        // Button width
		25,        // Button height
		hWnd,     // Parent window
		(HMENU)102,       // No menu.
		(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
		NULL); ;

	if (b == nullptr)
		cout << "Button 'Заново'" << GetLastError() << endl;

	ShowWindow(hWnd, SW_SHOWNORMAL);

	MSG msg;
	/*while (GetMessage(&msg, NULL, 0 , 0))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);

	}*/
	ULONG tc = GetTickCount64(), t = tc;
	/*BOOL b;	GetSystemTimeAdjustment(&tc, &t, &b);*/

	while (1) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
			continue;
		}

		t = GetTickCount64();
		if(pause)
			tc = GetTickCount64();
		else if(t > tc)
		{
			ActionWin(t - tc);
			ShowWin();
			tc = t; // GetTickCount();
		}

	}
	return 0;
}