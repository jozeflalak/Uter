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

#ifndef SRC_TARGET_DWARF_TYPE_H_
#define SRC_TARGET_DWARF_TYPE_H_

class DwfType: public DwfItem {
public:
	enum DwfDataType {
		TYPE_BASE = 0,
		TYPE_EN_MEM,
		TYPE_EN,
		TYPE_ST_MEM,
		TYPE_ST,
		TYPE_UN_MEM,
		TYPE_UN,
		TYPE_ARRAY,
		TYPE_SUBRT_MEM,
		TYPE_SUBRT,
		TYPE_VOID,
		TYPE_POINT,
		TYPE_TYPEDEF,
		TYPE_T_MAX
	};
	struct TypeDesc {
		const char *m_upper;
		const char *m_lower;
	};
	enum MakeScriptType {
		MAKE_DEFINE = 0,
		MAKE_OPT_TYPE,
		MAKE_TYPE_NEW,
		MAKE_OPT_OBJ,
		MAKE_MAX,
	};
	DwfType(DwfCu *cu, unsigned long long offset)
	:DwfItem(cu, offset)
	{
		m_element_type = DWF_ITEM_TYPE;
		m_data_type = TYPE_T_MAX;
		m_declare = false;
		m_const = false;
		m_size = 0;
		m_name = "";
		return;
	}
	static TypeDesc type_desc[];					/*Data type class name, eg: struct union*/
	static int typeDesc(DwfType *type, QString &before, QString &after, unsigned long long array_index = 0);	/*Make data type description from DwfType*/
	static QString typeDesc(DwfType *type);
	static QString typeDesc(DwfType *type, QString name);
	DwfDataType m_data_type;								/*Data type, eg: base type, struct, union*/
	bool m_declare;											/*Data type declare, no size, no ...*/
	bool m_const;											/*Const data type*/
	unsigned int m_size;									/*Data size*/
	QString m_name;											/*Data type name, eg: struct xxxx, union xxxx*/
	DwfType *finalType();									/*If type is typedef, return date type which typedef to step by step*/
	virtual unsigned int dataSize(){return 0xffffffff;};	/*Get data type size*/
	virtual unsigned int dataAlign(){return 0xffffffff;};	/*Get data type alignment*/
	virtual QString makeScript(MakeScriptType type, QString name, QString addition) {return "";}
};

/*Base type, eg: int char long ...*/
class DwfTypeBase: public DwfType {
public:
	enum DwfBaseDataType {
		BASE_CHAR = 0,
		BASE_UCHAR,
		BASE_SHORT,
		BASE_USHORT,
		BASE_INT,
		BASE_UINT,
		BASE_LONG,
		BASE_ULONG,
		BASE_LONGLONG,
		BASE_ULONGLONG,
		BASE_FLOAT,
		BASE_DOUBLE,
		BASE_SIZETYPE,
		BASE_MAX,
	};
	struct BaseDesc {
		const char *m_utername;
		const char *m_short;
		const char *m_long;
	};
	static BaseDesc base_desc[];
	DwfTypeBase(DwfCu *cu, unsigned long long offset)
	:DwfType(cu, offset)
	{
		m_data_type = TYPE_BASE;
		m_base_type = BASE_MAX;
		return;
	}
	DwfBaseDataType m_base_type;						/*Base data type*/
	unsigned int dataSize();
	unsigned int dataAlign();
	QString makeScript(MakeScriptType type, QString name, QString addition);
};

/*Enum member*/
class DwfTypeEnMem: public DwfType {
public:
	DwfTypeEnMem(DwfCu *cu)
	:DwfType(cu, DWF_INVALID_OFF)
	{
		m_data_type = TYPE_EN_MEM;
		m_value = 0;
		return;
	}
	long long m_value;						/*Enum member value*/
	unsigned int dataSize();
	unsigned int dataAlign();
};

/*Enum Type*/
class DwfTypeEn: public DwfType {
public:
	DwfTypeEn(DwfCu *cu, unsigned long long offset)
	:DwfType(cu, offset)
	{
		m_data_type = TYPE_EN;
		return;
	}
	~DwfTypeEn()
	{
		QList<DwfTypeEnMem*>::iterator iter;
		for (iter = m_member_list.begin(); iter != m_member_list.end(); /*none*/) {
			DwfTypeEnMem *enmem = *iter;
			delete enmem;
			m_member_list.erase(iter++);
		}
		return;
	}
	QList<DwfTypeEnMem*> m_member_list;
	unsigned int dataSize();
	unsigned int dataAlign();
	QString makeScript(MakeScriptType type, QString name, QString addition);
};

/*Struct member*/
class DwfTypeStMem: public DwfType {
public:
	DwfTypeStMem(DwfCu *cu)
	:DwfType(cu, DWF_INVALID_OFF)
	{
		m_data_type = TYPE_ST_MEM;
		m_type = NULL;
		m_member_location = 0;
		return;
	}
	DwfType *m_type;						/*Struct member data type*/
	unsigned long long m_member_location;	/*The member offset in struct*/
	unsigned int dataSize();
	unsigned int dataAlign();
};

/*Struct data type*/
class DwfTypeSt: public DwfType {
public:
	DwfTypeSt(DwfCu *cu, unsigned long long offset)
	:DwfType(cu, offset)
	{
		m_data_type = TYPE_ST;
		return;
	}
	~DwfTypeSt()
	{
		QList<DwfTypeStMem*>::iterator iter;
		for (iter = m_member_list.begin(); iter != m_member_list.end(); /*none*/) {
			DwfTypeStMem *stmem = *iter;
			delete stmem;
			m_member_list.erase(iter++);
		}
		return;
	}
	QList<DwfTypeStMem*> m_member_list;					/*Struct member list*/
	unsigned int dataSize();
	unsigned int dataAlign();
	QString makeScript(MakeScriptType type, QString name, QString addition);
};

/*Union member*/
class DwfTypeUnMem: public DwfType {
public:
	DwfTypeUnMem(DwfCu *cu)
	:DwfType(cu, DWF_INVALID_OFF)
	{
		m_data_type = TYPE_UN_MEM;
		m_type = NULL;
		return;
	}
	DwfType *m_type;						/*Union memeber data type*/
	unsigned int dataSize();
	unsigned int dataAlign();
};

/*Union data type*/
class DwfTypeUn:public DwfType {
public:
	DwfTypeUn(DwfCu *cu, unsigned long long offset)
	:DwfType(cu, offset)
	{
		m_data_type = TYPE_UN;
		return;
	}
	~DwfTypeUn()
	{
		QList<DwfTypeUnMem*>::iterator iter;
		for (iter = m_member_list.begin(); iter != m_member_list.end(); /*none*/) {
			DwfTypeUnMem *unmem = *iter;
			delete unmem;
			m_member_list.erase(iter++);
		}
		return;
	}
	QList<DwfTypeUnMem*> m_member_list;				/*Union member list*/
	unsigned int dataSize();
	unsigned int dataAlign();
	QString makeScript(MakeScriptType type, QString name, QString addition);
};

/*Array data type*/
class DwfTypeArray: public DwfType {
public:
	DwfTypeArray(DwfCu *cu, unsigned long long offset)
	:DwfType(cu, offset)
	{
		m_data_type = TYPE_ARRAY;
		m_type = NULL;
		return;
	}
	DwfType *m_type;								/*The type array to*/
	QList<unsigned long long> m_range_list;			/*Array index array, eg: int xx[1][2][3] -> 1,2,3*/
	unsigned int dataSize();
	unsigned int dataAlign();
	QString makeScript(MakeScriptType type, QString name, QString addition);
};

/*Function point argument type*/
class DwfTypeSubrtPara: public DwfType {
public:
	DwfTypeSubrtPara(DwfCu *cu)
	:DwfType(cu, DWF_INVALID_OFF)
	{
		m_data_type = TYPE_SUBRT_MEM;
		m_type = NULL;
		return;
	}
	DwfType *m_type;							/*The argument type to*/
	/*No name because function point arguments no name specified*/
};

/*Function point data type*/
class DwfTypeSubrt: public DwfType {
public:
	DwfTypeSubrt(DwfCu *cu, unsigned long long offset)
	:DwfType(cu, offset)
	{
		m_data_type = TYPE_SUBRT;
		m_type = NULL;
		return;
	}
	~DwfTypeSubrt()
	{
		QList<DwfTypeSubrtPara*>::iterator iter;
		for (iter = m_para_list.begin(); iter != m_para_list.end(); /*none*/) {
			DwfTypeSubrtPara *subrtpara = *iter;
			delete subrtpara;
			m_para_list.erase(iter++);
		}
		return;
	}
	DwfType *m_type;						/*Function return type*/
	QList<DwfTypeSubrtPara*> m_para_list;	/*Function arguments*/
	QString makeScript(MakeScriptType type, QString name, QString addition);
};

/*Void data type*/
class DwfTypeVoid: public DwfType {
public:
	DwfTypeVoid(DwfCu *cu)
	:DwfType(cu, DWF_INVALID_OFF)
	{
		m_data_type = TYPE_VOID;
		return;
	}
};

/*Point data type*/
class DwfTypePoint: public DwfType {
public:
	DwfTypePoint(DwfCu *cu, unsigned long long offset)
	:DwfType(cu, offset)
	{
		m_data_type = TYPE_POINT;
		m_type = NULL;
		return;
	}
	DwfType *m_type;						/*The type point to*/
	unsigned int dataSize();
	unsigned int dataAlign();
	QString makeScript(MakeScriptType type, QString name, QString addition);
};

/*Typedef data type*/
class DwfTypeTypedef: public DwfType {
public:
	DwfTypeTypedef(DwfCu *cu, unsigned long long offset)
	:DwfType(cu, offset)
	{
		m_data_type = TYPE_TYPEDEF;
		m_type = NULL;
		return;
	}
	DwfType *m_type;						/*Typedef to*/
	unsigned int dataSize();
	unsigned int dataAlign();
	QString makeScript(MakeScriptType type, QString name, QString addition);
};

extern DwfTypeVoid g_dwf_type_void;

/*Variable Item*/
class DwfVar: public DwfItem {
public:
	DwfVar(DwfCu *cu)
	:DwfItem(cu)
	{
		m_element_type = DWF_ITEM_VAR;
		m_type = NULL;
		m_name = "";
		m_extern = false;
		return;
	}
	DwfType *m_type;						/*Variable data type*/
	DwfLoc m_loc;							/*Variable location*/
	QString m_name;							/*Variable name*/
	bool m_extern;							/*Is extern*/
	QString varDesc();						/*Variable discription*/
};

/*Function parameter*/
class DwfPara: public DwfItem {
public:
	DwfPara(DwfCu *cu)
	:DwfItem(cu, DWF_INVALID_OFF)
	{
		m_element_type = DWF_ITEM_FUN_PARA;
		m_type = NULL;
		m_name = "";
		return;
	}
	DwfType *m_type;							/*Parameter data type*/
	DwfLoc m_loc;								/*Parameter location*/
	QString m_name;								/*Parameter name*/
	QString paraDesc();							/*Parameter discription*/
};

/*Function*/
class DwfFun: public DwfItem {
public:
	DwfFun(DwfCu *cu)
	:DwfItem(cu, DWF_INVALID_OFF)
	{
		m_element_type = DWF_ITEM_FUN;
		m_type = NULL;
		m_inline = 0;
		m_name = "";
		m_extern = false;
		return;
	}
	~DwfFun()
	{
		QList<DwfPara*>::iterator iter;
		for (iter = m_para_list.begin(); iter != m_para_list.end(); /*none*/) {
			DwfPara *dwfpara = *iter;
			delete dwfpara;
			m_para_list.erase(iter++);
		}
		return;
	}
	DwfType *m_type;						/*Function return type*/
	unsigned long long m_inline;			/*Inline number from dwarf*/
	QList<DwfPara*> m_para_list;			/*Function arguments*/
	QString m_name;							/*Function name*/
	DwfAddrRange m_add_range;				/*Function address range*/
	bool m_extern;							/*Is extern function, not static*/
	CovTokenSet m_cov_token;				/*Coverage control block*/
	QString funDesc();						/*Function description*/
};

#endif /*SRC_TARGET_DWARF_TYPE_H_*/
