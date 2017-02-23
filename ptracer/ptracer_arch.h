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

#ifndef SRC__PTRACER_PTRACER_ARCH_H_
#define SRC__PTRACER_PTRACER_ARCH_H_

class PtracerBreak;
class PtracerArch {
public:
	virtual ~PtracerArch()
	{
		return;
	}
    virtual int memBytesGet(unsigned int pid, unsigned long long addr, unsigned char *arry, int num) = 0;
    virtual int memBytesSet(unsigned int pid, unsigned long long addr, unsigned char *arry, int num) = 0;
    virtual int regWordGet(unsigned int pid, const char *regname, unsigned long long *val) = 0;
    virtual int regWordSet(unsigned int pid, const char *regname, unsigned long long val) = 0;
    virtual int brkpntSet(unsigned int pid, PtracerBreak *brkpnt) = 0;
    virtual int brkpntClear(unsigned int pid, PtracerBreak *brkpnt) = 0;
    virtual int instrAddrGet(unsigned int pid, unsigned long long *addr) = 0;
    virtual int stackAddrGet(unsigned int pid, unsigned long long *addr) = 0;
    virtual int breakAdjust(unsigned int pid, unsigned long long *instr_now) = 0;
};

#endif /* SRC__PTRACER_PTRACER_ARCH_H_ */
