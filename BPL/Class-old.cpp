#include <windows.h>
#include <sstream>
#pragma hdrstop

#include "class-old.h"
#include "MyExcept.h"

bool MyStr_GenerateError(true);

bool MyStrGenerateError(){ return MyStr_GenerateError; };
void MyStrGenerateError(bool val){ MyStr_GenerateError = val; };

//--------------------------  MyUnStr --------------------------------------------
MyUnStr::~MyUnStr(){
	if (!dt)return;
	if (dt->ss)	{
		dt->ss--;
	}
	else{
		if (buf) delete[]buf;
		delete dt;
	}
}//---------------------------------------------------------------------------

MyUnStr::MyUnStr(MyAnciiStr val){ Init(); Set(this, val.Data(), val.Length()); };

void MyUnStr::Length(size_t val, bool Copy){
	if (val == Length()) return;
	SetSize(val, val && Copy ? buf : NULL);
}//---------------------------------------------------------------------------

void MyUnStr::Clear(){
	if (!dt)return;
	if (dt->ss)	{
		dt->ss--;
		dt = NULL;
		buf = NULL;
	}
	else	{
		if (buf) delete[] buf;
		buf = NULL;
		dt->ss = 0;
		dt->len = 0;
	}
}//---------------------------------------------------------------------------

bool MyUnStr::ThrowBuffer() {
	if (!dt) return false;
	buf = NULL;
	if (dt->ss)	{ dt->ss--;	dt = NULL; return true; }
	else {dt->ss = 0; dt->len = 0; return false;	}
	}//////////////////////////////////////////////////////////////////////////

void MyUnStr::SetSize(size_t size, const WCHAR *old, UINT Count, bool FirstBuf){
	if (size + 1 == 0) throw TMyEx(100, "Превышение максимального размера буфера", "MyUnStr");
	if (!size && old) { size = wcslen(old); if (!Count) Count = size; }
	if (!size) { Clear(); return; }
	WCHAR *h = size == Length() && dt && dt->ss == 0? buf: new WCHAR[size+1];	
	if (old == buf && buf == h) return;
	h[size] = 0;

	if (old) {		
		if (!Count||Count > size) Count = min(size, wcslen(old));		
		if (FirstBuf) {
			memcpy(h, old, Count * sizeof(WCHAR));
			if (size != Count) *(h+Count) = 0;
		}
		else memcpy(h+size-Count, old, Count * sizeof(WCHAR));
	}
	else if (size) h[0] = 0;
	if (buf == h) return;
	Clear();
	if (!dt) dt = new StDt;
	dt->ss = 0;
	dt->len = size;
	buf = h;
};//---------------------------------------------------------------------------


void MyUnStr::Set(MyUnStr *STR, const MyUnStr  &val){
	if (STR == &val) return;
	STR->Clear();
	if (val.Length()){
		if (STR->dt) delete STR->dt;
		memcpy(STR, &val, sizeof(MyUnStr));
		STR->dt->ss++;
	}
}//---------------------------------------------------------------------------

void MyUnStr::Set(MyUnStr *STR, const WCHAR* buf, size_t Count, size_t Beg){
	if (!Count && !Beg && buf == STR->buf) return;
	STR->SetSize(Count, buf+Beg, Count);
}//---------------------------------------------------------------------------

void MyUnStr::Set(MyUnStr *STR, const char* buf, size_t Count, size_t Beg){
	if (STR->FromAscii(buf+Beg, Count)<=0) STR->Clear();
}//---------------------------------------------------------------------------

void MyUnStr::Set(MyUnStr *STR, const INT64 val, const int sys){
	if (sys <2) { STR->Clear(); return; }
	WCHAR* buf = new WCHAR[_CVTBUFSIZE];
	_i64tow_s(val, buf, _CVTBUFSIZE, sys);
	Set(STR, buf);
	delete[] buf;
}//---------------------------------------------------------------------------

void MyUnStr::Set(MyUnStr *STR, const double val, const int digits){
	char buf[_CVTBUFSIZE];
	int err = _gcvt_s(buf, _CVTBUFSIZE, val, digits);
	if (err!=0) {
		SetLastError(err);
		STR->Clear();
		return;
	}
	Set(STR, buf);
}//---------------------------------------------------------------------------

void MyUnStr::Set(MyUnStr *STR, const bool val){
	Set(STR, val ? L"True" : L"False", val ? 4 : 5);
}//---------------------------------------------------------------------------


void MyUnStr::Add(bool post, MyUnStr *STR, const MyUnStr  &val){
	if (!val.Length()) return;
	size_t sz = STR->Length();
	post = post||STR==&val;
	STR->SetSize(sz+val.dt->len, STR->buf, sz, post);
	if (post)	memcpy(STR->buf+sz, val.buf, (STR==&val ? sz : val.dt->len) * sizeof(WCHAR));
	else		memcpy(STR->buf, val.buf, (STR==&val ? sz : val.dt->len) * sizeof(WCHAR));
}//---------------------------------------------------------------------------

void MyUnStr::Add(bool post, MyUnStr *STR, const WCHAR* val, size_t Count, size_t Beg){
	if (!val) return;
	size_t sz = Count ? Count : wcslen(val+Beg);
	if (!sz) return;
	if (STR->buf==val)
	if (sz==STR->Length() && !Beg) { Add(post, STR, *STR); return; }
	else {
		MyUnStr _tmp(*STR);
		STR->SetSize(Count+STR->Length(), val+Beg, sz, !post);
		memcpy(post ? STR->buf : STR->buf+sz, _tmp.buf, _tmp.Length() * sizeof(WCHAR));
		return;
	}
	STR->SetSize(STR->Length()+sz, STR->buf, STR->Length(), post);
	if (post)	memcpy(STR->buf+STR->Length()-sz, val+Beg, sz * sizeof(WCHAR));
	else		memcpy(STR->buf, val+Beg, sz * sizeof(WCHAR));
}//---------------------------------------------------------------------------

void MyUnStr::Add(bool post, MyUnStr *STR, const  char* buf, size_t Count, size_t Beg){
	MyUnStr _tmp;
	if (_tmp.FromAscii(buf+Beg, Count)<=0) return;
	Add(post, STR, _tmp);
}//---------------------------------------------------------------------------

void MyUnStr::Add(bool post, MyUnStr *STR, const  INT64 val, int sys){
	if (sys <2) {return; }
	WCHAR buf[_CVTBUFSIZE];
	_i64tow_s(val, buf, _CVTBUFSIZE, sys);
	Add(true, STR, buf);
}//---------------------------------------------------------------------------

void MyUnStr::Add(bool post, MyUnStr *STR, const double val, int digits){
	char buf[_CVTBUFSIZE];
	int err = _gcvt_s(buf, _CVTBUFSIZE, val, digits);
	if (err) {
		SetLastError(err);
		STR->Clear();
		return;
	}
	Add(true, STR, buf);
}//---------------------------------------------------------------------------

void MyUnStr::Add(bool post, MyUnStr *STR, const   bool val){
	Add(true, STR, val ? L"True" : L"False", val ? 4 : 5);
}//---------------------------------------------------------------------------

MyUnStr MyUnStr::SubStr(size_t Beg, size_t Count){
	if (Beg >= Length()) return MyUnStr();
	if (!Count||Count+Beg>Length()) Count = Length()-Beg;
	MyUnStr _tmp;
	Set(&_tmp, *this, Count, Beg);
	return _tmp;
};//---------------------------------------------------------------------------

MyUnStr MyUnStr::GetLine(size_t Beg){
	int Ps = Pos(L"\r\n", Beg);
	if (Ps < 0) Ps = Length();
	return SubStr(Beg, Ps-Beg);
}//---------------------------------------------------------------------------
/*
MyUnStr* MyUnStr::GetLines(size_t &Count){
	int Ps(0), Ts(0);
	MyUnStr *array;
	do
	Ps = Pos(L"\r\n", Ts);

	while (Ps >=0)
	if (Ps < 0) Ps = Length();
	return SubStr(Beg, Ps-Beg);
	}//---------------------------------------------------------------------------*/

int MyUnStr::Pos(const WCHAR* val, size_t Beg, size_t Len){
	if (!buf || Beg > Length()) return -1;
	if (Len==0) Len = wcslen(val);
	WCHAR *P = buf+Beg;
	while (P<=buf+Length()-Len){
		if (*P==val[0]){
			size_t i;
			for (i = 1; i < Len; i++)
			if (*(P+i)!=val[i]) break;
			if (i==Len) return P-buf;
		}
		P++;
	}
	return -1;
};//---------------------------------------------------------------------------

int MyUnStr::PosOf(const WCHAR* val, size_t Beg, size_t Len){
	if (Beg >= Length()) return -1;
	if (Len==0) Len = wcslen(val);
	WCHAR *P = buf-Beg+Length()-Len;
	while (P>=buf){
		if (*P==val[0])
		{
			size_t i;
			for (i = 0; i<Len; i++)
			if (*(P+i)!=val[i]) break;
			if (i==Len) return P-buf;
		}
		P--;
	}
	return -1;
};//---------------------------------------------------------------------------

size_t MyUnStr::Replase(WCHAR* Val, WCHAR* dest, size_t Len, size_t LenDest){
	if (!Val)return 0;
	if (!Len) Len = wcslen(Val);
	if (!LenDest && dest) LenDest = wcslen(dest);
	size_t Count(0);
	MyUnStr TMP;
	if (LenDest != Len){
		int PS(0);
		PS = Pos(Val, 0, Len);
		while (PS>=0){
			PS += Len;
			PS = Pos(Val, PS, Len);
			Count++;
			}
		if (!Count) return 0;
		TMP.SetSize(Length()+(LenDest-Len) * Count, NULL);
		Count = 0;
		}
	else TMP = *this;	
	WCHAR *S = buf, *B = S, *D = TMP.buf;
	while (*S != 0){
		if (*S == Val[0]){
			size_t i;
			for (i = 1; i<Len; i++)
			if (*(S+i) != Val[i]) break;
			if (i == Len) {
				memcpy(D, B, (S-B) * sizeof(WCHAR));
				D += (S-B);
				B += (S-B+Len);
				if (LenDest){memcpy(D, dest, LenDest * sizeof(WCHAR));	D += LenDest;}
				Count++;
			}
		}
		S++;
	}
	if (*B) memcpy(D, B, (S-B) * sizeof(WCHAR));
	*this = TMP;
	return Count;
}//---------------------------------------------------------------------------

MyUnStr MyUnStr::Clone(const WCHAR * Val, size_t Count, size_t Len){
	if (!Len) Len = wcslen(Val);
	if (buf && buf==Val){
		SetSize(Len * Count, Val, Len);
		Val = buf;
	}
	else SetSize(Len * Count, NULL);
	WCHAR *B = buf;
	while (Count && Len){
		memcpy(B, Val, Len * sizeof(WCHAR));
		B += Len;
		Count--;
	}
	return *this;
}//---------------------------------------------------------------------------

int MyUnStr::FromAscii(const char *val, size_t Count){
	if (val == NULL) return -1;
	int sz = MultiByteToWideChar(CP_ACP, 0, val, Count ? Count+1 : -1, NULL, 0);
	if (!sz) { Clear(); return 0;}
	SetSize(sz-1, NULL);
	return MultiByteToWideChar(CP_ACP, 0, val, Count ? Count+1 : -1, buf, sz)-1;
};//---------------------------------------------------------------------------

int MyUnStr::FromUTF8(const char *val, size_t Count){
	if (val==NULL)return -1;
	int sz = MultiByteToWideChar(CP_UTF8, 0, val, Count ? Count+1 : -1, buf, 0);
	if (!sz) { Clear(); return 0; }
	SetSize(sz-1, NULL);
	return MultiByteToWideChar(CP_UTF8, 0, val, Count ? Count+1 : -1, buf, sz)-1;
};//---------------------------------------------------------------------------

int MyUnStr::DecodeFrom(UINT Code, const char *val, size_t Count){
	if (val==NULL)return -1;
	int sz = MultiByteToWideChar(Code, 0, val, Count ? Count : -1, buf, 0);
	SetSize(Count? sz: sz - 1, NULL);
	return MultiByteToWideChar(Code, 0, val, Count ? Count : -1, buf, Length()+1)-1;
	};//---------------------------------------------------------------------------

MyAnciiStr MyUnStr::AnsiString(){ return MyAnciiStr(buf, Length()); };

MyAnciiStr MyUnStr::UTF8String(){
	int sz = WideCharToMultiByte(CP_UTF8, 0, buf, -1, NULL, 0, NULL, NULL);
	MyAnciiStr res;
	sz = WideCharToMultiByte(CP_UTF8, 0, buf, -1, res.ReceiveBuffer(sz - 1), sz, NULL, NULL) - 1;
	if (sz -1 != res.Length()) if (!MyStr_GenerateError) res.Length(sz);
	else throw TMyEx(21, sz == 0 ? "Не удалось конвертировать строку!" : "Не совпадает длинна строки при конвертации", "MyUnStr");
	return res;
	};//---------------------------------------------------------------------------

MyAnciiStr MyUnStr::DecodeTo(UINT Code){
	if (!Length()) return MyAnciiStr();
	int sz = WideCharToMultiByte(Code, 0, buf, Length()+1, NULL, 0, NULL, NULL);
	MyAnciiStr res;
	sz = WideCharToMultiByte(Code, 0, buf, Length()+1, res.ReceiveBuffer(sz-1), sz, NULL, NULL) - 1;
	if (sz != res.Length()) if (!MyStr_GenerateError) res.Length(sz>0? sz: 0);
	else throw TMyEx(21, sz < 0 ? ("Не удалось конвертировать строку: " + TextErrorAnsiiSTR(GetLastError())) : "Не совпадает длинна строки при конвертации", "MyUnStr");
	return res;
	};//---------------------------------------------------------------------------

MyUnStr MyUnStr::printf(WCHAR* format, ...){
	WCHAR bf[1024];
	va_list val;
	va_start(val, format);
	wvsprintfW(bf, format, val);
	va_end(val);
	Set(this, bf, 0, 0);
	return *this;
};//---------------------------------------------------------------------------

MyUnStr MyUnStr::printf(MyUnStr format, ...){
	WCHAR bf[1024];
	va_list val;
	va_start(val, format);
	wvsprintfW(bf, format.buf, val);
	va_end(val);
	return *this = bf;
};//---------------------------------------------------------------------------

int MyUnStr::ToInt(int sys){
	if (!buf) return 0;
	WCHAR *e;
	int r = wcstol(buf, &e, sys);
	if (*e!=0) { SetLastError(e-buf+1); Clear(); }
	return r;
};//---------------------------------------------------------------------------

double MyUnStr::ToFloat(){
	if (!buf) return 0;
	WCHAR *e;
	double r = wcstod(buf, &e);
	if (*e!=0) { SetLastError(e-buf+1); Clear(); }
	return r;
};//---------------------------------------------------------------------------

int MyUnStr::GetActualLenght(){
	return wcslen(buf);
};//---------------------------------------------------------------------------

MyUnStr MyUnStr::Concat_MyUnStr(size_t Count, ...){
	if (!Count) return MyUnStr();
	va_list val;
	va_start(val, Count);
	size_t Sz(0);
	for (size_t i(0); i<Count; i++) Sz += va_arg(val, MyUnStr).Length();
	va_start(val, Count);
	SetSize(Sz, NULL);
	MyUnStr *P;
	WCHAR *Buf = buf;
	for (size_t i(0); i<Count; i++) {
		P = &va_arg(val, MyUnStr);
		memcpy(Buf, P->buf, P->Length() * sizeof(WCHAR));
		Buf += P->Length();
		}
	va_end(val);
	return *this;
}//---------------------------------------------------------------------------

MyUnStr MyUnStr::Concat_MyUnStr(size_t Count, const MyUnStr* Array){
	if (!Count) return MyUnStr();
	size_t Sz(0);
	for (size_t i(0); i<Count; i++) Sz += Array[i].Length();
	SetSize(Sz, NULL);
	wchar_t *Buf = buf;
	for (size_t i(0); i<Count; i++){
		memcpy(Buf, Array[i].buf, Array[i].Length() * sizeof(WCHAR));
		Buf += Array[i].Length();
		}
	return *this;
}//---------------------------------------------------------------------------

MyUnStr MyUnStr::FromTime(_SYSTEMTIME* Val, DWORD Type){
	Init();
	int Sz = GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, Type, Val, NULL, NULL, NULL);
	GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, Type, Val, NULL, ReceiveBuffer(Sz-1), Sz);
	return *this;
}//////////////////////////////////////////////////////////////////////////

MyUnStr MyUnStr::FromDate(_SYSTEMTIME* Val, DWORD Type){
	Init();
	int Sz = GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, Type, Val, NULL, NULL, NULL, NULL);
	GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, Type, Val, NULL, ReceiveBuffer(Sz - 1), Sz, NULL);
	return *this;
}//////////////////////////////////////////////////////////////////////////

DWORD MyUnStr::LoadFile(MyUnStr FileName, bool Decode, UINT Code){
	HANDLE hFile;
	if ((hFile = CreateFileW(FileName, GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
		return GetLastError();
	int Sz = GetFileSize(hFile, NULL);
	MyAnciiStr _tmp;
	BOOL fg = ReadFile(hFile, _tmp.ReceiveBuffer(Sz+1), Sz, LPDWORD(&Sz), NULL);
	CloseHandle(hFile);
	if (Decode)	{
		if (DecodeFrom(Code, _tmp, Sz)<0)
		if (MyStr_GenerateError) throw TMyEx(10, TextErrorAnsiiSTR(GetLastError()), "MyUnStr"); 
		}	
	else { SetBuffer((WCHAR*)_tmp.Data(), Sz / sizeof(WCHAR)); _tmp.ThrowBuffer(); buf[Length()] = 0; }
	return 0;
	}//----------------------------------------------

DWORD MyUnStr::SaveToFile(MyUnStr FileName, bool Decode, UINT Code){
	if (Decode)	return DecodeTo(Code).SaveToFile(FileName.AnsiString());
	HANDLE hFile;
	if ((hFile = CreateFileW(FileName, GENERIC_WRITE, FILE_SHARE_READ,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
		return GetLastError();
	int Sz = Length() * sizeof(WCHAR), LR;
	WriteFile(hFile, (char*)buf, Sz, (DWORD*)&LR, NULL);
	CloseHandle(hFile);
	return Sz - LR;
	}//----------------------------------------------

/*	DWORD MyUnStr::SaveToFile(MyUnStr FileName, bool Decode, UINT Code){
	char *Buf;
	int Sz = Length() * sizeof(WCHAR), LR;
	if (Decode)	{
		MyAnciiStr _tmp = DecodeTo(Code);
		Sz = _tmp.Length();
		Buf = _tmp;
		_tmp.ThrowBuffer();
	}
	else Buf = (char*)buf;
	HANDLE hFile;
	if ((hFile = CreateFileW(FileName, GENERIC_WRITE, FILE_SHARE_READ,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
		throw TMyEx(2, TextErrorAnsiiSTR(GetLastError()), "MyUnStr");
	WriteFile(hFile, Buf, Sz, (DWORD*)&LR, NULL);
	if (Decode) delete[]Buf;
	CloseHandle(hFile);
	return LR == Sz;
}//----------------------------------------------*/




//--------------------------  MyAnciiStr --------------------------------------------
MyAnciiStr::~MyAnciiStr(){
	if (!dt) return;
	if (dt->ss == 0)	{
		delete[]buf;
		delete dt;
	}
	else dt->ss--;
}//---------------------------------------------------------------------------

MyAnciiStr::MyAnciiStr(MyUnStr val){ Init(); Set(this, val.Data(), val.Length()); };

void MyAnciiStr::Length(size_t val, bool Copy){
	if (val == Length()) return;
	SetSize(val, val && Copy ? buf : NULL);
}//---------------------------------------------------------------------------

void MyAnciiStr::Clear(){
	if (!dt)return;
	if (dt->ss)
	{
		dt->ss--;
		dt = NULL;
		buf = NULL;
	}
	else
	{
		if (buf) delete[] buf;
		buf = NULL;
		dt->ss = 0;
		dt->len = 0;
	}
}//---------------------------------------------------------------------------

bool MyAnciiStr::ThrowBuffer() {
	if (!dt) return false;
	buf = NULL;
	if (dt->ss)	{ dt->ss--;	dt = NULL; return true; }
	else { dt->ss = 0; dt->len = 0; return false; }
	}//////////////////////////////////////////////////////////////////////////

void MyAnciiStr::SetSize(size_t size, const char *old, size_t Count, bool FirstBuf){
	if(size + 1 == 0) throw TMyEx(100, "Превышение максимального размера буфера", "MyAnciiStr");
	if (!size && old) { size = strlen(old); if (!Count) Count = size; }
	if (!size) { Clear(); return; }
	char *h = size == Length() && dt && dt->ss == 0 ? buf : new char[size + 1];
	if (old == buf && buf == h) return;
	h[size] = 0;

	if (old) {
		if (!Count || Count > size) Count = min(size, strlen(old));
		if (FirstBuf) {
			memcpy(h, old, Count);
			if (size != Count) *(h + Count) = 0;
		}
		else memcpy(h + size - Count, old, Count);
	}
	else if (size) h[0] = 0;
	if (buf == h) return;
	Clear();
	if (!dt) dt = new StDt;
	dt->ss = 0;
	dt->len = size;
	buf = h;
};//---------------------------------------------------------------------------


void MyAnciiStr::Set(MyAnciiStr *STR, const MyAnciiStr  &val){
	if (STR == &val) return;
	STR->Clear();
	if (val.Length())
	{
		if (STR->dt) delete STR->dt;
		memcpy(STR, &val, sizeof(MyAnciiStr));
		STR->dt->ss++;
	}
}//---------------------------------------------------------------------------

void MyAnciiStr::Set(MyAnciiStr *STR, const char* buf, size_t Count, size_t Beg){
	if (!Count && !Beg && buf == STR->buf) return;
	STR->SetSize(Count, buf + Beg, Count);
}//---------------------------------------------------------------------------

void MyAnciiStr::Set(MyAnciiStr *STR, const WCHAR* buf, size_t Count, size_t Beg){
	if (STR->FromUnicode(buf + Beg, Count) <= 0) STR->Clear();
}//---------------------------------------------------------------------------

void MyAnciiStr::Set(MyAnciiStr *STR, const INT64 val, const int sys){
	if (sys <2) { STR->Clear(); return; }
	char* buf = new char[_CVTBUFSIZE];
	_i64toa_s(val, buf, _CVTBUFSIZE, sys);
	Set(STR, buf);
	delete[] buf;
}//---------------------------------------------------------------------------

void MyAnciiStr::Set(MyAnciiStr *STR, const double val, const int digits){
	char buf[_CVTBUFSIZE];
	int err = _gcvt_s(buf, _CVTBUFSIZE, val, digits);
	if (err != 0) {
		SetLastError(err);
		STR->Clear();
		return;
	}
	Set(STR, buf);
}//---------------------------------------------------------------------------

void MyAnciiStr::Set(MyAnciiStr *STR, const bool val){
	Set(STR, val ? "True" : "False", val ? 4 : 5);
}//---------------------------------------------------------------------------


void MyAnciiStr::Add(bool post, MyAnciiStr *STR, const MyAnciiStr  &val){
	if (!val.Length()) return;
	size_t sz = STR->Length();
	post = post || STR == &val;
	STR->SetSize(sz + val.dt->len, STR->buf, sz, post);
	if (post)	memcpy(STR->buf + sz, val.buf, (STR == &val ? sz : val.dt->len));
	else		memcpy(STR->buf, val.buf, (STR == &val ? sz : val.dt->len));
}//---------------------------------------------------------------------------

void MyAnciiStr::Add(bool post, MyAnciiStr *STR, const char* val, size_t Count, size_t Beg){
	size_t sz = Count ? Count : strlen(val + Beg);
	if (!sz) return;
	if (STR->buf == val)
	if (sz == STR->Length() && !Beg) { Add(post, STR, *STR); return; }
	else {
		MyAnciiStr _tmp(*STR);
		STR->SetSize(Count + STR->Length(), val + Beg, sz, !post);
		memcpy(post ? STR->buf : STR->buf + sz, _tmp.buf, _tmp.Length());
		return;
	}
	STR->SetSize(STR->Length() + sz, STR->buf, STR->Length(), post);
	if (post)	memcpy(STR->buf + STR->Length() - sz, val + Beg, sz);
	else		memcpy(STR->buf, val + Beg, sz);
}//---------------------------------------------------------------------------

void MyAnciiStr::Add(bool post, MyAnciiStr *STR, const WCHAR* buf, size_t Count, size_t Beg){
	MyAnciiStr _tmp;
	if(_tmp.FromUnicode(buf + Beg, Count) <= 0) return;
	Add(post, STR, _tmp);
}//---------------------------------------------------------------------------

void MyAnciiStr::Add(bool post, MyAnciiStr *STR, const  INT64 val, int sys){
	if (sys <2) { return; }
	char buf[_CVTBUFSIZE];
	_i64toa_s(val, buf, _CVTBUFSIZE, sys);
	Add(true, STR, buf);
}//---------------------------------------------------------------------------

void MyAnciiStr::Add(bool post, MyAnciiStr *STR, const double val, int digits){
	char buf[_CVTBUFSIZE];
	int err = _gcvt_s(buf, _CVTBUFSIZE, val, digits);
	if (err) {
		SetLastError(err);
		STR->Clear();
		return;
	}
	Add(true, STR, buf);
}//---------------------------------------------------------------------------

void MyAnciiStr::Add(bool post, MyAnciiStr *STR, const   bool val){
	Add(true, STR, val ? "True" : "False", val ? 4 : 5);
}//---------------------------------------------------------------------------


MyAnciiStr MyAnciiStr::SubStr(size_t Beg, size_t Count){
	if (Beg >= Length()) return MyAnciiStr();
	if (!Count || Count + Beg>Length()) Count = Length() - Beg;
	MyAnciiStr _tmp;
	Set(&_tmp, *this, Count, Beg);
	return _tmp;
};//---------------------------------------------------------------------------

MyAnciiStr MyAnciiStr::GetLine(size_t Beg){
	int Ps = Pos("\r\n", Beg);
	if (Ps < 0) Ps = Length();
	return SubStr(Beg, Ps - Beg);
}//---------------------------------------------------------------------------

int MyAnciiStr::Pos(const char* val, size_t Beg, size_t Len){
	if (!buf || Beg > Length()) return -1;
	if (Len == 0) Len = strlen(val);
	char *P = buf + Beg;
	while (P <= buf + Length() - Len){
		if (*P == val[0]){
			size_t i;
			for (i = 1; i < Len; i++)
			if (*(P + i) != val[i]) break;
			if (i == Len) return P - buf;
		}
		P++;
	}
	return -1;
};//---------------------------------------------------------------------------

int MyAnciiStr::PosOf(const char* val, size_t Beg, size_t Len){
	if (Beg >= Length()) return -1;
	if (Len == 0) Len = strlen(val);
	char *P = buf - Beg + Length() - Len;
	while (P >= buf){
		if (*P == val[0])
		{
			size_t i;
			for (i = 0; i<Len; i++)
			if (*(P + i) != val[i]) break;
			if (i == Len) return P - buf;
		}
		P--;
	}
	return -1;
};//---------------------------------------------------------------------------

size_t MyAnciiStr::Replase(char* Val, char* dest, size_t Len, size_t LenDest){
	if (!Val)return 0;
	if (!Len) Len = strlen(Val);
	if (!LenDest && dest) LenDest = strlen(dest);
	size_t Count(0);
	MyAnciiStr TMP;
	if (LenDest != Len){
		int PS(0);
		PS = Pos(Val, 0, Len);
		while (PS >= 0){
			PS += Len;
			PS = Pos(Val, PS, Len);
			Count++;
		}
		if (!Count) return 0;
		TMP.SetSize(Length() + (LenDest - Len) * Count, NULL);
		Count = 0;
	}
	else TMP = *this;
	char *S = buf, *B = S, *D = TMP.buf;
	while (*S != 0){
		if (*S == Val[0]){
			size_t i;
			for (i = 1; i<Len; i++)
			if (*(S + i) != Val[i]) break;
			if (i == Len) {
				memcpy(D, B, (S - B));
				D += (S - B);
				B += (S - B + Len);
				if (LenDest){ memcpy(D, dest, LenDest);	D += LenDest; }
				Count++;
			}
		}
		S++;
	}
	if (*B) memcpy(D, B, (S - B));
	*this = TMP;
	return Count;
}//---------------------------------------------------------------------------

MyAnciiStr MyAnciiStr::Clone(const char * Val, size_t Count, size_t Len){
	if (!Len) Len = strlen(Val);
	if (buf && buf == Val){
		SetSize(Len * Count, Val, Len);
		Val = buf;
	}
	else SetSize(Len * Count, NULL);
	char *B = buf;
	while (Count && Len){
		memcpy(B, Val, Len * sizeof(WCHAR));
		B += Len;
		Count--;
	}
	return *this;
}//---------------------------------------------------------------------------

int MyAnciiStr::FromUnicode(const WCHAR *val, size_t Count){
	if (val == NULL)return -1;
	int sz = WideCharToMultiByte(CP_ACP, 0, val, Count ? Count + 1 : -1, buf, 0, NULL, NULL);
	SetSize(sz - 1, NULL);
	return WideCharToMultiByte(CP_ACP, 0, val, Count ? Count + 1 : -1, buf, sz, NULL, NULL) - 1;
};//---------------------------------------------------------------------------

int MyAnciiStr::DecodeFrom(UINT Code, const WCHAR *val, size_t Count){
	if (val == NULL) return -1;
	int sz = WideCharToMultiByte(Code, 0, val, Count? Count + 1: -1, NULL, 0, NULL, NULL);
	SetSize(sz - 1, NULL);
	return WideCharToMultiByte(Code, 0, val, Count? Count: -1, buf, sz, NULL, NULL) - 1;
};//---------------------------------------------------------------------------

MyUnStr MyAnciiStr::DecodeTo(UINT Code){
	MyUnStr _tmp;
	_tmp.DecodeFrom(Code, buf, Length());
	return _tmp;
};//---------------------------------------------------------------------------

MyAnciiStr MyAnciiStr::printf(char* format, ...){
	char bf[1024];
	va_list val;
	va_start(val, format);
	wvsprintfA(bf, format, val);
	va_end(val);
	Set(this, bf, 0, 0);
	return *this;
};//---------------------------------------------------------------------------

MyAnciiStr MyAnciiStr::printf(MyAnciiStr format, ...){
	char bf[1024];
	va_list val;
	va_start(val, format);	
	wvsprintfA(bf, format.buf, val);
	va_end(val);
	return *this = bf;
};//---------------------------------------------------------------------------

int MyAnciiStr::ToInt(int sys){
	if (!buf) return 0;
	char *e;
	int r = strtol(buf, &e, sys);
	if (*e != 0) { SetLastError(e - buf + 1); Clear(); }
	return r;
};//---------------------------------------------------------------------------

double MyAnciiStr::ToFloat(){
	if (!buf) return 0;
	char *e;
	double r = strtod(buf, &e);
	if (*e != 0) { SetLastError(e - buf + 1); Clear(); }
	return r;
};//---------------------------------------------------------------------------

int MyAnciiStr::GetActualLenght(){
	return strlen(buf);
};//---------------------------------------------------------------------------

MyAnciiStr MyAnciiStr::Concat_MyAnciiStr(size_t Count, ...){
	if (!Count) return MyAnciiStr();
	va_list val;
	va_start(val, Count);
	size_t Sz(0);
	for (size_t i(0); i<Count; i++) Sz += va_arg(val, MyAnciiStr).Length();
	va_start(val, Count);
	SetSize(Sz, NULL);
	MyAnciiStr *P;
	char *Buf = buf;
	for (size_t i(0); i<Count; i++) {
		P = &va_arg(val, MyAnciiStr);
		memcpy(Buf, P->buf, P->Length());
		Buf += P->Length();
	}
	va_end(val);
	return *this;
}//---------------------------------------------------------------------------

MyAnciiStr MyAnciiStr::Concat_MyAnciiStr(size_t Count, const MyAnciiStr* Array){
	if (!Count) return MyAnciiStr();
	size_t Sz(0);
	for (size_t i(0); i<Count; i++) Sz += Array[i].Length();
	SetSize(Sz, NULL);
	char *Buf = buf;
	for (size_t i(0); i<Count; i++){
		memcpy(Buf, Array[i].buf, Array[i].Length() * sizeof(WCHAR));
		Buf += Array[i].Length();
	}
	return *this;
}//---------------------------------------------------------------------------

MyAnciiStr MyAnciiStr::FromTime(_SYSTEMTIME* Val, DWORD Type){
	Init();
	int Sz = GetTimeFormatA(LOCALE_NAME_USER_DEFAULT, Type, Val, NULL, NULL, NULL);
	GetTimeFormatA(LOCALE_NAME_USER_DEFAULT, Type, Val, NULL, ReceiveBuffer(Sz-1), Sz);
	return *this;
}//////////////////////////////////////////////////////////////////////////

MyAnciiStr MyAnciiStr::FromDate(_SYSTEMTIME* Val, DWORD Type){
	Init();
	int Sz = GetDateFormatA(LOCALE_NAME_USER_DEFAULT, Type, Val, NULL, NULL, NULL);
	GetDateFormatA(LOCALE_NAME_USER_DEFAULT, Type, Val, NULL, ReceiveBuffer(Sz - 1), Sz);
	return *this;
}//////////////////////////////////////////////////////////////////////////

DWORD MyAnciiStr::LoadFile(MyAnciiStr FileName, bool Decode, UINT Code){
	HANDLE hFile;
	if ((hFile = CreateFileA(FileName, GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
		return GetLastError();
	int Sz = GetFileSize(hFile, NULL);
	SetSize(Sz, NULL);
	BOOL fg = ReadFile(hFile, buf, Sz, LPDWORD(&Sz), NULL);
	CloseHandle(hFile);
	if (Decode)	*this = DecodeTo(Code).AnsiString();
	return 0;
}//----------------------------------------------

DWORD MyAnciiStr::SaveToFile(MyAnciiStr FileName, bool Decode, UINT Code){
	if (Decode) return DecodeTo(Code).SaveToFile(FileName.UnicodeString());

	HANDLE hFile;
	if ((hFile = CreateFileA(FileName, GENERIC_WRITE, FILE_SHARE_READ,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE)
		return GetLastError();
	int LR;
	WriteFile(hFile, buf, Length(), (DWORD*)&LR, NULL);
	CloseHandle(hFile);
	return Length() - LR;
}//----------------------------------------------



MyAnciiStr TextErrorAnsiiSTR(DWORD Code){
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |FORMAT_MESSAGE_FROM_SYSTEM, NULL, Code,
				  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Язык по умолчанию
				  (LPTSTR)&lpMsgBuf, 0, NULL);
	MyAnciiStr _tmp = (LPTSTR)lpMsgBuf;
	LocalFree(lpMsgBuf);
	return _tmp;
	}


//--------------------------  FreeObj --------------------------------------------

FreeObj::FreeObj(const FreeObj &Val){
	memcpy(this, &Val, sizeof(FreeObj));
	if(dt) dt->ss++;
	}

FreeObj::FreeObj(const FreeObj* Val){
	Init();
	SetPointer(Val);
	}

void FreeObj::NewDt(){
	if (!dt) {
		dt = new StDt;
		dt->len = 0;
		dt->ss = 0;
		}
	else if (dt->ss) { Clear(); NewDt(); }
	}

void FreeObj::Clear(const bool dest){
	if (!dt)return;
	if (dt->ss){
		dt->ss--;
		if (!dest) { dt = NULL; buf = NULL; }
		}
	else{
		if (buf) delete[] buf;
		if (!dest){
			buf = NULL;
			dt->ss = 0;
			dt->len = 0;
			}
		else delete dt;
		}
	}

void FreeObj::SetSize(size_t size){
	if (!size) { Clear(); return; }
	if (dt && dt->len == size) return;
	NewDt();
	dt->len = size;
	buf = new char[dt->len];
	}

void FreeObj::Set(void *Dt, int Length){
	SetSize(Length);
	if (!Length) return;
	memcpy(buf, Dt, dt->len);
	}

void FreeObj::Сapture(void *Dt, int Length){
	NewDt();
	buf = Dt;
	dt->len = Length;
	}

void* FreeObj::Data(){ return buf; }

FreeObj* FreeObj::GetPointer(bool destroy){
	if (!dt) NewDt();
	FreeObj* _tmp = new FreeObj;
	memcpy(_tmp, this, sizeof(FreeObj));	
	buf = NULL;
	dt = NULL;
	return _tmp;
	}

void FreeObj::SetPointer(const FreeObj* Val){
	Clear(true);
	memcpy(this, Val, sizeof(FreeObj));
	delete (void*)Val;
	}

const FreeObj& FreeObj::operator =(const FreeObj& val){
	Clear(true);
	memcpy(this, &val, sizeof(FreeObj));
	if (dt) dt->ss++;
	return *this; }

const FreeObj& FreeObj::operator =(const FreeObj* val){ SetPointer(val); return *this; };






