#include "resource.h"
#include "D3D11_Framework.h"
#include "MyRender.h"



using namespace D3D11Framework;

class MyInput : public InputListener
{
public:
	bool KeyPressed(const KeyEvent& arg)
	{
		printf("key press %c\n", arg.wc);
		return false;
	}
	bool MouseMove(const MouseEvent& arg)
	{
		printf("mouse %d - %d\n", arg.x, arg.y);
		return false;
	}
};



int main()
{
	Framework framework;

	MyRender* render = new MyRender();
	//MyInput* input = new MyInput();

	FrameworkDesc dsk;
	dsk.render = render;
	dsk.wnd.caption = L"DX11 Frame";
	dsk.wnd.width = 1200;
	dsk.wnd.height = 800;
	dsk.wnd.posx = 50;
	dsk.wnd.posy = 50;
	dsk.icon = LoadIcon(NULL, (LPCTSTR)IDI_ICON1);

	framework.Init(dsk);
	//framework.AddInputListener(input);

	framework.Run();

	framework.Close();

	return 0;
}