#include "DISPLAY.H"

static draw_f drawFunc;

void Set_Draw_Func(draw_f New_drawFunc)
{
	if(New_drawFunc != NULL)
	{
		drawFunc = New_drawFunc;
	}
}

void display_updata(void)
{
	animotion_updata();

	drawFunc();
	
	
}
