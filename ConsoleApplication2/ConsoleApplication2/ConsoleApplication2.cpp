// ConsoleApplication2.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.

#ifdef _DEBUG//
#include <crtdbg.h>
#define _CRTDBG_MAP_ALLOC 
/*#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW*/
#endif

#include <cassert>
#include <string>
#include <iostream>
#include "..\..\Iincludes\class.h"
#include "..\..\Iincludes\DataClass.cpp"
//#include "..\..\Iincludes\wincom.h"
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <variant>
#include <functional>

#define _USE_MATH_DEFINES // for C++
#include <math.h>
#include <windef.h>

using namespace std;

template<class T>
void ShowBites(const void* ptr, size_t countBytes, const char* str = nullptr) {
	auto count = countBytes / sizeof(T);
	const T* c = static_cast<const T*>(ptr);
	if (str)cout << str;
	cout << ptr  << ":	"<< std::hex;
	auto size = count - 1;
	while (count-- > 0)
		cout << c[size-count] << " ";
	cout << std::dec << endl;
}

template<>
void ShowBites<char>(const void* ptr, size_t count, const char* str) {
	const unsigned char* c = static_cast<const unsigned char*>(ptr);
	if (str)cout << str;
	cout << ptr << ":	" << std::hex;
	auto size = count - 1;
	while (count-- > 0)
		cout << static_cast<int>(c[size - count]) << " ";
	cout << std::dec << endl;
}

void go2() {
	
	srand(time(NULL));

	//Заполняем массив числами попорядку
	int l[100];
	for (int i = 1; i <= 100; i++)
		l[i-1] = i;
	
	int r[100];
	//случайно перемешиваем
	int i(99), j(0);
	while (i >0) {
		auto c = rand() % (i+1);
		r[i] = l[c];
		l[c] += l[i] - l[c];
		i--;
	}
	r[0] = l[0];

	//выводим
	for (auto& a : r)
		cout << a << "	";
	cout << "\n\n";

	//выводим циклы
	for (int k = 1, j = 0; j < 100; )
	{
		i = k;
		int s(0), t;
		do {
			t = r[i - 1];
			cout << i << "=" << t << "	";
			r[i - 1] = 0;
			i = t;
			s++;
			if (s % 10 == 0)cout << endl;
		} while (i != k);
		cout << "\n-----" << s << "-----\n" << endl;
		j+=s;	
		if (j < 100)
			k = *std::find_if(r, r + 99, [](auto& a) {return a > 0; });
	}

}


class ssa {
	static int gc;
	int cu;
public:
	//static void clear() { gc = 0; }

	ssa()noexcept:cu(++gc) { cout << " Created " << cu; }

	ssa(const ssa& val)noexcept :cu(val.cu) { cout << " Copy " << cu; }
	 
	ssa(ssa&& val)noexcept :cu(val.cu) { val.cu*=-1; cout << " Move " << cu; }

	ssa& operator =(const ssa& val) { 
		cout << cu << '=' << val.cu; 
		cu = val.cu; 
		return *this; 
	}
	ssa& operator =(ssa&& val) noexcept{
		cout << cu << '<-' << val.cu; 
		cu = val.cu; 
		val.cu = 0;
		return *this;
	}

	~ssa() { cout << " Destroy " << cu; }

	 
};
int ssa::gc;

class sa {
public:
	int r, g, b;
	sa(const int& r, const int& g, const int& b) :r(r), g(g), b(b) {}
	~sa() { cout << "dest sa\n"; }
};

class saa : public sa {
public:
	int alfa;
	saa(const int& r, const int& g, const int& b, const int& a = 0):sa(r,g,b), alfa(a) {}
	saa(const sa& v, const int& a = 0) :sa(v), alfa(a) {}
	~saa() { cout << "dest saa\n"; }
};

template <unsigned short N, typename Type, typename... >
struct get_index_by_type {
	constexpr static const unsigned short value = 0;
	get_index_by_type() = delete;
};

template <unsigned short N, typename Type, typename First, typename... Other>
struct get_index_by_type<N, Type, First, Other...> {
	constexpr static const unsigned short value = std::is_same_v<First, Type> ? N : get_index_by_type<N + 1, Type, Other...>::value;
	get_index_by_type() = delete;
};

template<typename T, typename... Other>
auto get_index_by_type_v() {
	return get_index_by_type<0, T, Other...>::value;
};


/*
template <unsigned short N, typename... >
struct get_type_by_index {
	using vtype = void;
	get_type_by_index() = delete;
};

template <typename First, typename... Other>
struct get_type_by_index<0, First, Other...> {
	using vtype = First;
	get_type_by_index() = delete;
};

template <unsigned short N, typename First, typename... Other>
struct get_type_by_index<N, First, Other...> {
	using vtype = get_type_by_index<N - 1, Other...>::vtype;
	get_type_by_index() = delete;
};*/


template <typename... Types>
class Mvariant {
	template <typename... Types2>
	union varU {
		template <typename T>
		T* GetVal();// = delete;
		//~varU() {}
	};

	template <typename Head, typename... Tile>
	union varU<Head, Tile...> {
		Head val;
		varU<Tile...> vals;

	template <typename T>
	T* GetVal() {
		//if constexpr (N == 0)
		if constexpr (std::is_same_v<Head, T>)
			return &val;
		else
			return vals.GetVal<T>();
	}

	template <typename T>
	varU(const T& val) { 
		auto p = GetVal<T>(); 
		new(p) T(val); 
	}

	template <typename T>
	varU(T&& val) {
		auto p = GetVal<T>();
		new(p) T(std::forward<T>(val));
	}

	varU() {}
	~varU() {}
	};
	unsigned short curenttype;
	const unsigned short curtype() { return curenttype; }
public:
	varU<Types...> vals;

	template<typename T>
	static unsigned short get_index_by_type() {
		return get_index_by_type_v<T, Types...>();
	}

	template<typename T>
	T& it(T&& val) {
		unsigned short i = Mvariant::get_index_by_type<T>();
		auto a = this->vals.GetVal<T>();
		if (i != curenttype) {
			//auto p = this->vals.GetVal<get_type_by_index<curtype(), Types...>::vtype>();
			//this->vals.GetVal<curenttype, get_type_by_index_struct<curenttype, Types...>::vtype>();
			a->~T();
			//this->vals.GetVal<curenttype, get_type_by_index_struct<curenttype, Types...>::vtype>();
			//auto b = this->vals.GetVal<get_type_by_index_struct<N, Types...>::vtype>();
			curenttype = i;
			new (a) T(std::forward<T>(val));

		}
		else
			*a = std::forward<T>(val);

		return *a;
	}

	template<typename T>
	T& it() {
		unsigned short i = Mvariant::get_index_by_type<T>();
		assert(i == curenttype);
		return *this->vals.GetVal<T>();
	}

	template<typename T>
	Mvariant(const T& val):vals(val)
	{
		curenttype = get_index_by_type<T>();
		//it(val);
	};
	template<typename T>
	Mvariant(T&& val) :vals(std::forward<T>(val))
	{
		curenttype = get_index_by_type<T>();
		//it(val);
	};
	~Mvariant() {};
};



void go() {

	auto m = Mvariant<int, double, char, ssa>(ssa());
	//Mvariant<int, double, char, INT64> m(5000000000000);
	m.it(5.13);
	cout << sizeof(m) << endl;

	cout << m.it<double>();

	//get_type_by_index_struct<2, char, int, bool, double>::vtype f;

	m.it(ssa());
	//m.it(7001035);
	auto a= get_index_by_type_v<float, int, double, char, float>();
	cout << a << "	" << m.get_index_by_type<char>() << endl;

	m.it(32);

	/*variant<int, double, char> iv;
	iv = 55.3;
	cout << iv.valueless_by_exception();*/

	/*m.vars
	
	cout << m.c.size() << " : " << m.i << " : " << m.f << endl;
	ShowBites<char>(&m, sizeof(m));

	m.i = 32;
	cout << m.c.size() << " : " << m.i << " : " << m.f << endl;
	ShowBites<char>(&m, sizeof(m));

	m.c = "Andrey";
	cout << m.c.size() << " : " << m.i << " : " << m.f << endl;
	ShowBites<char>(&m, sizeof(m));

	m.f = 1;
	cout << m.c.size() << " : " << m.i << " : " << m.f << endl;*/
	//ShowBites<char>(&m, sizeof(m));

	//cout << m.c << endl;

	
	//auto d = new ssa();

	//delete d;

	/*auto a = std::tuple<int, bool, double, char>(50, true, 3.12, 15);

	auto& [r,g,b, x] = a;
	cout << r << "\t" << g << "\t" << b << endl;
	std::get<int>(a) += 20;
	//a._Get_rest
	cout << r << "\t" << g << "\t" << b << endl;*/
}

class sa2 {
private:
	struct base {
		//base() = delete;
		virtual base* get_copy() { return this; };
		virtual ~base() { std::cout << "~base\n"; }

		//template<typename T>
		//virtual void& GetValue();
	};

	template<typename T>
	struct Derived: public base {
		T value;
		Derived(const T& val):value(val) {}
		~Derived() { std::cout << "~Derived "<< value << "\n"; }
		base* get_copy()override { return new Derived(value); };
		T& GetValue() { return value; };
	};

	base* storage = nullptr;

public:
	template<typename T>
	sa2(const T& val)// :storage(new Derived(val))
	{ storage = new Derived<T>(val); }
	sa2(const sa2& val) :storage(val.storage->get_copy()) {}
	~sa2() { delete storage; }

	template<typename T>
	sa2& operator=(const T& val){
		delete storage;
		storage = new Derived<T>(val);
		return *this;
	}

	sa2& operator=(const sa2& val) {
		delete storage;
		storage = val.storage->get_copy();
		return *this;
	}
};

struct t2 {
	bool operator()(int a, int b)const { return a > b; }
	bool Att(int a, int b)const { return a == b; }
};

template<typename T, typename...args>
auto f(const T& v, args... Args) ->decltype(T().Att(Args...))
{
	std::cout << "@2 ";
	return v.Att(Args...);
}

template<typename T, typename...args>
auto f(const T& v, args... Args) -> decltype(T().size(Args...))
{
	std::cout << "@3 ";
	return v.size(Args...);
}

int f(...) {
	std::cout <<"@1 ";
	return 1;
}

template <class cp = double, cp pi = M_PI, class T>
cp sqrround(const T& r)
{ return pi * pow(r, 2); }

template <class T>
T sum(const T& a, const T& b) { return a + b; }

template <class T, class U = T, class = std::enable_if_t<!std::is_same_v<U,T>>>
auto sum(const T& a, const U& b) { return a + b; }

//template <class T, class U>
//auto sum(const T& a, const U& b) { return a + b; }

class A {
public:int a = 16;
	A() { cout << "A\n"; }
	A(int i) { cout << "A " << i << '\n'; }
	void fo() { cout << "A\n"; }
	~A() { cout << "~A\n"; }
};

template <class T>
struct SunClass_: public T {
	virtual ~SunClass_() {}
};

class B:public SunClass_<A> {
public: int b = 1;
	B() { cout << "B\n"; }
	B(int i) { cout << "B " << i << '\n'; }
	~B()override { cout << "~B\n"; }
};

class C :public SunClass_<A> {
public:int c = 2;
	C() { cout << "C\n"; }
	C(int i) { cout << "С " << i << '\n'; }
	~C()override { cout << "~C\n"; }
};

class D :public B, public C {
public: int d = 3;
	D() { cout << "D\n"; }
	D(int i) { cout << "D " << i << '\n'; }
};

class C1:public B, public A { 
	//A a = 0;
public:
	C1() { cout << "C1 " << '\n'; }
	C1(int i) { cout << "C1 " << i << '\n';	}
	void fo() { cout << "c1\n"; }
	void fo2() { cout << "fc1 "; }
};

class C2: public C1 {
public:
	C2() { cout << "C2\n"; }
	C2(int i):C1(i) { cout << "C2 " << i << '\n'; }

	void fo() const { cout << "C2\n"; }
	void fo2() const { cout << "fC2\n"; }
};

class C3 : public C1 {
public:
	C3() { cout << "C3\n"; }
	C3(int i) :C1(i) { cout << "C3 " << i << '\n'; }
	void fo() { cout << "c3\n"; }
	void fo2() { cout << "fC3\n"; }
};


template<class T>
void pi(const char* str, void* p1, T* p2) { 
	cout << str << p2 << "=" << *p2 << " / " << (int)(p2)-(int)(p1) << "(" << sizeof(T) << ")\n"; }



class ca {
public:
	virtual void fo() const =0;
};

void aaa(const ca& val) 
{ val.fo(); }

class caNew:public ca, public C2 {
public:
	//caNew() {};
	void fo() const override { C2::fo(); }
};

template <typename T>
class divider {
public:
	T v;
	divider(int v) :v(v) { if (v == 0) throw std::exception("Divisor is 0"); }
	operator T() { return v; }
};
//int operator /(const int& a, const mint& b) { if (b.v == 0) throw 0; return a / b.v; };

template <typename T, typename U>
void mm(T a, U b){
	cout << 1000 + a / divider<U>(b) << endl;
}

template <typename T>
void mm(T a, T b) {
	cout << 3000 + a + b << endl;
}

template <>
void mm<>(int a, int b) {
	cout << 5000 + a + b << endl;
}

//void mm(int a, int b) {
//	cout << 2000 + a + b << endl; }


template < typename...>
struct Object {};

template <typename First, typename... Other>
struct Object<First, Other...> {
public:
	First val;
	Object<Other ...> is;
	Object<First, Other...>(const First& val, const Other& ... vals):val(val), is(vals...) {}
};

template <typename T>
struct Object<T> {
public:
	T val;
	Object<T>(const T& val):val(val){}
};

template <typename X, typename T>
X getx(T val) { return val + val; }

void go3() {

	float p11 = getx<float>(7);
	ShowBites<char>(&p11, sizeof(p11));
	//cout << p11.getx() << '\n';

	auto obj = Object<int, int, const char*, int>(5, 10, "Hellou", 100);

	ShowBites<int>(&obj, sizeof(obj));
	cout << obj.val << ", " << obj.is.val << ", " << obj.is.is.val << "\n";

	try {
		//cB ccbb;
		mm(2, 0.1);
		mm(2.0, 0.1);
		mm(2.0, 1);
		mm<int>(2, 1);
	}
	catch (exception& i){
		cout << "ERROR: " << i.what() << " !!!\n";
	}
	catch (...)	{
		cout << "ERROR!!!\n";
	}


	C c;
	SunClass_<A>* A_val = &c;
	cout << "A size:" << sizeof(A) << " *: " << A_val << '\n';
	cout << "C size:" << sizeof(C) << " typeid: " << &typeid(C) << '\n';
	ShowBites<char>(A_val, sizeof(A_val));
	ShowBites<char>(&c, sizeof(C));

	int* ptr = reinterpret_cast<int*>(A_val);

	auto& ti = typeid(C);
	ShowBites<char>(&ti, sizeof(ti), "typeid(C) - ");
	ShowBites<int>(A_val, 12, "A_val (3int)\n");
	//cout << ti.name() << ' ' << hex << *(reinterpret_cast<int*>(A_val)) << "		" << reinterpret_cast<int>(&ti)<<endl;

	ShowBites<int>((*(int**)A_val - 1), 20,			 "&A_val -1 (5int) | ");
	ShowBites<char>(*(int**)(*(int**)A_val - 1), 20, "    &(&A_val -1) | ");

	//delete A_val;

	return;
	

	caNew dd;

	C2& dd2 = dd;

	cout << sizeof(dd) << " &: " << &dd << '\n';
	pi("Aa: ", &dd, &dd.C2::A::a);
	pi("Ba: ", &dd, &dd.C2::B::a);
	pi("b: ", &dd, &dd.b);
	//pi("c: ", &dd, &dd.c);
	//pi("d: ", &dd, &dd.d);

	ShowBites<char>(&dd, sizeof(dd));

	cout << sizeof(dd2) << " &: " << &dd2 << '\n';
	pi("Aa: ", &dd2, &dd2.C2::A::a);
	pi("Ba: ", &dd2, &dd2.C2::B::a);
	pi("b: ", &dd2, &dd2.b);
	//pi("c: ", &dd, &dd.c);
	//pi("d: ", &dd, &dd.d);

	ShowBites<char>(&dd2, sizeof(dd2));

	/* {auto ai = reinterpret_cast<int*>(&dd);
	for (auto i = 0; i < 6; ++i)
		cout << ai[i] << " ";
	cout << endl; 

	cout << "T0 ^ " << *reinterpret_cast<int*>(ai[0]) << endl;
	cout << "T0 ^ " << *(reinterpret_cast<int*>(ai[0]) + 1) << endl;
	cout << "T2 ^ " << *reinterpret_cast<int*>(ai[2]) << endl; 
	cout << "T2 ^ " << *(reinterpret_cast<int*>(ai[2]) + 1) << endl;
}*/


	C1 fi1(25);
	auto fi2 = C2(45);
	caNew can;

	C3 fi3(35);
	C1& fi = fi2;

	aaa(can);
	cout << sizeof(can) << "	" << sizeof(C2) << endl;

	ca& aca = can;

	ShowBites<char>(&fi2, sizeof(fi1));
	fi1.A::a = 14;
	fi1.B::a = 15;
	ShowBites<char>(&fi2, sizeof(fi1));

	//cout << "\n!!" << sizeof(cc);
	fi.fo();
	cout << "fi1 " << sizeof(fi1);
	ShowBites<char>(&fi1, sizeof(fi1));
	cout<<"fi2 " << sizeof(fi2);
	ShowBites<char>(&fi2, sizeof(fi2));
	cout << "fi3 " << sizeof(fi3);
	ShowBites<char>(&fi3, sizeof(fi3));
	
	cout << "fi " << sizeof(fi);
	ShowBites<char>(&fi, sizeof(fi));


	return;

	//int g;
	cout << sum(150, 160) << endl;
	cout << sum<int>(150.0, 160.5) << endl;
	cout << sum<char>(150, 160) << endl;
	cout << sum<int>(150, 160) << endl;
	cout << sum(150.5, 160) << endl;

	cout << sqrround(10) << endl;
	cout << sqrround<float, 3.1415f>(10) << endl;

	return;

	//cout << sizeof(sa) << endl;
	cout << sizeof(sa2) << endl;

	double db = 3.14;

	sa2 v = sa2(100);
	v = 150.3;

	int a = 10;

	function<bool(int, int)>r = [](int v1, int v2) { return v1 < v2; };
	
	cout << r(3, 4)<< '\n';
	r = t2();
	cout << r(3, 4) << '\n';
	function<bool(t2&, int, int)> r2 = &t2::Att;

	t2 p;
	t2 o;
	vector<int> o2{1,3};

	cout << "t2 " << f(o, 4, 6) << endl;
	cout << "vector "<< f(o2) << endl;
	cout << "int "<< f(4, 45) << endl;

	auto r3 = std::bind(&t2::Att, p, std::placeholders::_1, std::placeholders::_2);
			
	cout << r2(p, 3, 4) << '\n';
	cout << r3(3, 4) << '\n';

	//cout << (p.*r3)(3, 4) << '\n';


	//decltype(r) r2(5);

	std::cout << a << "	" << sizeof(r)<< endl;

	//std::any_cast<int>(v);

	//sa2 g;

	//ForwardList<sa> ls = { (1, 10, 20), (30, 50, 70) };
	//ls.push_back(100, 150, 200);
	

	/*
	struct A;
	printf("%zu\n", sizeof(void(A::*)()));
	
	auto d = 3.14234567890123456789034534587574;
	auto a = 101;

	//new int(454);

	SmartStrW s(d);

	s.Length(4);
	s >> 777;
	

	s << 500 << "==============" << d << s;

	wcout << *s << "\n" << "\n" << to_string(d).c_str();*/
}

int main()
{
	go3();
	_CrtDumpMemoryLeaks();
	return 0;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
