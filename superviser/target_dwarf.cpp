/******************************************************************************
 * Copyright(C),2015-2015, Yaozhongkai
 * FileName:
 * Author:Yaozhongkai
 * Version:
 * Date:
 * Description: 用于解析debug信息
 * Others:
 * Function List:
 * History:
 * ****************************************************************************/

#include <uter_public.h>
#include <target.hxx>

static int uter_libdwarf_flag = UTER_LIBDWARF_FLAG;

DwfItem *DwfCu::itemGetByOff(unsigned long long offset)
{
	QMap<unsigned long long, DwfItem*>::iterator iter;
	iter = m_elements.find(offset);
	if (iter == m_elements.end()) {
		return NULL;
	}
	UTER_ASSERT(iter.value() != NULL);
	return iter.value();
}

int DwfCu::itemInsert(DwfItem *element)
{
	UTER_ASSERT(element != NULL);
	QMap<unsigned long long, DwfItem*>::iterator iter;
	iter = m_elements.find(element->m_offset);
	if (iter != m_elements.end()) {
		UTER_ASSERT(0);
		return -1;
	}
	m_elements.insert(element->m_offset, element);
	return 0;
}

int DwfCu::itemRemove(DwfItem *element)
{
	UTER_ASSERT(element != NULL);
	QMap<unsigned long long, DwfItem*>::iterator iter;
	iter = m_elements.find(element->m_offset);
	if (iter == m_elements.end()) {
		return -1;
	}
	m_elements.erase(iter);
	return 0;
}

DwfFun *DwfCu::getFunByAddr(unsigned long long addr)
{
	QList<DwfFun*>::iterator iter;
	for (iter = m_funs.begin(); iter != m_funs.end(); iter++) {
		DwfFun *fun = *iter;
		if (fun->m_add_range.m_begin == addr) {
			return fun;
		}
	}
	return NULL;
}

DwfFun *DwfCu::getFunByAddrIn(unsigned long long addr)
{
	QList<DwfFun*>::iterator iter;
	for (iter = m_funs.begin(); iter != m_funs.end(); iter++) {
		DwfFun *fun = *iter;
		if (fun->m_add_range.check(addr)) {
			return fun;
		}
	}
	return NULL;
}

/*Set variable information in block*/
int DwfCu::getLocalVarsInBlock(unsigned long long addr, Dwarf_Die blockdie, DwfLocalVars *varset)
{
	int result;
	Dwarf_Error error;
	Dwarf_Attribute attr;

	Dwarf_Off offset = getDieOff(blockdie);
	if (offset == DWF_INVALID_OFF) {
		return -1;
	}

	bool final_block = true;

	Dwarf_Die child_die;
	result = dwarf_child(blockdie, &child_die, &error);
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

		Dwarf_Off offset = getDieOff(child_die);
		if (offset == DWF_INVALID_OFF) {
	    	UTER_LOG("Get offset of child in function block failed.");
			goto break_loop;
		}

    	Dwarf_Half tag;
    	result = dwarf_tag(child_die, &tag, &error);
        if (result == DW_DLV_ERROR) {
	    	UTER_LOG("Get tag of child die :0x%016llx in function block failed, description %s.",
	    				offset, dwarf_errmsg(error));
        	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
        	goto break_loop;
        }

        switch(tag)
        {
        case DW_TAG_formal_parameter:
        case DW_TAG_variable:
        {
        	QMap<QString, DwfLocalVars::Item*>::iterator iter;
        	DwfLocalVars::Item *var = new DwfLocalVars::Item();

        	var->m_name = getDieName(child_die);
        	if (var->m_name == "") {
    	    	UTER_LOG("Get name of parameter or variable die :0x%016llx failed.", offset);
    	    	goto del_var_break_loop;
        	}
        	var->m_fileno = getDieFileNo(child_die);
            if (var->m_fileno == DWF_INVALID_FILELINE) {
            	UTER_LOG("Get file number of parameter or variable die :0x%016llx failed.", offset);
            	goto del_var_break_loop;
            }
        	var->m_lineno = getDieLineNo(child_die);
            if (var->m_lineno == DWF_INVALID_FILELINE) {
            	UTER_LOG("Get line number of parameter or variable die :0x%016llx failed.", offset);
            	goto del_var_break_loop;
            }

            Dwarf_Off type_off;
            type_off = getDieTypeOn(child_die);
        	if(type_off == DWF_INVALID_OFF) {
        		UTER_LOG("Get type offset of parameter or variable die :0x%016llx failed.", offset);
        		goto del_var_break_loop;
        	}
        	var->m_type = getTypeOnOff(type_off);
        	if (var->m_type == NULL) {
        		UTER_LOG("Get type by offset 0x%016llx of parameter or variable die :0x%016llx failed.", type_off, offset);
        		goto del_var_break_loop;
        	}

        	/*DW_AT_location     DW_FORM_exprloc*/
            result = dwarf_attr(child_die, DW_AT_location, &attr, &error);
            if(result == DW_DLV_NO_ENTRY) {
                //UTER_LOG("No location attribute.");
                var->m_loc.loadFromAttr(m_dbg, NULL);
            } else if(result == DW_DLV_ERROR) {
            	UTER_LOG("Get location of parameter or variable die :0x%016llx failed, description %s.", offset, dwarf_errmsg(error));
            	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
            	goto del_var_break_loop;
            } else {
            	result = var->m_loc.loadFromAttr(m_dbg, attr);
            	dwarf_dealloc(m_dbg, attr, DW_DLA_ATTR);
            	if (result < 0) {
            		UTER_LOG("Load location of parameter or variable die :0x%016llx failed.", offset);
            		goto del_var_break_loop;
            	}
            }

            /*Overwrite variable on outside block*/
            iter = varset->m_var_map.find(var->m_name);
            if (iter != varset->m_var_map.end()) {
            	DwfLocalVars::Item *var = iter.value();
            	delete var;
            	varset->m_var_map.erase(iter);
            }
            varset->m_var_map.insert(var->m_name, var);
            break;
del_var_break_loop:
			delete var;
			goto break_loop;
            //break;
        }
        case DW_TAG_lexical_block:
        {
        	Dwarf_Addr lowpc = getDieLowPc(child_die);
        	if (lowpc == DWF_INVALID_ADDR) {
        		UTER_LOG("Get lowpc of function block die :0x%016llx failed.", offset);
        		goto break_loop;
        	}
        	Dwarf_Unsigned highpc = getDieHighPc(child_die);
        	if (highpc == DWF_INVALID_ADDR) {
        		UTER_LOG("Get highpc of function block die :0x%016llx failed.", offset);
        		goto break_loop;
        	}

            if (addr >= lowpc && addr < lowpc + highpc) {
            	/*Address in the block, into block*/
            	final_block = false;
            	varset->excludeClear();

            	varset->m_range.set(lowpc, highpc);
            	result = getLocalVarsInBlock(addr, child_die, varset);
            	if (result < 0) {
            		UTER_LOG("Update sub function block die :0x%016llx failed.", offset);
            		goto break_loop;
            	}
            } else if (final_block) {
            	varset->excludeAdd(lowpc, highpc);
            }

        	break;
        }
        default:
        {
        	break;
        }
        }

continue_loop:
		Dwarf_Die next_child_die;
 		result = dwarf_siblingof(m_dbg, child_die, &next_child_die, &error);
    	dwarf_dealloc(m_dbg, child_die, DW_DLA_DIE);
    	child_die = next_child_die;
        continue;
break_loop:
		dwarf_dealloc(m_dbg, child_die, DW_DLA_DIE);
        goto error_ret;
	}
	return 0;
error_ret:
	return -1;
}

/*Get all variable location on address*/
DwfLocalVars *DwfCu::getLocalVars(unsigned long long addr)
{
	int result;
	Dwarf_Error error;

	/*Get function first*/
	DwfFun *fun = getFunByAddrIn(addr);
	if (fun == NULL) {
		return NULL;
	}

	/*Get function die by offset*/
	Dwarf_Die fundie = NULL;
	result = dwarf_offdie_b(m_dbg, fun->m_offset, 1, &fundie, &error);
	if (result == DW_DLV_NO_ENTRY) {
		UTER_LOG("No function die on offset 0x%016llx.", fun->m_offset);
		return NULL;
	} else if (result == DW_DLV_ERROR) {
    	UTER_LOG("Get function die on offset 0x%016llx failed, description %s.",
    				fun->m_offset, dwarf_errmsg(error));
    	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
    	return NULL;
	}
	/*fundie need free*/

	DwfLocalVars *localvars = new DwfLocalVars();

	localvars->m_range.m_begin = getDieLowPc(fundie);
	if (localvars->m_range.m_begin == DWF_INVALID_ADDR) {
    	UTER_LOG("Get lowpc of function die :0x%016llx failed, description %s.",
    				fun->m_offset, dwarf_errmsg(error));
		goto error_ret;
	}
	localvars->m_range.m_offset = getDieHighPc(fundie);
	if (localvars->m_range.m_offset == DWF_INVALID_ADDR) {
    	UTER_LOG("Get highpc of function die :0x%016llx failed, description %s.",
    				fun->m_offset, dwarf_errmsg(error));
		goto error_ret;
	}

	/*Function is a block too*/
	result = getLocalVarsInBlock(addr, fundie, localvars);
	if (result < 0) {
    	UTER_LOG("Get variable information of block in function die :0x%016llx failed.", fun->m_offset);
		goto error_ret;
	}

	dwarf_dealloc(m_dbg, fundie, DW_DLA_DIE);
	return localvars;
error_ret:
	dwarf_dealloc(m_dbg, fundie, DW_DLA_DIE);
	delete localvars;
	return NULL;
}

QString DwfCu::getFileNameByNo(unsigned int fileno)
{
	QMap<unsigned int, QString>::iterator iter;
	iter = m_file_no_map.find(fileno);
	if (iter == m_file_no_map.end()) {
		return "";
	}
	return iter.value();
}

int DwfCu::getFileLineNoByAddrIn(unsigned long long addr, unsigned int &fileno, unsigned int &lineno)
{
	DwfLine *lastline = NULL;
	QList<DwfLine*>::iterator iter;
	for (iter = m_dwf_line_list.begin(); iter != m_dwf_line_list.end(); iter++) {
		DwfLine *dwfline = *iter;
		if (addr >= dwfline->m_lineaddr) {
			lastline = dwfline;
		} else {
			break;
		}
	}
	if (lastline == NULL) {
		return -1;
	}
	fileno = lastline->m_fileno;
	lineno = lastline->m_lineno;
	return 0;
}

int DwfCu::getFileLineByAddrIn(unsigned long long addr, QString &filename, unsigned int &lineno)
{
	unsigned int fileno;
	int result = getFileLineNoByAddrIn(addr, fileno, lineno);
	if (result < 0) {
		return -1;
	}

	filename = getFileNameByNo(fileno);
	if (filename == "") {
		return -1;
	}

	return 0;
}

int DwfCu::getFileLineNoByAddr(unsigned long long addr, unsigned int &fileno, unsigned int &lineno)
{
	DwfLine *lastline = NULL;
	QList<DwfLine*>::iterator iter;
	for (iter = m_dwf_line_list.begin(); iter != m_dwf_line_list.end(); iter++) {
		DwfLine *dwfline = *iter;
		if (addr == dwfline->m_lineaddr) {
			fileno = dwfline->m_fileno;
			lineno = dwfline->m_lineno;
			return 0;
		}
	}
	return -1;
}

int DwfCu::parseLines(Dwarf_Die cu_die)
{
	UTER_ASSERT(cu_die != NULL);
	int result;
	Dwarf_Error error;
	unsigned int iloop;

	/*File name information*/
	char **srcfiles;
	Dwarf_Signed cnt;
	result = dwarf_srcfiles(cu_die, &srcfiles, &cnt, &error);
	if(result == DW_DLV_NO_ENTRY) {
		UTER_LOG("No file name information in cu.");
		return -1;
    } else if (result == DW_DLV_ERROR) {
    	UTER_LOG("Get file name information failed, description %s.", dwarf_errmsg(error));
    	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
    	return -1;
    }
	/*Save all file name*/
	for (iloop = 0; iloop < cnt; iloop++) {
		char *filename = srcfiles[iloop];
		unsigned int fileno = iloop + 1;			/*File number begin from 1*/
		QString path = filename;
		dwarf_dealloc(m_dbg, filename, DW_DLA_STRING);

		if (path.left(1) != QDir::separator()) {
			path = this->m_dir + QDir::separator() + filename;
		}
		path = QDir::cleanPath(path);

		m_file_path_map.insert(path, fileno);
		m_file_no_map.insert(fileno, path);
	}
	dwarf_dealloc(m_dbg, srcfiles, DW_DLA_LIST);

	/*File line information*/
	Dwarf_Line *linebuf;
	result = dwarf_srclines(cu_die, &linebuf, &cnt, &error);
	if(result == DW_DLV_NO_ENTRY) {
		UTER_LOG("No line number information in cu.");
		return -1;
	} else if(result == DW_DLV_ERROR) {
		UTER_LOG("Get line number information failed, description %s.", dwarf_errmsg(error));
    	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
    	return -1;
    }
	/*Save all line number*/
	for (iloop = 0; iloop < cnt; iloop++) {
		Dwarf_Line line = linebuf[iloop];

		DwfLine *dwfline = new DwfLine;

		/*Line address*/
		Dwarf_Addr ret_lineaddr;
		result = dwarf_lineaddr(line, &ret_lineaddr, &error);
		if(result == DW_DLV_ERROR) {
		   	UTER_LOG("Get debug information error, description %s.", dwarf_errmsg(error));
		   	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
		   	goto break_line_loop;
		}
		dwfline->m_lineaddr = ret_lineaddr;

		/*File number*/
		Dwarf_Unsigned return_no;
		result = dwarf_line_srcfileno(line, &return_no, &error);
		if(result == DW_DLV_ERROR) {
		   	UTER_LOG("Get debug information error, description %s.", dwarf_errmsg(error));
		   	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
		   	goto break_line_loop;;
		}
		dwfline->m_fileno = return_no;

		/*Line number*/
		result = dwarf_lineno(line, &return_no, &error);
		if(result == DW_DLV_ERROR) {
		   	UTER_LOG("Get debug information error, description %s.", dwarf_errmsg(error));
		   	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
		   	goto break_line_loop;
		}
		dwfline->m_lineno = return_no;

		/*Line off*/
		Dwarf_Signed ret_lineoff;
		result = dwarf_lineoff(line, &ret_lineoff, &error);
		if(result == DW_DLV_ERROR) {
		   	UTER_LOG("Get debug information error, description %s.", dwarf_errmsg(error));
		   	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
		   	goto break_line_loop;
		}
		dwfline->m_offno = ret_lineoff;

		/*Is begin*/
		Dwarf_Bool return_bool;
		result = dwarf_linebeginstatement(line, &return_bool, &error);
		if(result == DW_DLV_ERROR) {
		   	UTER_LOG("Get debug information error, description %s.", dwarf_errmsg(error));
		   	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
		   	goto break_line_loop;
		}
		dwfline->m_isbegin = return_bool;

		/*Is end*/
		result = dwarf_lineendsequence(line, &return_bool, &error);
		if(result == DW_DLV_ERROR) {
		   	UTER_LOG("Get debug information error, description %s.", dwarf_errmsg(error));
		   	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
		   	goto break_line_loop;
		}
		dwfline->m_isend = return_bool;

		/*block*/
		result = dwarf_lineblock(line, &return_bool, &error);
		if(result == DW_DLV_ERROR) {
		   	UTER_LOG("Get debug information error, description %s.", dwarf_errmsg(error));
		   	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
		   	goto break_line_loop;
		}
		dwfline->m_isblock = return_bool;

		/**/
		result = dwarf_line_is_addr_set(line, &return_bool, &error);
		if(result == DW_DLV_ERROR) {
		   	UTER_LOG("Get debug information error, description %s.", dwarf_errmsg(error));
		   	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
		   	goto break_line_loop;
		}
		dwfline->m_isaddr_set = return_bool;

		m_dwf_line_list.push_back(dwfline);
		m_dwf_line_map.insert(dwfline->m_lineaddr, dwfline);
		continue;
break_line_loop:
		delete dwfline;
		goto break_lines;
	}
	dwarf_srclines_dealloc(m_dbg, linebuf, cnt);

	if (UTER_DUMPING()) {
		/*Dump file*/
		QMap<unsigned int, QString>::Iterator iter;
		for (iter = m_file_no_map.begin(); iter != m_file_no_map.end(); iter++) {
			unsigned int id = iter.key();
			QString path = iter.value();
			UTER_DUMP("Dump filename in cu, %d:%s.", id, path.toStdString().c_str());
		}
	}
	return 0;

break_lines:
	dwarf_srclines_dealloc(m_dbg, linebuf, cnt);
	return -1;
}

int DwfCu::parse(Dwarf_Die cu_die)
{
	int result;
	Dwarf_Error error;

    result = getDieStrAttr(cu_die, DW_AT_name, m_name);
    if (result < 0) {
    	UTER_LOG("Get name of cu failed.");
    	return -1;
    }
    result = getDieStrAttr(cu_die, DW_AT_comp_dir, m_dir);
    if (result < 0) {
    	UTER_LOG("Get dir of cu failed.");
    	return -1;
    }
    m_path = m_dir + QDir::separator() + m_name;

	m_offset = getDieOff(cu_die);
	if (m_offset == DWF_INVALID_OFF) {
		UTER_LOG("Get offset of cu %s failed.", m_path.toStdString().c_str());
		return -1;
	}

	result = getDieStrAttr(cu_die, DW_AT_producer, m_producer);
	if (result < 0) {
		UTER_LOG("Get producer of cu %s failed.", m_path.toStdString().c_str());
		return -1;
	}

	Dwarf_Unsigned language;
    result = getDieUdataAttr(cu_die, DW_AT_language, language);
    if (result < 0) {
    	UTER_LOG("Get language of cu %s failed.", m_path.toStdString().c_str());
    	return -1;
    }
    switch(language)
    {
    case DW_LANG_C89:
    {
    	m_lang = LANGUAGE_C89;
    	break;
    }
    case DW_LANG_C99:
    {
    	m_lang = LANGUAGE_C99;
    	break;
    }
    case DW_LANG_C:
    {
    	m_lang = LANGUAGE_C;
    	break;
    }
    case DW_LANG_C_plus_plus:
    {
    	m_lang = LANGUAGE_CPP;
    	break;
    }
    default :
    {
    	UTER_LOG("Get language code %lld of cu %s failed.", language, m_path.toStdString().c_str());
    	return -1;
    }
    }

    if (getDieHasAttr(cu_die, DW_AT_low_pc)) {
        m_addr_range.m_begin = getDieLowPc(cu_die);
        if (m_addr_range.m_begin == DWF_INVALID_ADDR) {
        	UTER_LOG("Get lowpc of cu %s failed.", m_path.toStdString().c_str());
        	m_addr_range.m_begin = 0;
        }
    } else {
    	m_addr_range.m_begin = 0;
    }

    if (getDieHasAttr(cu_die, DW_AT_high_pc)) {
        m_addr_range.m_offset = getDieHighPc(cu_die);
        if (m_addr_range.m_offset == DWF_INVALID_ADDR) {
        	UTER_LOG("Get highpc of cu %s failed.", m_path.toStdString().c_str());
        	m_addr_range.m_offset = 0;
        }
    } else {
    	m_addr_range.m_offset = 0;
    }

    UTER_DEBUG("Parse cu %s on offset 0x%016llx, address 0x%016llx and 0x%016llx, "
    			"language 0x%016llx with producer \"%s\".",
    			m_path.toStdString().c_str(), m_offset, m_addr_range.m_begin, m_addr_range.m_offset,
				language, m_producer.toStdString().c_str());
	result = parseLines(cu_die);
	if (result < 0) {
		UTER_LOG("Parse line information of cu %s failed.", m_path.toStdString().c_str());
		return -1;
	}

    result = getDieRefAttr(cu_die, DW_AT_stmt_list, m_in_line_off);
    if (result < 0) {
    	UTER_LOG("Get line off of cu %s failed.", m_path.toStdString().c_str());
    	return -1;
    }

    /*TODO:*/
#if 0
    result = getDieRefAttr(cu_die, DW_AT_GNU_macros, m_in_macro_off);
    if (result < 0) {
    	goto error_ret;
    }
#endif

	Dwarf_Die top_die;
	result = dwarf_child(cu_die, &top_die, &error);
	while (1) {
		if (result == DW_DLV_NO_ENTRY) {
			/*End of child foreach*/
			break;
		} else if (result == DW_DLV_ERROR) {
	    	UTER_LOG("Get first or next child in cu %s failed, description %s.",
	    				m_path.toStdString().c_str(), dwarf_errmsg(error));
	    	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
			break;
		}

		Dwarf_Off offset = getDieOff(top_die);
		if (offset == DWF_INVALID_OFF) {
	    	UTER_LOG("Get offset of child die :0x%016llx in cu %s failed.",
	    				offset, m_path.toStdString().c_str());
			goto break_loop;
		}

    	Dwarf_Half tag;
    	result = dwarf_tag(top_die, &tag, &error);
        if (result == DW_DLV_ERROR) {
	    	UTER_LOG("Get tag of child die :0x%016llx in cu %s failed, description %s.",
	    			offset, m_path.toStdString().c_str(), dwarf_errmsg(error));
        	dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
        	goto break_loop;
        }
        switch(tag)
        {
        case DW_TAG_subprogram:
        {
        	//if (getDieHasAttr(top_die, DW_AT_abstract_origin)) {
        	//	/*TODO:*/
        	//	break;
        	//}
        	DwfFun *fun = parseFun(top_die);
        	if (fun == NULL) {
        		UTER_LOG("Parse function die :0x%016llx failed.", offset);
        		goto break_loop;
        	}
        	m_funs.push_back(fun);
        	break;
        }
        case DW_TAG_variable:
        {
        	if (getDieIsDeclare(top_die)) {
        		break;
        	}
        	DwfVar *var = parseVar(top_die);
        	if (var == NULL) {
        		UTER_LOG("Parse variable die :0x%016llx failed.", offset);
        		goto break_loop;
        	}
       		m_vars.push_back(var);
        	break;
        }
        case DW_TAG_structure_type:
        case DW_TAG_union_type:
        case DW_TAG_enumeration_type:
        case DW_TAG_typedef:
        {
        	DwfType *type = getTypeOnOff(offset);
        	if (type == NULL) {
        		UTER_LOG("Parse data type die :0x%016llx failed.", offset);
        		goto break_loop;
        	}
        	/*Save data type*/
        	switch(type->m_data_type)
        	{
        	case DwfType::TYPE_EN:
        	{
            	m_enums.push_back(type);
        		break;
        	}
        	case DwfType::TYPE_ST:
        	{
        		m_structs.push_back(type);
        		break;
        	}
        	case DwfType::TYPE_UN:
        	{
        		m_unions.push_back(type);
        		break;
        	}
        	case DwfType::TYPE_TYPEDEF:
        	{
        		m_typedefs.push_back(type);
        		break;
        	}
        	default:
        	{
        		break;
        	}
        	}
        	break;
        }
        default:
        {
        	break;
        }
        }

		Dwarf_Die next_top_die;
        result = dwarf_siblingof(m_dbg, top_die, &next_top_die, &error);
    	dwarf_dealloc(m_dbg, top_die, DW_DLA_DIE);
    	top_die = next_top_die;
        continue;
break_loop:
		dwarf_dealloc(m_dbg, top_die, DW_DLA_DIE);
        goto error_ret;
	}

	return 0;
error_ret:
	return -1;
}

int TargetMod::dwarfLoad()
{
	UTER_ASSERT(m_dwarf_fd == -1);
	UTER_ASSERT(m_dwarf_dbg == NULL);
	int result;
	Dwarf_Error error;

	/*Open handle*/
	m_dwarf_fd = open(m_local_path.toStdString().c_str(), O_RDONLY);
	if (m_dwarf_fd < 0) {
		UTER_LOG("Open module file %s failed, errno %d.", m_local_path.toStdString().c_str(), errno);
		return -1;
	}

	result = dwarf_init(m_dwarf_fd, DW_DLC_READ, NULL, NULL, &m_dwarf_dbg, &error);
	if (result == DW_DLV_NO_ENTRY) {
		UTER_LOG("No debug information in module file %s.", m_local_path.toStdString().c_str());
		goto error_close_fd;
	} else if (result == DW_DLV_ERROR) {
		UTER_LOG("Get debug information failed, description %s.", dwarf_errmsg(error));
		free(error);	/*Not dwarf_dealloc here*/
		goto error_close_fd;
	}

	/*Initialization for loccation system*/
	dwf_init_dbg_for_loc(m_dwarf_dbg);

	/*Parse ef frame information*/
    result = dwarf_get_fde_list_eh(m_dwarf_dbg, &m_dwarf_cie_data, &m_dwarf_cie_count,
    		                              &m_dwarf_fde_data, &m_dwarf_fde_count, &error);
    if (result == DW_DLV_NO_ENTRY) {
    	UTER_LOG("No ef frame information in module file %s.", m_local_path.toStdString().c_str());
    	goto error_close_dbg;
    } else if (result == DW_DLV_ERROR) {
		UTER_LOG("Get ef frame information failed, description %s.", dwarf_errmsg(error));
		dwarf_dealloc(m_dwarf_dbg, error, DW_DLA_ERROR);
		goto error_close_dbg;
    }

    /*goto error_return; when error*/

	int iloop;
	for (iloop = 0; iloop < m_dwarf_fde_count; iloop++) {
		Dwarf_Fde fde = m_dwarf_fde_data[iloop];

		DwfFrameCie *dwfcie;
		Dwarf_Cie cie;
		result = dwarf_get_cie_of_fde(fde, &cie, &error);
		if (result == DW_DLV_ERROR) {
			UTER_LOG("Get cie for fde failed, description %s.", dwarf_errmsg(error));
			dwarf_dealloc(m_dwarf_dbg, error, DW_DLA_ERROR);
			goto error_return;
		}
		Dwarf_Signed cieindex;
		result = dwarf_get_cie_index(cie, &cieindex, &error);
		if (result == DW_DLV_ERROR) {
			UTER_LOG("Get index of cie failed, description %s.", dwarf_errmsg(error));
			dwarf_dealloc(m_dwarf_dbg, error, DW_DLA_ERROR);
			goto error_return;
		}
		QMap<long long, DwfFrameCie*>::iterator cie_iter;
		cie_iter = m_dwarf_cie_map.find(cieindex);
		if (cie_iter == m_dwarf_cie_map.end()) {
			DwfFrameCie *dwfcie = new DwfFrameCie();
			dwfcie->m_cie = cie;
			dwfcie->m_index = cieindex;
			m_dwarf_cie_map.insert(dwfcie->m_index, dwfcie);
		} else {
			dwfcie = cie_iter.value();
		}

		Dwarf_Addr low_pc;
		Dwarf_Unsigned func_len;
		Dwarf_Ptr fde_bytes;
		Dwarf_Unsigned fde_byte_len;
		Dwarf_Off cie_offset;
		Dwarf_Signed cie_index;
		Dwarf_Off fde_offset;
		result = dwarf_get_fde_range(fde, &low_pc, &func_len,
				                          &fde_bytes, &fde_byte_len,
									      &cie_offset, &cie_index,
										  &fde_offset, &error);
		if (result == DW_DLV_ERROR) {
			UTER_LOG("Get fde information failed, description %s.", dwarf_errmsg(error));
			dwarf_dealloc(m_dwarf_dbg, error, DW_DLA_ERROR);
			goto error_return;
		}

		/*Save fde information*/
		DwfFrameFde *dwffde = new DwfFrameFde(m_dwarf_dbg, fde, dwfcie, low_pc, func_len);
		m_dwarf_fde_list.push_back(dwffde);
#if 0
		dwffde->load();
		dwffde->m_loaded = true;
#endif
	}

	/*Parse cu information*/
	while(1) {
		Dwarf_Unsigned cu_header_length;
		Dwarf_Half version_stamp;
		Dwarf_Unsigned abbrev_offset;
		Dwarf_Half address_size;
		Dwarf_Unsigned next_cu_header;
		result = dwarf_next_cu_header(m_dwarf_dbg, &cu_header_length, &version_stamp,
									  &abbrev_offset, &address_size, &next_cu_header, &error);
		if (result == DW_DLV_NO_ENTRY) {
			/*No more cu*/
			break;
		} else if (result == DW_DLV_ERROR) {
			UTER_LOG("Get cu information failed, description %s.", dwarf_errmsg(error));
			dwarf_dealloc(m_dwarf_dbg, error, DW_DLA_ERROR);
			goto error_return;
		}

		Dwarf_Die no_die = NULL;
        Dwarf_Die cu_first_die;
        result = dwarf_siblingof(m_dwarf_dbg, no_die, &cu_first_die, &error);
        if (result == DW_DLV_NO_ENTRY) {
        	/*No die in cu*/
			continue;
        } else if (result == DW_DLV_ERROR) {
        	UTER_LOG("Get first die in cu failed, description %s.", dwarf_errmsg(error));
        	dwarf_dealloc(m_dwarf_dbg, error, DW_DLA_ERROR);
        	goto error_return;
        }
        UTER_ASSERT(cu_first_die != NULL);
        /*cu_first_die need free*/

        Dwarf_Half tagval;
        result = dwarf_tag(cu_first_die, &tagval, &error);
        if(result == DW_DLV_ERROR) {
        	UTER_LOG("Get tag for die failed, description %s.", dwarf_errmsg(error));
        	dwarf_dealloc(m_dwarf_dbg, error, DW_DLA_ERROR);
        	goto break_loop;
        }

        if (tagval != DW_TAG_compile_unit) {
        	goto continue_loop;
        }

        DwfCu *cu;
        cu = new DwfCu(m_dwarf_dbg);

        g_ui_main.setWaitCursor();
        result = cu->parse(cu_first_die);
        g_ui_main.restoreCursor();
        if (result < 0) {
        	delete cu;
        	UTER_LOG("Parse compile unit failed.");
        	goto break_loop;
        } else {
        	m_dwarf_cu_map.insert(cu->m_path, cu);
        }

continue_loop:
        dwarf_dealloc(m_dwarf_dbg, cu_first_die, DW_DLA_DIE);
        continue;
break_loop:
		dwarf_dealloc(m_dwarf_dbg, cu_first_die, DW_DLA_DIE);
		goto error_return;
	}

	/*TODO:Macro process*/
	int parse_macro(Dwarf_Debug dbg, TargetMod *modinfo);
	parse_macro(m_dwarf_dbg, this);

	return 0;
finish:
	return 0;
error_return:
	dwarfRelease();
	return -1;
error_close_dbg:
	dwarf_finish(m_dwarf_dbg, NULL);
	m_dwarf_dbg = NULL;
error_close_fd:
	close(m_dwarf_fd);
	m_dwarf_fd = -1;
	return -1;
}

int TargetMod::dwarfRelease()
{
	QMap<QString, DwfCu*>::iterator cu_iter;
	for (cu_iter = m_dwarf_cu_map.begin(); cu_iter != m_dwarf_cu_map.end(); /*none*/) {
		DwfCu *cu = cu_iter.value();
		delete cu;
		m_dwarf_cu_map.erase(cu_iter++);
	}

	QList<DwfFrameFde*>::iterator fde_iter;
	for (fde_iter = m_dwarf_fde_list.begin(); fde_iter != m_dwarf_fde_list.end(); /*none*/) {
		DwfFrameFde *dwffde = *fde_iter;
		m_dwarf_fde_list.erase(fde_iter++);
	}

	QMap<long long, DwfFrameCie*>::iterator cie_iter;
	for (cie_iter = m_dwarf_cie_map.begin(); cie_iter != m_dwarf_cie_map.end(); /*none*/) {
		DwfFrameCie *dwfcie = cie_iter.value();
		m_dwarf_cie_map.erase(cie_iter++);
	}

	if (m_dwarf_dbg != NULL) {
		dwarf_fde_cie_list_dealloc(m_dwarf_dbg,
									m_dwarf_cie_data, m_dwarf_cie_count,
			                        m_dwarf_fde_data, m_dwarf_fde_count);
		m_dwarf_cie_data = NULL;
		m_dwarf_cie_count = 0;
		m_dwarf_fde_data = NULL;
		m_dwarf_fde_count = 0;

		dwarf_finish(m_dwarf_dbg, NULL);
		m_dwarf_dbg = NULL;
	}

	if (m_dwarf_fd != -1) {
		close(m_dwarf_fd);
		m_dwarf_fd = -1;
	}

	return 0;
}

DwfCu *TargetMod::dwarfGetCuByPath(QString path)
{
	QMap<QString, DwfCu*>::iterator iter = m_dwarf_cu_map.find(path);
	if (iter == m_dwarf_cu_map.end()) {
		return NULL;
	}
	return iter.value();
}

DwfCu *TargetMod::dwarfGetCuByAddrIn(unsigned long long addr)
{
	QMap<QString, DwfCu*>::iterator iter;
	for (iter = m_dwarf_cu_map.begin(); iter != m_dwarf_cu_map.end(); iter++) {
		DwfCu *cu = iter.value();
		if (cu->m_addr_range.check(addr)) {
			return cu;
		}
	}
	return NULL;
}

DwfCu *TargetMod::dwarfGetCuByMacroOff(unsigned long long off)
{
	QMap<QString, DwfCu*>::iterator iter;
	for (iter = m_dwarf_cu_map.begin(); iter != m_dwarf_cu_map.end(); iter++) {
		DwfCu *cu = iter.value();
		if (off == cu->m_in_macro_off) {
			return cu;
		}
	}
	return NULL;
}

DwfCu *TargetMod::dwarfGetCuByLineOff(unsigned long long off)
{
	QMap<QString, DwfCu*>::iterator iter;
	for (iter = m_dwarf_cu_map.begin(); iter != m_dwarf_cu_map.end(); iter++) {
		DwfCu *cu = iter.value();
		if (off == cu->m_in_line_off) {
			return cu;
		}
	}
	return NULL;
}

int TargetMod::dwarfInfoLevel()
{
	if (m_dwarf_cu_map.empty()) {
		return 0;
	}

	/*TODO: macro return 2*/
	return 1;
}

DwfFrameFde *TargetMod::dwarfGetFdeByAddrIn(unsigned long long addr)
{
	QList<DwfFrameFde*>::iterator iter;
	for (iter = m_dwarf_fde_list.begin(); iter != m_dwarf_fde_list.end(); iter++) {
		DwfFrameFde *fde = *iter;
		if (fde->m_addrange.check(addr)) {
			break;
		}
	}
	if (iter == m_dwarf_fde_list.end()) {
		return NULL;
	} else {
		return *iter;
	}
}
