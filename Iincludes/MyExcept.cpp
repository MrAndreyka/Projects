#pragma hdrstop

#include "MyExcept.h"
#include <string.h>

typedef unsigned char byte;

void Aut::operator =(const Aut &val){
	if (this==&val)return;
	Clear();
	if (!val.data)return;
	cs = val.cs;
	data = val.data;
	*cs += 1;
	return;
	}//---------------------------------------------------------------------------

void Aut::Clear(){
	if (!cs)return;
	if (*cs<2&&data)
		{
		delete[] data;
		delete cs;
		data = NULL;
		cs = NULL;
		}
	else *cs -= 1;
	};//---------------------------------------------------------------------------

void Aut::SetS(char *s){
	Clear();
	if (!s)return;
	cs = new int;
	*cs = 1;
	data = new char[strlen(s)+1];
	strcpy_s(data, strlen(s)+1, s);
};//---------------------------------------------------------------------------

TMyEx::TMyEx(TMyEx &val){
	*val.ss += 1;
	memcpy(this, &val, sizeof(val));
};//---------------------------------------------------------------------------

TMyEx::TMyEx(int eCode, const char *vStr, const char *ClassName, void *vData, int Dsz){
	Code = eCode;
	ss = new int;
	*ss = 0;
	size_t Sz;
	if (ClassName){
		Sz = strlen(ClassName) + 1;
		ClName = new char[Sz];
		strcpy_s(ClName, Sz, ClassName);
		}
	else ClName = nullptr;
	if (vStr){
		Sz = strlen(vStr) + 1;
		Str = new char[Sz];
		strcpy_s(Str, Sz, vStr);
		}
	else Str = nullptr;
	if (Dsz>0)
		{
		Data = new byte[Dsz];
		memcpy(Data, vData, Dsz);
		}
	else Data = vData;
	};//---------------------------------------------------------------------------

TMyEx::~TMyEx(){
	if (*ss==0)
		{
		delete[]Str;
		delete[]Data;
		delete[]ClName;
		delete ss;
		}
	else *ss -= 1;
	};//---------------------------------------------------------------------------