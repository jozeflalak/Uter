
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

#ifndef SRC_TARGET_DWARF_LOC_H_
#define SRC_TARGET_DWARF_LOC_H_

/* Dwarf location description
 * All variable save in memory, the memory address description by DwfLoc*/
class DwfLoc {
public:
	enum DwfLocType {
		LOC_ADDR = 0,				/*Location is a address, for example global variable*/
		LOC_REG,					/*Location is register value*/
		LOC_REG_OFF,				/*Location is register value and offset*/
		LOC_CFA_OFF,				/*Location is CFA value and offset, CFA get from some real register*/
		LOC_NO_LOC,					/*No valid location*/
	};
	/* UTER_REG_xxxx_VAL is special register, not a real register, but can be convert from real register.
	 * Consumer can define the special register number, but the number must be bigger than all real register.
	 * the UTER_REG_CFA_VAL meaningful in function or function block, it is point to return address, consumer
	 * can be make value from other register in register table, for example: esp ebp.*/
	enum SpecialRegister {
		REG_TABLE_SIZE = 128,
		REG_CFA_VAL = REG_TABLE_SIZE + 1,		/*129*/
		REG_SAME_VAL = REG_TABLE_SIZE + 2,		/*130*/
		REG_UNDEF_VAL = REG_TABLE_SIZE + 3,		/*131*/
	};
	DwfLoc()
	{
		m_loc_type = LOC_NO_LOC;
		m_reg_num = 0;
		m_addr = 0;
		m_offset = 0;
		return;
	}
	/*Load location from dwarf*/
	int loadFromLoc(Dwarf_Debug dbg, Dwarf_Loc *loc);
	int loadFromAttr(Dwarf_Debug dbg, Dwarf_Attribute attr);
	DwfLocType m_loc_type;						/*Location type*/
	/*Location in register and offset, alive in DWF_LOC_REG, DWF_LOC_REG_OFF, DWF_LOC_DFA_OFF*/
	int m_reg_num;
	long long m_offset;
	unsigned long long m_addr;					/*Location is this address, alive in DWF_LOC_ADDR*/
};

/*Register table*/
class DwfRegTable {
public:
	class Item {
	public:
		Item(unsigned int regidx)
		{
			m_reg_index = regidx;

			m_isexpress = false;
			m_express_len = 0;

			m_reg_num = 0;
			m_offset = 0;
			return;
		}
		unsigned int m_reg_index;						/*Register number*/

		bool m_isexpress;								/*Register value is dwarf express, most type of register is value by register*/
		unsigned long long m_express_len;				/*Express length*/
		DwfLoc m_express_loc;							/*Express in location*/

		unsigned int m_reg_num;							/*Register value from other register value*/
		int m_offset;									/*Offset of destination register value*/
	};
	~DwfRegTable()
	{
		QMap<unsigned int, Item*>::iterator iter;
		for (iter = m_regs.begin(); iter != m_regs.end(); /*none*/) {
			Item *entry = iter.value();
			delete entry;
			m_regs.erase(iter++);
		}
		return;
	}
	Item *getItemById(unsigned int regnum)					/*Get item by register number*/
	{
		QMap<unsigned int, Item*>::iterator iter;
		iter = m_regs.find(regnum);
		if (iter == m_regs.end()) {
			return NULL;
		}
		return iter.value();
	}
	DwfAddrRange m_addr_range;								/*Register table address range*/
	QMap<unsigned int, Item*> m_regs;						/*Register table items*/
};

class DwfFrameCie {
public:
	DwfFrameCie()
	{
		m_index = 0;
		m_cie = NULL;
		return;
	}
	long long m_index;
	Dwarf_Cie m_cie;
};

/*Frame information for function*/
class DwfFrameFde {
public:
	DwfFrameFde(Dwarf_Debug dbg, Dwarf_Fde fde, DwfFrameCie *dwfcie,
	       unsigned long long lowpc, unsigned long long length)
	:m_addrange(lowpc, length)
	{
		m_dbg = dbg;
		m_fde = fde;
		m_dwfcie = dwfcie;
		m_loaded = false;
		return;
	}
	~DwfFrameFde()
	{
		QList<DwfRegTable*>::iterator iter;
		for (iter = m_regtab_list.begin(); iter != m_regtab_list.end(); /*none*/) {
			DwfRegTable *regtab = *iter;
			delete regtab;
			m_regtab_list.erase(iter++);
		}
	}
	Dwarf_Debug m_dbg;											/*Module dwarf master*/
	Dwarf_Fde m_fde;											/*Dwarf fde, use for load register table*/
	DwfFrameCie *m_dwfcie;										/*Cie for Fde*/
	DwfAddrRange m_addrange;									/*Fde address range*/
	DwfRegTable *getRegTabByAddrIn(unsigned long long addr);	/*Get register table by address in*/
	int loadRegTab();											/*Load register table now*/
private:
	DwfRegTable::Item *makeRegItem(unsigned int regidx, Dwarf_Regtable_Entry3_s *entry);
	bool m_loaded;								/*Fde loaded flag, because register table too much*/
	QList<DwfRegTable*> m_regtab_list;			/*Register table list*/
};

int dwf_init_dbg_for_loc(Dwarf_Debug dbg);

#endif /*SRC_TARGET_DWARF_LOC_H_*/
