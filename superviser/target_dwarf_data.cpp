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

/*Add # prefix before every line*/
static QString pythonComment(QString input)
{
	QString retstr;
	QStringList linelist = input.split("\n");
	QStringList::iterator iter;
	for (iter = linelist.begin(); iter != linelist.end(); iter++) {
		QString line = *iter;
		retstr += "#" + line + "\n";
	}
	return retstr;
}

QString DwfItem::makeItemScript(QString cu, QString name)
{
	QString retstr;
	switch(m_element_type)
	{
	case DWF_ITEM_TYPE:
	{
		DwfType *type = (DwfType *)this;
		retstr += "##################################################\n";
		retstr += "##Data type export\n";
		retstr += "##################################################\n";
		retstr += "#Compilation unit: " + m_cu->m_path + "\n";
		retstr += "#Source file: " + m_cu->getFileNameByNo(m_fileno) + "\n";
		retstr += "#Source line: " + QString::asprintf("%llu", m_lineno) + "\n";
		retstr += pythonComment(type->makeScript(DwfType::MAKE_DEFINE, "", ""));
		retstr += QString() + "#Export data type with compilation unit which the " + DwfType::type_desc[type->m_data_type].m_lower + " define or used.\n";
		retstr += QString() + "my_data_type = uter.export." + DwfType::type_desc[type->m_data_type].m_upper + "(\"" + name + "\", cus = " + cu + ")\n";
		retstr += "#Export data type without compilation unit.\n";
		retstr += QString() + "my_data_type = uter.export." + DwfType::type_desc[type->m_data_type].m_upper + "(\"" + name + "\"" +              ")\n";
		retstr += "##################################################\n";
		retstr += "##Data type operation\n";
		retstr += "##################################################\n";
		retstr += type->makeScript(DwfType::MAKE_OPT_TYPE, "my_data_type", "");
		retstr += "##################################################\n";
		retstr += "##New data object\n";
		retstr += "##################################################\n";
		retstr += type->makeScript(DwfType::MAKE_TYPE_NEW, "my_data_type", "my_data_obj");
		retstr += "##################################################\n";
		retstr += "##Data object operation\n";
		retstr += "##################################################\n";
		retstr += type->makeScript(DwfType::MAKE_OPT_OBJ, "my_data_obj", "");
		retstr += "#Convert to other type\n";
		retstr += QString("data_object_of_other_type = ") + "my_data_obj" + "._AsType(other_data_type)\n";
		break;
	}
	case DWF_ITEM_VAR:
	{
		DwfVar *var = (DwfVar *)this;
		retstr += "##################################################\n";
		retstr += "##Data object export\n";
		retstr += "##################################################\n";
		retstr += "#Compilation unit: " + m_cu->m_path + " \n";
		retstr += "#Source file: " + m_cu->getFileNameByNo(m_fileno) + "\n";
		retstr += "#Source line: " + QString::asprintf("%llu", m_lineno) + "\n";
		retstr += pythonComment(var->varDesc());
		retstr += "#Export data object with compilation unit which the variable define in.\n";
		retstr += "my_data_obj = uter.export.variable(\"" + name + "\", cus = " + cu + ")\n";
		retstr += "#Export data object without compilation unit.\n";
		retstr += "my_data_obj = uter.export.variable(\"" + name + "\"" +              ")\n";
		retstr += "#Get data object address.\n";
		retstr += "print my_data_obj._GetAddress()        #print address of variable " + name + "\n";
		retstr += "print hex(my_data_obj._GetAddress())        #hex format\n";
		retstr += "##################################################\n";
		retstr += "##Data object operation\n";
		retstr += "##################################################\n";
		DwfType *type = var->m_type;
		retstr += type->makeScript(DwfType::MAKE_OPT_OBJ, "my_data_obj", "");
		retstr += "#Convert to other type\n";
		retstr += "data_object_of_other_type = my_data_obj._AsType(other_data_type)\n";
		retstr += "##################################################\n";
		retstr += "##Get data type\n";
		retstr += "##################################################\n";
		retstr += "#Get data from data object\n";
		retstr += "my_data_type = my_data_obj._GetType()        #my_data_type is a data type\n";
		retstr += "##################################################\n";
		retstr += "##Data type operation\n";
		retstr += "##################################################\n";
		retstr += type->makeScript(DwfType::MAKE_OPT_TYPE, "my_data_type", "");
		break;
	}
	case DWF_ITEM_FUN:
	{
		DwfFun *fun = (DwfFun *)this;
		retstr += "##################################################\n";
		retstr += "##Function object export\n";
		retstr += "##################################################\n";
		retstr += "#Compilation unit: " + m_cu->m_path + " \n";
		retstr += "#Source file: " + m_cu->getFileNameByNo(m_fileno) + " \n";
		retstr += "#Source line: " + QString::asprintf("%llu", m_lineno) + " \n";
		retstr += pythonComment(fun->funDesc());
		retstr += "#Export function object with compilation unit which the function define in.\n";
		retstr += "my_fun_obj = uter.export.function(\"" + name + "\", cus = " + cu + ")\n";
		retstr += "#Export function object without compilation unit.\n";
		retstr += "my_fun_obj = uter.export.function(\"" + name + "\"" +              ")\n";
		retstr += "#Get fun object address.\n";
		retstr += "print my_fun_obj._GetAddress()        #print address of function " + name + "\n";
		retstr += "print hex(my_fun_obj._GetAddress())        #hex format\n";
		retstr += "##################################################\n";
		retstr += "##Call function object\n";
		retstr += "##################################################\n";
		retstr += "#Call function " + fun->m_name + "\n";
		retstr += "my_ret_val = " + fun->m_name + "._Call(";
		int iloop = 1;
		QList<DwfPara*>::iterator iter;
		for (iter = fun->m_para_list.begin(); iter != fun->m_para_list.end(); iter++) {
			DwfPara *dwfpara = *iter;
			DwfType *type = dwfpara->m_type;
			if (iter != fun->m_para_list.begin()) {
				retstr += ", ";
			}
			if (type->m_data_type == DwfType::TYPE_BASE) {
				DwfTypeBase *base = (DwfTypeBase *)type;
				retstr += QString("uter.") + DwfTypeBase::base_desc[base->m_base_type].m_utername + "(" + QString::asprintf("%d", iloop++) + ")";
			} else {
				retstr += "data_object_for_arg_" + dwfpara->m_name;
			}
		}
		retstr += ")        # return the return type data object \n";
		retstr += "##################################################\n";
		retstr += "##Stub function object\n";
		retstr += "##################################################\n";
		retstr += "#Stub function " + fun->m_name + "\n";
		retstr += "def stub_fun(";
		for (iter = fun->m_para_list.begin(); iter != fun->m_para_list.end(); iter++) {
			DwfPara *dwfpara = *iter;
			if (iter != fun->m_para_list.begin()) {
				retstr += ", ";
			}
			retstr += "funarg_" + dwfpara->m_name;
		}
		retstr += "):\n{\n";
		retstr += "    #......\n";
		retstr += "    #......\n";
		retstr += "    #......\n";
		if (fun->m_type->m_data_type == DwfType::TYPE_BASE) {
			DwfTypeBase *base = (DwfTypeBase *)fun->m_type;
			retstr += QString("    return uter.") + DwfTypeBase::base_desc[base->m_base_type].m_utername + "(5)\n";
		} else {
			retstr += "    return data_object_with_return_type\n";
		}
		retstr += "}\n\n";
		retstr += "my_fun_obj._Stub(stub_fun)        #stub function\n";
		retstr += "#Do something run call function " + fun->m_name + ", and trigger script stub_fun()\n";
		retstr += "my_fun_obj._Stub(None)        #clear stub\n";
		break;
	}
	}
	return retstr;
}

DwfType::TypeDesc DwfType::type_desc[] = {
	{"", ""},					/*TYPE_BASE*/
	{"", ""},					/*TYPE_EN_MEM*/
	{"ENUM", "enum"},			/*TYPE_EN*/
	{"", ""},					/*TYPE_ST_MEM*/
	{"STRUCT", "struct"},		/*TYPE_ST*/
	{"", ""},					/*TYPE_UN_MEM*/
	{"UNION", "union"},			/*TYPE_UN*/
	{"", ""},					/*TYPE_ARRAY*/
	{"", ""},					/*TYPE_SUBRT_MEM*/
	{"", ""},					/*TYPE_SUBRT*/
	{"", ""},					/*TYPE_VOID*/
	{"TYPEDEF", "typedef"},		/*TYPE_POINT*/
	{"", ""},					/*TYPE_TYPEDEF*/
};

DwfTypeBase::BaseDesc DwfTypeBase::base_desc[] = {
	{"CHAR", "char", "char"},
	{"UCHAR", "uchar", "unsigned char"},
	{"SHORT", "short", "short"},
	{"USHORT", "ushort", "unsigned short"},
	{"INT", "int", "int"},
	{"UINT", "uint", "unsigned int"},
	{"LONG", "long", "long"},
	{"ULONG", "ulong", "unsigned long"},
	{"LONGLONG", "longlong", "long long"},
	{"ULONGLONG", "ulonglong", "unsigned long long"},
	{"FLOAT", "float", "float"},
	{"DOUBLE", "double", "double"},
	{"", "sizetype", "sizetype"}
};

/* Static function in DwfType, make description for DwfType, the description include two part.
 * eg: data                                   before          after
 *     int normal_data                        int
 *     int array_data[5]                      int             [5]
 *     int (*function_point)(int, int)        int (*          )(int, int)
 **/
int DwfType::typeDesc(DwfType *type, QString &before, QString &after, unsigned long long array_index)
{
	UTER_ASSERT(type != NULL);
	before = "";
	after = "";

	switch(type->m_data_type)
	{
	case TYPE_BASE:
	{
		/*eg: int int_data     ==>      int  ->  int_data -> */
		DwfTypeBase *type_base = (DwfTypeBase *)type;
		before = DwfTypeBase::base_desc[type_base->m_base_type].m_long;
		break;
	}
	case TYPE_ST:
	{
		DwfTypeSt *type_st = (DwfTypeSt *)type;
		if (type_st->m_name == "") {
			/*eg: struct {....} struct_data       ==>   struct {...} -> struct_data -> */
			before = "struct {...}";
		} else {
			/*eg: struct struct_name struct_data  ==>   struct struct_name -> struct_data -> */
			before = QString("struct ") + type_st->m_name;
		}
		break;
	}
	case TYPE_UN:
	{
		DwfTypeUn *type_un = (DwfTypeUn *)type;
		if (type_un->m_name == "") {
			/*eg: union {....} union_data      ==>    union {...} -> union_data -> */
			before = "union {...}";
		} else {
			/*eg: union union_name union_data  ==>    union union_name -> union_data -> */
			before = QString("union ") + type_un->m_name;
		}
		break;
	}
	case TYPE_EN:
	{
		DwfTypeEn *type_en = (DwfTypeEn *)type;
		if (type_en->m_name == "") {
			/*eg: enum {....} enum_data     ==>     enum {...} -> enum_data -> */
			before = "enum {...}";
		} else {
			/*eg: enum enum_name enum_data  ==>     enum enum_name -> enum_data -> */
			before = "enum " + type_en->m_name;
		}
		break;
	}
	case TYPE_SUBRT:
	{
		/*eg: int subrt_name(int, int)      ==>      int( -> subrt_name -> )(int, int)*/
		DwfTypeSubrt *type_subrt = (DwfTypeSubrt *)type;
		DwfType *rettype = type_subrt->m_type;
		QString ret_before;
		QString ret_after;
		typeDesc(rettype, ret_before, ret_after);
		before += ret_before + ret_after + "(";

		after += ")(";
		QList<DwfTypeSubrtPara*>::iterator iter;
		for (iter = type_subrt->m_para_list.begin(); iter != type_subrt->m_para_list.end(); iter++) {
			DwfTypeSubrtPara *para = *iter;
			DwfType *argtype = para->m_type;
			QString arg_before;
			QString arg_after;
			typeDesc(argtype, arg_before, arg_after);
			if (iter != type_subrt->m_para_list.begin()) {
				after += ", ";
			}
			after += arg_before + arg_after;
		}
		after += ")";
		break;
	}
	case TYPE_POINT:
	{
		/* eg:
		 * int *p_int_data          ==> int -> ->               ==> int * -> p_int_data ->
		 * int *pp_int_data         ==> int -> ->               ==> int * -> -> => int ** -> p_int_data ->
		 * int (*p_subrt)(int, int) ==> int ( -> -> )(int, int) ==> int (* -> p_subrt -> )(int, int)
		 * */
		DwfTypePoint *type_point = (DwfTypePoint *)type;
		DwfType *totype = type_point->m_type;
		QString to_before;
		QString to_after;
		typeDesc(totype, to_before, to_after);
		if (to_before.right(1) == "*" || to_before.right(1) == "(") {
			before = to_before + "*";
		} else {
			before = to_before + " *";
		}

		after = to_after;
		break;
	}
	case TYPE_ARRAY:
	{
		/* eg(index = 0):
		 * int array_int_data[2]                ==>  int -> ->            ==> int -> array_int_data -> [0]
		 * int array_int_data[2][2]             ==> int -> ->             ==> int -> -> [0]             ==> int -> array_int_data -> [0][0]
		 * int (*array_p_fun_data[2])(int, int) ==> int (-> ->)(int, int) ==> int (* -> -> )(int, int)  ==> int (* -> array_p_fun_data -> [0])(int, int)
		 * */
		DwfTypeArray *type_array = (DwfTypeArray *)type;
		DwfType *totype = type_array->m_type;						/*Final Type of array, not the array*/
		QString to_before;
		QString to_after;
		typeDesc(totype, to_before, to_after);
		before += to_before;

		/*It is a sub array or final type, foreach the remain array index*/
		unsigned int iloop;
		for (iloop = array_index; iloop < type_array->m_range_list.length(); iloop++) {
			unsigned long long range = type_array->m_range_list.at(iloop);
			after += "[" + QString::asprintf("%lld", range) + "]";		/*Index is the size of index, equal to bound add 1*/
		}

		after += to_after;
		break;
	}
	case TYPE_TYPEDEF:
	{
		/*eg: typedef_type typedef_data     ==>    typedef_type -> typedef_data -> */
		DwfTypeTypedef *type_typedef = (DwfTypeTypedef *)type;
		before = type_typedef->m_name;
		break;
	}
	case TYPE_VOID:
	{
		before = "void";
		break;
	}
	default:
	{
		break;
	}
	}

	return 0;
}

QString DwfType::typeDesc(DwfType *type)
{
	UTER_ASSERT(type != NULL);
	QString before;
	QString after;
	typeDesc(type, before, after);
	return before + after;
}

QString DwfType::typeDesc(DwfType *type, QString name)
{
	UTER_ASSERT(type != NULL);
	QString before;
	QString after;
	typeDesc(type, before, after);
	return before + " " + name + after;
}

DwfType *DwfType::finalType()
{
	if (m_data_type == TYPE_TYPEDEF) {
		DwfTypeTypedef *type = (DwfTypeTypedef *)this;
		return type->m_type->finalType();
	}
	if (m_data_type == TYPE_ST_MEM) {
		DwfTypeStMem *type = (DwfTypeStMem *)this;
		return type->m_type->finalType();
	}
	if (m_data_type == TYPE_UN_MEM) {
		DwfTypeUnMem *type = (DwfTypeUnMem *)this;
		return type->m_type->finalType();
	}

	return this;
}

unsigned int DwfTypeBase::dataSize()
{
	return m_size;
}

unsigned int DwfTypeBase::dataAlign()
{
	return m_size;
}

QString DwfTypeBase::makeScript(MakeScriptType type, QString name, QString addition)
{
	QString retstr;
	switch(type)
	{
	case MAKE_DEFINE:
	{
		retstr += typeDesc(this);
		break;
	}
	case MAKE_OPT_TYPE:
	{
		retstr += "print " + name + " is uter." + base_desc[m_base_type].m_utername + "        #print True, because base data type is singleton\n";
		retstr += "print " + name + "        #print detail about the data type\n";
		retstr += "print " + name + "._GetSize()        #print " + QString::asprintf("%u", dataSize()) + "\n";
		retstr += "print " + name + "._GetAlign()        #print " + QString::asprintf("%u", dataAlign()) + "\n";
		retstr += "#......\n";
		break;
	}
	case MAKE_TYPE_NEW:
	{
		retstr += QString() + "#Create data object with uter." + base_desc[m_base_type].m_utername + " direct\n";
		retstr += addition + " = uter." + base_desc[m_base_type].m_utername + "()\n";
		retstr += addition + " = " + name + "()        #create data object without initialization\n";
		retstr += "print " + addition + "._GetValue()        #print 0\n";
		retstr += addition + " = " + name + "(88)        #create data object with initialization\n";
		retstr += "print " + addition + "._GetValue()        #print 88\n";
		break;
	}
	case MAKE_OPT_OBJ:
	{
		retstr += "print " + name + "        #print detail about the data object\n";
		retstr += name + "._SetValue(100)        #set value to data object";
		retstr += "print " + name + "._GetValue()        #print 100, get value from data object\n";
		retstr += "print " + name + "._GetSize()        #print " + QString::asprintf("%u", dataSize()) + ", same as data type\n";
		retstr += "print " + name + "._GetAlign()        #print " + QString::asprintf("%u", dataAlign()) + ", same as data type\n";
		retstr += "#......\n";
		break;
	}
	}
	return retstr;
}

unsigned int DwfTypeEnMem::dataSize()
{
	/*enum same as unsigned int*/
	return 4;
}
unsigned int DwfTypeEnMem::dataAlign()
{
	/*enum same as unsigned int*/
	return 4;
}
unsigned int DwfTypeEn::dataSize()
{
	/*enum same as unsigned int*/
	return 4;
}
unsigned int DwfTypeEn::dataAlign()
{
	/*enum same as unsigned int*/
	return 4;
}

QString DwfTypeEn::makeScript(MakeScriptType type, QString name, QString addition)
{
	QString retstr;
	QList<DwfTypeEnMem*>::iterator iter;
	switch(type)
	{
	case MAKE_DEFINE:
	{
		retstr += "enum " + m_name + " {\n";
		for (iter = m_member_list.begin(); iter != m_member_list.end(); iter++) {
			DwfTypeEnMem *mem = *iter;
			retstr += "    " + mem->m_name + " = " + QString::asprintf("%lld", mem->m_value) + ",\n";
		};
		retstr += "}";
		break;
	}
	case MAKE_OPT_TYPE:
	{
		retstr += "print " + name + "        #print detail about the data type\n";
		retstr += "print " + name + "._GetSize()        #print " + QString::asprintf("%u", dataSize()) + "\n";
		retstr += "print " + name + "._GetAlign()        #print " + QString::asprintf("%u", dataAlign()) + "\n";
		retstr += "#Enum data type have same name member, the member type is integer(not a data object).\n";
		for (iter = m_member_list.begin(); iter != m_member_list.end(); iter++) {
			DwfTypeEnMem *mem = *iter;
			if (mem->m_name.left(1) == "_") {
				retstr += "#print " + name + "." + mem->m_name + "        #not support access by dot\n";
				retstr += "print " + name + "._Member(\"" + mem->m_name + "\")        #print " + QString::asprintf("%lld", mem->m_value) + "\n";
			} else {
				retstr += "print " + name + "." + mem->m_name + "        #print " + QString::asprintf("%lld", mem->m_value) + "\n";
			}
		}
		retstr += "#......\n";
		break;
	}
	case MAKE_TYPE_NEW:
	{
		retstr += addition + " = " + name + "()        #create data object without initialization\n";
		retstr += "print " + addition + "._GetValue()        #print 0\n";
		for (iter = m_member_list.begin(); iter != m_member_list.end(); iter++) {
			DwfTypeEnMem *mem = *iter;
			if (mem->m_name.left(1) == "_") {
				retstr += "#" + addition + " = " + name + "(" + name + "." + mem->m_name + ")        #not support access by dot\n";
				retstr += addition + " = " + name + "(" + name + "._Member(\"" + mem->m_name + "\"))        #create data object with initialization\n";
			} else {
				retstr += addition + " = " + name + "(" + name + "." + mem->m_name + ")        #create data object with initialization\n";
			}
			retstr += "print " + addition + "._GetValue()        #print " + QString::asprintf("%lld", mem->m_value) + "\n";
		}
		retstr += addition + " = " + name + "(100)        #create data object with initialization\n";
		retstr += "print " + addition + "._GetValue()        #print 100\n";
		break;
	}
	case MAKE_OPT_OBJ:
	{
		retstr += "print " + name + "        #print detail about the data object\n";
		retstr += "print " + name + "._GetSize()        #print " + QString::asprintf("%u", dataSize()) + ", same as data type\n";
		retstr += "print " + name + "._GetAlign()        #print " + QString::asprintf("%u", dataAlign()) + ", same as data type\n";
		retstr += name + "._SetValue(400)        #set and get value to data object\n";
		retstr += "print " + name + "._GetValue()        #print 400, get value from data object\n";
		retstr += "#......\n";
		break;
	}
	}
	return retstr;
}

unsigned int DwfTypeStMem::dataSize()
{
	return m_type->dataSize();
}

unsigned int DwfTypeStMem::dataAlign()
{
	return m_type->dataAlign();
}

unsigned int DwfTypeSt::dataSize()
{
	return m_size;
}

unsigned int DwfTypeSt::dataAlign()
{
	unsigned int align = 0;
	QList<DwfTypeStMem*>::iterator iter;
	for (iter = m_member_list.begin(); iter != m_member_list.end(); iter++) {
		DwfTypeStMem *mem = *iter;
		unsigned int mem_align = mem->dataSize();
		if (mem_align > align) {
			align = mem_align;
		}
	}
	return align;
}

QString DwfTypeSt::makeScript(MakeScriptType type, QString name, QString addition)
{
	QString retstr;
	QList<DwfTypeStMem*>::iterator iter;
	switch(type)
	{
	case MAKE_DEFINE:
	{
		retstr += "struct " + m_name + " {\n";
		for (iter = m_member_list.begin(); iter != m_member_list.end(); iter++) {
			DwfTypeStMem *mem = *iter;
			retstr += "    " + typeDesc(mem->m_type, mem->m_name) + ";\n";
		};
		retstr += "};";
		break;
	}
	case MAKE_OPT_TYPE:
	{
		retstr += "print " + name + "        #print detail about the data type\n";
		retstr += "print " + name + "._GetSize()        #print " + QString::asprintf("%u", dataSize()) + "\n";
		retstr += "print " + name + "._GetAlign()        #print " + QString::asprintf("%u", dataAlign()) + "\n";
		retstr += "#Struct data type have same name member, and the member type is uter data type also.\n";
		for (iter = m_member_list.begin(); iter != m_member_list.end(); iter++) {
			DwfTypeStMem *mem = *iter;
			if (mem->m_name.left(1) == "_") {
				retstr += "#print " + name + "." + mem->m_name + "        #not support access by dot\n";
				retstr += "print " + name + "._Member(\"" + mem->m_name + "\")        #print detail about member " + name + " data type\n";
			} else {
				retstr += "print " + name + "." + mem->m_name + "        #print detail about member " + name + " data type\n";
			}
			retstr += "print " + name + "._Member(\"" + mem->m_name + "\")._GetSize()        #print " + QString::asprintf("%u", mem->dataSize()) + "\n";
			retstr += "print " + name + "._Member(\"" + mem->m_name + "\")._GetAlign()        #print " + QString::asprintf("%u", mem->dataAlign()) + "\n";
		}
		retstr += "#......\n";
		break;
	}
	case MAKE_TYPE_NEW:
	{
		retstr += addition + " = " + name + "()        #create data object without initialization\n";
		retstr += "#Make member type data object without initialization\n";
		for (iter = m_member_list.begin(); iter != m_member_list.end(); iter++) {
			DwfTypeStMem *mem = *iter;
			if (mem->m_name.left(1) == "_") {
				retstr += "#data_obj_with_type_member_" + mem->m_name + " = " + name + "." + mem->m_name + "()        #not support access by dot\n";
				retstr += "data_obj_with_type_member_" + mem->m_name + " = " + name + "._Member(\"" + mem->m_name + "\")()        #create data object with type of member " + mem->m_name + "\n";
			} else {
				retstr += "data_obj_with_type_member_" + mem->m_name + " = " + name + "." + mem->m_name + "()        #create data object with type of member " + mem->m_name + "\n";
			}
		}
		retstr += "#Make data object with number initialization\n";
		retstr += addition + " = " + name + "(";
		int iloop = 5;
		for (iter = m_member_list.begin(); iter != m_member_list.end(); iter++) {
			DwfTypeStMem *mem = *iter;
			if (iter != m_member_list.begin()) {
				retstr += ", ";
			}
			if (mem->m_type->m_data_type == TYPE_BASE) {
				retstr += QString::asprintf("%d", iloop++);
			} else {
				retstr += "data_obj_with_type_member_" + mem->m_name;
			}
		}
		retstr += ")\n";
		retstr += "#Make data object with data object initialization\n";
		retstr += addition + " = " + name + "(";
		for (iter = m_member_list.begin(); iter != m_member_list.end(); iter++) {
			DwfTypeStMem *mem = *iter;
			if (iter != m_member_list.begin()) {
				retstr += ", ";
			}
			retstr += "data_obj_with_type_member_" + mem->m_name;
		}
		retstr += ")\n";
		break;
	}
	case MAKE_OPT_OBJ:
	{
		retstr += "print " + name + "        #print detail about the data object\n";
		retstr += "print " + name + "._GetSize()        #print " + QString::asprintf("%d", dataSize()) + "\n";
		retstr += "print " + name + "._GetAlign()        #print " + QString::asprintf("%d", dataAlign()) + "\n";
		int iloop = 10;
		for (iter = m_member_list.begin(); iter != m_member_list.end(); iter++, iloop++) {
			DwfTypeStMem *mem = *iter;
			if (mem->m_name.left(1) == "_") {
				retstr += "#" + name + "." + mem->m_name + "        #not support access by dot\n";
				retstr += name + "._Member(\"" + mem->m_name + "\")._SetValue(";
				if (mem->m_type->m_data_type == TYPE_BASE) {
					retstr += QString::asprintf("%d", iloop);
				} else {
					retstr += "data_obj_with_type_member_" + mem->m_name;
				}
				retstr += ")\n";
			} else {
				retstr += name + "." + mem->m_name + "._SetValue(";
				if (mem->m_type->m_data_type == TYPE_BASE) {
					retstr += QString::asprintf("%d", iloop);
				} else {
					retstr += "data_obj_with_type_member_" + mem->m_name;
				}
				retstr += ")\n";
			}
			if (mem->m_name.left(1) == "_") {
				retstr += "#print " + name + "." + mem->m_name + "        #not support access by dot\n";
				retstr += "print" + name + "._Member(\"" + mem->m_name + "\")._GetValue()";
				if (mem->m_type->m_data_type == TYPE_BASE) {
					retstr += "        #print " + QString::asprintf("%d", iloop) + "\n";
				} else {
					retstr += "        #display data object detail of member " + mem->m_name + "\n";
				}
			} else {
				retstr += "print " + name + "." + mem->m_name + "._GetValue()";
				if (mem->m_type->m_data_type == TYPE_BASE) {
					retstr += "        #print " + QString::asprintf("%d", iloop) + "\n";
				} else {
					retstr += "        #display data object detail of member " + mem->m_name + "\n";
				}
			}
			retstr += "print " + name + "._Member(\"" + mem->m_name + "\")._GetSize()        #print " + QString::asprintf("%d", mem->dataSize()) + "\n";
			retstr += "print " + name + "._Member(\"" + mem->m_name + "\")._GetAlign()        #print " + QString::asprintf("%d", mem->dataAlign()) + "\n";
		}
		retstr += "#......\n";
		break;
	}
	default:
	{
		break;
	}
	}

	return retstr;
}

unsigned int DwfTypeUnMem::dataSize()
{
	return m_type->dataSize();
}

unsigned int DwfTypeUnMem::dataAlign()
{
	return m_type->dataAlign();
}

unsigned int DwfTypeUn::dataSize()
{
	return m_size;
}

unsigned int DwfTypeUn::dataAlign()
{
	unsigned int align = 0;
	QList<DwfTypeUnMem*>::iterator iter;
	for (iter = m_member_list.begin(); iter != m_member_list.end(); iter++) {
		DwfTypeUnMem *mem = *iter;
		unsigned int mem_align = mem->dataSize();
		if (mem_align > align) {
			align = mem_align;
		}
	}
	return align;
}

QString DwfTypeUn::makeScript(MakeScriptType type, QString name, QString addition)
{
	QString retstr;
	QList<DwfTypeUnMem*>::iterator iter;
	switch(type)
	{
	case MAKE_DEFINE:
	{
		retstr += "union " + m_name + " {\n";
		for (iter = m_member_list.begin(); iter != m_member_list.end(); iter++) {
			DwfTypeUnMem *mem = *iter;
			QString desc_before;
			QString desc_after;
			typeDesc(mem->m_type, desc_before, desc_after);
			retstr += "    " + desc_before + " " + mem->m_name + desc_after + ";\n";
		};
		retstr += "}\n";
		break;
	}
	case MAKE_OPT_TYPE:
	{
		retstr += "print " + name + "        #print detail about the data type\n";
		retstr += "print " + name + "._GetSize()        #print " + QString::asprintf("%u", dataSize()) + "\n";
		retstr += "print " + name + "._GetAlign()        #print " + QString::asprintf("%u", dataAlign()) + "\n";
		retstr += "#Union data type have same name member, the member type is uter data type also.\n";
		for (iter = m_member_list.begin(); iter != m_member_list.end(); iter++) {
			DwfTypeUnMem *mem = *iter;
			if (mem->m_name.left(1) == "_") {
				retstr += "#print " + name + "." + mem->m_name + "        #not support access by dot\n";
				retstr += "print " + name + "._Member(\"" + mem->m_name + "\")        #print detail about member " + mem->m_name + " data type\n";
			} else {
				retstr += "print " + name + "." + mem->m_name + "        #print detail about member " + mem->m_name + " data type\n";
			}
			retstr += "print " + name + "._Member(\"" + mem->m_name + "\")._GetSize()        #print " + QString::asprintf("%u", mem->dataSize()) + "\n";
			retstr += "print " + name + "._Member(\"" + mem->m_name + "\")._GetAlign()        #print " + QString::asprintf("%u", mem->dataAlign()) + "\n";
		}
		retstr += "#......\n";
		break;
	}
	case MAKE_TYPE_NEW:
	{
		retstr += addition + " = " + name + "()        #create data object without initialization\n";
		retstr += "#Make member type data object without initialization\n";
		for (iter = m_member_list.begin(); iter != m_member_list.end(); iter++) {
			DwfTypeUnMem *mem = *iter;
			if (mem->m_name.left(1) == "_") {
				retstr += "#data_obj_with_type_member_" + mem->m_name + " = " + name + "." + mem->m_name + "()        #not support access by dot\n";
				retstr += "data_obj_with_type_member_" + mem->m_name + " = " + name + "._Member(\"" + mem->m_name + "\")()        #create data object with type of member " + mem->m_name + "\n";
			} else {
				retstr += "data_obj_with_type_member_" + mem->m_name + " = " + name + "." + mem->m_name + "()        #create data object with type of member " + mem->m_name + "\n";
			}
		}
		retstr += "#Make data object with number initialization\n";
		int iloop = 5;
		for (iter = m_member_list.begin(); iter != m_member_list.end(); iter++) {
			DwfTypeUnMem *mem = *iter;
			retstr += addition + " = " + name + "(\"" + mem->m_name + "\", (";
			if (mem->m_type->m_data_type == TYPE_BASE) {
				retstr += QString::asprintf("%d", iloop++);
			} else {
				retstr += "data_obj_with_type_member_" + mem->m_name;
			}
			retstr += ",))        #initialize as member " + mem->m_name + "\n";
		}
		retstr += "#Make data object with data object initialization\n";
		for (iter = m_member_list.begin(); iter != m_member_list.end(); iter++) {
			DwfTypeUnMem *mem = *iter;
			retstr += addition + " = " + name + "(\"" + mem->m_name + "\", (";
			retstr += "data_obj_with_type_member_" + mem->m_name;
			retstr += ",))        #initialize as member " + mem->m_name + "\n";
		}
		break;
	}
	case MAKE_OPT_OBJ:
	{
		retstr += "print " + name + "        #print detail about the data object\n";
		retstr += "print " + name + "._GetSize()        #print " + QString::asprintf("%d", dataSize()) + "\n";
		retstr += "print " + name + "._GetAlign()        #print " + QString::asprintf("%d", dataAlign()) + "\n";
		int iloop = 10;
		for (iter = m_member_list.begin(); iter != m_member_list.end(); iter++, iloop++) {
			DwfTypeUnMem *mem = *iter;
			if (mem->m_name.left(1) == "_") {
				retstr += "#" + name + "." + mem->m_name + "._SetValue(...)        #not support access by dot\n";
				retstr += name + "._Member(\"" + mem->m_name + "\")._SetValue(";
				if (mem->m_type->m_data_type == TYPE_BASE) {
					retstr += QString::asprintf("%d", iloop);
				} else {
					retstr += "data_obj_with_type_member_" + mem->m_name;
				}
				retstr += ")\n";
			} else {
				retstr += name + "." + mem->m_name + "._SetValue(";
				if (mem->m_type->m_data_type == TYPE_BASE) {
					retstr += QString::asprintf("%d", iloop);
				} else {
					retstr += "data_obj_with_type_member_" + mem->m_name;
				}
				retstr += ")\n";
			}

			if (mem->m_name.left(1) == "_") {
				retstr += "#" + name + "." + mem->m_name + "._GetValue()        #not support access by dot\n";
				retstr += "print " + name + "._Member(\"" + mem->m_name + "\")._GetValue()";
				if (mem->m_type->m_data_type == TYPE_BASE) {
					retstr += "        #print " + QString::asprintf("%d", iloop++) + "\n";
				} else {
					retstr += "        #display data object detail of member " + mem->m_name + "\n";
				}
			} else {
				retstr += "print " + name + "." + mem->m_name + "._GetValue()";
				if (mem->m_type->m_data_type == TYPE_BASE) {
					retstr += "        #print " + QString::asprintf("%d", iloop++) + "\n";
				} else {
					retstr += "        #display data object detail of member " + mem->m_name + "\n";
				}
			}

			retstr += "print " + name + "._Member(\"" + mem->m_name + "\")._GetSize()    #print " + QString::asprintf("%d", mem->dataSize()) + "\n";
			retstr += "print " + name + "._Member(\"" + mem->m_name + "\")._GetAlign()    #print " + QString::asprintf("%d", mem->dataAlign()) + "\n";
		}
		break;
	}
	}
	return retstr;
}

unsigned int DwfTypeArray::dataSize()
{
	unsigned int numbers = 1;
	int levels = m_range_list.length();
	int iloop;
	for (iloop = 0; iloop < levels; iloop++) {
		int range = m_range_list[iloop];
		numbers *= range;
	}
	return m_type->dataSize() * numbers;
}

unsigned int DwfTypeArray::dataAlign()
{
	return m_type->dataAlign();
}

QString DwfTypeArray::makeScript(MakeScriptType type, QString name, QString addition)
{
	QString retstr;
	switch(type)
	{
	case MAKE_DEFINE:
	{
		retstr += typeDesc(this);
		break;
	}
	case MAKE_OPT_TYPE:
	{
		retstr += "print " + name + "        #print detail about the data type\n";
		retstr += "print " + name + "._GetSize()        #print " + QString::asprintf("%d", dataSize()) + ", size of array data type\n";
		retstr += "print " + name + "._GetAlign()        #print " + QString::asprintf("%d", dataAlign()) + ", alignment of array data type\n";
		retstr += "my_array_data_type_to = " + name + "._ToType()        #type of array\n";
		retstr += "print my_array_data_type_to._GetSize()        #print " + QString::asprintf("%d", m_type->dataSize()) + ", size of data type which define the array\n";
		retstr += "print my_array_data_type_to._GetAlign()        #print " + QString::asprintf("%d", m_type->dataAlign()) + ", alignment of data type which define the array\n";
		retstr += "#......\n";
		break;
	}
	case MAKE_TYPE_NEW:
	{
		retstr += addition + " = " + name + "()        #create new array data object.\n";
		break;
	}
	case MAKE_OPT_OBJ:
	{
		retstr += "print " + name + "        #print detail about the data object\n";
		retstr += "print " + name + "._GetSize()        #print " + QString::asprintf("%d", dataSize()) + "\n";
		retstr += "print " + name + "._GetAlign()        #print " + QString::asprintf("%d", dataAlign()) + "\n";
		retstr += "my_type_for_array = " + name + "._ToType()        #my_type_for_array is data type which define the array\n";
		int levels = m_range_list.length();
		int iloop;
		for (iloop = 0; iloop < levels; iloop++) {
			int range = m_range_list[iloop];
			int jloop;
			for (jloop = 0; jloop < range; jloop++) {
				if (jloop == 0 || jloop == 1 || jloop == range - 1) {
					QString var_name;
					if (iloop == levels - 1) {
						var_name = "my_sub_array";
					} else {
						var_name = "my_sub_data_object";
					}
					QString prefix;
					int kloop;
					for (kloop = 0; kloop < iloop; kloop++) {
						prefix += "[0]";
					}
					int number_remain = 1;
					int mloop;
					for (mloop = iloop; mloop < levels; mloop++) {
						int range = m_range_list[mloop];
						number_remain *= range;
					}
					retstr += var_name + " = " + name + prefix + "[" + QString::asprintf("%d", jloop) + "]\n";
					if (jloop == 0) {
						retstr += "print " + var_name + "._GetSize()        #print " + QString::asprintf("%d", number_remain * m_type->dataSize()) + "\n";
					}
				} else if (jloop == 2) {
					retstr += "#......\n";
				}
			}
			if (iloop != 0) {
				retstr += "#......\n";
			}
		}
		retstr += "#......\n";
		break;
	}
	}
	return retstr;
}

QString DwfTypeSubrt::makeScript(MakeScriptType type, QString name, QString addition)
{
	QString retstr;
	return retstr;
}

unsigned int DwfTypePoint::dataSize()
{
	Arch *arch = g_target->archInfoGet();
	if (arch == NULL) {
		return 4;			/*32bit*/
	}
	return arch->m_pointlen;
}

unsigned int DwfTypePoint::dataAlign()
{
	return dataSize();
}

QString DwfTypePoint::makeScript(MakeScriptType type, QString name, QString addition)
{
	QString retstr;
	switch(type)
	{
	case MAKE_DEFINE:
	{
		break;
	}
	case MAKE_OPT_TYPE:
	{
		retstr += "print " + name + "        #print detail about the data type\n";
		retstr += "print " + name + "._GetSize()        #print " + QString::asprintf("%u", dataSize()) + "\n";
		retstr += "print " + name + "._GetAlign()        #print " + QString::asprintf("%u", dataAlign()) + "\n";
		retstr += "my_point_data_type_to = " + name + "_ToType()        #type of point";
		retstr += "print my_point_data_type_to._GetSize()        #print " + QString::asprintf("%u", m_type->dataSize()) + "\n";
		retstr += "print my_point_data_type_to._GetAlign()        #print " + QString::asprintf("%u", m_type->dataAlign()) + "\n";
		retstr += "#......\n";
		break;
	}
	case MAKE_TYPE_NEW:
	{
		retstr += addition + " = " + name + "()        #create new point data object without initialization\n";
		retstr += addition + " = " + name + "(0x12345678)        #create new point data object with address number\n";
		retstr += addition + " = " + name + "(other_point_object_with_same_type)        #create new point data object with other point\n";
		retstr += addition + " = uter.point(other_data_type_with_type_point_to)        #create new point data object and point to the data object\n";
		break;
	}
	case MAKE_OPT_OBJ:
	{
		retstr += "print " + name + "        #print detail about the data object\n";
		retstr += "print " + name + "._GetSize()        #print " + QString::asprintf("%d", dataSize()) + "\n";
		retstr += "print " + name + "._GetAlign()        #print " + QString::asprintf("%d", dataAlign()) + "\n";
		retstr += "print " + name + "._GetValue()        #print address the object point to, if NULL, print 0\n";
		retstr += name + "._Setvalue(some_data_object._GetAddress())        #set address to point data object\n";
		retstr += "my_data_object_the_point_to = uter.depoint(" + name + ")        #Get the data object which the point object point to\n";
		retstr += "#......\n";
		break;
	}
	}
	return retstr;
}

unsigned int DwfTypeTypedef::dataSize()
{
	return m_type->dataSize();
}

unsigned int DwfTypeTypedef::dataAlign()
{
	return m_type->dataAlign();
}

QString DwfTypeTypedef::makeScript(MakeScriptType type, QString name, QString addition)
{
	QString retstr;
	switch(type)
	{
	case MAKE_DEFINE:
	{
		break;
	}
	case MAKE_OPT_TYPE:
	{
		retstr += "#The typedef data type is the final data type with typedef attribute\n";
		retstr += m_type->makeScript(MAKE_TYPE_NEW, name, addition);
		break;
	}
	case MAKE_TYPE_NEW:
	{
		retstr += m_type->makeScript(MAKE_TYPE_NEW, name, addition);
		break;
	}
	case MAKE_OPT_OBJ:
	{
		retstr += m_type->makeScript(MAKE_OPT_OBJ, name, addition);
		break;
	}
	}
	return retstr;
}

QString DwfVar::varDesc()
{
	QString desc;
	QString desc_before;
	QString desc_after;
	DwfType::typeDesc(m_type, desc_before, desc_after);
	desc = desc_before;
	if (desc_before.right(1) != "*") {
		desc += " ";
	}
	desc += m_name;
	desc += desc_after;
	return desc;
}

QString DwfPara::paraDesc()
{
	QString desc;
	QString desc_before;
	QString desc_after;
	DwfType::typeDesc(m_type, desc_before, desc_after);
	desc += desc_before;
	if (desc_before.right(1) != "*") {
		desc += " ";
	}
	desc += m_name;
	desc += desc_after;
	return desc;
}

QString DwfFun::funDesc()
{
	QString desc;
	QString desc_before;
	QString desc_after;
	DwfType::typeDesc(m_type, desc_before, desc_after);
	desc = desc_before + desc_after;
	if (desc.right(1) != "*") {
		desc += " ";
	}
	desc += m_name + "(";
	QList<DwfPara*>::iterator iter;
	for (iter = m_para_list.begin(); iter != m_para_list.end(); iter++) {
		DwfPara *dwfpara = *iter;
		if (iter != m_para_list.begin()) {
			desc += ", ";
		}
		desc += dwfpara->paraDesc();
	}
	desc += ")";
	return desc;
}

bool DwfCu::getDieHasAttr(Dwarf_Die some_die, Dwarf_Half some_attr)
{
	UTER_ASSERT(some_die != NULL);
	int result;
	Dwarf_Error error;

	const char *attr_name = "NULL";
	dwarf_get_AT_name(some_attr, &attr_name);

	Dwarf_Bool return_bool;
	result = dwarf_hasattr(some_die, some_attr, &return_bool, &error);
	if (result == DW_DLV_ERROR) {
		UTER_LOG("Get attribute %s:%d of die failed, description %s.",
					attr_name, some_attr, dwarf_errmsg(error));
		dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
		return false;
	}

	return return_bool;
}

int DwfCu::getDieStrAttr(Dwarf_Die some_die, Dwarf_Half some_strattr, QString &out)
{
	UTER_ASSERT(some_die != NULL);
	int result;
	Dwarf_Error error;
	Dwarf_Attribute attr;

	const char *attr_name = "NULL";
	dwarf_get_AT_name(some_strattr, &attr_name);

	result = dwarf_attr(some_die, some_strattr, &attr, &error);
	if(result == DW_DLV_NO_ENTRY) {
		UTER_LOG("No attribute %s:%d of die.", attr_name, some_strattr);
		return -1;
	} else if(result == DW_DLV_ERROR) {
		UTER_LOG("Get attribute %s:%d of die failed, description %s.",
					attr_name, some_strattr, dwarf_errmsg(error));
		dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
		return -1;
	}

	Dwarf_Half form;
	result = dwarf_whatform(attr, &form, &error);
	if(result == DW_DLV_ERROR) {
		UTER_LOG("Get form code for attribute %s:%d failed, description %s.",
					attr_name, some_strattr, dwarf_errmsg(error));
		dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
		dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
		return -1;
	}

	const char *form_name = "NULL";
	dwarf_get_FORM_name(form, &form_name);

	if (form == DW_FORM_strp) {
		char *return_str;
		result = dwarf_formstring(attr, &return_str, &error);
		dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
		if(result == DW_DLV_ERROR) {
			UTER_LOG("Get string by code %s:%d for attribute %s:%d failed, description %s.",
						form_name, form, attr_name, some_strattr, dwarf_errmsg(error));
			dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
			return -1;
		}
		out = return_str;
		dwarf_dealloc(m_dbg, return_str, DW_DLA_STRING);
		return 0;
	} else {
		UTER_LOG("Form code %s:%d of attribute %s:%d unexpected.",
					form_name, form, attr_name, some_strattr);
		dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
		return -1;
	}
}

int DwfCu::getDieRefAttr(Dwarf_Die some_die, Dwarf_Half some_refattr, unsigned long long &out)
{
	UTER_ASSERT(some_die != NULL);
	int result;
	Dwarf_Error error;
	Dwarf_Attribute attr;

	const char *attr_name = "NULL";
	dwarf_get_AT_name(some_refattr, &attr_name);

	result = dwarf_attr(some_die, some_refattr, &attr, &error);
	if(result == DW_DLV_NO_ENTRY) {
		UTER_LOG("No attribute %s:%d of die.", attr_name, some_refattr);
		return -1;
	} else if(result == DW_DLV_ERROR) {
		UTER_LOG("Get attribute %s:%d of die failed, description %s.",
					attr_name, some_refattr, dwarf_errmsg(error));
		dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
		return -1;
	}

	Dwarf_Half form;
	result = dwarf_whatform(attr, &form, &error);
	if(result == DW_DLV_ERROR) {
		UTER_LOG("Get form code for attribute %s:%d failed, description %s.",
					attr_name, some_refattr, dwarf_errmsg(error));
		dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
		dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
		return -1;
	}

	const char *form_name = "NULL";
	dwarf_get_FORM_name(form, &form_name);

	if (form == DW_FORM_sec_offset) {
	    Dwarf_Off ref_off;
	    result = dwarf_global_formref(attr, &ref_off, &error);
	    dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
	    if(result == DW_DLV_ERROR) {
			UTER_LOG("Get offset by code %s:%d for attribute %s:%d failed, description %s.",
						form_name, form, attr_name, some_refattr, dwarf_errmsg(error));
	    	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
	    	return -1;
	    }
	    out = ref_off;
	    return 0;
	} else {
		UTER_LOG("Form code %s:%d of attribute %s:%d unexpected.",
					form_name, form, attr_name, some_refattr);
		dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
		return -1;
	}
}

int DwfCu::getDieUdataAttr(Dwarf_Die some_die, Dwarf_Half some_udataattr, unsigned long long &out)
{
	UTER_ASSERT(some_die != NULL);
	int result;
	Dwarf_Error error;
	Dwarf_Attribute attr;

	const char *attr_name = "NULL";
	dwarf_get_AT_name(some_udataattr, &attr_name);

    result = dwarf_attr(some_die, some_udataattr, &attr, &error);
    if(result == DW_DLV_NO_ENTRY) {
    	UTER_LOG("No attribute %s:%d of die.", attr_name, some_udataattr);
        return -1;
    } else if(result == DW_DLV_ERROR) {
		UTER_LOG("Get attribute %s:%d of die failed, description %s.",
					attr_name, some_udataattr, dwarf_errmsg(error));
    	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
    	return -1;
    }

	Dwarf_Half form;
	result = dwarf_whatform(attr, &form, &error);
	if(result == DW_DLV_ERROR) {
		UTER_LOG("Get form code for attribute %s:%d failed, description %s.",
					attr_name, some_udataattr, dwarf_errmsg(error));
		dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
		dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
		return -1;
	}

	const char *form_name = "NULL";
	dwarf_get_FORM_name(form, &form_name);

	if (form == DW_FORM_data1 || form == DW_FORM_data2 || form == DW_FORM_data4) {
	    Dwarf_Unsigned udata;
	    result = dwarf_formudata(attr, &udata, &error);
	    dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
	    if(result == DW_DLV_ERROR) {
			UTER_LOG("Get unsigned data by code %s:%d for attribute %s:%d failed, description %s.",
						form_name, form, attr_name, some_udataattr, dwarf_errmsg(error));
	    	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
	    	return -1;
	    }
	    out = udata;
	    return 0;
	} else {
		UTER_LOG("Form code %s:%d of attribute %s:%d unexpected.",
					form_name, form, attr_name, some_udataattr);
		dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
		return -1;
	}
}

QString DwfCu::getDieName(Dwarf_Die named_die)
{
	UTER_ASSERT(named_die != NULL);
	Dwarf_Error error;
	QString retval;

	char *name;
	int result = dwarf_diename(named_die, &name, &error);
    if(result == DW_DLV_NO_ENTRY) {
    	UTER_LOG("No name of die.");
    	return "";
    } else if(result == DW_DLV_ERROR) {
    	UTER_LOG("Get name of die failed, description %s.", dwarf_errmsg(error));
    	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
    	return "";
    }

    retval = name;
    dwarf_dealloc(m_dbg, name, DW_DLA_STRING);
    return retval;
}

Dwarf_Off DwfCu::getDieOff(Dwarf_Die offd_die)
{
	UTER_ASSERT(offd_die != NULL);
	Dwarf_Error error;
	Dwarf_Off offset;

	int result = dwarf_dieoffset(offd_die, &offset, &error);
	if(result == DW_DLV_ERROR) {
	    UTER_LOG("Get offset of die failed, description %s.", dwarf_errmsg(error));
	    dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
	    return DWF_INVALID_OFF;
	}

	return offset;
}

/*return DWF_INVALID_SIZE when some mistake*/
unsigned int DwfCu::getDieSize(Dwarf_Die sized_die)
{
	UTER_ASSERT(sized_die != NULL);
	int result;
	Dwarf_Error error;
	Dwarf_Attribute attr;

	result = dwarf_attr(sized_die, DW_AT_byte_size, &attr, &error);
	if(result == DW_DLV_NO_ENTRY) {
		UTER_LOG("No size of die.");
		return DWF_INVALID_SIZE;
	} else if(result == DW_DLV_ERROR) {
		UTER_LOG("Get size of die failed, description %s.", dwarf_errmsg(error));
		dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
		return DWF_INVALID_SIZE;
	}

	Dwarf_Half form;
	result = dwarf_whatform(attr, &form, &error);
	if(result == DW_DLV_ERROR) {
		UTER_LOG("Get form code for size failed, description %s.", dwarf_errmsg(error));
		dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
		dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
		return DWF_INVALID_SIZE;
	}

	const char *form_name = "NULL";
	dwarf_get_FORM_name(form, &form_name);

	if (form == DW_FORM_data1 || form == DW_FORM_data2 || form == DW_FORM_data4) {
		Dwarf_Unsigned size;
		result = dwarf_formudata(attr, &size, &error);
		dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
		if(result == DW_DLV_ERROR) {
			UTER_LOG("Get unsigned data by code %s:%d for size failed, description %s.",
						form_name, form, dwarf_errmsg(error));
			dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
			return DWF_INVALID_SIZE;
		}
		return size;
    } else {
		UTER_LOG("Form code %s:%d of size unexpected.", form_name, form);
    	dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
    	return DWF_INVALID_SIZE;
    }
}

/*return DWF_INVALID_ADDR when some mistake*/
unsigned long long DwfCu::getDieLowPc(Dwarf_Die lowpcd_die)
{
	UTER_ASSERT(lowpcd_die != NULL);
	int result;
	Dwarf_Error error;
	Dwarf_Attribute attr;

    result = dwarf_attr(lowpcd_die, DW_AT_low_pc, &attr, &error);
    if (result == DW_DLV_NO_ENTRY) {
    	UTER_LOG("No lowpc of die.");
        return DWF_INVALID_ADDR;
    } else if(result == DW_DLV_ERROR) {
    	UTER_LOG("Get lowpc of die failed, description %s.", dwarf_errmsg(error));
    	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
    	return DWF_INVALID_ADDR;
    }

	Dwarf_Half form;
	result = dwarf_whatform(attr, &form, &error);
	if(result == DW_DLV_ERROR) {
		UTER_LOG("Get form code for lowpc failed, description %s.", dwarf_errmsg(error));
		dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
		dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
		return DWF_INVALID_ADDR;
	}

	const char *form_name = "NULL";
	dwarf_get_FORM_name(form, &form_name);

    if (form == DW_FORM_addr) {
        Dwarf_Addr lowpc;
        result = dwarf_formaddr(attr, &lowpc, &error);
        dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
        if(result == DW_DLV_ERROR) {
        	UTER_LOG("Get address by code %s:%d for lowpc failed, description %s.",
        				form_name, form, dwarf_errmsg(error));
        	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
        	return DWF_INVALID_ADDR;
        }
        return lowpc;
    } else {
		UTER_LOG("Form code %s:%d of lowpc unexpected.", form_name, form);
    	dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
    	return DWF_INVALID_ADDR;
    }
}

/*return DWF_INVALID_ADDR when some mistake*/
unsigned long long DwfCu::getDieHighPc(Dwarf_Die highpcd_die)
{
	UTER_ASSERT(highpcd_die != NULL);
	int result;
	Dwarf_Error error;
	Dwarf_Attribute attr;

    result = dwarf_attr(highpcd_die, DW_AT_high_pc, &attr, &error);
    if(result == DW_DLV_NO_ENTRY) {
    	UTER_LOG("No highpc of die.");
    	return DWF_INVALID_ADDR;
    } else if(result == DW_DLV_ERROR) {
    	UTER_LOG("Get highpc of die failed, description %s.", dwarf_errmsg(error));
    	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
    	return DWF_INVALID_ADDR;
    }

	Dwarf_Half form;
	result = dwarf_whatform(attr, &form, &error);
	if(result == DW_DLV_ERROR) {
		UTER_LOG("Get form code for highpc failed, description %s.", dwarf_errmsg(error));
		dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
		dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
		return DWF_INVALID_ADDR;
	}

	const char *form_name = "NULL";
	dwarf_get_FORM_name(form, &form_name);

	if (form == DW_FORM_data4) {
	    Dwarf_Unsigned highpc;
	    result = dwarf_formudata(attr, &highpc, &error);
	    dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
	    if(result == DW_DLV_ERROR) {
        	UTER_LOG("Get address by code %s:%d for highpc failed, description %s.",
        				form_name, form, dwarf_errmsg(error));
	    	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
	    	return DWF_INVALID_ADDR;
	    }
	    return highpc;
	} else {
		UTER_LOG("Form code %s:%d of highpc unexpected.", form_name, form);
		dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
		return DWF_INVALID_ADDR;
	}
}

/*return DWF_INVALID_OFF when some mistake*/
unsigned long long DwfCu::getDieTypeOn(Dwarf_Die typed_die)
{
	UTER_ASSERT(typed_die != NULL);
	int result;
	Dwarf_Error error;
	Dwarf_Attribute attr;

    result = dwarf_attr(typed_die, DW_AT_type, &attr, &error);
    if(result == DW_DLV_NO_ENTRY) {
    	UTER_LOG("No type offset of die.");
        return DWF_INVALID_OFF;
    } else if(result == DW_DLV_ERROR) {
    	UTER_LOG("Get type offset of die failed, description %s.", dwarf_errmsg(error));
    	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
    	return DWF_INVALID_OFF;
    }

	Dwarf_Half form;
	result = dwarf_whatform(attr, &form, &error);
	if(result == DW_DLV_ERROR) {
		UTER_LOG("Get form code for type offset failed, description %s.", dwarf_errmsg(error));
		dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
		dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
		return DWF_INVALID_OFF;
	}

	const char *form_name = "NULL";
	dwarf_get_FORM_name(form, &form_name);

	if (form == DW_FORM_ref4) {
		Dwarf_Off type_off;
	    result = dwarf_global_formref(attr, &type_off, &error);
	    dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
	    if(result == DW_DLV_ERROR) {
        	UTER_LOG("Get offset by code %s:%d for type offset failed, description %s.",
        				form_name, form, dwarf_errmsg(error));
	    	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
	    	return DWF_INVALID_OFF;
	    }
	    return type_off;
	} else {
		UTER_LOG("Form code %s:%d of type offset unexpected.", form_name, form);
		dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
		return DWF_INVALID_OFF;
	}
}

/*return DWF_INVALID_FILELINE when some mistake*/
unsigned long long DwfCu::getDieFileNo(Dwarf_Die nod_die)
{
	UTER_ASSERT(nod_die != NULL);
	int result;
	Dwarf_Error error;
	Dwarf_Attribute attr;

    result = dwarf_attr(nod_die, DW_AT_decl_file, &attr, &error);
    if(result == DW_DLV_NO_ENTRY) {
    	UTER_LOG("No file number of die.");
        return DWF_INVALID_FILELINE;
    } else if(result == DW_DLV_ERROR) {
    	UTER_LOG("Get file number of die failed, description %s.", dwarf_errmsg(error));
    	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
    	return DWF_INVALID_FILELINE;
    }

	Dwarf_Half form;
	result = dwarf_whatform(attr, &form, &error);
	if(result == DW_DLV_ERROR) {
		UTER_LOG("Get form code for file number failed, description %s.", dwarf_errmsg(error));
		dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
		dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
		return DWF_INVALID_FILELINE;
	}

	const char *form_name = "NULL";
	dwarf_get_FORM_name(form, &form_name);

	if (form == DW_FORM_data1) {
	    Dwarf_Unsigned file_index;
	    result = dwarf_formudata(attr, &file_index, &error);
	    dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
	    if(result == DW_DLV_ERROR) {
        	UTER_LOG("Get number by code %s:%d for file number failed, description %s.",
        				form_name, form, dwarf_errmsg(error));
	    	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
	    	return DWF_INVALID_FILELINE;
	    }
	    return file_index;
	} else {
		UTER_LOG("Form code %s:%d of file number unexpected.", form_name, form);
		dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
		return DWF_INVALID_FILELINE;
	}
}

/*return DWF_INVALID_FILELINE when some mistake*/
unsigned long long DwfCu::getDieLineNo(Dwarf_Die nod_die)
{
	UTER_ASSERT(nod_die != NULL);
	int result;
	Dwarf_Error error;
	Dwarf_Attribute attr;

    result = dwarf_attr(nod_die, DW_AT_decl_line, &attr, &error);
    if(result == DW_DLV_NO_ENTRY) {
    	UTER_LOG("No line number of die.");
        return DWF_INVALID_FILELINE;
    } else if(result == DW_DLV_ERROR) {
    	UTER_LOG("Get line number of die failed, description %s.", dwarf_errmsg(error));
    	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
    	return DWF_INVALID_FILELINE;
    }

	Dwarf_Half form;
	result = dwarf_whatform(attr, &form, &error);
	if(result == DW_DLV_ERROR) {
		UTER_LOG("Get form code for line number failed, description %s.", dwarf_errmsg(error));
		dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
		dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
		return DWF_INVALID_FILELINE;
	}

	const char *form_name = "NULL";
	dwarf_get_FORM_name(form, &form_name);

	if (form == DW_FORM_data1 || form == DW_FORM_data2) {
	    Dwarf_Unsigned line_index;
	    result = dwarf_formudata(attr, &line_index, &error);
	    dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
	    if(result == DW_DLV_ERROR) {
        	UTER_LOG("Get number by code %s:%d for line number failed, description %s.",
        				form_name, form, dwarf_errmsg(error));
	    	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
	    	return DWF_INVALID_FILELINE;
	    }
	    return line_index;
	} else {
		UTER_LOG("Form code %s:%d of line number unexpected.", form_name, form);
		dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
		return DWF_INVALID_FILELINE;
	}
}

bool DwfCu::getDieIsExtern(Dwarf_Die exted_die)
{
	UTER_ASSERT(exted_die != NULL);
	int result;
	Dwarf_Error error;
	Dwarf_Attribute attr;

    result = dwarf_attr(exted_die, DW_AT_external, &attr, &error);
    if(result == DW_DLV_NO_ENTRY) {
    	/*No log here*/
    	return false;
    } else if(result == DW_DLV_ERROR) {
    	UTER_LOG("Get external failed, description %s.", dwarf_errmsg(error));
    	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
    	return false;
    }

	Dwarf_Half form;
	result = dwarf_whatform(attr, &form, &error);
	if(result == DW_DLV_ERROR) {
		UTER_LOG("Get form code for external failed, description %s.", dwarf_errmsg(error));
		dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
		dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
		return false;
	}

	const char *form_name = "NULL";
	dwarf_get_FORM_name(form, &form_name);

	if (form == DW_FORM_flag_present) {
    	Dwarf_Bool ext_flag;
        result = dwarf_formflag(attr, &ext_flag, &error);
        dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
        if(result == DW_DLV_ERROR) {
        	UTER_LOG("Get bool data by code %s:%d for external failed, description %s.",
        				form_name, form, dwarf_errmsg(error));
        	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
        	return false;
        }
        return ext_flag;
	} else {
		UTER_LOG("Form code %s:%d of external unexpected.", form_name, form);
		dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
		return false;
	}
}

bool DwfCu::getDieIsDeclare(Dwarf_Die declared_die)
{
	UTER_ASSERT(declared_die != NULL);
	int result;
	Dwarf_Error error;
	Dwarf_Attribute attr;

    result = dwarf_attr(declared_die, DW_AT_declaration, &attr, &error);
    if(result == DW_DLV_NO_ENTRY) {
    	/*No log here*/
    	return false;
    } else if(result == DW_DLV_ERROR) {
    	UTER_LOG("Get declare failed, description %s.", dwarf_errmsg(error));
    	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
    	return false;
    }

	Dwarf_Half form;
	result = dwarf_whatform(attr, &form, &error);
	if(result == DW_DLV_ERROR) {
		UTER_LOG("Get form code for declare failed, description %s.", dwarf_errmsg(error));
		dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
		dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
		return false;
	}

	const char *form_name = "NULL";
	dwarf_get_FORM_name(form, &form_name);

	if (form == DW_FORM_flag_present) {
    	Dwarf_Bool ext_flag;
        result = dwarf_formflag(attr, &ext_flag, &error);
        dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
        if(result == DW_DLV_ERROR) {
        	UTER_LOG("Get bool data by code %s:%d for declare failed, description %s.",
        				form_name, form, dwarf_errmsg(error));
        	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
        	return false;
        }
        return ext_flag;
	} else {
		UTER_LOG("Form code %s:%d of declare unexpected.", form_name, form);
		dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
		return false;
	}
}

DwfTypeVoid g_dwf_type_void(NULL);

/* Get DwfType by die offset, get DwfType on records first.*/
DwfType *DwfCu::getTypeOnOff(unsigned long long die_off)
{
	int result;
	Dwarf_Error error;

    DwfItem *item = itemGetByOff(die_off);
    if (item != NULL) {
    	if (item->m_element_type != DwfItem::DWF_ITEM_TYPE) {
    		UTER_LOG("Element on offset 0x%016llx not data type.", die_off);
    		return NULL;
    	}
    	return (DwfType *)item;
    } else {
    	/*Make new DwfType from offset*/
    	Dwarf_Die type_die;
    	result = dwarf_offdie(m_dbg, die_off, &type_die, &error);
    	if (result == DW_DLV_NO_ENTRY) {
    		UTER_LOG("No die on offset 0x%016llx.", die_off);
    		return NULL;
    	} else if (result == DW_DLV_ERROR) {
        	UTER_LOG("Get die on offset 0x%016llx failed, description %s.", die_off, dwarf_errmsg(error));
        	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
        	return NULL;
    	}
    	/*TODO:*/
    	//if (getDieHasAttr(type_die, DW_AT_declaration)) {
    	//}
    	DwfType *new_type = parseType(type_die);
    	dwarf_dealloc(m_dbg, type_die, DW_DLA_DIE);
    	if (new_type == NULL) {
    		UTER_LOG("Parse type for die :0x%016llx failed.", die_off);
    	}
    	return new_type;
    }
}

DwfType *DwfCu::getTypeForDie(Dwarf_Die typed_die)
{
	if (getDieHasAttr(typed_die, DW_AT_type)) {
		Dwarf_Off offset = getDieOff(typed_die);
		/*Get type offset*/
		Dwarf_Off type_off = getDieTypeOn(typed_die);
		if (type_off == DWF_INVALID_OFF) {
			UTER_LOG("Get type offset of die :0x%016llx failed.", offset);
			return NULL;
		}
		DwfType *type = getTypeOnOff(type_off);
	    if (type == NULL) {
	    	UTER_LOG("Get type by offset 0x%016llx of die :0x%016llx failed.", type_off, offset);
	    	return NULL;
	    }
	    return type;
	} else {
		/*Void type*/
		return &g_dwf_type_void;
	}
}

DwfTypeBase *DwfCu::parseTypeBase(Dwarf_Die type_die)
{
	UTER_ASSERT(type_die != NULL);
	int result;
	Dwarf_Error error;
	Dwarf_Attribute attr;

	Dwarf_Off offset = getDieOff(type_die);
	if (offset == DWF_INVALID_OFF) {
		return NULL;
	}
	DwfTypeBase *type = new DwfTypeBase(this, offset);
	/*Insert to records first*/
	itemInsert((DwfItem *)type);

    type->m_name = getDieName(type_die);
    if (type->m_name == "") {
    	UTER_LOG("No name of base type die :0x%016llx.", offset);
    	goto error_ret;
    }
    type->m_size = getDieSize(type_die);
    if (type->m_size == DWF_INVALID_SIZE) {
    	UTER_LOG("No size of base type die %s on offset 0x%016llx.",
    				type->m_name.toStdString().c_str(), offset);
    	goto error_ret;
    }
    if (type->m_name == "signed char" || type->m_name == "char") {
    	type->m_base_type = DwfTypeBase::BASE_CHAR;
    } else if (type->m_name == "unsigned char") {
    	type->m_base_type = DwfTypeBase::BASE_UCHAR;
    } else if (type->m_name == "short signal int" || type->m_name == "signal short int" || type->m_name == "short int") {
    	type->m_base_type = DwfTypeBase::BASE_SHORT;
    } else if (type->m_name == "short unsigned int" || type->m_name == "unsigned short int") {
    	type->m_base_type = DwfTypeBase::BASE_USHORT;
    } else if (type->m_name == "signal int" || type->m_name == "int") {
    	type->m_base_type = DwfTypeBase::BASE_INT;
    } else if (type->m_name == "unsigned int") {
    	type->m_base_type = DwfTypeBase::BASE_UINT;
    } else if (type->m_name == "signal long int" || type->m_name == "long int") {
    	type->m_base_type = DwfTypeBase::BASE_LONG;
    } else if (type->m_name == "long unsigned int" || type->m_name == "unsigned long int") {
    	type->m_base_type = DwfTypeBase::BASE_ULONG;
    } else if (type->m_name == "signed long long int" || type->m_name == "long long int") {
    	type->m_base_type = DwfTypeBase::BASE_LONGLONG;
    } else if (type->m_name == "long long unsigned int" || type->m_name == "unsigned long long int") {
    	type->m_base_type = DwfTypeBase::BASE_ULONGLONG;
    } else if (type->m_name == "float") {
    	type->m_base_type = DwfTypeBase::BASE_FLOAT;
    } else if (type->m_name == "double") {
    	type->m_base_type = DwfTypeBase::BASE_DOUBLE;
    } else if (type->m_name == "sizetype") {
    	type->m_base_type = DwfTypeBase::BASE_SIZETYPE;
    }else {
    	UTER_LOG("Unknown type of base type die %s on offset 0x%016llx.",
    				type->m_name.toStdString().c_str(), offset);
    	goto error_ret;
    }
	return type;
error_ret:
	itemRemove((DwfItem *)type);
	delete type;
	return NULL;
}

DwfTypeEnMem *DwfCu::parseTypeEnMem(Dwarf_Die type_die)
{
	UTER_ASSERT(type_die != NULL);
	int result;
	Dwarf_Error error;
	Dwarf_Attribute attr;

	Dwarf_Off offset = getDieOff(type_die);
	if (offset == DWF_INVALID_OFF) {
		return NULL;
	}
	DwfTypeEnMem *type = new DwfTypeEnMem(this);

	type->m_name = getDieName(type_die);
    if (type->m_name == "") {
    	UTER_LOG("Get name of enum member die :0x%016llx failed.", offset);
    	goto error_ret;
    }
    /*No source file and line information in enumerator member*/
    /*Enumerator value:DW_AT_const_value  DW_FORM_sdata*/
	result = dwarf_attr(type_die, DW_AT_const_value, &attr, &error);
    if(result == DW_DLV_NO_ENTRY) {
    	UTER_LOG("No const value attribute of enum member die %s:0x%016llx.",
    				type->m_name.toStdString().c_str(), offset);
        goto error_ret;
    } else if(result == DW_DLV_ERROR) {
    	UTER_LOG("Get const value attribute of enum member die %s:0x%016llx failed, description %s.",
    				type->m_name.toStdString().c_str(), offset, dwarf_errmsg(error));
    	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
    	goto error_ret;
    }
    Dwarf_Signed const_value;
    result = dwarf_formsdata(attr, &const_value, &error);
    dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
    if(result == DW_DLV_ERROR) {
    	UTER_LOG("Get const value in attribute of enum member die %s:0x%016llx failed, description %s.",
    				type->m_name.toStdString().c_str(), offset, dwarf_errmsg(error));
    	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
    	goto error_ret;
    }
    type->m_value = const_value;
    return type;
error_ret:
	delete type;
	return NULL;
}

DwfTypeEn *DwfCu::parseTypeEn(Dwarf_Die type_die)
{
	UTER_ASSERT(type_die != NULL);
	int result;
	Dwarf_Error error;
	Dwarf_Attribute attr;

	Dwarf_Off offset = getDieOff(type_die);
	if (offset == DWF_INVALID_OFF) {
		return NULL;
	}
	DwfTypeEn *type = new DwfTypeEn(this, offset);
	/*Insert to records first*/
	itemInsert((DwfItem *)type);

	type->m_name = "";
	if (getDieHasAttr(type_die, DW_AT_name)) {
		type->m_name = getDieName(type_die);
	    if (type->m_name == "") {
	    	UTER_LOG("Get name of enum die :0x%016llx failed.", offset);
	    	goto error_ret;
	    }
	}
	if (getDieHasAttr(type_die, DW_AT_declaration)) {
		type->m_declare = true;
		return type;
	}
    type->m_size = getDieSize(type_die);
    if (type->m_size == DWF_INVALID_SIZE) {
    	UTER_LOG("Get size of enum die %s:0x%016llx failed.",
    				type->m_name.toStdString().c_str(), offset);
    	goto error_ret;
    }
    type->m_fileno = getDieFileNo(type_die);
    if (type->m_fileno == DWF_INVALID_FILELINE) {
    	UTER_LOG("Get file number of enum die %s:0x%016llx failed.",
    				type->m_name.toStdString().c_str(), offset);
    	goto error_ret;
    }
    type->m_lineno = getDieLineNo(type_die);
    if (type->m_lineno == DWF_INVALID_FILELINE) {
    	UTER_LOG("Get line number of enum die %s:0x%016llx failed.",
    				type->m_name.toStdString().c_str(), offset);
    	goto error_ret;
    }
	Dwarf_Die member_die;
	result = dwarf_child(type_die, &member_die, &error);
	while (1) {
		if (result == DW_DLV_NO_ENTRY) {
			/*End of child foreach*/
			break;
		} else if (result == DW_DLV_ERROR) {
	    	UTER_LOG("Get first or next child in enum die %s:0x%016llx failed, description %s.",
	    				type->m_name.toStdString().c_str(), offset, dwarf_errmsg(error));
	    	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
			break;
		}

		DwfTypeEnMem *member = parseTypeEnMem(member_die);
		if (member == NULL) {
			UTER_LOG("Parse member in enum die %s:0x%016llx failed.",
						type->m_name.toStdString().c_str(), offset);
			goto break_loop;
		}
		type->m_member_list.push_back(member);

		Dwarf_Die next_member_die;
		result = dwarf_siblingof(m_dbg, member_die, &next_member_die, &error);
		dwarf_dealloc(m_dbg, member_die, DW_DLA_DIE);
		member_die = next_member_die;
		continue;
break_loop:
		dwarf_dealloc(m_dbg, member_die, DW_DLA_DIE);
		goto error_ret;
	}
	return type;
error_ret:
	itemRemove((DwfItem *)type);
	delete type;
	return NULL;
}

DwfTypeStMem *DwfCu::parseTypeStMem(Dwarf_Die type_die)
{
	UTER_ASSERT(type_die != NULL);
	int result;
	Dwarf_Error error;
	Dwarf_Attribute attr;

	Dwarf_Off offset = getDieOff(type_die);
	if (offset == DWF_INVALID_OFF) {
		return NULL;
	}
	DwfTypeStMem *type = new DwfTypeStMem(this);
	type->m_name = getDieName(type_die);
	if (type->m_name == "") {
		UTER_LOG("Get name of struct member die :0x%016llx failed.", offset);
		goto error_ret;
	}
    type->m_fileno = getDieFileNo(type_die);
    if (type->m_fileno == DWF_INVALID_FILELINE) {
    	UTER_LOG("Get file number of struct member die %s:0x%016llx failed.",
    				type->m_name.toStdString().c_str(), offset);
    	goto error_ret;
    }
    type->m_lineno = getDieLineNo(type_die);
    if (type->m_lineno == DWF_INVALID_FILELINE) {
    	UTER_LOG("Get line number of struct member die %s:0x%016llx failed.",
    				type->m_name.toStdString().c_str(), offset);
    	goto error_ret;
    }

    Dwarf_Off type_off;
    type_off = getDieTypeOn(type_die);
    if (type_off == DWF_INVALID_OFF) {
    	UTER_LOG("Get type offset of struct member die %s:0x%016llx failed.",
    				type->m_name.toStdString().c_str(), offset);
    	goto error_ret;
    }
    type->m_type = getTypeOnOff(type_off);
    if (type->m_type == NULL) {
    	UTER_LOG("Get type by offset 0x%016llx of struct member die %s:0x%016llx failed.",
    				type_off, type->m_name.toStdString().c_str(), offset);
    	goto error_ret;
    }

    /*Get member offset in struct*/
    result = getDieUdataAttr(type_die, DW_AT_data_member_location, type->m_member_location);
    if (result < 0) {
    	UTER_LOG("Get location in struct of struct member die %s:0x%016llx failed.",
    				type->m_name.toStdString().c_str(), offset);
    	goto error_ret;
    }
    return type;
error_ret:
	delete type;
	return NULL;
}

DwfTypeSt *DwfCu::parseTypeSt(Dwarf_Die type_die)
{
	UTER_ASSERT(type_die != NULL);
	int result;
	Dwarf_Error error;
	Dwarf_Attribute attr;

	Dwarf_Off offset = getDieOff(type_die);
	if (offset == DWF_INVALID_OFF) {
		return NULL;
	}
	DwfTypeSt *type = new DwfTypeSt(this, offset);
	/*Insert to records first*/
	itemInsert((DwfItem *)type);

	type->m_name = "";
	if (getDieHasAttr(type_die, DW_AT_name)) {
		type->m_name = getDieName(type_die);
	    if (type->m_name == "") {
	    	UTER_LOG("Get name of struct die :0x%016llx failed.", offset);
	    	goto error_ret;
	    }
	}
	if (getDieHasAttr(type_die, DW_AT_declaration)) {
		/*TODO: About declare in script*/
		type->m_declare = true;
		return type;
	}
    type->m_size = getDieSize(type_die);
    if (type->m_size == DWF_INVALID_SIZE) {
    	UTER_LOG("Get size of struct die %s:0x%016llx failed.",
    				type->m_name.toStdString().c_str(), offset);
    	goto error_ret;
    }
    type->m_fileno = getDieFileNo(type_die);
    if (type->m_fileno == DWF_INVALID_FILELINE) {
    	UTER_LOG("Get file number of struct die %s:0x%016llx failed.",
    				type->m_name.toStdString().c_str(), offset);
    	goto error_ret;
    }
    type->m_lineno = getDieLineNo(type_die);
    if (type->m_lineno == DWF_INVALID_FILELINE) {
    	UTER_LOG("Get line number of struct die %s:0x%016llx failed.",
    				type->m_name.toStdString().c_str(), offset);
    	goto error_ret;
    }

	Dwarf_Die member_die;
	result = dwarf_child(type_die, &member_die, &error);
	while (1) {
		if (result == DW_DLV_NO_ENTRY) {
			/*End of child foreach*/
			break;
		} else if (result == DW_DLV_ERROR) {
	    	UTER_LOG("Get first or next child in struct die %s:0x%016llx failed, description %s.",
	    				type->m_name.toStdString().c_str(), offset, dwarf_errmsg(error));
	    	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
			break;
		}

		DwfTypeStMem *member = parseTypeStMem(member_die);
		if (member == NULL) {
			UTER_LOG("Parse member in struct die %s:0x%016llx failed.",
						type->m_name.toStdString().c_str(), offset);
			goto break_loop;
		}
		type->m_member_list.push_back(member);

		Dwarf_Die next_member_die;
		result = dwarf_siblingof(m_dbg, member_die, &next_member_die, &error);
		dwarf_dealloc(m_dbg, member_die, DW_DLA_DIE);
		member_die = next_member_die;
		continue;
break_loop:
		dwarf_dealloc(m_dbg, member_die, DW_DLA_DIE);
		goto error_ret;
	}
	return type;
error_ret:
	itemRemove((DwfItem *)type);
	delete type;
	return NULL;
}

DwfTypeUnMem *DwfCu::parseTypeUnMem(Dwarf_Die type_die)
{
	UTER_ASSERT(type_die != NULL);
	int result;
	Dwarf_Error error;
	Dwarf_Attribute attr;

	Dwarf_Off offset = getDieOff(type_die);
	if (offset == DWF_INVALID_OFF) {
		return NULL;
	}
	DwfTypeUnMem *type = new DwfTypeUnMem(this);
	type->m_name = getDieName(type_die);
	if (type->m_name == "") {
		UTER_LOG("Get name of union member die :0x%016llx failed.", offset);
		goto error_ret;
	}

    type->m_fileno = getDieFileNo(type_die);
    if (type->m_fileno == DWF_INVALID_FILELINE) {
    	UTER_LOG("Get file number of union member die %s:0x%016llx failed.",
    				type->m_name.toStdString().c_str(), offset);
    	goto error_ret;
    }
    type->m_lineno = getDieLineNo(type_die);
    if (type->m_lineno == DWF_INVALID_FILELINE) {
    	UTER_LOG("Get line number of union member die %s:0x%016llx failed.",
    				type->m_name.toStdString().c_str(), offset);
    	goto error_ret;
    }

    Dwarf_Off type_off;
    type_off = getDieTypeOn(type_die);
    if (type_off == DWF_INVALID_OFF) {
    	UTER_LOG("Get type offset of union member die %s:0x%016llx failed.",
    				type->m_name.toStdString().c_str(), offset);
    	goto error_ret;
    }
    type->m_type = getTypeOnOff(type_off);
    if (type->m_type == NULL) {
    	UTER_LOG("Get type by offset 0x%016llx of union member die %s:0x%016llx failed.",
    				type_off, type->m_name.toStdString().c_str(), offset);
    	goto error_ret;
    }

    return type;
error_ret:
	delete type;
	return NULL;
}

DwfTypeUn *DwfCu::parseTypeUn(Dwarf_Die type_die)
{
	UTER_ASSERT(type_die != NULL);
	int result;
	Dwarf_Error error;
	Dwarf_Attribute attr;

	Dwarf_Off offset = getDieOff(type_die);
	if (offset == DWF_INVALID_OFF) {
		return NULL;
	}
	DwfTypeUn *type = new DwfTypeUn(this, offset);
	/*Insert to records first*/
	itemInsert((DwfItem *)type);

	type->m_name = "";
	if (getDieHasAttr(type_die, DW_AT_name)) {
		type->m_name = getDieName(type_die);
	    if (type->m_name == "") {
	    	UTER_LOG("Get name of union die :0x%016llx failed.", offset);
	    	goto error_ret;
	    }
	}
	if (getDieHasAttr(type_die, DW_AT_declaration)) {
		type->m_declare = true;
		return type;
	}
    type->m_size = getDieSize(type_die);
    if (type->m_size == DWF_INVALID_SIZE) {
    	UTER_LOG("Get size of union die %s:0x%016llx failed.",
    				type->m_name.toStdString().c_str(), offset);
    	goto error_ret;
    }
    type->m_fileno = getDieFileNo(type_die);
    if (type->m_fileno == DWF_INVALID_FILELINE) {
    	UTER_LOG("Get file number of union die %s:0x%016llx failed.",
    				type->m_name.toStdString().c_str(), offset);
    	goto error_ret;
    }
    type->m_lineno = getDieLineNo(type_die);
    if (type->m_lineno == DWF_INVALID_FILELINE) {
    	UTER_LOG("Get line number of union die %s:0x%016llx failed.",
    				type->m_name.toStdString().c_str(), offset);
    	goto error_ret;
    }

	Dwarf_Die member_die;
	result = dwarf_child(type_die, &member_die, &error);
	while (1) {
		if (result == DW_DLV_NO_ENTRY) {
			/*End of child foreach*/
			break;
		} else if (result == DW_DLV_ERROR) {
	    	UTER_LOG("Get first or next child in union die %s:0x%016llx failed, description %s.",
	    				type->m_name.toStdString().c_str(), offset, dwarf_errmsg(error));
	    	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
			break;
		}

		DwfTypeUnMem *member = parseTypeUnMem(member_die);
		if (member == NULL) {
			UTER_LOG("Parse member in union die %s:0x%016llx failed.",
						type->m_name.toStdString().c_str(), offset);
			goto break_loop;
		}
		type->m_member_list.push_back(member);

		Dwarf_Die next_member_die;
		result = dwarf_siblingof(m_dbg, member_die, &next_member_die, &error);
		dwarf_dealloc(m_dbg, member_die, DW_DLA_DIE);
		member_die = next_member_die;
		continue;
break_loop:
		dwarf_dealloc(m_dbg, member_die, DW_DLA_DIE);
		goto error_ret;
	}
	return type;
error_ret:
	itemRemove((DwfItem *)type);
	delete type;
	return NULL;
}

DwfTypeSubrtPara *DwfCu::parseTypeSubrtPara(Dwarf_Die type_die)
{
	UTER_ASSERT(type_die != NULL);

	Dwarf_Off offset = getDieOff(type_die);
	if (offset == DWF_INVALID_OFF) {
		return NULL;
	}
	DwfTypeSubrtPara *type = new DwfTypeSubrtPara(this);

	/*No name,file,line for DwfTypeSubrtPara*/

	Dwarf_Off type_off = getDieTypeOn(type_die);
	if (type_off == DWF_INVALID_OFF) {
    	UTER_LOG("Get type offset of subrouting parameter die :0x%016llx failed.", offset);
		goto error_ret;
	}
    type->m_type = getTypeOnOff(type_off);
    if (type->m_type == NULL) {
    	UTER_LOG("Get type by offset 0x%016llx of subrouting parameter die :0x%016llx failed.", type_off, offset);
    	goto error_ret;
    }
	return type;
error_ret:
	delete type;
	return NULL;
}

DwfTypeSubrt *DwfCu::parseTypeSubrt(Dwarf_Die type_die)
{
	UTER_ASSERT(type_die != NULL);
	int result;
	Dwarf_Error error;

	Dwarf_Off offset = getDieOff(type_die);
	if (offset == DWF_INVALID_OFF) {
		return NULL;
	}
	DwfTypeSubrt *type = new DwfTypeSubrt(this, offset);
	/*Insert to records first*/
	itemInsert((DwfItem *)type);

	/*Return type*/
	type->m_type = getTypeForDie(type_die);
    if (type->m_type == NULL) {
    	UTER_LOG("Get type of subrouting die :0x%016llx failed.", offset);
    	goto error_ret;
    }

	Dwarf_Die para_die;
	result = dwarf_child(type_die, &para_die, &error);
	while (1) {
		if (result == DW_DLV_NO_ENTRY) {
			/*End of child foreach*/
			break;
		} else if (result == DW_DLV_ERROR) {
	    	UTER_LOG("Get first or next parameter in subrouting die :0x%016llx failed, description %s.",
	    				offset, dwarf_errmsg(error));
	    	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
			break;
		}

		DwfTypeSubrtPara *para = parseTypeSubrtPara(para_die);
		if (para == NULL) {
			UTER_LOG("Parse parameter in subrouting die %s:0x%016llx failed.",
						type->m_name.toStdString().c_str(), offset);
			goto break_loop;
		}
		type->m_para_list.push_back(para);

		Dwarf_Die next_para_die;
		result = dwarf_siblingof(m_dbg, para_die, &next_para_die, &error);
		dwarf_dealloc(m_dbg, para_die, DW_DLA_DIE);
		para_die = next_para_die;
		continue;
break_loop:
		dwarf_dealloc(m_dbg, para_die, DW_DLA_DIE);
		goto error_ret;
	}
	return type;
error_ret:
	itemRemove((DwfItem *)type);
	delete type;
	return NULL;
}

DwfTypePoint *DwfCu::parseTypePoint(Dwarf_Die type_die)
{
	UTER_ASSERT(type_die != NULL);

	Dwarf_Off offset = getDieOff(type_die);
	if (offset == DWF_INVALID_OFF) {
		return NULL;
	}
	DwfTypePoint *type = new DwfTypePoint(this, offset);
	/*Insert to records first*/
	itemInsert((DwfItem *)type);

    type->m_size = getDieSize(type_die);
    if (type->m_size == DWF_INVALID_SIZE) {
    	UTER_LOG("Get size of point die :0x%016llx failed.", offset);
    	goto error_ret;
    }

    type->m_type = getTypeForDie(type_die);
    if (type->m_type == NULL) {
    	UTER_LOG("Get type of point die :0x%016llx failed.", offset);
    	goto error_ret;
    }

	return type;
error_ret:
	itemRemove((DwfItem *)type);
	delete type;
	return NULL;
}

DwfTypeArray *DwfCu::parseTypeArray(Dwarf_Die type_die)
{
	UTER_ASSERT(type_die != NULL);
	int result;
	Dwarf_Error error;

	Dwarf_Off offset = getDieOff(type_die);
	if (offset == DWF_INVALID_OFF) {
		return NULL;
	}
	DwfTypeArray *type = new DwfTypeArray(this, offset);
	/*Insert to records first*/
	itemInsert((DwfItem *)type);

	Dwarf_Off type_off = getDieTypeOn(type_die);
    if (type_off == DWF_INVALID_OFF) {
    	UTER_LOG("Get type offset of array die :0x%016llx failed.", offset);
    	goto error_ret;
    }
    type->m_type = getTypeOnOff(type_off);
    if (type->m_type == NULL) {
    	UTER_LOG("Get type by offset 0x%016llx of array die :0x%016llx failed.", type_off, offset);
    	goto error_ret;
    }

	Dwarf_Die bound_die;
	result = dwarf_child(type_die, &bound_die, &error);
	while (1) {
		if (result == DW_DLV_NO_ENTRY) {
			/*End of child foreach*/
			break;
		} else if (result == DW_DLV_ERROR) {
	    	UTER_LOG("Get first or next bound in array die :0x%016llx failed, description %s.",
	    				offset, dwarf_errmsg(error));
	    	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
			break;
		}

		/*Switch type*/
        Dwarf_Half tagval;
        result = dwarf_tag(bound_die, &tagval, &error);
        if(result == DW_DLV_ERROR) {
        	UTER_LOG("Get tag for bound in array die :0x%016llx, description %s.",
        				offset, dwarf_errmsg(error));
        	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
        	goto break_loop;
        }
        if (tagval != DW_TAG_subrange_type) {
        	goto continue_loop;
        }

        /*Get bound: DW_TAG_subrange_type*/
        unsigned long long range;
        if (getDieHasAttr(bound_die, DW_AT_upper_bound)) {
        	Dwarf_Unsigned upper_bound;
            result = getDieUdataAttr(bound_die, DW_AT_upper_bound, upper_bound);
            if (result < 0) {
            	UTER_LOG("Get bound in array die :0x%016llx failed.", offset);
            	goto break_loop;
            }
            range = upper_bound + 1;
        } else {
        	range = 0;
        }

        type->m_range_list.push_back(range);

continue_loop:
        Dwarf_Die next_bound_die;
        result = dwarf_siblingof(m_dbg, bound_die, &next_bound_die, &error);
    	dwarf_dealloc(m_dbg, bound_die, DW_DLA_DIE);
    	bound_die = next_bound_die;
        continue;
break_loop:
		dwarf_dealloc(m_dbg, bound_die, DW_DLA_DIE);
		goto error_ret;
	}
	return type;
error_ret:
	itemRemove((DwfItem *)type);
	delete type;
	return NULL;
}

DwfTypeTypedef *DwfCu::parseTypeTypedef(Dwarf_Die type_die)
{
	UTER_ASSERT(type_die != NULL);

	Dwarf_Off offset = getDieOff(type_die);
	if (offset == DWF_INVALID_OFF) {
		return NULL;
	}
	DwfTypeTypedef *type = new DwfTypeTypedef(this, offset);
	/*Insert to records first*/
	itemInsert((DwfItem *)type);

	type->m_name = getDieName(type_die);
	if (type->m_name == NULL) {
    	UTER_LOG("Get name of typedef die :0x%016llx failed.", offset);
    	goto error_ret;
	}

    type->m_fileno = getDieFileNo(type_die);
    if (type->m_fileno == DWF_INVALID_FILELINE) {
    	UTER_LOG("Get file number of typedef die %s:0x%016llx failed.",
    				type->m_name.toStdString().c_str(), offset);
    	goto error_ret;
    }
    type->m_lineno = getDieLineNo(type_die);
    if (type->m_lineno == DWF_INVALID_FILELINE) {
    	UTER_LOG("Get line number of typedef die %s:0x%016llx failed.",
    				type->m_name.toStdString().c_str(), offset);
    	goto error_ret;
    }

    type->m_type = getTypeForDie(type_die);
	if (type->m_type == NULL) {
    	UTER_LOG("Get type of typedef die %s:0x%016llx failed.",
    				type->m_name.toStdString().c_str(), offset);
		goto error_ret;
	}

    return type;
error_ret:
	itemRemove((DwfItem *)type);
	delete type;
	return NULL;
}

/*All of type*/
DwfType *DwfCu::parseType(Dwarf_Die type_die)
{
	UTER_ASSERT(type_die != NULL);
	int result;
	Dwarf_Error error;

    Dwarf_Half tagval;
    result = dwarf_tag(type_die, &tagval, &error);
    if(result == DW_DLV_ERROR) {
    	UTER_LOG("Get die tag failed, description %s.", dwarf_errmsg(error));
    	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
    	return NULL;
    }

    if (tagval == DW_TAG_const_type) {
    	DwfType *const_to_type = getTypeForDie(type_die);
    	if (const_to_type == NULL) {
        	UTER_LOG("Get type of const die failed.");
        	return NULL;
    	} else if (const_to_type == &g_dwf_type_void) {
    		return const_to_type;
    	} else {
            const_to_type->m_const = true;
            return const_to_type;
    	}
    }

    if (tagval == DW_TAG_restrict_type
        || tagval == DW_TAG_volatile_type) {
    	DwfType *to_type = getTypeForDie(type_die);
    	if (to_type == NULL) {
        	UTER_LOG("Get type of special die failed.");
        	return NULL;
    	} else {
    		return to_type;
    	}
    }

    switch(tagval)
    {
    case DW_TAG_base_type:
    {
    	return parseTypeBase(type_die);
    }
    case DW_TAG_enumeration_type:
    {
    	return parseTypeEn(type_die);
    }
    case DW_TAG_structure_type:
    {
    	return parseTypeSt(type_die);
    }
    case DW_TAG_union_type:
    {
    	return parseTypeUn(type_die);
    }
    case DW_TAG_array_type:
    {
    	return parseTypeArray(type_die);
    }
    case DW_TAG_subroutine_type:
    {
    	return parseTypeSubrt(type_die);
    }
    case DW_TAG_pointer_type:
    {
    	return parseTypePoint(type_die);
    }
    case DW_TAG_typedef:
    {
    	return parseTypeTypedef(type_die);
    }
    case DW_TAG_reference_type:
    {
    	/*Reference*/
    	break;
    }
    default:
    {
    	/*Others*/
    	break;
    }
    }

	return NULL;
}

DwfPara *DwfCu::parseFunPara(DwfFun *fun, Dwarf_Die para_die)
{
	UTER_ASSERT(fun != NULL);
	UTER_ASSERT(para_die != NULL);
	int result;
	Dwarf_Error error;
	Dwarf_Attribute attr;

	Dwarf_Off offset = getDieOff(para_die);
	if (offset == DWF_INVALID_OFF) {
		return NULL;
	}
	DwfPara *para = new DwfPara(this);

	para->m_name = getDieName(para_die);
	if (para->m_name == "") {
		UTER_LOG("Get name of function parameter die :0x%016llx failed.", offset);
		goto error_ret;
	}

	para->m_fileno = getDieFileNo(para_die);
    if (para->m_fileno == DWF_INVALID_FILELINE) {
    	UTER_LOG("Get file number of function parameter die %s:0x%016llx failed.",
    				para->m_name.toStdString().c_str(), offset);
    	goto error_ret;
    }
	para->m_lineno = getDieLineNo(para_die);
    if (para->m_lineno == DWF_INVALID_FILELINE) {
    	UTER_LOG("Get line number of function parameter die %s:0x%016llx failed.",
    				para->m_name.toStdString().c_str(), offset);
    	goto error_ret;
    }

    /*Return type*/
	Dwarf_Off type_off;
	type_off = getDieTypeOn(para_die);
	if (type_off == DWF_INVALID_OFF) {
    	UTER_LOG("Get type offset of function parameter die %s:0x%016llx failed.",
    				para->m_name.toStdString().c_str(), offset);
		goto error_ret;
	}
	para->m_type = getTypeOnOff(type_off);
	if (para->m_type == NULL) {
    	UTER_LOG("Get type by offset 0x%016llx of function parameter die %s:0x%016llx failed.",
    				type_off, para->m_name.toStdString().c_str(), offset);
		goto error_ret;
	}

	if (fun->m_inline != 0) {
		/*No location information in inline function*/
		return para;
	}

    /*Function location:DW_AT_location     DW_FORM_exprloc*/
    result = dwarf_attr(para_die, DW_AT_location, &attr, &error);
    if(result == DW_DLV_NO_ENTRY) {
    	UTER_LOG("No location of function parameter die %s:0x%016llx.",
    				para->m_name.toStdString().c_str(), offset);
        goto error_ret;
    } else if(result == DW_DLV_ERROR) {
    	UTER_LOG("Get location of function parameter die %s:0x%016llx failed, description %s.",
    				para->m_name.toStdString().c_str(), offset, dwarf_errmsg(error));
    	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
    	goto error_ret;
    }
    result = para->m_loc.loadFromAttr(m_dbg, attr);
    dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
    if (result < 0) {
    	UTER_LOG("Load location of function parameter die %s:0x%016llx failed.",
    				para->m_name.toStdString().c_str(), offset);
    	goto error_ret;
    }
	return para;
error_ret:
	delete para;
	return NULL;
}

/*Block in function, parse data type only*/
int DwfCu::parseFunBlock(Dwarf_Die block_die)
{
	int result;
	Dwarf_Error error;

	Dwarf_Off offset = getDieOff(block_die);
	if (offset == DWF_INVALID_OFF) {
		return -1;
	}

	Dwarf_Die child_die;
	result = dwarf_child(block_die, &child_die, &error);
	while (1) {
		if (result == DW_DLV_NO_ENTRY) {
			/*End of child foreach*/
			break;
		} else if (result == DW_DLV_ERROR) {
	    	UTER_LOG("Get first or next child in function block die :0x%016llx failed, description %s.",
	    				offset, dwarf_errmsg(error));
	    	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
			break;
		}

    	Dwarf_Half tag;
    	result = dwarf_tag(child_die, &tag, &error);
        if (result == DW_DLV_ERROR) {
	    	UTER_LOG("Get tag of child in function block die :0x%016llx failed, description %s.",
	    				offset, dwarf_errmsg(error));
        	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
        	goto break_loop;
        }

        if (tag == DW_TAG_lexical_block) {
        	/*Recursion here*/
        	parseFunBlock(child_die);
        } else if (tag == DW_TAG_enumeration_type || tag == DW_TAG_structure_type
				   || tag == DW_TAG_union_type || tag == DW_TAG_typedef) {
        	Dwarf_Off type_off = getDieOff(child_die);
        	if (type_off == DWF_INVALID_OFF) {
            	UTER_LOG("Get type offset of child in function block die :0x%016llx failed.", offset);
        		goto break_loop;
        	}
        	DwfType *type = getTypeOnOff(type_off);
        	if (type == NULL) {
            	UTER_LOG("Get type by offset 0x%016llx of child in function block die :0x%016llx failed.", type_off, offset);
        		goto break_loop;
        	}
        	/*Do not save data type here*/
        } else {
        	/*Do nothing, goto next*/
        	;
        }

		Dwarf_Die next_child_die;
		result = dwarf_siblingof(m_dbg, child_die, &next_child_die, &error);
		dwarf_dealloc(m_dbg, child_die, DW_DLA_DIE);
		child_die = next_child_die;
		continue;
break_loop:
		dwarf_dealloc(m_dbg, child_die, DW_DLA_DIE);
		break;		/*no goto here*/
	}

	return 0;
}

DwfFun *DwfCu::parseFun(Dwarf_Die fun_die)
{
	UTER_ASSERT(fun_die != NULL);
	int result;
	Dwarf_Error error;

	Dwarf_Off offset = getDieOff(fun_die);
	if (offset == DWF_INVALID_OFF) {
		return NULL;
	}
	DwfFun *fun = new DwfFun(this);

	/*Offset of function in module, locate function when process local variable*/
	fun->m_offset = offset;

    fun->m_name = getDieName(fun_die);
    if (fun->m_name == "") {
	    UTER_LOG("Get name of function die :0x%016llx failed.", offset);
    	goto error_ret;
    }

	fun->m_extern = getDieIsExtern(fun_die);

    fun->m_fileno = getDieFileNo(fun_die);
    if (fun->m_fileno == DWF_INVALID_FILELINE) {
    	UTER_LOG("Get file number of function die %s:0x%016llx failed.",
    				fun->m_name.toStdString().c_str(), offset);
    	goto error_ret;
    }
    fun->m_lineno = getDieLineNo(fun_die);
    if (fun->m_lineno == DWF_INVALID_FILELINE) {
    	UTER_LOG("Get line number of function die %s:0x%016llx failed.",
    				fun->m_name.toStdString().c_str(), offset);
    	goto error_ret;
    }

    /*Return type*/
    fun->m_type = getTypeForDie(fun_die);
    if (fun->m_type == NULL) {
    	UTER_LOG("Get type of function die %s:0x%016llx failed.",
    				fun->m_name.toStdString().c_str(), offset);
    	goto error_ret;
    }

    if (getDieHasAttr(fun_die, DW_AT_inline)) {
    	unsigned long long out;
    	result = getDieUdataAttr(fun_die, DW_AT_inline, out);
        if (result < 0) {
        	UTER_LOG("Get inline of function die %s:0x%016llx failed.",
        				fun->m_name.toStdString().c_str(), offset);
        	goto error_ret;
        }
        fun->m_inline = out;
    } else {
        /*Address range*/
        fun->m_add_range.m_begin = getDieLowPc(fun_die);
        if (fun->m_add_range.m_begin == DWF_INVALID_ADDR) {
        	UTER_LOG("Get lowpc of function die %s:0x%016llx failed.",
        				fun->m_name.toStdString().c_str(), offset);
        	goto error_ret;
        }
        fun->m_add_range.m_offset = getDieHighPc(fun_die);
        if (fun->m_add_range.m_offset == DWF_INVALID_ADDR) {
        	UTER_LOG("Get highpc of function die %s:0x%016llx failed.",
        				fun->m_name.toStdString().c_str(), offset);
        	goto error_ret;
        }
    }

	Dwarf_Die child_die;
	result = dwarf_child(fun_die, &child_die, &error);
	while (1) {
		if (result == DW_DLV_NO_ENTRY) {
			/*End of child foreach*/
			break;
		} else if (result == DW_DLV_ERROR) {
	    	UTER_LOG("Get first or next child in function die %s:0x%016llx failed, description %s.",
	    				fun->m_name.toStdString().c_str(), offset, dwarf_errmsg(error));
	    	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
			break;
		}

		/*Swith type*/
    	Dwarf_Half tag;
    	result = dwarf_tag(child_die, &tag, &error);
        if (result == DW_DLV_ERROR) {
	    	UTER_LOG("Get tag of child in function die %s:0x%016llx failed, description %s.",
	    				fun->m_name.toStdString().c_str(), offset, dwarf_errmsg(error));
        	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
        	goto break_loop;
        }

        if (tag == DW_TAG_formal_parameter) {
        	/*For parameter*/
            DwfPara *para = parseFunPara(fun, child_die);
    		if (para == NULL) {
    	    	UTER_LOG("Parse parameter in function die %s:0x%016llx failed.",
    	    				fun->m_name.toStdString().c_str(), offset);
    			goto break_loop;
    		}
    		fun->m_para_list.push_back(para);
        } else {
        	/*Ignore others on top level*/
        	;
        }

		Dwarf_Die next_child_die;
		result = dwarf_siblingof(m_dbg, child_die, &next_child_die, &error);
		dwarf_dealloc(m_dbg, child_die, DW_DLA_DIE);
		child_die = next_child_die;
		continue;
break_loop:
		dwarf_dealloc(m_dbg, child_die, DW_DLA_DIE);
		goto error_ret;
	}

	/*Function is a block too, parse data type only*/
	parseFunBlock(fun_die);

	return fun;
error_ret:
	delete fun;
	return NULL;
}

DwfVar *DwfCu::parseVar(Dwarf_Die var_die)
{
	UTER_ASSERT(var_die != NULL);
	int result;
	Dwarf_Error error;
	Dwarf_Attribute attr;

	Dwarf_Off offset = getDieOff(var_die);
	if (offset == DWF_INVALID_OFF) {
		return NULL;
	}
	DwfVar *var = new DwfVar(this);

	var->m_name = getDieName(var_die);
	if (var->m_name == NULL) {
		UTER_LOG("Get name of variable die :0x%016llx failed.", offset);
		goto error_ret;
	}

	var->m_fileno = getDieFileNo(var_die);
    if (var->m_fileno == DWF_INVALID_FILELINE) {
    	UTER_LOG("Get file number of variable die %s:0x%016llx failed.",
    				var->m_name.toStdString().c_str(), offset);
    	goto error_ret;
    }
	var->m_lineno = getDieLineNo(var_die);
    if (var->m_lineno == DWF_INVALID_FILELINE) {
    	UTER_LOG("Get line number of variable die %s:0x%016llx failed.",
    				var->m_name.toStdString().c_str(), offset);
    	goto error_ret;
    }

	Dwarf_Off type_off;
	type_off = getDieTypeOn(var_die);
	if (type_off == DWF_INVALID_OFF) {
    	UTER_LOG("Get type offset of variable die %s:0x%016llx failed.",
    				var->m_name.toStdString().c_str(), offset);
		goto error_ret;
	}
	var->m_type = getTypeOnOff(type_off);
	if (var->m_type == NULL) {
    	UTER_LOG("Get type by offset 0x%016llx of variable die %s:0x%016llx failed.",
    				type_off, var->m_name.toStdString().c_str(), offset);
		goto error_ret;
	}

    var->m_extern = getDieIsExtern(var_die);

   	/*DW_AT_location     DW_FORM_exprloc*/
	result = dwarf_attr(var_die, DW_AT_location, &attr, &error);
	if(result == DW_DLV_NO_ENTRY) {
		UTER_LOG("No location of variable die %s:0x%016llx failed.",
					var->m_name.toStdString().c_str(), offset);
		goto error_ret;
	} else if(result == DW_DLV_ERROR) {
		UTER_LOG("Get location of variable die %s:0x%016llx failed, description %s.",
					var->m_name.toStdString().c_str(), offset, dwarf_errmsg(error));
		dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
		goto error_ret;
	}
	result = var->m_loc.loadFromAttr(m_dbg, attr);
	dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
	if (result < 0) {
		UTER_LOG("Load location of variable die %s:0x%016llx failed.",
					var->m_name.toStdString().c_str(), offset);
		goto error_ret;
	}

	return var;
error_ret:
	delete var;
	return NULL;
}
