/*
 * main.c
 *
 *  Created on: 2015年4月29日
 *      Author: yaozhongkai
 */

#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <sys/types.h>

#include "head.h"
#include "so_debug.h"
#include "so_release.h"

/*Base type*/
static char ____base_type_data_____ = 0;
/*char*/
char g_char_0 = 0;
char g_char_1 = 1;
char g_char_2 = 2;
char g_char_max = 0x7f;
char g_char_min = 0x80;
/*uchar*/
unsigned char g_uchar_0 = 0;
unsigned char g_uchar_1 = 1;
unsigned char g_uchar_2 = 2;
unsigned char g_uchar_max = 0xff;
unsigned char g_uchar_min = 0;
/*short*/
short g_short_0 = 0;
short g_short_1 = 1;
short g_short_2 = 2;
short g_short_max = 0x7fff;
short g_short_min = 0x8000;
/*ushort*/
unsigned short g_ushort_0 = 0;
unsigned short g_ushort_1 = 1;
unsigned short g_ushort_2 = 2;
unsigned short g_ushort_max = 0xffff;
unsigned short g_ushort_min = 0;
/*int*/
int g_int_0 = 0;
int g_int_1 = 1;
int g_int_2 = 2;
int g_int_max = 0x7fffffff;
int g_int_min = 0x80000000;
/*uint*/
unsigned int g_uint_0 = 0;
unsigned int g_uint_1 = 1;
unsigned int g_uint_2 = 2;
unsigned int g_uint_max = 0xffffffff;
unsigned int g_uint_min = 0;
/*long*/
long g_long_0 = 0;
long g_long_1 = 1;
long g_long_2 = 2;
long g_long_max = 0x7fffffff;
long g_long_min = 0x80000000;
/*ulong*/
unsigned long g_ulong_0 = 0;
unsigned long g_ulong_1 = 1;
unsigned long g_ulong_2 = 2;
unsigned long g_ulong_max = 0xffffffff;
unsigned long g_ulong_min = 0;
/*longlong*/
long long g_longlong_0 = 0;
long long g_longlong_1 = 1;
long long g_longlong_2 = 2;
long long g_longlong_max = 0x7fffffffffffffff;
long long g_longlong_min = 0x8000000000000000;
/*ulonglong*/
unsigned long long g_ulonglong_0 = 0;
unsigned long long g_ulonglong_1 = 1;
unsigned long long g_ulonglong_2 = 2;
unsigned long long g_ulonglong_max = 0xffffffffffffffff;
unsigned long long g_ulonglong_min = 0;
/*float*/
float g_float_0 = 0;
float g_float_1 = 1;
float g_float_2 = 2;
/*double*/
double g_double_0 = 0;
double g_double_1 = 1;
double g_double_2 = 2;

/*Export*/
static char ____export_something_____ = 0;
int export_var = 5;

int export_fun(int x, int y)
{
	return x + y + 6;
}

static int export_var_static = 9;

static int export_fun_static(int x, int y)
{
	return x + y + 6;
}

static int export_var_static_dup = 9;

static int export_fun_static_dup(int x, int y)
{
	return x + y + 6;
}

struct export_type {
	int x;
	int y;
};

struct export_type export_type_var;
struct export_type_on_h export_type_on_h_var_on_main;

struct export_type_dup {
	int x;
	int y;
};

struct export_type_dup export_type_dup_var;

/*Struct*/
static char ____datatype_struct_____ = 0;

struct st_base_0 {
};

struct st_base_1 {
    int m_1;
};
struct st_base_1 st_base_1_var;

struct st_base_2 {
    int m_1;
    int m_2;
};
struct st_base_2 st_base_2_var;

struct st_base_3 {
    char m_1;
    char m_2;
};
struct st_base_3 st_base_3_var;

struct st_base_4 {
    char m_1;
    int m_2;
};
struct st_base_4 st_base_4_var;

/*Struct alignment*/
struct st_align_1 {
	int m_1;
	char m_2;
};
struct st_align_1 st_align_1_var;

struct st_align_2 {
	int m_1;
	char m_2;
	int m_3;
};
struct st_align_2 st_align_2_var;

struct st_align_3 {
	int m_1;
	char m_2;
	short m_3;
	int m_4;
};
struct st_align_3 st_align_3_var;

struct st_align_4 {
	int m_1;
	long long m_2;
	int m_3;
};
struct st_align_4 st_align_4_var;

/*Struct in struct*/
struct stinst_1_in_1 {
	char m_1;
	int m_2;
};

struct stinst_1_in_2 {
	int m_1;
	char m_2;
};

struct stinst_1_out_1 {
	struct stinst_1_in_1 m_1;
	struct stinst_1_in_2 m_2;
	int m_3;
};

struct stinst_1_out_1 stinst_1_out_1_var;

/*Member name*/
struct stname_1 {
	int m_1;
	int _m_2;
	int __m_3;
	int ___m_4;
};
struct stname_1 stname_1_var;

struct /*stnoname_1*/ {
	int m_1;
	int m_2;
} stnoname_1_var;

struct /*stnoname_2*/ {
	int m_1;
	char m_2;
} stnoname_2_var;

struct stnomem_1 {

};
struct stnomem_1 stnomem_1_var;

struct st_yesiuseit {
	char m_1;
	struct stnomem_1 m_2;
	char m_3;
};

struct st_yesiuseit st_yesiuseit_var;

/*Union*/
static char ____datatype_union_____ = 0;

union un_base_0 {
};

union un_base_1 {
	unsigned int m_1;
};
union un_base_1 un_base_1_var;

union un_base_2 {
	unsigned int m_1;
	unsigned char m_2;
};
union un_base_2 un_base_2_var;

/*Union in union*/
struct someinun_1_in_1 {
	unsigned char m_1;
	unsigned int m_2;
};

union someinun_1_in_2 {
	unsigned int m_1;
	unsigned char m_2;
};

struct someinun_1_in_3 {
	unsigned char m_1;
	unsigned char m_2;
	unsigned char m_3;
	unsigned char m_4;
	unsigned char m_5;
	unsigned char m_6;
	unsigned char m_7;
	unsigned char m_8;
};

union someinun_1_out_1 {
	struct someinun_1_in_1 m_1;
	union someinun_1_in_2 m_2;
	struct someinun_1_in_3 m_3;
	int m_4;
};

union someinun_1_out_1 someinun_1_out_1_var;

/*Union name*/
union unname_1 {
	int m_1;
	int _m_2;
	int __m_3;
	int ___m_4;
};
union unname_1 unname_1_var;

union /*unnoname_1*/ {
	int m_1;
	int m_2;
} unnoname_1_var;

union /*unnoname_2*/ {
	int m_1;
	char m_2;
} unnoname_2_var;

/*Enum*/
static char ____datatype_enum_____ = 0;

//enum en_base_0 {
//};

enum en_base_1 {
	EN1_M0,
	EN1_M1,
	EN1_M2,
	EN1_M3 = -1,
	EN1_M4,
};
enum en_base_1 en_base_1_var;

enum en_base_2 {
	EN2_M0 = -4,
	EN2_M1 = 5,
	EN2_M2,
	EN2_M3 = 5,
	EN2_M4,
};
enum en_base_2 en_base_2_var;

/*Enum name*/
enum enname_1 {
	ENNAME1,
	_ENNAME2,
	__ENNAME3,
	___ENNAME4,
};
enum enname_1 enname_1_var;

enum {
	NONAME1_M1,
	NONAME1_M2,
	NONAME1_M3,
} ennoname_1_var;

enum {
	NONAME2_M1,
	NONAME2_M2,
} ennoname_2_var;

/*Point*/
static char ____datatype_point_____ = 0;
int g_p_int_2_v = 666;

int *g_p_int_0 = NULL;
int *g_p_int_1 = (int *)0x12345678;
int *g_p_int_2 = &g_p_int_2_v;
int **g_pp_int_0 = NULL;

struct g_p_st_1_t {
	int m_1;
	int m_2;
};

struct g_p_st_1_t g_p_st_2_v = {3, 4};

struct g_p_st_1_t *g_p_st_0 = NULL;
struct g_p_st_1_t *g_p_st_1 = (struct g_p_st_1_t *)0x12345678;
struct g_p_st_1_t *g_p_st_2 = &g_p_st_2_v;
struct g_p_st_1_t **g_pp_st_0 = NULL;

static char ____datatype_void_____ = 0;

void *g_p_void_0 = NULL;
void *g_p_void_1 = (void *)0x12345678;

/*Array*/
static char ____datatype_array_____ = 0;

int array_int_1[1] = {};
int array_int_2[2] = {};
int array_int_5[5] = {};

int array_int_31[3][1] = {};
int array_int_32[3][2] = {};
int array_int_35[3][5] = {};

unsigned int array_nosize_1[] = {
    0x00000000,
	0x00000001,
	0x00000002,
	0x00000003,
	0x00000004,
};

int array_zerosize_1_1[10];
int array_zerosize_1_2[0];
int array_zerosize_2_1[10][10];
int array_zerosize_2_2[0][0];
int array_zerosize_2_3[5][0];
int array_zerosize_2_4[0][5];

struct st_for_arry {
	int m_1;
	int m_2;
};

struct st_for_arry array_st_1[1] = {};
struct st_for_arry array_st_2[2] = {};
struct st_for_arry array_st_5[5] = {};

struct st_for_arry array_st_31[3][1] = {};
struct st_for_arry array_st_32[3][2] = {};
struct st_for_arry array_st_35[3][5] = {};



/*Typedef*/
static char ____datataype_typedef_____ = 0;

typedef unsigned int typedef_uint;
typedef_uint typedef_uint_var;

struct st_for_typedef_1 {
	int m_1;
	int m_2;
};
typedef struct st_for_typedef_1 typedef_st1;
typedef_st1 typedef_st1_var;

typedef typedef_uint typedef_uint_in_def;
typedef_uint_in_def typedef_uint_in_def_var;

typedef typedef_st1 typedef_st1_in_def;
typedef_st1_in_def typedef_st1_in_def_var;

typedef unsigned long typedef_ul_level_1;
typedef typedef_ul_level_1 typedef_ul_level_2;
typedef typedef_ul_level_2 typedef_ul_level_3;
typedef typedef_ul_level_3 typedef_ul_level_4;
typedef typedef_ul_level_4 typedef_ul_level_5;
typedef_ul_level_5 typedef_ul_level_5_var;

/*Macro*/
static char ____macro_____ = 0;

#define NUM_MACRO_1  123
#define NUM_MACRO_2  (123)
#define NUM_MACRO_3  ((123))

#define NUM_MACRO_4  (123 + 123)

#define STR_MACRO_1  "abc"
#define STR_MACRO_2  ("abc")
#define STR_MACRO_3  (("abc"))

#define STR_MACRO_4  ("abc" + "abc")
#define STR_MACRO_5  ("\"abc\"" + "\"abc\"")

#define BIT_MACRO_1  (0x00000001)
#define BIT_MACRO_2  (0x00000002)
#define BIT_MACRO_3  (0x00000004)
#define BIT_MACRO_4  (0x00000008)
#define BIT_MACRO_5  (0x00000010)

#define CMP_MACRO_1(x)  ((x) & BIT_MACRO_1)
#define CMP_MACRO_2(x)  ((x) & BIT_MACRO_2)
#define CMP_MACRO_3(x)  ((x) & BIT_MACRO_3)
#define CMP_MACRO_4(x)  ((x) & BIT_MACRO_4)
#define CMP_MACRO_5(x)  ((x) & BIT_MACRO_5)

#define FUN_MACRO_1(x)	(strlen(x) % 4)

#define MACROINMACRO_IN_1  (0x12345678)
#define MACROINMACRO_IN_2(x)  ((x)++)
#define MACROINMACRO_OUT(x)  (MACROINMACRO_IN_1 + MACROINMACRO_IN_2(x) + 6)

#define MACRO_WITHUNDEF_1  1
#undef MACRO_WITHUNDEF_1
#define MACRO_WITHUNDEF_1  2
#undef MACRO_WITHUNDEF_1
#define MACRO_WITHUNDEF_1  3
#undef MACRO_WITHUNDEF_1

#define MACRO_NO_VALUE
#define __SOME_HEAD_H__

#define SOME_ENABLE
#ifdef SOME_ENABLE
int g_some_enable = 100;
#endif
//#define SOME_NO_ENABLE
#ifdef SOME_NO_ENABLE
int g_some_no_enable = 100;
#endif

//#define MACRO_WRONG (x) 100

/*Function*/
static char ____function_base_____ = 0;
int fun_base_0()
{
	return 0;
}

int fun_base_1(int a)
{
	return 0;
}

int fun_base_2(int a, int b)
{
	return 0;
}

int fun_base_3(int a, int b, int c)
{
	return 0;
}

int fun_base_s(int a, int b, int c)
{
	return 0;
}

void fun_base_v0()
{
	return;
}

void fun_base_v1(int a)
{
	return;
}


int fun_base_fun_1(int a)
{
	return 0;
}

int (* fun_base_point_1)(int) = fun_base_fun_1;

int fun_base_fun_2(int a, int b)
{
	return 0;
}

int (* fun_base_point_2)(int, int) = fun_base_fun_2;

/*Function call*/
static char ____function_call_____ = 0;
int fun_call_base_0()
{
	return 1;
}

int fun_call_base_1(int x)
{
	printf("x = %d\n", x);
	return x + 5;
}

int fun_call_base_2(int x, int y)
{
	printf("x = %d, y = %d\n", x, y);
	return x + y + 5;
}

int fun_call_base_3(int *a, char *b)
{
	return *a + *b;
}

typedef int fun_call_base_typedef_1;
typedef char fun_call_base_typedef_2;
int fun_call_base_4(fun_call_base_typedef_1 a, fun_call_base_typedef_2 b)
{
	return a + b;
}

int fun_call_align_1(char a)
{
	printf("a = %d\n", a);
	return a;
}

int fun_call_align_2(char a, char b)
{
	printf("a = %d, b = %d\n", a, b);
	return a + b;
}

int fun_call_align_3(char a, int b, char c)
{
	printf("a = %d, b = %d, c = %d\n", a, b, c);
	return a + b + c;
}

int fun_call_align_4(int a, char b, int c)
{
	printf("a = %d, b = %d, c = %d\n", a, b, c);
	return a + b + c;
}

int fun_call_align_5(short a)
{
	printf("a = %d\n", a);
	return a;
}

int fun_call_align_6(short a, short b)
{
	printf("a = %d, b = %d\n", a, b);
	return a + b;
}

int fun_call_align_7(short a, int b, short c)
{
	printf("a = %d, b = %d, c = %d\n", a, b, c);
	return a + b + c;
}

int fun_call_align_8(int a, short b, int c)
{
	printf("a = %d, b = %d, c = %d\n", a, b, c);
	return a + b + c;
}

int fun_call_align_ll_1(long long a)
{
	printf("a = %lld\n", a);
	return a;
}

int fun_call_align_ll_2(long long a, long long b)
{
	printf("a = %lld, b = %lld\n", a, b);
	return a + b;
}

int fun_call_align_ll_3(long long a, int b, long long c)
{
	printf("a = %lld, b = %d, c = %lld\n", a, b, c);
	return a + b + c;
}

int fun_call_align_ll_4(int a, long long b, int c)
{
	printf("a = %d, b = %lld, c = %d\n", a, b, c);
	return a + b + c;
}

int fun_call_ret_1()
{
	return 0x7fffffff;
}

char fun_call_ret_2()
{
	return 120;
}

short fun_call_ret_3()
{
	return 255 * 4;
}

long long fun_call_ret_4()
{
	long long ret = 0xffffffff;
	ret += 1;
	return ret;
}

long long fun_call_ret_5()
{
	long long ret = 0xffffffff;
	ret += 0xffffffff;
	return ret;
}

void fun_call_ret_6()
{
	printf("I am return void\n");
	return;
}

struct fun_call_starg_st_1 {
	int a;
	int b;
};
int fun_call_starg_1(struct fun_call_starg_st_1 x)
{
	return x.a + x.b;
}

struct fun_call_starg_st_2 {
	char a;
	char b;
};
int fun_call_starg_2(struct fun_call_starg_st_2 x)
{
	return x.a + x.b;
}

struct fun_call_starg_st_3 {
	char a;
	int b;
	char c;
};
int fun_call_starg_3(struct fun_call_starg_st_3 x)
{
	return x.a + x.b + x.c;
}

struct fun_call_starg_st_4 {
	int a;
	char b;
	int c;
};
int fun_call_starg_4(struct fun_call_starg_st_4 x)
{
	return x.a + x.b + x.c;
}

struct fun_call_starg_st_5 {
	char a;
};
int fun_call_starg_5(struct fun_call_starg_st_5 x)
{
	return x.a;
}

struct fun_call_starg_st_6 {
	char a;
	char b;
};
int fun_call_starg_6(struct fun_call_starg_st_6 x, int y)
{
	return x.a + x.b + y;
}

struct fun_call_starg_st_7 {
	char a;
	char b;
};
int fun_call_starg_7(int z, struct fun_call_starg_st_7 x, int y)
{
	return x.a + x.b + y + z;
}

struct fun_call_starg_st_8 {
	char a;
	char b;
};
int fun_call_starg_8(struct fun_call_starg_st_8 x, int y, struct fun_call_starg_st_8 z)
{
	return x.a + x.b + y + z.a + z.b;
}

struct fun_call_starg_st_9 {
	char a;
};
int fun_call_starg_9(struct fun_call_starg_st_9 x, int y)
{
	return x.a + y;
}

struct fun_call_stret_st_1 {
	int a;
	int b;
};
struct fun_call_stret_st_1 fun_call_stret_1(int a, int b)
{
	struct fun_call_stret_st_1 l;
	l.a = a;
	l.b = b;
	return l;
}

struct fun_call_stret_st_2 {
	char a;
	char b;
};
struct fun_call_stret_st_2 fun_call_stret_2(int a, int b)
{
	struct fun_call_stret_st_2 l;
	l.a = a;
	l.b = b;
	return l;
}

struct fun_call_stret_st_3 {
	char a;
};
struct fun_call_stret_st_3 fun_call_stret_3(int a, int b)
{
	struct fun_call_stret_st_3 l;
	l.a = a + b;
	return l;
}

union fun_call_unarg_un_1 {
	unsigned int a;
	unsigned int b;
};
int fun_call_unarg_1(union fun_call_unarg_un_1 x)
{
	return x.a;
}

union fun_call_unarg_un_2 {
	unsigned char a;
	unsigned char b;
};
int fun_call_unarg_2(union fun_call_unarg_un_2 x)
{
	return x.a;
}

union fun_call_unarg_un_3 {
	unsigned int a;
	unsigned char b;
};
int fun_call_unarg_3(union fun_call_unarg_un_3 x)
{
	return x.a;
}

union fun_call_unarg_un_4 {
	unsigned int a;
	char b[4];
};
int fun_call_unarg_4(union fun_call_unarg_un_4 x)
{
	return x.a;
}

union fun_call_unarg_un_5 {
	unsigned int a;
	unsigned char b;
};
int fun_call_unarg_5(union fun_call_unarg_un_5 x, int y)
{
	return x.a + y;
}

union fun_call_unret_un_1 {
	unsigned int a;
	unsigned char b;
};
union fun_call_unret_un_1 fun_call_unret_1(int a, int b)
{
	union fun_call_unret_un_1 l;
	l.a = a + b;
	return l;
}

/*Function stub*/
static char ____function_stub_____ = 0;

int fun_stub_base_0_stub()
{
	printf("fun_stub_base_0_stub: \n");
	return 5;
}
int fun_stub_base_0_call()
{
	printf("fun_stub_base_0_call: \n");
	int x = fun_stub_base_0_stub();
	printf("fun_stub_base_0_stub return %d\n", x);
	return x;
}

int fun_stub_base_1_stub(int a)
{
	printf("fun_stub_base_1_stub:a = %d \n", a);
	return a;
}
int fun_stub_base_1_call(int a)
{
	printf("fun_stub_base_1_call:a = %d \n", a);
	int x = fun_stub_base_1_stub(a);
	printf("fun_stub_base_1_stub return %d\n", x);
	return x;
}

int fun_stub_base_2_stub(int a, int b)
{
	printf("fun_stub_base_2_stub:a = %d, b = %d\n", a, b);
	return a + b;
}
int fun_stub_base_2_call(int a, int b)
{
	printf("fun_stub_base_2_call:a = %d, b = %d\n", a, b);
	int x = fun_stub_base_2_stub(a, b);
	printf("fun_stub_base_2_stub return %d\n", x);
	return x;
}

int fun_stub_base_3_stub(char a)
{
	printf("fun_stub_base_3_stub:a = %d \n", a);
	return a;
}
int fun_stub_base_3_call(char a)
{
	printf("fun_stub_base_3_call:a = %d \n", a);
	int x = fun_stub_base_3_stub(a);
	printf("fun_stub_base_3_stub return %d\n", x);
	return x;
}

int fun_stub_base_4_stub(char a, char b)
{
	printf("fun_stub_base_4_stub:a = %d, b = %d\n", a, b);
	return a + b;
}
int fun_stub_base_4_call(char a, char b)
{
	printf("fun_stub_base_4_call:a = %d, b = %d\n", a, b);
	int x = fun_stub_base_4_stub(a, b);
	printf("fun_stub_base_4_stub return %d\n", x);
	return x;
}

int fun_stub_base_5_stub(char a, int b, char c)
{
	printf("fun_stub_base_5_stub:a = %d, b = %d, c = %d\n", a, b, c);
	return a + b + c;
}
int fun_stub_base_5_call(char a, int b, char c)
{
	printf("fun_stub_base_5_call:a = %d, b = %d, c = %d\n", a, b, c);
	int x = fun_stub_base_5_stub(a, b, c);
	printf("fun_stub_base_5_stub return %d\n", x);
	return x;
}

int fun_stub_base_6_stub(int a, char b, int c)
{
	printf("fun_stub_base_6_stub:a = %d, b = %d, c = %d\n", a, b, c);
	return a + b + c;
}
int fun_stub_base_6_call(int a, char b, int c)
{
	printf("fun_stub_base_6_call:a = %d, b = %d, c = %d\n", a, b, c);
	int x = fun_stub_base_6_stub(a, b, c);
	printf("fun_stub_base_6_stub return %d\n", x);
	return x;
}

int fun_stub_base_7_stub(short a)
{
	printf("fun_stub_base_7_stub:a = %d \n", a);
	return a;
}
int fun_stub_base_7_call(short a)
{
	printf("fun_stub_base_7_call:a = %d \n", a);
	int x = fun_stub_base_7_stub(a);
	printf("fun_stub_base_7_stub return %d\n", x);
	return x;
}

int fun_stub_base_8_stub(short a, short b)
{
	printf("fun_stub_base_8_stub:a = %d, b = %d\n", a, b);
	return a + b;
}
int fun_stub_base_8_call(short a, short b)
{
	printf("fun_stub_base_8_call:a = %d, b = %d\n", a, b);
	int x = fun_stub_base_8_stub(a, b);
	printf("fun_stub_base_8_stub return %d\n", x);
	return x;
}

int fun_stub_base_9_stub(short a, int b, short c)
{
	printf("fun_stub_base_9_stub:a = %d, b = %d, c = %d\n", a, b, c);
	return a + b + c;
}
int fun_stub_base_9_call(short a, int b, short c)
{
	printf("fun_stub_base_9_call:a = %d, b = %d, c = %d\n", a, b, c);
	int x = fun_stub_base_9_stub(a, b, c);
	printf("fun_stub_base_9_stub return %d\n", x);
	return x;
}

int fun_stub_base_10_stub(int a, short b, int c)
{
	printf("fun_stub_base_10_stub:a = %d, b = %d, c = %d\n", a, b, c);
	return a + b + c;
}
int fun_stub_base_10_call(int a, short b, int c)
{
	printf("fun_stub_base_10_call:a = %d, b = %d, c = %d\n", a, b, c);
	int x = fun_stub_base_10_stub(a, b, c);
	printf("fun_stub_base_10_stub return %d\n", x);
	return x;
}

int fun_stub_base_11_stub(long long a)
{
	printf("fun_stub_base_11_stub:a = %lld \n", a);
	return a;
}
int fun_stub_base_11_call(long long a)
{
	printf("fun_stub_base_11_call:a = %lld \n", a);
	int x = fun_stub_base_11_stub(a);
	printf("fun_stub_base_11_stub return %d\n", x);
	return x;
}

int fun_stub_base_12_stub(long long a, long long b)
{
	printf("fun_stub_base_12_stub:a = %lld, b = %lld\n", a, b);
	return a + b;
}
int fun_stub_base_12_call(long long a, long long b)
{
	printf("fun_stub_base_12_call:a = %lld, b = %lld\n", a, b);
	int x = fun_stub_base_12_stub(a, b);
	printf("fun_stub_base_12_stub return %d\n", x);
	return x;
}

int fun_stub_base_13_stub(long long a, int b, long long c)
{
	printf("fun_stub_base_13_stub:a = %lld, b = %d, c = %lld\n", a, b, c);
	return a + b + c;
}
int fun_stub_base_13_call(long long a, int b, long long c)
{
	printf("fun_stub_base_13_call:a = %lld, b = %d, c = %lld\n", a, b, c);
	int x = fun_stub_base_13_stub(a, b, c);
	printf("fun_stub_base_13_stub return %d\n", x);
	return x;
}

int fun_stub_base_14_stub(int a, long long b, int c)
{
	printf("fun_stub_base_14_stub:a = %d, b = %lld, c = %d\n", a, b, c);
	return a + b + c;
}
int fun_stub_base_14_call(int a, long long b, int c)
{
	printf("fun_stub_base_14_call:a = %d, b = %lld, c = %d\n", a, b, c);
	int x = fun_stub_base_14_stub(a, b, c);
	printf("fun_stub_base_14_stub return %d\n", x);
	return x;
}

void fun_stub_base_v_stub(int *a)
{
	printf("fun_stub_base_v_stub:a = %d \n", *a);
	*a = 55;
	return;
}
int fun_stub_base_v_call(int a)
{
	printf("fun_stub_base_v_call:a = %d \n", a);
	fun_stub_base_v_stub(&a);
	printf("fun_stub_base_v_stub return %d\n", a);
	return a;
}

long long fun_stub_base_ll_stub(int a)
{
	printf("fun_stub_base_ll_stub:a = %d \n", a);
	long long l = 0xffffffff;
	return l + a;
}
long long fun_stub_base_ll_call(int a)
{
	printf("fun_stub_base_v_call:a = %d \n", a);
	long long x = fun_stub_base_ll_stub(a);
	printf("fun_stub_base_v_stub return %lld\n", x);
	return x;
}

struct fun_stub_starg_st_1 {
	int a;
};
int fun_stub_starg_1_stub(struct fun_stub_starg_st_1 x)
{
	printf("fun_stub_starg_1_stub:x.a = %d \n", x.a);
	return x.a;
}
int fun_stub_starg_1_call(int a)
{
	printf("fun_stub_start_1_call:a = %d \n", a);
	struct fun_stub_starg_st_1 l;
	l.a = a;
	int x = fun_stub_starg_1_stub(l);
	printf("fun_stub_starg_1_stub return %d\n", x);
	return x;
}

struct fun_stub_starg_st_2 {
	int a;
	int b;
};
int fun_stub_starg_2_stub(struct fun_stub_starg_st_2 x)
{
	printf("fun_stub_starg_2_stub:x.a = %d, x.b = %d \n", x.a, x.b);
	return x.a + x.b;
}
int fun_stub_starg_2_call(int a, int b)
{
	printf("fun_stub_start_2_call:a = %d, b = %d \n", a, b);
	struct fun_stub_starg_st_2 l;
	l.a = a;
	l.b = b;
	int x = fun_stub_starg_2_stub(l);
	printf("fun_stub_starg_2_stub return %d\n", x);
	return x;
}

struct fun_stub_starg_st_3 {
	int a;
	char b;
	int c;
};
int fun_stub_starg_3_stub(struct fun_stub_starg_st_3 x)
{
	printf("fun_stub_starg_3_stub:x.a = %d, x.b = %d, x.c = %d \n", x.a, x.b, x.c);
	return x.a + x.b + x.c;
}
int fun_stub_starg_3_call(int a, int b, int c)
{
	printf("fun_stub_start_3_call:a = %d, b = %d, c = %d \n", a, b, c);
	struct fun_stub_starg_st_3 l;
	l.a = a;
	l.b = b;
	l.c = c;
	int x = fun_stub_starg_3_stub(l);
	printf("fun_stub_starg_3_stub return %d\n", x);
	return x;
}

struct fun_stub_starg_st_4 {
	int a;
	char b;
	int c;
};
int fun_stub_starg_4_stub(struct fun_stub_starg_st_4 x, int y)
{
	printf("fun_stub_starg_4_stub:x.a = %d, x.b = %d, x.c = %d \n", x.a, x.b, x.c);
	return x.a + x.b + x.c + y;
}
int fun_stub_starg_4_call(int a, int b, int c, int y)
{
	printf("fun_stub_start_4_call:a = %d, b = %d, c = %d \n", a, b, c);
	struct fun_stub_starg_st_4 l;
	l.a = a;
	l.b = b;
	l.c = c;
	int x = fun_stub_starg_4_stub(l, y);
	printf("fun_stub_starg_4_stub return %d\n", x);
	return x;
}

struct fun_stub_stret_st_1 {
	int a;
	char b;
	int c;
};
struct fun_stub_stret_st_1 fun_stub_stret_1_stub(int x, int y, int z)
{
	printf("fun_stub_stret_1_stub:x = %d, y = %d, z = %d \n", x, y, z);
	struct fun_stub_stret_st_1 l;
	l.a = x;
	l.b = y;
	l.c = z;
	return l;
}
int fun_stub_stret_1_call(int x, int y, int z)
{
	printf("fun_stub_stret_1_call:x = %d, y = %d, z = %d \n", x, y, z);
	struct fun_stub_stret_st_1 r = fun_stub_stret_1_stub(x, y, z);
	printf("fun_stub_stret_1_stub return r.a = %d, r.b = %d, r.c = %d \n", r.a, r.b, r.c);
	return r.a + r.b + r.c;
}

union fun_stub_unarg_un_1 {
	int a;
	char b;
};
int fun_stub_unarg_1_stub(union fun_stub_unarg_un_1 x)
{
	printf("fun_stub_starg_1_stub:x.a = %d, x.b = %d \n", x.a, x.b);
	return x.a;
}
int fun_stub_unarg_1_call(int a, int b)
{
	printf("fun_stub_unarg_1_call:a = %d, b = %d \n", a, b);
	union fun_stub_unarg_un_1 l;
	l.a = a;
	int x = fun_stub_unarg_1_stub(l);
	printf("fun_stub_unarg_1_stub return %d\n", x);
	return x;
}

union fun_stub_unret_un_1 {
	int a;
	char b;
	int c;
};
union fun_stub_unret_un_1 fun_stub_unret_1_stub(int x, int y, int z)
{
	printf("fun_stub_unret_1_stub:x = %d, y = %d, z = %d \n", x, y, z);
	union fun_stub_unret_un_1 l;
	l.a = x;
	return l;
}
int fun_stub_unret_1_call(int x, int y, int z)
{
	printf("fun_stub_unret_1_call:x = %d, y = %d, z = %d \n", x, y, z);
	union fun_stub_unret_un_1 r = fun_stub_unret_1_stub(x, y, z);
	printf("fun_stub_unret_1_stub return r.a = %d, r.b = %d, r.c = %d \n", r.a, r.b, r.c);
	return r.a;
}

/**/
int fun_stub(int a, int b)
{
	int ret_val = 0;
	/*do something*/
	return  ret_val;
}

int fun_call(int a, int b)
{
	int stub_ret =  fun_stub(a + 1, b + 1);
	if (stub_ret < 0) {
		return -1;
	} else if (stub_ret == 0) {
		return 0;
	} else {
		return 1;
	}
	return -1;
}

/* Coverage */
static char ____coverage_____ = 0;

int fun_coverage_base()
{
	return 0;
}

int fun_coverage_if_eq(int x)
{
	if (x == 5) {
		return 8;
	} else {
		return 18;
	}
}

int fun_coverage_if_neq(int x)
{
	if (x != 5) {
		return 8;
	} else {
		return 18;
	}
}

int fun_coverage_if_lt(int x)
{
	if (x < 5) {
		return 8;
	} else {
		return 18;
	}
}

int fun_coverage_if_gt(int x)
{
	if (x > 5) {
		return 8;
	} else {
		return 18;
	}
}

int fun_coverage_if_le(int x)
{
	if (x <= 5) {
		return 8;
	} else {
		return 18;
	}
}

int fun_coverage_if_ge(int x)
{
	if (x >= 5) {
		return 8;
	} else {
		return 18;
	}
}

int fun_coverage_if_noelse(int x)
{
	if (x == 5) {
		return 8;
	}

	return 0;
}

int fun_coverage_if_elseif(int x)
{
	if (x == 1) {
		return 8;
	} else if (x == 2) {
		return 9;
	} else if (x == 3) {
		return 10;
	} else if (x == 4) {
		return 11;
	} else {
		return 12;
	}
}

int fun_coverage_switch(int x)
{
	int ret = 0;

	switch(x)
	{
	case 1:
	{
		ret += 2;
		break;
	}
	case 2:
	{
		ret += 3;
		break;
	}
	case 3:
	{
		ret += 4;
		break;
	}
	default:
	{
		ret += 100;
		break;
	}
	}

	return ret;
}

int fun_coverage_switch_nobreak(int x)
{
	int ret = 0;

	switch(x)
	{
	case 1:
	{
		ret += 2;
		break;
	}
	case 2:
	{
		ret += 3;
		//break;
	}
	/* no break */
	case 3:
	{
		ret += 4;
		break;
	}
	default:
	{
		ret += 100;
		break;
	}
	}

	return ret;
}

int fun_coverage_for(int x)
{
	int ret = 0;

	int iloop;
	for (iloop = 0; iloop < x; iloop++) {
		ret += 2;
	}

	return ret;
}

int fun_coverage_while(int x)
{
	int ret = 0;

	int iloop = 0;
	while(iloop < x) {
		ret += 2;
		iloop++;
	}

	return ret;
}

int fun_coverage_dowhile(int x)
{
	int ret = 0;

	int iloop = 0;
	do {
		ret += 2;
		iloop++;
	} while(iloop < x);

	return ret;
}

static char ____dyn_so_____ = 0;
int dyn_test()
{
	printf("fun_in_debug_so return %d \n", fun_in_debug_so(5, 6));
	printf("var_in_debug_so is %d \n", var_in_debug_so);

	printf("fun_in_release_so return %d \n", fun_in_release_so(5, 6));
	printf("var_in_release_so is %d \n", var_in_release_so);
	return 0;
}

/*For others*/
static char ____support_____ = 0;
int static_need_use()
{
	export_fun_static(export_var_static, 1);
	export_fun_static_dup(export_var_static_dup, 1);

	unsigned int x;
	x = array_declare_1[10];
	x = array_declare_2[10];
	//x = array_declare_3[1][1];
	//x = array_declare_4[1][1];
	x = array_declare_5[1][1];
	x = array_declare_6[1][1];

	return 0;
}

int test_stub_without_call_stub()
{
	printf("test_stub_without_call_stub\n");
	return 0;
}

int test_stub_without_call()
{
	int iloop;
	for (iloop = 0; iloop < 5; iloop++) {
		test_stub_without_call_stub();
	}
	return 0;
}

void *new_thread_1(void *arg)
{
	prctl(PR_SET_NAME, (unsigned long)"New_thread_1");

	int iloop;
	for (iloop = 0; ; iloop++) {
		printf("New thread 1 never return loop %d\n", iloop);
		sleep(1);
	}

	return NULL;
}

void *new_thread_2(void *arg)
{
	prctl(PR_SET_NAME, (unsigned long)"New_thread_2");

	int iloop;
	for (iloop = 0; ; iloop++) {
		printf("New thread 2 never return loop %d\n", iloop);
		sleep(1);
	}

	return NULL;
}

static char ____declare_____ = 0;
struct declare_st_1;
struct declare_st_use_1 {
	int x;
	int y;
	struct declare_st_1 *z;
};
struct declare_st_use_1 declare_st_use_1_1;
struct declare_st_use_1 *p_declare_st_use_1_1 = NULL;

union declare_un_1;
struct declare_un_use_1 {
	int x;
	int y;
	union declare_un_1 *z;
};
struct declare_un_use_1 declare_un_use_1_1;
struct declare_un_use_1 *p_declare_un_use_1_1 = NULL;

enum declare_en_1;
struct declare_en_use_1 {
	int x;
	int y;
	enum declare_en_1 *z;
};
struct declare_en_use_1 declare_en_use_1_1;
struct declare_en_use_1 *p_declare_en_use_1_1 = NULL;

static char ____const_____ = 0;

int const const_int_1 = 1;		//-> const_type -> base_type
const int const_int_2 = 2;		//-> const_type -> base_type

/* left const of point, point to const
 * right const of point, const point*/
const char * const_charp_1;		//-> pointer_type -> const_type -> base_type
char const * const_charp_2;		//-> pointer_type -> const_type -> base_type
char * const const_charp_3;		//-> const_type -> pointer_type -> base_type

const char * const const_charp_4;	//-> const_type -> pointer_type -> const_type -> base_type
char const * const const_charp_5;	//-> const_type -> pointer_type -> const_type -> base_type

static void const_test()
{
	const_charp_1 = NULL;
	const_charp_2 = NULL;
	//*const_charp_1 = 0;
	//*const_charp_2 = 0;

	//const_charp_3 = NULL;
	*const_charp_3 = 0;

	//const_charp_4 = NULL;
	//const_charp_5 = NULL;
	//*const_charp_4 = 0;
	//*const_charp_5 = 0;

	return;
}

static char ____stack_____ = 0;

/*Stack test*/
int stack_test_1()
{
	int x = 6;
	return x;
}
int stack_test_2()
{
	stack_test_1();
	return 0;
}
int stack_test_3()
{
	stack_test_2();
	return 0;
}
int stack_test_4()
{
	stack_test_3();
	return 0;
}
int stack_test_5()
{
	stack_test_4();
	return 0;
}
int stack_so_deep()
{
	stack_test_5();
	return 0;
}

int debug_loop()
{
	while(1) {
		int stop_point_1 = 100;
		stack_so_deep();
		int stop_point_2 = 200;
	}
	return 0;
}

static char ____step_____ = 0;

/*Step test*/
void debug_step_01()
{
	int x = 1;
	x++;
	x++;x++;
	x++;x++;x++;
	x++;x++;x++;x++;
	return;
}

int debug_step_02()
{
	int x = 1;
	x++;
	x++;x++;
	x++;x++;x++;
	x++;x++;x++;x++;
	return x;
}

int debug_step_03(int y)
{
	int x = y;
	x++;
	x++;x++;
	x++;x++;x++;
	x++;x++;x++;x++;
	return x;
}

int debug_step_04(int y, int z)
{
	int x = y + z;
	x++;
	x++;x++;
	x++;x++;x++;
	x++;x++;x++;x++;
	return x;
}

int debug_step()
{
	int stop_here_1 = 100;

	/*Seq test*/
	int seq_01 = 1;
	seq_01++;
	int seq_02 = 2;
	seq_02++;
	int seq_03 = 3;
	seq_03++;
	seq_03++;seq_03++;
	seq_03++;seq_03++;seq_03++;
	seq_03++;seq_03++;seq_03++;seq_03++;
	seq_03++;seq_03++;seq_03++;seq_03++;seq_03++;
	seq_03++;seq_03++;seq_03++;seq_03++;seq_03++;seq_03++;

	/*If test*/
	int if_x = 10;
	int if_y = 9;
	if (if_x > if_y) {
		int stop_point = 100;
	}
	if (if_x < if_y) {
		int stop_point = 100;
	}
	if (if_x == if_y) {
		int stop_point = 100;
	} else {
		int stop_point = 100;
	}

	/*SwitchCase*/
	int switch_x = 100;
	int switch_y = 80;
	switch(switch_x)
	{
	case 1:
	{
		int stop_point = 100;
		break;
	}
	case 10:
	{
		int stop_point = 100;
		break;
	}
	case 100:
	{
		int stop_point = 100;
		break;
	}
	default:
	{
		int stop_point = 100;
		break;
	}
	}
	switch(switch_y)
	{
	case 1:
	{
		int stop_point = 100;
		break;
	}
	case 10:
	{
		int stop_point = 100;
		break;
	}
	case 100:
	{
		int stop_point = 100;
		break;
	}
	default:
	{
		int stop_point = 100;
		break;
	}
	}

	/*For test*/
	int iloop;
	for (iloop = 0; iloop < 20; iloop++) {
		if (iloop < 5) {
			int stop_point = 100;
		} else if (iloop < 10) {
			int stop_point = 100;
		} else if (iloop < 15) {
			int stop_point = 100;
		} else {
			int stop_point = 100;
		}
	}

	int stop_here_2 = 100;

	debug_step_01();
	debug_step_02();
	debug_step_03(1);
	debug_step_04(1, 2);

	return 0;
}

/*Data type test*/
int debug_datatype()
{
	/*Base*/
	char base_char_1 = 1;
	unsigned char base_uchar_1 = 2;
	short base_short_1 = 1;
	unsigned char base_ushort_1 = 2;
	int base_int_1 = 1;
	unsigned int base_uint_1 = 2;
	long base_long_1 = 1;
	unsigned long base_ulong_1 = 2;

	/*Struct*/
	struct st_in_1 {
		int x;
		int y;
	};

	struct st_in_1 st_in_1_data_1 = {4, 5};

	struct {
		int x;
		int y;
	} st_in_2_data_1 = {6, 7};

	struct {
		int x;
		int y;
	} *st_in_3_p_data_1 = NULL;

	/*Union*/
	union un_in_1 {
		char m_char_1;
		int m_int_2;
	};

	union un_in_1 un_in_1_data_1 = {0};

	union {
		int x;
		int y;
	} un_in_2_data_1 = {0};

	union {
		int x;
		int y;
	} *un_in_3_p_data_1 = NULL;

	/*Enum*/
	enum en_in_1 {
		EN_IN_1_1 = 4,
		EN_IN_1_2 = 5,
		EN_IN_1_3 = 5,
	};

	enum en_in_1 en_in_1_data_1 = EN_IN_1_3;
	enum en_in_1 en_in_1_data_2 = 9;

	enum {
		EN_IN_2_1 = 4,
		EN_IN_2_2 = 5,
	} en_in_2_data_1 = EN_IN_2_2;

	enum {
		EN_IN_3_1 = 4,
		EN_IN_3_2 = 5,
	} *en_in_3_p_data_1 = NULL;

	/*Point*/
	int int_tmp = 0;
	int *p_int_1 = (int *)0x12345678;
	int *p_int_2 = &int_tmp;
	int **pp_int = &p_int_2;
	struct st_in_1 *p_st_1 = (struct st_in_1 *)0x12345678;
	struct st_in_1 *p_st_2 = &st_in_1_data_1;
	struct st_in_1 **pp_st = &p_st_2;
	union un_in_1 *p_un_1 = (union un_in_1 *)0x12345678;
	union un_in_1 *p_un_2 = &un_in_1_data_1;
	union un_in_1 **pp_un = &p_un_2;
	enum en_in_1 *p_en_1 = (enum en_in_1 *)0x12345678;
	enum en_in_1 *p_en_2 = &en_in_1_data_1;
	enum en_in_1 **pp_en = &p_en_2;
	void *p_void_1 = (void *)0x12345678;
	void *p_void_2 = NULL;
	void *p_void_3 = &int_tmp;

	/*Array*/
	int a_int_1[4] = {0};
	int a_int_2[4] = {0, 1, 2, 3};
	int aa_int_1[4][5] = {0};
	int aa_int_2[4][5] = {{0, 1, 2, 3, 4},
			              {0, 1, 2, 3, 4},
		                  {0, 1, 2, 3, 4},
		                  {0, 1, 2, 3, 4}};

	struct st_in_1 a_st_1[4] = {0};
	struct st_in_1 a_st_2[4] = {{0}, {0}, {0}, {0}};
	struct st_in_1 a_st_3[4] = {{1,2}, {1,2}, {1,2}, {1,2}};
	struct st_in_1 aa_st_1[5][6] = {0};
	struct st_in_1 aa_st_2[5][6] = {{{0}, {0}, {0}, {0}, {0}, {0}},
									{{0}, {0}, {0}, {0}, {0}, {0}},
									{{0}, {0}, {0}, {0}, {0}, {0}},
									{{0}, {0}, {0}, {0}, {0}, {0}},
									{{0}, {0}, {0}, {0}, {0}, {0}}};
	struct st_in_1 aa_st_3[5][6] = {{{1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2}},
									{{1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2}},
									{{1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2}},
									{{1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2}},
									{{1,2}, {1,2}, {1,2}, {1,2}, {1,2}, {1,2}}};

	union un_in_1 a_un_1[4] = {0};
	union un_in_1 a_un_2[4] = {{0}, {0}, {0}, {0}};
	union un_in_1 aa_un_1[5][6] = {0};
	union un_in_1 aa_un_2[5][6] = {{{0}, {0}, {0}, {0}, {0}, {0}},
								   {{0}, {0}, {0}, {0}, {0}, {0}},
								   {{0}, {0}, {0}, {0}, {0}, {0}},
								   {{0}, {0}, {0}, {0}, {0}, {0}},
								   {{0}, {0}, {0}, {0}, {0}, {0}}};

	enum en_in_1 a_en_1[4] = {0};
	enum en_in_1 a_en_2[4] = {EN_IN_1_1, EN_IN_1_1, EN_IN_1_1, EN_IN_1_1};
	enum en_in_1 aa_en_1[5][6] = {0};
	enum en_in_1 aa_en_2[5][6] = {
			{EN_IN_1_1, EN_IN_1_1, EN_IN_1_1, EN_IN_1_1, EN_IN_1_1, EN_IN_1_1},
			{EN_IN_1_1, EN_IN_1_1, EN_IN_1_1, EN_IN_1_1, EN_IN_1_1, EN_IN_1_1},
			{EN_IN_1_1, EN_IN_1_1, EN_IN_1_1, EN_IN_1_1, EN_IN_1_1, EN_IN_1_1},
			{EN_IN_1_1, EN_IN_1_1, EN_IN_1_1, EN_IN_1_1, EN_IN_1_1, EN_IN_1_1},
			{EN_IN_1_1, EN_IN_1_1, EN_IN_1_1, EN_IN_1_1, EN_IN_1_1, EN_IN_1_1}};

	/*Function*/
	int (*p_f_char_1)(char, char) = NULL;
	int (*p_f_char_2)(char, char) = (int (*)(char, char))0x12345678;
	int (*p_f_int_1)(int, int) = NULL;
	int (*p_f_int_2)(int, int) = (int (*)(int, int))0x12345678;

	struct st_in_1 (*p_f_st_1)(struct st_in_1, struct st_in_1) = NULL;
	struct st_in_1 (*p_f_st_2)(struct st_in_1, struct st_in_1) = \
			(struct st_in_1 (*)(struct st_in_1, struct st_in_1))0x12345678;
	struct st_in_1 *(*p_f_st_3)(struct st_in_1 *, struct st_in_1 *) = NULL;
	struct st_in_1 *(*p_f_st_4)(struct st_in_1 *, struct st_in_1 *) = \
			(struct st_in_1 *(*)(struct st_in_1 *, struct st_in_1 *))0x12345678;

	union un_in_1 (*p_f_un_1)(union un_in_1, union un_in_1) = NULL;
	union un_in_1 (*p_f_un_2)(union un_in_1, union un_in_1) = \
			(union un_in_1 (*)(union un_in_1, union un_in_1))0x12345678;
	union un_in_1 *(*p_f_un_3)(union un_in_1 *, union un_in_1 *) = NULL;
	union un_in_1 *(*p_f_un_4)(union un_in_1 *, union un_in_1 *) = \
			(union un_in_1 *(*)(union un_in_1 *, union un_in_1 *))0x12345678;

	/*Typedef*/
	typedef unsigned char typedef_char;
	typedef unsigned int typedef_int;
	typedef struct st_in_1 typedef_st;

	typedef_char typedef_char_data_1 = 1;
	typedef_int typedef_int_data_1 = 2;
	typedef_st typedef_st_data_1 = {0};

	/*Typedef typedef*/
	typedef typedef_char typedef_typedef_char;
	typedef typedef_int typedef_typedef_int;
	typedef typedef_st typedef_typedef_st;

	typedef_typedef_char typedef_typedef_char_data_1 = 1;
	typedef_typedef_int typedef_typedef_int_data_1 = 2;
	typedef_typedef_st typedef_typedef_st_data_1 = {0};

	int stop_here = 100;
	return 0;
}

static char ____dump_____ = 0;

/*Dump arguments*/
int dump_args(int argc, char *argv[])
{
	/*Dump arguments first*/
	printf("=========Dump arguments=========\n");
	int iloop;
	for (iloop = 0; iloop < argc; iloop++) {
		char *arg = argv[iloop];
		printf("arg %d: %s\n", iloop, arg);
	}
	printf("=========Dump end================\n");

	return 0;
}

/*String test*/
int dump_string()
{
	char *dump_str = "my_string";
	return 0;
}

int dump_main_malloc_free()
{
	printf("Pid = %d\n", getpid());

	extern int main(int argc, char *argv[]);
	printf("Main address 0x%08x\n", (unsigned int)main);

    void *handle = dlopen("/lib/i386-linux-gnu/libc-2.19.so", RTLD_NOW);
    if (handle == NULL) {
    	printf("dlopen failed, errno info %s\n", dlerror());
    	return -1;
    }

    void *malloc_addr = dlsym(handle, "malloc");
    if (malloc_addr == 0) {
    	printf("find symbol malloc failed\n");
    	return -1;
    }
    printf("Malloc_addr = 0x%08x\n", (unsigned int)malloc_addr);

    void *free_addr = dlsym(handle, "free");
    if (free_addr == 0) {
    	printf("find symbol free failed\n");
    	return -1;
    }
    printf("Free_addr = 0x%08x\n", (unsigned int)free_addr);

    dlclose(handle);
    return 0;
}

int test_std_out_err()
{
	int iloop;
	for (iloop = 0; iloop < 15; iloop++) {
		fprintf(stdout,"Stdout test\n");
	}
	for (iloop = 0; iloop < 15; iloop++) {
		fprintf(stderr,"Stderr test\n");
	}
	return 0;
}

int main(int argc, char *argv[])
{
	setvbuf(stdout ,NULL,_IONBF, 0);

	dyn_test();

	call_file_1();

	dump_args(argc, argv);			/*Dump arguments*/

	dump_main_malloc_free();		/*Malloc free address*/

	dump_string();					/*String*/

	test_std_out_err();

	debug_datatype();				/*Data type*/

	stack_so_deep();				/*Deep stack*/

	debug_step();					/*Step test*/

#if 1
	debug_loop();					/*Loop test*/
#endif

	call_to_debug();				/*In other file*/

	test_stub_without_call();		/*Stub test*/

#if 1
	while(1) {
		sleep(1);
	}
#endif

	/*Thread start*/
	pthread_t pid;
	pthread_create(&pid, NULL, new_thread_1, NULL);
	pthread_create(&pid, NULL, new_thread_2, NULL);

	/*Keep running*/
	int iloop;
	for (iloop = 0; ; iloop++) {
		printf("Main thread never return loop %d\n", iloop);
		sleep(1);
	}

    return 0;
}
