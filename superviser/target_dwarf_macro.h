
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

#ifndef SRC_TARGET_DWARF_MACRO_H_
#define SRC_TARGET_DWARF_MACRO_H_

/*TODO:all this file*/

//--------------------------------------------------------------------------
//宏信息的处理，仍需要进行整理

class dwf_macro_item;
class dwf_macro_file;

enum dwf_macro_item_type {
	DWF_MACRO_ITEM_FILE = 1,
	DWF_MACRO_ITEM_DEF,
	DWF_MACRO_ITEM_UNDEF,
	DWF_MACRO_ITEM_DEF_INDIRECT,
	DWF_MACRO_ITEM_UNDEF_INDIRECT,
	DWF_MACRO_ITEM_MAX
};

class dwf_macro_item {
public:
	dwf_macro_item()
	{
		m_parrent = NULL;
		m_item_type = DWF_MACRO_ITEM_MAX;
		return;
	}
	dwf_macro_file *m_parrent;
	enum dwf_macro_item_type m_item_type;
};

class dwf_macro_file:public dwf_macro_item {
public:
	dwf_macro_file()
	{
		m_item_type = DWF_MACRO_ITEM_FILE;

		m_lineno = 0;
		m_filenum = 0;
		m_dir = "";
		m_name = "";
		return;
	}
	unsigned int m_lineno;
	unsigned int m_filenum;
	QString m_dir;
	QString m_name;
	QList<dwf_macro_item*> m_item_list;
};

class dwf_macro_define:public dwf_macro_item {
public:
	dwf_macro_define()
	{
		m_item_type = DWF_MACRO_ITEM_DEF;

		m_lineno = 0;
		m_defstr = "";
		return;
	}

	unsigned int m_lineno;
	QString m_defstr;
};

class dwf_macro_undefine:public dwf_macro_item {
public:
	dwf_macro_undefine()
	{
		m_item_type = DWF_MACRO_ITEM_UNDEF;

		m_lineno = 0;
		m_defstr = "";
		return;
	}

	unsigned int m_lineno;
	QString m_defstr;
};

class dwf_macro_define_indirect:public dwf_macro_item {
public:
	dwf_macro_define_indirect()
	{
		m_item_type = DWF_MACRO_ITEM_DEF_INDIRECT;

		m_lineno = 0;
		m_defstr = "";
		return;
	}

	unsigned int m_lineno;
	QString m_defstr;
};

class dwf_macro_undefine_indirect:public dwf_macro_item {
public:
	dwf_macro_undefine_indirect()
	{
		m_item_type = DWF_MACRO_ITEM_UNDEF_INDIRECT;

		m_lineno = 0;
		m_defstr = "";
		return;
	}

	unsigned int m_lineno;
	QString m_defstr;
};

class dwf_macro {
public:
	dwf_macro ()
	{
		m_line_off = 0xffffffffffffffff;
		return;
	}
	unsigned long long m_line_off;
	dwf_macro_file m_root_file;
};

int get_macro_value(dwf_macro_file *macro_file, char *macro_name, const char **macro_value);

#endif //SRC_TARGET_DWARF_MACRO_H_
