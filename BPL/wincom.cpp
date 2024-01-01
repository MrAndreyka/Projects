#ifndef WindowsComands
#define WindowsComands

#include <tchar.h>
#include <forward_list>

int ShowMessage(HWND Window, SmartStr Text, SmartStr Caption = SmartStr(), UINT Type = MB_ICONINFORMATION) {
	return MessageBox(Window, Text, Caption, Type);
}//---------------------------------------------------------------------------------------------

int ShowMessage(SmartStr Text, SmartStr Caption = _T(""), UINT Type = MB_ICONINFORMATION, HWND Window = NULL) {
	return MessageBox(Window, Text, Caption, Type);
}//-----------------------------------------------------------------------------------------------

HWND GetDlgItem_fromDlg(HWND hwnd, UINT Glg1, UINT Glg2) {
	return GetDlgItem(GetDlgItem(hwnd, Glg1), Glg2);
}

HWND GetDlgItem_list(HWND hwnd, std::initializer_list<UINT> items) {
	for each(auto item in items)
		hwnd = GetDlgItem(hwnd, item);
	return hwnd;
}


SmartStr TextError(DWORD Code) {
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, Code,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Язык по умолчанию
		(LPTSTR)&lpMsgBuf, 0, NULL);
	SmartStr _tmp((LPTSTR)lpMsgBuf);
	if (lpMsgBuf) LocalFree(lpMsgBuf);
	else _tmp <<_T("Неверный код ошибки: ") << Code;
	return _tmp;
}

//	Получает Текст Окна
SmartStr GetText(HWND Window, int Item = 0){
	if (Item) Window = GetDlgItem(Window, Item);
	int L = GetWindowTextLength(Window);
	_ASSERTE(L >= 0);
	SmartStr s;
	if(L) s.ReleaseBuffer(GetWindowText(Window, s.GetBuffer(L), L + 1));
	return s;
	}//-----------------------------------------------------------------------------------------------

bool GetState(HWND Window, int Item = 0){// возвращает состояние кнопки
	if (Item) Window = GetDlgItem(Window, Item);
	return SendMessage(Window, BM_GETSTATE, 0, 0) == 1;
	}//--------------------------------------------------------------------------------------------

INT64 GetLineCount(HWND Window, int Item = 0) {
	if (Item) Window = GetDlgItem(Window, Item);
	return SendMessage(Window, LB_GETCOUNT, 0, 0);
	}//---------------------------------------------------------------------------------------------

INT64 GetLineCurent(HWND Window, int Item = 0) {
	if (Item) Window = GetDlgItem(Window, Item);
	return SendMessage(Window, LB_GETCURSEL, 0, 0);
	}//---------------------------------------------------------------------------------------------

SmartStr GetLineText(HWND Window, INT64 Line = -2) {
	if (Line < -1) Line = GetLineCurent(Window);
	SmartStr Res;
	if (Line == -1) return Res;
	LRESULT L = SendMessage(Window, LB_GETTEXTLEN, (WPARAM)Line, 0);
	if (L < 0) ShowMessage(TextError(GetLastError()), _T("Ошибка"));
	else {
		SendMessage(Window, LB_GETTEXT, (WPARAM)Line, (LPARAM)Res.GetBuffer(L));
		Res.ReleaseBuffer();
	}
	return Res;
	}//--------------------------------------------------------------------------------------------

void ClearLines(HWND Window, size_t Beg = 0, size_t Count = 0){
	if (!Count) if (!Beg) { SendMessage(Window, LB_RESETCONTENT, 0, 0); return; }
	else Count = SendMessage(Window, LB_GETCOUNT, 0, 0) - Beg;
	Count += Beg;

	while (Beg < Count){	
		Count--;
		SendMessage(Window, LB_DELETESTRING, Count, 0);
		}
	}//////////////////////////////////////////////////////////////////////////

size_t AddLine(HWND Window, SmartStr Text, int SetHorizontal = -1){
	LRESULT L = SendMessage(Window, LB_ADDSTRING, 0, (LPARAM)Text.Data ());
	if (SetHorizontal){
		tagSIZE Size;
		if (SetHorizontal <0) {
			GetTextExtentPoint32(GetDC(Window), Text, (int)Text.Length(), &Size); 
			Size.cx += 10; 
		}
		else Size.cx = SetHorizontal;
		SendMessage(Window, LB_SETHORIZONTALEXTENT, (WPARAM)Size.cx, 0);
		}
	return L;
	}//-----------------------------------------------------------------------------------------------
#endif