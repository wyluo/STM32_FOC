#include "ITEM2.H"


static uint8_t optionCount = 5;

static void menus_select(void);		//菜单确定选择函数
static void loader(uint8_t num);	//菜单查找执行函数

static prev_menu_s prev_menu;

void ITEM2_menu(void)
{
	Set_Menu_Info(optionCount,MENU_TYPE_ICON,PSTR(STR_SETTINGMENU));
	Set_Menu_Func(prevOption,menus_select,nextOption,loader);
	Set_Menu_Pre(&prev_menu,ITEM2_menu);
	
	Animation_InTo(NULL);
}

static void menus_select(void)
{
	Get_Menu_Pre(&prev_menu);
	DoAction(true);
}

static void loader(uint8_t num)
{
	uint8_t i = 0;
	
	Set_Menu_Option(i++, PSTR(MENU1), &SETTING_MUNE[0][0], NULL);
	Set_Menu_Option(i++, PSTR(MENU2), &SETTING_MUNE[1][0], NULL);
	Set_Menu_Option(i++, PSTR(MENU3), &SETTING_MUNE[2][0], NULL);
	Set_Menu_Option(i++, PSTR(MENU4), &SETTING_MUNE[3][0], NULL);
	AddExitOption();
}