#include "mainC.h"

HDC hdc, secondHdc;

//HDC thirdHdc;

POINT ClSize, M;
//HBITMAP bMap;

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

	void Show(HDC dc) {
		SetDCBrushColor(dc, Color);
		Ellipse(dc, x - r, y - r, x + r, y + r);
	}

	void ShowLine(HDC dc) {
		SetDCPenColor(dc, Color);
		LineTo(dc, x, y);
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
			//r++;
		}

		return live > d;
	}
	void setLive(DWORD live_) { live = live_;}
	void setColor(COLORREF col) { Color = col; ColorDef = col; }

//private:
	DWORD pc=0;
	DWORD live = 0;
	COLORREF Color,ColorDef = 0;
};

//bool compareInterval(MyLivePoint &a, MyLivePoint &b) { return a.right > b.right; }
list<MyLivePoint<bool>> lst;

BOOL pause = true, showInfo = false, step_stop = false, lines = true;

void get_graph() {
	cout << " *";
	SelectObject(secondHdc, GetStockObject(DC_PEN));
	MoveToEx(secondHdc, ClSize.x / 2, 0, NULL);
	LineTo(secondHdc, ClSize.x / 2, ClSize.y);
	MoveToEx(secondHdc, 0, ClSize.y / 2, NULL);
	LineTo(secondHdc, ClSize.x, ClSize.y / 2);

	SelectObject(secondHdc, GetStockObject(BLACK_BRUSH));

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
}

void ShowWin()
{
	SelectObject(secondHdc, GetStockObject(BLACK_BRUSH));
	Rectangle(secondHdc, 0, 0, ClSize.x + 1, ClSize.y + 1);

	SelectObject(secondHdc, GetStockObject(DC_PEN));
	SelectObject(secondHdc, GetStockObject(DC_BRUSH));

	//bool vg = true;
	//if(lst.begin() != lst.end())
  	//	Rectangle(secondHdc, lst.begin()->x, lst.begin()->y, lst.begin()->x + 4, lst.begin()->y + 4);
	//for (auto a : lst) Rectangle(secondHdc, a.x, a.y, a.x + 4, a.y + 4);
		//a.Show(secondHdc);
//	{
		//SetDCPenColor(secondHdc, a.Color);
		/*if (lines and a.val) continue;
		if (vg and !a.val) {
			get_graph();
			vg = false;
			MoveToEx(secondHdc, 0, a.y - 1, NULL);
			cout << "==========\nMoveToEx(secondHdc, " << 0 << ", " << a.y - 1 << ");" << endl;
		}*/

		//a.Show(secondHdc);

//	}
	SetDCPenColor(secondHdc, 355);
	get_graph();

	if(lst.begin() != lst.end())
	{
 		get_graph();
	}
	if (showInfo)
		for (int i = 0; i < mtf; i++)
		{
			RECT R = { 5 , 5 + i * 20, 300, 20 + i * 20 };
			SetBkColor(secondHdc, mf[i].color);
			auto s = to_string(i + 1) + " - " + mf[i].Name;
			DrawText(secondHdc, s.c_str(), s.length(), &R, NULL);
		}

	BitBlt(hdc, 0, 0, ClSize.x, ClSize.y, secondHdc, 0, 0, SRCCOPY);
}

/*
void ShowWin2()
{
	SelectObject(secondHdc, GetStockObject(BLACK_BRUSH));
	Rectangle(secondHdc, 0, 0, ClSize.x+1, ClSize.y+1);	
	
	//SelectObject(secondHdc, GetStockObject(NULL_PEN));
	SelectObject(secondHdc, GetStockObject(DC_BRUSH));
	SelectObject(secondHdc, GetStockObject(DC_PEN));
	if(lines)
		SelectObject(secondHdc, GetStockObject(DC_PEN));
	else
		SelectObject(secondHdc, GetStockObject(DC_BRUSH));

	bool vg = true;

	auto b = lst.begin();
	while(b!= lst.end())
	//for (auto& a : lst) 
	{
 		auto& a = *b;
		b++;
		SetDCPenColor(secondHdc, a.Color);
		if (lines and a.val) continue;
		if (vg and !a.val) { 
			get_graph();
			vg = false;
			MoveToEx(secondHdc, 0, a.y-1, NULL);
			cout << "==========\nMoveToEx(secondHdc, " << 0 << ", " << a.y-1 << ");" << endl;
		}

		LineTo(secondHdc, a.x, a.y);
		cout << "LineTo(secondHdc, "<< a.x << ", " << a.y << ");" << endl;
		LineTo(secondHdc, a.x, a.y);

		/*if (lines) //a.ShowLine(secondHdc);
 			LineTo(secondHdc, a.x, a.y);
		else a.Show(secondHdc);
		
	}

	get_graph();
	if (showInfo)
		for (int i = 0; i < mtf; i++)
		{
			RECT R = { 5 , 5 + i * 20, 300, 20 + i * 20 };
			SetBkColor(secondHdc, mf[i].color);
			auto s = to_string(i + 1) + " - " + mf[i].Name;
			DrawText(secondHdc, s.c_str(), s.length(), &R, NULL);
		}

	BitBlt(hdc, 0, 0, ClSize.x, ClSize.y, secondHdc, 0, 0, SRCCOPY);
}*/
 
void ActionWin(HWND hWnd, DWORD cou)
{
    static DWORD ct = 0; ct += cou;

	int count = lst.size();
	/*auto current = lst.begin(), prev = lst.before_begin();
	while (current != lst.end())
		if (!current->go(ct))
		{
			lst.erase_after(current = prev);
			current++;
		}
		else
		{
			prev = current++;
			count++;
		}*/

	static int X = 0;
	cou = (cou / 1000.0) * M.y;
	   	
	while (cou-- > 0 and !step_stop) {

		double x = (X - ClSize.x / 2.0) / M.x,
			y = mf[tf].func(x);

		MyLivePoint <bool> v;

		v.x = x * M.x + ClSize.x / 2.0;
		v.y = y * M.x + ClSize.y / 2.0;
		v.val = false;

		v.r = rp;
		v.setLive(3500);
		v.setColor(mf[tf].color);

		lst.emplace_back(v);
		count++;
		
		X += range;

		if (X > ClSize.x)
		{
			X = 0;
			if (++tf == mtf) tf = 0;
			
			static auto b = lst.begin();
			if (!lst.begin()->val) b = lst.begin();
			std::for_each(b, lst.end(), [](auto& a) {a.val = true; });

		}
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
			break;
		case 32:
		{
			ActionWin(hWnd, M.y/20);
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

		HBITMAP bMap = CreateCompatibleBitmap(hdc, ClSize.x, ClSize.y);
		DeleteObject(SelectObject(secondHdc, bMap));
		lst.clear();
		break;
	}

	case WM_MOUSEMOVE:{
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc2 = BeginPaint(hWnd, &ps); 
		BitBlt(hdc2, 0, 0, ClSize.x, ClSize.y, secondHdc, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break; 
	default: return DefWindowProcW(hWnd, Msg, wParam, lParam);
	}

	if(hdc != NULL)
		ReleaseDC(hWnd, hdc);
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

	HWND hwnd = CreateWindowExW(WS_EX_APPWINDOW, L"MyApiClass", L"My window", WS_OVERLAPPEDWINDOW, 100,  100, 800, 600, NULL, NULL, NULL, NULL);
	hdc = GetDC(hwnd);

	secondHdc = CreateCompatibleDC(hdc);
	HBITMAP bMap = CreateCompatibleBitmap(hdc, ClSize.x, ClSize.y);
	DeleteObject(SelectObject(secondHdc, bMap));

	SelectObject(secondHdc, GetStockObject(DC_BRUSH));
	SelectObject(secondHdc, GetStockObject(DC_PEN));
	SetDCPenColor(secondHdc, RGB(80, 80, 80));
	
	ShowWindow(hwnd, SW_SHOWNORMAL);
	ShowWin();

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
			ActionWin(hwnd, t - tc);
			ShowWin();
			tc = t; // GetTickCount();
		}

	}


	ReleaseDC(hwnd, hdc);

	return 0;
}