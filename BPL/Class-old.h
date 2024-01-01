#ifndef MyStrClass
#define MyStrClass

#include <windows.h>
#include "MyExcept.h"

class MyAnciiStr;

struct StDt{
	unsigned int len;
	unsigned char ss;
	};

bool MyStrGenerateError();
void MyStrGenerateError(bool val);

//////////////////////////////////////   MyUnStr   ////////////////////////////////////

class MyUnStr{
private: 
	StDt *dt;
	WCHAR *buf;
	void Init(){ buf = NULL; dt = NULL; };
	void SetSize(size_t size, const WCHAR *old, size_t sb = 0, bool FirstBuf = true);

	static void Set(MyUnStr *STR, const MyUnStr &val);
	static void Set(MyUnStr *STR, const WCHAR* buf, size_t Count = 0, size_t Beg = 0);
	static void Set(MyUnStr *STR, const  char* buf, size_t Count = 0, size_t Beg = 0);	
	static void Set(MyUnStr *STR, const  INT64 val, const int sys = 10);
	static void Set(MyUnStr *STR, const double val, const int digits = 15);
	static void Set(MyUnStr *STR, const   bool val);

	static void Add(bool post, MyUnStr *STR, const MyUnStr  &val);
	static void Add(bool post, MyUnStr *STR, const WCHAR* buf, size_t Count = 0, size_t Beg = 0);
	static void Add(bool post, MyUnStr *STR, const  char* buf, size_t Count = 0, size_t Beg = 0);
	static void Add(bool post, MyUnStr *STR, const  INT64 val, int sys = 10);
	static void Add(bool post, MyUnStr *STR, const double val, int digits = 15);
	static void Add(bool post, MyUnStr *STR, const   bool val);

	double ToFloat();
public:

	~MyUnStr();

	inline MyUnStr(void){ Init(); };	
	inline MyUnStr(const MyUnStr& val)	{ Init(); Set(this, val); };	
	inline MyUnStr(const WCHAR* val, size_t Count = 0, size_t Beg = 0)	{ Init(); Set(this, val, Count, Beg); };
	inline MyUnStr(const  char* val, size_t Count = 0, size_t Beg = 0)	{ Init(); Set(this, val, Count, Beg); };
	inline MyUnStr(const  WCHAR val)	{ Init(); Set(this, &val, 1); };
	inline MyUnStr(const   char val)	{ Init(); Set(this, &val, 1); };
	inline MyUnStr(const   bool val)	{ Init(); Set(this, val); };	
	inline MyUnStr(const  INT64 val, const int sys = 10)		{ Init(); Set(this, val, sys); };
	inline MyUnStr(const	  int val, const int sys = 10)		{ Init(); Set(this, (INT64)val, sys); };
	inline MyUnStr(const double val, const int digits = 15)	{ Init(); Set(this, val, 1); };
	inline MyUnStr(const DWORD val)	{ Init(); Set(this, (INT64)val); };	
	MyUnStr(MyAnciiStr val);

	inline const MyUnStr& operator =(const MyUnStr& val) { Set(this, val);  return *this; };
	inline const MyUnStr& operator =(const WCHAR* val) { Set(this, val);  return *this; };
	inline const MyUnStr& operator =(const char*  val) { Set(this, val);  return *this; };
	inline const MyUnStr& operator =(const WCHAR  val) { Set(this, &val, 1);  return *this; };
	inline const MyUnStr& operator =(const char   val) { Set(this, &val, 1);  return *this; };
	inline const MyUnStr& operator =(const bool   val) { Set(this, val);  return *this; };
	inline const MyUnStr& operator =(const INT64  val) { Set(this, val);  return *this; };
	inline const MyUnStr& operator =(const int    val) { Set(this, (INT64)val);  return *this; };
	inline const MyUnStr& operator =(const double val) { Set(this, val);  return *this; };
	inline const MyUnStr& operator =(const DWORD  val) { Set(this, (INT64)val);  return *this; };

	inline const MyUnStr& operator +=(const MyUnStr& val) { Add(true, this, val);  return *this; };
	inline const MyUnStr& operator +=(const WCHAR* val) { Add(true, this, val);  return *this; };
	inline const MyUnStr& operator +=(const  char* val) { Add(true, this, val);  return *this; };
	inline const MyUnStr& operator +=(const  WCHAR val) { Add(true, this, &val, 1);  return *this; };
	inline const MyUnStr& operator +=(const   char val) { Add(true, this, &val, 1);  return *this; };
	inline const MyUnStr& operator +=(const	bool val) { Add(true, this, val);  return *this; };
	inline const MyUnStr& operator +=(const  INT64 val) { Add(true, this, val);  return *this; };
	inline const MyUnStr& operator +=(const	 int val) { Add(true, this, (INT64)val);  return *this; };
	inline const MyUnStr& operator +=(const double val) { Add(true, this, val);  return *this; };

	inline MyUnStr& operator <<(const MyUnStr& val) { Add(true, this, val);  return *this; };
	inline MyUnStr& operator <<(const WCHAR* val) { Add(true, this, val);  return *this; };
	inline MyUnStr& operator <<(const  char* val) { Add(true, this, val);  return *this; };
	inline MyUnStr& operator <<(const	 WCHAR val) { Add(true, this, &val, 1);  return *this; };
	inline MyUnStr& operator <<(const   char val) { Add(true, this, &val, 1);  return *this; };
	inline MyUnStr& operator <<(const	  bool val) { Add(true, this, val);  return *this; };
	inline MyUnStr& operator <<(const  INT64 val) { Add(true, this, val);  return *this; };
	inline MyUnStr& operator <<(const	   int val) { Add(true, this, (INT64)val);  return *this; };
	inline MyUnStr& operator <<(const double val) { Add(true, this, val);  return *this; };

	inline friend MyUnStr operator +(const MyUnStr& val1, const MyUnStr& val2)	{ MyUnStr _tmp(val1); Add(true, &_tmp, val2); return _tmp; };
	template <class Type>
	inline friend MyUnStr operator +(const MyUnStr& val1, const  Type& val2)	{ MyUnStr _tmp(val1); Add(true, &_tmp, MyUnStr(val2)); return _tmp; };
	template <class Type>
	inline friend MyUnStr operator +(const  Type& val1, const MyUnStr& val2)	{ MyUnStr _tmp(val2); Add(false, &_tmp, MyUnStr(val1)); return _tmp; };


	const MyUnStr& operator --(){ Length() > 1 ? Length(Length() - 1) : Clear(); return *this; };

	/*template <class Type>
	friend int operator ==(MyUnStr& val1, const Type& val2){ return val1.Compare(MyUnStr(val2)) == 0; };
	template <class Type>
	friend int operator <(MyUnStr& val1, const Type& val2){ return val1.Compare(MyUnStr(val2)) < 0; };
	template <class Type>
	friend int operator >(MyUnStr& val1, const Type& val2){ return val1.Compare(MyUnStr(val2)) > 0;; };*/

	operator WCHAR*() { return Data(); }
	operator int() { return ToInt(); }
	operator unsigned int() { return ToInt(); }
	operator double() { ToFloat(); };
	operator LPARAM(){ return (LPARAM)Data(); };

	MyUnStr SubStr(size_t Beg, size_t Count = 0);
	MyUnStr GetLine(size_t Beg);
//	MyUnStr* GetLines(size_t &Count);	

	inline WCHAR* Data(){ return buf? buf: L""; };
	// Создает новую строку не уничтожая буфер, указывает есть ли еще на него cсылки
	bool ThrowBuffer();
	// Берет буфер в свое распоряжение, буфер должен быть создан при помощи new
	void SetBuffer(WCHAR* Buf, UINT Size = 0){ Clear(); buf = Buf; if (!dt) dt = new StDt; dt->ss = 0; dt->len = Size? Size: (int)wcslen(Buf); };

	// получает буфер указанного размера
	inline WCHAR* ReceiveBuffer(size_t Size) { SetSize(Size, NULL); return Data(); };
	inline void* ReceiveVoidBuffer(size_t Size) { SetSize(Size*sizeof(WCHAR), NULL); return Data(); };
	
	//	Установка части строки
	inline void SetCut(const MyUnStr &val, size_t Count, size_t Beg = 0)	{ Set(this, val.buf, Count, Beg); };
	inline void SetCut(WCHAR *val, size_t Count, size_t Beg = 0)	{ Set(this, val, Count, Beg); };
	inline void SetCut(const char *val, size_t Count, size_t Beg = 0)	{ Set(this, val, Count, Beg); };
	
	//	Добавление части строки
	inline void AddCut(const MyUnStr &val, size_t Count, size_t Beg = 0)	{ Add(true, this, val.buf, Count, Beg); };
	inline void AddCut(WCHAR *val, size_t Count, size_t Beg = 0)	{ Add(true, this, val, Count, Beg); };
	inline void AddCut(const char *val, size_t Count, size_t Beg = 0)	{ Add(true, this, val, Count, Beg); };
	
	void Length(size_t val, bool Copy = true);
	inline size_t Length()const { return dt == NULL ? 0 : dt->len; };
	// Возвращает размер буфера в байтах
	inline size_t FullLength()const { return dt == NULL ? 0 : (dt->len + 1) * sizeof(WCHAR); };
	void Clear();
	inline bool isEmpty(){ return Length() == 0; };
	int GetActualLenght();
	int Pos(const WCHAR* val, size_t Beg = 0, size_t Len = 0);
	int PosOf(const WCHAR* val, size_t Beg = 0, size_t Len = 0);
	inline int Pos(MyUnStr& val, size_t Beg = 0){ return Pos(val.Data(), Beg, val.Length()); };
	inline int PosOf(MyUnStr& val, size_t Beg = 0){ return PosOf(val.Data(), Beg, val.Length()); };
	int ToInt(int sys = 10);
	size_t Replase(WCHAR* Val, WCHAR* dest, size_t Len = 0, size_t LenDest = 0);
	inline size_t Replase(MyUnStr Val, MyUnStr dest){ return Replase(Val.Data(), dest.Data(), Val.Length(), dest.Length()); };
	MyUnStr Clone(const WCHAR * Val, size_t Count, size_t Length = 0);
	inline MyUnStr Clone(MyUnStr Val, size_t Count){ return Clone(Val.Data(), Count, Val.Length()); };

	inline int Compare(const MyUnStr &val){ return wcscmp(buf, val.buf); }

	template <class Type>
	void DisplayArray(const size_t Length, const Type* val, const MyUnStr separator);

	// изменение кодировок
	int FromAscii(const char *val, size_t Count = 0);
	int FromUTF8(const char *val, size_t Count = 0);
	int DecodeFrom(UINT Code, const char *val, size_t Count = 0);
	MyAnciiStr AnsiString();
	MyAnciiStr UTF8String();
	MyAnciiStr DecodeTo(UINT Code);
	//////////////////////////////////////////////////////////////////////////

	MyUnStr printf(WCHAR* format, ...);
	MyUnStr printf(MyUnStr format, ...);

	MyUnStr Concat_MyUnStr(size_t Count, ...);
	MyUnStr Concat_MyUnStr(size_t Count, const MyUnStr* Array);
	MyUnStr FromTime(_SYSTEMTIME* Val, DWORD Type = LOCALE_USE_CP_ACP);
	MyUnStr FromDate(_SYSTEMTIME* Val, DWORD Type = LOCALE_USE_CP_ACP);

	DWORD LoadFile(MyUnStr FileName, bool Decode = false, UINT Code = CP_ACP);
	DWORD SaveToFile(MyUnStr FileName, bool Decode = false, UINT Code = CP_ACP);
};

template <class Type>
void MyUnStr::DisplayArray(size_t Length, const Type* val, const MyUnStr separator){
	Clear();
	if (!Length || !val) return;

	MyUnStr *_Tmp = new MyUnStr[Length * 2 - 1];
	for (size_t i = 0; i < Length; i++)	{
		_Tmp[i * 2] = MyUnStr(val[i]);
		if (Length - i >1) _Tmp[i * 2 + 1] = separator;
		}
	Concat_MyUnStr(Length * 2 - 1, _Tmp);
	delete[]_Tmp;
	}//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////   MyAnciiStr   ////////////////////////////////////
class MyAnciiStr{
private:
	StDt *dt;
	char *buf;
	void Init(){ buf = NULL; dt = NULL; };
	void SetSize(size_t size, const char *old, size_t sb = 0, bool FirstBuf = true);

	static void Set(MyAnciiStr *STR, const MyAnciiStr  &val);
	static void Set(MyAnciiStr *STR, const  char* buf, size_t Count = 0, size_t Beg = 0);
	static void Set(MyAnciiStr *STR, const WCHAR* buf, size_t Count = 0, size_t Beg = 0);
	static void Set(MyAnciiStr *STR, const  INT64 val, const int sys = 10);
	static void Set(MyAnciiStr *STR, const double val, const int digits = 15);
	static void Set(MyAnciiStr *STR, const   bool val);

	static void Add(bool post, MyAnciiStr *STR, const MyAnciiStr  &val);
	static void Add(bool post, MyAnciiStr *STR, const  char* buf, size_t Count = 0, size_t Beg = 0);
	static void Add(bool post, MyAnciiStr *STR, const WCHAR* buf, size_t Count = 0, size_t Beg = 0);
	static void Add(bool post, MyAnciiStr *STR, const  INT64 val, int sys = 10);
	static void Add(bool post, MyAnciiStr *STR, const double val, int digits = 15);
	static void Add(bool post, MyAnciiStr *STR, const   bool val);
	
	double ToFloat();
public:
	~MyAnciiStr();

	inline MyAnciiStr(void){ Init(); };
	inline MyAnciiStr(const MyAnciiStr& val)	{ Init(); Set(this, val); };
	inline MyAnciiStr(const	 char* val, size_t Count = 0, size_t Beg = 0)	{ Init(); Set(this, val, Count, Beg); };
	inline MyAnciiStr(const WCHAR* val, size_t Count = 0, size_t Beg = 0)	{ Init(); Set(this, val, Count, Beg); };
	inline MyAnciiStr(const   char val)	{ Init(); Set(this, &val, 1); };
	inline MyAnciiStr(const  WCHAR val)	{ Init(); Set(this, &val, 1); };
	inline MyAnciiStr(const   bool val)	{ Init(); Set(this, val); };
	inline MyAnciiStr(const  INT64 val, const int sys = 10)		{ Init(); Set(this, val, sys); };
	inline MyAnciiStr(const	   int val, const int sys = 10)		{ Init(); Set(this, (INT64)val, sys); };
	inline MyAnciiStr(const double val, const int digits = 15)	{ Init(); Set(this, val, 1); };
	inline MyAnciiStr(const  DWORD val)	{ Init(); Set(this, (INT64)val); };
	MyAnciiStr(MyUnStr val);

	inline const MyAnciiStr& operator =(const MyAnciiStr& val) { Set(this, val);  return *this; };
	inline const MyAnciiStr& operator =(const char*  val) { Set(this, val);  return *this; };
	inline const MyAnciiStr& operator =(const WCHAR* val) { Set(this, val);  return *this; };
	inline const MyAnciiStr& operator =(const char   val) { Set(this, &val, 1);  return *this; };
	inline const MyAnciiStr& operator =(const WCHAR  val) { Set(this, &val, 1);  return *this; };
	inline const MyAnciiStr& operator =(const bool   val) { Set(this, val);  return *this; };
	inline const MyAnciiStr& operator =(const INT64  val) { Set(this, val);  return *this; };
	inline const MyAnciiStr& operator =(const int    val) { Set(this, (INT64)val);  return *this; };
	inline const MyAnciiStr& operator =(const double val) { Set(this, val);  return *this; };
	inline const MyAnciiStr& operator =(const DWORD  val) { Set(this, (INT64)val);  return *this; };

	inline const MyAnciiStr& operator +=(const MyAnciiStr& val) { Add(true, this, val);  return *this; };
	inline const MyAnciiStr& operator +=(const  char* val) { Add(true, this, val);  return *this; };
	inline const MyAnciiStr& operator +=(const WCHAR* val) { Add(true, this, val);  return *this; };
	inline const MyAnciiStr& operator +=(const   char val) { Add(true, this, &val, 1);  return *this; };
	inline const MyAnciiStr& operator +=(const  WCHAR val) { Add(true, this, &val, 1);  return *this; };
	inline const MyAnciiStr& operator +=(const	 bool val) { Add(true, this, val);  return *this; };
	inline const MyAnciiStr& operator +=(const  INT64 val) { Add(true, this, val);  return *this; };
	inline const MyAnciiStr& operator +=(const	  int val) { Add(true, this, (INT64)val);  return *this; };
	inline const MyAnciiStr& operator +=(const double val) { Add(true, this, val);  return *this; };

	inline MyAnciiStr& operator <<(const MyAnciiStr& val) { Add(true, this, val);  return *this; };
	inline MyAnciiStr& operator <<(const  char* val) { Add(true, this, val);  return *this; };
	inline MyAnciiStr& operator <<(const WCHAR* val) { Add(true, this, val);  return *this; };
	inline MyAnciiStr& operator <<(const   char val) { Add(true, this, &val, 1);  return *this; };
	inline MyAnciiStr& operator <<(const  WCHAR val) { Add(true, this, &val, 1);  return *this; };
	inline MyAnciiStr& operator <<(const   bool val) { Add(true, this, val);  return *this; };
	inline MyAnciiStr& operator <<(const  INT64 val) { Add(true, this, val);  return *this; };
	inline MyAnciiStr& operator <<(const	int val) { Add(true, this, (INT64)val);  return *this; };
	inline MyAnciiStr& operator <<(const double val) { Add(true, this, val);  return *this; };

	inline friend MyAnciiStr operator +(const MyAnciiStr& val1, const MyAnciiStr& val2)	{ MyAnciiStr _tmp(val1); Add(true, &_tmp, val2); return _tmp; };
	template <class Type>
	inline friend MyAnciiStr operator +(const MyAnciiStr& val1, const  Type& val2)	{ MyAnciiStr _tmp(val1); Add(true, &_tmp, MyAnciiStr(val2)); return _tmp; };
	template <class Type>
	inline friend MyAnciiStr operator +(const  Type& val1, const MyAnciiStr& val2)	{ MyAnciiStr _tmp(val2); Add(false, &_tmp, MyAnciiStr(val1)); return _tmp; };


	const MyAnciiStr& operator --(){ Length() > 1 ? Length(Length() - 1) : Clear(); return *this; };

	template <class Type>
	friend int operator ==(MyAnciiStr& val1, const Type& val2){ return val1.Compare(MyAnciiStr(val2)) == 0; };
	template <class Type>
	friend int operator <(MyAnciiStr& val1, const Type& val2){ return val1.Compare(MyAnciiStr(val2)) < 0; };
	template <class Type>
	friend int operator >(MyAnciiStr& val1, const Type& val2){ return val1.Compare(MyAnciiStr(val2)) > 0;; };

	operator char*() { return Data(); }
	operator int() { return ToInt(); }
	operator unsigned int() { return ToInt(); }
	operator double() { ToFloat(); };
	operator LPARAM(){ return (LPARAM)Data(); };

	MyAnciiStr SubStr(size_t Beg, size_t Count = 0);
	MyAnciiStr GetLine(size_t Beg);
	//	MyAnciiStr* GetLines(size_t &Count);	

	inline char* Data(){ return buf? buf: ""; };
	// Создает новую строку не уничтожая буфер, указывает есть ли еще на него сылки
	bool ThrowBuffer();
	// Берет буфер в свое распоряжение, буфер должен быть создан при помощи new
	void SetBuffer(char* Buf, unsigned int Size = 0){ Clear(); buf = Buf; if (!dt) dt = new StDt; dt->ss = 0; dt->len = Size? Size: (UINT)strlen(buf); };

	// получает буфер указанного размера
	inline char* ReceiveBuffer(size_t Size) { SetSize(Size, NULL); return Data(); };
	inline void* ReceiveVoidBuffer(size_t Size) { SetSize(Size-1, NULL); return Data(); };

	//	Установка части строки
	inline void SetCut(const MyAnciiStr &val, size_t Count, size_t Beg = 0)	{ Set(this, val.buf, Count, Beg); };
	inline void SetCut(char *val, size_t Count, size_t Beg = 0)	{ Set(this, val, Count, Beg); };
	inline void SetCut(const char *val, size_t Count, size_t Beg = 0)	{ Set(this, val, Count, Beg); };

	//	Добавление части строки
	inline void AddCut(const MyAnciiStr &val, size_t Count, size_t Beg = 0)	{ Add(true, this, val.buf, Count, Beg); };
	inline void AddCut(char *val, size_t Count, size_t Beg = 0)	{ Add(true, this, val, Count, Beg); };
	inline void AddCut(const char *val, size_t Count, size_t Beg = 0)	{ Add(true, this, val, Count, Beg); };

	void Length(size_t val, bool Copy = true);
	inline size_t Length()const { return dt == NULL ? 0 : dt->len; };
	//Возвращает полный размер буфера
	inline size_t FullLength()const { return dt == NULL ? 0 : dt->len + 1; };
	void Clear();
	inline bool isEmpty(){ return Length() == 0; };
	int GetActualLenght();
	
	int Pos(const char* val, size_t Beg = 0, size_t Len = 0);
	int PosOf(const char* val, size_t Beg = 0, size_t Len = 0);
	inline int Pos(MyAnciiStr& val, size_t Beg = 0){ return Pos(val.Data(), Beg, val.Length()); };
	inline int PosOf(MyAnciiStr& val, size_t Beg = 0){ return PosOf(val.Data(), Beg, val.Length()); };
	int ToInt(int sys = 10);
	size_t Replase(char* Val, char* dest, size_t Len = 0, size_t LenDest = 0);
	inline size_t Replase(MyAnciiStr Val, MyAnciiStr dest){ return Replase(Val.Data(), dest.Data(), Val.Length(), dest.Length()); };
	MyAnciiStr Clone(const char * Val, size_t Count, size_t Length = 0);
	inline MyAnciiStr Clone(MyAnciiStr Val, size_t Count){ return Clone(Val.Data(), Count, Val.Length()); };

	inline int Compare(const MyAnciiStr &val){return strcmp(buf, val.buf); }

	template <class Type>
	void DisplayArray(const size_t Length, const Type* val, const MyAnciiStr separator);

	// изменение кодировок
	int FromUnicode(const WCHAR *val, size_t Count = 0);
	int DecodeFrom(UINT Code, const WCHAR *val, size_t Count = 0);
	MyUnStr UnicodeString() {return MyUnStr(buf, Length());};
	MyUnStr DecodeTo(UINT Code);
	//////////////////////////////////////////////////////////////////////////

	MyAnciiStr printf(char* format, ...);
	MyAnciiStr printf(MyAnciiStr format, ...);

	MyAnciiStr Concat_MyAnciiStr(size_t Count, ...);
	MyAnciiStr Concat_MyAnciiStr(size_t Count, const MyAnciiStr* Array);

	MyAnciiStr FromTime(_SYSTEMTIME* Val, DWORD Type = LOCALE_USE_CP_ACP);
	MyAnciiStr FromDate(_SYSTEMTIME* Val, DWORD Type = LOCALE_USE_CP_ACP);

	DWORD LoadFile(MyAnciiStr FileName, bool Decode = false, UINT Code = CP_ACP);
	DWORD SaveToFile(MyAnciiStr FileName, bool Decode = false, UINT Code = 1200); //CP_WINUNICODE
};

template <class Type>
void MyAnciiStr::DisplayArray(size_t Length, const Type* val, const MyAnciiStr separator){
	Clear();
	if (!Length || !val) return;

	MyAnciiStr *_Tmp = new MyAnciiStr[Length * 2 - 1];
	for (size_t i = 0; i < Length; i++)	{
		_Tmp[i * 2] = MyAnciiStr(val[i]);
		if (Length - i >1) _Tmp[i * 2 + 1] = separator;
	}
	Concat_MyAnciiStr(Length * 2 - 1, _Tmp);
	delete[]_Tmp;
}//////////////////////////////////////////////////////////////////////////

#ifdef UNICODE
#define MyStr  MyUnStr
#else
#define MyStr  MyAnciiStr
#endif

MyAnciiStr TextErrorAnsiiSTR(DWORD Code);

class FreeObj{
private:
	StDt *dt;
	void *buf;
	void Init(){ buf = NULL; dt = NULL; };	
	void NewDt();
	void SetSize(size_t size);
public:	
	FreeObj(){Init();}
	FreeObj(const FreeObj &Val);
	FreeObj(const FreeObj* Val);
	~FreeObj(){	Clear(true);}

	void Clear(const bool dest = false);
	size_t Length() { return dt? dt->len: 0; }
	void* Data();
	//Копирует из указателя в буфер Length байт
	void Set(void *Data, int Length);
	//Захватывает буфер размером Length байт(буфер будет уничтжен)
	void Сapture(void *Data, int Length);
	//Создает буфер указанного размера и возвращает на него указатель
	inline void* GetBuffer(size_t Val){ SetSize(Val); return buf; };
	/*Возвращает указатель на данные и не уничтожает их после уничтожения, 
	если destroy истина, сразу уничтожает свою ссылку на данные*/
	FreeObj* GetPointer(bool destroy = false);
	//Захватывает во владение указатель созданный с помощью GetPointer, с последующим удаление
	void SetPointer(const FreeObj* Val);

	const FreeObj& operator =(const FreeObj& val);
	const FreeObj& operator =(const FreeObj* val);

	//Помещает переменную в буфер, после этого ее нужно обязательно извлечь с помощью Extract
	template <class Type>
	void Put(Type val);

	//Копирует данные из переменной в свой буфер
	template <class Type>
	void Set(Type val);

	//Извлекает переменную из буфера, помещенную с помощью Put
	template <class Type>
	bool Extract(Type &Val);

	//Преобразовывает значение буфера к указанному типу (не уничтожая его)
	template <class Type>
	operator Type();
	};

template <class Type>
void FreeObj::Put(Type val){
	NewDt();
	dt->len = sizeof(Type);
	buf = new Type;
	*(Type*)buf = val;
	}

template <class Type>
void FreeObj::Set(Type val){
	SetSize(sizeof(val));
	if (!Length()) return;
	memcpy(buf, &val, dt->len);
	}

template <class Type>
bool FreeObj::Extract(Type &Val){
	if (sizeof(Type) != Length()) return false;
	Type* _tmp = (Type*)buf;
	Val = *_tmp;
	delete _tmp;
	buf = NULL;
	Clear();
	return true;
	}

template <class Type>
FreeObj::operator Type(){
	return *(Type*)buf;
	}

#include "Class-old.cpp"
#endif