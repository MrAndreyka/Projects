#ifndef SmatrClasses
#define SmatrClasses

#include <windows.h>
#include "MyExcept.h"
#include "WCHAR.h"

#define SHIFT(P, Sz, Tp)	((Tp*)P + Sz)
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
		//Bufer<Type>* P = (Bufer<Type>*)new char[sizeof(Type) * Size + sizeof(InfoDataB)];
		Bufer<Type>* P = (Bufer<Type>*)malloc(sizeof(Type)* Size + sizeof(InfoDataB));
		if (!P) {			
			int e;
			_get_errno(&e);
			TMyEx E(1, nullptr, "Bufer", &Size, sizeof(Size));
			E.Str = new char[75];
			strcpy_s(E.Str, 9, "Create: ");
			strerror_s(E.Str + 8, 75 - 8, e);
			throw E;

			/*VRTSmartStr<char> ECreate;
			E.Length(75);
			E << "Bufer->Create(" << Size << "): ";
			strerror_s(E.GetBuffer() + E.Length(), 75 - E.Length(), e);
			E.lengthDetermine();
			throw E;*/
			return nullptr;
		}
		P->Set(Size, Size, 1);
		if (Val) {
			if (!Len) Len = Size;
			P->FillBuf(Val, Len);
		}
		return P;
	}

	//Переоздает новый буфер и копирует из старого данные
	static Bufer* ReCreate(Bufer* Old, size_t Size) {
		/*Bufer<Type>* P = (Bufer<Type>*)new char[sizeof(Type) * Size + sizeof(InfoDataB)];
		P->FillBuf(Old, _msize(Old));
		delete[]Old;*/
		Bufer<Type>* P;
		if (!Old || Old->Cou == 1) P = (Bufer<Type>*)realloc(Old, sizeof(Type)* Size + sizeof(InfoDataB));
		else {
			P = Create(Size, Old->Data, Old->Len+1);
			if (!P) return Old;
			if(!Old->DecCou ()) Old->Destroy();
			return P;
		}
		if (!P) {
			int e;
			_get_errno(&e);
			VRTSmartStr<char> E;
			E.Length(75);
			E << "Bufer->Create(" << Size << "): ";
			strerror_s(E.GetBuffer() + E.Length(), 75 - E.Length(), e);
			E.lengthDetermine();
			throw E;
			return Old;
		}
		P->Set(Size, Size, 1);
		return P;
	}

	void Destroy() {
		//delete[]this;
		free(this);
	};

	//Копирует в готовый буфер данные
	// BUF - Данные; len - Размер данных; Beg - Начало вставки
	// никакой проверки данных нет
	void FillBuf(const Type* BUF, size_t len = 0, size_t Beg = 0) {
		if (!len) len = Len - Beg;
		if (Beg + len > LBuf)
			return;
		memcpy(Data + Beg, BUF, len * sizeof(Type));
	}
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
class InfoBuf {
	typedef typename strangeType<Type>::L_Char L_Char;
	typedef typename strangeType<Type>::T_Str T_Str;
	typedef typename strangeType<Type>::TC_Str TC_Str;
	typedef typename strangeType<Type>::LC_Str LC_Str;
public:
	TC_Str Data;
	size_t Len, *Cou = nullptr;

	~InfoBuf() {
		if (!Cou) return;
		if (!InterlockedDecrementAcquire(Cou)) 
			((Bufer<Type>*)SHIFT(Data, -1, InfoDataB))->Destroy();
	}

	InfoBuf(const InfoBuf &Val) : Data(nullptr) {
		this->operator=(Val);
	}

	InfoBuf(const VRTSmartStr<Type>& Val) : Data(Val) {
		Len = Val.Length();
		Data = Val.Data();
	}

	InfoBuf(VRTSmartStr<Type>& Val) : Data(Val) {
		Len = Val.Length();
		Cou = SHIFT(Data, -2, size_t);
		InterlockedIncrementAcquire(Cou);
	}

	InfoBuf(TC_Str Val, const size_t Len = 0) : Data(Val) {
		this->Len = Len ? Len : StringLength(Val);
	}

	InfoBuf(LC_Str Val, size_t Size = 0) : Data(nullptr) {
		size_t Sz = strangeType<Type>::BaseTypeLen(Val, Size);
		Bufer<Type> *I = Bufer<Type>::Create(Sz + 1);
		Cou = &I->Cou;
		Data = I->Data;
		Len = I->Len - 1;
		strangeType<Type>::ToBaseType(*this, Val, Size);
	}

	InfoBuf(const INT64 Val, byte sys = 10) : Data(nullptr) {
		Bufer<Type> *I = Bufer<Type>::Create(sys==10 ? 21 : _CVTBUFSIZE);
		Cou = &I->Cou;
		Data = I->Data;
		Len = I->Len - 1;
		strangeType<Type>::NumberToStr(Val, sys, *this);
		Len = StringLength(Data);
		I->Len = Len;
	}

	InfoBuf(const int Val, byte sys = 10) : Data(nullptr) {
		*this = InfoBuf((INT64)Val);
	}

	InfoBuf(const DWORD Val, byte sys = 10) : Data(nullptr) {
		*this = InfoBuf((INT64)Val);
	}

	InfoBuf(const size_t Val, byte sys = 10) : Data(nullptr) {
		*this = InfoBuf((INT64)Val);
	}

	InfoBuf(const double Val, byte dig = 10) : Data(nullptr) {
		Bufer<Type> *I = Bufer<Type>::Create(dig + 31);
		Cou = &I->Cou;
		Data = I->Data;
		Len = I->Len/2;
		strangeType<Type>::NumberToStr(Val, dig, *(InfoBuf<char>*)this);
		I->Len = Len = StringLength((char*)Data);
		I->Data[Len] = 0;
		size_t Tm = Len;
		while (Tm) 	I->Data[Tm] = ((char*)Data)[--Tm];
	}

	InfoBuf& operator =(const InfoBuf &Val) {
		if (Val.Cou) InterlockedIncrementAcquire(Val.Cou);
		memcpy(this, &Val, sizeof(*this));
		return *this;
	}
};

template <class Type> class VRTSmartStr;

template <class Type>
class InfoBuf2 {
	typedef typename strangeType<Type>::L_Char L_Char;
	typedef typename strangeType<Type>::T_Str T_Str;
	typedef typename strangeType<Type>::TC_Str TC_Str;
	typedef typename strangeType<Type>::LC_Str LC_Str;
public:
	void* Data = nullptr;
	size_t Len, type;

	~InfoBuf2() {
		if (!Data) return;
		if (!InterlockedDecrementAcquire((LONG*)Data)) {
			free(Data);
		}
	}

	InfoBuf2(const InfoBuf2 &Val) {
		this->operator=(Val);
	}

	InfoBuf2(VRTSmartStr<Type>& Val) : Data(Val) {
		Len = Val.Length();
		InterlockedIncrementAcquire(SHIFT(Data, -2, size_t));
		type = 1;
	}	
	
	InfoBuf2(const VRTSmartStr<Type>& Val) : Data(Val) {
		Len = Val.Length();
		Data = Val.Data();
		type = 2;
	}

	InfoBuf2(TC_Str Val, const size_t Size = 0) : Data(Val) {
		Len = Size ? Size : StringLength(Val);
		type = 2;
	}

	InfoBuf2(LC_Str Val, size_t Size = 0) : Data(Val) {
		Len = Size;
		type = 3;
	}

	void CreateData(byte size) {
		Data = malloc(sizeof(LONG) + size);
		*(LONG*)(Data) = 1;
	};

	InfoBuf2(INT64 Val, byte sys = 10) {
		CreateData(sizeof(INT64)+sizeof(byte));
		*(INT64*)SHIFT(Data, 1, LONG) = Val;
		*(byte*)SHIFT(SHIFT(Data, 1, LONG), 1, INT64) = sys;
		Len = sys == 10 ? 21 : _CVTBUFSIZE;
		type = 4;
	}

	InfoBuf2(const int Val, byte sys = 10) : Data(nullptr) {
		*this = InfoBuf2((INT64)Val, sys);
	}

	InfoBuf2(const DWORD Val, byte sys = 10) : Data(nullptr) {
		*this = InfoBuf2((INT64)Val, sys);
	}

	InfoBuf2(const size_t Val, byte sys = 10) : Data(nullptr) {
		*this = InfoBuf2((INT64)Val, sys);
	}

	InfoBuf2(const double Val, byte dig = 10) : Data(Val&) {
		Len = dig;
	}



	InfoBuf2& operator = (const InfoBuf2 &Val) {
		if (Data) this->~InfoBuf2();
		if (Val.Data) InterlockedIncrementAcquire((LONG*)Val.Data);
		memcpy(this, &Val, sizeof(*this));
		return *this;
	}
private:
	friend VRTSmartStr<Type>;
	size_t SaveToBuf(T_Str Buf) {
		if (!Data) return 0;
		if (type == 4) {
			strangeType<Type>::NumberToStr(*(INT64*)SHIFT(Data, 1, LONG), *(byte*)SHIFT(SHIFT(Data, 1, LONG), 1, INT64), Buf, Len);
			return StringLength(Buf);
		}
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

	static size_t NB_Size(size_t Size, Info* Old) {
		size_t SZ;
		if (Old && Old->LBuf < Size + 1) {
			if (Old->LBuf * sizeof(Type) > 1024 * 1024 * 1024) SZ = Old->LBuf + 1024 * 1024;
			else SZ = Old->LBuf + Old->LBuf / 2;
		} else return Size;
		if (SZ < Size) SZ = Size;
		return SZ;
	}

	// Создает новый бефер размера Size, 
	// Pointer - указатель на старый блок
	// Copy - указывает нужно ли копировать содержимое блока
	static Info* NewBuf(size_t Size, Info *Pointer = nullptr) {
		Info *I;
		if(Pointer)	I = Info::ReCreate(Pointer, Size + 1);
		else I = Info::Create(Size + 1);
		I->Len = Size;
		return I;
	}

	// Пересоздает буфер нового размера без копирования
	Info* DeBuf(const size_t Size, bool FirstClear = false) {
		Info *I = GetInfo();
		if (!I || I->Cou > 1 || I->LBuf < Size + 1) {
			size_t SZ = NB_Size(Size, I);
			if (I && FirstClear) Clear();
			I = NewBuf(SZ);
		}
		I->Len = Size;
		return I;
	}

	// Пересоздает буфер нового размера с копированием
	Info* DeBufCopy(const size_t Size) {
		Info *I = GetInfo();
		if (!I || I->Cou > 1 || I->LBuf < Size + 1) {
			size_t SZ = NB_Size(Size, I);
			I = NewBuf(SZ, I);
			if (I->Data != Buf) Buf = I->Data;
		} 
		I->Len = Size;
		return I;
	}


	int FindStr(TC_Str Val, size_t Beg)const {
		if (!Buf || Beg > Length() || !Val) return -1;
		TC_Str P = strangeType<Type>::Find(Buf + Beg, Val);
		return P == nullptr ? -1 : P - Buf;
	};

	int FindStrOf(TC_Str Val, size_t Beg)const {
		if (!Buf || !Val || Beg > Length() || !Length()) return -1;
		if (!Beg) Beg = Length();
		TC_Str T, V, P = Buf + Beg - 1;

		while (P != Buf) {
			if (*P == *Val) {
				T = P, V = Val;
				while (*(T++) == *(V++) && *V && *T) {}
				if (!*V) return P - Buf;
			}
			P--;
		}
		return -1;
	};
	size_t InsertPrv(size_t Pos, InfoBuf<Type>& Val, size_t Count = 0) {
		size_t LEN = Length();
		if (!Val.Len) return LEN;
		if (Pos > LEN) Pos = LEN;
		if (Count + Pos > LEN) Count = LEN - Pos;
		Info *P = DeBuf(Val.Len + LEN - Count);
		if (Pos && P->Data != Buf) P->FillBuf(Buf, Pos);
		if (Count + Pos < LEN && P->Data != Buf && LEN <> Count)
			P->FillBuf(Buf + Pos + Count, LEN - Pos - Count, Val.Len + Pos);
		P->FillBuf(Val.Data, Val.Len, Pos);
		P->Data[P->Len] = 0;
		SetBuf(P);
		return P->Len;
	}
	size_t RemChars(TC_Str Home, const InfoBuf<Type>& Val) {
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

	VRTSmartStr(const InfoBuf<Type>& Val) {
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
		strangeType<Type>::ToBaseType(InfoBuf<Type>(I->Data, I->Len), Val, Size);
		I->Data[I->Len] = 0;
		SetBuf(I);
	};

	T_Str GetBuffer(size_t NewSize = 0) {
		Length(NewSize ? NewSize : Length(), true);
		return Buf; 
	};

	T_Str ReleaseBuffer(size_t NewSize = 0) {
		_ASSERTE(NewSize < GetInfo()->LBuf);
		lengthDetermine(NewSize);
		return Buf; 
	};

	TSmartString& operator = (const TSmartString& Val) {
		if (this != &Val && this->Buf != Val.Buf) {
			if (Val.Buf) Val.GetInfo()->IncCou();
			Clear();
			Buf = Val.Buf;
		}
		return *this;
	};

	TSmartString& operator = (LC_Str Val) {
		size_t Len = strangeType<Type>::BaseTypeLen(Val, 0);
		Info* I = DeBuf(Len, true);
		strangeType<Type>::ToBaseType(InfoBuf<Type>(I->Data, I->Len), Val, 0);
		I->Data[I->Len] = 0;
		SetBuf(I);
		return *this;
	};

	TSmartString& operator = (TC_Str Val) {
		size_t Size = StringLength(Val);
		SetValue(Size, Val, Size);
		return *this;
	};

	//TSmartString& operator = (InfoBuf2<Type> Val) {
	//	Info *I = GetInfo();
	//	if (Val.Cou && (!I || I->LBuf < Val.Len || I->Cou > 1)) {
	//		Clear();
	//		Buf = const_cast<T_Str>(Val.Data);
	//		GetInfo()->IncCou();
	//	} else SetValue(Val.Len, Val.Data, Val.Len);
	//	return *this;
	//};

	TSmartString& operator = (InfoBuf2<Type> Val) {
		if (!Val.Len) return *this;
		Length(Val.Len + Length(), true);
		GetInfo()->Len += Val.SaveToBuf(Buf + Length());
		return *this;
	};

	TSmartString& operator += (LC_Str Val) {
		size_t Len = strangeType<Type>::BaseTypeLen(Val, 0);
		Length(Len + Length(), true);
		Info* I = GetInfo();
		InfoBuf<Type> IB(Buf + I->Len, Len);
		strangeType<Type>::ToBaseType(IB, Val, 0);
		I->Len += Len;
		return *this;
	};

	TSmartString& operator += (const InfoBuf<Type>& Val) {
		if (Val.Len) {
			size_t SZ = Length(), Rb = !Val.Cou && IncludeBuf(Val.Data);
			Info*I = DeBufCopy(Val.Len + SZ);
			I->FillBuf(Rb ? Buf + Rb - 1 : Val.Data, Val.Len, SZ);
			I->Data[I->Len] = 0;
			SetBuf(I);
		}
		return *this;
	};

	bool operator < (const InfoBuf<Type>& Val) {
		return this->Compare(Val) <0;
	};

	TSmartString& operator << (LC_Str Val) {
		return this->operator+=(Val);
	};	

	TSmartString& operator << (const InfoBuf<Type>& Val) {
		return this->operator+=(Val);
	};
	
	TSmartString& operator >> (LC_Str Val) {
		size_t Len = strangeType<Type>::BaseTypeLen(Val, 0);
		SetValue(Len + Length(), Buf, Length(), Len);
		strangeType<Type>::ToBaseType(InfoBuf<Type>(Buf, Len), Val, Len, 0);
		return *this;
	};

	TSmartString& operator >> (InfoBuf<Type>& Val) {
		if (Val.Len) {
			size_t SZ = Length();
			Info * I = DeBuf(Val.Len + SZ);
			I->FillBuf(Buf, SZ, Val.Len);
			I->FillBuf(Val.Data, Val.Len);
			I->Data[I->Len] = 0;
			SetBuf(I);
		}
		return *this;
	};

	inline friend TSmartString operator + (const TSmartString& Val1, const TSmartString& Val2) {
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
	};

	inline friend bool operator == (const TSmartString& Val1, const TSmartString& Val2) throw(){
		return Compare(val1, val2);
	};

	inline friend bool operator==(
		const TSmartString& str1,
		TC_Str psz2
		) throw() {
		return !str1.Compare(psz2);
	};

	//Сохраняет строковое представление числа
	TSmartString& SetInt(INT64 Val, byte sys = 10) {
		Bufer<Type> *I = DeBuf(sys == 10 ? 21 : _CVTBUFSIZE);
		strangeType<Type>::NumberToStr(Val, sys, InfoBuf<Type>(I->Data, I->LBuf-1));
		I->Len = StringLength(I->Data, I->Len);
		SetBuf(I);		
		return *this;
	}

	TSmartString& SetFloat(const double Val, byte dig = 10) {
		Bufer<Type> *I = DeBuf(dig + 31);
		strangeType<Type>::NumberToStr(Val, dig, InfoBuf<char>((char*)I->Data, I->LBuf));
		I->Len = StringLength((char*)I->Data);
		I->Data[I->Len] = 0;
		SetBuf(I);
		size_t Tm = I->Len;
		while (Tm) 	I->Data[Tm] = ((char*)Buf)[--Tm];		
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
	
	//Рассчитывает актуальную длину строки вмещаемую в буфер, при необходимости добавляет символ конца строки
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
		Info *I = GetInfo();
		if (I && I->Len == Size && (!SetResize || I->LBuf == Size + 1)) return Size;

		if (!I || I->Cou > 1 || I->LBuf < Size + 1 || SetResize) {
			I = Info::Create(Size + 1);
			if (Copy) {
				I->Len = min(Length(), Size);
				if (Buf && I->Len) I->FillBuf(Buf, I->Len, 0);
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

	//Сравнение строк: указатель на буфер, размер, различать заглавные
	int Compare(const TC_Str Val, size_t Sz = 0, bool NoCase = false)const {
		if (!Sz) Sz = StringLength(Val);
		return strangeType<Type>::CompareStr(Data(), Val, Length(), Sz, NoCase);
	}

	int Compare(const TSmartString Val, bool NoCase = false)const {
		return strangeType<Type>::CompareStr(Data(), Val.Data(), Length(), Val.Length(), NoCase);
		//из "const wchar_t *" в "const strangeType<WCHAR>::T_Str"
	}

	//Удаляет часть строки (size_t Начало, size_t Количество = 1)
	int Delete(size_t Beg, size_t Count = 1) {
		size_t SZ = Length();
		if (!Buf || Beg >= SZ || !Count) return SZ;
		if (Beg + Count > SZ) Count = SZ - Beg;
		return Concatenation(SZ - Count, Beg, Buf, SZ - Count - Beg, Buf + Beg + Count);
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

	inline void Reverse() {
		if (!Buf) return;
		Info *P = DeBufCopy(Length());
		strangeType<Type>::Reverse(P->Data);
		SetBuf(P);
		return *this;
	}

	// ищет подстроку в строке (что ищем, Начало = 0)
	int Find(const TSmartString& Val, size_t Beg = 0)const {
		return FindStr(Val.Buf, Beg);
	};

	int Find(TC_Str Val, size_t Beg = 0)const {
		return FindStr(Val, Beg);
	};

	int Find(LC_Str Val, size_t Beg = 0)const {
		TSmartString _tmp(Val);
		return FindStr(_tmp, Beg);
	};

	int Find(const InfoBuf<Type>& Val, size_t Beg = 0)const {
		return FindStr(Val.Data, Beg);
	};

	// ищет подстроку в строке начиная с конца(что ищем, символ начала поиска = 0)
	int FindOf(const TSmartString& Val, size_t Beg = 0)const {
		return FindStrOf(Val.Buf, Beg);
	};

	int FindOf(TC_Str Val, size_t Beg = 0)const {
		return FindStrOf(Val, Beg);
	};

	int FindOf(LC_Str Val, size_t Beg = 0)const {
		TSmartString _tmp(Val);
		return FindStrOf(_tmp, Beg);
	};

	int FindOf(const InfoBuf<Type>& Val, size_t Beg = 0)const {
		return FindStrOf(Val.Data, Beg);
	};

	//Ищет в строке позицию первого символа из указанной строки
	int FindOne(const T_Str Val)const {return strangeType<Type>::FindOne(Buf, Val);};
	//Ищет в строке позицию первого символа из указанной строки
	int FindOne(const InfoBuf<Type>& Val, size_t Beg = -1, size_t Len = 0)const {
		return strangeType<Type>::FindOne(Buf, Val.Data());};

	void RemoveChars(const InfoBuf<Type>& Val) {
		if (!Buf) return;
		RemChars(Buf, Val);
		Buf[Length()] = 0;
	};

	// Заменяет в строке все вхождения ValOld на ValNew, возвращает количество замен
	size_t Replace(const InfoBuf<Type>& ValOld, const InfoBuf<Type>& ValNew) {
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
		strangeType<Type>::Format(InfoBuf<Type>(P->Data, P->Len), StrFormat, Par);
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
		if (len < 1) {
			Length(0);
			return;
		}
		size_t SZ = Length();
		Info* P = DeBufCopy(len + SZ);
		if (P->Data == Buf && IncludeBuf(StrFormat)) {
			size_t SZ = (StrFormat - Buf);
			SZ = (SZ > P->Len ? P->Len : P->LBuf) - SZ;
			Buf = Info::Create(SZ + 1, StrFormat, SZ + 1)->Data;
			StrFormat = Buf + (StrFormat - P->Data);
		}
		strangeType<Type>::Format(InfoBuf<Type>(P->Data + SZ, P->Len-SZ), StrFormat, Par);
		P->Data[P->Len] = 0;
		SetBuf(P);
	};

	//Вставляет подстроку в строку(позиция вставки, подстрока, количество удаляемых символов)
	size_t Insert(size_t Pos, const TSmartString& Val, size_t Count = 0) { return InsertPrv(Pos, InfoBuf<Type>(Val.Data(), Val.Length()), Count); }
	size_t Insert(size_t Pos, TC_Str Val, size_t Size = 0, size_t Count = 0) { return InsertPrv(Pos, InfoBuf<Type>(Val, Size), Count); }

	const TC_Str Data()const { return Buf; }
	operator TC_Str() const throw() { return Buf; }
};

////////////////////////////////////// VRTSmartStr /// END /////////////////////////////////

template< typename BaseType = char>
class strangeType {
public:
	typedef wchar_t		L_Char;
	typedef LPWSTR		L_Str;
	typedef char		T_Char;
	typedef LPSTR		T_Str;
	typedef LPCWSTR		LC_Str;
	typedef LPCSTR		TC_Str;
	
	static void NumberToStr(const INT64 val, int sys, InfoBuf<T_Char> &Buf) {
		//if (sys < 2) sys = 2;
		//if (sys >36) sys = 36;
		_ASSERTE(sys >= 2 && sys <= 36);
		_i64toa_s(val, const_cast<T_Str>(Buf.Data), Buf.Len + 1, sys);
	}

	static void NumberToStr(const INT64 val, int sys, T_Str Buf, size_t Size) {
		_ASSERTE(sys >= 2 && sys <= 36);
		_i64toa_s(val, Buf, Size + 1, sys);
	}
	
	static void NumberToStr(const double val, int sys, InfoBuf<char> &Buf) {
		//if (sys < 0) sys = 0;
		_ASSERTE(sys >= 0);
		_gcvt_s(const_cast<char>(Buf.Data), Buf.Len + 1, val, sys);
	}

	static size_t BaseTypeLen(LC_Str pszSrc, size_t Count, UINT Code = CP_ACP) {
		if (pszSrc == nullptr) return 0;
		int sz = MultiByteToWideChar(Code, 0, pszSrc, Count ? Count : -1, nullptr, 0);
		return  Count ? sz : sz - 1;
	}
	static void ToBaseType(InfoBuf<T_Char>& Buf, LC_Str pszSrc, size_t Count, UINT Code = CP_ACP) {
		const_cast<T_Char*>(Buf.Data)[Buf.Len] = 0;
		MultiByteToWideChar(Code, 0, pszSrc, Count ? Count : -1, const_cast<T_Char*>(Buf.Data), Buf.Len + 1);
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
	static void Format(InfoBuf<T_Char>& Buf, TC_Str Str, va_list Param) {
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

	static void NumberToStr(const INT64 val, int sys, InfoBuf<T_Char> &Buf) {
		//if (sys < 2) sys = 2;
		//if (sys >36) sys = 36;
		_ASSERTE(sys >= 2 && sys <= 36);
		_i64tow_s(val, const_cast<T_Str>(Buf.Data), Buf.Len + 1, sys);
	}

	static void NumberToStr(const INT64 val, int sys, T_Str Buf, size_t Size) {
		_ASSERTE(sys >= 2 && sys <= 36);
		_i64tow_s(val, Buf, Size + 1, sys);
	}


	static void NumberToStr(const double val, int sys, InfoBuf<char> &Buf) {
		//if (sys < 0) sys = 0;
		_ASSERTE(sys >= 0);
		_gcvt_s(const_cast<char*>(Buf.Data), Buf.Len + 1, val, sys);
	}
	static size_t BaseTypeLen(LC_Str pszSrc, size_t Count, UINT Code = CP_ACP) {
		if (pszSrc == nullptr) return 0;
		int sz = MultiByteToWideChar(Code, 0, pszSrc, Count ? Count : -1, nullptr, 0);
		return  Count ? sz : sz - 1;
	}
	static void ToBaseType(InfoBuf<T_Char>& Buf, LC_Str pszSrc, size_t Count, UINT Code = CP_ACP) {
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
	static int FindOne(TC_Str Val1, TC_Str Val2) {
		TC_Str P = FindOneP(Val1, Val2);
		return  !P ? -1 : (P - Val1);
	}
	static int FormatLen(TC_Str Str, va_list Param) {
		return _vscwprintf(Str, Param);
	};
	static void Format(InfoBuf<T_Char>& Buf, TC_Str Str, va_list Param) {
		vswprintf_s((T_Str)Buf.Data, Buf.Len + 1, Str, Param);
	};
	static void LowerStr(T_Str Val, size_t Size) { _wcslwr_s(Val, Size); }
	static void UpperStr(T_Str Val, size_t Size) { _wcsupr_s(Val, Size); }
	static void Reverse(T_Str Val) { _wcsrev(Val); }
};


#define SmartStrA	VRTSmartStr<char>
#define SmartStrW	VRTSmartStr<wchar_t>

#ifdef UNICODE
#define SmartStr	SmartStrW
#else
#define SmartStr	SmartStrA
#endif

#endif