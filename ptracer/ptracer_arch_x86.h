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

#ifndef SRC_PTRACER_I386_H_
#define SRC_PTRACER_I386_H_

#include <ptracer_arch.h>

struct uter_user_regs_struct_x86
{
  long int ebx;
  long int ecx;
  long int edx;
  long int esi;
  long int edi;
  long int ebp;
  long int eax;
  long int xds;
  long int xes;
  long int xfs;
  long int xgs;
  long int orig_eax;
  long int eip;
  long int xcs;
  long int eflags;
  long int esp;
  long int xss;
};

class PtracerArchX86: public PtracerArch {
private:
    int memBytesGet(unsigned int pid, unsigned long long addr, unsigned char *arry, int num);
    int memBytesSet(unsigned int pid, unsigned long long addr, unsigned char *arry, int num);
    int regWordGet(unsigned int pid, const char *regname, unsigned long long *val);
    int regWordSet(unsigned int pid, const char *regname, unsigned long long val);
    int brkpntSet(unsigned int pid, PtracerBreak *brkpnt);
    int brkpntClear(unsigned int pid, PtracerBreak *brkpnt);
    int instrAddrGet(unsigned int pid, unsigned long long *address);
    int stackAddrGet(unsigned int pid, unsigned long long *address);
    int breakAdjust(unsigned int pid, unsigned long long *instr_now);
};

#endif /* SRC_PTRACER_I386_H_ */
