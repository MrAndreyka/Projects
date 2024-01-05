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
	static size_t MemListindex;
	void* LP;
	bool isFree;
	size_t Size, nLine, indexIn;
	SBufer_mem(void* P, size_t S, size_t numLine) {
		LP = P; Size = S; nLine = numLine;
		isFree = false; indexIn = MemListindex++;
	}
	void Out(const char* prewstr)const 
	{ std::cout << prewstr << "	Pointer: " << LP << "	indexIn:" << indexIn << "	size: " << Size << "	in line: " << nLine << std::endl; }
};

size_t SBufer_mem::MemListindex = 0;

std::list<SBufer_mem> Bufer_mems;
auto find_in_Bufer_mems(void *lp) {
	auto p = std::find_if(Bufer_mems.begin(), Bufer_mems.end(), [lp](const SBufer_mem& x) -> bool {
		return x.LP == lp; });
	auto res = (p==Bufer_mems.end())? false: (p->isFree? false: p->isFree = true);
	
	if (!res)
		if(p == Bufer_mems.end())
			std::cout << "Release of unallocated " << lp << __LINE__;
		else
			p->Out("Re-release");
	return res;
}

void report_Bufer_mems() {

	auto p = std::for_each(Bufer_mems.begin(), Bufer_mems.end(), [](const SBufer_mem& x) {
		if (!x.isFree) x.Out("Is not free"); });
}

#define Save_toMemList(p) Bufer_mems.push_back(SBufer_mem(p,_msize(p),__LINE__))
#define Free_inMemList(p)	find_in_Bufer_mems(p)
#define report_MemList()	report_Bufer_mems()
#else
#define Save_toMemList(p)
#define Free_inMemList(p) true
#define report_MemList()
#endif

#define __max(a,b)  (((a) > (b)) ? (a) : (b))

template <class Type>
class VRTSmartStr;

template <class Type>
class  Info_Buf;

struct InfoData {
	size_t Len, Cou;
	void Set(size_t Len_, size_t Cou_) {
		Len = Len_; Cou = Cou_;
	}

	// ������������ �������� ��������
	size_t IncCou() { return InterlockedIncrementAcquire(&Cou); };
	size_t DecCou() { return InterlockedDecrementAcquire(&Cou); };
};

struct InfoDataB : InfoData {
	size_t LenBuf;
	void Set(size_t Len_, size_t LBuf_, size_t Cou_){
		InfoData::Set(Len_, Cou_);
		LenBuf = LBuf_;
	}
};

template <class Type>
struct Bufer : InfoDataB {
	Type Data[1];

	//������� ����� ����� � ���� ����� �������� � ���� ������
	// Size - ������������; Len - ������; dest - �������� ������; destLen - ������ ���������� ������
	static Bufer* Create(size_t Size, const Type* dest = nullptr, size_t destLen = 0) {
		_ASSERTE(Size != 0);
		Bufer<Type>* P = (Bufer<Type>*) new char[Size*sizeof(Type) + sizeof(InfoDataB)];
		if (!dest)
			destLen = 0;
		else {
			if (!destLen)
				destLen = Size;
			P->FillBuf(dest, destLen);
		}
		P->Set(destLen, Size, 1);
		return P;
	}

	bool FreeBuf() { return DecCou(); }

	void Destroy() {
		_ASSERT(Free_inMemList(this));
		delete []this;
	};

	//�������� � ������� ����� ������
	// BUF - ������; len - ������ ������; Beg - ������ �������
	// ������� �������� ������ ���
	void FillBuf(const Type* BUF, size_t len = 0, size_t Beg = 0) {
		_ASSERT(Beg + len > Beg);
		if (!len || Beg + len > LenBuf || Data+Beg == BUF)
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
	Cell<Type>* next = nullptr;

	Cell() {};
	Cell(Type val) :Data(val) {};
};

template <class Type>
struct BuferList : InfoData {
	using value_type = Type;

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

/*template <class Type>
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

	// ��������� ������� � ������� Pos
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
*/

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
class TransformationType;

template <class Type>
class BufPointer {
	using cl = TransformationType<Type>;

	using Base_Char = typename cl::Base_Char;
	using Base_Str = typename cl::Base_Str;
	using Base_CStr = typename cl::Base_CStr;

	using F_Char = typename cl::F_Char;
	using F_Str = typename cl::F_Str;
	using F_CStr = typename cl::F_CStr;
public:
	Base_CStr Data;
	size_t Len, *Cou = nullptr;

	~BufPointer() {
		if (!Cou) return;
		if (!InterlockedDecrementAcquire(Cou))
			((Bufer<Type>*)SHIFT(Data, -1, InfoDataB))->Destroy();
	}

	BufPointer(const BufPointer &Val) : Data(nullptr) {
		this->operator=(Val);
	}

	BufPointer(const VRTSmartStr<Type>& Val) : Data(Val) {
		Len = Val.Length();
		if (Len) {
			Cou = SHIFT(Data, -2, size_t);
			InterlockedIncrementAcquire(Cou);
		}
	}

	BufPointer(Base_CStr Val, const size_t Len = 0) : Data(Val) {
		this->Len = Len ? Len : StringLength(Val);
	}

	BufPointer(F_Str Val, size_t Size = 0) : Data(nullptr) {
		size_t Sz = cl::BaseTypeLen(Val, Size);
		Bufer<Type> *I = Bufer<Type>::Create(Sz + 1);
		Cou = &I->Cou;
		Data = re_cast(Type*, I->Data);
		Len = I->Len - 1;
		cl::ToBaseType(*this, Val, Size);
	}

	BufPointer(const INT64 Val, byte sys = 10) : Data(nullptr) {
		Bufer<Type> *I = Bufer<Type>::Create(sys == 10 ? 21 : _CVTBUFSIZE);
		Cou = &I->Cou;
		Data = I->Data;
		Len = I->Len - 1;
		cl::NumberToStr(Val, sys, *this);
		Len = StringLength(Data);
		I->Len = Len;
	}

	explicit BufPointer(const double Val, byte dig = 10) : Data(nullptr) {
		Bufer<Type> *I = Bufer<Type>::Create(dig + 31);
		Cou = &I->Cou;
		Data = I->Data;
		Len = I->Len / 2;
		cl::NumberToStr(Val, dig, *(BufPointer<char>*)this);
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
public:
	using cl = TransformationType<Type>;

	using Base_Char = typename cl::Base_Char;
	using Base_Str = typename cl::Base_Str;
	using Base_CStr = typename cl::Base_CStr;

	using F_Char = typename cl::F_Char;
	using F_Str = typename cl::F_Str;
	using F_CStr = typename cl::F_CStr;

	~Info_Buf() {
		if (!Data || type < 3) return;
		if (type == 5) {
			((VRTSmartStr<Type>*)Data)->Clear();
			delete [](Data);
		}
		else if (!InterlockedDecrementAcquire((LONG*)Data))
			delete[](Data);
	}

	Info_Buf() { Data = nullptr; }

	Info_Buf(const Info_Buf &Val) {
		Data = nullptr;
		this->operator=(Val);
	}

	Info_Buf(Base_CStr Val, const size_t Size = 0) {
		Len = Size ? Size : StringLength(Val);
		Data = const_cast<Base_Str>(Val);
		type = 1;
	}

	Info_Buf(F_CStr Val, size_t Size = 0) {
		Len = cl::BaseTypeLen(Val);
		Data = const_cast<F_Str>(Val);
		type = 2;
	}

	Info_Buf(INT64 Val, byte sys = 10) {
		From(Val, sys);
	}

	Info_Buf(const int Val, byte sys = 10) {
		From((INT64)Val, sys);
	}
	/*
	Info_Buf(const DWORD Val, byte sys = 10) {
		*this = Info_Buf((INT64)Val, sys);
	}

	Info_Buf(const size_t Val, byte sys = 10) {
		*this = Info_Buf((INT64)Val, sys);
	}*/

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
			Data = new char[sizeof(Val)];
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
	void* Data;
	size_t Len; byte type;

	friend VRTSmartStr<Type>;
	void CreateData(byte size) {
		Data = new char[sizeof(LONG) + size];
		*(LONG*)(Data) = 1;
	};

	void From(INT64 Val, byte sys = 10) {
		CreateData(sizeof(INT64) + sizeof(byte));
		*(INT64*)SHIFT(Data, 1, LONG) = Val;
		*(byte*)SHIFT(SHIFT(Data, 1, LONG), 1, INT64) = sys;
		Len = 1;
		while (Val /= 10) Len++;
		type = 3;
	}

	size_t SaveToBuf(Base_Str Buf) const {
		if (!Data) return 0;
		switch (type) {
		case 1: {
			memcpy(Buf, Data, (Len)*sizeof(Type));
			return Len;
		}break;
		case 2: {
			auto r = BufPointer<Base_Char>(Buf, Len);
			cl::ToBaseType(r, (F_Char*)Data, Len);
			return Len;
		}break;
		case 3: {
			auto r = BufPointer<Base_Char>(Buf, Len);
			cl::NumberToStr(*(INT64*)SHIFT(Data, 1, LONG), *(byte*)SHIFT(SHIFT(Data, 1, LONG), 1, INT64), r);
			return Len;
		}break;
		case 4: {
			auto r = BufPointer<Base_Char>(Buf, Len);
			return cl::NumberToStr(*(double*)SHIFT(Data, 1, LONG), *(byte*)SHIFT(SHIFT(Data, 1, LONG), 1, double), r); 
		}break;
		case 5: {
			memcpy(Buf, *(Type**)Data, (Len)*sizeof(Type));
			return Len;
		}break;
		};
		return 0;
	};
};

template <class Type>
class bufList {
public:
	template <class Type>
	struct cellBuf {
		Type val;
		cellBuf<Type>* next = nullptr;

		cellBuf() {};
		cellBuf(const Type& Val):val(Val){};
	};

	template <class Type>
	class iterator : public std::iterator<std::forward_iterator_tag, Type> {
		cellBuf<Type>* tec;
	public:
		iterator();
		iterator(const iterator& val) { tec = val.tec; };
		iterator& operator=(const iterator& val) { tec = val.tec; };
		Type& operator*() const { return tec->val; };
		Type* operator->() const { return tec->next; };
		bool operator==(const iterator& val) const { return tec == val.tec; };
		bool operator!=(const iterator& val) const { return tec != val.tec; }
		iterator& operator++() { return tec->next; };
	//	iterator operator++(int);
	//	iterator& operator--();
	//	iterator operator--(int);
	};
	/*using cl = TransformationType<Type>;

	using Base_Char = typename cl::Base_Char;
	using Base_Str = typename cl::Base_Str;
	using Base_CStr = typename cl::Base_CStr;

	using F_Char = typename cl::F_Char;
	using F_Str = typename cl::F_Str;
	using F_CStr = typename cl::F_CStr;*/

	bufList() { beg = end = nullptr; }// zero = new cellBuf<Type>(); zero->next = zero; }
	~bufList() {
		cellBuf<Type> *p, *t = beg;
		while (t != nullptr){
			p = t; t = t->next;
			delete p;}
	}

	Begin()
	End()

	bufList& operator << (const Type &Val) {
		auto a = new cellBuf<Type>(Val);
		end = end == nullptr ? beg = a : end->next = a;
		return *this;
	}

private:
	cellBuf<Type>* beg, * end;
};

template <class Type>
class VRTSmartStr {
public: 
	using stranType = TransformationType<Type>;
	using Base_Str = typename stranType::Base_Str;
	using Base_CStr = typename stranType::Base_CStr;
	using F_Char = typename stranType::F_Char;
	using F_Str = typename stranType::F_Str;
private:
	using Info = Bufer<Type>;
	using TSmartString = VRTSmartStr<Type>;

	Base_Str Buf = nullptr;

	Info* GetInfo()const { return Buf ? (Info*)SHIFT(Buf, -1, InfoDataB) : nullptr; }

	void SetBuf(Info* P) {
		if (Buf == P->Data) return;
		Clear();
		Buf = P->Data;
	}

	static size_t NB_Size(size_t Size, Info* Old){
		size_t NewSize;
		Size++;
		if (!Old || Old->LenBuf > Size) 
			return Size;
		NewSize = Old->LenBuf + (Old->LenBuf * sizeof(Base_Str) > 2097152 ? 104857 : Old->LenBuf / 2); //2097152 - 2�� 104857 - 100��
		if (NewSize < Size) NewSize = Size;
		return NewSize;
	}

	// ����������� ����� ������ ������� ��� �����������
	Info* NewBuf(const size_t requisiteSize, bool copy = false) {
		_ASSERT(requisiteSize + 1 > 1);
		Info *I = GetInfo();

		if (!I || I->Cou > 1 || I->LenBuf < requisiteSize + 1) {
			size_t NewSize = NB_Size(requisiteSize, I);
			//if (I && FirstClear) Clear();
			I = (copy && I) ? Info::Create(NewSize, I->Data, I->Len): Info::Create(NewSize);
		}
		I->Data[I->Len] = 0;
		return I;
	}

	// ���������� ������� ������� ��������� Val ������� � ������� Beg
	bool FindStr(Base_CStr Val, size_t& Pos)const {
		if (!Buf || !Val || Pos > Length()) return false;
		Base_CStr P = stranType::Find(Buf + Pos, Val);
		Pos = P == nullptr ? 0 : P - Buf;
		return P != nullptr;
	};

	// ���������� ������� ���������� ��������� Val �� ����� ������� Beg (����� ������� � ����� ������)
	bool FindStrOf(Base_CStr Val, size_t& Pos)const {
		if (!Buf || !Val || Pos > Length() || !Length()) return false;
		if (!Pos) Pos = Length();
		Base_CStr T, V, P = Buf + Pos - 1;

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
	size_t InsertPrv(size_t Pos, Base_CStr Val, size_t Size, size_t Count = 0) {
		auto LEN = Length();
		if (!Size && !Count || Pos > LEN) return LEN;
		_ASSERTE(Count + Pos >= Pos); // ������������ ���� size_t
		if (Count + Pos > LEN) Count = LEN - Pos;
		_ASSERTE(LEN + Size > LEN && LEN + Size > Size); // ������������ ���� size_t
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

	size_t RemChars(Base_CStr Home, const Info& Val) {
		Base_CStr Finds = stranType::FindOneP(Home, Val.Data);

		if (!Finds) return Length() - (Home - Buf);

		size_t P = RemChars(Finds + 1, Val);		
		Info *I = GetInfo();
		I->FillBuf(Finds +1, P, Finds-Buf);
		I->Len--;
		return Finds - Home + P ;
	};

	size_t IncludeBuf(Base_CStr Val)const {
		return (!Buf || !Val || Val<Buf || Val > Buf + GetInfo()->LenBuf) ? 0 : Val - Buf +1;
	}

	// ������������� ������ ������ �� ����� Size, 
	//�������� ��� �������� ������ � Val �������� Len ��� �� ����� ������ ������� � Beg
	void SetValue(size_t Size, Base_CStr Val, size_t Len, const size_t Beg = 0) {
		Info *I = NewBuf(Size, !IncludeBuf(Val));
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
			Buf = const_cast<Base_Str>(Val.Data);
			GetInfo()->IncCou();
		}	
	};

	VRTSmartStr(Base_CStr Val, size_t Size = 0) {
		if (!Size) Size = StringLength(Val);
		SetValue(Size, Val, Size);
	};

	VRTSmartStr(F_Str Val, size_t Size = 0) {
		size_t Len = stranType::BaseTypeLen(Val, Size);
		Info *I = DeBuf(Len);
		stranType::ToBaseType(Info(I->Data, I->Len), Val, Size);
		I->Data[I->Len] = 0;
		SetBuf(I);
	};

	VRTSmartStr(const INT64 Val, byte sys = 10) {
		BufPointer<Type> r(Val, sys);
		*this = r;
	};

	/*explicit VRTSmartStr(const double Val, byte sys = 10) {
		BufPointer<Type> r(Val, sys);
		*this = r;
	};*/

	Base_Str GetBuffer(size_t NewSize = 0) {
		if (NewSize) {
			auto I = NewBuf(NewSize);
			SetBuf(I);
			return I->Data;
		}
		_ASSERT(Length());
		return Buf;
	};

	Base_Str ReleaseBuffer(size_t NewSize = 0) {
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
		
	TSmartString& operator = (Base_CStr Val) {
		size_t Size = StringLength(Val);
		SetValue(Size, Val, Size);
		return *this;
	};

	TSmartString& operator = (F_Str Val) {
		size_t Len = stranType::BaseTypeLen(Val, 0);
		Info* I = DeBuf(Len, true);
		stranType::ToBaseType(BufPointer<Type>(I->Data, I->Len), Val, 0);
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


	TSmartString& operator += (const bufList<Type>& Val) {


		if (Val.Len) {
			auto ol = Length();
			auto I = NewBuf(Val.Len + ol, true);
			I->Len = ol + Val.SaveToBuf(I->Data + ol);
			I->Data[I->Len] = 0;
			SetBuf(I);
		}
		return *this;
	};


	TSmartString& operator += (const Info_Buf<Type>& Val) {
		if (Val.Len) {
			auto ol = Length();
			auto I = NewBuf(Val.Len + ol, true);
			I->Len = ol + Val.SaveToBuf(I->Data + ol);
			I->Data[I->Len] = 0;
			SetBuf(I);
		}
		return *this;
	};
	TSmartString& operator += (F_Str Val) {
		size_t Len = stranType::BaseTypeLen(Val, 0);
		Length(Len + Length(), true);
		Info* I = GetInfo();
		stranType::ToBaseType(BufPointer<Type> (Buf + I->Len, Len), Val, 0);
		I->Len += Len;
		return *this;
	};

	TSmartString& operator << (const Info_Buf<Type>& Val) {
		return this->operator+=(Val);
	};
	
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

	static friend TSmartString operator + (const TSmartString& Val1, const BufPointer<Type>& Val2) {
		if (!Val1.Length()) return TSmartString(Val2);
		if (!Val2.Data) return Val1;
		Info* buf = Info::Create(Val2.Len + Val1.Length() + 1, Val1.Buf, Val1.Length());
		buf->FillBuf(Val2.Data, Val2.Len, Val1.Length());
		buf->Data[--buf->Len] = 0;
		TSmartString _tmp;
		_tmp.Buf = buf->Data;
		return _tmp;
	};
	
	friend TSmartString operator + (const BufPointer<Type>& Val1, const TSmartString& Val2) {
		if (!Val1.Len) return Val2;
		if (!Val2.Data) return Val1;
		Info* buf = Info::Create(Val2.Length() + Val1.Len + 1, Val1.Data, Val1.Len);
		buf->FillBuf(Val2.Buf, Val2.Length(), Val1.Len);
		buf->Data[--buf->Len] = 0;
		TSmartString _tmp;
		_tmp.Buf = buf->Data;
		return _tmp;
	};

	friend TSmartString operator + (const TSmartString& Val1, const TSmartString& Val2) {
		if (!Val1.Length()) return Val2;
		if (!Val2.Length()) return Val1;
		Info* buf = Info::Create(Val1.Length() + Val2.Length() + 1, Val1.Buf, Val1.Length());
		buf->FillBuf(Val2.Buf, Val2.Length(), Val1.Length());
		buf->Data[--buf->Len] = 0;
		TSmartString _tmp;
		_tmp.Buf = buf->Data;
		return _tmp;
	};

	inline friend TSmartString operator + (const TSmartString& Val1, const Base_Str Val2) {
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

	inline friend TSmartString operator + (const Base_Str Val1, const TSmartString& Val2) {
		if (!Val1) return Val2;
		if (!Val2.Length()) return TSmartString(Val1);
		size_t Sz = StringLength(Val1);
		Info* buf = Info::Create(Sz + Val2.Length() + 1, Val1, Sz);
		buf->FillBuf(Val2.Buf, Val2.Length(), Sz);
		buf->Data[--buf->Len] = 0;
		TSmartString _tmp;
		_tmp.Buf = buf->Data;
		return _tmp;
	};

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
		return Val1.Compare(Val2) == 0;
	};
	
	friend bool operator==(
		const TSmartString& str1,
		Base_CStr psz2
		) throw() {
		return !str1.Compare(psz2);
	};

	inline friend bool operator != (const TSmartString& Val1, const TSmartString& Val2) throw() {
		return Val1.Compare (Val2) !=0;
	};

	friend bool operator!=(
		const TSmartString& str1,
		Base_CStr psz2
		) throw() {
		return str1.Compare(psz2);
	};

	/*/��������� ��������� ������������� �����
	TSmartString& SetInt(INT64 Val, byte sys = 10) {
		Bufer<Type> *I = NewBuf(sys == 10 ? 21 : _CVTBUFSIZE);
		stranType::NumberToStr(Val, sys, BufPointer<Type>(I->Data, I->LBuf-1));
		I->Len = StringLength(I->Data, I->Len);
		SetBuf(I);		
		return *this;
	}

	TSmartString& SetFloat(const double Val, byte dig = 10) {
		Bufer<Type> *I = DeBuf(dig + 31);
		I->Len = stranType::NumberToStr(Val, dig, BufPointer<Type>(I->Data, I->LBuf-1));
		I->Data[I->Len] = 0;
		SetBuf(I);
		return *this;
	}*/

	//�������� � ������ ���������� ����������, FullSize = ����� ������, ���������: ������ � ��������� �� ������ ������ ����������� ���� �� ������ �� FullSize
	//	���������� ����� ������ ������
	size_t Concatenation(size_t FullSize, ...) { return Concatenation((va_list)&FullSize); }

	size_t Concatenation(va_list Par) {
		size_t tmp, Zp(0), FullSize = va_arg(Par, size_t);	

		if (!FullSize) return Length(0);
		Info* I = NewBuf(FullSize, true);

		while (FullSize > Zp) {
			tmp = va_arg(Par, size_t);
			I->FillBuf(va_arg(Par, Base_Str), tmp, Zp);
			Zp += tmp;
		}
		va_end(Par);
		I->Len = Zp;
		I->Data[I->Len] = 0;
		SetBuf(I);
		return Zp;
	}

	inline bool isEmpty() { return !Length();}

	//���������� ������ ������ ������ �������� ������ ����� ������
	inline size_t BufLength()const { return Buf ? GetInfo()->LBuf : 0; };

	//���������� ����� ������, ��� ������� ����� ������
	inline size_t Length()const { return Buf ? GetInfo()->Len : 0; };
	
	//������������ ���������� ����� ������ �� ����� maxCou �������� ��������� � �����, ��� ������������� ��������� ������ ����� ������
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

	//������������� ������ ������ 
	//(Size - ������ (�� �������� ������� ����� ������), Copy - ���������� �� ����������, Resizible - ��������� �� ������ ������)
	size_t Length(size_t Size, bool Copy = false, bool Resizible = false) {
		_ASSERTE(Size != -1);
		Info *I = GetInfo();
		
		if (!Size) {
			if (Resizible) Clear();
			elseif(I) I->Data[0] = 0;
			return 0;
		}

		if (!I || I->Cou > 1 || I->LBuf < Size + 1 || Resizible) {
			I = NewBuf(Size + 1, Copy);
			//!!!!!!!!!!!
			/*if (Copy) {
				I->Len = min(Length(), Size);
				if (Buf != I->Data && I->Len) I->FillBuf(Buf, I->Len, 0);
			} else I->Len = 0;*/
		} else if (Size < I->Len) I->Len = Size;

		I->Data[I->Len] = 0;
		SetBuf(I);
		return I->Len;
	}

	//������� ������, ���������� �����
	void Clear() {
		if (!Buf) return;
		Info *T = GetInfo();
		if (!T->DecCou()) 
			T->Destroy();
		Buf = nullptr;
	};

	//������������ �� ������� val ������� Sz. ���������� >0 ���� Val ������ 
	//	NoCase - ��������� �� ���������
	int Compare(const TSmartString Val, bool NoCase = false)const {
		return stranType::CompareStr(Data(), Val.Data(), Length(), Val.Length(), NoCase);
		//�� "const wchar_t *" � "const strangeType<WCHAR>::Base_Str"
	}
	int Compare(const Base_CStr Val, size_t Sz = 0, bool NoCase = false)const {
		if (!Sz) Sz = StringLength(Val);
		return stranType::CompareStr(Data(), Val, Length(), Sz, NoCase);
	}

	//������� ����� ������ ������� c Beg ������, ������ Count
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

	//���������� ����� ����� ������ (size_t ����������)
	TSmartString Left(size_t Count)const {
		if (Count >= Length()) return *this;
		return TSmartString(Buf, Count);
	}

	//���������� ������ ����� ������ (size_t ����������)
	TSmartString Rigth(size_t Count)const {
		if (Count >= Length()) return *this;
		return TSmartString(Buf + Length() - Count, Count);
	}

	//���������� ������� ������ (size_t ������, size_t ���������� = 1)
	TSmartString Mid(size_t Beg, size_t Count = 1)const {
		if (!Buf || !Count || Beg >= Length() || (!Beg && Count == Length())) return TSmartString();
		if (Count + Beg > Length() || Count > Length())
			Count = Length() - Beg;
		return TSmartString(Buf + Beg, Count);
	}

	// ��������� ������ � ������ �������
	TSmartString& MakeLower() {
		if (!Buf) return *this;
		Info *P = DeBufCopy(Length());
		stranType::LowerStr(P->Data, P->Len);
		SetBuf(P);
		return *this;
	}

	// ��������� ������ � ������ �������
	TSmartString& MakeUpper() {
		if (!Buf) return *this;
		Info *P = DeBufCopy(Length());
		stranType::UpperStr(P->Data, P->Len);
		SetBuf(P);
		return *this;
	}

	void Reverse() {
		if (!Buf) return;
		Info *P = DeBufCopy(Length());
		stranType::Reverse(P->Data);
		SetBuf(P);
		return *this;
	}

	// ���� �������� Val � ������ ������� � ������� Pos. ��� �� ��������� ������
	bool Find(const BufPointer<Type>& Val, size_t& Pos)const {
		return FindStr(Val.Data, Pos);
	};	
	
	/*bool Find(const TSmartString& Val, size_t& Pos)const {
		return FindStr(Val.Buf, Pos);
	};

	INT64 Find(Base_CStr Val, size_t Beg = 0)const {
		return FindStr(Val, Beg);
	};

	INT64 Find(F_Str Val, size_t Beg = 0)const {
		TSmartString _tmp(Val);
		return FindStr(_tmp, Beg);
	};

	INT64 Find(const BufPointer<Type>& Val, size_t Beg = 0)const {
		return FindStr(Val.Data, Beg);
	};
*/
	// ���� ��������� � ������ ������� � �����(��� ����, ������ ������ ������ = 0)
/*	bool FindOf(const TSmartString& Val, size_t& Pos)const {
		return FindStrOf(Val.Buf, Pos);
	};*/
	bool FindOf(const BufPointer<Type>& Val, size_t& Pos)const {
		return FindStrOf(Val.Data, Pos);
	};

	/*INT64 FindOf(Base_CStr Val, size_t Beg = 0)const {
		return FindStrOf(Val, Beg);
	};

	INT64 FindOf(F_Str Val, size_t Beg = 0)const {
		TSmartString _tmp(Val);
		return FindStrOf(_tmp, Beg);
	};*/

	//���� � ������ ������� ������� ������� �� ��������� ������
	INT64 FindOne(const Base_Str Val)const { return stranType::FindOne(Buf, Val); };
	//���� � ������ ������� ������� ������� �� ��������� ������
	INT64 FindOne(const BufPointer<Type>& Val, size_t Beg = -1, size_t Len = 0)const {
		return stranType::FindOne(Buf, Val.Data());};

	void RemoveChars(const BufPointer<Type>& Val) {
		if (!Buf) return;
		RemChars(Buf, Val);
		Buf[Length()] = 0;
	};

	// �������� � ������ ��� ��������� ValOld �� ValNew, ���������� ���������� �����
	size_t Replace(const BufPointer<Type>& ValOld, const BufPointer<Type>& ValNew) {
		if (!Buf || !ValOld.Len) return 0;
		size_t Cou(0), T = Length();
		Base_Str BegP = Buf, EndP = Buf + T;

		{while (BegP < EndP && (BegP = stranType::Find(BegP, ValOld.Data)))
			Cou++, BegP += ValOld.Len; }
		if (!Cou) return 0;

		_ASSERTE(!IncludeBuf(ValOld.Data) && !IncludeBuf(ValNew.Data));

		Info *I;
		size_t NewLen = T + int(Cou * (ValNew.Len - ValOld.Len));
		I = DeBufCopy(NewLen);
		
		BegP = Buf, EndP = Buf + T;
		while (BegP < EndP && (BegP = stranType::Find(BegP, ValOld.Data))) {
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

	// ���������� ������ �� ������� ������� �� ��������� ������
	TSmartString SpanExcluding(Base_CStr Val) {
		if (!Buf || !Val) return *this;
		int r = stranType::FindOne(Buf, Val);
		if (r < 0) return TSmartString();
		return TSmartString(Buf, r);
	}

	// ���������� ������ �� ������� ������� �� ��������� � ��������� ������
	TSmartString SpanIncluding(Base_CStr Val) {
		if (!Buf || !Val) return TSmartString();
		size_t SZ = stranType::FindNotOne(Buf, Val);
		return SZ ? this->Left(SZ) : TSmartString();
	}

	//��������� ������ �� �������
	void Format(F_Str StrFormat, ...) {
		va_list Par;
		va_start(Par, StrFormat);
		*this = StrFormat;
		Format(*this, Par);
		va_end(Par);
	};

	void Format(Base_CStr StrFormat, ...) {
		va_list Par;
		va_start(Par, StrFormat);
		Format(StrFormat, Par);
		va_end(Par);
	};

	void Format(Base_CStr StrFormat, va_list Par) {
		if (!StrFormat) return;

		size_t len = stranType::FormatLen(StrFormat, Par);
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
		stranType::Format(BufPointer<Type>(P->Data, P->Len), StrFormat, Par);
		P->Data[P->Len] = 0;
		SetBuf(P);
	};

	//��������� ������ �� �������
	void AppendFormat(F_Str StrFormat, ...) {
		va_list Par;
		va_start(Par, StrFormat);
		*this = StrFormat;
		AppendFormat(*this, Par);
		va_end(Par);
	};

	void AppendFormat(Base_CStr StrFormat, ...) {
		va_list Par;
		va_start(Par, StrFormat);
		AppendFormat(StrFormat, Par);
		va_end(Par);
	};

	void AppendFormat(Base_CStr StrFormat, va_list Par) {
		if (!StrFormat) return;
		size_t len = stranType::FormatLen(StrFormat, Par);
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
		stranType::Format(BufPointer<Type>(P->Data + SZ, P->Len-SZ), StrFormat, Par);
		P->Data[P->Len] = 0;
		SetBuf(P);
	};

	//��������� ��������� Val ������� � ������� Pos ������  Count ��������)
	inline size_t Insert(const BufPointer<Type>& Val, size_t Pos,  size_t Count = 0) { return InsertPrv(Pos, Val.Data, Val.Len, Count); }
	//��������� ��������� Val ������� Val_Size, ������� � ������� Pos ������  Count ��������)
	inline size_t Insert(Base_CStr Val, size_t Pos, size_t Count, size_t Val_Size = StringLength(Val)) { return InsertPrv(Pos, Val, Val_Size, Count); }

	const Base_CStr Data() const { return Buf; }
	operator Base_CStr() const throw() { return Buf; }
};

////////////////////////////////////// VRTSmartStr /// END /////////////////////////////////

template<typename t = wchar_t>
void _i64to_s(INT64 Value, LPWSTR Buffer, size_t BufferCount, int Radix) {
	_i64tow_s(Value, Buffer, BufferCount, Radix);
}

template<typename t = char>
void _i64to_s(INT64 Value, LPSTR Buffer, size_t BufferCount, int Radix) {
	_i64toa_s(Value, Buffer, BufferCount, Radix);
}

template< typename BaseType, typename FriendType>
class descriptionType {
public:
	using Base_Char = BaseType;
	using Base_Str =  BaseType*;
	using Base_CStr = const BaseType*;

	using F_Char = FriendType;
	using F_Str =  FriendType*;
	using F_CStr = const FriendType*;
};


template< typename BaseType = char>
class TransformationType {
public:
	using cl = descriptionType<BaseType, wchar_t>;

	using Base_Char = typename cl::Base_Char;
	using Base_Str = typename cl::Base_Str;
	using Base_CStr = typename cl::Base_CStr;

	using F_Char = typename cl::F_Char;
	using F_Str = typename cl::F_Str;
	using F_CStr = typename cl::F_CStr;
	
	static void NumberToStr(const INT64 val, byte sys, BufPointer<Base_Char> &Buf) {
		_ASSERTE(sys >= 2 && sys <= 36);
		_i64toa_s(val, const_cast<Base_Str>(Buf.Data), Buf.Len + 1, sys);
	}

	static size_t NumberToStr(const double val, byte sys, BufPointer<Base_Char> &Buf) {
		_ASSERTE(sys >= 0);
		_gcvt_s(const_cast<char*>(Buf.Data), Buf.Len + 1, val, sys);
		return StringLength(Buf.Data);
	}

	static size_t BaseTypeLen(F_Str pszSrc, size_t Count, UINT Code = CP_ACP) {
		if (pszSrc == nullptr) return 0;
		int sz = WideCharToMultiByte(Code, 0, pszSrc, Count ? Count : -1, nullptr, 0, NULL, nullptr);
		return  Count!=0 ? sz : sz - 1;
	}
	static void ToBaseType(BufPointer<Base_Char>& Buf, F_Str pszSrc, size_t Count, UINT Code = CP_ACP) {
		const_cast<cl::Base_Str>(Buf.Data)[Buf.Len] = 0;
		WideCharToMultiByte(Code, 0,pszSrc, Count ? Count : -1, const_cast<char*>(Buf.Data), Buf.Len + 1, NULL, nullptr);
	}
	static int CompareStr(Base_CStr Val1, Base_CStr Val2, size_t Len1, size_t Len2, bool noCase) {
		return CompareStringA(LOCALE_CUSTOM_DEFAULT, noCase ? NORM_IGNORECASE : 0, Val1, Len1, Val2, Len2) - 2;
	}
	static Base_Str Find(Base_Str Val1, Base_CStr Val2) {
		return strstr(Val1, Val2);
	}
	static int FindOneP(Base_CStr Val1, Base_CStr Val2) {
		return strpbrk(Val1, Val2);
	}
	static size_t FindNotOne(Base_CStr Val1, Base_CStr Val2) { return  strspn(Val1, Val2); }
	static int FindOne(Base_CStr Val1, Base_CStr Val2) {
		Base_Str P = FindOneP(Val1, Val2);
		return  !P ? -1 : (P - Val1);
	}
	static int FormatLen(Base_CStr Str, va_list Param) {
		return _vscprintf(Str, Param);
	};
	static void Format(BufPointer<Base_Char>& Buf, Base_CStr Str, va_list Param) {
		vsprintf_s((Base_Str)Buf.Data, Buf.Len + 1, Str, Param);
	};

	static void LowerStr(Base_Str Val, size_t Size) { _strlwr_s(Val, Size); }
	static void UpperStr(Base_Str Val, size_t Size) { _strupr_s(Val, Size); }
	static void Reverse(Base_Str Val) { _strrev(Val); }
};


template<>
class TransformationType<WCHAR>{
public:
	using cl = descriptionType<WCHAR, char>;

	using Base_Char = typename cl::Base_Char;
	using Base_Str = typename cl::Base_Str;
	using Base_CStr = typename cl::Base_CStr;

	using F_Char = typename cl::F_Char;
	using F_Str = typename cl::F_Str;
	using F_CStr = typename cl::F_CStr;

	static void NumberToStr(const INT64 val, int sys, BufPointer<cl::Base_Char> &Buf) {
		_ASSERTE(sys >= 2 && sys <= 36);
		_i64tow_s(val, const_cast<Base_Str>(Buf.Data), Buf.Len+1, sys);
	}

	static size_t NumberToStr(const double val, int sys, BufPointer<cl::Base_Char> &Buf) {
		_ASSERTE(sys >= 0);
		cl::Base_Str Bf = const_cast<WCHAR*>(Buf.Data);
		_gcvt_s((char*)Bf, Buf.Len, val, sys);
		size_t Tm = StringLength((char*)Bf), a = Tm+ 1;
		while (a) Bf[a] = ((char*)Bf)[--a];
		return Tm;
	}

static size_t BaseTypeLen(cl::F_Str pszSrc, UINT Count, UINT Code = CP_ACP) {
		if (pszSrc == nullptr) return 0;
		int sz = MultiByteToWideChar(Code, 0, pszSrc, Count ? Count : -1, nullptr, 0);
		return  Count ? sz : sz - 1;
	}
static void ToBaseType(BufPointer<Base_Char>& Buf, cl::F_Str pszSrc, int Count, UINT Code = CP_ACP) {
		MultiByteToWideChar(Code, 0, pszSrc, Count ? Count : -1, const_cast<Base_Str>(Buf.Data), Buf.Len + 1);
	}
	static int CompareStr(const cl::Base_CStr Val1, cl::Base_CStr Val2, size_t Len1, size_t Len2, bool noCase) {
		return CompareStringW(LOCALE_CUSTOM_DEFAULT, noCase ? NORM_IGNORECASE : 0, Val1, Len1, Val2, Len2) - 2;
	}
	static cl::Base_Str Find(cl::Base_Str Val1, cl::Base_CStr Val2) {
		return wcsstr(Val1, Val2);
	}
	static cl::Base_CStr FindOneP(cl::Base_CStr Val1, cl::Base_CStr Val2) {
		return wcspbrk(Val1, Val2);
	}
	static size_t FindNotOne(cl::Base_CStr Val1, cl::Base_CStr Val2) { return  wcsspn(Val1, Val2); }
	static INT64 FindOne(cl::Base_CStr Val1, cl::Base_CStr Val2) {
		cl::Base_CStr P = FindOneP(Val1, Val2);
		return  !P ? -1 : (P - Val1);
	}
	static int FormatLen(cl::Base_CStr Str, va_list Param) {
		return _vscwprintf(Str, Param);
	};
	static void Format(BufPointer<cl::Base_Char>& Buf, cl::Base_CStr Str, va_list Param) {
		vswprintf_s((cl::Base_Str)Buf.Data, Buf.Len + 1, Str, Param);
	};
	static void LowerStr(cl::Base_Str Val, size_t Size) { _wcslwr_s(Val, Size); }
	static void UpperStr(cl::Base_Str Val, size_t Size) { _wcsupr_s(Val, Size); }
	static void Reverse(cl::Base_Str Val) { _wcsrev(Val); }
};

typedef VRTSmartStr<char>		SmartStrA;
typedef VRTSmartStr<wchar_t>	SmartStrW;

#ifdef UNICODE
typedef SmartStrW SmartStr;
#else
typedef SmartStrA SmartStr;
#endif

#endif