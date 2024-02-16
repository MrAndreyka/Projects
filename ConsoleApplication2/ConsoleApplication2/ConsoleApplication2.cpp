// ConsoleApplication2.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#ifdef _DEBUG
#include <crtdbg.h>
#define _CRTDBG_MAP_ALLOC 
/*#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW*/
#endif

#include <string>
#include <iostream>
#include "..\..\Iincludes\class.h"
#include "..\..\Iincludes\DataClass.cpp"
//#include "..\..\Iincludes\wincom.h"
#include <stdlib.h>
#include <algorithm>
#include <vector>

using namespace std;
void go2() {
	
	srand(time(NULL));
	int r[100];

	/*ForwardList<int> l;
	for (int i = 1; i <= 100; i++)
		l.push_back(i);


	int i = 0;
	auto c = l.cbegin();
	while (i < 100) {
		auto r = rand() % 99;
		while (r > 0)
			if (r--,(++c).isEnd())
				c = l.cbegin();
		if (c == l.cbefore_end())
			r[i] = l.cut_begin();
		else
			r[i] = l.cut_after(c);
		i++;
	}*/
	int l[100];
	for (int i = 1; i <= 100; i++)
		l[i-1] = i;

	int i(99), j(0);
	while (i >0) {
		auto c = rand() % (i+1);
		r[i] = l[c];
		l[c] += l[i] - l[c];
		i--;
	}
	r[0] = l[0];

	for (auto& a : r)
		cout << a << "	";
	cout << "\n\n";

	ForwardList<int> ls;

	for (int k = 1, j = 0; j < 100; )
	{
		ls.clear();

		i = k;
		do {
			cout << i << "=" << r[i - 1] << "	";
			ls.push_back(r[i - 1]);
			r[i - 1] = 0;
			i = ls.back();			
			j++;
			if (j % 10 == 0)cout << endl;
		} while (i != k);
		cout << "\n-----" << ls.get_size() << "-----\n" << endl;
		auto rrr= std::min_element(ls.begin(), ls.end());
		cout << "\n-----" << ls.get_size() << "-----\n" << endl;
			
		if (j < 100)
			k = *std::find_if(r, r + 99, [](auto& a) {return a > 0; });
	}

}


void go() {

	auto d = 3.14234567890123456789034534587574;
	auto a = 101;

	//new int(454);

	SmartStrW s(d);

	s.Length(4);
	s >> 777;
	

	s << 500 << "==============" << d << s;

	wcout << *s << "\n" << "\n" << to_string(d).c_str();
}

int main()
{
	go2();
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
