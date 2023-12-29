#include <graphics.h>
#include <iostream.h>
#include <stdlib.h>

using std;

struct Point {
	int x;
	int y;
};


int main() {

	int grdriver = DETECT;	//Инициализируем графический драйвер
	int gmode;
	initgraph(&grdriver, &gmode, "");	//Инициализируем графический режим


	Point T1, T2;	//объявили точки

	T1.x = 10;		//Задали значения
	T1.y = 10;

	T2.x = 100;
	T2.y = 150;

	rectangle(T1.x, T1.y, T2.x, T2.y);	//нарисовали прямоугольник
	cin.get();

	closegraph();	//Прекращаем работу графического режима
	return 0;
}