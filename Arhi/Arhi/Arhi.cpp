// Arhi.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <bit>
#include <bitset> 

#include <iostream>
#include <fstream>
#include <list>

#include <algorithm>
//#include <numeric>

#include <vector>
#include <map>

#pragma once

typedef unsigned char UC;
using namespace std;

bool SaveVal(UC val, UC size, UC& tec_bit, UC*& buf) {

    tec_bit += size;

    if (tec_bit > 8) {
        tec_bit -= 8;
        *buf |= (val >> tec_bit);
        val <<= 8 - tec_bit;

        buf++;
        *buf = val;
    }
    else if (tec_bit == 8) {
        *buf |= val;
        tec_bit = 0;
        buf++;
    }
    else {
        val <<= 8 - tec_bit;
        *buf |= val;
        return false;
    }
    return true;
}

bool SaveNull(UC size, UC& tec_bit, UC*& buf, bool full = false) {

    tec_bit += size;
    if (tec_bit < 8)
        return false;
    
    while (tec_bit > 8){
        buf++;
        tec_bit -= 8;
    }
    return true;
}


auto masks = map<pair<UC, UC>, UC>();

UC GetMask(UC size, UC tec_bit) {
    auto p = pair<UC, UC>(tec_bit, size);
    auto f = masks.find(p);
    if (f != masks.end()) return f->second;

    UC res = 0xff;
    res <<= 8 - size;
    res >>= tec_bit;

    masks.insert(pair<pair<UC, UC>, UC>(p, res));
    return res;
}

UC LoadVal(UC size, UC& tec_bit, UC*& buf) {
    UC res;

    if(tec_bit + size < 9){
        res = GetMask(size, tec_bit);
        res &= *buf;
        res >>= 8 - size - tec_bit;
        tec_bit += size;
    }
    else {
        size -= 8 - tec_bit;
        res = LoadVal(8 - tec_bit, tec_bit, buf);
        res <<= size;
        res |= LoadVal(size, tec_bit, buf);
    }

    if (tec_bit > 7){
        tec_bit -= 8;
        buf++;
    }

    return res;
}

UC GetBitSize(int val) {
    UC res = 1;
    while (val >>= 1 > 0) res++;
    return res;
}

bool FullBits(UC size, UC& tec_bit, UC*& buf, bool full = false) {
    if (tec_bit + size < 9) {
        if (full) 
            *buf |= GetMask(size, tec_bit);
        tec_bit += size;
    }
    else {
        size -= 8 - tec_bit;
        FullBits(8 - tec_bit, tec_bit, buf, full);
        while (size > 0) {
            FullBits(size > 8 ? 8 : size, tec_bit, buf, full);
            size -= size > 8 ? 8 : size;
        }
        return true;
    }

    if (tec_bit > 7) {
        tec_bit -= 8;
        buf++;
        return true;
    }
    
    return false;
}

void ArchIn(int argc, char* argv[]) {

    ifstream filein(argv[2], ios::binary | ios::ate);
    if (!filein.is_open()) {
        std::cout << "Не удалось открыть файл: " << argv[2] << endl;
        exit(0);
    }

    int fsz = filein.tellg();
    std::cout << "Размер файла: " << fsz << endl;
    if (fsz == 0) {
        std::cout << "Пустой файл" << endl;
        exit(0);
    }

    filein.seekg(0);

    int sz = fsz;
    if (sz > 5000) sz = 5000;
    unsigned char* buf = new unsigned char[sz];

    struct st1 {
    public:
        int count = 0, fi = 0;
        UC val = 0;
        
        st1() {};
        st1(UC _val, UC _fi, int _count = 0) {
            val = _val;
            fi = _fi;
            count = _count;
        }
    };

    auto lp = vector<st1>();

    filein.read((char*)buf, sz);

    struct MyStruct {
        int count = 0, countall = 0, size = 0, sizeone = 0, sizeplus = 0;
        unsigned char max = 0;
    };
    auto infobytes = map<char, MyStruct>();
    auto sizes = map<char, pair<UC, UC>>();

    unsigned char tmp, tec_bit, ch = 2, first = (buf[0] & 0x80);
    bool  globbyte = first == 0x80;
    st1 pch = st1();

    tmp = 1;
    for (auto i = 1; tmp < 7; i++) {
        if (i >= ch) { tmp++; ch <<= 1; }
        sizes[i] = pair<UC, UC>(tmp, (1 << tmp) - 1);
    }
    
    std::cout << "Начальный бит:" << (first == 0x80 ? 1 : 0) << endl;
    auto mx = sizes.rbegin()->first;

    ch = 0;
    while (fsz > 0) {
        std::cout << "\rПрочитано: " << filein.tellg() << std::flush;
        //for (auto pos = 0; pos <= sz; pos++) showByte(buf[pos], " ");
        for (auto pos = 0; pos <= sz; pos++)
        {
            UC& tecByte = buf[pos];
            tmp = 0x80;
            for (char i = 0; i < 8; i++) {
                tec_bit = tecByte & tmp;
                if (tec_bit == tmp != globbyte) {
                    if (pch.val == ch)
                        pch.count++;
                    else {
                        pch.val = ch;
                        if (pch.count > 1)
                            for_each(lp.rbegin(), lp.rbegin() + pch.count, [&pch](auto& a) {a.count = pch.count;});
                        pch.fi = lp.size();
                        pch.count = 1;
                    }
                    lp.push_back(st1(ch, lp.size() - pch.fi, 0));

                    if (ch > mx && sizes.count(ch) == 0) {
                        UC  k = GetBitSize(ch);
                        sizes[ch] = pair<UC, UC>(k, (1 << k) - 1);
                    }
                    tec_bit = sizes.at(ch).first;
                    infobytes[tec_bit].count++;
                    //if(ch >= 10)cout << "("<<(int)ch<<")"; else cout << (int)ch;
                    ch = 1;
                    globbyte = !globbyte;
                }
                else ch++;
                tmp >>= 1;
            }
            //if(pos%100 == 0)
              //  std::cout << "\rCompleted " << sz-pos << std::flush;
        }

        if (pch.count > 0)
            lp[pch.fi].count = pch.count;

        fsz -= sz;
        if (fsz > 0) {
            if (fsz < sz)
                sz = fsz;
            filein.read((char*)buf, sz);
        }
    }
    filein.close();

    UC bitsize = 1;
    {
        auto x = max_element(infobytes.begin(), infobytes.end(), [](auto& x, auto& y) { return x.first < y.first; });
        bitsize = GetBitSize(x->first);
    }

    auto res = max_element(lp.begin(), lp.end(), [](auto& a, auto& b) {return a.val < b.val; });
    std::cout << "\n==================================\nДлина выходных данных: " << lp.size() << " (Макс. значение: " << 
        (int)res->val << ")\nДлина записи размера: " << (int)bitsize << endl;

    for (auto& y : infobytes) {
        for_each(infobytes.begin(), infobytes.end(), [&y](auto x) {
            if (x.first < y.first) y.second.countall += x.second.count; });
        y.second.countall += y.second.count;
        y.second.sizeone = y.second.count * y.first;
        y.second.size = y.second.countall * y.first;

        y.second.max = (1 << y.first) - 1;
    }

    for (auto& y : infobytes)
        for_each(infobytes.begin(), infobytes.end(), [bitsize, &y](auto x)
            {if (x.first > y.first) y.second.sizeplus += x.second.sizeone + x.second.count * (bitsize+ y.first); });

    for (auto& x : infobytes) {
        auto& size = x.second;
        std::cout << (short)x.first << " C:" << size.count << " max:" << (int)size.max
            << " Ca:" << size.countall
            << " So:" << size.sizeone << " S:" << size.size << " Sp:" << size.sizeplus
            << " Sm:" << size.size + size.sizeplus << endl;
    }
    std::cout << "\n==================================" << endl;

    auto x = sizes[0];
    {
        auto j = min_element(infobytes.begin(), infobytes.end(), [](auto& x, auto& y)
            { return x.second.size + x.second.sizeplus < y.second.size + y.second.sizeplus; });

        auto& size = j->second;
        std::cout << (short)j->first << " C:" << size.count << " Ca:" << size.countall
            << " So:" << size.sizeone << " S:" << size.size << " Sp:" << size.sizeplus
            << " Sm:" << size.size + size.sizeplus << endl;
        x = pair<UC, UC>(j->first, (1 << j->first) - 1);

        UC rv = 0;
        cin >> rv;

        rv -= '1';
        if(rv > infobytes.size()) exit(0);
        x = pair<UC, UC>(rv+1, (1 << (rv + 1)) - 1);
    }

    std::cout << "\n>>>>> Save <<<<<" << endl;
    ofstream fileout;
    {
        string fn;
        if (argc > 3) fn = argv[3];
        else fn = string(argv[2]) + "arhc";

        fileout.open(fn, ios::binary | ios::out);
        if (!fileout.is_open()) {
            std::cout << "Не удалось создать файл: " << fn << endl;
            exit(0);
        }
    }

    sz = 5000;
    buf = new UC[sz + 5];
    std::memset(buf, 0, sz + 5);
    UC* tb = buf;
    *tb = first;
    tec_bit = 1;

    SaveVal(x.first, 4, tec_bit, tb);
    SaveVal(bitsize, 7, tec_bit, tb);
    UC j, bsz = 0, l = 0;

    for_each(lp.begin(), lp.end(), [&l](auto x) {if (x.count > l) l = x.count;  });

    UC bitsize2 = GetBitSize(l);
    l = (bitsize2 + x.first * 2) / x.first;

    SaveVal(bitsize2, 7, tec_bit, tb);

    cout << (int)x.first << " " << (int)bitsize << " " << (int)bitsize2 << endl;

    for (auto& i : lp) {
        cout << (int)i.val;

        if (i.count > l) {
            if (i.fi > 0) continue;
            SaveNull(x.first * 2, tec_bit, tb);
            SaveVal(i.count, bitsize2, tec_bit, tb);
            //std::cout << "[" << (int)i.count << "] " << (int)i.val << "   " << endl;
        }

        if (i.val <= x.second)
            j = x.first;
        else {
            SaveNull(x.first, tec_bit, tb);
            j = sizes[i.val].first;
            SaveVal(j, bitsize, tec_bit, tb);
            //std::cout << "(" << (int)j << ") " << (int)i.val << "   " << bitset<8>(tb[-1]) << endl;
        }
        tmp = i.val;

        if (SaveVal(tmp, j, tec_bit, tb) && tb > buf + sz) {
            fileout.write((char*)buf, tb - buf);
            std::cout << "\rЗаписано: " << tb - buf << " " << std::endl;
            buf[0] = *tb;
            memset(buf, 1, sz + 4);
            tb = buf;
        };

        if((tb - buf) % 100 == 0)
            std::cout << "\rОбработано: " << tb - buf << " " << std::flush;
    }

    tmp = tec_bit;
    if (tec_bit > 5) {
        tb++;
        tmp = 6 + 8 - tec_bit;
    }

    tec_bit = 5;
    SaveVal(tmp, 3, tec_bit, tb);

    std::cout << ">>>>> Save end <<<<<" << endl;

    fileout.write((char*)buf, tb - buf);
    fileout.close();
}

void ArchFrom(int argc, char* argv[]) {

    ifstream filein(argv[2], ios::binary | ios::ate);
    if (!filein.is_open()) {
        std::cout << "Не удалось открыть файл: " << argv[2] << endl;
        exit(0);
    }

    int fsz = filein.tellg();
    std::cout << "Размер файла: " << fsz << endl;
    if (fsz == 0) {
        std::cout << "Пустой файл" << endl;
        exit(0);
    }

    ofstream fileout;
    {
        string fn;
        if (argc > 3) fn = argv[3];
        else {fn = string(argv[2]); fn = fn.substr(0, fn.length() - 4); }

        fileout.open(fn, ios::binary | ios::out);
        if (!fileout.is_open()) {
            std::cout << "Не удалось создать файл: " << fn << endl;
            exit(0);
        }
    }

    filein.seekg(0);

    int sz = fsz;
    if (sz > 5000) sz = 5000;
    unsigned char* buf = new unsigned char[sz];

    filein.read((char*)buf, sz);

    UC *tecByte = buf;
    UC tmp, tec_bit = 1, tec_bit2 = 0, ch, end = 0xff;
   
    const int sz2 = 5000;
    auto buf2 = new unsigned char[sz2+4];
    memset(buf2, 0, sz2 + 4);
    UC* tb = buf2;
    
    bool tf = (tb[0] & 0x80) != 0x80;
    
    ch = LoadVal(4, tec_bit, tecByte);
    UC bitsize = LoadVal(7, tec_bit, tecByte),
        bitsize2 = LoadVal(7, tec_bit, tecByte),
        loop = 0;

    std::cout << "Начальный бит:" << !tf << endl;

    while (fsz > 0) {
        //std::cout << "\rПрочитано: " << filein.tellg() << std::flush;
        if (fsz == sz) {
            end = buf[sz - 1] & 7;
            if (end > 5) { sz--; end = 8 - end + 5; }
        }
        
        while (tecByte - buf < sz){

            if (end != 0xff and tecByte - buf == sz - 1 and end == tec_bit)
                break;

            if (loop == 0) {
                tmp = LoadVal(ch, tec_bit, tecByte);
                if (tmp == 0) {
                    tmp = LoadVal(ch, tec_bit, tecByte);
                    if (tmp == 0) {
                        loop = LoadVal(bitsize2, tec_bit, tecByte)-1;
                        tmp = LoadVal(ch, tec_bit, tecByte);
                    }else {
                    tmp = LoadVal(tmp, tec_bit, tecByte);
                    }
                }
            }
            else loop--;

            std::cout << (int)tmp;
            FullBits(tmp, tec_bit2, tb, tf);
            tf = !tf;

            if (tb > buf2 + sz2) {
                fileout.write((char*)buf2, tb - buf2);
                std::cout << "\rЗаписано: " << tb - buf2 << std::endl;
                buf2[0] = *tb;
                memset(buf2, 1, sz + 4);
                tb = buf2;
            };
        }

        fsz -= sz;
        if (fsz > 0) {
            if (fsz < sz)
                sz = fsz;
            filein.read((char*)buf, sz);
        }
    }
    filein.close();

    fileout.write((char*)buf2, tb - buf2);
    fileout.close();

    /*
    UC bitsize2 = GetBitSize(l);
    l = (bitsize2 + x.first * 2) / x.first;

    cout << bitset<8>(*tb) << " **********\n";

    for (auto& i : lp) {

        if (i.count > l) {
            if (i.fi > 0) continue;
            SaveNull(x.first * 2, tec_bit, tb);
            SaveVal(i.count, bitsize2, tec_bit, tb);
            //std::cout << "[" << (int)i.count << "] " << (int)i.val << "   " << endl;
        }

        if (i.val <= x.second)
            j = x.first;
        else {
            SaveNull(x.first, tec_bit, tb);
            j = sizes[i.val].first;
            SaveVal(j, bitsize, tec_bit, tb);
            //std::cout << "(" << (int)j << ") " << (int)i.val << "   " << bitset<8>(tb[-1]) << endl;
        }
        tmp = i.val;

        if (SaveVal(tmp, j, tec_bit, tb) && tb > buf + sz) {
            fileout.write((char*)buf, tb - buf);
            std::cout << "\rЗаписано: " << tb - buf << std::endl;
            buf[0] = *tb;
            memset(buf, 1, sz + 4);
            tb = buf;
        };

        if ((tb - buf) % 100 == 0)
            std::cout << "\rОбработано: " << tb - buf << std::flush;
    }

    SaveVal(tec_bit, 3, tec_bit, tb);

    fileout.write((char*)buf, tb - buf);
    fileout.close(); */
}

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "");
    if (argc < 3) {
        std::cout << "(/A|/U) FileIn {FileOut}" << endl;
        exit(0);
    }

    /*auto a = map<int, int>();
    a.insert(pair<int, int>(1, 3));
    a.insert(pair<int, int>(1, 2));
    a.insert(pair<int, int>(2, 2));
    auto b = a.insert(pair<int, int>(1, 4));

    cout << (a.find(1) != a.end());

    exit(0);*/
    std::cout << argc << endl << argv[2] << endl;

    if (strcmp(argv[1], "/A") == 0 )
        ArchIn(argc, argv);
    else if (strcmp(argv[1], "/U") == 0)
        ArchFrom(argc, argv);
    else cout << "Неизвестный режим работы: " << argv[1];
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
