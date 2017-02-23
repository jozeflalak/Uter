/*
 * useful.c
 *
 *  Created on: 2015年6月29日
 *      Author: yaozhongkai
 */

#include <stdio.h>

//调用打桩充分验证函数功能
int fun_call_by_other(int a, int b)
{
	//do some thing

	return 1;	//return 1 work done, return 0 not ready, return -1 failed.
}

int fun_do_something_1(int x, int y)
{
	int result;

	//参数检查
	if (x < 0 || y < 0) {
		return -1;
	}

	result = fun_call_by_other(x, y);
	if (result == 1) {
		printf("Do something ok.\n");
	} else if (result == 0) {
		printf("Do something not ready.\n");
	} else {
		printf("Do something failed.\n");
	}

	return 0;
}

//传递链表，复杂指针操作
struct my_node {
	struct my_node *next;
	int value;
};

int fun_walk_list_1(struct my_node *input)
{
	int ret_val = 0;

	if (input == NULL) {
		return 0;
	}

	struct my_node *pos = input;
	while(pos != NULL) {
		printf("Input value:%d\n", pos->value);
		ret_val += pos->value;
		pos = pos->next;
	}

	return ret_val;
}

//传消息
#define my_msg_type_work (0x00000001)

struct my_msg_head {
	unsigned int type;
	unsigned int length;			//包含头长度
};

//如果my_msg_type_work，则再my_msg_head保存work data

int my_msg_proc_1(struct my_msg_head *msg)
{
	if (msg == NULL) {
		return -1;
	}

	if (msg->type == 0x00000001) {
		unsigned int length = msg->length - sizeof(struct my_msg_head);
		unsigned char *data = (unsigned char *)(msg + 1);
		printf("Work request with %d bytes:\n", length);
		int ret_val = 0;
		int iloop;
		for (iloop = 0; iloop < length; iloop++) {
			printf("%02x ", data[iloop]);
			ret_val += data[iloop];
		}
		printf("\n");
		return ret_val;
	} else {
		printf("Unsupport message type.\n");
		return -1;
	}

	return 0;
}
