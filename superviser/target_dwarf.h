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

#ifndef SRC_TARGET_DWARF_H_
#define SRC_TARGET_DWARF_H_

#include <dwarf.h>
#include <libdwarf.h>

#include <target_dwarf_public.h>
#include <target_dwarf_loc.h>
#include <target_dwarf_data.h>
#include <target_dwarf_macro.h>

/* Local variables information in function block, local variable environment is
 * alive in the block and exclude its sub block， if not in this area，
 * the environment is invalid*/
class DwfLocalVars {
public:
	class Item {
	public:
		Item()
		{
			m_type = NULL;
			m_name = "";
			m_fileno = 0;
			m_lineno = 0;
			m_file = "";
			return;
		}
		DwfType *m_type;				/*Variable type(dwarf type)*/
		QString m_name;					/*Variable name*/
		DwfLoc m_loc;					/*Variable location*/
		unsigned int m_fileno;			/*Variable at file*/
		unsigned int m_lineno;			/*Variable at line*/
		QString m_file;					/*Variable at file path*/
	};
	~DwfLocalVars()
	{
		QMap<QString, Item*>::iterator iter;
		for (iter = m_var_map.begin(); iter != m_var_map.end(); /**/) {
			Item *var = iter.value();
			delete var;
			m_var_map.erase(iter++);
		}
		excludeClear();
		return;
	}
	/*TODO:每当进入一个内层块的时候，已有的排除范围已经不再使用，需要清除*/
	int excludeClear()
	{
		QList<DwfAddrRange*>::iterator iter;
		for (iter = m_exclude_list.begin(); iter != m_exclude_list.end(); /*none*/) {
			DwfAddrRange *range = *iter;
			delete range;
			m_exclude_list.erase(iter++);
		}
		return 0;
	}
	/*TODO:增加新排除*/
	int excludeAdd(unsigned long long low, unsigned long long high)
	{
		DwfAddrRange *range = new DwfAddrRange(low, high);
		m_exclude_list.push_back(range);
		return 0;
	}
	/*TODO:检查地址是否在本集合适用的范围内*/
	bool checkAddr(unsigned long long addr)
	{
		/*首先需要处于适用范围中*/
		if (m_range.check(addr)) {
			/*然后必须不在排除范围内*/
			QList<DwfAddrRange*>::iterator iter;
			for (iter = m_exclude_list.begin(); iter != m_exclude_list.end(); iter++) {
				DwfAddrRange *range = *iter;
				if (range->check(addr)) {
					return false;
				}
			}
			return true;
		}
		return false;
	}
	QMap<QString, Item*> m_var_map;						/*Variable name to DwfLocalVar*/
	DwfAddrRange m_range;								/*Block address range*/
	QList<DwfAddrRange*> m_exclude_list;				/*Exclude sub address range*/
};

/*Dwarf source information*/
class DwfLine {
public:
	DwfLine()
	{
		m_lineaddr = 0;
		m_fileno = 0;
		m_lineno = 0;
		m_offno = 0;

		m_isbegin = false;
		m_isend = false;
		m_isblock = false;
		m_isaddr_set = false;
		return;
	}
	unsigned long long m_lineaddr;			/*Statement address*/
	unsigned int m_fileno;					/*Statement file number, begin from 1*/
	unsigned int m_lineno;					/*Statement line number*/
	unsigned int m_offno;					/*Statement line off*/
	bool m_isbegin;
	bool m_isend;
	bool m_isblock;
	bool m_isaddr_set;
};

/*Dwarf compilation unit*/
class DwfCu {
public:
	enum DwfLanguage {
		LANGUAGE_C89 = 0,
		LANGUAGE_C99,
		LANGUAGE_C,
		LANGUAGE_CPP,
	};
	DwfCu(Dwarf_Debug dbg)
	{
		UTER_ASSERT(dbg != NULL);
		m_dbg = dbg;

		m_offset = DWF_INVALID_OFF;
		m_producer = "";
		m_lang = LANGUAGE_C89;

		m_dir = "";
		m_name = "";
		m_path = "";

		m_in_line_off = DWF_INVALID_OFF;
		m_in_macro_off = DWF_INVALID_OFF;

		m_macro = NULL;
		return;
	}
	~DwfCu()
	{
		QList<DwfVar*>::iterator var_iter;
		for (var_iter = m_vars.begin(); var_iter != m_vars.end(); /*none*/) {
			DwfVar *dwfvar = *var_iter;
			delete dwfvar;
			m_vars.erase(var_iter++);
		}
		QList<DwfFun*>::iterator fun_iter;
		for (fun_iter = m_funs.begin(); fun_iter != m_funs.end(); /*none*/) {
			DwfFun *dwffun = *fun_iter;
			delete dwffun;
			m_funs.erase(fun_iter++);
		}
		QMap<unsigned long long, DwfItem*>::iterator item_iter;
		for (item_iter = m_elements.begin(); item_iter != m_elements.end(); /*none*/) {
			DwfItem *dwfitem = item_iter.value();
			delete dwfitem;
			m_elements.erase(item_iter++);
		}
		QList<DwfLine*>::iterator line_iter;
		for (line_iter = m_dwf_line_list.begin(); line_iter != m_dwf_line_list.end(); /*none*/) {
			DwfLine *dwfline = *line_iter;
			delete dwfline;
			m_dwf_line_list.erase(line_iter++);
		}
		if (m_macro != NULL) {
			delete m_macro;
		}
		return;
	}
private:
	/*Get attribute from Dwarf die*/
	bool getDieHasAttr(Dwarf_Die some_die, Dwarf_Half some_attr);
	int getDieStrAttr(Dwarf_Die some_die, Dwarf_Half some_strattr, QString &out);
	int getDieRefAttr(Dwarf_Die some_die, Dwarf_Half some_refattr, unsigned long long &out);
	int getDieUdataAttr(Dwarf_Die some_die, Dwarf_Half some_udataattr, unsigned long long &out);
	QString getDieName(Dwarf_Die named_die);
	Dwarf_Off getDieOff(Dwarf_Die offd_die);
	unsigned int getDieSize(Dwarf_Die sized_die);
	unsigned long long getDieLowPc(Dwarf_Die lowpcd_die);
	unsigned long long getDieHighPc(Dwarf_Die highpcd_die);
	unsigned long long getDieTypeOn(Dwarf_Die typed_die);
	unsigned long long getDieFileNo(Dwarf_Die nod_die);
	unsigned long long getDieLineNo(Dwarf_Die nod_die);
	bool getDieIsExtern(Dwarf_Die exted_die);
	bool getDieIsDeclare(Dwarf_Die declared_die);
	/*Convert die to data type*/
	DwfTypeBase *parseTypeBase(Dwarf_Die type_die);
	DwfTypeEnMem *parseTypeEnMem(Dwarf_Die type_die);
	DwfTypeEn *parseTypeEn(Dwarf_Die type_die);
	DwfTypeStMem *parseTypeStMem(Dwarf_Die type_die);
	DwfTypeSt *parseTypeSt(Dwarf_Die type_die);
	DwfTypeUnMem *parseTypeUnMem(Dwarf_Die type_die);
	DwfTypeUn *parseTypeUn(Dwarf_Die type_die);
	DwfTypeArray *parseTypeArray(Dwarf_Die type_die);
	DwfTypeSubrtPara *parseTypeSubrtPara(Dwarf_Die type_die);
	DwfTypeSubrt *parseTypeSubrt(Dwarf_Die type_die);
	DwfTypePoint *parseTypePoint(Dwarf_Die type_die);
	DwfTypeTypedef *parseTypeTypedef(Dwarf_Die type_die);
public:
	/*Convert offset to data type ...*/
	DwfType *getTypeOnOff(unsigned long long die_off);
	/*High-level:get offset from DW_AT_type, convert offset to data type*/
	DwfType *getTypeForDie(Dwarf_Die typed_die);
	DwfType *parseType(Dwarf_Die type_die);
	DwfPara *parseFunPara(DwfFun *fun, Dwarf_Die para_die);
	int parseFunBlock(Dwarf_Die block_die);				/*Called in parseFun*/
	DwfFun *parseFun(Dwarf_Die fun_die);
	DwfVar *parseVar(Dwarf_Die var_die);
	int parse(Dwarf_Die cu_die);
	/*Base information of compilation unit*/
	Dwarf_Debug m_dbg;									/*Dwarf master, alive on module attach*/
	unsigned long long m_offset;						/*Compilation unit offset in module dwarf master*/
	QString m_producer;									/*Compilation command*/
	DwfLanguage m_lang;									/*Program language*/
	QString m_dir;										/*Source file directory*/
	QString m_name;										/*Source file name*/
	QString m_path;										/*Source file path*/
	DwfAddrRange m_addr_range;							/*Compilation unit data range*/
	/* Some DwfItem save in m_elements, for example DwfTypeSt, release it by Cu.
	 * Some DwfItem no save in m_elements, for example DwfTypeStMem, release it by DwfTypeSt.
	 * If some DwfItem set in m_elements, it may be referenced by other DwfItem, so need Cu release it.*/
	DwfItem *itemGetByOff(unsigned long long offset);
	int itemInsert(DwfItem *element);
	int itemRemove(DwfItem *element);
	QMap<unsigned long long, DwfItem*> m_elements;	/*Offset(global offset, offset in module) to Item*/
	QList<DwfType*> m_structs;						/*Element which type is global struct for export*/
	QList<DwfType*> m_unions;						/*Element which type is global union for export*/
	QList<DwfType*> m_enums;						/*Element which type is global enum for export*/
	QList<DwfType*> m_typedefs;						/*Element which type is global typedef for export*/
	QList<DwfVar*> m_vars;							/*Element which type is global variable for export*/
	QList<DwfFun*> m_funs;							/*Element which type is        function for export*/
	DwfFun *getFunByAddr(unsigned long long addr);
	DwfFun *getFunByAddrIn(unsigned long long addr);
	/*Get local variables list on the address.*/
	int getLocalVarsInBlock(unsigned long long addr, Dwarf_Die blockdie, DwfLocalVars *varset);
	DwfLocalVars *getLocalVars(unsigned long long addr);
	/*Function coverage*/
	FunCovInfo *funCovInfoGet(DwfFun *fun);
	int funCovOn(DwfFun *fun);
	int funCovOff(DwfFun *fun);
	int funCovReset(DwfFun *fun);
	int funCovCount(DwfFun *fun, unsigned int &hit, unsigned int &num);
	int funCovShow(DwfFun *fun, QString &out);
	/*Source file line information*/
	QString getFileNameByNo(unsigned int fileno);
	int getFileLineNoByAddrIn(unsigned long long addr, unsigned int &fileno, unsigned int &lineno);
	int getFileLineByAddrIn(unsigned long long addr, QString &filename, unsigned int &lineno);
	int getFileLineNoByAddr(unsigned long long addr, unsigned int &fileno, unsigned int &lineno);
	int parseLines(Dwarf_Die cu_die);
	QMap<QString, unsigned int> m_file_path_map;				/*File path to file number, file number begin from 1*/
	QMap<unsigned int, QString> m_file_no_map;					/*File number to file path, file number begin from 1*/
	QList<DwfLine*> m_dwf_line_list;							/*All source statement information in Cu*/
	QMap<unsigned long long, DwfLine*> m_dwf_line_map;			/*TODO:*/

	/*TODO:关于line信息和macro信息，这里还有待调整*/
	unsigned long long m_in_line_off;		/*TODO:line信息偏移*/
	unsigned long long m_in_macro_off;		/*TODO:macro信息偏移*/
	dwf_macro *m_macro;						/*TODO:宏信息*/
};

#endif /* SRC_TARGET_DWARF_H_ */
