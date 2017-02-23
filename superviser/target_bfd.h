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

#ifndef SRC_TARGET_BFD_H_
#define SRC_TARGET_BFD_H_

#define PACKAGE
#define PACKAGE_VERSION
#include <bfd.h>

#define STARTAT_STEP  (64)

/*Assembler statement*/
class BfdAsm {
public:
	BfdAsm()
	{
		m_offset = 0;
		m_octets = 0;
		m_asm_code = "";
		return;
	}
	unsigned long long m_offset;		/*Instruction at*/
	int m_octets;						/*Instruction length(Bytes)*/
	QString m_asm_code;					/*Assembler statement string*/
};

/*Assembler statement list*/
class BfdAsmSet {
public:
	BfdAsmSet()
	{
		return;
	}
	~BfdAsmSet()
	{
		QList<BfdAsm*>::iterator iter;
		for (iter = m_bfd_asm_list.begin(); iter != m_bfd_asm_list.end(); /*none*/) {
			BfdAsm *myasm = *iter;
			delete myasm;
			m_bfd_asm_list.erase(iter++);
		}
		return;
	}
	QList<BfdAsm*> m_bfd_asm_list;
};

/*Bfd section description*/
class TargetMod;
class BfdSec {
public:
	BfdSec()
	{
		m_mod = NULL;
		m_section = NULL;
		m_foff = 0;
		m_name = "";
		m_size = 0;
		m_vma = 0;
		m_lma = 0;

		m_sec_load = false;
		m_sec_code = false;
		m_data = NULL;

		m_load_at = 0;
		return;
	}
	BfdAsmSet *dasm(unsigned long long from, unsigned long long to);
	int flushCode();
	/*Setup following member when foreach bfd section*/
	TargetMod *m_mod;							/*Section in module, get bfd handle from m_mod->m_bfd_file*/
	asection *m_section;						/*Bfd section point*/
	unsigned long long m_foff;					/*Section offset in image file, m_foff equal m_vma in so*/
	QString m_name;								/*Section name*/
	unsigned long long m_size;					/*Section size*/
	unsigned long long m_vma;					/*Section vma*/
	unsigned long long m_lma;					/*Section lma*/
	bool m_sec_load;							/*Section loadable*/
	bool m_sec_code;							/*Section is code*/
	unsigned char *m_data;						/*Section code, alive when is code*/
	/*Setup following member when post foreach*/
	unsigned long long m_load_at;				/*Section load at in memory*/
	QMap<unsigned long long, unsigned long long> m_startat;	/*Instruction begin at*/
};

/*Bfd symbol, use for dynamic symbol and debug symbol*/
class BfdSym {
public:
	BfdSym()
	{
		m_section = "";
		m_name = "";
		m_addr = 0;
		m_flag = 0;
		return;
	}
	QString flagsDesc();
	QString m_section;						/*Section name*/
	QString m_name;							/*Symbol name*/
	unsigned long long m_addr;				/*Symbol address at*/
	unsigned int m_flag;					/*TODO:来自asymbol， 都是 BSF_XXX 格式*/
};

/*Bfd Reloc*/
class BbdReloc {
public:
	BbdReloc()
	{
		m_offset = 0;
		m_value = "";
		m_type = "";

		m_relocto_name = "";
		return;
	}
	unsigned long long m_offset;			/*TODO:*/
	QString m_value;						/*TODO:*/
	QString m_type;							/*TODO:*/
	QString m_relocto_name;					/*TODO:*/
};

#endif /* SRC_TARGET_BFD_H_ */
