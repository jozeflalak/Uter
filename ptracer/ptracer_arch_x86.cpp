/******************************************************************************
 * Copyright(C),2015-2015, Yaozhongkai
 * FileName:
 * Author:Yaozhongkai
 * Version:
 * Date:
 * Description:
 * Others:
 * Function List:
 * History:
 * ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ptrace.h>
#include <sys/user.h>

#include <inter_msg.h>
#include <ptracer_pub.h>
#include <ptracer.h>
#include <ptracer_arch_x86.h>

struct {
	const char *reg_name;
	int offset;
	int size;
} static x86_regs[] = {
	{"ebx", offsetof(uter_user_regs_struct_x86, ebx), sizeof(long int)},
	{"ecx", offsetof(uter_user_regs_struct_x86, ecx), sizeof(long int)},
	{"edx", offsetof(uter_user_regs_struct_x86, edx), sizeof(long int)},
	{"esi", offsetof(uter_user_regs_struct_x86, esi), sizeof(long int)},
	{"edi", offsetof(uter_user_regs_struct_x86, edi), sizeof(long int)},
	{"ebp", offsetof(uter_user_regs_struct_x86, ebp), sizeof(long int)},
	{"eax", offsetof(uter_user_regs_struct_x86, eax), sizeof(long int)},
	{"xds", offsetof(uter_user_regs_struct_x86, xds), sizeof(long int)},
	{"xes", offsetof(uter_user_regs_struct_x86, xes), sizeof(long int)},
	{"xfs", offsetof(uter_user_regs_struct_x86, xfs), sizeof(long int)},
	{"xgs", offsetof(uter_user_regs_struct_x86, xgs), sizeof(long int)},
	{"orig_eax", offsetof(uter_user_regs_struct_x86, orig_eax), sizeof(long int)},
	{"eip", offsetof(uter_user_regs_struct_x86, eip), sizeof(long int)},
	{"xcs", offsetof(uter_user_regs_struct_x86, xcs), sizeof(long int)},
	{"eflags", offsetof(uter_user_regs_struct_x86, eflags), sizeof(long int)},
	{"esp", offsetof(uter_user_regs_struct_x86, esp), sizeof(long int)},
	{"xss", offsetof(uter_user_regs_struct_x86, xss), sizeof(long int)},
};

static int find_off(const char *name)
{
	PTRACER_ASSERT(name != NULL);

	int iloop;
	for (iloop = 0; iloop < sizeof(x86_regs)/sizeof(x86_regs[0]); iloop++) {
		if (strcasecmp(name, x86_regs[iloop].reg_name) == 0) {
			return iloop;
		}
	}

	return -1;
}

int PtracerArchX86::memBytesGet(unsigned int pid, unsigned long long addr, unsigned char *arry, int num)
{
	PTRACER_ASSERT(arry != 0);
	PTRACER_DEBUG("Get memory request at 0x%016llx on pid %d with length %d.", addr, pid, num);

	union {
		long ld;
		char cd[4];
	} data;

	int iloop;
	if (num == 1 || num == 2 || num == 4) {				//1,2,4字节的情况
		errno = 0;
		data.ld = ptrace(PTRACE_PEEKDATA, pid, ldata_low(addr), 0);
		if (data.ld == -1 && errno != 0) {
			PTRACER_LOG("Get memory failed at 0x%08x on pid %d, errno %d.", ldata_low(addr), pid, errno);
			return -1;
		}
		for (iloop = 0; iloop < num; iloop++) {
			arry[iloop] = data.cd[iloop];
		}
	} else if (num == 8) {								//8字节的情况
		errno = 0;
		data.ld = ptrace(PTRACE_PEEKDATA, pid, ldata_low(addr), 0);
		if (data.ld == -1 && errno != 0) {
			PTRACER_LOG("Get memory failed at 0x%08x on pid %d, errno %d.", ldata_low(addr), pid, errno);
			return -1;
		}
		for (iloop = 0; iloop < 4; iloop++) {
			arry[iloop] = data.cd[iloop];
		}

		errno = 0;
		data.ld = ptrace(PTRACE_PEEKDATA, pid, ldata_low(addr) + 4, 0);
		if (data.ld == -1 && errno != 0) {
			PTRACER_LOG("Get memory failed at 0x%08x on pid %d, errno %d.", ldata_low(addr) + 4, pid, errno);
			return -1;
		}
		for (iloop = 0; iloop < 4; iloop++) {
			arry[4 + iloop] = data.cd[iloop];
		}
	} else {
		PTRACER_LOG("Get memory request with wrong length %d.", num);
		return -1;
	}

	return 0;
}

int PtracerArchX86::memBytesSet(unsigned int pid, unsigned long long addr, unsigned char *arry, int num)
{
	PTRACER_ASSERT(arry != 0);
	PTRACER_DEBUG("Set memory request at 0x%016llx on pid %d with length %d.", addr, pid, num);

	union {
		long ld;
		char cd[4];
	} data;

	int iloop;
	long longret;
	if (num == 1 || num == 2 || num == 4) {				//1,2,4字节的情况
		errno = 0;
		data.ld = ptrace(PTRACE_PEEKDATA, pid, ldata_low(addr), 0);
		if (data.ld == -1 && errno != 0) {
			PTRACER_LOG("Get memory failed at 0x%08x on pid %d, errno %d.", ldata_low(addr), pid, errno);
			return -1;
		}
		for (iloop = 0; iloop < num; iloop++) {
			data.cd[iloop] = arry[iloop];
		}
		longret = ptrace(PTRACE_POKEDATA, pid, ldata_low(addr), data.ld);
		if (longret == -1) {
			PTRACER_LOG("Set memory failed at 0x%08x on pid %d, errno %d.", ldata_low(addr), pid, errno);
			return -1;
		}
	} else if (num == 8) {								//8字节的情况
		for (iloop = 0; iloop < 4; iloop++) {
			data.cd[iloop] = arry[iloop];
		}
		longret = ptrace(PTRACE_POKEDATA, pid, ldata_low(addr), data.ld);
		if (longret == -1) {
			PTRACER_LOG("Set memory failed at 0x%08x on pid %d, errno %d.", ldata_low(addr), pid, errno);
			return -1;
		}

		for (iloop = 0; iloop < 4; iloop++) {
			data.cd[iloop] = arry[4 + iloop];
		}
		longret = ptrace(PTRACE_POKEDATA, pid, ldata_low(addr) + 4, data.ld);
		if (longret == -1) {
			PTRACER_LOG("Set memory failed at 0x%08x on pid %d, errno %d.", ldata_low(addr) + 4, pid, errno);
			return -1;
		}
	} else {
		PTRACER_LOG("Set memory request with wrong length %d.", num);
		return -1;
	}

	return 0;
}

int PtracerArchX86::regWordGet(unsigned int pid, const char *regname, unsigned long long *val)
{
	PTRACER_ASSERT(regname != NULL);
	PTRACER_ASSERT(val != NULL);
	PTRACER_DEBUG("Get register request for %s on pid %d.", regname, pid);

	struct uter_user_regs_struct_x86 regset;

	int reg_offset = find_off(regname);
	if (reg_offset == -1) {
		PTRACER_LOG("Get register request for wrong register name %s.", regname);
		return -1;
	}

	long longret = ptrace(PTRACE_GETREGS, pid, 0, &regset);
	if (longret == -1) {
		PTRACER_LOG("Get register failed on pid %d, errno %d.", pid, errno);
		return -1;
	}

	long int reg_val = *((long int *)((char *)&regset + x86_regs[reg_offset].offset));
	*val = ldata(0, reg_val);

	return 0;
}

int PtracerArchX86::regWordSet(unsigned int pid, const char *regname, unsigned long long val)
{
	PTRACER_ASSERT(regname != NULL);
	PTRACER_DEBUG("Set register request for %s on pid %d.", regname, pid);

	struct uter_user_regs_struct_x86 regset;

	PTRACER_DEBUG("Set register %s to value 0x%016llx.", regname, val);

	int reg_offset = find_off(regname);
	if (reg_offset == -1) {
		PTRACER_LOG("Set register request for wrong register name %s.", regname);
		return -1;
	}

	long longret = ptrace(PTRACE_GETREGS, pid, 0, &regset);
	if (longret == -1) {
		PTRACER_LOG("Get register failed on pid %d, errno %d.", pid, errno);
		return -1;
	}

	*((long int *)((char *)&regset + x86_regs[reg_offset].offset)) = ldata_low(val);

	longret = ptrace(PTRACE_SETREGS, pid, 0, &regset);
	if (longret == -1) {
		PTRACER_LOG("Set register failed on pid %d, errno %d.", pid, errno);
		return -1;
	}

	return 0;
}

int PtracerArchX86::brkpntSet(unsigned int pid, PtracerBreak *brkpnt)
{
	PTRACER_ASSERT(brkpnt != NULL);
	PTRACER_DEBUG("Set break request at 0x%016llx on pid %d.", brkpnt->m_address, pid);

	union {
		long ld;
		char cd[4];
	} data;

	//读取内存
	errno = 0;
	data.ld = ptrace(PTRACE_PEEKDATA, pid, ldata_low(brkpnt->m_address), 0);		//x86只取地址低位
	if (data.ld == -1 && errno != 0) {
		PTRACER_LOG("Get memory failed at 0x%08x on pid %d, errno %d.", ldata_low(brkpnt->m_address), pid, errno);
		return -1;
	}

	//备份一个字节
	brkpnt->m_backup[0] = data.cd[0];

	//修改一个字节，是断点指令
	data.cd[0] = 0xcc;

	//写回内存
	long longret = ptrace(PTRACE_POKEDATA, pid, ldata_low(brkpnt->m_address), data.ld);	//x86只设置地址地位
	if (longret == -1) {
		PTRACER_LOG("Set memory failed at 0x%08x on pid %d, errno %d.", ldata_low(brkpnt->m_address), pid, errno);
		return -1;
	}

	return 0;
}

int PtracerArchX86::brkpntClear(unsigned int pid, PtracerBreak *brkpnt)
{
	PTRACER_ASSERT(brkpnt != NULL);
	PTRACER_DEBUG("Unset break request at 0x%016llx on pid %d.", brkpnt->m_address, pid);

	union {
		long ld;
		char cd[4];
	} data;

	//读取内存
	errno = 0;
	data.ld = ptrace(PTRACE_PEEKDATA, pid, ldata_low(brkpnt->m_address), 0);		//x86只取地址低位
	if (data.ld == -1 && errno != 0) {
		PTRACER_LOG("Get memory failed at 0x%08x on pid %d, errno %d.", ldata_low(brkpnt->m_address), pid, errno);
		return -1;
	}

	//恢复内容
	data.cd[0] = brkpnt->m_backup[0];

	//写回内存
	long longret = ptrace(PTRACE_POKEDATA, pid, ldata_low(brkpnt->m_address), data.ld);	//x86只设置地址地位
	if (longret == -1) {
		PTRACER_LOG("Set memory failed at 0x%08x on pid %d, errno %d.", ldata_low(brkpnt->m_address), pid, errno);
		return -1;
	}

	return 0;
}

//当前指令地址
int PtracerArchX86::instrAddrGet(unsigned int pid, unsigned long long *address)
{
	PTRACER_ASSERT(address != NULL);
	PTRACER_DEBUG("Get current instruction on pid %d.", pid);

	struct uter_user_regs_struct_x86 regset;
	long longret = ptrace(PTRACE_GETREGS, pid, 0, &regset);
	if (longret == -1) {
		PTRACER_LOG("Get register failed on pid %d, errno %d.", pid, errno);
		return -1;
	}

	*address = ldata(0, regset.eip);

	return 0;
}

int PtracerArchX86::stackAddrGet(unsigned int pid, unsigned long long *address)
{
	PTRACER_ASSERT(address != NULL);
	PTRACER_DEBUG("Get current stack on pid %d.", pid);

	struct uter_user_regs_struct_x86 regset;
	long longret = ptrace(PTRACE_GETREGS, pid, 0, &regset);
	if (longret == -1) {
		PTRACER_LOG("Get register failed on pid %d, errno %d.", pid, errno);
		return -1;
	}

	*address = ldata(0, regset.esp);
	return 0;
}

int PtracerArchX86::breakAdjust(unsigned int pid, unsigned long long *instr_now)
{
	PTRACER_ASSERT(instr_now != NULL);
	PTRACER_DEBUG("Adjust current instruction for break on pid %d.", pid);

	struct uter_user_regs_struct_x86 regset;
	long longret = ptrace(PTRACE_GETREGS, pid, 0, &regset);
	if (longret == -1) {
		PTRACER_LOG("Get register failed on pid %d, errno %d.", pid, errno);
		return -1;
	}

	regset.eip = regset.eip - 1;

	longret = ptrace(PTRACE_SETREGS, pid, 0, &regset);
	if (longret == -1) {
		PTRACER_LOG("Set register failed on pid %d, errno %d.", pid, errno);
		return -1;
	}

	*instr_now = (unsigned long)regset.eip;
	return 0;
}
