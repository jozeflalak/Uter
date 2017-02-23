
#include <stdio.h>
#include "so_release.h"

int fun_in_release_so(int a, int b)
{
	return a + b;
}

int var_in_release_so = 100;

static char ____datatype_base_____ = 0;

char char_in_release_so = -3;
unsigned char uchar_in_release_so = 3;
short short_in_release_so = -3;
unsigned short ushort_in_release_so = 3;
int int_in_release_so = -3;
unsigned int uint_in_release_so = 3;
long long_in_release_so = -3;
unsigned long ulong_in_release_so = 3;
long long longlong_in_release_so = -3;
unsigned long long ulonglong_in_release_so = 3;

static char ____datatype_struct_____ = 0;

struct {
	int m_1;
	char m_2;
} g_noname_st_in_release_so = {.m_1 = 3, .m_2 = 5};
struct st_in_release_so {
	int m_1;
	char m_2;
};
struct st_in_release_so g_st_in_release_so = {.m_1 = 3, .m_2 = 5};

static char ____datatype_union_____ = 0;

union {
	int m_1;
	char m_2;
} g_noname_un_in_release_so = {.m_1 = 0x12345678};
union un_in_release_so {
	int m_1;
	char m_2;
};
union un_in_release_so g_un_in_release_so = {.m_1 = 0x12345678};

static char ____datatype_enum_____ = 0;

enum {
	M_A = 1,
	M_B,
	M_C,
} g_noname_en_in_release_so = M_B;
enum en_in_release_so {
	M_1 = 1,
	M_2,
	M_3,
};
enum en_in_release_so g_en_in_release_so = M_2;

static char ____datatype_typedef_____ = 0;

typedef unsigned int typedef_int_in_release_so;
typedef_int_in_release_so g_typedef_int_in_release_so = 100;

typedef struct st_in_release_so typedef_st_in_release_so;
typedef_st_in_release_so g_typedef_st_in_release_so = {.m_1 = 5, .m_2 = 7};

static char ____datatype_point_____ = 0;

int use_for_int_point = 199;
int *g_int_point_in_release_so = &use_for_int_point;

int *g_char_point_in_release_so = (int *)0x12345678;

int *g_short_point_in_release_so = NULL;

int *g_void_point_in_release_so = NULL;

struct st_in_release_so *g_st_point_in_release_so = &g_st_in_release_so;
struct st_in_release_so **g_st_ppoint_in_release_so = &g_st_point_in_release_so;

static char ____datatype_array_____ = 0;

int array_int_1_in_release_so[2] = {};
int array_int_2_in_release_so[3][2] = {};

struct st_for_arry {
	int m_1;
	int m_2;
};
struct st_for_arry array_st_1_in_release_so[2] = {};
struct st_for_arry array_st_2_in_release_so[3][2] = {};

static char ____function_____ = 0;

int fun_0_in_release_so()
{
	return 20;
}

int fun_1_in_release_so(int x)
{
	return 20 + x;
}

int fun_2_in_release_so(int x, int y)
{
	return 20 + x + y;
}

int fun_3_in_release_so(int x, int y, int z)
{
	return 20 + x + y + z;
}

void fun_v_in_release_so()
{
	return;
}

int fun_p_in_release_so(int *x, int *y)
{
	return 20 + *x + *y;
}

int fun_pp_in_release_so(int **x, int **y)
{
	return 20 + **x + **y;
}

int fun_a_int_release_so(int x[], int y[])
{
	return 20 + x[0] + y[0];
}

int fun_aa_int_release_so(int x[][5], int y[][5])
{
	return 20 + x[0][0] + y[0][0];
}

struct st_arg {
	int x;
	int y;
};

int fun_st_in_release_so(struct st_arg arg)
{
	return 20 + arg.x + arg.y;
}

int fun_stp_in_release_so(struct st_arg *arg)
{
	return 20 + arg->x + arg->y;
}

int fun_stub_in_release_so(int x, int y)
{
	return x + y;
}

int fun_call_in_release_so(int x, int y)
{
	return fun_stub_in_release_so(x, y);
}
