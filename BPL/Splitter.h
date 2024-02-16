#ifndef class_Spliter
#define class_Spliter

#include <windows.h>
#define WC_CONTSPLITER	_T("ContSpliterClass")


class ContSpliter {
protected:
	HWND W1, W2;
	int dwSplitterPos;
	bool bSplitterMoving = false;
	byte width;
	HCURSOR hCur;
	
	static LRESULT CALLBACK static_Procedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	ContSpliter();
public:
	void Init(HWND Item, HWND Ch1, HWND Ch2, byte width, bool hor);
	
	virtual void Resize(HWND hwnd, WPARAM wParam, LPARAM lParam) =0;
	virtual void Move(HWND hwnd, WPARAM wParam, LPARAM lParam) = 0;

	void ResizeV(HWND hwnd, WPARAM wParam, LPARAM lParam);
	void ResizeH(HWND hwnd, WPARAM wParam, LPARAM lParam);
	void MoveV(HWND hwnd, WPARAM wParam, LPARAM lParam);
	void MoveH(HWND hwnd, WPARAM wParam, LPARAM lParam);

	static function<ContSpliter * (HWND)> ProcGet = nullptr;
	//static ContSpliter* (*ProcGet)(HWND)=nullptr;
	static void Reg_Class(HINSTANCE hInst);
};


class ContSpliter_hor : public ContSpliter {
public:
	inline void Resize(HWND hwnd, WPARAM wParam, LPARAM lParam) override { ResizeH(hwnd, wParam, lParam); };
	inline void Move(HWND hwnd, WPARAM wParam, LPARAM lParam) override { MoveH(hwnd, wParam, lParam); };
	inline ContSpliter_hor(HWND Item, HWND Ch1, HWND Ch2, byte width = 4) {Init(Item, Ch1, Ch2, width, true);};
};


class ContSpliter_vert : public ContSpliter {
public:
	inline void Resize(HWND hwnd, WPARAM wParam, LPARAM lParam) override { ResizeV(hwnd, wParam, lParam); };
	inline void Move(HWND hwnd, WPARAM wParam, LPARAM lParam) override { MoveV(hwnd, wParam, lParam); };
	inline ContSpliter_vert(HWND Item, HWND Ch1, HWND Ch2, byte width = 4) { Init(Item, Ch1, Ch2, width, false); };
};


void ContSpliter::Reg_Class(HINSTANCE hInst) {
	WNDCLASS C;
	if (GetClassInfo(hInst, WC_CONTSPLITER, &C)) return;
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpfnWndProc = static_Procedure;
	//wcex.lpfnWndProc = nullptr;
	wcex.hInstance = hInst;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszClassName = WC_CONTSPLITER;
	_ASSERTE(RegisterClassEx(&wcex) != 0);
};	// ---------------------------------------------------------------

ContSpliter::ContSpliter() {
	W1 = W2 = nullptr;
	width = 0;
	dwSplitterPos = 0;
}; // ---------------------------------------------------------------

void ContSpliter::Init(HWND Item, HWND Ch1, HWND Ch2, byte wid, bool hor) {
	W1 = Ch1;
	W2 = Ch2;
	width = wid;
	auto s = SetWindowLong(Item, GWLP_USERDATA, (LONG)this);
	s = GetWindowLong(Item, GWLP_USERDATA);
	hCur = LoadCursor(NULL, hor ? IDC_SIZEWE : IDC_SIZENS);
	dwSplitterPos = 0;
	SetWindowLong(Item, GWLP_WNDPROC, (LONG)&static_Procedure);
	s = GetWindowLong(Item, GWLP_WNDPROC);
}; // ---------------------------------------------------------------

void ContSpliter::MoveH(HWND hwnd, WPARAM wParam, LPARAM lParam) {
	if (LOWORD(lParam) > width * 2) {// do not allow above this mark
		SetCursor(hCur);
		if ((wParam == MK_LBUTTON) && bSplitterMoving)
		{
			RECT rect;
			GetClientRect(hwnd, &rect);
			if (LOWORD(lParam) + width * 2 > rect.right)
				return;

			dwSplitterPos = LOWORD(lParam);
			Resize(hwnd, 1, MAKELPARAM(rect.right, rect.bottom));
		}
	}
};// ---------------------------------------------------------------

void ContSpliter::MoveV(HWND hwnd, WPARAM wParam, LPARAM lParam) {
	if (HIWORD(lParam) > width * 2) {// do not allow above this mark
		SetCursor(hCur);
		if ((wParam == MK_LBUTTON) && bSplitterMoving)
		{
			RECT rect;
			GetClientRect(hwnd, &rect);
			if (HIWORD(lParam) + width * 2 > rect.bottom)
				return;

			dwSplitterPos = HIWORD(lParam);
			Resize(hwnd, 1, MAKELPARAM(rect.right, rect.bottom));
		}
	}
};//---------------------------------------------------------------

void ContSpliter::ResizeH(HWND hwnd, WPARAM wParam, LPARAM lParam) {
	if (!dwSplitterPos)
		dwSplitterPos = LOWORD(lParam) / 2;

	if ((wParam != 1) && (LOWORD(lParam) < dwSplitterPos))
		dwSplitterPos = LOWORD(lParam) - width * 2;

	/* Adjust the children's size and position */
	HDWP hdwp = BeginDeferWindowPos(2);
	DeferWindowPos(hdwp, W1, nullptr, 0, 0, dwSplitterPos - 1, HIWORD(lParam), 0);
	DeferWindowPos(hdwp, W2, nullptr, dwSplitterPos + width, 0, LOWORD(lParam) - dwSplitterPos, HIWORD(lParam), 0);
	if (hdwp) EndDeferWindowPos(hdwp);
	
};//---------------------------------------------------------------

void ContSpliter::ResizeV(HWND hwnd, WPARAM wParam, LPARAM lParam) {
	if (!dwSplitterPos)
		dwSplitterPos = HIWORD(lParam) / 2;

	if ((wParam != 1) && (HIWORD(lParam) < dwSplitterPos))
		dwSplitterPos = HIWORD(lParam) - width * 2;

	/* Adjust the children's size and position */
	HDWP hdwp = BeginDeferWindowPos(2);
	DeferWindowPos(hdwp, W1, nullptr, 0, 0, LOWORD(lParam), dwSplitterPos - 1, 0);
	DeferWindowPos(hdwp, W2, nullptr, 0, dwSplitterPos + width, LOWORD(lParam), HIWORD(lParam) - dwSplitterPos - width, 0);
	if (hdwp) EndDeferWindowPos(hdwp);

};//---------------------------------------------------------------
//			ContSpliter <<<<<<<<<<<<<<<<<<<<<<<


LRESULT CALLBACK ContSpliter::static_Procedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (ProcGet == nullptr)
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	ContSpliter* A = ProcGet(hwnd);

	/*if (A == nullptr)
		return DefWindowProc(hwnd, uMsg, wParam, lParam);*/

	switch (uMsg) {
	case WM_SIZE: 
		A->Resize(hwnd, wParam, lParam);
	break;
	case WM_MOUSEMOVE:
		A->Move(hwnd, wParam, lParam);
	break;
	case WM_LBUTTONDOWN: {
		//ContSpliter* _this = ContSpliter::GetObject(hwnd);
		A->bSplitterMoving = TRUE;
		SetCapture(hwnd);
	}
	break;
	case WM_LBUTTONUP: {
		ReleaseCapture();
		//ContSpliter* _this = ContSpliter::GetObject(hwnd);
		A->bSplitterMoving = FALSE;
	}
	break;
	case WM_DESTROY: {
		//delete  ContSpliter::GetObject(hwnd);
	}
	case WM_COMMAND: 
		SendMessage(GetParent(hwnd), WM_COMMAND, wParam, lParam);
	break;
	default: return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

#endif