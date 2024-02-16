﻿// ConsoleAnlizer.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include "parser.h"
#include <list>

using namespace std;
int main()
{
    setlocale(LC_ALL, "russian");
    
    string s("Hello World! P.S.: Mr.Bimm");
    std::cout << s << endl;
    std::cout << s.find_first_not_of(" \n", 5) << endl; 
    
    list<token> tl;
    tl.emplace_back(token::Type::Number, s,323);
    //tl.push_back(token::Type::Number, s);

    s = "pi*598 -   (456 / 1.2)";
    s += "\t-/1";
    std::cout << s << endl;

    parser a;
    auto r = a.parse(s);
    if (r.error != 0) 
        std::cout << s.insert(r.pos, string("[Позиция:") + to_string(r.pos)+"\t" + a.ErrorToStr(r.error) + "]") << endl;
 
   for (const auto &i: a.Items())
       std::cout << i << endl;
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
