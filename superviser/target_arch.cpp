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

#include <uter_public.h>
#include <target.hxx>
#include <tester.hxx>

struct CommDataDesc {
	const char *base_short_name;
	const char *base_daily_name;
	const char *base_long_name;
	const char *base_full_name;
	bool signed_flag;
};
struct ArchDataDesc {
	unsigned int data_size;
	unsigned int data_align;
};
struct ArchRegDesc {
	const char *m_regname;
	unsigned int m_dwf_regno;
};
struct ArchDesc {
	ArchType m_type;				/*Arch index*/
	const char *m_name;				/*Arch name*/
	const char *m_stack_reg;		/*Stack register name*/
	bool m_bigendian;				/*Is big endian*/
	unsigned long long m_max_addr;	/*Max address*/

	unsigned int m_pointlen;		/*Point length by byte*/
	unsigned int m_stackalign;		/*Stack align by byte*/

	unsigned int m_datanum;			/*Number of base type*/
	ArchDataDesc *m_datadesc;		/*Base type description array*/
	unsigned int m_regnum;			/*Number of register*/
	ArchRegDesc *m_regdesc;			/*Register description array*/
};

CommDataDesc g_CommDataDesc[] = {
	{"char", 		"char",					"signed char", 			"signed char", 			true},
	{"uchar", 		"unsigned char",		"unsigned char", 		"unsigned char", 		false},
	{"short", 		"short",				"signed short", 		"signed short int", 	true},
	{"ushort", 		"unsigned short",		"unsigned short", 		"unsigned short int", 	false},
	{"int", 		"int",					"signed int", 			"signed int", 			true},
	{"uint",		"unsigned int",			"unsigned int",			"unsigned int", 		false},
	{"long",		"long",					"signed long",			"signed long int",		true},
	{"ulong",		"unsigned long",		"unsigned long",		"unsigned long int",	false},
	{"longlong",	"long long",			"signed long long",		"signed long long",		true},
	{"ulonglong",	"unsigned long long",	"unsigned long long",	"unsigned long long",	false},
	{"float",		"float",				"float",				"float",				true},
	{"double",		"double",				"double",				"double",				true},
	{"point",		"point",				"point",				"point",				true},
};

ArchDataDesc g_I386ArchDataDesc[] = {
	{1, 1},	/*char*/
	{1, 1},	/*uchar*/
	{2, 2},	/*short*/
	{2, 2},	/*ushort*/
	{4, 4},	/*int*/
	{4, 4},	/*uint*/
	{4, 4},	/*long*/
	{4, 4},	/*ulong*/
	{8, 4},	/*longlong*/
	{8, 4},	/*ulonglong*/
	{4, 4},	/*float*/
	{8, 8},	/*double*/
	{4, 4},	/*point*/
};

/*see: third_party_backup/binutils-2.25/binutils/dwarf.c*/
ArchRegDesc g_I386ArchRegDesc[] = {
	{"ebx", 3},
	{"ecx", 1},
	{"edx", 2},
	{"esi", 6},
	{"edi", 7},
	{"ebp", 5},
	{"eax", 0},
	{"xds", 43},
	{"xes", 40},
	{"xfs", 44},
	{"xgs", 45},
	{"orig_eax", DwfLoc::REG_UNDEF_VAL},
	{"eip", 8},
	{"xcs", 41},
	{"eflags", 9},
	{"esp", 4},
	{"xss", 42},
};
ArchDesc g_I386ArchDesc[] = {
	{ARCH_TYPE_I386, "i386", "esp", false, 0xffffffff, 4, 4,
			UTER_ARRAY_SIZE(g_I386ArchDataDesc), g_I386ArchDataDesc,
			UTER_ARRAY_SIZE(g_I386ArchRegDesc), g_I386ArchRegDesc},
	/* ... ... */
};

ArchBaseType *Arch::getBaseByName(QString name)
{
	QMap<QString, ArchBaseType*>::iterator iter;
	iter = m_base_name_map.find(name);
	if (iter == m_base_name_map.end()) {
		return NULL;
	}
	return iter.value();
}

int Arch::regContextSave(unsigned int pid, RegContext &context)
{
	int result;
	context.clear();

	QList<ArchRegister*>::iterator reg_iter;
	for (reg_iter = m_reg_list.begin(); reg_iter != m_reg_list.end(); reg_iter++) {
		ArchRegister *reg = *reg_iter;

		unsigned long long value = 0;
		result = g_tester->registerGet(pid, reg->m_name, value);
		if (result != 0) {
			UTER_LOG("Get register %s value for save context failed.",
						reg->m_name.toStdString().c_str());
			return -1;
		}

		RegContext::Item *item = new RegContext::Item();
		item->m_name = reg->m_name;
		item->m_value = value;

		context.m_reg_list.push_back(item);
	}

	return 0;
}

int Arch::regContextRestore(unsigned int pid, RegContext &context)
{
	int result;

	QList<RegContext::Item*>::iterator item_iter;
	for (item_iter = context.m_reg_list.begin();
		 item_iter != context.m_reg_list.end(); item_iter++) {
		RegContext::Item *item = *item_iter;

		result = g_tester->registerGet(pid, item->m_name, item->m_value);
		if (result != 0) {
			UTER_LOG("Set register %s value for restore context failed.",
						item->m_name.toStdString().c_str());
			return -1;
		}
	}

	return 0;
}

ArchRegister *Arch::getRegByIndex(unsigned int index)
{
	if (index >= m_reg_list.length()) {
		return NULL;
	}
	return m_reg_list[index];
}
ArchRegister *Arch::getRegByName(QString name)
{
	QMap<QString, ArchRegister*>::iterator iter;
	iter = m_reg_name_map.find(name);
	if (iter == m_reg_name_map.end()) {
		return NULL;
	}
	return iter.value();
}
ArchRegister *Arch::getRegByDwfNum(unsigned int dwf_reg_no)
{
	QMap<unsigned int, ArchRegister*>::iterator iter;
	iter = m_reg_dwf_map.find(dwf_reg_no);
	if (iter == m_reg_dwf_map.end()) {
		return NULL;
	}
	return iter.value();
}



ArchMaster::ArchMaster()
{
	int iloop;
	for (iloop = 0; iloop < ARCH_TYPE_MAX; iloop++) {
		ArchDesc *archdesc = &(g_I386ArchDesc[iloop]);
		UTER_ASSERT(archdesc->m_type == iloop);

		/*arch*/
		Arch *arch = new Arch();
		arch->m_type = archdesc->m_type;
		arch->m_name = archdesc->m_name;
		arch->m_bigend = archdesc->m_bigendian;
		arch->m_max_addr = archdesc->m_max_addr;

		arch->m_pointlen = archdesc->m_pointlen;
		arch->m_stack_align = archdesc->m_stackalign;

		arch->m_regnum = archdesc->m_regnum;
		arch->m_stack = NULL;

		/*data type*/
		UTER_ASSERT(archdesc->m_datanum == UTER_ARRAY_SIZE(g_CommDataDesc));
		int kloop;
		for (kloop = 0; kloop < archdesc->m_datanum; kloop++) {
			CommDataDesc *comm_basedesc = &(g_CommDataDesc[kloop]);
			ArchDataDesc *arch_basedesc = &(archdesc->m_datadesc[kloop]);

			ArchBaseType *basetype = new ArchBaseType();
			basetype->m_short_name = comm_basedesc->base_short_name;
			basetype->m_daily_name = comm_basedesc->base_daily_name;
			basetype->m_long_name = comm_basedesc->base_long_name;
			basetype->m_full_name = comm_basedesc->base_full_name;
			basetype->m_signed_flag = comm_basedesc->signed_flag;
			basetype->m_data_size = arch_basedesc->data_size;
			basetype->m_data_align = arch_basedesc->data_align;

			arch->m_base_list.push_back(basetype);
			arch->m_base_name_map.insert(basetype->m_short_name, basetype);
		}

		/*register*/
		int jloop;
		for (jloop = 0; jloop < arch->m_regnum; jloop++) {
			ArchRegDesc *regdesc = &(archdesc->m_regdesc[jloop]);

			ArchRegister *reg = new ArchRegister();
			reg->m_index = jloop;
			reg->m_name = regdesc->m_regname;
			reg->m_dwf_no = regdesc->m_dwf_regno;

			arch->m_reg_list.push_back(reg);
			arch->m_reg_name_map.insert(reg->m_name, reg);
			arch->m_reg_dwf_map.insert(reg->m_dwf_no, reg);

			if (strcmp(archdesc->m_stack_reg, regdesc->m_regname) == 0) {
				UTER_ASSERT(arch->m_stack == NULL);
				arch->m_stack = reg;
			}
		}
		UTER_ASSERT(arch->m_stack != NULL);

		m_arch_map.insert(arch->m_type, arch);
	}

	return;
}

ArchMaster g_uter_arch_master;
