#ifndef SmatrClasses
#define SmatrClasses

#include <windows.h>
#include "MyExcept.h"
#include "WCHAR.h"
#include "myInit.h"

#ifdef _DEBUG
	//#define _SmartStr_mem
#endif

#ifdef _SmartStr_mem
#include <list>
#include <iostream>
#include <algorithm>

struct SBufer_mem {
	void* LP;
	bool isFree;
	size_t Size, nLine;
	SBufer_mem(void* P, size_t S, size_t numLine) {
		LP = P; Size = S; nLine = numLine; isFree = false;
	}
};
std::list<SBufer_mem> Bufer_mems;
auto find_in_Bufer_mems(void *lp) {
	auto p = std::find_if(Bufer_mems.begin(), Bufer_mems.end(), [lp](const SBufer_mem& x) -> bool {
		return x.LP == lp; });
	return (p==Bufer_mems.end())? false: (p->isFree? false: p->isFree = true);
}

#define Save_toMemList(p,s) Bufer_mems.push_back(SBufer_mem(p,s,__LINE__))
#define Free_inMemList(p)	find_in_Bufer_mems(p)
#else
#define Save_toMemList(p,s)
#define Free_inMemList(p) true
#endif

#define __max(a,b)  (((a) > (b)) ? (a) : (b))

template <class Type>
class VRTSmartStr;

struct InfoData {
	size_t Len, Cou;
	void Set(size_t Len_, size_t Cou_) {
		Len = Len_; Cou = Cou_;
	}

	// атомарныеные операции счетчика
	size_t IncCou() { return InterlockedIncrementAcquire(&Cou); };
	size_t DecCou() { return InterlockedDecrementAcquire(&Cou); };
};

struct InfoDataB : InfoData {
	size_t LBuf;
	void Set(size_t Len_, size_t LBuf_, size_t Cou_){
		InfoData::Set(Len_, Cou_);
		LBuf = LBuf_;
	}
};

template <class Type>
struct Bufer : InfoDataB {
	Type Data[1];

	//Создает новый буфер и если нужно копирует в него данные
	static Bufer* Create(size_t Size, const Type* Val = nullptr, size_t Len = 0) {
		_ASSERTE(Size != 0);
		Bufer<Type>* P = (Bufer<Type>*) malloc(Size*sizeof(Type) + sizeof(InfoDataB));
		if (!P) {			
			int e;
			_get_errno(&e);
			TMyEx E(1, nullptr, "Bufer", &Size, sizeof(Size));
			E.Str = new char[75];
			strcpy_s(E.Str, 9, "Create: ");
			strerror_s(E.Str + 8, 75 - 8, e);
			throw E;
			return nullptr;
		}
		P->Set(Size, Size, 1);
		Save_toMemList(P, _msize(P));
		if (Val) {
			if (!Len) Len = Size;
			P->FillBuf(Val, Len);			
		}
		return P;
	}

	//Переcоздает новый буфер и копирует из старого данные
	static Bufer* ReCreate(Bufer* Old, size_t Size) {
		_ASSERTE(Size > 0);
		Bufer<Type>* P;
		if (!Old || Old->Cou == 1) 
			P = (Bufer<Type>*)realloc(Old, sizeof(Type)* Size + sizeof(InfoDataB));
		else {
			P = Create(Size, Old->Data, Old->Len+1);
			if (!P) return Old;
			if(!Old->DecCou ()) Old->Destroy();
			return P;
		}
		if (!P) {
			int e;
			if (_get_errno(&e)) e = 0;
			if (Old) free(Old);
			VRTSmartStr<char> E;
			E.Length(75);
			E << "Bufer->Create(" << Size << "): ";
			strerror_s(E.GetBuffer() + E.Length(), 75 - E.Length(), e);
			E.ReleaseBuffer(75);
			throw E;
		}
		P->Set(Size, Size, 1);
		Save_toMemList(P, _msize(P));
		return P;
	}

	void Destroy() {
		_ASSERT(Free_inMemList(this));
		free(this);
	};

	//Копирует в готовый буфер данные
	// BUF - Данные; len - Размер данных; Beg - Начало вставки
	// никакой проверки данных нет
	void FillBuf(const Type* BUF, size_t len = 0, size_t Beg = 0) {
		_ASSERT(Beg + len > Beg);
		if (!len || Beg + len > LBuf || Data+Beg == BUF)
			return;
		memcpy(Data + Beg, BUF, len * sizeof(Type));
	}
private:
	Bufer() {};
};

//struct Cell;
template <class Type>
struct Cell {
	Type Data;
	Cell* next = nullptr;	

	Cell() {};
	Cell(Type val) :Data(val) {};
};

template <class Type>
struct BuferList : InfoData {
	Cell<Type> *Beg;
public:
	BuferList() {
		Cou = 1;
		Beg = nullptr;
		Len = 0;
	}

	void Clear() {
		Cell<Type>* C = Beg;
		while (C->next) {
			Beg = C->next;
			delete C;
			C = Beg;
		}
		delete C;
	};
};

template <class Type>
class List  {
	BuferList<Type>* Info = nullptr;
	Cell<Type> *CurCell;
	size_t Cur;	

	Cell<Type>* GetCell(size_t Pos) {
		Cell<Type>* C;
		if (Cur <= Pos) {
			C = CurCell;
			Pos -= Cur;
		} else C = Info->Beg;

		while (Pos && C->next) {
			C = C->next;
			Pos--;
		}
		return C;
	};

	BuferList<Type>* Copy() {
		if (!Info || !Info->Beg) return Info;
		BuferList<Type>* new BuferList<Type>()
	}
public:
	List() {
		CurCell = nullptr;
		Cur = 0;
	}
	List(const List<Type>& Val) { 
		operator =(Val);
	}
	
	List<Type>& operator =(const List<Type>& Val) {
		Clear();
		if (!Val.Info) {
			Cur = 0;
			CurCell = nullptr;
			return *this;
		}
		CopyMemory(this, &Val, sizeof(Val));
		Info->IncCou();
		return *this;
		}

	~List() { Clear(); }

	void Clear() {
		if (!Info || Info->DecCou()) return;
		Info->Clear();
		delete Info;
		Info = nullptr;
		CurCell = nullptr;
		Cur = 0;
	};

	bool Next() {
		if (!Info || !CurCell->next) return false;
		CurCell = CurCell->next;
		return true;
	};

	size_t Add(Type Val) {
		Cell<Type>** C = Info ? &GetCell(Info->Len - 1)->next : &(Info = new BuferList<Type>())->Beg;
		CurCell = new Cell<Type>(Val);
		CurCell->next = *C;
		*C = CurCell;
		Cur = Info->Len++;
		return Cur;
	}

	size_t AddList(List<Type>& Val, size_t Beg = 0, size_t Count = 0) {
		if (Val.Length() <= Beg) return 0;
		if (!Count) Count = Val.Length() - Beg;
		Count += Beg;
		while (Count>Beg){
			Val.CurentPos(Beg);
			Add(Val.Curent());
			Beg++;
			}
		return Length();
	}

	// Вставляет элемент в позицию Pos
	size_t Insert(size_t Pos, Type Val) {
		if (!Info) return Add(Val);
		if (Pos > Info->Len) Pos = Info->Len;
		Cell<Type>** C = Pos ? &GetCell(Pos - 1)->next : &(Info->Beg);
		CurCell = new Cell<Type>(Val);
		CurCell->next = *C;
		if (*C == Info->Beg)Info->Beg = *C;
		*C = CurCell;
		Info->Len++;
		Cur = Pos;
		return Cur;
	}

	void Set(size_t Pos, Type Val) {
		if (Len < Pos + 1) return;
		Cell<Type>* C = GetCell();
		C->Data = Val;
	}

	Type Get(size_t Pos)const {
		Cell<Type>* C = GetCell(Pos);
		return C->Data;
	}

	Type& operator [](size_t Pos) {
		if (!Info) {
			Type S;
			return S;
		}
		Cell<Type>* C = GetCell(Pos);
		return C->Data;
	}

	size_t Length()const{ return Info? Info->Len: 0; }

	void ToStart() { Cur = 0; CurCell = Info ? Info->Beg: nullptr; }
	Type Curent()const { return CurCell->Data; };	
	size_t CurentPos()const { return Cur; }
	size_t CurentPos(size_t Pos){
		if (!Info) return 0;
		if (Pos + 1 > Info->Len) Pos = Info->Len - 1;
		CurCell = GetCell(Pos);
		Cur = Pos;
		return Cur;}
};

///////////////////////////////////////////////////////

static size_t StringLength(_In_opt_z_ const LPCSTR psz, size_t Size = 0) throw() {
	if (psz == nullptr) return 0;
	return Size ? strnlen(psz, Size) : strlen(psz);
}

static size_t StringLength(_In_opt_z_ const LPCWSTR psz, size_t Size = 0) throw() {
	if (psz == nullptr) return 0;
	return Size ? wcsnlen(psz, Size) : wcslen(psz);
}

template< typename BaseType>
class strangeType;

template <class Type>
class BufPointer {
	typedef typename strangeType<Type>::L_Char L_Char;
	typedef typename strangeType<Type>::T_Str T_Str;
	typedef typename strangeType<Type>::TC_Str TC_Str;
	typedef typename strangeType<Type>::LC_Str LC_Str;
public:
	TC_Str Data;
	size_t Len, *Cou = nullptr;

	~BufPointer() {
		if (!Cou) return;
		if (!InterlockedDecrementAcquire(Cou))
			((Bufer<Type>*)SHIFT(Data, -1, InfoDataB))->Destroy();
	}

	BufPointer(const BufPointer &Val) : Data(nullptr) {
		this->operator=(Val);
	}

	//BufPointer(const VRTSmartStr<Type>& Val) : Data(Val) {
	//	Len = Val.Length();
	//	Data = Val.Data();
	//}

	BufPointer(const VRTSmartStr<Type>& Val) : Data(Val) {
		Len = Val.Length();
		if (Len) {
			Cou = SHIFT(Data, -2, size_t);
			InterlockedIncrementAcquire(Cou);
		}
	}

	BufPointer(TC_Str Val, const size_t Len = 0) : Data(Val) {
		this->Len = Len ? Len : StringLength(Val);
	}

	BufPointer(LC_Str Val, size_t Size = 0) : Data(nullptr) {
		size_t Sz = strangeType<Type>::BaseTypeLen(Val, Size);
		Bufer<Type> *I = Bufer<Type>::Create(Sz + 1);
		Cou = &I->Cou;
		Data = re_cast(Type*, I->Data);
		Len = I->Len - 1;
		strangeType<Type>::ToBaseType(*this, Val, Size);
	}

	BufPointer(const INT64 Val, byte sys = 10) : Data(nullptr) {
		Bufer<Type> *I = Bufer<Type>::Create(sys == 10 ? 21 : _CVTBUFSIZE);
		Cou = &I->Cou;
		Data = I->Data;
		Len = I->Len - 1;
		strangeType<Type>::NumberToStr(Val, sys, *this);
		Len = StringLength(Data);
		I->Len = Len;
	}

	BufPointer(const int Val, byte sys = 10) : Data(nullptr) {
		*this = BufPointer((INT64)Val);
	}

	BufPointer(const DWORD Val, byte sys = 10) : Data(nullptr) {
		*this = BufPointer((INT64)Val);
	}

	BufPointer(const size_t Val, byte sys = 10) : Data(nullptr) {
		*this = BufPointer((INT64)Val);
	}

	BufPointer(const double Val, byte dig = 10) : Data(nullptr) {
		Bufer<Type> *I = Bufer<Type>::Create(dig + 31);
		Cou = &I->Cou;
		Data = I->Data;
		Len = I->Len / 2;
		strangeType<Type>::NumberToStr(Val, dig, *(BufPointer<char>*)this);
		I->Len = Len = StringLength((char*)Data);
		I->Data[Len] = 0;
		size_t Tm = Len;
		while (Tm) 	I->Data[Tm] = ((char*)Data)[--Tm];
	}

	BufPointer& operator =(const BufPointer &Val) {
		if (Val.Cou) InterlockedIncrementAcquire(Val.Cou);
		memcpy(this, &Val, sizeof(*this));
		return *this;
	}
};

template <class Type>
class  Info_Buf {
	typedef typename strangeType<Type>::L_Char L_Char;
	typedef typename strangeType<Type>::T_Str T_Str;
	typedef typename strangeType<Type>::TC_Str TC_Str;
	typedef typename strangeType<Type>::LC_Str LC_Str;
public:

	~Info_Buf() {
		if (!Data || type < 3) return;
		if (type == 5) {
			((VRTSmartStr<Type>*)Data)->Clear();
			free(Data);
		}
		else if (!InterlockedDecrementAcquire((LONG*)Data)) free(Data);
	}

	Info_Buf(const Info_Buf &Val) {
		this->operator=(Val);
	}

	Info_Buf(TC_Str Val, const size_t Size = 0) {
		Len = Size ? Size : StringLength(Val);
		Data = const_cast<T_Str>(Val);
		type = 1;
	}

	Info_Buf(LC_Str Val, size_t Size = 0) {
		Len = strangeType<Type>::BaseTypeLen(Val);
		Data = const_cast<L_Char*>(Val);
		type = 2;
	}

	Info_Buf(INT64 Val, byte sys = 10) {
		CreateData(sizeof(INT64) + sizeof(byte));
		*(INT64*)SHIFT(Data, 1, LONG) = Val;
		*(byte*)SHIFT(SHIFT(Data, 1, LONG), 1, INT64) = sys;
		Len = 1;
		while (Val /= 10) Len++;
		type = 3;
	}

	Info_Buf(const int Val, byte sys = 10) {
		*this = Info_Buf((INT64)Val, sys);
	}

	Info_Buf(const DWORD Val, byte sys = 10) {
		*this = Info_Buf((INT64)Val, sys);
	}

	Info_Buf(const size_t Val, byte sys = 10) {
		*this = Info_Buf((INT64)Val, sys);
	}

	Info_Buf(const double Val, byte dig = 10) {
		Len = dig + 31;
		CreateData(sizeof(Val)+ sizeof(dig));
		*(double*)SHIFT(Data, 1, LONG) = Val;
		*(byte*)SHIFT(SHIFT(Data, 1, LONG), 1, double) = dig;
		type = 4;
	}

	Info_Buf(const VRTSmartStr<Type>& Val) {
		Len = Val.Length();
		if (Len) {
			Data = malloc(sizeof(Val));
			ZeroMemory(Data, sizeof(Val));
			*(VRTSmartStr<Type>*)Data = Val;
		}
		type = 5;
	}

	Info_Buf& operator = (const Info_Buf &Val) {
		if (Data) this->~Info_Buf();
		if (Val.Data) InterlockedIncrementAcquire((LONG*)Val.Data);
		memcpy(this, &Val, sizeof(*this));
		return *this;
	}
private:
	void* Data = nullptr;
	size_t Len, type;

	friend VRTSmartStr<Type>;
	void CreateData(byte size) {
		Data = malloc(sizeof(LONG) + size);
		*(LONG*)(Data) = 1;
	};
	size_t SaveToBuf(T_Str Buf) const {
		if (!Data) return 0;
		switch (type) {
		case 1: {
			memcpy(Buf, Data, (Len)*sizeof(Type));
			return Len;
		}break;
		case 2: {
			strangeType<Type>::ToBaseType(BufPointer<Type>(Buf, Len), (L_Char*)Data, Len);
			return Len;
		}break;
		case 3: {
			strangeType<Type>::NumberToStr(*(INT64*)SHIFT(Data, 1, LONG), *(byte*)SHIFT(SHIFT(Data, 1, LONG), 1, INT64), BufPointer<Type>(Buf, Len));
			return Len;
		}break;
		case 4: 
			return strangeType<Type>::NumberToStr(*(double*)SHIFT(Data, 1, LONG), *(byte*)SHIFT(SHIFT(Data, 1, LONG), 1, double), BufPointer<Type>(Buf, Len));
		break;
		case 5: {
			memcpy(Buf, *(Type**)Data, (Len)*sizeof(Type));
			return Len;
		}break;
		};
		return 0;
	};
};

template <class Type>
class VRTSmartStr {

private:
	//friend strangeType<Type>;
#define Info Bufer<Type>
#define TSmartString VRTSmartStr<Type>
	typedef typename strangeType<Type>::L_Char L_Char;
	typedef typename strangeType<Type>::T_Str T_Str;
	typedef typename strangeType<Type>::TC_Str TC_Str;
	typedef typename strangeType<Type>::LC_Str LC_Str;
	T_Str Buf = nullptr;

	Info* GetInfo()const { return Buf ? (Info*)SHIFT(Buf, -1, InfoDataB) : nullptr; }

	void SetBuf(Info* P) {
		if (Buf == P->Data) return;
		Clear();
		Buf = P->Data;
	}

	static size_t NB_Size(size_t Size, Info* Old){
		size_t NewSize;
		if (!Old || Old->LBuf > Size) 
			return Size;
		NewSize = Old->LBuf + (Old->LBuf * sizeof(Type) > 1073741824 ? 1048576 : Old->LBuf / 2); //1073741824 - 1Gb; 1048576 - 1Mb
		if (NewSize < Size) NewSize = Size;
		return NewSize;
	}

	// Создает новый бефер размера Size, 
	// Pointer - указатель на старый блок
	static Info* NewBuf(size_t Size, Info *Pointer = nullptr) {
		_ASSERT(Size+1 > 1);
		Info *I;
		if(Pointer)	I = Info::ReCreate(Pointer, Size + 1);
		else I = Info::Create(Size + 1);
		I->Len = Size;
		return I;
	}

	// Пересоздает буфер нового размера без копирования
	Info* DeBuf(const size_t requisiteSize, bool FirstClear = false) {
		_ASSERT(requisiteSize + 1 > 1);
		Info *I = GetInfo();
		if (!I || I->Cou > 1 || I->LBuf < requisiteSize + 1) {
			size_t NewSize = NB_Size(requisiteSize, I);
			if (I && FirstClear) Clear();
			I = NewBuf(NewSize);
		}
		I->Len = requisiteSize;
		return I;
	}

	// Пересоздает буфер нового размера с копированием
	Info* DeBufCopy(const size_t requisiteSize) {
		_ASSERT(requisiteSize + 1 > 1);
		Info *I = GetInfo();
		if (!I || I->Cou > 1 || I->LBuf < requisiteSize + 1) {
			size_t NewSize = NB_Size(requisiteSize, I);
			I = NewBuf(NewSize, I);
		} 
		I->Len = requisiteSize;
		return I;
	}

	// Возвращает позицию первого вхождения Val начиная с позиции Beg
	bool FindStr(TC_Str Val, size_t& Pos)const {
		if (!Buf || !Val || Pos > Length()) return false;
		TC_Str P = strangeType<Type>::Find(Buf + Pos, Val);
		Pos = P == nullptr ? 0 : P - Buf;
		return P != nullptr;
	};

	// Возвращает позицию последнено вхождения Val не далее позиции Beg (поиск ведется с конца строки)
	bool FindStrOf(TC_Str Val, size_t& Pos)const {
		if (!Buf || !Val || Pos > Length() || !Length()) return false;
		if (!Pos) Pos = Length();
		TC_Str T, V, P = Buf + Pos - 1;

		while (P != Buf) {
			if (*P == *Val) {
				T = P, V = Val;
				while (*(T++) == *(V++) && *V && *T) {}
				if (!*V) {
					Pos = P - Buf;
					return true;
				}
			}
			P--;
		}
		return false;
	};

	//	
	size_t InsertPrv(size_t Pos, TC_Str Val, size_t Size, size_t Count = 0) {
		auto LEN = Length();
		if (!Size && !Count || Pos > LEN) return LEN;
		_ASSERTE(Count + Pos >= Pos); // переполнение типа size_t
		if (Count + Pos > LEN) Count = LEN - Pos;
		_ASSERTE(LEN + Size > LEN && LEN + Size > Size); // переполнение типа size_t
		Info *P = DeBuf(Size + LEN - Count);
		if (Pos && P->Data != Buf) P->FillBuf(Buf, Pos);
		if (Count != Size && Pos + Count < LEN)
			P->FillBuf(Buf + Pos + Count, LEN - Pos - Count, Size + Pos);
		P->FillBuf(Val, Size, Pos);
		P->Data[P->Len] = 0;
		SetBuf(P);
		return P->Len;
	}

	//inline size_t InsertPrv(size_t Pos, BufPointer<Type>& Val, size_t Count = 0) { return InsertPrv(Pos, Val.Data, Val.Len, Count); }

	size_t RemChars(TC_Str Home, const BufPointer<Type>& Val) {
		TC_Str Finds = strangeType<Type>::FindOneP(Home, Val.Data);

		if (!Finds) return Length() - (Home - Buf);

		size_t P = RemChars(Finds + 1, Val);		
		Info *I = GetInfo();
		I->FillBuf(Finds +1, P, Finds-Buf);
		I->Len--;
		return Finds - Home + P ;
	};

	size_t IncludeBuf(TC_Str Val)const {
		return (!Buf || !Val || Val<Buf || Val > Buf + GetInfo()->LBuf) ? 0 : Val - Buf +1;
	}

	// Устанавливает размер буфера не менее Size, 
	//копирует при указании данные с Val размером Len или до конца строки начиная с Beg
	void SetValue(size_t Size, TC_Str Val, size_t Len, const size_t Beg = 0) {
		Info *I = DeBuf(Size, !IncludeBuf(Val));
		if (Val && Len) {
			if (Len + Beg > Size) Len = Size - Beg;
			I->FillBuf(Val, Len, Beg);
			I->Len = Len + Beg;
		} else I->Len = 0;
		I->Data[I->Len] = 0;
		SetBuf(I);
	}

public:
	//operator LPARAM() { return (LPARAM)Data(); };
	~VRTSmartStr() { Clear(); };

	VRTSmartStr() {};

	VRTSmartStr(const TSmartString &Val) {
		this->operator=(Val);
	};

	VRTSmartStr(const BufPointer<Type>& Val) {
		if (!Val.Cou) *this = VRTSmartStr(Val.Data, Val.Len);
		else {
			Buf = const_cast<T_Str>(Val.Data);
			GetInfo()->IncCou();
		}	
	};

	VRTSmartStr(TC_Str Val, size_t Size = 0) {
		if (!Size) Size = StringLength(Val);
		SetValue(Size, Val, Size);
	};

	VRTSmartStr(LC_Str Val, size_t Size = 0) {
		size_t Len = strangeType<Type>::BaseTypeLen(Val, Size);
		Info *I = DeBuf(Len);
		strangeType<Type>::ToBaseType(BufPointer<Type>(I->Data, I->Len), Val, Size);
		I->Data[I->Len] = 0;
		SetBuf(I);
	};

	VRTSmartStr(const INT64 Val, byte sys = 10) {
		BufPointer<Type> r(Val, sys);
		*this = r;
	};

	explicit VRTSmartStr(const double Val, byte sys = 10) {
		BufPointer<Type> r(Val, sys);
		*this = r;
	};

	T_Str GetBuffer(size_t NewSize = 0) {
		if (NewSize) {
			auto I = DeBuf(NewSize);
			SetBuf(I);
			return I->Data;
		}
		_ASSERT(Length());
		return Buf;
	};

	T_Str ReleaseBuffer(size_t NewSize = 0) {
		_ASSERTE(NewSize < GetInfo()->LBuf);
		lengthDetermine(NewSize);
		return Buf;
	};

	TSmartString& operator = (const TSmartString& Val) {
		if (this->Buf != Val.Buf) 
			if (Val.Length()) {
				Val.GetInfo()->IncCou();
				Clear();
				Buf = Val.Buf;
			} else Clear();
		return *this;
	};
		
	TSmartString& operator = (TC_Str Val) {
		size_t Size = StringLength(Val);
		SetValue(Size, Val, Size);
		return *this;
	};

	TSmartString& operator = (LC_Str Val) {
		size_t Len = strangeType<Type>::BaseTypeLen(Val, 0);
		Info* I = DeBuf(Len, true);
		strangeType<Type>::ToBaseType(BufPointer<Type>(I->Data, I->Len), Val, 0);
		I->Data[I->Len] = 0;
		SetBuf(I);
		return *this;
	};

	TSmartString& operator = (Info_Buf<Type> Val) {
		if (!Val.Len) return *this;
		auto I = DeBuf(Val.Len, true);
		I->Len = Val.SaveToBuf(I->Data);
		I->Data[I->Len] = 0;
		SetBuf(I);
		return *this;
	};

	TSmartString& operator += (const Info_Buf<Type>& Val) {
		if (Val.Len) {
			auto ol = Length();
			auto I = DeBufCopy(Val.Len + ol);
			I->Len = ol + Val.SaveToBuf(I->Data + ol);
			I->Data[I->Len] = 0;
			SetBuf(I);
		}
		return *this;
	};
	TSmartString& operator += (LC_Str Val) {
		size_t Len = strangeType<Type>::BaseTypeLen(Val, 0);
		Length(Len + Length(), true);
		Info* I = GetInfo();
		strangeType<Type>::ToBaseType(BufPointer<Type> (Buf + I->Len, Len), Val, 0);
		I->Len += Len;
		return *this;
	};

	//TSmartString& operator << (LC_Str Val) {
	//	return this->operator+=(Val);
	//};	

	TSmartString& operator << (const Info_Buf<Type>& Val) {
		return this->operator+=(Val);
	};
	
	//TSmartString& operator >> (LC_Str Val) {
	//	size_t Len = strangeType<Type>::BaseTypeLen(Val, 0);
	//	SetValue(Len + Length(), Buf, Length(), Len);
	//	strangeType<Type>::ToBaseType(BufPointer<Type>(Buf, Len), Val, Len, 0);
	//	return *this;
	//};

	TSmartString& operator >> (const Info_Buf<Type>& Val) {
		if (Val.Len) {
			auto ol = Length();
			SetValue(Val.Len + ol, Buf, ol, Val.Len);
			auto C = Buf[Val.Len];
			auto L = Val.SaveToBuf(Buf);
			if (Val.Len != L) GetInfo()->FillBuf(Buf + Val.Len, Length() - Val.Len+1, L);
			Buf[L] = C;
		}
		return *this;
	};

	TSmartString& operator + (const BufPointer<Type>& Val2) {
		if (!Val1.Length()) return TSmartString(Val2);
		if (!Val2.Data) return Val1;
		Info* buf = Info::Create(Val2.Len + Val1.Length() + 1, Val1.Buf, Val1.Length());
		buf->FillBuf(Val2.Data, Val2.Len, Val1.Length());
		buf->Data[--buf->Len] = 0;
		TSmartString _tmp;
		_tmp.Buf = buf->Data;
		return _tmp;
	};

	/*friend TSmartString operator + (const TSmartString& Val1, const BufPointer<Type>& Val2) {
		if (!Val1.Length()) return TSmartString(Val2);
		if (!Val2.Data) return Val1;
		Info* buf = Info::Create(Val2.Len + Val1.Length() + 1, Val1.Buf, Val1.Length());
		buf->FillBuf(Val2.Data, Val2.Len, Val1.Length());
		buf->Data[--buf->Len] = 0;
		TSmartString _tmp;
		_tmp.Buf = buf->Data;
		return _tmp;
	};
	
	/*friend TSmartString operator + (const BufPointer<Type>& Val1, const TSmartString& Val2) {
		if (!Val1.Len) return Val2;
		if (!Val2.Data) return Val1;
		Info* buf = Info::Create(Val2.Length() + Val1.Len + 1, Val1.Data, Val1.Len);
		buf->FillBuf(Val2.Buf, Val2.Length(), Val1.Len);
		buf->Data[--buf->Len] = 0;
		TSmartString _tmp;
		_tmp.Buf = buf->Data;
		return _tmp;
	};

	/* friend TSmartString operator + (const TSmartString& Val1, const TSmartString& Val2) {
		if (!Val1.Length()) return Val2;
		if (!Val2.Length()) return Val1;
		Info* buf = Info::Create(Val1.Length() + Val2.Length() + 1, Val1.Buf, Val1.Length());
		buf->FillBuf(Val2.Buf, Val2.Length(), Val1.Length());
		buf->Data[--buf->Len] = 0;
		TSmartString _tmp;
		_tmp.Buf = buf->Data;
		return _tmp;
	};

	inline friend TSmartString operator + (const TSmartString& Val1, const T_Str Val2) {
		if (!Val1.Length()) return TSmartString(Val2);
		if (!Val2) return Val1;
		size_t Sz = StringLength(Val2);
		Info* buf = Info::Create(Sz + Val1.Length() + 1, Val1.Buf, Val1.Length());
		buf->FillBuf(Val2, Sz, Val1.Length());
		buf->Data[--buf->Len] = 0;
		TSmartString _tmp;
		_tmp.Buf = buf->Data;
		return _tmp;
	};

	inline friend TSmartString operator + (const T_Str Val1, const TSmartString& Val2) {
		if (!Val1) return Val2;
		if (!Val2.Length()) return TSmartString(Val1);
		size_t Sz = StringLength(Val1);
		Info* buf = Info::Create(Sz + Val2.Length() + 1, Val1, Sz);
		buf->FillBuf(Val2.Buf, Val2.Length(), Sz);
		buf->Data[--buf->Len] = 0;
		TSmartString _tmp;
		_tmp.Buf = buf->Data;
		return _tmp;
	};*/


	bool operator < (const BufPointer<Type>& Val) {
		return this->Compare(Val) <0;
	};

	bool operator > (const BufPointer<Type>& Val) {
		return this->Compare(Val) > 0;
	};

	inline friend bool operator == (const TSmartString& Val1, const TSmartString& Val2) throw() {
		return Val1.Compare(Val2) == 0;
	};	

	inline friend bool operator == (const TSmartString& Val1, const BufPointer<Type>& Val2) throw() {
		return Val1.Compare(val2) == 0;
	};
	
	friend bool operator==(
		const TSmartString& str1,
		TC_Str psz2
		) throw() {
		return !str1.Compare(psz2);
	};

	inline friend bool operator != (const TSmartString& Val1, const TSmartString& Val2) throw() {
		return Val1.Compare (Val2) !=0;
	};

	friend bool operator!=(
		const TSmartString& str1,
		TC_Str psz2
		) throw() {
		return str1.Compare(psz2);
	};

	//Сохраняет строковое представление числа
	TSmartString& SetInt(INT64 Val, byte sys = 10) {
		Bufer<Type> *I = DeBuf(sys == 10 ? 21 : _CVTBUFSIZE);
		strangeType<Type>::NumberToStr(Val, sys, BufPointer<Type>(I->Data, I->LBuf-1));
		I->Len = StringLength(I->Data, I->Len);
		SetBuf(I);		
		return *this;
	}

	TSmartString& SetFloat(const double Val, byte dig = 10) {
		Bufer<Type> *I = DeBuf(dig + 31);
		I->Len = strangeType<Type>::NumberToStr(Val, dig, BufPointer<Type>(I->Data, I->LBuf-1));
		I->Data[I->Len] = 0;
		SetBuf(I);
		return *this;
	}

	//Сложение в строку нескольких источников, FullSize = общий размер, параметры: размер и указатель на начало строки извлекается пока не дойдет до FullSize
	//	возвращает общую длинну строки
	size_t Concatenation(size_t FullSize, ...) { return Concatenation((va_list)&FullSize); }

	size_t Concatenation(va_list Par) {
		size_t tmp, Zp(0), FullSize = va_arg(Par, size_t);	

		if (!FullSize) return Length(0);
		Info* I = DeBuf(FullSize, true);

		while (FullSize > Zp) {
			tmp = va_arg(Par, size_t);
			I->FillBuf(va_arg(Par, T_Str), tmp, Zp);
			Zp += tmp;
		}
		va_end(Par);
		I->Len = Zp;
		I->Data[I->Len] = 0;
		SetBuf(I);
		return Zp;
	}

	inline bool isEmpty() { return !Length();}

	//Возвращает размер буфера памяти учитывая символ конца строки
	inline size_t BufLength()const { return Buf ? GetInfo()->LBuf : 0; };

	//Возвращает длину строки, без символа конца строки
	inline size_t Length()const { return Buf ? GetInfo()->Len : 0; };
	
	//Рассчитывает актуальную длину строки не более maxCou символов вмещаемую в буфер, при необходимости добавляет символ конца строки
	size_t lengthDetermine(size_t maxCou = 0)const {
		if (!Buf) return 0;
		maxCou = maxCou ? min(maxCou, GetInfo()->LBuf-1) : GetInfo()->LBuf;
		size_t SZ = StringLength(Buf, maxCou);
		if (GetInfo()->LBuf < SZ + 1) {
			SZ = GetInfo()->LBuf - 1;
			Buf[SZ] = 0;
		}
		return GetInfo()->Len = SZ;
	}

	//Устанавливает длинну строки 
	//(Size - Размер (не учитывая символа конца строки), Copy - Копировать ли содержимое, SetResize - Уменьшать ли размер буфера)
	size_t Length(size_t Size, bool Copy = false, bool SetResize = false) {
		_ASSERTE(Size != -1);
		Info *I = GetInfo();
		
		if (!Size) {
			if (SetResize) Clear();
			elseif(I) I->Data[0] = 0;
			return 0;
		}

		if (!I || I->Cou > 1 || I->LBuf < Size + 1 || SetResize) {
			I = Info::ReCreate(I, Size + 1);
			if (Copy) {
				I->Len = min(Length(), Size);
				if (Buf != I->Data && I->Len) I->FillBuf(Buf, I->Len, 0);
			} else I->Len = 0;
		} else if (Size < I->Len) I->Len = Size;
		I->Data[I->Len] = 0;
		SetBuf(I);
		return I->Len;
	}

	//Очищает строку, уничтожает буфер
	void Clear() {
		if (!Buf) return;
		Info *T = GetInfo();
		if (!T->DecCou()) T->Destroy();
		Buf = nullptr;
	};

	//Сравнивается со строкой val размера Sz. Возвращает >0 если Val меньше 
	//	NoCase - Различать ли заглавные
	int Compare(const TSmartString Val, bool NoCase = false)const {
		return strangeType<Type>::CompareStr(Data(), Val.Data(), Length(), Val.Length(), NoCase);
		//из "const wchar_t *" в "const strangeType<WCHAR>::T_Str"
	}
	int Compare(const TC_Str Val, size_t Sz = 0, bool NoCase = false)const {
		if (!Sz) Sz = StringLength(Val);
		return strangeType<Type>::CompareStr(Data(), Val, Length(), Sz, NoCase);
	}

	//Удаляет часть строки начиная c Beg Начало, длиной Count
	int Delete(size_t Beg, size_t Count = 1) {
		size_t SZ = Length();
		if (!SZ || Beg >= SZ || !Count) return SZ;
		_ASSERTE(Beg + Count > Count);
		if (Beg + Count > SZ) Count = SZ - Beg;
		auto I = DeBufCopy(SZ - Count);
		if (I->Len)
			memmove(I->Data + Beg, Buf + Beg + Count, SZ - Count - Beg);
		I->Data[I->Len] = 0;
		SetBuf(I);
		return I->Len;
		//return Concatenation(SZ - Count, Beg, Buf, SZ - Count - Beg, Buf + Beg + Count);
	};

	//Возвращает левую часть строки (size_t Количество)
	TSmartString Left(size_t Count)const {
		if (Count >= Length()) return *this;
		return TSmartString(Buf, Count);
	}

	//Возвращает правую часть строки (size_t Количество)
	TSmartString Rigth(size_t Count)const {
		if (Count >= Length()) return *this;
		return TSmartString(Buf + Length() - Count, Count);
	}

	//Возвращает средину строки (size_t Начало, size_t Количество = 1)
	TSmartString Mid(size_t Beg, size_t Count = 1)const {
		if (!Buf || !Count || Beg >= Length() || (!Beg && Count == Length())) return TSmartString();
		if (Count + Beg > Length() || Count > Length())
			Count = Length() - Beg;
		return TSmartString(Buf + Beg, Count);
	}

	// переводит строку в нижний регистр
	TSmartString& MakeLower() {
		if (!Buf) return *this;
		Info *P = DeBufCopy(Length());
		strangeType<Type>::LowerStr(P->Data, P->Len);
		SetBuf(P);
		return *this;
	}

	// переводит строку в нижний регистр
	TSmartString& MakeUpper() {
		if (!Buf) return *this;
		Info *P = DeBufCopy(Length());
		strangeType<Type>::UpperStr(P->Data, P->Len);
		SetBuf(P);
		return *this;
	}

	void Reverse() {
		if (!Buf) return;
		Info *P = DeBufCopy(Length());
		strangeType<Type>::Reverse(P->Data);
		SetBuf(P);
		return *this;
	}

	// ищет значение Val в строке начиная с символа Pos. Там же найденный символ
	bool Find(const BufPointer<Type>& Val, size_t& Pos)const {
		return FindStr(Val.Data, Pos);
	};	
	
	/*bool Find(const TSmartString& Val, size_t& Pos)const {
		return FindStr(Val.Buf, Pos);
	};

	INT64 Find(TC_Str Val, size_t Beg = 0)const {
		return FindStr(Val, Beg);
	};

	INT64 Find(LC_Str Val, size_t Beg = 0)const {
		TSmartString _tmp(Val);
		return FindStr(_tmp, Beg);
	};

	INT64 Find(const BufPointer<Type>& Val, size_t Beg = 0)const {
		return FindStr(Val.Data, Beg);
	};
*/
	// ищет подстроку в строке начиная с конца(что ищем, символ начала поиска = 0)
/*	bool FindOf(const TSmartString& Val, size_t& Pos)const {
		return FindStrOf(Val.Buf, Pos);
	};*/
	bool FindOf(const BufPointer<Type>& Val, size_t& Pos)const {
		return FindStrOf(Val.Data, Pos);
	};

	/*INT64 FindOf(TC_Str Val, size_t Beg = 0)const {
		return FindStrOf(Val, Beg);
	};

	INT64 FindOf(LC_Str Val, size_t Beg = 0)const {
		TSmartString _tmp(Val);
		return FindStrOf(_tmp, Beg);
	};*/

	//Ищет в строке позицию первого символа из указанной строки
	INT64 FindOne(const T_Str Val)const { return strangeType<Type>::FindOne(Buf, Val); };
	//Ищет в строке позицию первого символа из указанной строки
	INT64 FindOne(const BufPointer<Type>& Val, size_t Beg = -1, size_t Len = 0)const {
		return strangeType<Type>::FindOne(Buf, Val.Data());};

	void RemoveChars(const BufPointer<Type>& Val) {
		if (!Buf) return;
		RemChars(Buf, Val);
		Buf[Length()] = 0;
	};

	// Заменяет в строке все вхождения ValOld на ValNew, возвращает количество замен
	size_t Replace(const BufPointer<Type>& ValOld, const BufPointer<Type>& ValNew) {
		if (!Buf || !ValOld.Len) return 0;
		size_t Cou(0), T = Length();
		T_Str BegP = Buf, EndP = Buf + T;

		{while (BegP < EndP && (BegP = strangeType<Type>::Find(BegP, ValOld.Data)))
			Cou++, BegP += ValOld.Len; }
		if (!Cou) return 0;

		_ASSERTE(!IncludeBuf(ValOld.Data) && !IncludeBuf(ValNew.Data));

		Info *I;
		size_t NewLen = T + int(Cou * (ValNew.Len - ValOld.Len));
		I = DeBufCopy(NewLen);
		
		BegP = Buf, EndP = Buf + T;
		while (BegP < EndP && (BegP = strangeType<Type>::Find(BegP, ValOld.Data))) {
			if (ValNew.Len != ValOld.Len) {
				T = (EndP - BegP - ValOld.Len) * sizeof(Type);
				memmove_s(BegP + ValNew.Len, T, BegP + ValOld.Len, T);
				EndP += ValNew.Len - ValOld.Len;
			}
			if (ValNew.Len) I->FillBuf(ValNew.Data, ValNew.Len, BegP - Buf);
			BegP += ValNew.Len;	}
		
		I->Data[I->Len] = 0;
		SetBuf(I);
		return Cou;
	}

	// Возвращает строку до первого символа из указанной строки
	TSmartString SpanExcluding(TC_Str Val) {
		if (!Buf || !Val) return *this;
		int r = strangeType<Type>::FindOne(Buf, Val);
		if (r < 0) return TSmartString();
		return TSmartString(Buf, r);
	}

	// Возвращает строку до первого символа не входящего в указанную строку
	TSmartString SpanIncluding(TC_Str Val) {
		if (!Buf || !Val) return TSmartString();
		size_t SZ = strangeType<Type>::FindNotOne(Buf, Val);
		return SZ ? this->Left(SZ) : TSmartString();
	}

	//Формирует строку по формату
	void Format(LC_Str StrFormat, ...) {
		va_list Par;
		va_start(Par, StrFormat);
		*this = StrFormat;
		Format(*this, Par);
		va_end(Par);
	};

	void Format(TC_Str StrFormat, ...) {
		va_list Par;
		va_start(Par, StrFormat);
		Format(StrFormat, Par);
		va_end(Par);
	};

	void Format(TC_Str StrFormat, va_list Par) {
		if (!StrFormat) return;

		size_t len = strangeType<Type>::FormatLen(StrFormat, Par);
		if (len < 1) {
			Length(0);
			return;
		}

		Info* P = DeBuf(len);
		len = IncludeBuf(StrFormat);
		if (len && P->Data == Buf) {
			size_t SZ = (len - 1);
			SZ = (SZ > P->Len ? P->Len : P->LBuf) - SZ;
			Buf = Info::Create(SZ + 1, StrFormat, SZ + 1)->Data;
			StrFormat = Buf + (len - 1);
		}
		strangeType<Type>::Format(BufPointer<Type>(P->Data, P->Len), StrFormat, Par);
		P->Data[P->Len] = 0;
		SetBuf(P);
	};

	//Добавляет строку по формату
	void AppendFormat(LC_Str StrFormat, ...) {
		va_list Par;
		va_start(Par, StrFormat);
		*this = StrFormat;
		AppendFormat(*this, Par);
		va_end(Par);
	};

	void AppendFormat(TC_Str StrFormat, ...) {
		va_list Par;
		va_start(Par, StrFormat);
		AppendFormat(StrFormat, Par);
		va_end(Par);
	};

	void AppendFormat(TC_Str StrFormat, va_list Par) {
		if (!StrFormat) return;
		size_t len = strangeType<Type>::FormatLen(StrFormat, Par);
		if (!len) {
			Length(0);
			return;
		}
		size_t SZ = Length();
		Info* P = DeBufCopy(len + SZ);
		if (P->Data == Buf && IncludeBuf(StrFormat)) {
			len = StrFormat - Buf;
			Buf = Info::Create(SZ - len + 1, StrFormat, SZ - len + 1)->Data;
			StrFormat = Buf;
		}
		strangeType<Type>::Format(BufPointer<Type>(P->Data + SZ, P->Len-SZ), StrFormat, Par);
		P->Data[P->Len] = 0;
		SetBuf(P);
	};

	//Вставляет подстроку Val начиная с позиции Pos удаляя  Count символов)
	inline size_t Insert(const BufPointer<Type>& Val, size_t Pos,  size_t Count = 0) { return InsertPrv(Pos, Val.Data, Val.Len, Count); }
	//Вставляет подстроку Val длинной Val_Size, начиная с позиции Pos удаляя  Count символов)
	inline size_t Insert(TC_Str Val, size_t Pos, size_t Count, size_t Val_Size = StringLength(Val)) { return InsertPrv(Pos, Val, Val_Size, Count); }

	const TC_Str Data()const { return Buf; }
	operator TC_Str() const throw() { return Buf; }
};

////////////////////////////////////// VRTSmartStr /// END /////////////////////////////////

template< typename BaseType = char>
class strangeType {
public:
	typedef char		T_Char;
	typedef LPSTR		T_Str;
	typedef wchar_t		L_Char;
	typedef LPWSTR		L_Str;
	typedef LPCWSTR		LC_Str;
	typedef LPCSTR		TC_Str;

	static void NumberToStr(const INT64 val, int sys, BufPointer<T_Char> &Buf) {
		_ASSERTE(sys >= 2 && sys <= 36);
		_i64toa_s(val, const_cast<T_Str>(Buf.Data), Buf.Len + 1, sys);
	}

	static size_t NumberToStr(const double val, int sys, BufPointer<char> &Buf) {
		_ASSERTE(sys >= 0);
		_gcvt_s(const_cast<char*>(Buf.Data), Buf.Len + 1, val, sys);
		return StringLength(Buf.Data);
	}

	static size_t BaseTypeLen(LC_Str pszSrc, size_t Count, UINT Code = CP_ACP) {
		if (pszSrc == nullptr) return 0;
		int sz = WideCharToMultiByte(Code, 0, pszSrc, Count ? Count : -1, nullptr, 0, NULL, nullptr);
		return  Count ? sz : sz - 1;
	}
	static void ToBaseType(BufPointer<T_Char>& Buf, LC_Str pszSrc, size_t Count, UINT Code = CP_ACP) {
		const_cast<T_Char*>(Buf.Data)[Buf.Len] = 0;
		WideCharToMultiByte(Code, 0,pszSrc, Count ? Count : -1, const_cast<char*>(Buf.Data), Buf.Len + 1, NULL, nullptr);
	}
	static int CompareStr(const TC_Str Val1, const TC_Str Val2, size_t Len1, size_t Len2, bool noCase) {
		return CompareStringA(LOCALE_CUSTOM_DEFAULT, noCase ? NORM_IGNORECASE : 0, Val1, Len1, Val2, Len2) - 2;
	}
	static T_Str Find(T_Str Val1, TC_Str Val2) {
		return strstr(Val1, Val2);
	}
	static int FindOneP(TC_Str Val1, TC_Str Val2) {
		return strpbrk(Val1, Val2);
	}
	static size_t FindNotOne(TC_Str Val1, TC_Str Val2) { return  strspn(Val1, Val2); }
	static int FindOne(TC_Str Val1, TC_Str Val2) {
		T_Str P = FindOneP(Val1, Val2);
		return  !P ? -1 : (P - Val1);
	}
	static int FormatLen(TC_Str Str, va_list Param) {
		return _vscprintf(Str, Param);
	};
	static void Format(BufPointer<T_Char>& Buf, TC_Str Str, va_list Param) {
		vsprintf_s((T_Str)Buf.Data, Buf.Len + 1, Str, Param);
	};

	static void LowerStr(T_Str Val, size_t Size) { _strlwr_s(Val, Size); }
	static void UpperStr(T_Str Val, size_t Size) { _strupr_s(Val, Size); }
	static void Reverse(T_Str Val) { _strrev(Val); }
};


template<>
class strangeType<WCHAR> {
public:
	typedef wchar_t		T_Char;
	typedef LPWSTR		T_Str;
	typedef char		L_Char;
	typedef LPSTR		L_Str;
	typedef LPCWSTR		TC_Str;
	typedef LPCSTR		LC_Str;

	static void NumberToStr(const INT64 val, int sys, BufPointer<T_Char> &Buf) {
		_ASSERTE(sys >= 2 && sys <= 36);
		_i64tow_s(val, const_cast<T_Str>(Buf.Data), Buf.Len+1, sys);
	}

	static size_t NumberToStr(const double val, int sys, BufPointer<T_Char> &Buf) {
		_ASSERTE(sys >= 0);
		T_Str Bf = const_cast<WCHAR*>(Buf.Data);
		_gcvt_s((char*)Bf, Buf.Len, val, sys);
		size_t Tm = StringLength((char*)Bf), a = Tm+ 1;
		while (a) Bf[a] = ((char*)Bf)[--a];
		return Tm;
	}

static size_t BaseTypeLen(LC_Str pszSrc, UINT Count, UINT Code = CP_ACP) {
		if (pszSrc == nullptr) return 0;
		int sz = MultiByteToWideChar(Code, 0, pszSrc, Count ? Count : -1, nullptr, 0);
		return  Count ? sz : sz - 1;
	}
static void ToBaseType(BufPointer<T_Char>& Buf, LC_Str pszSrc, int Count, UINT Code = CP_ACP) {
		MultiByteToWideChar(Code, 0, pszSrc, Count ? Count : -1, const_cast<T_Char*>(Buf.Data), Buf.Len + 1);
	}
	static int CompareStr(const TC_Str Val1, TC_Str Val2, size_t Len1, size_t Len2, bool noCase) {
		return CompareStringW(LOCALE_CUSTOM_DEFAULT, noCase ? NORM_IGNORECASE : 0, Val1, Len1, Val2, Len2) - 2;
	}
	static T_Str Find(T_Str Val1, TC_Str Val2) {
		return wcsstr(Val1, Val2);
	}
	static TC_Str FindOneP(TC_Str Val1, TC_Str Val2) {
		return wcspbrk(Val1, Val2);
	}
	static size_t FindNotOne(TC_Str Val1, TC_Str Val2) { return  wcsspn(Val1, Val2); }
	static INT64 FindOne(TC_Str Val1, TC_Str Val2) {
		TC_Str P = FindOneP(Val1, Val2);
		return  !P ? -1 : (P - Val1);
	}
	static int FormatLen(TC_Str Str, va_list Param) {
		return _vscwprintf(Str, Param);
	};
	static void Format(BufPointer<T_Char>& Buf, TC_Str Str, va_list Param) {
		vswprintf_s((T_Str)Buf.Data, Buf.Len + 1, Str, Param);
	};
	static void LowerStr(T_Str Val, size_t Size) { _wcslwr_s(Val, Size); }
	static void UpperStr(T_Str Val, size_t Size) { _wcsupr_s(Val, Size); }
	static void Reverse(T_Str Val) { _wcsrev(Val); }
};

typedef VRTSmartStr<char>		SmartStrA;
typedef VRTSmartStr<wchar_t>	SmartStrW;

#ifdef UNICODE
typedef SmartStrW SmartStr;
#else
typedef SmartStrA SmartStr;
#endif

#endif