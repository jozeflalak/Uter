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

#include <Python.h>

#include <uter_public.h>
#include <target.hxx>
#include <tester.hxx>

#include <tester_python.h>

static int uter_libpython_flag __attribute__((unused)) = UTER_LIBPYTHON_FLAG;

typedef QMap<DwfType*,PyObject*> RecursionBreak;

PyObject *UterPythonDataType(RecursionBreak &recurbrk, DwfType *type);

/*description of base type with python object*/
PyObject *UterPythonDataTypeBase(RecursionBreak &recurbrk, DwfTypeBase *type)
{
	UTER_ASSERT(type != NULL);
	int result;

    PyObject *return_dict = PyDict_New();
    if (return_dict == NULL) {
    	return NULL;
    }
    recurbrk.insert((DwfType*)type, return_dict);

    PyObject *head_str = PyString_FromString("basetype");
    if (head_str == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(return_dict, "head", head_str);
    Py_DECREF(head_str);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    UTER_ASSERT(type->m_base_type >= 0 && type->m_base_type < DwfTypeBase::BASE_MAX);
    PyObject *body_str = PyString_FromString(DwfTypeBase::base_desc[type->m_base_type].m_short);
    if (body_str == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(return_dict, "body", body_str);
    Py_DECREF(body_str);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

	return return_dict;
}

PyObject *UterPythonDataTypeStMem(RecursionBreak &recurbrk, DwfTypeStMem *type)
{
	UTER_ASSERT(type != NULL);
	int result;

    PyObject *return_dict = PyDict_New();
    if (return_dict == NULL) {
    	return NULL;
    }
    recurbrk.insert((DwfType*)type, return_dict);

    PyObject *head_str = PyString_FromString("struct_mem");
    if (head_str == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(return_dict, "head", head_str);
    Py_DECREF(head_str);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *body_dict = PyDict_New();
    if (body_dict == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(return_dict, "body", body_dict);
    Py_DECREF(body_dict);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *mem_name_str = PyString_FromString(type->m_name.toStdString().c_str());
    if (mem_name_str == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(body_dict, "name", mem_name_str);
    Py_DECREF(mem_name_str);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *mem_off_int = PyInt_FromSsize_t(type->m_member_location);
    if (mem_off_int == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(body_dict, "offset", mem_off_int);
    Py_DECREF(mem_off_int);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *mem_type_obj = UterPythonDataType(recurbrk, type->m_type);
    if (mem_type_obj == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(body_dict, "type", mem_type_obj);
    Py_DECREF(mem_type_obj);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

	return return_dict;
}

PyObject *UterPythonDataTypeSt(RecursionBreak &recurbrk, DwfTypeSt *type)
{
	UTER_ASSERT(type != NULL);
	int result;

    PyObject *return_dict = PyDict_New();
    if (return_dict == NULL) {
    	return NULL;
    }
    recurbrk.insert((DwfType*)type, return_dict);

    PyObject *head_string = PyString_FromString("struct");
    if (head_string == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(return_dict, "head", head_string);
    Py_DECREF(head_string);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *body_dict = PyDict_New();
    if (body_dict == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(return_dict, "body", body_dict);
    Py_DECREF(body_dict);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *name_str = PyString_FromString(type->m_name.toStdString().c_str());
    if (name_str == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(body_dict, "name", name_str);
    Py_DECREF(name_str);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *size_int = PyInt_FromSsize_t(type->dataSize());
    if (size_int == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(body_dict, "size", size_int);
    Py_DECREF(size_int);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *mem_list = PyList_New(0);
    if (mem_list == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(body_dict, "member", mem_list);
    Py_DECREF(mem_list);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    QList<DwfTypeStMem*>::iterator iter;
    for (iter = type->m_member_list.begin(); iter != type->m_member_list.end(); iter++) {
    	DwfTypeStMem *mem = *iter;

    	PyObject *mem_obj = UterPythonDataTypeStMem(recurbrk, mem);
    	if (mem_obj == NULL) {
        	Py_DECREF(return_dict);
        	return NULL;
    	}
    	result = PyList_Append(mem_list, mem_obj);
    	Py_DECREF(mem_obj);
    	if (result < 0) {
        	Py_DECREF(return_dict);
        	return NULL;
    	}
    }

	return return_dict;
}

PyObject *UterPythonDataTypeUnMem(RecursionBreak &recurbrk, DwfTypeUnMem *type)
{
	UTER_ASSERT(type != NULL);
	int result;

    PyObject *return_dict = PyDict_New();
    if (return_dict == NULL) {
    	return NULL;
    }
    recurbrk.insert((DwfType*)type, return_dict);

    PyObject *head_str = PyString_FromString("union_mem");
	if (head_str == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
	}
	result = PyDict_SetItemString(return_dict, "head", head_str);
    Py_DECREF(head_str);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *body_dict = PyDict_New();
	if (body_dict == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
	}
	result = PyDict_SetItemString(return_dict, "body", body_dict);
    Py_DECREF(body_dict);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *mem_name_str = PyString_FromString(type->m_name.toStdString().c_str());
	if (mem_name_str == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
	}
	result = PyDict_SetItemString(body_dict, "name", mem_name_str);
    Py_DECREF(mem_name_str);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *mem_type_obj = UterPythonDataType(recurbrk, type->m_type);
	if (mem_type_obj == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
	}
	result = PyDict_SetItemString(body_dict, "type", mem_type_obj);
    Py_DECREF(mem_type_obj);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

	return return_dict;
}

PyObject *UterPythonDataTypeUn(RecursionBreak &recurbrk, DwfTypeUn *type)
{
	UTER_ASSERT(type != NULL);
	int result;

    PyObject *return_dict = PyDict_New();
    if (return_dict == NULL) {
    	return NULL;
    }
    recurbrk.insert((DwfType*)type, return_dict);

    PyObject *head_str = PyString_FromString("union");
    if (head_str == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(return_dict, "head", head_str);
    Py_DECREF(head_str);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *body_dict = PyDict_New();
    if (body_dict == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(return_dict, "body", body_dict);
    Py_DECREF(body_dict);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *name_str = PyString_FromString(type->m_name.toStdString().c_str());
    if (name_str == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(body_dict, "name", name_str);
    Py_DECREF(name_str);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *size_int = PyInt_FromSsize_t(type->dataSize());
    if (size_int == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(body_dict, "size", size_int);
    Py_DECREF(size_int);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *mem_list = PyList_New(0);
    if (mem_list == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(body_dict, "member", mem_list);
    Py_DECREF(mem_list);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    QList<DwfTypeUnMem*>::iterator iter;
    for (iter = type->m_member_list.begin(); iter != type->m_member_list.end(); iter++) {
    	DwfTypeUnMem *mem = *iter;

    	PyObject *mem_obj = UterPythonDataTypeUnMem(recurbrk, mem);
    	if (mem_obj == NULL) {
        	Py_DECREF(return_dict);
        	return NULL;
    	}
    	result = PyList_Append(mem_list, mem_obj);
    	Py_DECREF(mem_obj);
    	if (result < 0) {
        	Py_DECREF(return_dict);
        	return NULL;
    	}
    }

	return return_dict;
}

PyObject *UterPythonDataTypeEnMem(RecursionBreak &recurbrk, DwfTypeEnMem *type)
{
	UTER_ASSERT(type != NULL);
	int result;

    PyObject *return_dict = PyDict_New();
    if (return_dict == NULL) {
    	return NULL;
    }
    recurbrk.insert((DwfType*)type, return_dict);

    PyObject *head_str = PyString_FromString("enum_mem");
    if (head_str == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(return_dict, "head", head_str);
    Py_DECREF(head_str);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *body_dict = PyDict_New();
    if (body_dict == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(return_dict, "body", body_dict);
    Py_DECREF(body_dict);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *mem_name_str = PyString_FromString(type->m_name.toStdString().c_str());
    if (mem_name_str == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(body_dict, "name", mem_name_str);
    Py_DECREF(mem_name_str);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *mem_value_long = PyLong_FromLongLong(type->m_value);
    if (mem_value_long == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(body_dict, "value", mem_value_long);
    Py_DECREF(mem_value_long);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

	return return_dict;
}

PyObject *UterPythonDataTypeEn(RecursionBreak &recurbrk, DwfTypeEn *type)
{
	UTER_ASSERT(type != NULL);
	int result;

    PyObject *return_dict = PyDict_New();
    if (return_dict == NULL) {
    	return NULL;
    }
    recurbrk.insert((DwfType*)type, return_dict);

    PyObject *head_str = PyString_FromString("enum");
    if (head_str == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(return_dict, "head", head_str);
    Py_DECREF(head_str);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *body_dict = PyDict_New();
    if (body_dict == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(return_dict, "body", body_dict);
    Py_DECREF(body_dict);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *name_str = PyString_FromString(type->m_name.toStdString().c_str());
    if (name_str == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(body_dict, "name", name_str);
    Py_DECREF(name_str);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *size_int = PyInt_FromSsize_t(type->dataSize());
    if (size_int == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(body_dict, "size", size_int);
    Py_DECREF(size_int);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *mem_list = PyList_New(0);
    if (mem_list == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(body_dict, "member", mem_list);
    Py_DECREF(mem_list);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    QList<DwfTypeEnMem*>::iterator iter;
    for (iter = type->m_member_list.begin(); iter != type->m_member_list.end(); iter++) {
    	DwfTypeEnMem *mem = *iter;

    	PyObject *mem_obj = UterPythonDataTypeEnMem(recurbrk, mem);
    	if (mem_obj == NULL) {
        	Py_DECREF(return_dict);
        	return NULL;
    	}
    	result = PyList_Append(mem_list, mem_obj);
    	Py_DECREF(mem_obj);
    	if (result < 0) {
        	Py_DECREF(return_dict);
        	return NULL;
    	}
    }

	return return_dict;
}

PyObject *UterPythonDataTypeSubrt(RecursionBreak &recurbrk, DwfTypeSubrt *type)
{
	UTER_ASSERT(type != NULL);
	int result;

    PyObject *return_dict = PyDict_New();
    if (return_dict == NULL) {
    	return NULL;
    }
    recurbrk.insert((DwfType*)type, return_dict);

    PyObject *head_str = PyString_FromString("subroutine");
    if (head_str == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(return_dict, "head", head_str);
    Py_DECREF(head_str);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *body_dict = PyDict_New();
    if (body_dict == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(return_dict, "body", body_dict);
    Py_DECREF(body_dict);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *rettype_obj = UterPythonDataType(recurbrk, type->m_type);
    if (rettype_obj == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(body_dict, "ret", rettype_obj);
    Py_DECREF(rettype_obj);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *args_list = PyList_New(0);
    if (args_list == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(body_dict, "args", args_list);
    Py_DECREF(args_list);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    QList<DwfTypeSubrtPara*>::iterator iter;
    for (iter = type->m_para_list.begin(); iter != type->m_para_list.end(); iter++) {
    	DwfTypeSubrtPara *para = *iter;

    	PyObject *arg_obj = UterPythonDataType(recurbrk, para->m_type);
        if (arg_obj == NULL) {
        	Py_DECREF(return_dict);
        	return NULL;
        }
        result = PyList_Append(args_list, arg_obj);
    	Py_DECREF(arg_obj);
    	if (result < 0) {
        	Py_DECREF(return_dict);
        	return NULL;
    	}
    }

	return return_dict;
}

PyObject *UterPythonDataTypePoint(RecursionBreak &recurbrk, DwfTypePoint *type)
{
	UTER_ASSERT(type != NULL);
	int result;

    PyObject *return_dict = PyDict_New();
    if (return_dict == NULL) {
    	return NULL;
    }
    recurbrk.insert((DwfType*)type, return_dict);

    PyObject *head_str = PyString_FromString("point");
    if (head_str == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(return_dict, "head", head_str);
    Py_DECREF(head_str);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *body_dict = PyDict_New();
    if (body_dict == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(return_dict, "body", body_dict);
    Py_DECREF(body_dict);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *totype_obj = UterPythonDataType(recurbrk, type->m_type);
    if (totype_obj == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(body_dict, "pointto", totype_obj);
    Py_DECREF(totype_obj);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

	return return_dict;
}

PyObject *UterPythonDataTypeArray(RecursionBreak &recurbrk, DwfTypeArray *type)
{
	UTER_ASSERT(type != NULL);
	int result;

    PyObject *return_dict = PyDict_New();
    if (return_dict == NULL) {
    	return NULL;
    }
    recurbrk.insert((DwfType*)type, return_dict);

    PyObject *head_str = PyString_FromString("array");
    if (head_str == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(return_dict, "head", head_str);
    Py_DECREF(head_str);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *body_dict = PyDict_New();
    if (body_dict == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(return_dict, "body", body_dict);
    Py_DECREF(body_dict);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *totype_obj = UterPythonDataType(recurbrk, type->m_type);
    if (totype_obj == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(body_dict, "arrayto", totype_obj);
    Py_DECREF(totype_obj);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *index_list = PyList_New(0);
    if (index_list == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(body_dict, "index", index_list);
    Py_DECREF(index_list);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    QList<unsigned long long>::iterator iter;
    for (iter = type->m_range_list.begin(); iter != type->m_range_list.end(); iter++) {
    	unsigned long long range = *iter;

    	PyObject *index_int = PyInt_FromSsize_t(range);
        if (index_int == NULL) {
        	Py_DECREF(return_dict);
        	return NULL;
        }
        result = PyList_Append(index_list, index_int);
    	Py_DECREF(index_int);
    	if (result < 0) {
        	Py_DECREF(return_dict);
        	return NULL;
    	}
    }

	return return_dict;
}

PyObject *UterPythonDataTypeDef(RecursionBreak &recurbrk, DwfTypeTypedef *type)
{
	UTER_ASSERT(type != NULL);
	int result;

    PyObject *return_dict = PyDict_New();
    if (return_dict == NULL) {
    	return NULL;
    }
    recurbrk.insert((DwfType*)type, return_dict);

    PyObject *head_str = PyString_FromString("typedef");
    if (head_str == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(return_dict, "head", head_str);
    Py_DECREF(head_str);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *body_dict = PyDict_New();
    if (body_dict == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(return_dict, "body", body_dict);
    Py_DECREF(body_dict);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *totype_obj = UterPythonDataType(recurbrk, type->m_type);
    if (totype_obj == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(body_dict, "typeto", totype_obj);
    Py_DECREF(totype_obj);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *name_str = PyString_FromString(type->m_name.toStdString().c_str());
    if (name_str == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(body_dict, "name", name_str);
    Py_DECREF(name_str);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

	return return_dict;
}

PyObject *UterPythonDataTypeVoid(RecursionBreak &recurbrk, DwfTypeVoid *type)
{
	UTER_ASSERT(type != NULL);
	int result;

    PyObject *return_dict = PyDict_New();
    if (return_dict == NULL) {
    	return NULL;
    }
    recurbrk.insert((DwfType*)type, return_dict);

    PyObject *head_str = PyString_FromString("void");
    if (head_str == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(return_dict, "head", head_str);
    Py_DECREF(head_str);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    Py_INCREF(Py_None);
    result = PyDict_SetItemString(return_dict, "body", Py_None);
    Py_DECREF(Py_None);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

	return return_dict;
}

PyObject *UterPythonDataType(RecursionBreak &recurbrk, DwfType *type)
{
	UTER_ASSERT(type != NULL);

	/*Check object exist*/
	RecursionBreak::iterator recurbrk_iter = recurbrk.find(type);
	if (recurbrk_iter != recurbrk.end()) {
		PyObject *obj_exist = recurbrk_iter.value();
		Py_INCREF(obj_exist);
		return obj_exist;
	}

	switch(type->m_data_type)
	{
	case DwfType::TYPE_BASE:
	{
		return UterPythonDataTypeBase(recurbrk, (DwfTypeBase *)type);
	}
	case DwfType::TYPE_ST_MEM:
	{
		return UterPythonDataTypeStMem(recurbrk, (DwfTypeStMem *)type);
	}
	case DwfType::TYPE_ST:
	{
		return UterPythonDataTypeSt(recurbrk, (DwfTypeSt *)type);
	}
	case DwfType::TYPE_UN_MEM:
	{
		return UterPythonDataTypeUnMem(recurbrk, (DwfTypeUnMem *)type);
	}
	case DwfType::TYPE_UN:
	{
		return UterPythonDataTypeUn(recurbrk, (DwfTypeUn *)type);
	}
	case DwfType::TYPE_EN_MEM:
	{
		return UterPythonDataTypeEnMem(recurbrk, (DwfTypeEnMem *)type);
	}
	case DwfType::TYPE_EN:
	{
		return UterPythonDataTypeEn(recurbrk, (DwfTypeEn *)type);
	}
	case DwfType::TYPE_SUBRT:
	{
		return UterPythonDataTypeSubrt(recurbrk, (DwfTypeSubrt *)type);
	}
	case DwfType::TYPE_VOID:
	{
		return UterPythonDataTypeVoid(recurbrk, (DwfTypeVoid *)type);
	}
	case DwfType::TYPE_POINT:
	{
		return UterPythonDataTypePoint(recurbrk, (DwfTypePoint *)type);
	}
	case DwfType::TYPE_ARRAY:
	{
		return UterPythonDataTypeArray(recurbrk, (DwfTypeArray *)type);
	}
	case DwfType::TYPE_TYPEDEF:
	{
		return UterPythonDataTypeDef(recurbrk, (DwfTypeTypedef *)type);
	}
	default:
	{
		UTER_ASSERT(0);
		break;
	}
	}

	return NULL;
}

PyObject *UterPythonFuncType(DwfFun *fun)
{
	UTER_ASSERT(fun != NULL);
	int result;

    PyObject *return_dict = PyDict_New();
    if (return_dict == NULL) {
    	return NULL;
    }

    PyObject *head_str = PyString_FromString("function");
    if (head_str == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(return_dict, "head", head_str);
    Py_DECREF(head_str);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *body_dict = PyDict_New();
    if (body_dict == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(return_dict, "body", body_dict);
    Py_DECREF(body_dict);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    RecursionBreak rettype_recurbrk;
    PyObject *rettype_obj = UterPythonDataType(rettype_recurbrk, fun->m_type);
    if (rettype_obj == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(body_dict, "ret", rettype_obj);
    Py_DECREF(rettype_obj);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    PyObject *args_list = PyList_New(0);
    if (args_list == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(body_dict, "args", args_list);
    Py_DECREF(args_list);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    QList<DwfPara*>::iterator iter;
    for (iter = fun->m_para_list.begin(); iter != fun->m_para_list.end(); iter++) {
    	DwfPara *para = *iter;

    	RecursionBreak argtype_recurbrk;
    	PyObject *argtype_obj = UterPythonDataType(argtype_recurbrk, para->m_type);
        if (argtype_obj == NULL) {
        	Py_DECREF(return_dict);
        	return NULL;
        }
        result = PyList_Append(args_list, argtype_obj);
    	Py_DECREF(argtype_obj);
    	if (result < 0) {
        	Py_DECREF(return_dict);
        	return NULL;
    	}
    }

	return return_dict;
}

PyObject *UterPythonExtern_Hello(PyObject *self, PyObject *args)
{
	UTER_ASSERT(self == NULL);
	UTER_ASSERT(args != NULL);
	UTER_ASSERT(PyTuple_Check(args));

	PyObject *return_string = PyString_FromString("Hello World");
    return return_string;
}

/*TODO:Macro*/
PyObject *UterPythonExtern_Macro(PyObject *self, PyObject *args, PyObject *kw)
{
	if (self != NULL || args == NULL || !PyTuple_Check(args) || kw == NULL || !PyDict_Check(kw)) {
		PyErr_Format(PyExc_SyntaxError, "Python extern method dict base argument error.");
		return NULL;
	}

	/*mod*/
	PyObject *mod_obj = PyDict_GetItemString(kw, "mod");
	if (mod_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "mod");
		return NULL;
	}
	if (! PyString_Check(mod_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "mod", "string");
		return NULL;
	}
	char *module_path = PyString_AsString(mod_obj);

	/*cu*/
	PyObject *cu_obj = PyDict_GetItemString(kw, "cu");
	if (cu_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "cu");
		return NULL;
	}
	if (! PyString_Check(cu_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "cu", "string");
		return NULL;
	}
	char *cu_path = PyString_AsString(cu_obj);

	/*变量名字*/
	PyObject *name_obj = PyDict_GetItemString(kw, "name");
	if (name_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "name");
		return NULL;
	}
	if (! PyString_Check(name_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "name", "string");
		return NULL;
	}
	char *macro_name = PyString_AsString(name_obj);

	/*数据上锁*/
	QMutexLocker locker(&(g_target->m_data_lock));

	/*定位到模块*/
	TargetMod *mod = g_target->modGetByPath(module_path);
	if (mod == NULL) {
		PyErr_Format(PyExc_ValueError, "No such module with path %s.", module_path);
		return NULL;
	}

	/*定位到Cu*/
	DwfCu *cu = mod->dwarfGetCuByPath(cu_path);
	if (cu == NULL) {
		PyErr_Format(PyExc_ValueError, "No such compile unit with path %s.", cu_path);
		return NULL;
	}

	/*宏信息经常为空*/
	dwf_macro *macro = cu->m_macro;
	if (macro == NULL) {
	    PyObject *return_dict = PyDict_New();
	    if (return_dict == NULL) {
	    	return NULL;
	    }

	    PyObject *head_str = PyString_FromString("no_macro_info");
	    if (head_str == NULL) {
	    	Py_DECREF(return_dict);
	    	return NULL;
	    }
	    int result = PyDict_SetItemString(return_dict, "head", head_str);
	    Py_DECREF(head_str);
	    if (result < 0) {
	    	Py_DECREF(return_dict);
	    	return NULL;
	    }

	    Py_INCREF(Py_None);
	    result = PyDict_SetItemString(return_dict, "body", Py_None);
	    Py_DECREF(Py_None);
	    if (result < 0) {
	    	Py_DECREF(return_dict);
	    	return NULL;
	    }

	    return return_dict;
	}

	/*提取宏值*/
	const char *macro_value = NULL;
	int result = get_macro_value(&(macro->m_root_file), macro_name, &macro_value);
	if (result == 0) {
	    PyObject *return_dict = PyDict_New();
	    if (return_dict == NULL) {
	    	return NULL;
	    }

	    PyObject *head_str = PyString_FromString("no_such_macro");
	    if (head_str == NULL) {
	    	Py_DECREF(return_dict);
	    	return NULL;
	    }
	    result = PyDict_SetItemString(return_dict, "head", head_str);
	    Py_DECREF(head_str);
	    if (result < 0) {
	    	Py_DECREF(return_dict);
	    	return NULL;
	    }

	    Py_INCREF(Py_None);
	    result = PyDict_SetItemString(return_dict, "body", Py_None);
	    Py_DECREF(Py_None);
	    if (result < 0) {
	    	Py_DECREF(return_dict);
	    	return NULL;
	    }

	    return return_dict;
	} else if (result == 1) {
	    PyObject *return_dict = PyDict_New();
	    if (return_dict == NULL) {
	    	return NULL;
	    }

	    PyObject *head_str = PyString_FromString("macro_undef");
	    if (head_str == NULL) {
	    	Py_DECREF(return_dict);
	    	return NULL;
	    }
	    result = PyDict_SetItemString(return_dict, "head", head_str);
	    Py_DECREF(head_str);
	    if (result < 0) {
	    	Py_DECREF(return_dict);
	    	return NULL;
	    }

	    PyObject *body_str = PyString_FromString(macro_value);
	    if (body_str == NULL) {
	    	Py_DECREF(return_dict);
	    	return NULL;
	    }
	    result = PyDict_SetItemString(return_dict, "body", body_str);
	    Py_DECREF(body_str);
	    if (result < 0) {
	    	Py_DECREF(return_dict);
	    	return NULL;
	    }

	    return return_dict;
	} else if (result == 2) {
	    PyObject *return_dict = PyDict_New();
	    if (return_dict == NULL) {
	    	return NULL;
	    }

	    PyObject *head_str = PyString_FromString("macro_define");
	    if (head_str == NULL) {
	    	Py_DECREF(return_dict);
	    	return NULL;
	    }
	    result = PyDict_SetItemString(return_dict, "head", head_str);
	    Py_DECREF(head_str);
	    if (result < 0) {
	    	Py_DECREF(return_dict);
	    	return NULL;
	    }

	    PyObject *body_str = PyString_FromString(macro_value);
	    if (body_str == NULL) {
	    	Py_DECREF(return_dict);
	    	return NULL;
	    }
	    result = PyDict_SetItemString(return_dict, "body", body_str);
	    Py_DECREF(body_str);
	    if (result < 0) {
	    	Py_DECREF(return_dict);
	    	return NULL;
	    }

	    return return_dict;
	} else {
	    PyObject *return_dict = PyDict_New();
	    if (return_dict == NULL) {
	    	return NULL;
	    }

	    PyObject *head_str = PyString_FromString("no_macro_info");
	    if (head_str == NULL) {
	    	Py_DECREF(return_dict);
	    	return NULL;
	    }
	    result = PyDict_SetItemString(return_dict, "head", head_str);
	    Py_DECREF(head_str);
	    if (result < 0) {
	    	Py_DECREF(return_dict);
	    	return NULL;
	    }

	    Py_INCREF(Py_None);
	    result = PyDict_SetItemString(return_dict, "body", Py_None);
	    Py_DECREF(Py_None);
	    if (result < 0) {
	    	Py_DECREF(return_dict);
	    	return NULL;
	    }

	    return return_dict;
	}
}

PyObject *UterPythonExtern_Type(PyObject *self, PyObject *args, PyObject *kw)
{
	if (self != NULL || args == NULL || !PyTuple_Check(args) || kw == NULL || !PyDict_Check(kw)) {
		PyErr_Format(PyExc_SyntaxError, "Python extern method dict base argument error.");
		return NULL;
	}

	/*mod*/
	PyObject *mod_obj = PyDict_GetItemString(kw, "mod");
	if (mod_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "mod");
		return NULL;
	}
	if (! PyString_Check(mod_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "mod", "string");
		return NULL;
	}
	char *module_path = PyString_AsString(mod_obj);

	/*cu*/
	PyObject *cu_obj = PyDict_GetItemString(kw, "cu");
	if (cu_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "cu");
		return NULL;
	}
	if (! PyString_Check(cu_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "cu", "string");
		return NULL;
	}
	char *cu_path = PyString_AsString(cu_obj);

	/*type*/
	PyObject *class_obj = PyDict_GetItemString(kw, "type");
	if (class_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "type");
		return NULL;
	}
	if (! PyString_Check(class_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "type", "string");
		return NULL;
	}
	char *type_id = PyString_AsString(class_obj);

	/*name*/
	PyObject *name_obj = PyDict_GetItemString(kw, "name");
	if (name_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "name");
		return NULL;
	}
	if (! PyString_Check(name_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "name", "string");
		return NULL;
	}
	char *type_name = PyString_AsString(name_obj);

	/**/
	QMutexLocker locker(&(g_target->m_data_lock));

	/*mod*/
	TargetMod *mod = g_target->modGetByPath(module_path);
	if (mod == NULL) {
		PyErr_Format(PyExc_ValueError, "No such module with path %s.", module_path);
		return NULL;
	}

	/*cu*/
	DwfCu *cu = mod->dwarfGetCuByPath(cu_path);
	if (cu == NULL) {
		PyErr_Format(PyExc_ValueError, "No such compile unit with path %s.", cu_path);
		return NULL;
	}

	/*which type*/
	QList<DwfType*> *find_list = NULL;
	if (strcmp(type_id, "struct") == 0) {
		find_list = &cu->m_structs;
	} else if (strcmp(type_id, "union") == 0) {
		find_list = &cu->m_unions;
	} else if (strcmp(type_id, "enum") == 0) {
		find_list = &cu->m_enums;
	} else if (strcmp(type_id, "typedef") == 0) {
		find_list = &cu->m_typedefs;
	} else {
		PyErr_Format(PyExc_ValueError, "Unknown type id %s.", type_id);
		return NULL;
	}

	/*find type*/
	QList<DwfType*>::iterator iter;
	for (iter = find_list->begin(); iter != find_list->end(); iter++) {
		DwfType *dwftype = *iter;
		if (dwftype->m_name == type_name) {
			break;
		}
	}
	if (iter == find_list->end()) {
		PyErr_Format(PyExc_ValueError, "No such %s type named %s", type_id, type_name);
		return NULL;
	}
	DwfType *dwftype = *iter;

	/*make type object*/
	RecursionBreak type_recurbrk;
	PyObject *type_obj = UterPythonDataType(type_recurbrk, dwftype);
	if (type_obj == NULL) {
		return NULL;
	}
	return type_obj;
}

PyObject *UterPythonExtern_Variable(PyObject *self, PyObject *args, PyObject *kw)
{
	if (self != NULL || args == NULL || !PyTuple_Check(args) || kw == NULL || !PyDict_Check(kw)) {
		PyErr_Format(PyExc_SyntaxError, "Python extern method dict base argument error.");
		return NULL;
	}

	/*mod*/
	PyObject *mod_obj = PyDict_GetItemString(kw, "mod");
	if (mod_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "mod");
		return NULL;
	}
	if (! PyString_Check(mod_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "mod", "string");
		return NULL;
	}
	char *module_path = PyString_AsString(mod_obj);

	/*cu*/
	PyObject *cu_obj = PyDict_GetItemString(kw, "cu");
	if (cu_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "cu");
		return NULL;
	}
	if (! PyString_Check(cu_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "cu", "string");
		return NULL;
	}
	char *cu_path = PyString_AsString(cu_obj);

	/*name*/
	PyObject *name_obj = PyDict_GetItemString(kw, "name");
	if (name_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "name");
		return NULL;
	}
	if (! PyString_Check(name_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "name", "string");
		return NULL;
	}
	char *varname = PyString_AsString(name_obj);

	/**/
	QMutexLocker locker(&(g_target->m_data_lock));

	/*mod*/
	TargetMod *mod = g_target->modGetByPath(module_path);
	if (mod == NULL) {
		PyErr_Format(PyExc_ValueError, "No such module with path %s.", module_path);
		return NULL;
	}

	/*cu*/
	DwfCu *cu = mod->dwarfGetCuByPath(cu_path);
	if (cu == NULL) {
		PyErr_Format(PyExc_ValueError, "No such compile unit with path %s.", cu_path);
		return NULL;
	}

	/*find variable*/
	QList<DwfVar*>::iterator iter;
	for (iter = cu->m_vars.begin(); iter != cu->m_vars.end(); iter++) {
		DwfVar *var = *iter;
		if (var->m_name == varname) {
			break;
		}
	}
	if (iter == cu->m_vars.end()) {
		PyErr_Format(PyExc_ValueError, "No such variable named %s.", varname);
		return NULL;
	}
	DwfVar *var = *iter;

	/*Get variable address*/
	if (var->m_loc.m_loc_type != DwfLoc::LOC_ADDR) {
		PyErr_Format(PyExc_ValueError, "Variable %s no valid address.", varname);
		return NULL;
	}
	unsigned long long address = mod->m_add_mkup + var->m_loc.m_addr;

	/*variable description for return*/
    PyObject *return_dict = PyDict_New();
    if (return_dict == NULL) {
    	return NULL;
    }

    /*extern*/
    PyObject *bool_obj;
	if (var->m_extern) {
		Py_INCREF(Py_True);
		bool_obj = Py_True;
	} else {
		Py_INCREF(Py_False);
		bool_obj = Py_False;
	}
	int result = PyDict_SetItemString(return_dict, "ext", bool_obj);
	Py_DECREF(bool_obj);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

	/*data type*/
	RecursionBreak type_recurbrk;
	PyObject *type_obj = UterPythonDataType(type_recurbrk, var->m_type);
	if (type_obj == NULL) {
		Py_DECREF(return_dict);
		return NULL;
	}
	result = PyDict_SetItemString(return_dict, "type", type_obj);
	Py_DECREF(type_obj);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

	/*varialbe entry*/
    PyObject *var_dict = PyDict_New();
    if (var_dict == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(return_dict, "var", var_dict);
    Py_DECREF(var_dict);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    /*variable name*/
    PyObject *name_str = PyString_FromString(varname);
    if (name_str == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    result = PyDict_SetItemString(var_dict, "name", name_str);
    Py_DECREF(name_str);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

	/*varialbe address*/
	PyObject *addr_long = PyLong_FromUnsignedLongLong(address);
	if (addr_long == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
	}
	result = PyDict_SetItemString(var_dict, "address", addr_long);
	Py_DECREF(addr_long);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

	return return_dict;
}

PyObject *UterPythonExtern_Function(PyObject *self, PyObject *args, PyObject *kw)
{
	if (self != NULL || args == NULL || !PyTuple_Check(args) || kw == NULL || !PyDict_Check(kw)) {
		PyErr_Format(PyExc_SyntaxError, "Python extern method dict base argument error.");
		return NULL;
	}

	/*mod*/
	PyObject *mod_obj = PyDict_GetItemString(kw, "mod");
	if (mod_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "mod");
		return NULL;
	}
	if (! PyString_Check(mod_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "mod", "string");
		return NULL;
	}
	char *module_path = PyString_AsString(mod_obj);

	/*cu*/
	PyObject *cu_obj = PyDict_GetItemString(kw, "cu");
	if (cu_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "cu");
		return NULL;
	}
	if (! PyString_Check(cu_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "cu", "string");
		return NULL;
	}
	char *cu_path = PyString_AsString(cu_obj);

	/*name*/
	PyObject *name_obj = PyDict_GetItemString(kw, "name");
	if (name_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "name");
		return NULL;
	}
	if (! PyString_Check(name_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "name", "string");
		return NULL;
	}
	char *funname = PyString_AsString(name_obj);

	/**/
	QMutexLocker locker(&(g_target->m_data_lock));

	/*mod*/
	TargetMod *mod = g_target->modGetByPath(module_path);
	if (mod == NULL) {
		PyErr_Format(PyExc_ValueError, "No such module with path %s.", module_path);
		return NULL;
	}

	/*cu*/
	DwfCu *cu = mod->dwarfGetCuByPath(cu_path);
	if (cu == NULL) {
		PyErr_Format(PyExc_ValueError, "No such compile unit with path %s.", cu_path);
		return NULL;
	}

	/*find function*/
	QList<DwfFun*>::iterator iter;
	for (iter = cu->m_funs.begin(); iter != cu->m_funs.end(); iter++) {
		DwfFun *fun = *iter;
		if (fun->m_name == funname) {
			break;
		}
	}
	if (iter == cu->m_funs.end()) {
		PyErr_Format(PyExc_ValueError, "No such function named %s.", funname);
		return NULL;
	}
	DwfFun *fun = *iter;

	/*function description for return*/
    PyObject *return_dict = PyDict_New();
    if (return_dict == NULL) {
    	return NULL;
    }

    /*extern*/
    PyObject *bool_obj;
	if (fun->m_extern) {
		Py_INCREF(Py_True);
		bool_obj = Py_True;
	} else {
		Py_INCREF(Py_False);
		bool_obj = Py_False;
	}
	int result = PyDict_SetItemString(return_dict, "ext", bool_obj);
	Py_DECREF(bool_obj);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

	/*data type*/
	PyObject *funtype_obj = UterPythonFuncType(fun);
	if (funtype_obj == NULL) {
		Py_DECREF(return_dict);
		return NULL;
	}
	result = PyDict_SetItemString(return_dict, "type", funtype_obj);
	Py_DECREF(funtype_obj);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

	/*function entry*/
    PyObject *fun_dict = PyDict_New();
    if (fun_dict == NULL) {
		Py_DECREF(return_dict);
		return NULL;
    }
    result = PyDict_SetItemString(return_dict, "fun", fun_dict);
    Py_DECREF(fun_dict);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    /*function name*/
    PyObject *name_str = PyString_FromString(funname);
    if (name_str == NULL) {
		Py_DECREF(return_dict);
		return NULL;
    }
    result = PyDict_SetItemString(fun_dict, "name", name_str);
    Py_DECREF(name_str);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    /*function address low*/
	PyObject *low_addr_long = PyLong_FromUnsignedLongLong(mod->m_add_mkup + fun->m_add_range.m_begin);
	if (low_addr_long == NULL) {
		Py_DECREF(return_dict);
		return NULL;
	}
	result = PyDict_SetItemString(fun_dict, "low", low_addr_long);
	Py_DECREF(low_addr_long);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    /*function address high*/
	PyObject *high_addr_long = PyLong_FromUnsignedLongLong(fun->m_add_range.m_offset);
	if (high_addr_long == NULL) {
		Py_DECREF(return_dict);
		return NULL;
	}
	result = PyDict_SetItemString(fun_dict, "high", high_addr_long);
	Py_DECREF(high_addr_long);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

	return return_dict;
}

PyObject *UterPythonExtern_Symbol(PyObject *self, PyObject *args, PyObject *kw)
{
	if (self != NULL || args == NULL || !PyTuple_Check(args) || kw == NULL || !PyDict_Check(kw)) {
		PyErr_Format(PyExc_SyntaxError, "Python extern method dict base argument error.");
		return NULL;
	}

	/*mod*/
	PyObject *mod_obj = PyDict_GetItemString(kw, "mod");
	if (mod_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "mod");
		return NULL;
	}
	if (! PyString_Check(mod_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "mod", "string");
		return NULL;
	}
	char *module_path = PyString_AsString(mod_obj);

	/*name*/
	PyObject *name_obj = PyDict_GetItemString(kw, "name");
	if (name_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "name");
		return NULL;
	}
	if (! PyString_Check(name_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "name", "string");
		return NULL;
	}
	char *name = PyString_AsString(name_obj);

	/*source*/
	PyObject *source_obj = PyDict_GetItemString(kw, "source");
	if (source_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "source");
		return NULL;
	}
	if (! PyString_Check(source_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "source", "string");
		return NULL;
	}
	char *source = PyString_AsString(source_obj);

	/*section*/
	PyObject *section_obj = PyDict_GetItemString(kw, "section");
	if (section_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "section");
		return NULL;
	}
	if (! PyString_Check(section_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "section", "string");
		return NULL;
	}
	char *section = PyString_AsString(section_obj);

	/*bind*/
	PyObject *bind_obj = PyDict_GetItemString(kw, "bind");
	if (bind_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "bind");
		return NULL;
	}
	if (! PyString_Check(bind_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "bind", "string");
		return NULL;
	}
	/*no use now*/
	char *bind = PyString_AsString(bind_obj);

	/**/
	QMutexLocker locker(&(g_target->m_data_lock));

	/*mod*/
	TargetMod *mod = g_target->modGetByPath(module_path);
	if (mod == NULL) {
		PyErr_Format(PyExc_ValueError, "No such module with path %s.", module_path);
		return NULL;
	}

	/*symbol at section*/
	QList<BfdSym*> *sym_list;
	if (strcmp(".dynsym", source) == 0) {
		sym_list = &(mod->m_bfd_dynsym_list);
	} else if (strcmp(".symtab", source) == 0) {
		sym_list = &(mod->m_bfd_dbgsym_list);
	} else {
		PyErr_Format(PyExc_ValueError, "Argument source should be .dynsym or .symtab.");
		return NULL;
	}

	PyObject *return_list = PyList_New(0);
	if (return_list == NULL) {
		return NULL;
	}

	QList<BfdSym*>::iterator iter;
	for (iter = sym_list->begin(); iter != sym_list->end(); iter++) {
		BfdSym *sym = *iter;
		if (strlen(section) != 0 && sym->m_section != section) {
			continue;
		}

		if (strlen(name) != 0 && sym->m_name != name) {
			continue;
		}

		PyObject *sym_dict = PyDict_New();
		if (sym_dict == NULL) {
			Py_DECREF(return_list);
			return NULL;
		}

    	int result = PyList_Append(return_list, sym_dict);
    	Py_DECREF(sym_dict);
    	if (result < 0) {
        	Py_DECREF(return_list);
        	return NULL;
    	}

        /*mod*/
        PyObject *mod_str = PyString_FromString(mod->m_remote_path.toStdString().c_str());
        if (mod_str == NULL) {
        	Py_DECREF(return_list);
        	return NULL;
        }
        result = PyDict_SetItemString(sym_dict, "mod", mod_str);
        Py_DECREF(mod_str);
        if (result < 0) {
        	Py_DECREF(return_list);
        	return NULL;
        }

    	/*name*/
        PyObject *name_str = PyString_FromString(sym->m_name.toStdString().c_str());
        if (name_str == NULL) {
        	Py_DECREF(return_list);
        	return NULL;
        }
        result = PyDict_SetItemString(sym_dict, "name", name_str);
        Py_DECREF(name_str);
        if (result < 0) {
        	Py_DECREF(return_list);
        	return NULL;
        }

        /*section*/
        PyObject *section_str = PyString_FromString(sym->m_section.toStdString().c_str());
        if (section_str == NULL) {
        	Py_DECREF(return_list);
        	return NULL;
        }
        result = PyDict_SetItemString(sym_dict, "section", section_str);
        Py_DECREF(section_str);
        if (result < 0) {
        	Py_DECREF(return_list);
        	return NULL;
        }

        /*offset*/
    	PyObject *offset_long = PyLong_FromUnsignedLongLong(sym->m_addr);
    	if (offset_long == NULL) {
    		Py_DECREF(return_list);
    		return NULL;
    	}
        result = PyDict_SetItemString(sym_dict, "offset", offset_long);
        Py_DECREF(offset_long);
        if (result < 0) {
        	Py_DECREF(return_list);
        	return NULL;
        }

        /*address*/
    	PyObject *address_long = PyLong_FromUnsignedLongLong(mod->m_add_mkup + sym->m_addr);
    	if (address_long == NULL) {
    		Py_DECREF(return_list);
    		return NULL;
    	}
        result = PyDict_SetItemString(sym_dict, "address", address_long);
        Py_DECREF(address_long);
        if (result < 0) {
        	Py_DECREF(return_list);
        	return NULL;
        }
	}

	return return_list;
}

PyObject *UterPythonExtern_Files(PyObject *self, PyObject *args, PyObject *kw)
{
	if (self != NULL || args == NULL || !PyTuple_Check(args) || kw == NULL || !PyDict_Check(kw)) {
		PyErr_Format(PyExc_SyntaxError, "Python extern method dict base argument error.");
		return NULL;
	}

	/*mod*/
	PyObject *mod_obj = PyDict_GetItemString(kw, "mod");
	if (mod_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "mod");
		return NULL;
	}
	if (! PyString_Check(mod_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "mod", "string");
		return NULL;
	}
	char *module_path = PyString_AsString(mod_obj);

	/*cu*/
	PyObject *cu_obj = PyDict_GetItemString(kw, "cu");
	if (cu_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "cu");
		return NULL;
	}
	if (! PyString_Check(cu_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "cu", "string");
		return NULL;
	}
	char *cu_path = PyString_AsString(cu_obj);

	/**/
	QMutexLocker locker(&(g_target->m_data_lock));

	/*mod*/
	TargetMod *mod = g_target->modGetByPath(module_path);
	if (mod == NULL) {
		PyErr_Format(PyExc_ValueError, "No such module with path %s.", module_path);
		return NULL;
	}

	/*cu*/
	DwfCu *cu = mod->dwarfGetCuByPath(cu_path);
	if (cu == NULL) {
		PyErr_Format(PyExc_ValueError, "No such compile unit with path %s.", cu_path);
		return NULL;
	}

	/*file list for return*/
	PyObject *return_dict = PyDict_New();
	if (return_dict == NULL) {
		return NULL;
	}

	QMap<unsigned int, QString>::iterator iter;
	for (iter = cu->m_file_no_map.begin(); iter != cu->m_file_no_map.end(); iter++) {
		unsigned int number = iter.key();
		QString path = iter.value();

		PyObject *file_int = PyInt_FromSize_t(number);
		if (file_int == NULL) {
			Py_DECREF(return_dict);
			return NULL;
		}

		PyObject *file_str = PyString_FromString(path.toStdString().c_str());
		if (file_str == NULL) {
			Py_DECREF(return_dict);

			Py_DECREF(file_int);
			return NULL;
		}

		int result = PyDict_SetItem(return_dict, file_int, file_str);
		Py_DECREF(file_int);
		Py_DECREF(file_str);
    	if (result < 0) {
        	Py_DECREF(return_dict);
        	return NULL;
    	}
	}

	return return_dict;
}

PyObject *UterPythonExtern_CompileUnits(PyObject *self, PyObject *args, PyObject *kw)
{
	if (self != NULL || args == NULL || !PyTuple_Check(args) || kw == NULL || !PyDict_Check(kw)) {
		PyErr_Format(PyExc_SyntaxError, "Python extern method dict base argument error.");
		return NULL;
	}

	/*mod*/
	PyObject *mod_obj = PyDict_GetItemString(kw, "mod");
	if (mod_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "mod");
		return NULL;
	}
	if (! PyString_Check(mod_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "mod", "string");
		return NULL;
	}
	char *module_path = PyString_AsString(mod_obj);

	/**/
	QMutexLocker locker(&(g_target->m_data_lock));

	/*mod*/
	TargetMod *mod = g_target->modGetByPath(module_path);
	if (mod == NULL) {
		PyErr_Format(PyExc_ValueError, "No such module with path %s.", module_path);
		return NULL;
	}

	/*cu list for return*/
	PyObject *return_list = PyList_New(0);
	if (return_list == NULL) {
		return NULL;
	}

	QMap<QString, DwfCu*>::iterator iter;
	for (iter = mod->m_dwarf_cu_map.begin(); iter != mod->m_dwarf_cu_map.end(); iter++) {
		DwfCu *cu = iter.value();

		QString path = cu->m_dir + QDir::separator() + cu->m_name;
		PyObject *cupath_str = PyString_FromString(path.toStdString().c_str());
		if (cupath_str == NULL) {
			Py_DECREF(return_list);
			return NULL;
		}
		int result = PyList_Append(return_list, cupath_str);
		Py_DECREF(cupath_str);
    	if (result < 0) {
        	Py_DECREF(return_list);
        	return NULL;
    	}
	}

	return return_list;
}

PyObject *UterPythonExtern_Modules(PyObject *self, PyObject *args, PyObject *kw)
{
	if (self != NULL || args == NULL || !PyTuple_Check(args)
		|| kw == NULL || !PyDict_Check(kw)) {
		PyErr_Format(PyExc_SyntaxError, "Python extern method dict base argument error.");
		return NULL;
	}

	/**/
	QMutexLocker locker(&(g_target->m_data_lock));

	/*mod list for return*/
	PyObject *return_list = PyList_New(0);
	if (return_list == NULL) {
		return NULL;
	}

	QMap<QString, TargetMod*>::iterator iter;
	for (iter = g_target->m_mod_items.begin(); iter != g_target->m_mod_items.end(); iter++) {
		TargetMod *mod = iter.value();

		PyObject *modpath_str = PyString_FromString(mod->m_remote_path.toStdString().c_str());
		if (modpath_str == NULL) {
			Py_DECREF(return_list);
			return NULL;
		}
		int result = PyList_Append(return_list, modpath_str);
		Py_DECREF(modpath_str);
    	if (result < 0) {
        	Py_DECREF(return_list);
        	return NULL;
    	}
	}

	return return_list;
}

PyObject *UterPythonExtern_ProjectInfo(PyObject *self, PyObject *args, PyObject *kw)
{
	if (self != NULL || args == NULL || !PyTuple_Check(args) || kw == NULL || !PyDict_Check(kw)) {
		PyErr_Format(PyExc_SyntaxError, "Python extern method dict base argument error.");
		return NULL;
	}

	/*project information for return*/
	PyObject *return_dict = PyDict_New();
    if (return_dict == NULL) {
    	return NULL;
    }

    /*case directory*/
	PyObject *casedir_list = PyList_New(0);
	if (casedir_list == NULL) {
		Py_DECREF(return_dict);
		return NULL;
	}
	int result = PyDict_SetItemString(return_dict, "casedir", casedir_list);
	Py_DECREF(casedir_list);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

	QList<QString> casedirs;
	g_tester->signalProjectCaseDirs(&casedirs);

	QList<QString>::iterator dir_iter;
	for (dir_iter = casedirs.begin(); dir_iter != casedirs.end(); dir_iter++) {
		QString dir = *dir_iter;

	    PyObject *path_str = PyString_FromString(dir.toStdString().c_str());
	    if (path_str == NULL) {
			Py_DECREF(return_dict);
			return NULL;
	    }
	    result = PyList_Append(casedir_list, path_str);
		Py_DECREF(path_str);
    	if (result < 0) {
        	Py_DECREF(return_dict);
        	return NULL;
    	}
	}

	return return_dict;
}

PyObject *UterPythonExtern_TargetInfo(PyObject *self, PyObject *args, PyObject *kw)
{
	if (self != NULL || args == NULL || !PyTuple_Check(args) || kw == NULL || !PyDict_Check(kw)) {
		PyErr_Format(PyExc_SyntaxError, "Python extern method dict base argument error.");
		return NULL;
	}

	Arch *arch = g_target->archInfoGet();
	if (arch == NULL) {
		PyErr_Format(PyExc_RuntimeError, "Can not get target type.");
		return NULL;
	}

	PyObject *return_dict = PyDict_New();
    if (return_dict == NULL) {
    	return NULL;
    }

	PyObject *types_dict = PyDict_New();
    if (types_dict == NULL) {
    	Py_DECREF(return_dict);
    	return NULL;
    }
    int result = PyDict_SetItemString(return_dict, "types", types_dict);
    Py_DECREF(types_dict);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    QList<ArchBaseType*>::iterator base_iter;
    for (base_iter = arch->m_base_list.begin(); base_iter != arch->m_base_list.end(); base_iter++) {
    	ArchBaseType *basetype = *base_iter;

		PyObject *type_dict = PyDict_New();
	    if (type_dict == NULL) {
	    	Py_DECREF(return_dict);
	    	return NULL;
	    }
	    int result = PyDict_SetItemString(types_dict, basetype->m_short_name.toStdString().c_str(), type_dict);
	    Py_DECREF(type_dict);
	    if (result < 0) {
	    	Py_DECREF(return_dict);
	    	return NULL;
	    }

	    PyObject *short_str = PyString_FromString(basetype->m_short_name.toStdString().c_str());
	    if (short_str == NULL) {
	    	Py_DECREF(return_dict);
	    	return NULL;
	    }
	    result = PyDict_SetItemString(type_dict, "short", short_str);
	    Py_DECREF(short_str);
	    if (result < 0) {
	    	Py_DECREF(return_dict);
	    	return NULL;
	    }

	    PyObject *daily_str = PyString_FromString(basetype->m_daily_name.toStdString().c_str());
	    if (daily_str == NULL) {
	    	Py_DECREF(return_dict);
	    	return NULL;
	    }
	    result = PyDict_SetItemString(type_dict, "daily", daily_str);
	    Py_DECREF(daily_str);
	    if (result < 0) {
	    	Py_DECREF(return_dict);
	    	return NULL;
	    }

	    PyObject *long_str = PyString_FromString(basetype->m_long_name.toStdString().c_str());
	    if (long_str == NULL) {
	    	Py_DECREF(return_dict);
	    	return NULL;
	    }
	    result = PyDict_SetItemString(type_dict, "long", long_str);
	    Py_DECREF(long_str);
	    if (result < 0) {
	    	Py_DECREF(return_dict);
	    	return NULL;
	    }

	    PyObject *full_str = PyString_FromString(basetype->m_full_name.toStdString().c_str());
	    if (full_str == NULL) {
	    	Py_DECREF(return_dict);
	    	return NULL;
	    }
	    result = PyDict_SetItemString(type_dict, "full", full_str);
	    Py_DECREF(full_str);
	    if (result < 0) {
	    	Py_DECREF(return_dict);
	    	return NULL;
	    }

	    PyObject *size_int = PyInt_FromSsize_t(basetype->m_data_size);
	    if (size_int == NULL) {
	    	Py_DECREF(return_dict);
	    	return NULL;
	    }
	    result = PyDict_SetItemString(type_dict, "size", size_int);
	    Py_DECREF(size_int);
	    if (result < 0) {
	    	Py_DECREF(return_dict);
	    	return NULL;
	    }

	    PyObject *align_int = PyInt_FromSsize_t(basetype->m_data_align);
	    if (align_int == NULL) {
	    	Py_DECREF(return_dict);
	    	return NULL;
	    }
	    result = PyDict_SetItemString(type_dict, "align", align_int);
	    Py_DECREF(align_int);
	    if (result < 0) {
	    	Py_DECREF(return_dict);
	    	return NULL;
	    }

	    PyObject *bool_obj;
	    if (basetype->m_signed_flag) {
	    	Py_INCREF(Py_True);
	    	bool_obj = Py_True;
	    } else {
	    	Py_INCREF(Py_False);
	    	bool_obj = Py_False;
	    }
	    result = PyDict_SetItemString(type_dict, "signed", bool_obj);
	    Py_DECREF(bool_obj);
	    if (result < 0) {
	    	Py_DECREF(return_dict);
	    	return NULL;
	    }
	}

    PyObject *bool_obj;
    if (arch->m_bigend) {
    	Py_INCREF(Py_True);
    	bool_obj = Py_True;
    } else {
    	Py_INCREF(Py_False);
    	bool_obj = Py_False;
    }
	result = PyDict_SetItemString(return_dict, "bigend", bool_obj);
    Py_DECREF(bool_obj);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

    Py_INCREF(Py_True);
    result = PyDict_SetItemString(return_dict, "alive", Py_True);
    Py_DECREF(Py_True);
    if (result < 0) {
    	Py_DECREF(return_dict);
    	return NULL;
    }

	return return_dict;
}

PyObject *UterPythonExtern_GetMemory(PyObject *self, PyObject *args, PyObject *kw)
{
	if (self != NULL || args == NULL || !PyTuple_Check(args) || kw == NULL || !PyDict_Check(kw)) {
		PyErr_Format(PyExc_SyntaxError, "Python extern method dict base argument error.");
		return NULL;
	}

	/*address*/
	PyObject *addr_obj = PyDict_GetItemString(kw, "addr");
	if (addr_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "addr");
		return NULL;
	}
	if (! PyLong_Check(addr_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "addr", "long");
		return NULL;
	}
	unsigned long long address_from = PyLong_AsUnsignedLongLong(addr_obj);

	/*length*/
	PyObject *num_obj = PyDict_GetItemString(kw, "len");
	if (num_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "len");
		return NULL;
	}
	if (! PyInt_Check(num_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "len", "int");
		return NULL;
	}
	unsigned int num_of_byte = PyInt_AsSsize_t(num_obj);

	/*get memory*/
	QByteArray array;
	int result = g_tester->memoryGet(address_from, num_of_byte, array);
	if (result < 0) {
		PyErr_Format(PyExc_RuntimeError, "Get memory failed.");
		return NULL;
	}

    PyObject *return_list = PyList_New(0);
    if (return_list == NULL) {
    	return NULL;
    }

    unsigned int iloop;
    for (iloop = 0; iloop < num_of_byte; iloop++) {
    	/*Force convert to uchar for zero extern, not use signal extern*/
    	PyObject *byte_int = PyInt_FromSsize_t((unsigned char)(array[iloop]));
    	if (byte_int == NULL) {
    		Py_DECREF(return_list);
    		return NULL;
    	}
    	result = PyList_Append(return_list, byte_int);
    	Py_DECREF(byte_int);
    	if (result < 0) {
        	Py_DECREF(return_list);
        	return NULL;
    	}
    }

    return return_list;
}

PyObject *UterPythonExtern_SetMemory(PyObject *self, PyObject *args, PyObject *kw)
{
	if (self != NULL || args == NULL || !PyTuple_Check(args) || kw == NULL || !PyDict_Check(kw)) {
		PyErr_Format(PyExc_SyntaxError, "Python extern method dict base argument error.");
		return NULL;
	}

	/*address*/
	PyObject *addr_obj = PyDict_GetItemString(kw, "addr");
	if (addr_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "addr");
		return NULL;
	}
	if (! PyLong_Check(addr_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "addr", "long");
		return NULL;
	}
	unsigned long long address_from = PyLong_AsUnsignedLongLong(addr_obj);

	/*bytes*/
	PyObject *bytes_tuple = PyDict_GetItemString(kw, "bytes");
	if (bytes_tuple == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "bytes");
		return NULL;
	}
	if (! PyTuple_Check(bytes_tuple)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "bytes", "tuple");
		return NULL;
	}

	QByteArray array;
	unsigned int num_of_value = PyTuple_Size(bytes_tuple);
	if (num_of_value == 0) {
		PyErr_Format(PyExc_ValueError, "Wrong number of bytes %d for memory set.", num_of_value);
		return NULL;
	}
	int iloop;
	for (iloop = 0; iloop < num_of_value; iloop++) {
		PyObject *byte_obj = PyTuple_GetItem(bytes_tuple, iloop);
		if (byte_obj == NULL) {
			PyErr_Format(PyExc_IndexError, "No index %d of memory bytes.", iloop);
			return NULL;
		}
		if (! PyInt_Check(byte_obj)) {
			PyErr_Format(PyExc_TypeError, "Index %d of memory byte should be python %s type.", iloop, "int");
			return NULL;
		}
		array.append(PyInt_AsSsize_t(byte_obj));
	}

	/*Set memory*/
	int result = g_tester->memorySet(address_from, num_of_value, array);
	if (result < 0) {
		PyErr_Format(PyExc_RuntimeError, "Set memory failed.");
		return NULL;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *UterPythonExtern_Malloc(PyObject *self, PyObject *args, PyObject *kw)
{
	if (self != NULL || args == NULL || !PyTuple_Check(args) || kw == NULL || !PyDict_Check(kw)) {
		PyErr_Format(PyExc_SyntaxError, "Python extern method dict base argument error.");
		return NULL;
	}

	/*length*/
	PyObject *len_obj = PyDict_GetItemString(kw, "len");
	if (len_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "len");
		return NULL;
	}
	if (! PyInt_Check(len_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "len", "int");
		return NULL;
	}
	unsigned int malloc_size = PyInt_AsSsize_t(len_obj);

	/*select pid*/
	unsigned int pid = g_tester->callSelectThread();
	if (pid == UTER_INVALID_PID) {
		PyErr_Format(PyExc_RuntimeError, "Cat not get an applicable thread.");
		return NULL;
	}

	/*alloc memory*/
	unsigned long long point;
	int result = UterMalloc(pid, malloc_size, &point);
	if (result != 0) {
		PyErr_Format(PyExc_RuntimeError, "Malloc failed.");
		return NULL;
	}

	PyObject *point_obj = PyLong_FromUnsignedLongLong(point);
	if (point_obj == NULL) {
		return NULL;
	}
	return point_obj;
}

PyObject *UterPythonExtern_Free(PyObject *self, PyObject *args, PyObject *kw)
{
	if (self != NULL || args == NULL || !PyTuple_Check(args) || kw == NULL || !PyDict_Check(kw)) {
		PyErr_Format(PyExc_SyntaxError, "Python extern method dict base argument error.");
		return NULL;
	}

	/*address*/
	PyObject *addr_obj = PyDict_GetItemString(kw, "addr");
	if (addr_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "addr");
		return NULL;
	}
	if (! PyLong_Check(addr_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "addr", "long");
		return NULL;
	}
	unsigned long long free_point = PyLong_AsUnsignedLong(addr_obj);

	/*select pid*/
	unsigned int pid = g_tester->callSelectThread();
	if (pid == UTER_INVALID_PID) {
		PyErr_Format(PyExc_RuntimeError, "Cat not get an applicable thread.");
		return NULL;
	}

	/*free memory*/
	int result = UterFree(pid, free_point);
	if (result != 0) {
		PyErr_Format(PyExc_RuntimeError, "Free failed.");
		return NULL;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *UterPythonExtern_Call(PyObject *self, PyObject *args, PyObject *kw)
{
	if (self != NULL || args == NULL || !PyTuple_Check(args) || kw == NULL || !PyDict_Check(kw)) {
		PyErr_Format(PyExc_SyntaxError, "Python extern method dict base argument error.");
		return NULL;
	}

	/*address*/
	PyObject *fun_addr_obj = PyDict_GetItemString(kw, "addr");
	if (fun_addr_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "addr");
		return NULL;
	}
	if (! PyLong_Check(fun_addr_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "addr", "long");
		return NULL;
	}
	unsigned long long fun_addr = PyLong_AsUnsignedLongLong(fun_addr_obj);

	/*return variable*/
	PyObject *fun_ret_dict = PyDict_GetItemString(kw, "ret");
	if (fun_ret_dict == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "ret");
		return NULL;
	}
	if (! PyDict_Check(fun_ret_dict)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "ret", "dict");
		return NULL;
	}

	/*argument variable in tuple*/
	PyObject *fun_args_tuple = PyDict_GetItemString(kw, "args");
	if (fun_args_tuple == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "args");
		return NULL;
	}
	if (! PyTuple_Check(fun_args_tuple)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "args", "dict");
		return NULL;
	}

	CallFunDesc fun_desc;

	/*return variable address*/
	PyObject *addr_obj = PyDict_GetItemString(fun_ret_dict, "address");
	if (addr_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword for ret with name %s.", "address");
		return NULL;
	}
	if (! PyLong_Check(addr_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword %s for ret should be python %s type.", "address", "long");
		return NULL;
	}
	fun_desc.m_ret.m_addr = PyLong_AsUnsignedLong(addr_obj);

	/*return variable type description*/
	PyObject *desc_obj = PyDict_GetItemString(fun_ret_dict, "desc");
	if (desc_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword for ret with name %s.", "desc");
		return NULL;
	}
	if (! PyString_Check(desc_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword %s for ret should be python %s type.", "desc", "string");
		return NULL;
	}
	fun_desc.m_ret.m_desc = PyString_AsString(desc_obj);

	/*return variable length*/
	PyObject *len_obj = PyDict_GetItemString(fun_ret_dict, "len");
	if (len_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword for ret with name %s.", "len");
		return NULL;
	}
	if (! PyInt_Check(len_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword %s for ret should be python %s type.", "len", "int");
		return NULL;
	}
	fun_desc.m_ret.m_len = PyInt_AsSsize_t(len_obj);

	/*return variable align*/
	PyObject *align_obj = PyDict_GetItemString(fun_ret_dict, "align");
	if (len_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword for ret with name %s.", "align");
		return NULL;
	}
	if (! PyInt_Check(len_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword %s for ret should be python %s type.", "align", "int");
		return NULL;
	}
	fun_desc.m_ret.m_align = PyInt_AsSsize_t(align_obj);

	/*arguments*/
	unsigned int count = PyTuple_Size(fun_args_tuple);
	unsigned int iloop;
	for (iloop = 0; iloop < count; iloop++) {
		/*argument variable*/
		PyObject *fun_arg_dict = PyTuple_GetItem(fun_args_tuple, iloop);
		if (fun_arg_dict == NULL) {
			PyErr_Format(PyExc_IndexError, "No object on argument index %d.", iloop);
			return NULL;
		}
		if (! PyDict_Check(fun_arg_dict)) {
			PyErr_Format(PyExc_TypeError, "Function argument index %d should be python %s type.", iloop, "dict");
			return NULL;
		}

		/*argument address*/
		PyObject *addr_obj = PyDict_GetItemString(fun_arg_dict, "address");
		if (addr_obj == NULL) {
			PyErr_Format(PyExc_KeyError, "Function argument index %d should have key %s.", iloop, "address");
			return NULL;
		}
		if(! PyLong_Check(addr_obj)) {
			PyErr_Format(PyExc_TypeError, "Function argument index %d should have key %s with python %s type.", iloop, "address", "long");
			return NULL;
		}
		unsigned long long addr = PyLong_AsUnsignedLong(addr_obj);

		/*argument variable type description*/
		PyObject *desc_obj = PyDict_GetItemString(fun_arg_dict, "desc");
		if (desc_obj == NULL) {
			PyErr_Format(PyExc_KeyError, "Function argument index %d should have key %s.", iloop, "desc");
			return NULL;
		}
		if(! PyString_Check(desc_obj)) {
			PyErr_Format(PyExc_TypeError, "Function argument index %d should have key %s with python %s type.", iloop, "desc", "string");
			return NULL;
		}
		char *desc = PyString_AsString(desc_obj);

		/*argument variable length*/
		PyObject *len_obj = PyDict_GetItemString(fun_arg_dict, "len");
		if (len_obj == NULL) {
			PyErr_Format(PyExc_KeyError, "Function argument index %d should have key %s.", iloop, "len");
			return NULL;
		}
		if(! PyInt_Check(len_obj)) {
			PyErr_Format(PyExc_TypeError, "Function argument index %d should have key %s with python %s type.", iloop, "len", "int");
			return NULL;
		}
		unsigned int len = PyInt_AsSsize_t(len_obj);

		/*argument variable align*/
		PyObject *align_obj = PyDict_GetItemString(fun_arg_dict, "align");
		if (align_obj == NULL) {
			PyErr_Format(PyExc_KeyError, "Function argument index %d should have key %s.", iloop, "align");
			return NULL;
		}
		if(! PyInt_Check(align_obj)) {
			PyErr_Format(PyExc_TypeError, "Function argument index %d should have key %s with python %s type.", iloop, "align", "int");
			return NULL;
		}
		unsigned int align = PyInt_AsSsize_t(align_obj);

		/*save argument*/
		CallDataDesc *arg_desc = new CallDataDesc;
		arg_desc->m_addr = addr;
		arg_desc->m_desc = desc;
		arg_desc->m_len = len;
		arg_desc->m_align = align;

		fun_desc.m_args.push_back(arg_desc);
	}

	/*analyse return type*/
	if (fun_desc.m_ret.m_desc == "void") {
		fun_desc.m_call_type = CallFunDesc::CALL_RET_VOID;
	} else if (fun_desc.m_ret.m_desc == "longlong"
	        || fun_desc.m_ret.m_desc == "ulonglong")  {
		fun_desc.m_call_type = CallFunDesc::CALL_RET_EAXEDX;
	} else if (fun_desc.m_ret.m_desc == "struct"
	        || fun_desc.m_ret.m_desc == "union")  {
		fun_desc.m_call_type = CallFunDesc::CALL_RET_POINT;
	} else {
		fun_desc.m_call_type = CallFunDesc::CALL_RET_EAX;
	}

	/*select pid*/
	unsigned int pid = g_tester->callSelectThread();
	if (pid == UTER_INVALID_PID) {
		PyErr_Format(PyExc_RuntimeError, "Cat not get applicable thread.");
		return NULL;
	}

	/*run call*/
	int result = TestCall(pid, fun_addr, &fun_desc);
	if (result != 0) {
		PyErr_Format(PyExc_RuntimeError, "Call error");
		return NULL;
	}

	/*no return value, because return variable exist already*/
	PyObject *ret_obj = PyLong_FromUnsignedLongLong(0);
	if (ret_obj == NULL) {
		return NULL;
	}
	return ret_obj;
}

/*Just operate stub register table*/
PyObject *UterPythonExtern_Stub(PyObject *self, PyObject *args, PyObject *kw)
{
	if (self != NULL || args == NULL || !PyTuple_Check(args) || kw == NULL || !PyDict_Check(kw)) {
		PyErr_Format(PyExc_SyntaxError, "Python extern method dict base argument error.");
		return NULL;
	}

	QMutexLocker locker(&(g_tester->m_data_lock));

	/*function address*/
	PyObject *fun_addr_obj = PyDict_GetItemString(kw, "addr");
	if (fun_addr_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "addr");
		return NULL;
	}
	if (! PyLong_Check(fun_addr_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "addr", "long");
		return NULL;
	}
	unsigned long long fun_addr = PyLong_AsUnsignedLongLong(fun_addr_obj);

	/*set or clear*/
	PyObject *is_set_obj = PyDict_GetItemString(kw, "set");
	if (is_set_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "set");
		return NULL;
	}
	if (! PyBool_Check(is_set_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "set", "bool");
		return NULL;
	}

	if (is_set_obj == Py_False) {
		/*Clear*/
		QMap<unsigned long long, TesterStub*>::iterator iter;
		iter = g_tester->m_stub_map.find(fun_addr);
		if (iter == g_tester->m_stub_map.end()) {
			Py_INCREF(Py_None);
			return Py_None;
		}
		TesterStub *stub = iter.value();
		g_tester->m_stub_map.erase(iter);

		/*Delete breakpoint*/
		g_target->breakDelete(stub->m_address, TargetBreak::BREAK_STUB);

		/*Release stub object*/
		Py_DECREF(stub->m_driver_arg_obj);
		stub->m_driver_arg_obj = NULL;
		/*Release stub information*/
		delete stub->m_fun_desc;
		delete stub;

		Py_INCREF(Py_None);
		return Py_None;
	}

	/*stub information dict*/
	PyObject *stubarg_dict = PyDict_GetItemString(kw, "driver_arg");
	if (stubarg_dict == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "driver_arg");
		return NULL;
	}
	if (! PyDict_Check(stubarg_dict)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "driver_arg", "dict");
		return NULL;
	}

	/*stub name*/
	PyObject *name_obj = PyDict_GetItemString(stubarg_dict, "name");
	if (name_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No key on argument %s with name %s.", "driver_arg", "name");
		return NULL;
	}
	if (! PyString_Check(name_obj)) {
		PyErr_Format(PyExc_TypeError, "Key on argument %s %s should be python %s type.", "driver_arg", "name", "string");
		return NULL;
	}

	/*stub driver*/
	PyObject *driver_obj = PyDict_GetItemString(stubarg_dict, "driver");
	if (driver_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No key on argument %s with name %s.", "driver_arg", "driver");
		return NULL;
	}
	if (! PyFunction_Check(driver_obj)) {
		PyErr_Format(PyExc_TypeError, "Key on argument %s %s should be python %s type.", "driver_arg", "driver", "function");
		return NULL;
	}

	/*stub script*/
	PyObject *stub_obj = PyDict_GetItemString(stubarg_dict, "stub");
	if (stub_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No key on argument %s with name %s.", "driver_arg", "stub");
		return NULL;
	}
	if (! PyFunction_Check(stub_obj)) {
		PyErr_Format(PyExc_TypeError, "Key on argument %s %s should be python %s type.", "driver_arg", "stub", "function");
		return NULL;
	}

	/*stub return description*/
	PyObject *fun_ret_dict = PyDict_GetItemString(stubarg_dict, "ret");
	if (fun_ret_dict == NULL) {
		PyErr_Format(PyExc_KeyError, "No key on argument %s with name %s.", "driver_arg", "ret");
		return NULL;
	}
	if (! PyDict_Check(fun_ret_dict)) {
		PyErr_Format(PyExc_TypeError, "Key on argument %s %s should be python %s type.", "driver_arg", "ret", "dict");
		return NULL;
	}

	/*stub argument description in tuple*/
	PyObject *fun_args_tuple = PyDict_GetItemString(stubarg_dict, "args");
	if (fun_args_tuple == NULL) {
		PyErr_Format(PyExc_KeyError, "No key on argument %s with name %s.", "driver_arg", "args");
		return NULL;
	}
	if (! PyTuple_Check(fun_args_tuple)) {
		PyErr_Format(PyExc_TypeError, "Key on argument %s %s should be python %s type.", "driver_arg", "args", "tuple");
		return NULL;
	}

	/*return description*/
	PyObject *desc_obj = PyDict_GetItemString(fun_ret_dict, "desc");
	if (desc_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword for %s with name %s.", "driver_arg.ret", "desc");
		return NULL;
	}
	if (! PyString_Check(desc_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword %s for %s should be python %s type.", "desc", "driver_arg.ret", "string");
		return NULL;
	}
	char *desc = PyString_AsString(desc_obj);

	/*return length*/
	PyObject *len_obj = PyDict_GetItemString(fun_ret_dict, "len");
	if (len_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword for %s with name %s.", "driver_arg.ret", "len");
		return NULL;
	}
	if (! PyInt_Check(len_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword %s for %s should be python %s type.", "len", "driver_arg.ret", "int");
		return NULL;
	}
	unsigned int len = PyInt_AsSsize_t(len_obj);

	/*return align*/
	PyObject *align_obj = PyDict_GetItemString(fun_ret_dict, "align");
	if (align_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword for %s with name %s.", "driver_arg.ret", "align");
		return NULL;
	}
	if (! PyInt_Check(align_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword %s for %s should be python %s type.", "align", "driver_arg.ret", "int");
		return NULL;
	}
	unsigned int align = PyInt_AsSsize_t(align_obj);

	CallFunDesc *fun_desc = new CallFunDesc();
	fun_desc->m_ret.m_desc = desc;
	fun_desc->m_ret.m_len = len;
	fun_desc->m_ret.m_align = align;

	/*analyse return type*/
	if (fun_desc->m_ret.m_desc == "void") {
		fun_desc->m_call_type = CallFunDesc::CALL_RET_VOID;
	} else if (fun_desc->m_ret.m_desc == "longlong"
	        || fun_desc->m_ret.m_desc == "ulonglong")  {
		fun_desc->m_call_type = CallFunDesc::CALL_RET_EAXEDX;
	} else if (fun_desc->m_ret.m_desc == "struct"
	        || fun_desc->m_ret.m_desc == "union")  {
		fun_desc->m_call_type = CallFunDesc::CALL_RET_POINT;
	} else {
		fun_desc->m_call_type = CallFunDesc::CALL_RET_EAX;
	}

	unsigned int count = PyTuple_Size(fun_args_tuple);
	unsigned int iloop;
	for (iloop = 0; iloop < count; iloop++) {
		PyObject *fun_arg_dict = PyTuple_GetItem(fun_args_tuple, iloop);
		if (fun_arg_dict == NULL) {
			PyErr_Format(PyExc_IndexError, "No object on argument index %d.", iloop);
			break;
		}
		if (! PyDict_Check(fun_arg_dict)) {
			PyErr_Format(PyExc_TypeError, "Function argument index %d should be python %s type.", iloop, "dict");
			break;
		}

		PyObject *desc_obj = PyDict_GetItemString(fun_arg_dict, "desc");
		if (desc_obj == NULL) {
			PyErr_Format(PyExc_KeyError, "Function argument index %d should have key %s.", iloop, "desc");
			break;
		}
		if (! PyString_Check(desc_obj)) {
			PyErr_Format(PyExc_TypeError, "Function argument index %d should have key %s with python %s type.", iloop, "desc", "string");
			break;
		}
		char *desc = PyString_AsString(desc_obj);

		PyObject *len_obj = PyDict_GetItemString(fun_arg_dict, "len");
		if (len_obj == NULL) {
			PyErr_Format(PyExc_KeyError, "Function argument index %d should have key %s.", iloop, "len");
			break;
		}
		if (! PyInt_Check(len_obj)) {
			PyErr_Format(PyExc_TypeError, "Function argument index %d should have key %s with python %s type.", iloop, "len", "int");
			break;
		}
		unsigned int len = PyInt_AsSsize_t(len_obj);

		PyObject *align_obj = PyDict_GetItemString(fun_arg_dict, "align");
		if (align_obj == NULL) {
			PyErr_Format(PyExc_KeyError, "Function argument index %d should have key %s.", iloop, "align");
			break;
		}
		if (! PyInt_Check(len_obj)) {
			PyErr_Format(PyExc_TypeError, "Function argument index %d should have key %s with python %s type.", iloop, "align", "int");
			break;
		}
		unsigned int align = PyInt_AsSsize_t(align_obj);

		CallDataDesc *arg_desc = new CallDataDesc;
		arg_desc->m_desc = desc;
		arg_desc->m_len = len;
		arg_desc->m_align = align;

		fun_desc->m_args.push_back(arg_desc);
	}
	if (iloop != count) {
		delete fun_desc;
		return NULL;
	}

	/*Update if stub exist*/
	QMap<unsigned long long, TesterStub*>::iterator iter_find;
	iter_find = g_tester->m_stub_map.find(fun_addr);
	if (iter_find != g_tester->m_stub_map.end()) {
		TesterStub *stub = iter_find.value();
		/*Clear old*/
		delete stub->m_fun_desc;
		stub->m_fun_desc = NULL;

		Py_DECREF(stub->m_driver_arg_obj);
		stub->m_driver_arg_obj = NULL;

		/*Set new*/
		stub->m_fun_desc = fun_desc;

		stub->m_driver_arg_obj = stubarg_dict;
		Py_INCREF(stub->m_driver_arg_obj);

		Py_INCREF(Py_None);
		return Py_None;
	}

	/*Create new stub*/
	int result = g_target->breakInsert(fun_addr, TargetBreak::BREAK_STUB);
	if (result != 0) {
		delete fun_desc;
		PyErr_Format(PyExc_RuntimeError, "Set breakpoint at 0x%016llx failed.", fun_addr);
		return NULL;
	}

	TesterStub *stub = new TesterStub();
	stub->m_address = fun_addr;
	stub->m_fun_desc = fun_desc;
	stub->m_driver_arg_obj = stubarg_dict;
	Py_INCREF(stub->m_driver_arg_obj);

	g_tester->m_stub_map.insert(stub->m_address, stub);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *UterPythonExtern_StubAdmin(PyObject *self, PyObject *args, PyObject *kw)
{
	if (self != NULL || args == NULL || !PyTuple_Check(args) || kw == NULL || !PyDict_Check(kw)) {
		PyErr_Format(PyExc_SyntaxError, "Python extern method dict base argument error.");
		return NULL;
	}

	/*operate type*/
	PyObject *opt_obj = PyDict_GetItemString(kw, "opt");
	if (opt_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "opt");
		return NULL;
	}
	if (! PyString_Check(opt_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "opt", "string");
		return NULL;
	}
	char *opt = PyString_AsString(opt_obj);

	QMutexLocker locker(&(g_target->m_data_lock));

	if (strcmp(opt, "list") == 0) {
		/*list stub*/
	    PyObject *return_list = PyList_New(0);
	    if (return_list == NULL) {
	    	return NULL;
	    }

	    QMap<unsigned long long, TesterStub*>::iterator iter;
	    for (iter = g_tester->m_stub_map.begin(); iter != g_tester->m_stub_map.end(); iter++) {
	    	TesterStub *teststub = iter.value();

	        PyObject *stub_dict = PyDict_New();
	        if (stub_dict == NULL) {
	        	Py_DECREF(return_list);
	        	return NULL;
	        }
	        int result = PyList_Append(return_list, stub_dict);
	    	Py_DECREF(stub_dict);
	    	if (result < 0) {
	        	Py_DECREF(return_list);
	        	return NULL;
	    	}

	    	/*address*/
	    	PyObject *addr_long = PyLong_FromUnsignedLongLong(teststub->m_address);
	    	if (addr_long == NULL) {
	    		Py_DECREF(return_list);
	    		return NULL;
	    	}
	    	result = PyDict_SetItemString(stub_dict, "address", addr_long);
	        Py_DECREF(addr_long);
	        if (result < 0) {
	        	Py_DECREF(return_list);
	        	return NULL;
	        }

	        /*stub information*/
	    	result = PyDict_SetItemString(stub_dict, "arg", teststub->m_driver_arg_obj);
	        /*no Py_DECREF();*/
	        if (result < 0) {
	        	Py_DECREF(return_list);
	        	return NULL;
	        }
	    }

	    return return_list;
	} else if (strcmp(opt, "clean") == 0) {
		/*stub clean*/
		unsigned long long addr = 0;
		PyObject *arg_obj = PyDict_GetItemString(kw, "addr");
		if (arg_obj != NULL) {
			if (! PyLong_Check(arg_obj)) {
				PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "addr", "long");
				return NULL;
			}
			addr = PyLong_AsUnsignedLongLong(arg_obj);
		}

		QMap<unsigned long long, TesterStub*>::iterator iter;
	    for (iter = g_tester->m_stub_map.begin(); iter != g_tester->m_stub_map.end(); /*none*/) {
	    	TesterStub *stub = iter.value();

	    	if (addr == 0 || addr == stub->m_address) {
	    		/*Clear stub*/
	    		g_tester->m_stub_map.erase(iter++);

	    		g_target->breakDelete(stub->m_address, TargetBreak::BREAK_STUB);

	    		Py_DECREF(stub->m_driver_arg_obj);
	    		stub->m_driver_arg_obj = NULL;

	    		delete stub->m_fun_desc;
	    		delete stub;
	    	} else {
	    		iter++;
	    	}
	    }

	    Py_INCREF(Py_None);
	    return Py_None;
	} else {
		PyErr_Format(PyExc_KeyError, "Unknown operate %s.", opt);
		return NULL;
	}
}

PyObject *UterPythonExtern_FunCover(PyObject *self, PyObject *args, PyObject *kw)
{
	if (self != NULL || args == NULL || !PyTuple_Check(args) || kw == NULL || !PyDict_Check(kw)) {
		PyErr_Format(PyExc_SyntaxError, "Python extern method dict base argument error.");
		return NULL;
	}

	/*address*/
	PyObject *addr_obj = PyDict_GetItemString(kw, "addr");
	if (addr_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "addr");
		return NULL;
	}
	if (! PyLong_Check(addr_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "addr", "long");
		return NULL;
	}
	unsigned long long addr = PyLong_AsUnsignedLongLong(addr_obj);

	/*operate type*/
	PyObject *opt_obj = PyDict_GetItemString(kw, "opt");
	if (opt_obj == NULL) {
		PyErr_Format(PyExc_KeyError, "No keyword argument with name %s.", "opt");
		return NULL;
	}
	if (! PyString_Check(opt_obj)) {
		PyErr_Format(PyExc_TypeError, "Keyword argument %s should be python %s type.", "opt", "string");
		return NULL;
	}
	char *opt = PyString_AsString(opt_obj);
	if (! (strcmp(opt, "on") == 0 || strcmp(opt, "off") == 0 || strcmp(opt, "reset") == 0
			|| strcmp(opt, "show") == 0 || strcmp(opt, "count") == 0)) {
		PyErr_Format(PyExc_ValueError, "Argument opt error");
		return NULL;
	}

	TargetFunCovArg arg;
	arg.m_opt = opt;

	/*Turn address to mod/cu/function*/
	g_target->m_data_lock.lock();
	TargetMod *mod = g_target->modGetByAddrIn(addr);
	if (mod == NULL) {
		g_target->m_data_lock.unlock();
		PyErr_Format(PyExc_ValueError, "No module with address 0x%016llx in", addr);
		return NULL;
	}
	unsigned long long offset_addr = addr - mod->m_add_mkup;
	DwfCu *cu = mod->dwarfGetCuByAddrIn(offset_addr);
	if (cu == NULL) {
		g_target->m_data_lock.unlock();
		PyErr_Format(PyExc_ValueError, "No cu with address 0x%016llx in", addr);
		return NULL;
	}
	DwfFun *fun = cu->getFunByAddr(offset_addr);
	if (fun == NULL) {
		g_target->m_data_lock.unlock();
		PyErr_Format(PyExc_ValueError, "No function begin with address 0x%016llx in", addr);
		return NULL;
	}
	arg.m_mod = mod->m_remote_path;
	arg.m_cu = cu->m_path;
	arg.m_fun = fun->m_name;
	g_target->m_data_lock.unlock();

	/*request*/
	emit g_tester->signalTargetCovSet(&arg);
	if (arg.m_err_str != "") {
		PyErr_Format(PyExc_RuntimeError, "%s", arg.m_err_str.toStdString().c_str());
		return NULL;
	}

	if (strcmp(opt, "show") == 0) {
		PyObject *show_str = PyString_FromString(arg.m_show_str.toStdString().c_str());
		return show_str;
	} else if (strcmp(opt, "count") == 0) {
		PyObject *count_dict = PyDict_New();
		if (count_dict == NULL) {
			return NULL;
		}

		PyObject *num_hit_obj = PyInt_FromSsize_t(arg.m_count_hit);
		if (num_hit_obj == NULL) {
			Py_DECREF(count_dict);
			return NULL;
		}
		int result = PyDict_SetItemString(count_dict, "covs", num_hit_obj);
		Py_DECREF(num_hit_obj);
        if (result < 0) {
        	Py_DECREF(count_dict);
        	return NULL;
        }

		PyObject *num_line_obj = PyInt_FromSsize_t(arg.m_count_line);
		if (num_line_obj == NULL) {
			Py_DECREF(count_dict);
			return NULL;
		}
		result = PyDict_SetItemString(count_dict, "sum", num_line_obj);
		Py_DECREF(num_line_obj);
        if (result < 0) {
        	Py_DECREF(count_dict);
        	return NULL;
        }

		return count_dict;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

PyMethodDef UterPythonExternMethods[] = {
	{"Hello",		(PyCFunction)UterPythonExtern_Hello,		METH_VARARGS, ""},
	{"Macro",		(PyCFunction)UterPythonExtern_Macro,		METH_VARARGS | METH_KEYWORDS, ""},
	{"Type",		(PyCFunction)UterPythonExtern_Type,			METH_VARARGS | METH_KEYWORDS, ""},
	{"Variable",	(PyCFunction)UterPythonExtern_Variable,		METH_VARARGS | METH_KEYWORDS, ""},
	{"Function",	(PyCFunction)UterPythonExtern_Function,		METH_VARARGS | METH_KEYWORDS, ""},
	{"Symbol",		(PyCFunction)UterPythonExtern_Symbol,		METH_VARARGS | METH_KEYWORDS, ""},
	{"Files",		(PyCFunction)UterPythonExtern_Files,		METH_VARARGS | METH_KEYWORDS, ""},
	{"CompileUnits",(PyCFunction)UterPythonExtern_CompileUnits,	METH_VARARGS | METH_KEYWORDS, ""},
	{"Modules",		(PyCFunction)UterPythonExtern_Modules,		METH_VARARGS | METH_KEYWORDS, ""},
	{"ProjectInfo",	(PyCFunction)UterPythonExtern_ProjectInfo,	METH_VARARGS | METH_KEYWORDS, ""},
	{"TargetInfo",	(PyCFunction)UterPythonExtern_TargetInfo,	METH_VARARGS | METH_KEYWORDS, ""},
	{"GetMemory",	(PyCFunction)UterPythonExtern_GetMemory,	METH_VARARGS | METH_KEYWORDS, ""},
	{"SetMemory",	(PyCFunction)UterPythonExtern_SetMemory,	METH_VARARGS | METH_KEYWORDS, ""},
	{"Malloc",		(PyCFunction)UterPythonExtern_Malloc,		METH_VARARGS | METH_KEYWORDS, ""},
	{"Free",		(PyCFunction)UterPythonExtern_Free,			METH_VARARGS | METH_KEYWORDS, ""},
	{"Call",		(PyCFunction)UterPythonExtern_Call,			METH_VARARGS | METH_KEYWORDS, ""},
	{"Stub",		(PyCFunction)UterPythonExtern_Stub,			METH_VARARGS | METH_KEYWORDS, ""},
	{"StubAdmin",	(PyCFunction)UterPythonExtern_StubAdmin,	METH_VARARGS | METH_KEYWORDS, ""},
	{"FunCover",	(PyCFunction)UterPythonExtern_FunCover,		METH_VARARGS | METH_KEYWORDS, ""},
	{NULL, NULL, 0, NULL}
};

static QString UterPython_FunArgsDesc(PyObject *fun_obj, bool ignore_self = false)
{
	QString ret_str = "";
	if (! PyFunction_Check(fun_obj)) {
		return ret_str;
	}

	PyObject *func_defaults_obj = NULL;
	PyObject *func_code_obj = NULL;
	PyObject *co_flags_obj = NULL;
	PyObject *co_argcount_obj = NULL;
	PyObject *co_varnames_obj = NULL;

	unsigned int default_count;
	func_defaults_obj = PyObject_GetAttrString(fun_obj, "func_defaults");
	if (func_defaults_obj == NULL) {
		goto finish;
	} else if (PyTuple_Check(func_defaults_obj)) {
		default_count = PyTuple_Size(func_defaults_obj);
	} else if (Py_None == func_defaults_obj) {
		default_count = 0;
	} else {
		goto finish;
	}

	func_code_obj = PyObject_GetAttrString(fun_obj, "func_code");
	if (func_code_obj == NULL || ! PyCode_Check(func_code_obj)) {
		goto finish;
	}

	/*co_flags*/
	co_flags_obj = PyObject_GetAttrString(func_code_obj, "co_flags");
	if (co_flags_obj == NULL || ! PyInt_Check(co_flags_obj)) {
		goto finish;
	}
	unsigned int flags;
	flags = PyInt_AsSsize_t(co_flags_obj);

	/*co_argcount*/
	co_argcount_obj = PyObject_GetAttrString(func_code_obj, "co_argcount");
	if (co_argcount_obj == NULL || ! PyInt_Check(co_argcount_obj)) {
		goto finish;
	}

	/*co_varnames*/
	co_varnames_obj = PyObject_GetAttrString(func_code_obj, "co_varnames");
	if (co_varnames_obj == NULL || ! PyTuple_Check(co_varnames_obj)) {
		goto finish;
	}

	int index;
	index = 0;
	/*Normal arguments list*/
	unsigned int arg_count;
	int iloop;
	arg_count = PyInt_AsSsize_t(co_argcount_obj);
	for (iloop = 0; iloop < arg_count; iloop++) {
		if (iloop == 0 && ignore_self) {
			/*Ignore self*/
			continue;
		}
		PyObject *varname_obj = PyTuple_GetItem(co_varnames_obj, iloop);
		if (varname_obj == NULL || !PyString_Check(varname_obj)) {
			ret_str = "";
			goto finish;
		}
		char *arg_name = PyString_AsString(varname_obj);
		if (arg_name == NULL) {
			ret_str = "";
			goto finish;
		}
		if (index++ != 0) {
			ret_str += ", ";
		}
		ret_str += arg_name;
		/*Defalut value*/
		if (iloop >= arg_count - default_count) {
			unsigned int default_at = iloop - (arg_count - default_count);
			PyObject *default_obj = PyTuple_GetItem(func_defaults_obj, default_at);

			PyObject *default_desc = PyObject_Str(default_obj);
			if (default_desc == NULL || ! PyString_Check(default_desc)) {
				ret_str = "";
				goto finish;
			}

			const char *default_str = PyString_AsString(default_desc);
			if (default_str == NULL) {
				Py_DECREF(default_desc);

				ret_str = "";
				goto finish;
			}

			ret_str += " = ";
			if (PyString_Check(default_obj)) {
				ret_str += "\"";
				ret_str += default_str;
				ret_str += "\"";
			} else {
				ret_str += default_str;
			}
			Py_DECREF(default_desc);
		}
	}
	/*Special arguments*/
	if (flags & CO_VARARGS) {
		PyObject *varname_obj = PyTuple_GetItem(co_varnames_obj, iloop);
		if (varname_obj == NULL || ! PyString_Check(varname_obj)) {
			ret_str = "";
			goto finish;
		}
		char *arg_name = PyString_AsString(varname_obj);
		if (arg_name == NULL) {
			ret_str = "";
			goto finish;
		}
		if (index++ != 0) {
			ret_str += ", ";
		}
		ret_str += "*";
		ret_str += arg_name;
		iloop++;
	}
	if (flags & CO_VARKEYWORDS) {
		PyObject *varname_obj = PyTuple_GetItem(co_varnames_obj, iloop);
		if (varname_obj == NULL || ! PyString_Check(varname_obj)) {
			ret_str = "";
			goto finish;
		}
		char *arg_name = PyString_AsString(varname_obj);
		if (arg_name == NULL) {
			ret_str = "";
			goto finish;
		}
		if (index++ != 0) {
			ret_str += ", ";
		}
		ret_str += "**";
		ret_str += arg_name;
		iloop++;
	}
finish:
	if (func_defaults_obj != NULL) {
		Py_DECREF(func_defaults_obj);
		func_defaults_obj = NULL;
	}
	if (func_code_obj != NULL) {
		Py_DECREF(func_code_obj);
		func_code_obj = NULL;
	}
	if (co_flags_obj != NULL) {
		Py_DECREF(co_flags_obj);
		co_flags_obj = NULL;
	}
	if (co_varnames_obj != NULL) {
		Py_DECREF(co_varnames_obj);
		co_varnames_obj = NULL;
	}
	if (co_argcount_obj != NULL) {
		Py_DECREF(co_argcount_obj);
		co_argcount_obj = NULL;
	}

	return ret_str;
}

static QStringList UterPython_ObjDesc(QString attr_name, PyObject *attr_obj)
{
	QStringList ret_list;

	if (attr_name == "") {
		int x = 5;
	}

	if (PyObject_HasAttrString(attr_obj, "__mro__")) {
		/*Have mro, so find and call __init___*/
		PyObject *mro_obj = PyObject_GetAttrString(attr_obj, "__mro__");
		if (mro_obj != NULL && PyTuple_Check(mro_obj)) {
			unsigned long count = PyTuple_Size(mro_obj);
			unsigned long iloop;
			for (iloop = 0; iloop < count; iloop++) {
				PyObject *parent_obj = PyTuple_GetItem(mro_obj, iloop);
				if (parent_obj != NULL) {
					/*Find __init__ in parent*/
					PyObject *init_obj = PyObject_GetAttrString(attr_obj, "__init__");
					if (init_obj != NULL && PyMethod_Check(init_obj)) {
						PyObject *func_obj = PyObject_GetAttrString(init_obj, "__func__");
						if (func_obj != NULL && PyFunction_Check(func_obj)) {
							QString desc = /*Class name*/attr_name + "(" + /*Arguments list*/UterPython_FunArgsDesc(func_obj, true) + ")";
							ret_list.append(desc);
							Py_DECREF(func_obj);
						}
						Py_DECREF(init_obj);
						break;
					}
				}
			}
			Py_DECREF(mro_obj);
		}
		/*Class have some attribute, so need show class name*/
		if (attr_name != "") {
			ret_list.append(attr_name);
		}
	} else if (PyMethod_Check(attr_obj)) {
		PyObject *func_obj = PyObject_GetAttrString(attr_obj, "__func__");
		if (func_obj != NULL && PyFunction_Check(func_obj)) {
			if (attr_name == "__init__") {
				QString desc = QString("(") + /*Arguments list*/UterPython_FunArgsDesc(func_obj, true) + ")";
				ret_list.append(desc);
			}
			if (attr_name == "__getitem__") {
				QString desc = QString("[") + /*Arguments list*/UterPython_FunArgsDesc(func_obj, true) + "]";
				ret_list.append(desc);
			}
			QString desc = /*Method name*/attr_name + "(" + /*Arguments list*/UterPython_FunArgsDesc(func_obj, true) + ")";
			ret_list.append(desc);
			Py_DECREF(func_obj);
		}
	} else if (PyFunction_Check(attr_obj)) {
		QString desc = /*Function name*/attr_name + "(" + /*Arguments list*/UterPython_FunArgsDesc(attr_obj) + ")";
		ret_list.append(desc);
	} else if (attr_name != "") {
		ret_list.append(attr_name);
	} else {
		;
	}
	return ret_list;
}

QStringList UterPython_AutoCompletion(QStringList &word_list)
{
	QStringList match_list;

	if (UTER_DUMPING()) {
		printf("\n=====================================\n");
		printf("Dump input:");
		int count = word_list.length();
		int iloop;
		for (iloop = 0; iloop < count; iloop++) {
			QString word = word_list[iloop];
			printf(">%s<", word.toStdString().c_str());
		}
		printf("\n");
	}

	/*find_obj with new reference*/
	PyObject *find_obj = PyImport_ImportModule("__main__");
	if (find_obj == NULL) {
		return match_list;
	}

	/*foreach all input word*/
	int count = word_list.length();
	int iloop;
	for (iloop = 0; iloop < count; iloop++) {
		QString word = word_list[iloop];

		if (UTER_DUMPING()) {
			printf("Find word \"%s\" in object with dir():", word.toStdString().c_str());
			PyObject *key_list = PyObject_Dir(find_obj);
			if (key_list != NULL) {
				Py_ssize_t len = PyList_Size(key_list);
				Py_ssize_t jloop;
				for (jloop = 0; jloop < len; jloop++) {
					PyObject *key_obj = PyList_GetItem(key_list, jloop);
					printf(">%s<", PyString_AsString(key_obj));
				}
				Py_DECREF(key_list);
			}
			printf("\n");
		}

		if (iloop + 1 == count) {
			/*Object self*/
			if (word == "") {
				QStringList desc_list = UterPython_ObjDesc("", find_obj);
				match_list.append(desc_list);
			}

			/*The last word, partial match, get attribute list with new reference, same as run dir()*/
			PyObject *key_list = PyObject_Dir(find_obj);
			if (key_list != NULL) {
				Py_ssize_t len = PyList_Size(key_list);
				Py_ssize_t jloop;
				for (jloop = 0; jloop < len; jloop++) {
					PyObject *key_obj = PyList_GetItem(key_list, jloop);

					QString key_str = PyString_AsString(key_obj);
					if (key_str.indexOf(word) == 0) {
						/*partial matched on left*/
						PyObject *attr_obj = PyObject_GetAttrString(find_obj, key_str.toStdString().c_str());
						if (attr_obj != NULL) {
							QStringList desc_list = UterPython_ObjDesc(key_str, attr_obj);
							match_list.append(desc_list);
							Py_DECREF(attr_obj);
						}
					}
				}
				Py_DECREF(key_list);
			}
			Py_DECREF(find_obj);
			find_obj = NULL;
			break;	/*no use*/
		} else {
			/*Middle word, full match, get attribute direct(new_obj with new reference)*/
			PyObject *new_obj = PyObject_GetAttrString(find_obj, word.toStdString().c_str());

			Py_DECREF(find_obj);
			find_obj = NULL;

			if (new_obj == NULL) {
				/*Not match*/
				break;
			}
			find_obj = new_obj;
		}
	}

	if (UTER_DUMPING()) {
		printf("Dump result:");
		QStringList::iterator iter;
		for (iter = match_list.begin(); iter != match_list.end(); iter++) {
			QString xxx = *iter;
			printf(">%s<", xxx.toStdString().c_str());
		}
		printf("\n");
	}

	/*sort by name*/
	QStringList list_bracket;
	QStringList list_member;
	QStringList list_underline;
	QStringList list_double_underline;
	QStringList::iterator iter;
	for (iter = match_list.begin(); iter != match_list.end(); iter++) {
		QString item = *iter;
		if (item.left(1) == "(") {
			list_bracket << item;
			continue;
		}
		if (item.left(2) == "__") {
			list_double_underline << item;
			continue;
		}
		if (item.left(1) == "_") {
			list_underline << item;
			continue;
		}
		list_member << item;
	}
	qSort(list_bracket);
	qSort(list_member);
	qSort(list_underline);
	qSort(list_double_underline);

	QStringList return_list;
	return_list << list_bracket;
	return_list << list_member;
	return_list << list_underline;
	return_list << list_double_underline;

	return return_list;
}
