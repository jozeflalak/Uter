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

#include <stdarg.h>
#define PACKAGE
#define PACKAGE_VERSION
#include <dis-asm.h>

#include <uter_public.h>
#include <target.hxx>

int dasm_fprintf(void *stream, const char *fmt, ...)
{
	UTER_ASSERT(stream != NULL);
	QString *str_buf = (QString *)stream;

	va_list args;
	va_start(args, fmt);
	QString tmp = QString::vasprintf(fmt, args);
	va_end(args);

	*str_buf += tmp;
	return tmp.length();
}

/*Adjust address on module*/
void address_print(bfd_vma addr, struct disassemble_info *dinfo)
{
	UTER_ASSERT(dinfo != NULL);

	BfdSec *sec = (BfdSec *)(dinfo->application_data);
	addr += sec->m_mod->m_add_mkup;

	/*Old print function*/
	generic_print_address(addr, dinfo);
	return;
}

/*Foreach section by asection*/
void for_each_section(bfd *file, asection *section, void *args)
{
	UTER_ASSERT(file != NULL);
	UTER_ASSERT(section != NULL);
	UTER_ASSERT(args != NULL);

	TargetMod *self = (TargetMod *)args;
	UTER_ASSERT(self->m_bfd_file == file);

	BfdSec *new_sec = new BfdSec();
	new_sec->m_mod = self;
	new_sec->m_section = section;
	new_sec->m_foff = section->filepos;
	new_sec->m_name = bfd_section_name(file, section);
	new_sec->m_size = bfd_section_size(file, section);
	new_sec->m_vma = bfd_section_vma(file, section);
	new_sec->m_lma = bfd_section_lma(file, section);

	if (section->flags & SEC_LOAD) {
		new_sec->m_sec_load = true;
	}

	/*Save code section*/
	if (section->flags & SEC_CODE) {
		new_sec->m_sec_code = true;
		new_sec->m_data = (unsigned char *)malloc(new_sec->m_size);
		if (new_sec->m_data == NULL) {
			new_sec->m_sec_code = false;
		} else {
			bfd_get_section_contents(file, section, new_sec->m_data, 0, new_sec->m_size);
		}
	}

	self->m_bfd_sec_list.push_back(new_sec);
	return;
}

/*Disassemble code*/
BfdAsmSet *BfdSec::dasm(unsigned long long from, unsigned long long to)
{
	UTER_ASSERT(from < to);

	if (! m_sec_code) {
		/*Just flush code section*/
		return NULL;
	}

	unsigned long long begin = from;
	unsigned long long end = to;

	/*Adjust begin and end*/
	if (begin < m_load_at) {
		begin = m_load_at;
	}
	if (end > m_load_at + m_size) {
		end = m_load_at + m_size;
	}

	/*Start in barrier, first instruction as begin*/
	unsigned long long barrier = begin / STARTAT_STEP * STARTAT_STEP;
	QMap<unsigned long long, unsigned long long>::iterator iter;
	iter = m_startat.find(barrier);
	if (iter == m_startat.end()) {
		return NULL;
	}
	begin = iter.value();

	/*Get disassembler*/
	disassembler_ftype disassemble_fn;
	disassemble_fn = disassembler(m_mod->m_bfd_file);

	/*QString as output stream*/
	QString dasm_out_string;

	/*Configure disassemble*/
	struct disassemble_info info;
	init_disassemble_info(&info, NULL, NULL);
	info.fprintf_func = dasm_fprintf;
	info.stream = (void *)&dasm_out_string;
	info.print_address_func = address_print;
	info.application_data = (void *)this;

	info.flavour = bfd_get_flavour(m_mod->m_bfd_file);
	info.arch = bfd_get_arch(m_mod->m_bfd_file);
	info.mach = bfd_get_mach(m_mod->m_bfd_file);
	info.disassembler_options = NULL;
	info.octets_per_byte = bfd_octets_per_byte(m_mod->m_bfd_file);
	info.skip_zeroes = 8;
	info.skip_zeroes_at_end = 3;
	info.disassembler_needs_relocs = 0;

	info.section = m_section;
	info.buffer = m_data;
	info.buffer_vma = m_vma;
	info.buffer_length = m_size;

	BfdAsmSet *asm_range = new BfdAsmSet();

	/*disassemble*/
	unsigned long long offset;
	int octets;
	for (offset = begin; offset < end; offset += octets) {
		dasm_out_string.clear();
		/*Offset address for disassembler, real address subtract make up*/
		octets = (*disassemble_fn)(offset - m_mod->m_add_mkup, &info);
		if (octets <= 0) {
			break;
		}

		/*Save asm in disassemble range*/
		if (offset >= from) {
			BfdAsm *asm_line = new BfdAsm();
			asm_line->m_offset = offset;
			asm_line->m_octets = octets;
			asm_line->m_asm_code = dasm_out_string;
			asm_range->m_bfd_asm_list.push_back(asm_line);
		}
	}

	return asm_range;
}

int BfdSec::flushCode()
{
	if (! m_sec_code) {
		/*Just flush code section*/
		return -1;
	}

	/*Get disassemble*/
	disassembler_ftype disassemble_fn;
	disassemble_fn = disassembler(m_mod->m_bfd_file);

	/*String as output stream*/
	QString dasm_out_string;

	/*Configure disassemble*/
	struct disassemble_info info;
	init_disassemble_info(&info, NULL, NULL);
	info.fprintf_func = dasm_fprintf;
	info.stream = (void *)&dasm_out_string;
	info.print_address_func = address_print;
	info.application_data = (void *)this;

	info.flavour = bfd_get_flavour(m_mod->m_bfd_file);
	info.arch = bfd_get_arch(m_mod->m_bfd_file);
	info.mach = bfd_get_mach(m_mod->m_bfd_file);
	info.disassembler_options = NULL;
	info.octets_per_byte = bfd_octets_per_byte(m_mod->m_bfd_file);
	info.skip_zeroes = 8;
	info.skip_zeroes_at_end = 3;
	info.disassembler_needs_relocs = 0;

	info.section = m_section;
	info.buffer = m_data;
	info.buffer_vma = m_vma;
	info.buffer_length = m_size;

	/*Foreach all instruction, record some instruction start at for dasm begin at*/
	unsigned long long offset;
	int octets;
	for (offset = m_vma; offset < m_vma + m_size; offset += octets) {
		dasm_out_string.clear();
		/*disassemble on instruction*/
		octets = (*disassemble_fn)(offset, &info);
		if (octets <= 0) {
			break;
		}
		/*barrier of instruction*/
		unsigned long long barrier = (m_mod->m_add_mkup + offset) / STARTAT_STEP * STARTAT_STEP;
		/*first instruction in barrier*/
		QMap<unsigned long long, unsigned long long>::iterator iter;
		iter = m_startat.find(barrier);
		if (iter == m_startat.end()) {
			m_startat.insert(barrier, m_mod->m_add_mkup + offset);
		}
	}

	return 0;
}

/*See https://sourceware.org/binutils/docs-2.25/binutils/objdump.html#objdump
 *symbol flag description*/
QString BfdSym::flagsDesc()
{
	QString ret_str;

	if (m_flag & (BSF_LOCAL | BSF_GLOBAL | BSF_GNU_UNIQUE)) {
		if ((m_flag & BSF_LOCAL) && (m_flag & (BSF_GLOBAL | BSF_GNU_UNIQUE))) {
			ret_str += "!";
		} else {
			/*local*/
			if (m_flag & BSF_LOCAL) {
				ret_str += "l";
			} else {
				ret_str += " ";
			}
			/*global*/
			if (m_flag & BSF_GLOBAL) {
				ret_str += "g";
			} else {
				ret_str += " ";
			}
			/*unique*/
			if (m_flag & BSF_GNU_UNIQUE) {
				ret_str += "u";
			} else {
				ret_str += " ";
			}
		}
	}

	/*weak*/
	if (m_flag & BSF_WEAK) {
		ret_str += "w";
	} else {
		ret_str += " ";
	}

	/*constructor*/
	if (m_flag & BSF_CONSTRUCTOR) {
		ret_str += "C";
	} else {
		ret_str += " ";
	}

	/*warning*/
	if (m_flag & BSF_WARNING) {
		ret_str += "W";
	} else {
		ret_str += " ";
	}

	/**/
	if (m_flag & BSF_INDIRECT) {
		ret_str += "I";
	} else {
		ret_str += " ";
	}

	/**/
	if (m_flag & BSF_GNU_INDIRECT_FUNCTION) {
		ret_str += "i";
	} else {
		ret_str += " ";
	}

	/*debug*/
	if (m_flag & BSF_DEBUGGING) {
		ret_str += "d";
	} else {
		ret_str += " ";
	}

	/*dynamic*/
	if (m_flag & BSF_DYNAMIC) {
		ret_str += "D";
	} else {
		ret_str += " ";
	}

	/*function*/
	if (m_flag & BSF_FUNCTION) {
		ret_str += "F";
	} else {
		ret_str += " ";
	}

	/*file*/
	if (m_flag & BSF_FILE) {
		ret_str += "f";
	} else {
		ret_str += " ";
	}

	/*Object*/
	if (m_flag & BSF_OBJECT) {
		ret_str += "O";
	} else {
		ret_str += " ";
	}

	return ret_str;
}

/*See http://www.sourceware.org/binutils/docs-2.20/bfd/index.html*/
int TargetMod::bfdLoad()
{
	int result;
	long longresult;

	QList<BfdSec*>::iterator iter;

	/*Open bfd handle*/
	m_bfd_file = bfd_openr(m_local_path.toStdString().c_str(), NULL);
	if (m_bfd_file == NULL) {
		return -1;
	}

	if (! bfd_check_format(m_bfd_file, bfd_object)) {
		goto error_close;
	}

	/*Foreach all bfd section*/
	bfd_map_over_sections(m_bfd_file, for_each_section, this);

	/*Get debug symbol*/
	longresult = bfd_get_symtab_upper_bound(m_bfd_file);
	if (longresult > 0) {
		asymbol **symbol_table = (asymbol **)malloc(longresult);
		if (symbol_table != NULL) {
			long number_of_symbols = bfd_canonicalize_symtab(m_bfd_file, symbol_table);
			if (number_of_symbols > 0) {
				long iloop;
				for (iloop = 0; iloop < number_of_symbols; iloop++) {
					asymbol *symbol = symbol_table[iloop];
					if (symbol->section == NULL) {
						continue;
					}
					symbol_info symbolinfo;
					bfd_symbol_info(symbol, &symbolinfo);

					BfdSym *new_sym = new BfdSym();
					new_sym->m_section = symbol->section->name;
					new_sym->m_flag = symbol->flags;
					new_sym->m_name = symbolinfo.name;
					new_sym->m_addr = symbolinfo.value;
					m_bfd_dbgsym_addr_map.insert(new_sym->m_addr, new_sym);
					m_bfd_dbgsym_str_map.insert(new_sym->m_name, new_sym);
					m_bfd_dbgsym_list.push_back(new_sym);
				}
			}
			free(symbol_table);
		}
	}

	/*Get dynamic symbol*/
	longresult = bfd_get_dynamic_symtab_upper_bound(m_bfd_file);
	if (longresult > 0) {
		asymbol **symbol_table = (asymbol **)malloc(longresult);
		if (symbol_table != NULL) {
			long number_of_symbols = bfd_canonicalize_dynamic_symtab(m_bfd_file, symbol_table);
			if (number_of_symbols > 0) {
				long iloop;
				for (iloop = 0; iloop < number_of_symbols; iloop++) {
					asymbol *symbol = symbol_table[iloop];
					if (symbol->section == NULL) {
						continue;
					}
					symbol_info symbolinfo;
					bfd_symbol_info(symbol, &symbolinfo);

					BfdSym *new_sym = new BfdSym();
					new_sym->m_section = symbol->section->name;
					new_sym->m_flag = symbol->flags;
					new_sym->m_name = symbolinfo.name;
					new_sym->m_addr = symbolinfo.value;
					m_bfd_dynsym_addr_map.insert(new_sym->m_addr, new_sym);
					m_bfd_dynsym_str_map.insert(new_sym->m_name, new_sym);
					m_bfd_dynsym_list.push_back(new_sym);
				}
			}

			/*dynamic-reloc need symbol_table*/
			longresult = bfd_get_dynamic_reloc_upper_bound(m_bfd_file);
			if (longresult > 0) {
				arelent **arelent_table = (arelent **)malloc(longresult);
				if (arelent_table != NULL) {
					long number_of_arelent = bfd_canonicalize_dynamic_reloc(m_bfd_file, arelent_table, symbol_table);
					if (number_of_arelent > 0) {
						int iloop;
						for (iloop = 0; iloop < number_of_arelent; iloop++) {
							arelent *arelent = arelent_table[iloop];
							bfd_symbol *sym_ptr = *(arelent->sym_ptr_ptr);
							reloc_howto_type *howto = arelent->howto;

							BbdReloc *new_reloc = new BbdReloc();
							new_reloc->m_offset = arelent->address;
							new_reloc->m_value = sym_ptr->name;
							new_reloc->m_relocto_name = howto->name;
							m_bfd_dynreloc_list.push_back(new_reloc);
						}
					}
					free(arelent_table);
				}
			}
			free(symbol_table);
		}
	}

	/*Flush code section*/
	for (iter = m_bfd_sec_list.begin(); iter != m_bfd_sec_list.end(); iter++) {
		BfdSec *sec = *iter;
		if (! sec->m_sec_code) {
			continue;
		}

		if (sec->m_vma - sec->m_foff == 0) {
			UTER_ASSERT(m_add_mkup == 0 || m_add_mkup == m_addr_range.m_begin);
			m_add_mkup = m_addr_range.m_begin;
		} else {
			UTER_ASSERT(m_add_mkup == 0);
		}

		sec->m_load_at = m_add_mkup + sec->m_vma;
		/*Flush code*/
		sec->flushCode();
	}
	return 0;

error_return:
	bfdRelease();
error_close:
	bfd_close(m_bfd_file);
	return -1;
}

int TargetMod::bfdRelease()
{
	m_bfd_dbgsym_addr_map.clear();
	m_bfd_dbgsym_str_map.clear();
	QList<BfdSym*>::iterator sym_iter;
	for (sym_iter = m_bfd_dbgsym_list.begin(); sym_iter != m_bfd_dbgsym_list.end(); /*none*/) {
		BfdSym *bfdsym = *sym_iter;
		delete bfdsym;
		m_bfd_dbgsym_list.erase(sym_iter++);
	}

	m_bfd_dynsym_addr_map.clear();
	m_bfd_dynsym_str_map.clear();
	QList<BfdSym*>::iterator dyn_iter;
	for (dyn_iter = m_bfd_dynsym_list.begin(); dyn_iter != m_bfd_dynsym_list.end(); /*none*/) {
		BfdSym *bfdsym = *dyn_iter;
		delete bfdsym;
		m_bfd_dynsym_list.erase(dyn_iter++);
	}

	QList<BbdReloc*>::iterator reloc_iter;
	for (reloc_iter = m_bfd_dynreloc_list.begin(); reloc_iter != m_bfd_dynreloc_list.end(); /*none*/) {
		BbdReloc *reloc = *reloc_iter;
		delete reloc;
		m_bfd_dynreloc_list.erase(reloc_iter++);
	}

	bfd_close(m_bfd_file);
	return 0;
}

/*Just for code section*/
BfdSec *TargetMod::bfdGetSecByAddrIn(unsigned long long addr)
{
	QList<BfdSec*>::iterator iter;
	for (iter = m_bfd_sec_list.begin(); iter != m_bfd_sec_list.end(); iter++) {
		BfdSec *sec = *iter;
		if (sec->m_sec_code) {
			if (addr >= sec->m_load_at && addr < sec->m_load_at + sec->m_size) {
				break;
			}
		}
	}
	if (iter == m_bfd_sec_list.end()) {
		return NULL;
	}
	return *iter;
}
