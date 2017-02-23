/*
 * head.h
 *
 *  Created on: 2015年4月29日
 *      Author: yaozhongkai
 */

#ifndef SUB_PROJECT_EXAMPLES_EXAMPLES_1_HEAD_H_
#define SUB_PROJECT_EXAMPLES_EXAMPLES_1_HEAD_H_

#include <stdio.h>

struct export_type_on_h {
	int x;
	int y;
};

#define DOWHILE0ONHFILE              \
do {					             \
	printf("DOWHILE0ONHFILE\n");	 \
} while(0)

static int st_in_debug()
{
	printf("st_in_debug\n");
	return 0;
}

int call_to_debug();

int call_file_1();

static char ____declare_h_____ = 0;

int declare_fun_1(int x, int y);
extern int declare_fun_2(int x, int y);

static char ____array_h_____ = 0;

extern unsigned int array_declare_1[];
extern unsigned int array_declare_2[50];
//extern unsigned int array_declare_3[][];
//extern unsigned int array_declare_4[3][];
extern unsigned int array_declare_5[][4];
extern unsigned int array_declare_6[3][4];

static char ____inline_h_____ = 0;
/*inline effect with gcc option -O*/

static inline int inline_fun_base_1(int x, int y)
{
	return x + y;
}
inline static int inline_fun_base_2(int x, int y)
{
	return x + y;
}

inline static int inline_fun_point_1(int x, int y)
{
	return x + y;
}
inline static int inline_fun_point_2(int x, int y)
{
	return x + y;
}
inline static int inline_fun_point_3(int x, int y)
{
	return x + y;
}
inline static int inline_fun_point_4(int x, int y)
{
	return x + y;
}
inline static int inline_fun_point_5(int x, int y)
{
	return x + y;
}
inline static int inline_fun_point_6(int x, int y)
{
	return x + y;
}

#endif /* SUB_PROJECT_EXAMPLES_EXAMPLES_1_HEAD_H_ */
