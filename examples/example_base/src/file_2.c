/*
 * file_1.c
 *
 *  Created on: 2015年4月29日
 *      Author: yaozhongkai
 */


#include "head.h"

static char ____declare_1_____ = 0;

int declare_use_1(int x, int y)
{
	int a = 1;
	int b = 2;
	int c = 0;
	c = declare_fun_1(a, b);
	return c;
}

int declare_use_2(int x, int y)
{
	int a = 1;
	int b = 2;
	int c = 0;
	c = declare_fun_2(a, b);
	return c;
}

static char ____inline_____ = 0;

int inline_use_base_2(int a, int b)
{
	int x = 1;
	int y = 2;
	int z = 0;
	z = inline_fun_base_1(x,y);
	z = inline_fun_base_2(x,y);
	return z;
}
