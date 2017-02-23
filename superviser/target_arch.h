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

#ifndef SRC_TARGET_ARCH_H_
#define SRC_TARGET_ARCH_H_

class RegContext {
public:
	class Item {
	public:
		Item()
		{
			m_name = "";
			m_value = 0;
			return;
		}
		QString m_name;
		unsigned long long m_value;
	};
	~RegContext()
	{
		clear();
		return;
	}
	void clear()
	{
		QList<Item*>::iterator iter;
		for (iter = m_reg_list.begin(); iter != m_reg_list.end(); /*none*/) {
			Item *item = *iter;
			delete item;
			m_reg_list.erase(iter++);
		}
		return;
	}
	QList<Item*> m_reg_list;
};

class ArchBaseType {
public:
	QString m_short_name;
	QString m_daily_name;
	QString m_long_name;
	QString m_full_name;
	bool m_signed_flag;
	unsigned int m_data_size;
	unsigned int m_data_align;
};

class ArchRegister {
public:
	unsigned int m_index;				/*Index of register*/
	unsigned int m_dwf_no;				/*Number of dwarf*/
	QString m_name;						/*Register name*/
};

enum ArchType {
	ARCH_TYPE_I386 = 0,
	ARCH_TYPE_MAX
};

class Arch {
public:
	~Arch()
	{
		QList<ArchBaseType*>::iterator base_iter;
		for (base_iter = m_base_list.begin(); base_iter != m_base_list.end(); /*none*/) {
			ArchBaseType *base = *base_iter;
			delete base;
			m_base_list.erase(base_iter++);
		}

		QList<ArchRegister*>::iterator reg_iter;
		for (reg_iter = m_reg_list.begin(); reg_iter != m_reg_list.end(); /*none*/) {
			ArchRegister *reg = *reg_iter;
			delete reg;
			m_reg_list.erase(reg_iter++);
		}
		return;
	}

	ArchType m_type;											/*Arch type*/
	QString m_name;												/*Arch name*/
	bool m_bigend;												/*Is big endian*/
	unsigned long long m_max_addr;								/*Address max*/

	unsigned int m_pointlen;									/*Point length (byte)*/
	unsigned int m_stack_align;									/*Stack align*/

	ArchBaseType *getBaseByName(QString name);
	QList<ArchBaseType*> m_base_list;							/*Base type list*/
	QMap<QString, ArchBaseType*> m_base_name_map;				/*Short name to base type*/

	int regContextSave(unsigned int pid, RegContext &context);
	int regContextRestore(unsigned int pid, RegContext &context);
	ArchRegister *getRegByIndex(unsigned int index);
	QList<ArchRegister*> m_reg_list;							/*Register list*/

	ArchRegister *getRegByName(QString name);
	ArchRegister *getRegByDwfNum(unsigned int dwf_reg_no);
	QMap<QString, ArchRegister*> m_reg_name_map;				/*Register name to register*/
	QMap<unsigned int, ArchRegister*> m_reg_dwf_map;			/*Register number to register*/

	unsigned int m_regnum;										/*Number of register*/
	ArchRegister *m_stack;										/*Stack register*/
};

/*Arch master*/
class ArchMaster {
public:
	ArchMaster();
	~ArchMaster()
	{
		QMap<ArchType, Arch*>::iterator iter;
		for (iter = m_arch_map.begin(); iter != m_arch_map.end(); /*none*/) {
			Arch *arch = iter.value();
			delete arch;
			m_arch_map.erase(iter++);
		}
	}
	Arch *getArchByType(ArchType type)
	{
		QMap<ArchType, Arch*>::iterator iter;
		iter = m_arch_map.find(type);
		if (iter == m_arch_map.end()) {
			return NULL;
		}
		return iter.value();
	}
	QMap<ArchType, Arch*> m_arch_map;				/*All arch description*/
};

extern ArchMaster g_uter_arch_master;

#endif /*SRC_TARGET_ARCH_H_*/
