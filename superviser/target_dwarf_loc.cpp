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

/*Load location from Dwarf_Loc*/
int DwfLoc::loadFromLoc(Dwarf_Debug dbg, Dwarf_Loc *loc)
{
	UTER_ASSERT(dbg != NULL);
	UTER_ASSERT(loc != NULL);

    if (loc->lr_atom == DW_OP_addr) {
    	/*Location is address*/
    	m_loc_type = LOC_ADDR;
    	m_addr = loc->lr_number;
    } else if (loc->lr_atom >= DW_OP_reg0 && loc->lr_atom <=DW_OP_reg31) {
    	/*Location is value in register*/
    	m_loc_type = LOC_REG;
    	m_reg_num = loc->lr_atom - DW_OP_reg0;
    } else if (loc->lr_atom >= DW_OP_breg0 && loc->lr_atom <= DW_OP_breg31) {
    	/*Location is value in register with offset*/
    	m_loc_type = LOC_REG_OFF;
    	m_reg_num = loc->lr_atom - DW_OP_breg0;
    	m_offset = loc->lr_number;
    } else if (loc->lr_atom == DW_OP_fbreg) {
    	/*Location is CFA value with offset*/
    	m_loc_type = LOC_CFA_OFF;
    	m_offset = loc->lr_number;
    } else {
    	m_loc_type = LOC_NO_LOC;
    	return -1;
    }

    return 0;
}

/*Load location from Dwarf_Attribute*/
int DwfLoc::loadFromAttr(Dwarf_Debug dbg, Dwarf_Attribute attr)
{
	UTER_ASSERT(dbg != NULL);
    if (attr == NULL) {
    	/*No location*/
    	m_loc_type = LOC_NO_LOC;
    	return 0;
    }

    int result;
    Dwarf_Error error;

	Dwarf_Half form;
	result = dwarf_whatform(attr, &form, &error);
	if(result == DW_DLV_ERROR) {
		UTER_LOG("Get form code from location attribute failed, description %s.", dwarf_errmsg(error));
		dwarf_dealloc(dbg, error, DW_DLA_ERROR);
		m_loc_type = LOC_NO_LOC;
		return -1;
	}
	const char *form_name = "NULL";
	dwarf_get_FORM_name(form, &form_name);
	if (! (form == DW_FORM_exprloc || form == DW_FORM_sec_offset)) {
		UTER_LOG("Form code %s:%d of location attribute unexpected.", form_name, form);
		m_loc_type = LOC_NO_LOC;
		return -1;
	}

    Dwarf_Locdesc *llbuf;
    Dwarf_Signed lcnt;
    result = dwarf_loclist(attr, &llbuf, &lcnt, &error);
    if (result == DW_DLV_ERROR) {
		UTER_LOG("Get location list from location attribute failed, description %s.", dwarf_errmsg(error));
		dwarf_dealloc(dbg, error, DW_DLA_ERROR);
    	m_loc_type = LOC_NO_LOC;
    	return -1;
    }

    /*Only one location in list*/
    UTER_ASSERT(lcnt == 1);
    Dwarf_Loc *loc = llbuf->ld_s;
    UTER_ASSERT(loc != NULL);

    result = loadFromLoc(dbg, loc);

    dwarf_dealloc(dbg, loc, DW_DLA_LOC_BLOCK);
    dwarf_dealloc(dbg, llbuf, DW_DLA_LOCDESC);

    return result;
}

DwfRegTable *DwfFrameFde::getRegTabByAddrIn(unsigned long long addr)
{
	if (! m_loaded) {
		loadRegTab();
		m_loaded = true;
	}

	QList<DwfRegTable*>::iterator iter;
	for (iter = m_regtab_list.begin(); iter != m_regtab_list.end(); iter++) {
		DwfRegTable *regtab = *iter;
		if (regtab->m_addr_range.check(addr)) {
			break;
		}
	}
	if (iter == m_regtab_list.end()) {
		return NULL;
	} else {
		return *iter;
	}
}

/*Item in register table with register number*/
DwfRegTable::Item *DwfFrameFde::makeRegItem(unsigned int regidx, Dwarf_Regtable_Entry3_s *entry)
{
	UTER_ASSERT(entry != NULL);
	int result;
	Dwarf_Error error;

	switch(entry->dw_value_type)
	{
	case DW_EXPR_OFFSET:			/*Register express by other register*/
	case DW_EXPR_VAL_OFFSET:		/*Register express by other register with offset*/
	{
		if (entry->dw_regnum == DwfLoc::REG_UNDEF_VAL) {
			return NULL;
		}
		if (entry->dw_offset_relevant == 0) {
			/*eg: <off r6=r2 >*/
			DwfRegTable::Item *dwfentry = new DwfRegTable::Item(regidx);
			dwfentry->m_offset = 0;
			dwfentry->m_reg_num = entry->dw_regnum;
			return dwfentry;
		} else {
			/*eg: <off cfa=04(r4) >*/
			DwfRegTable::Item *dwfentry = new DwfRegTable::Item(regidx);
			dwfentry->m_offset = entry->dw_offset_or_block_len;
			dwfentry->m_reg_num = entry->dw_regnum;
			return dwfentry;
		}
		/*break;*/
	}
	case DW_EXPR_EXPRESSION:		/*Register express by expression*/
	case DW_EXPR_VAL_EXPRESSION:	/**/
	{
		/*eg: <expr cfa=expr-block-len=11>*/
	    Dwarf_Locdesc *llbuf;
	    Dwarf_Signed listlen;
		result = dwarf_loclist_from_expr(m_dbg,
				           entry->dw_block_ptr, entry->dw_offset_or_block_len,
			               &llbuf, &listlen, &error);
		if (result == DW_DLV_ERROR) {
			UTER_LOG("Get location for register failed, description %s.", dwarf_errmsg(error));
			dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
			return NULL;
        }

		DwfRegTable::Item *dwfentry = new DwfRegTable::Item(regidx);
		dwfentry->m_isexpress = true;
		dwfentry->m_express_len = entry->dw_offset_or_block_len;
		dwfentry->m_express_loc.loadFromLoc(m_dbg, llbuf->ld_s);

		dwarf_dealloc(m_dbg, llbuf->ld_s, DW_DLA_LOC_BLOCK);
		dwarf_dealloc(m_dbg, llbuf, DW_DLA_LOCDESC);
		return dwfentry;
		/*break;*/
	}
	default:
	{
		return NULL;
	}
	}

	return NULL;
}

/*Load register table from fde*/
int DwfFrameFde::loadRegTab()
{
	int result;
	Dwarf_Error error;

	/*Get register table information to reg_table disposable.*/
	Dwarf_Regtable3 reg_table;
	/*Set table memory and size to reg_table*/
	Dwarf_Regtable_Entry3_s rt3_rules[DwfLoc::REG_TABLE_SIZE + 1];
	reg_table.rt3_rules = rt3_rules;
	reg_table.rt3_reg_table_size = DwfLoc::REG_TABLE_SIZE;

	DwfRegTable *last_dwftab = NULL;
    Dwarf_Addr addrloop = m_addrange.m_begin;
	for (/*none*/; /*none*/; addrloop++) {
		if (addrloop >= m_addrange.m_begin + m_addrange.m_offset) {
			/*Out of function*/
	        if (last_dwftab != NULL) {
	        	/*Adjust last table range*/
	        	last_dwftab->m_addr_range.m_offset
				        = m_addrange.m_begin + m_addrange.m_offset - last_dwftab->m_addr_range.m_begin;
	        }
			break;
		}

		/*Get register table at current address*/
		Dwarf_Addr row_pc = 0;
		result = dwarf_get_fde_info_for_all_regs3(m_fde, addrloop, &reg_table, &row_pc, &error);
        if (result == DW_DLV_NO_ENTRY) {
            continue;
        } else if (result == DW_DLV_ERROR) {
			UTER_LOG("Get register table on address, description %s.", dwarf_errmsg(error));
			dwarf_dealloc(m_dbg, error, DW_DLA_ERROR);
			return -1;
        }
        if (row_pc != addrloop) {
        	/*Ignore is not at start*/
        	continue;
        }

        /*Save reg_table to DwfRegTable*/
        DwfRegTable *dwftab = new DwfRegTable();
        dwftab->m_addr_range.m_begin = row_pc;
        m_regtab_list.push_back(dwftab);

        if (last_dwftab != NULL) {
        	/*End of the last table*/
        	last_dwftab->m_addr_range.m_offset = row_pc - last_dwftab->m_addr_range.m_begin;
        }
        last_dwftab = dwftab;

        Dwarf_Regtable_Entry3_s *entry = &(reg_table.rt3_cfa_rule);
        DwfRegTable::Item *dwfentry = makeRegItem(DwfLoc::REG_CFA_VAL, entry);
        if (dwfentry != NULL) {
        	dwftab->m_regs.insert(dwfentry->m_reg_index, dwfentry);
        }
        int jloop;
        for (jloop = 0; jloop < reg_table.rt3_reg_table_size; jloop++) {
        	DwfRegTable::Item *dwfentry = makeRegItem(jloop, reg_table.rt3_rules + jloop);
        	if (dwfentry != NULL) {
        		dwftab->m_regs.insert(dwfentry->m_reg_index, dwfentry);
        	}
        }
	}

	return 0;
}

int dwf_init_dbg_for_loc(Dwarf_Debug dbg)
{
    dwarf_set_frame_cfa_value(dbg, DwfLoc::REG_CFA_VAL);
    dwarf_set_frame_same_value(dbg, DwfLoc::REG_SAME_VAL);
    dwarf_set_frame_undefined_value(dbg, DwfLoc::REG_UNDEF_VAL);
    dwarf_set_frame_rule_initial_value(dbg, DwfLoc::REG_UNDEF_VAL);
    dwarf_set_frame_rule_table_size(dbg, DwfLoc::REG_TABLE_SIZE);
	return 0;
}
