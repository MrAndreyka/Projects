#ifndef SmatrClasses
#define SmatrClasses

#include <windows.h>
#include "MyExcept.h"
#include "WCHAR.h"
#include "myInit.h"

#ifdef _DEBUG
//#define _SmartStr_mem
#endif

#define __max(a,b)  (((a) > (b)) ? (a) : (b))

template <class Type>
class VRTSmartStr;

struct InfoData {
	long Cou;
	size_t Len;
	void Set(size_t Len_, short Cou_) {
		Len = Len_; Cou = Cou_;
	}

	// атомарныеные операции счетчика
	size_t IncCou() { return InterlockedIncrement(&Cou); };
	size_t DecCou() { return InterlockedDecrement(&Cou); };

	auto& length() { return Len; }
};

struct InfoDataB :public InfoData {
	size_t LenBuf;
	void Set(size_t Len_, size_t LBuf_, size_t Cou_) {
		InfoData::Set(Len_, Cou_);
		LenBuf = LBuf_;
	}
	auto& length() { return LenBuf; }
};

template <class Type>
struct Bufer : InfoDataB {
	using CurrentType = typename Bufer<Type>;
	Type Data[1];

	//Создает новый буфер и если нужно копирует в него данные
	// Size - РазмерБуфера; Len - Длинна; dest - Источник данных; destLen - Длинна копируемых данных
	static Bufer* Create(size_t Size, const Type* dest = nullptr, size_t destLen = 0) {
		_ASSERTE(Size != 0);
		CurrentType* P = (CurrentType*) new char[Size * sizeof(Type) + sizeof(InfoDataB)];
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

	void Destroy() { delete[]this; };
	void Relese() { if (!DecCou())	delete[]this; };

	Type* end() { return Data + Len; }
	
	//Копирует в готовый буфер данные
	// BUF - Данные; len - Размер данных; Beg - Начало вставки
	// никакой проверки данных нет
	void FillBuf(const Type* BUF, size_t len = 0, size_t Beg = 0) {
		_ASSERT(Beg + len > Beg);
		memcpy(Data + Beg, BUF, len * sizeof(Type));
	}
private:
	Bufer() {};
};

/*/struct Cell;
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
};*/


///////////////////////////////////////////////////////

size_t StringLength(_In_opt_z_ const LPCSTR psz, size_t Size = 0) throw() {
	if (psz == nullptr) return 0;
	return Size ? strnlen(psz, Size) : strlen(psz);
}

size_t StringLength(_In_opt_z_ const LPCWSTR psz, size_t Size = 0) throw() {
	if (psz == nullptr) return 0;
	return Size ? wcsnlen(psz, Size) : wcslen(psz);
}

size_t NumberToStr(const INT64 val, byte sys, LPSTR Buf, size_t LenBuf) {
	_ASSERTE(sys >= 2 && sys <= 36);
	_i64toa_s(val, Buf, LenBuf, sys);
	return StringLength(Buf);
}

size_t NumberToStr(const INT64 val, int sys, LPWSTR Buf, size_t lenBuf) {
	_ASSERTE(sys >= 2 && sys <= 36);
	_i64tow_s(val, Buf, lenBuf, sys);
	return StringLength(Buf);
}

size_t NumberToStr(const double val, byte sys, LPSTR Buf, size_t LenBuf) {
	_ASSERTE(sys >= 0);
	_gcvt_s(Buf, LenBuf, val, sys);
	return StringLength(Buf);
}

size_t NumberToStr(const double val, int sys, LPWSTR Buf, size_t lenBuf) {
	_ASSERTE(sys >= 0);
	_gcvt_s((char*)Buf, lenBuf, val, sys);
	size_t Tm = StringLength((char*)Buf), a = Tm + 1;
	while (a) Buf[a] = ((char*)Buf)[--a];
	return Tm;
}

template< typename BaseType>
class TransformationType;

template <class Type>
class VRTSmartStr;

template< typename Type>
class BufPointer{
	using tt = TransformationType<Type>;

	using Base_Char = typename tt::Base_Char;
	using Base_Str = typename tt::Base_Str;
	using Base_CStr = typename tt::Base_CStr;

	using F_Char = typename tt::F_Char;
	using F_Str = typename tt::F_Str;
	using F_CStr = typename tt::F_CStr;

	using MyBufer = typename Bufer<Type>;
private:
	char Buf[9];
	void SavePointer(void* bf) { *(INT64*)Buf = (INT64)bf; }
	template< typename T>
	void SaveData(const T val) {
		_ASSERT(sizeof(val) <= 8);
		new (Buf) T(val);
	}
	//	*(T*)Buf = val;	}
public:
	size_t Len;
	const byte type;

	bool ActualSize()const { return type < 2; }

	size_t SaveTo(Base_Str _Buf, size_t bufLength, bool exact = false)const {

		switch (type){
		case 0:case 10:
			CopyMemory(_Buf, *(Base_Str*)Buf, bufLength*sizeof(Type));
			return Len;
			break;
		case 1:
			return tt::ToBaseType(_Buf, bufLength, *(F_Str*)Buf, Len, Buf[8]);
			break;
		case 2:
			return NumberToStr(*(INT64*)Buf, Buf[8], _Buf, bufLength);
			break;
		case 3:
			return NumberToStr(*(double*)Buf, Buf[8], _Buf, bufLength);
			break;
		default:
			throw "Error type in BufPointer";
			break;
		}
		return 0;
	}

	~BufPointer() {
		if (type == 10)
			((Bufer<Base_Char>*)SHIFT(*(Base_Str*)Buf, -1, InfoDataB))->Relese();
		else if (type == 11)
			delete[]Buf;
	}

	BufPointer(BufPointer&& Val) : Buf(Val.Buf), Len(Val.Len), type(Val.type) {
		Val.Len = 0; }

	BufPointer(const VRTSmartStr<Type>& Val):Len(Val.Length()), type(10) {
		auto *I = Val.GetInfo();
		SavePointer((I && I->IncCou()>1)? I->Data: nullptr);
	}

	BufPointer(Base_CStr Val, const size_t Len = 0):type(0){
		this->Len = Len ? Len : StringLength(Val);
		SavePointer((void*)Val);
	}

	BufPointer(F_CStr Val, size_t Len = 0) : type(1) {
		this->Len = Len ? Len : StringLength(Val);
		SavePointer((void*)Val);
	}

	BufPointer(const INT64 Val, byte sys = 10) : type(2) {
		Len = sys == 10 ? 21 : _CVTBUFSIZE;
		SaveData(Val);
		Buf[8] = sys;
	}

	explicit BufPointer(const double Val, byte dig = 10) :type(3) {
		Len = dig + 31;
		SaveData(Val);
		Buf[8] = dig;
	}

	BufPointer& operator =(BufPointer&& Val) {
		memmove(this, &Val, sizeof(*this));
		Val.Len = 0;
		return *this;
	}
};

template <class Type>
class VRTSmartStr {
	friend BufPointer<Type>;
public:
	using tt = TransformationType<Type>;
	using Base_Char = typename tt::Base_Char;
	using Base_Str = typename tt::Base_Str;
	using Base_CStr = typename tt::Base_CStr;
	using F_Char = typename tt::F_Char;
	using F_Str = typename tt::F_Str;
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

	static size_t NB_Size(size_t Size, Info* Old) {
		Size++;
		if (!Old || Old->LenBuf > Size)
			return Size;
		size_t NewSize = Old->LenBuf + (Old->LenBuf * sizeof(Base_Char) > 2097152 ? 104857 : Old->LenBuf / 2); //2097152 - 2мб 104857 - 100кб
		if (NewSize < Size) NewSize = Size;
		return NewSize;
	}

	// Пересоздает буфер нового размера
	Info* NewBuf(const size_t requisiteSize, bool copy = false) {
		_ASSERT(requisiteSize + 1 > 1);
		Info* I = GetInfo();

		if (!I || I->Cou > 1 || I->LenBuf < requisiteSize + 1) {
			size_t NewSize = NB_Size(requisiteSize, I);
			//if (I && FirstClear) Clear();
			I = (copy && I) ? Info::Create(NewSize, I->Data, I->Len) : Info::Create(NewSize);
		}
		I->Data[I->Len] = 0;
		return I;
	}

	// Возвращает позицию первого вхождения Val начиная с позиции Beg
	bool FindStr(Base_CStr Val, size_t& Pos)const {
		if (!Buf || !Val || Pos > Length()) return false;
		Base_CStr P = tt::Find(Buf + Pos, Val);
		Pos = P == nullptr ? 0 : P - Buf;
		return P != nullptr;
	};

	// Возвращает позицию последнено вхождения Val не далее позиции Beg (поиск ведется с конца строки)
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
		_ASSERTE(Count + Pos >= Pos); // переполнение типа size_t
		if (Count + Pos > LEN) Count = LEN - Pos;
		_ASSERTE(LEN + Size > LEN && LEN + Size > Size); // переполнение типа size_t
		Info* P = NewBuf(Size + LEN - Count);
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
		Base_CStr Finds = tt::FindOneP(Home, Val.Data);

		if (!Finds) return Length() - (Home - Buf);

		size_t P = RemChars(Finds + 1, Val);
		Info* I = GetInfo();
		I->FillBuf(Finds + 1, P, Finds - Buf);
		I->Len--;
		return Finds - Home + P;
	};

	//Возвращает с какой позиции Val входит в текущий буфер
	inline size_t IncludeBuf(Base_CStr Val)const {
		return (!Buf || !Val || Val<Buf || Val > Buf + GetInfo()->LenBuf) ? 0 : Val - Buf + 1;
	}

	// Устанавливает размер буфера не менее Size, 
	//копирует при указании данные с Val размером Len или до конца строки начиная с Beg
	void SetValue(size_t Size, Base_CStr Val, size_t Len, const size_t Beg = 0) {
		Info* I = NewBuf(Size, !IncludeBuf(Val));
		if (Val && Len) {
			if (Len + Beg > Size) Len = Size - Beg;
			I->FillBuf(Val, Len, Beg);
			I->Len = Len + Beg;
		}
		else I->Len = 0;
		I->Data[I->Len] = 0;
		SetBuf(I);
	}

public:
	//operator LPARAM() { return (LPARAM)Data(); };
	~VRTSmartStr() { Clear(); };

	VRTSmartStr() {};

	VRTSmartStr(const TSmartString& Val) {this->operator=(Val);};

	VRTSmartStr(TSmartString&& Val) {this->operator=(Val);};

	VRTSmartStr(Base_CStr Val, size_t Size = 0) {
		if (!Size) Size = StringLength(Val);
		SetValue(Size, Val, Size);
	};

	VRTSmartStr(const F_Str Val, size_t Size = 0) {
		size_t Len = tt::BaseTypeLen(Val, Size);
		Info* I = DeBuf(Len);
		tt::ToBaseType(Info(I->Data, I->Len), Val, Size);
		I->Data[I->Len] = 0;
		SetBuf(I);
	};


	TSmartString& operator = (TSmartString&& Val) {
		if (this->Buf != Val.Buf)
			return *this;
		SetBuf(Val.GetInfo());
		Val.Buf = nullptr;
		return *this;
	};

	TSmartString& operator = (Base_CStr Val) {
		size_t Size = StringLength(Val);
		SetValue(Size, Val, Size);
		return *this;
	};

	TSmartString& operator = (const F_Str Val) {
		size_t Len = tt::BaseTypeLen(Val, 0);
		Info* I = DeBuf(Len, true);
		tt::ToBaseType(I->Data, I->Len, Val, 0);
		I->Data[I->Len] = 0;
		SetBuf(I);
		return *this;
	};

	VRTSmartStr(const INT64 Val, byte sys = 10) {
		Info* I = NewBuf(sys == 10 ? 21 : _CVTBUFSIZE);
		I->Len = NumberToStr(Val, sys, I->Data, I->LenBuf - 1);
		I->end()[0] = 0;
		SetBuf(I);
	}
	explicit VRTSmartStr(const int Val, byte sys = 10) : VRTSmartStr((INT64)Val, sys) {}

	explicit VRTSmartStr(const double Val, byte dig = 25) {
		Info* I = NewBuf(dig + 31);
		I->Len = NumberToStr(Val, dig, I->Data, I->LenBuf - 1);
		*I->end() = 0;
		Buf = I->Data;
	}

	TSmartString& operator = (const TSmartString& Val) {
		if (this->Buf != Val.Buf)
			if (Val.Length() && Val.GetInfo()->IncCou() > 1) {
				SetBuf(Val.GetInfo());
			}
			else Clear();
		return *this;
	};

	/*TSmartString& operator >> (const TSmartString& Val) {
		Info* I2 = Val.GetInfo();
		if (!I2 || I2->IncCou() < 2) return *this;
		Info* I = NewBuf(Length() + I2->Len);
		if (I->Len = Length())
			I->FillBuf(Buf, I->Len, I2->Len);
		I->FillBuf(Val.Buf, I2->Len);
		I->Len += I2->Len;
		I->Data[I->Len] = 0;
		I2->Relese();
		SetBuf(I);
		return *this;
	};*/

	TSmartString& operator >> (const BufPointer<Type>& Val) {
		if (!Val.Len) return *this;
		Info* I = NewBuf(Length() + Val.Len);
		
		if (!Val.ActualSize() && IncludeBuf(I->Data)) 
		{
			auto b = new Base_Char[Val.Len];
			Val.SaveTo(b, Val.Len);
			auto sz = StringLength(b);

			if (I->Len = Length())
				I->FillBuf(Buf, I->Len, sz);
			I->FillBuf(b, sz);
			I->Len += sz;
			delete[]b;
		}
		else
		{
			if (I->Len = Length())
				I->FillBuf(Buf, I->Len, Val.Len);
			I->Len += Val.SaveTo(I->Data, Val.Len);
		}
		I->Data[I->Len] = 0;
		SetBuf(I);
		return *this;
	};

	/*TSmartString& operator << (const TSmartString Val) {
		if (!Val.Length()) return *this;
		Info* I2 = Val.GetInfo();
		Info* I = NewBuf(Length() + I2->Len, true);
		I->FillBuf(I2->Data, I2->Len, I->Len);
		I->Len += I2->Len;
		I->Data[I->Len] = 0;
		SetBuf(I);
		return *this;
	};*/

	TSmartString& operator << (const BufPointer<Type>& Val) {
		if (!Val.Len) return *this;
		Info* I = NewBuf(Length() + Val.Len, true);
		I->Len += Val.SaveTo(I->end(), I->LenBuf - I->Len - 1);
		I->Data[I->Len] = 0;
		SetBuf(I);
		return *this;
	};
	/*
	TSmartString& operator << (const int Val) { return this->operator<<(BufPointer<Type>(Val));}

	TSmartString& operator << (const double Val) {return this->operator<<(BufPointer<Type>(Val));}

	bool operator < (const BufPointer<Type>& Val) {
		return this->Compare(Val) < 0;
	};

	bool operator > (const BufPointer<Type>& Val) {
		return this->Compare(Val) > 0;
	};

	inline friend bool operator == (const TSmartString& Val1, const BufPointer<Type>& Val2) throw() {
		return Val1.Compare(Val2) == 0;
	};*/

	TSmartString& operator += (const TSmartString& Val) {
		if (!Val.Length()) return *this;

		Info* I = GetInfo();

		if (!I || I->Len == 0) {
			I = Val.GetInfo();
			I->IncCou();
		}
		else {
			I = NewBuf(I.Len + Val.Length(), true);
			I->FillBuf(I->Data + I.Len, Val.Length());
		}
		SetBuf(I);
		return *this;
	};


	/*
		TSmartString& operator += (F_Str Val) {
			size_t Len = tt::BaseTypeLen(Val, 0);
			Length(Len + Length(), true);
			Info* I = GetInfo();
			tt::ToBaseType(BufPointer<Type> (Buf + I->Len, Len), Val, 0);
			I->Len += Len;
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
	*/
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

	friend TSmartString operator + (const TSmartString& Val1, const Base_Str Val2) {
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

	bool operator < (const TSmartString& Val) {
		return this->Compare(Val) < 0;
	};

	inline friend bool operator == (const TSmartString& Val1, const TSmartString& Val2) throw() {
		return Val1.Compare(Val2) == 0;
	};

	friend bool operator==(
		const TSmartString& str1,
		Base_CStr psz2
		) throw() {
		return !str1.Compare(psz2);
	};

	inline friend bool operator != (const TSmartString& Val1, const TSmartString& Val2) throw() {
		return Val1.Compare(Val2) != 0;
	};

	friend bool operator!=(
		const TSmartString& str1,
		Base_CStr psz2
		) throw() {
		return str1.Compare(psz2);
	};

	/*/Сохраняет строковое представление числа
	TSmartString& SetInt(INT64 Val, byte sys = 10) {
		Bufer<Type> *I = NewBuf(sys == 10 ? 21 : _CVTBUFSIZE);
		tt::NumberToStr(Val, sys, BufPointer<Type>(I->Data, I->LBuf-1));
		I->Len = StringLength(I->Data, I->Len);
		SetBuf(I);
		return *this;
	}

	TSmartString& SetFloat(const double Val, byte dig = 10) {
		Bufer<Type> *I = DeBuf(dig + 31);
		I->Len = tt::NumberToStr(Val, dig, BufPointer<Type>(I->Data, I->LBuf-1));
		I->Data[I->Len] = 0;
		SetBuf(I);
		return *this;
	}*/

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
		_ASSERTE(NewSize < GetInfo()->LenBuf);
		lengthDetermine(NewSize);
		return Buf;
	};

	//Сложение в строку нескольких источников, FullSize = общий размер, параметры: размер и указатель на начало строки извлекается пока не дойдет до FullSize
	//	возвращает общую длинну строки
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

	inline bool isEmpty() { return !Length(); }

	//Возвращает размер буфера памяти учитывая символ конца строки
	inline size_t BufLength()const { return Buf ? GetInfo()->LBuf : 0; };

	//Возвращает длину строки, без символа конца строки
	inline size_t Length()const { return Buf ? GetInfo()->Len : 0; };

	//Рассчитывает актуальную длину строки не более maxCou символов вмещаемую в буфер, при необходимости добавляет символ конца строки
	size_t lengthDetermine(size_t maxCou = 0)const {
		if (!Buf) return 0;
		maxCou = maxCou ? min(maxCou, GetInfo()->LenBuf - 1) : GetInfo()->LenBuf;
		size_t SZ = StringLength(Buf, maxCou);
		if (GetInfo()->LenBuf < SZ + 1) {
			SZ = GetInfo()->LenBuf - 1;
			Buf[SZ] = 0;
		}
		return GetInfo()->Len = SZ;
	}

	//Устанавливает длинну строки 
	//(Size - Размер (не учитывая символа конца строки), Copy - Копировать ли содержимое, Resizible - Уменьшать ли размер буфера)
	size_t Length(size_t Size, bool Copy = false, bool Resizible = false) {
		_ASSERTE(Size != -1);
		Info* I = GetInfo();

		if (!Size) {
			if (Resizible) Clear();
			elseif(I) I->Data[0] = 0;
			return 0;
		}

		if (!I || I->Cou > 1 || I->LenBuf < Size + 1 || Resizible) {
			I = NewBuf(Size + 1, Copy);
			//!!!!!!!!!!!
			/*if (Copy) {
				I->Len = min(Length(), Size);
				if (Buf != I->Data && I->Len) I->FillBuf(Buf, I->Len, 0);
			} else I->Len = 0;*/
		}
		else if (Size < I->Len) I->Len = Size;

		I->Data[I->Len] = 0;
		SetBuf(I);
		return I->Len;
	}

	//Очищает строку, уничтожает буфер
	void Clear() {
		if (!Buf) return;
		Info* T = GetInfo();
		if (!T->DecCou())
			T->Destroy();
		Buf = nullptr;
	};

	//Сравнивается со строкой val размера Sz. Возвращает >0 если Val меньше 
	//	NoCase - Различать ли заглавные
	int Compare(const TSmartString Val, bool NoCase = false)const {
		return tt::CompareStr(Data(), Val.Data(), Length(), Val.Length(), NoCase);
		//из "const wchar_t *" в "const strangeType<WCHAR>::Base_Str"
	}
	int Compare(const Base_CStr Val, size_t Sz = 0, bool NoCase = false)const {
		if (!Sz) Sz = StringLength(Val);
		return tt::CompareStr(Data(), Val, Length(), Sz, NoCase);
	}

	//Удаляет часть строки начиная c Beg, длиной Count
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
		Info* P = DeBufCopy(Length());
		tt::LowerStr(P->Data, P->Len);
		SetBuf(P);
		return *this;
	}

	// переводит строку в нижний регистр
	TSmartString& MakeUpper() {
		if (!Buf) return *this;
		Info* P = DeBufCopy(Length());
		tt::UpperStr(P->Data, P->Len);
		SetBuf(P);
		return *this;
	}

	void Reverse() {
		if (!Buf) return;
		Info* P = DeBufCopy(Length());
		tt::Reverse(P->Data);
		SetBuf(P);
		return *this;
	}

	// ищет значение Val в строке начиная с символа Pos. Там же найденный символ
	//bool Find(const BufPointer<Type>& Val, size_t& Pos)const {
	//return FindStr(Val.Data, Pos);};	

	bool Find(const TSmartString& Val, size_t& Pos)const {
		return FindStr(Val.Buf, Pos);
	};

	INT64 Find(Base_CStr Val, size_t Beg = 0)const {
		return FindStr(Val, Beg);
	};

	INT64 Find(F_Str Val, size_t Beg = 0)const {
		TSmartString _tmp(Val);
		return FindStr(_tmp, Beg);
	};

	//INT64 Find(const BufPointer<Type>& Val, size_t Beg = 0)const {
	//	return FindStr(Val.Data, Beg);};

	// ищет подстроку в строке начиная с конца(что ищем, символ начала поиска = 0)
	bool FindOf(const TSmartString& Val, size_t& Pos)const {
		return FindStrOf(Val.Buf, Pos);
	};
	//bool FindOf(const BufPointer<Type>& Val, size_t& Pos)const {
	//	return FindStrOf(Val.Data, Pos);};

	auto FindOf(Base_CStr Val, size_t Beg = 0)const {
		return FindStrOf(Val, Beg);
	};

	auto FindOf(F_Str Val, size_t Beg = 0)const {
		TSmartString _tmp(Val);
		return FindStrOf(_tmp, Beg);
	};

	//Ищет в строке позицию первого символа из указанной строки
	auto FindOne(const Base_Str Val)const { return tt::FindOne(Buf, Val); };


	//Ищет в строке позицию первого символа из указанной строки
	/*auto FindOne(const BufPointer<Type>& Val)const {
		return tt::FindOne(Buf, Val.Data);};

	void RemoveChars(const BufPointer<Type>& Val) {
		if (!Buf) return;
		RemChars(Buf, Val);
		Buf[Length()] = 0;
	};

	// Заменяет в строке все вхождения ValOld на ValNew, возвращает количество замен
	size_t Replace(const BufPointer<Type>& ValOld, const BufPointer<Type>& ValNew) {
		if (!Buf || !ValOld.Len) return 0;
		size_t Cou(0), T = Length();
		Base_Str BegP = Buf, EndP = Buf + T;

		{while (BegP < EndP && (BegP = tt::Find(BegP, ValOld.Data)))
			Cou++, BegP += ValOld.Len; }
		if (!Cou) return 0;

		_ASSERTE(!IncludeBuf(ValOld.Data) && !IncludeBuf(ValNew.Data));

		Info *I;
		size_t NewLen = T + int(Cou * (ValNew.Len - ValOld.Len));
		I = DeBufCopy(NewLen);

		BegP = Buf, EndP = Buf + T;
		while (BegP < EndP && (BegP = tt::Find(BegP, ValOld.Data))) {
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
	}*/

	// Возвращает строку до первого символа из указанной строки
	TSmartString SpanExcluding(Base_CStr Val) {
		if (!Buf || !Val) return *this;
		int r = tt::FindOne(Buf, Val);
		if (r < 0) return TSmartString();
		return TSmartString(Buf, r);
	}

	// Возвращает строку до первого символа не входящего в указанную строку
	TSmartString SpanIncluding(Base_CStr Val) {
		if (!Buf || !Val) return TSmartString();
		size_t SZ = tt::FindNotOne(Buf, Val);
		return SZ ? this->Left(SZ) : TSmartString();
	}

	//Формирует строку по формату
	void Format(const F_Str StrFormat, ...) {
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

		size_t len = tt::FormatLen(StrFormat, Par);
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
		tt::Format(P->Data, P->LenBuf, StrFormat, Par);
		P->Data[P->Len] = 0;
		SetBuf(P);
	};

	//Добавляет строку по формату
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
		size_t len = tt::FormatLen(StrFormat, Par);
		if (!len) {
			Length(0);
			return;
		}
		size_t SZ = Length();
		Info* P = NewBuf(len + SZ, true);
		if (P->Data == Buf && IncludeBuf(StrFormat)) {
			len = StrFormat - Buf;
			Buf = Info::Create(SZ - len + 1, StrFormat, SZ - len + 1)->Data;
			StrFormat = Buf;
		}
		tt::Format(P->Data + SZ, P->LenBuf - SZ, StrFormat, Par);
		P->Data[P->Len += len] = 0;
		SetBuf(P);
	};

	//Вставляет подстроку Val начиная с позиции Pos удаляя  Count символов)
	//inline size_t Insert(const BufPointer<Type>& Val, size_t Pos,  size_t Count = 0) { return InsertPrv(Pos, Val.Data, Val.Len, Count); }
	//Вставляет подстроку Val длинной Val_Size, начиная с позиции Pos удаляя  Count символов)
	inline size_t Insert(Base_CStr Val, size_t Pos, size_t Count, size_t Val_Size = StringLength(Val)) { return InsertPrv(Pos, Val, Val_Size, Count); }

	const Base_CStr Data() const { return Buf; }
	operator Base_CStr() const throw() { return Buf; }

	Base_CStr operator*() const { return Buf; }
};

////////////////////////////////////// VRTSmartStr /// END /////////////////////////////////

template< typename BaseType, typename FriendType>
class descriptionType {
public:
	using Base_Char = BaseType;
	using Base_Str = BaseType*;
	using Base_CStr = const BaseType*;

	using F_Char = FriendType;
	using F_Str = FriendType*;
	using F_CStr = const FriendType*;
};

template< typename BaseType = char>
class TransformationType {
public:
	using dt = descriptionType<BaseType, wchar_t>;

	using Base_Char = typename dt::Base_Char;
	using Base_Str = typename dt::Base_Str;
	using Base_CStr = typename dt::Base_CStr;

	using F_Char = typename dt::F_Char;
	using F_Str = typename dt::F_Str;
	using F_CStr = typename dt::F_CStr;

	/*static void NumberToStr(const INT64 val, byte sys, Base_Str Buf, size_t LenBuf) {
		_ASSERTE(sys >= 2 && sys <= 36);
		_i64toa_s(val, Buf, LenBuf, sys);
	}

	static size_t NumberToStr(const double val, byte sys, Base_Str Buf, size_t LenBuf) {
		_ASSERTE(sys >= 0);
		_gcvt_s(Buf, LenBuf, val, sys);
		return StringLength(Buf.Data);
	}*/

	static size_t BaseTypeLen(F_CStr pszSrc, size_t Count, UINT Code = CP_ACP) {
		if (pszSrc == nullptr) return 0;
		int sz = WideCharToMultiByte(Code, 0, pszSrc, Count ? Count : -1, nullptr, 0, NULL, nullptr);
		return  Count != 0 ? sz : sz - 1;
	}
	static int ToBaseType(Base_Str Buf, size_t LenBuf, F_CStr pszSrc, size_t Count, UINT Code = CP_ACP) {
		int sz = WideCharToMultiByte(Code, 0, pszSrc, Count ? Count : -1, Buf, LenBuf, NULL, nullptr);
		if (Count)Buf[sz] = 0; else sz--;
		return  sz;
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
	static int FormatLen(Base_CStr Str, va_list Param) { return _vscprintf(Str, Param); };

	static void Format(Base_Str Buf, size_t LenBuf, Base_CStr Str, va_list Param) { vsprintf_s(Buf, LenBuf, Str, Param); };

	static void LowerStr(Base_Str Val, size_t Size) { _strlwr_s(Val, Size); }
	static void UpperStr(Base_Str Val, size_t Size) { _strupr_s(Val, Size); }
	static void Reverse(Base_Str Val) { _strrev(Val); }
};

template<>
class TransformationType<WCHAR> {
public:
	using cl = descriptionType<WCHAR, char>;

	using Base_Char = typename cl::Base_Char;
	using Base_Str = typename cl::Base_Str;
	using Base_CStr = typename cl::Base_CStr;

	using F_Char = typename cl::F_Char;
	using F_Str = typename cl::F_Str;
	using F_CStr = typename cl::F_CStr;

	static size_t BaseTypeLen(F_CStr pszSrc, UINT Count, UINT Code = CP_ACP) {
		if (pszSrc == nullptr) return 0;
		int sz = MultiByteToWideChar(Code, 0, pszSrc, Count ? Count : -1, nullptr, 0);
		return  Count ? sz : sz - 1;
	}
	static int ToBaseType(Base_Str Buf, size_t lenBuf, F_CStr pszSrc, int Count, UINT Code = CP_ACP) {
		int sz = MultiByteToWideChar(Code, 0, pszSrc, Count ? Count : -1, Buf, lenBuf);
		if (Count)Buf[sz] = 0; else sz--;
		return sz;
	}
	static int CompareStr(const Base_CStr Val1, cl::Base_CStr Val2, size_t Len1, size_t Len2, bool noCase) {
		return CompareStringW(LOCALE_CUSTOM_DEFAULT, noCase ? NORM_IGNORECASE : 0, Val1, Len1, Val2, Len2) - 2;
	}
	static Base_Str Find(Base_Str Val1, Base_CStr Val2) {
		return wcsstr(Val1, Val2);
	}
	static Base_CStr FindOneP(Base_CStr Val1, Base_CStr Val2) {
		return wcspbrk(Val1, Val2);
	}
	static size_t FindNotOne(Base_CStr Val1, Base_CStr Val2) { return  wcsspn(Val1, Val2); }
	static INT64 FindOne(Base_CStr Val1, Base_CStr Val2) {
		Base_CStr P = FindOneP(Val1, Val2);
		return  !P ? -1 : (P - Val1);
	}
	static int FormatLen(Base_CStr Str, va_list Param) { return _vscwprintf(Str, Param); };
	static void Format(Base_Str Buf, size_t lenBuf, Base_CStr Str, va_list Param) { vswprintf_s(Buf, lenBuf, Str, Param); };

	static void LowerStr(Base_Str Val, size_t Size) { _wcslwr_s(Val, Size); }
	static void UpperStr(Base_Str Val, size_t Size) { _wcsupr_s(Val, Size); }
	static void Reverse(Base_Str Val) { _wcsrev(Val); }
};

typedef VRTSmartStr<char>		SmartStrA;
typedef VRTSmartStr<wchar_t>	SmartStrW;

#ifdef UNICODE
typedef SmartStrW SmartStr;
#else
typedef SmartStrA SmartStr;
#endif

#endif