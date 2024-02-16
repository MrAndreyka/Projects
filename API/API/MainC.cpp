#include "mainC.h"
#include "..\..\Iincludes\Rect_Point.h"

HDC secondHdc;
HWND hWnd;

POINT ClSize, M;


float GetY1(float x){
	return cos(x * 2) * (cos(x * 3) - 1) + sin(-x);
}
float GetY2(float x){
	return cos(x * 2) * cos(-x);
}
float GetY3(float x){
	return tan(x * 3) * sin(x * 2) - sin(-x);
}
float GetY4(float x){
	return cos(x * 4) * (1 - cos(x * 2.6)) + sin(x * 3);
}
float GetY5(float x){
	return tan(x) * tan(x + 2);
}
float GetY6(float x){
	return cos(x * 3) * (cos(x * 1.6) - 1.5) + sin(x * 2);
}
float GetY7(float x){
	return atan(x) * tan(-x);
}

struct fun_dat {
public:
	string Name;
	float(*func)(float) = nullptr;
	COLORREF color;
	fun_dat(string name, COLORREF col, float(*fun)(float)) :Name(name), color(col), func(fun) {};
};

const int points(5), range(3), mtf(7);
BYTE tf = 0, rp = 3, sauto = 0;

class MyPen {
public:
	HPEN pen = NULL;
	COLORREF color = 0;
	void Set(HDC hdc, int width, COLORREF color) {
		if (color != this->color) {
			if(pen != NULL) 
				DeleteObject(pen);
			pen = CreatePen(PS_SOLID, width, color);
			SelectObject(hdc, pen);
		}

	}
};

MyPen PEN;

fun_dat mf[] = {
	fun_dat("cos(x*2) * (cos(x*3)-1) + sin(-x)", 0x0000ff, GetY1),
	fun_dat("cos(x * 2)* cos(-x)", 0x00ff00, GetY2),
	fun_dat("tan(x*3) * sin(x * 2) - sin(-x)", 0xff0000, GetY3),
	fun_dat("cos(x * 4)* (1 - cos(x * 2.6)) + sin(x * 3)", 0x00ffff, GetY4),
	fun_dat(" tan(x) * tan(x+2)", 0xff00ff, GetY5),
	fun_dat("cos(x * 3)* (cos(x * 1.6) - 1.5) + sin(x*2)", 0xffff00, GetY6) ,
	fun_dat("atan(x) * tan(-x)", 0xffffff, GetY7) };

template <class T>
class MyLivePoint//:public Point
{
public:
	int x, y;
	
	DWORD r;
	T val;

	void Show(HDC hdc) {
		SetDCBrushColor(hdc, Color);
		Ellipse(hdc, x - r, y - r, x + r, y + r);
	}

	void ShowLine(HDC hdc, MyPen& pen) {
		//SetDCPenColor(hdc, Color);
		pen.Set(hdc, r, Color);
		LineTo(hdc, x, y);
	}

	bool go(DWORD ct)
	{
		if (pc == 0) {pc = ct; return true;}
		auto d = ct - pc;

		Color = ColorDef;

		byte* tb = ((byte*)& Color);
		for(int i = 0; i < 3; i++)
		*(tb+i) = *(tb + i) * (live-d) / live;

		if (live - d < live / 2)
		{
			y--; 
			r++;
		}

		return live > d;
	}
	void setLive(DWORD live_) { live = live_;}
	void setColor(COLORREF col) { Color = col; ColorDef = col; }

private:
	DWORD pc=0;
	DWORD live = 0;
	COLORREF Color,ColorDef = 0;
};

list<MyLivePoint<bool>> lst;

BOOL pause = false, showInfo = false, step_stop = false, lines = true;

void get_graph() {
	auto prev_pen = SelectObject(secondHdc, GetStockObject(DC_PEN));
	SetDCPenColor(secondHdc, RGB(80,80,80));

	MoveToEx(secondHdc, ClSize.x / 2, 0, NULL);
	LineTo(secondHdc, ClSize.x / 2, ClSize.y);
	MoveToEx(secondHdc, 0, ClSize.y / 2, NULL);
	LineTo(secondHdc, ClSize.x, ClSize.y / 2);

	for (int c, i = max(ClSize.y, ClSize.x) / M.x / 2; i > 0; i--)
	{
		c = i * M.x + ClSize.x / 2.0;
		MoveToEx(secondHdc, c, ClSize.y / 2 - 3, NULL);
		LineTo(secondHdc, c, ClSize.y / 2 + 3);
		c = -i * M.x + ClSize.x / 2.0;
		MoveToEx(secondHdc, c, ClSize.y / 2 - 3, NULL);
		LineTo(secondHdc, c, ClSize.y / 2 + 3);

		c = i * M.x + ClSize.y / 2.0;
		MoveToEx(secondHdc, ClSize.x / 2 - 3, c, NULL);
		LineTo(secondHdc, ClSize.x / 2 + 3, c);
		c = -i * M.x + ClSize.y / 2.0;
		MoveToEx(secondHdc, ClSize.x / 2 - 3, c, NULL);
		LineTo(secondHdc, ClSize.x / 2 + 3, c);
	}
	SelectObject(secondHdc, prev_pen);
}


void ShowWin(){
	SelectObject(secondHdc, GetStockObject(BLACK_BRUSH));
	SelectObject(secondHdc, GetStockObject(NULL_PEN));
	Rectangle(secondHdc, 0, 0, ClSize.x+1, ClSize.y+1);	
	
	//SelectObject(secondHdc, GetStockObject(DC_BRUSH));
	//SelectObject(secondHdc, GetStockObject(DC_PEN));

	if(lines)
   		SelectObject(secondHdc, PEN.pen);
	else
	{
		SelectObject(secondHdc, GetStockObject(NULL_PEN));
		SelectObject(secondHdc, GetStockObject(DC_BRUSH));
	}

	for (auto& a : lst) {
		if (a.val) {
			get_graph();
			MoveToEx(secondHdc, a.x, a.y, NULL);
		}
		else if (lines)
			a.ShowLine(secondHdc, PEN);
		else
			a.Show(secondHdc);
	}

	if (showInfo)
		for (int i = 0; i < mtf; i++){
			RECT R = { 5 , 5 + i * 20, 300, 20 + i * 20 };
			SetBkColor(secondHdc, mf[i].color);
			auto s = to_string(i + 1) + " - " + mf[i].Name;
			DrawText(secondHdc, s.c_str(), s.length(), &R, NULL);
		}

	InvalidateRect(hWnd, NULL, true);
}
 
void ActionWin(DWORD cou){
    static DWORD ct = 0; ct += cou;

	lst.remove_if([](auto& a) {return !a.go(ct); });
	if (lst.size() > 0 && !lst.begin()->val)
		lst.begin()->val = true;

	static int X = 0;
	cou = (cou / 1000.0) * M.y;

	   	
	while (cou-- > 0 and !step_stop) {

		double x = (X - ClSize.x / 2.0) / M.x,
			y = mf[tf].func(x);

		MyLivePoint <bool> v;

		v.x = x * M.x + ClSize.x / 2.0;
		v.y = y * M.x + ClSize.y / 2.0;
		v.val = X == 0;

		v.r = rp;
		v.setLive(3500);
		v.setColor(mf[tf].color);

 		lst.push_back(v);
		
		X += range;

		if (X > ClSize.x) {
			X = 0;
			if (++tf == mtf) tf = 0;
		}

		/*if (X > ClSize.x)
		{
			X = 0;
			if (++tf == mtf) tf = 0;
			
			static auto b = lst.begin();
			if (!lst.begin()->val) 
				b = lst.begin();
			std::for_each(b, lst.end(), [](auto& a) {a.val = true; });
			b = lst.end();
			b--;

		}*/
	}

	//printf(" /  %d\n", count);
	//string s = to_string(X) + " :: " + to_string(x) + " : " + to_string(y) + " ( " + to_string(v.x) + " : " + to_string(v.y) + ") / " + to_string(count);
	//SetWindowText(hWnd, s.c_str());

}

LRESULT WINAPI WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	//printf("%d	%d	%d\n", Msg, wParam, lParam);

	switch (Msg)
	{
	case WM_CREATE: // вызов сообщения при создании окна
	{
		/*RECT ClRect;
		GetClientRect(hWnd, &ClRect);
		ClSize.x = ClRect.right;
		ClSize.y = ClRect.bottom;
		/*M1.x = ClSize.x / 6;
		M1.y = ClSize.y / 6;
		M2.x = M1.x / 3;
		M2.y = M1.y / 3;*/

		//FillMass();
	}
	break;
	case WM_CHAR:
	{
		auto isShift = GetKeyState(VK_SHIFT) < 0;

		switch (wParam)
		{
		case 13:
			if (isShift) step_stop = !step_stop;
			else pause = !pause;
			break;
		case 105:
			showInfo = !showInfo;
			ShowWin();
			break;
		case 108:
			lines = !lines;
			ShowWin();
			break;
		case 32:
		{
			ActionWin(M.y/20);
			ShowWin();
		}	break;
		case 43:
			if(rp< 254)rp++;
			break;
		case 45:
			if (rp > 2)rp--;
			break;
		default:
			printf("%d", wParam);
			break;
		}
	}
	break;
	case WM_SIZE:{
		ClSize.x = LOWORD(lParam);
		ClSize.y = HIWORD(lParam);
		M.x = min(ClSize.x / points  / 2, ClSize.y / points / 2);
		M.y = (ClSize.x / 1.5) / range;

		string s = to_string(M.x) + " : " + to_string(M.y) + "    " + to_string(ClSize.x) + " : " + to_string(ClSize.y);

		SetWindowText(hWnd, s.c_str());

		HBITMAP bMap = CreateCompatibleBitmap(GetDC(hWnd), ClSize.x, ClSize.y);
		DeleteObject(SelectObject(secondHdc, bMap));
		lst.clear();
		ShowWin();
		break;
	}

	case WM_MOUSEMOVE:{
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		auto hdc = BeginPaint(hWnd, &ps); 
		BitBlt(hdc, 0, 0, ClSize.x, ClSize.y, secondHdc, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break; 
	default: return DefWindowProcW(hWnd, Msg, wParam, lParam);
	}

	//if(hdc != NULL)	ReleaseDC(hWnd, hdc);
	return 0;
}

int main()
{
	srand(GetTickCount());

	WNDCLASSW wlc;
	memset(&wlc, 0, sizeof(wlc));
	wlc.lpszClassName = L"MyApiClass";
	wlc.lpfnWndProc = WndProc;
	wlc.hCursor = LoadCursor(NULL, IDC_ARROW);

	if (!RegisterClassW(&wlc))
		return 0;

	hWnd = CreateWindowExW(WS_EX_APPWINDOW, L"MyApiClass", L"My window", WS_OVERLAPPEDWINDOW, 100,  100, 800, 600, NULL, NULL, NULL, NULL);
	auto hdc = GetDC(hWnd);

	secondHdc = CreateCompatibleDC(hdc);
	SelectObject(secondHdc, GetStockObject(NULL_PEN));
	
	ShowWindow(hWnd, SW_SHOWNORMAL);
	//ShowWin(hwnd);

	MSG msg;
	/*while (GetMessage(&msg, NULL, 0 , 0))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);

	}*/
	auto tc = GetTickCount(), t = tc;
	/*BOOL b;	GetSystemTimeAdjustment(&tc, &t, &b);*/

	while (1) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
			continue;
		}

		t = GetTickCount();
		if(pause)
			tc = GetTickCount();
		else if(t > tc)
		{
			ActionWin(t - tc);
			ShowWin();
			tc = t; // GetTickCount();
		}

	}


	ReleaseDC(hWnd, hdc);
	ReleaseDC(hWnd, secondHdc);

	return 0;
}