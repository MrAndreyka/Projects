// ConsoleApplication2.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#ifdef _DEBUG
#include <crtdbg.h>
#define _CRTDBG_MAP_ALLOC
#endif

#include <iostream>
#include <algorithm>

using namespace std;

template <class T>
class ForfardList {
private:
    template <class T>
    class Cell {
        friend ForfardList;
    private:
        T value;
        Cell<T> *next = nullptr;
    public:
        Cell() {};
        Cell(const T& val) :value(val) {};
        T& operator*() const { return value; };
        T& at() { return value; }
    };

    void push_after(const T& val, Cell<T>* elem) {
        auto t = new Cell<T>(val);
        t->next = elem->next;
        elem->next = t;
    }

    Cell<T>* _begin, * _end;
public:
    //using typename ForfardList<T> 
    template <class T, class To>
    class iterator {
        friend ForfardList;
    private:
        Cell<T> *pos;
        iterator(Cell<T>* _pos): pos(_pos){}
    public:
        iterator() {}
        iterator(const iterator& val):pos(val.pos) {}

        bool operator!=(iterator const& other) const{ return pos != other.pos; };
        bool operator==(iterator const& other) const{ return pos == other.pos; };
        To& operator*() const { return pos->at(); };
        To& operator->() const { return pos-> at(); };
        iterator& operator++() { pos = pos->next; return *this; };
        bool isEnd() { return pos == nullptr; }
    };
    
    ForfardList() { _begin = _end = nullptr; }
    ForfardList(const T &val) { _begin = _end = new Cell<T>(val); }

    ~ForfardList() {
        if (_begin == nullptr) return;
        for (Cell<T> *p, *i = p = _begin; i != nullptr; p = i)
        {   
            i = p->next;
            delete p;
        }
    }
    iterator<T, T> begin() const { return _begin; }
    iterator<T, T> end() const { return _end->next; }

    iterator<T, const T> cbegin() const { return _begin; }
    iterator<T, const T> cend() const { return _end->next; }

    iterator<T, T> before_end() const { return _end; }
    iterator<T, const T> cbefore_end() const { return _end; }

    void push_back(const T &val) {_end = (_end==nullptr)? _begin = new Cell<T>(val) : (_end->next = new Cell<T>(val));}
    void push_begin(const T& val) {
        auto t = new Cell<T>(val);
        t->next = _begin;
        _begin = t;
        if (!_end)
            _end = _begin;
    }

    void push_after(const T& val, const iterator<T, T>& elem) {push_after(val, elem.pos);}

    ForfardList<T>& operator <<(const T& val) {
        push_back(val);
        return *this;
    }
    ForfardList<T>& operator >>(const T& val) {
        push_begin(val);
        return *this;
    }
};

void go() {
 
    ForfardList<int> ls;
    for (int i = 1; i < 10; i++)
       ls << i;

    for_each(ls.begin(), ls.end(), [](auto &x) {
        x*=11; });

    auto p = find_if(ls.begin(), ls.end(), [](auto& x) {return x > 50; });
    p.isEnd() ? (cout << "not find" << endl) : (cout << "Find: " << *p << endl);
    if (!p.isEnd()) ls.push_after(444, p); 
    
    ls.push_begin(323);

    for(auto &x: ls)
        cout << x << endl;
}

int main()
{
    go();
   
    _CrtDumpMemoryLeaks();
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
