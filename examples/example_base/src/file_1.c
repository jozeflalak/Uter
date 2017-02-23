/*
 * file_1.c
 *
 *  Created on: 2015年4月29日
 *      Author: yaozhongkai
 */


#include "head.h"

static char ____declare_1_____ = 0;

int declare_fun_1(int x, int y)
{
	return x + y;
}

int declare_fun_2(int x, int y)
{
	return x + y;
}

static char ____static_____ = 0;
static int export_var_static_dup = 9;

static int export_fun_static_dup(int x, int y)
{
	return x + y + 6;
}

struct export_type_dup {
	int x;
	int y;
	int z;
};

struct export_type_dup export_type_dup_var;

static char ____inline_____ = 0;

int inline_use_base_1(int a, int b)
{
	int x = 1;
	int y = 2;
	int z = 0;
	z = inline_fun_base_1(x,y);
	z = inline_fun_base_2(x,y);
	return z;
}

int inline_use_point_1(int a, int b)
{
	int x = a;
	int y = b;
	int z = 0;
	int (*g_inline_fun_point_1)(int x, int y) = NULL;
	g_inline_fun_point_1 = inline_fun_point_1;
	z = g_inline_fun_point_1(x,y);		/*inline*/
	return z;
}

int inline_use_point_2()
{
	int x = 1;
	int y = 2;
	int z = 0;
	int (*g_inline_fun_point_2)(int x, int y) = NULL;
	g_inline_fun_point_2 = inline_fun_point_2;
	//z = g_inline_fun_point_2(x,y);	/*None*/
	return z;
}

int (*g_inline_fun_point_3)(int x, int y) = NULL;
int inline_use_point_3()
{
	int x = 1;
	int y = 2;
	int z = 0;
	g_inline_fun_point_3 = inline_fun_point_3;
	z = g_inline_fun_point_3(x,y);		/*inline*/
	return z;
}

int (*g_inline_fun_point_4)(int x, int y) = NULL;
int inline_use_point_4()
{
	int x = 1;
	int y = 2;
	int z = 0;
	g_inline_fun_point_4 = inline_fun_point_4;
	//z = g_inline_fun_point_4(x,y);	/*outline*/
	return z;
}

int (*g_inline_fun_point_5)(int x, int y) = inline_fun_point_5;
int inline_use_point_5()
{
	int x = 1;
	int y = 2;
	int z = 0;
	z = g_inline_fun_point_5(x,y);		/*outline*/
	return z;
}

int (*g_inline_fun_point_6)(int x, int y) = inline_fun_point_6;
int inline_use_point_6()
{
	int x = 1;
	int y = 2;
	int z = 0;
	//z = g_inline_fun_point_6(x,y);	/*outline*/
	return z;
}

static char ____array_____ = 0;

unsigned int array_declare_1[50];
unsigned int array_declare_2[50];
unsigned int array_declare_3[3][4];
unsigned int array_declare_4[3][4];
unsigned int array_declare_5[3][4];
unsigned int array_declare_6[3][4];

static char ____support_____ = 0;
int static_need_use_file_1()
{
	export_fun_static_dup(export_var_static_dup, 1);
	export_type_dup_var.x = 1;
	return 0;
}

struct export_type_on_h export_type_on_h_var_on_file1;

int call_to_debug()
{
	int x = 0;
	int y = 1;
	int z =  x + y;

	DOWHILE0ONHFILE;

	st_in_debug();

	return 0;
}

int call_file_1()
{
	int x = inline_use_point_1(5, 6);
	return 0;
}
