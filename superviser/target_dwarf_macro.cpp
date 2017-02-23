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

/*TODO:all file*/

static int i__about_macor_need_clean__ = 0;

typedef long long dwarf_signed_vma;
typedef unsigned long long dwarf_vma;
typedef unsigned long long elf_vma;

elf_vma byte_get_little_endian (unsigned char *field, int size)
{
  switch (size)
    {
    case 1:
      return *field;

    case 2:
      return  ((unsigned int) (field[0]))
	|    (((unsigned int) (field[1])) << 8);

    case 3:
      return  ((unsigned long) (field[0]))
	|    (((unsigned long) (field[1])) << 8)
	|    (((unsigned long) (field[2])) << 16);

    case 4:
      return  ((unsigned long) (field[0]))
	|    (((unsigned long) (field[1])) << 8)
	|    (((unsigned long) (field[2])) << 16)
	|    (((unsigned long) (field[3])) << 24);

    case 5:
      if (sizeof (elf_vma) == 8)
	return  ((elf_vma) (field[0]))
	  |    (((elf_vma) (field[1])) << 8)
	  |    (((elf_vma) (field[2])) << 16)
	  |    (((elf_vma) (field[3])) << 24)
	  |    (((elf_vma) (field[4])) << 32);
      else if (sizeof (elf_vma) == 4)
	/* We want to extract data from an 8 byte wide field and
	   place it into a 4 byte wide field.  Since this is a little
	   endian source we can just use the 4 byte extraction code.  */
	return  ((unsigned long) (field[0]))
	  |    (((unsigned long) (field[1])) << 8)
	  |    (((unsigned long) (field[2])) << 16)
	  |    (((unsigned long) (field[3])) << 24);

    case 6:
      if (sizeof (elf_vma) == 8)
	return  ((elf_vma) (field[0]))
	  |    (((elf_vma) (field[1])) << 8)
	  |    (((elf_vma) (field[2])) << 16)
	  |    (((elf_vma) (field[3])) << 24)
	  |    (((elf_vma) (field[4])) << 32)
	  |    (((elf_vma) (field[5])) << 40);
      else if (sizeof (elf_vma) == 4)
	/* We want to extract data from an 8 byte wide field and
	   place it into a 4 byte wide field.  Since this is a little
	   endian source we can just use the 4 byte extraction code.  */
	return  ((unsigned long) (field[0]))
	  |    (((unsigned long) (field[1])) << 8)
	  |    (((unsigned long) (field[2])) << 16)
	  |    (((unsigned long) (field[3])) << 24);

    case 7:
      if (sizeof (elf_vma) == 8)
	return  ((elf_vma) (field[0]))
	  |    (((elf_vma) (field[1])) << 8)
	  |    (((elf_vma) (field[2])) << 16)
	  |    (((elf_vma) (field[3])) << 24)
	  |    (((elf_vma) (field[4])) << 32)
	  |    (((elf_vma) (field[5])) << 40)
	  |    (((elf_vma) (field[6])) << 48);
      else if (sizeof (elf_vma) == 4)
	/* We want to extract data from an 8 byte wide field and
	   place it into a 4 byte wide field.  Since this is a little
	   endian source we can just use the 4 byte extraction code.  */
	return  ((unsigned long) (field[0]))
	  |    (((unsigned long) (field[1])) << 8)
	  |    (((unsigned long) (field[2])) << 16)
	  |    (((unsigned long) (field[3])) << 24);

    case 8:
      if (sizeof (elf_vma) == 8)
	return  ((elf_vma) (field[0]))
	  |    (((elf_vma) (field[1])) << 8)
	  |    (((elf_vma) (field[2])) << 16)
	  |    (((elf_vma) (field[3])) << 24)
	  |    (((elf_vma) (field[4])) << 32)
	  |    (((elf_vma) (field[5])) << 40)
	  |    (((elf_vma) (field[6])) << 48)
	  |    (((elf_vma) (field[7])) << 56);
      else if (sizeof (elf_vma) == 4)
	/* We want to extract data from an 8 byte wide field and
	   place it into a 4 byte wide field.  Since this is a little
	   endian source we can just use the 4 byte extraction code.  */
	return  ((unsigned long) (field[0]))
	  |    (((unsigned long) (field[1])) << 8)
	  |    (((unsigned long) (field[2])) << 16)
	  |    (((unsigned long) (field[3])) << 24);

    default:
      printf("Unhandled data length: %d\n", size);
      abort ();
    }
}

elf_vma (*byte_get)(unsigned char *, int) = byte_get_little_endian;

#define SAFE_BYTE_GET(VAL, PTR, AMOUNT, END)	\
  do						\
    {						\
      int dummy [sizeof (VAL) < (AMOUNT) ? -1 : 1] ATTRIBUTE_UNUSED ; \
      unsigned int amount = (AMOUNT);		\
      if (((PTR) + amount) >= (END))		\
	{					\
	  if ((PTR) < (END))			\
	    amount = (END) - (PTR);		\
	  else					\
	    amount = 0;				\
	}					\
      if (amount)				\
	VAL = byte_get ((PTR), amount);		\
      else					\
	VAL = 0;				\
    }						\
  while (0)

#define SAFE_BYTE_GET_AND_INC(VAL, PTR, AMOUNT, END)	\
  do							\
    {							\
      SAFE_BYTE_GET (VAL, PTR, AMOUNT, END);		\
      PTR += AMOUNT;					\
    }							\
  while (0)

dwarf_vma read_leb128 (unsigned char *data,
	     	 	 	 	 unsigned int *length_return,
						 bfd_boolean sign,
						 const unsigned char * const end)
{
  dwarf_vma result = 0;
  unsigned int num_read = 0;
  unsigned int shift = 0;
  unsigned char byte = 0;

  while (data < end)
    {
      byte = *data++;
      num_read++;

      result |= ((dwarf_vma) (byte & 0x7f)) << shift;

      shift += 7;
      if ((byte & 0x80) == 0)
	break;
    }

  if (length_return != NULL)
    *length_return = num_read;

  if (sign && (shift < 8 * sizeof (result)) && (byte & 0x40))
    result |= (dwarf_vma) -1 << shift;

  return result;
}

/* Create a signed version to avoid painful typecasts.  */
static inline dwarf_signed_vma read_sleb128 (unsigned char * data,
	      	  	  	  	  	  	  	  	  	 unsigned int *  length_return,
											 const unsigned char * const end)
{
	return (dwarf_signed_vma) read_leb128 (data, length_return, TRUE, end);
}

static inline dwarf_vma read_uleb128 (unsigned char * data,
	      	  	  	  	  	  	  	  unsigned int *  length_return,
									  const unsigned char * const end)
{
	return read_leb128 (data, length_return, FALSE, end);
}

static const char *get_FORM_name(unsigned long form)
{
	const char *name = NULL;

	if (form == 0) {
		return "DW_FORM value: 0";
	}

	int result = dwarf_get_FORM_name(form, &name);
	if (result == DW_DLV_OK) {
		return name;
	} else {
	    static char buffer[100];
	    snprintf (buffer, sizeof (buffer), "Unknown FORM value: %lx", form);
	    return buffer;
	}
}

static unsigned char *fetch_indirect_string(Dwarf_Debug dbg, dwarf_vma offset)
{
	int result;
	char *returned_str;
	Dwarf_Signed returned_str_len;
	Dwarf_Error error;

	result = dwarf_get_str(dbg, offset, &returned_str, &returned_str_len, &error);
	if (result == DW_DLV_NO_ENTRY) {
		UTER_DUMP("End of debug info.");
		return NULL;
	} else if (result == DW_DLV_ERROR) {
    	UTER_LOG("Get debug information error, description %s.", dwarf_errmsg(error));
    	dwarf_dealloc(dbg, error, DW_DLA_ERROR);
    	return NULL;
	}

	return (unsigned char *)returned_str;
}

static unsigned char *get_line_filename_and_dirname(dwarf_vma line_offset,
			       dwarf_vma fileidx,unsigned char **dir_name,
				   Dwarf_Small *dss_data, Dwarf_Unsigned dss_size)
{
  unsigned char *hdrptr, *dirtable, *file_name;
  unsigned int offset_size, initial_length_size;
  unsigned int version, opcode_base, bytes_read;
  dwarf_vma length, diridx;
  const unsigned char * end;

  *dir_name = NULL;
  if (dss_data == NULL
      || line_offset >= dss_size
      || fileidx == 0)
    return NULL;

  hdrptr = dss_data + line_offset;
  end = dss_data + dss_size;

  SAFE_BYTE_GET_AND_INC (length, hdrptr, 4, end);
  if (length == 0xffffffff)
    {
      /* This section is 64-bit DWARF 3.  */
      SAFE_BYTE_GET_AND_INC (length, hdrptr, 8, end);
      offset_size = 8;
      initial_length_size = 12;
    }
  else
    {
      offset_size = 4;
      initial_length_size = 4;
    }
  if (length + initial_length_size > dss_size)
    return NULL;

  SAFE_BYTE_GET_AND_INC (version, hdrptr, 2, end);
  if (version != 2 && version != 3 && version != 4)
    return NULL;
  hdrptr += offset_size + 1;/* Skip prologue_length and min_insn_length.  */
  if (version >= 4)
    hdrptr++;		    /* Skip max_ops_per_insn.  */
  hdrptr += 3;		    /* Skip default_is_stmt, line_base, line_range.  */

  SAFE_BYTE_GET_AND_INC (opcode_base, hdrptr, 1, end);
  if (opcode_base == 0)
    return NULL;

  hdrptr += opcode_base - 1;
  dirtable = hdrptr;
  /* Skip over dirname table.  */
  while (*hdrptr != '\0')
    hdrptr += strnlen ((char *) hdrptr, end - hdrptr) + 1;
  hdrptr++;		    /* Skip the NUL at the end of the table.  */
  /* Now skip over preceding filename table entries.  */
  for (; *hdrptr != '\0' && fileidx > 1; fileidx--)
    {
      hdrptr += strnlen ((char *) hdrptr, end - hdrptr) + 1;
      read_uleb128 (hdrptr, &bytes_read, end);
      hdrptr += bytes_read;
      read_uleb128 (hdrptr, &bytes_read, end);
      hdrptr += bytes_read;
      read_uleb128 (hdrptr, &bytes_read, end);
      hdrptr += bytes_read;
    }
  if (hdrptr == end || *hdrptr == '\0')
    return NULL;
  file_name = hdrptr;
  hdrptr += strnlen ((char *) hdrptr, end - hdrptr) + 1;
  diridx = read_uleb128 (hdrptr, &bytes_read, end);
  if (diridx == 0)
    return file_name;
  for (; *dirtable != '\0' && diridx > 1; diridx--)
    dirtable += strnlen ((char *) dirtable, end - dirtable) + 1;
  if (*dirtable == '\0')
    return NULL;
  *dir_name = dirtable;
  return file_name;
}

enum dwarf_macro_record_type {
	DW_MACRO_GNU_define = 1,
    DW_MACRO_GNU_undef = 2,
    DW_MACRO_GNU_start_file = 3,
    DW_MACRO_GNU_end_file = 4,
    DW_MACRO_GNU_define_indirect = 5,
    DW_MACRO_GNU_undef_indirect = 6,
    DW_MACRO_GNU_transparent_include = 7,
    DW_MACRO_GNU_define_indirect_alt = 8,
    DW_MACRO_GNU_undef_indirect_alt = 9,
    DW_MACRO_GNU_transparent_include_alt = 10,
    DW_MACRO_GNU_lo_user = 0xe0,
    DW_MACRO_GNU_hi_user = 0xff
};

/*参考：
 * extern_source/libdwarf-code/libdwarf/dwarf_macro.c/dwarf_get_macro_details
 * extern_source/binutils-2.25/binutils/dwarf.c
 * */
int parse_macro(Dwarf_Debug dbg, TargetMod *dwfinfo)
{
	int result;
	Dwarf_Small *macro_dss_data;
	Dwarf_Unsigned macro_dss_size;
	Dwarf_Error error;

	result = dwarf_get_macro_data(dbg, &macro_dss_data, &macro_dss_size, &error);
	if (result == DW_DLV_NO_ENTRY) {
		UTER_DUMP("End of debug info.");
		return 0;
	} else if (result == DW_DLV_ERROR) {
    	UTER_LOG("Get debug information error, description %s.", dwarf_errmsg(error));
    	dwarf_dealloc(dbg, error, DW_DLA_ERROR);
    	return -1;
	}

	Dwarf_Small *line_dss_data;
	Dwarf_Unsigned line_dss_size;
	result = dwarf_get_line_data(dbg, &line_dss_data, &line_dss_size, &error);
	if (result == DW_DLV_NO_ENTRY) {
		UTER_DUMP("End of debug info.");
		return 0;
	} else if (result == DW_DLV_ERROR) {
    	UTER_LOG("Get debug information error, description %s.", dwarf_errmsg(error));
    	dwarf_dealloc(dbg, error, DW_DLA_ERROR);
    	return -1;
	}

	//开始解析
	unsigned char *start = macro_dss_data;
	unsigned char *end = start + macro_dss_size;
	unsigned char *curr = start;
	unsigned char *extended_op_buf[256];
	unsigned int bytes_read;

	while (curr < end) {
		unsigned int lineno, version, flags;
		unsigned int offset_size = 4;
		const unsigned char *string;
		dwarf_vma line_offset = 0, sec_offset = curr - start, offset;
		unsigned char **extended_ops = NULL;

		//验证版本号
		SAFE_BYTE_GET_AND_INC(version, curr, 2, end);
		if (version != 4) {
			printf("Only GNU extension to DWARF 4 of xxx is currently supported.\n");
			return 0;
		}

		//获取和分析标志位
		SAFE_BYTE_GET_AND_INC(flags, curr, 1, end);
		if (flags & 1) {
			offset_size = 8;
		}

		//printf("  Offset:                      0x%lx\n", (unsigned long)sec_offset);
		//printf("  Version:                     %d\n", version);
		//printf("  Offset size:                 %d\n", offset_size);

		dwf_macro *macor = new dwf_macro();;
		dwf_macro_file *macor_file = &(macor->m_root_file);

		if (flags & 2) {
			SAFE_BYTE_GET_AND_INC(line_offset, curr, offset_size, end);
			//printf("  Offset into .debug_line:     0x%lx\n", (unsigned long)line_offset);

			//创建macro结构,最后是要给macor关联到cu的
			macor->m_line_off = line_offset;
		}

		if (flags & 4) {
			unsigned int i, count, op;
			dwarf_vma nargs, n;

			SAFE_BYTE_GET_AND_INC(count, curr, 1, end);
			memset (extended_op_buf, 0, sizeof (extended_op_buf));
			extended_ops = extended_op_buf;
			if (count) {
		      //printf("  Extension opcode arguments:\n");
		      for (i = 0; i < count; i++) {
		    	  SAFE_BYTE_GET_AND_INC(op, curr, 1, end);
		    	  extended_ops[op] = curr;
		    	  nargs = read_uleb128 (curr, &bytes_read, end);
		    	  curr += bytes_read;
		    	  if (nargs == 0) {
		    		  //printf ("    DW_MACRO_GNU_%02x has no arguments\n", op);
		    	  } else {
		    		  //printf ("    DW_MACRO_GNU_%02x arguments: ", op);
		    		  for (n = 0; n < nargs; n++) {
		    			  unsigned int form;

		    			  SAFE_BYTE_GET_AND_INC (form, curr, 1, end);
		    			  //printf("%s%s", get_FORM_name (form), n == nargs - 1 ? "\n" : ", ");
		    			  switch(form) {
		    			  	  case DW_FORM_data1:
		    			  	  case DW_FORM_data2:
		    			  	  case DW_FORM_data4:
		    			  	  case DW_FORM_data8:
		    			  	  case DW_FORM_sdata:
		    			  	  case DW_FORM_udata:
		    			  	  case DW_FORM_block:
		    			  	  case DW_FORM_block1:
		    			  	  case DW_FORM_block2:
		    			  	  case DW_FORM_block4:
		    			  	  case DW_FORM_flag:
		    			  	  case DW_FORM_string:
		    			  	  case DW_FORM_strp:
		    			  	  case DW_FORM_sec_offset:
		    			  		  break;
		    			  	  default:
		    			  		  //printf("Invalid extension opcode form %s\n", get_FORM_name (form));
		    			  		  return 0;
		    			  	  }
		    		  }
			    }
			}
		    }
		}

	    //printf("\n");

	    while (1) {
	    	unsigned int op;

	    	if (curr >= end) {
	    		printf(".debug_macro section not zero terminated\n");
	    		return 0;
	    	}

	    	SAFE_BYTE_GET_AND_INC (op, curr, 1, end);
	    	if (op == 0) {
	    		//op等于零就代表结束了
	    		break;
	    	}

	    	switch (op) {
		    case DW_MACRO_GNU_start_file:
		    {
		    	unsigned int filenum;
		    	unsigned char *file_name = NULL, *dir_name = NULL;

		    	lineno = read_uleb128(curr, &bytes_read, end);
		    	curr += bytes_read;
		    	filenum = read_uleb128(curr, &bytes_read, end);
		    	curr += bytes_read;

		    	if ((flags & 2) == 0) {
		    		//printf("DW_MACRO_GNU_start_file used, but no .debug_line offset provided.\n");
		    	} else {
		    		file_name = get_line_filename_and_dirname (line_offset, filenum, &dir_name, line_dss_data, line_dss_size);

			    	if (file_name == NULL) {
			    		//printf(" DW_MACRO_GNU_start_file - lineno: %d filenum: %d\n", lineno, filenum);
			    	} else {
			    		//printf(" DW_MACRO_GNU_start_file - lineno: %d filenum: %d filename: %s%s%s\n",
			    		//		lineno, filenum,
						//		dir_name != NULL ? (const char *) dir_name : "",
						//				dir_name != NULL ? "/" : "", file_name);
			    	}
		    	}

		    	//遇到文件开始就压入
		    	dwf_macro_file *macro_file_new = new dwf_macro_file();
		    	macro_file_new->m_parrent = macor_file;
		    	if (dir_name != NULL) {
		    		macro_file_new->m_dir = (char *)dir_name;
		    	}
		    	if (file_name != NULL) {
		    		macro_file_new->m_name = (char *)file_name;
		    	}
		    	macor_file->m_item_list.push_back(macro_file_new);
		    	macor_file = macro_file_new;
		    	break;
		    }
		    case DW_MACRO_GNU_end_file:
		    {
		    	//printf(" DW_MACRO_GNU_end_file\n");
		    	//遇到文件结束就弹出
		    	if (macor_file->m_parrent != NULL) {
		    		macor_file = macor_file->m_parrent;
		    	}
		    	break;
		    }

		    case DW_MACRO_GNU_define:
		    {
		    	lineno = read_uleb128 (curr, &bytes_read, end);
		    	curr += bytes_read;
		    	string = curr;
		    	curr += strnlen ((char *) string, end - string) + 1;
		    	//printf(" DW_MACRO_GNU_define - lineno : %d macro : %s\n", lineno, string);
		    	dwf_macro_define *macro_def = new dwf_macro_define();
		    	macro_def->m_lineno = lineno;
		    	macro_def->m_defstr = (char *)string;
		    	macor_file->m_item_list.push_back(macro_def);
		    	break;
		    }
		    case DW_MACRO_GNU_undef:
		    {
		    	lineno = read_uleb128 (curr, &bytes_read, end);
		    	curr += bytes_read;
		    	string = curr;
		    	curr += strnlen ((char *) string, end - string) + 1;
		    	//printf(" DW_MACRO_GNU_undef - lineno : %d macro : %s\n", lineno, string);
		    	dwf_macro_undefine *macro_undef = new dwf_macro_undefine();
		    	macro_undef->m_lineno = lineno;
		    	macro_undef->m_defstr = (char *)string;
		    	macor_file->m_item_list.push_back(macro_undef);
		    	break;
		    }
		    case DW_MACRO_GNU_define_indirect:
		    {
		    	lineno = read_uleb128 (curr, &bytes_read, end);
		    	curr += bytes_read;
		    	SAFE_BYTE_GET_AND_INC (offset, curr, offset_size, end);
		    	string = fetch_indirect_string(dbg, offset);
		    	//printf(" DW_MACRO_GNU_define_indirect - lineno : %d macro : %s\n", lineno, string);
		    	dwf_macro_define_indirect *macro_def = new dwf_macro_define_indirect();
		    	macro_def->m_lineno = lineno;
		    	macro_def->m_defstr = (char *)string;
		    	macor_file->m_item_list.push_back(macro_def);
		    	break;
		    }
		    case DW_MACRO_GNU_undef_indirect:
		    {
		    	lineno = read_uleb128 (curr, &bytes_read, end);
		    	curr += bytes_read;
		    	SAFE_BYTE_GET_AND_INC (offset, curr, offset_size, end);
		    	string = fetch_indirect_string(dbg, offset);
		    	//printf(" DW_MACRO_GNU_undef_indirect - lineno : %d macro : %s\n", lineno, string);
		    	dwf_macro_undefine_indirect *macro_undef = new dwf_macro_undefine_indirect();
		    	macro_undef->m_lineno = lineno;
		    	macro_undef->m_defstr = (char *)string;
		    	macor_file->m_item_list.push_back(macro_undef);
		    	break;
		    }
		    case DW_MACRO_GNU_transparent_include:
		    {
		    	SAFE_BYTE_GET_AND_INC (offset, curr, offset_size, end);
		    	//printf(" DW_MACRO_GNU_transparent_include - offset : 0x%lx\n", (unsigned long) offset);
		    	break;
		    }
		    case DW_MACRO_GNU_define_indirect_alt:
		    {
		    	lineno = read_uleb128 (curr, &bytes_read, end);
		    	curr += bytes_read;
		    	SAFE_BYTE_GET_AND_INC (offset, curr, offset_size, end);
		    	//printf(" DW_MACRO_GNU_define_indirect_alt - lineno : %d macro offset : 0x%lx\n", lineno, (unsigned long) offset);
		    	break;
		    }
		    case DW_MACRO_GNU_undef_indirect_alt:
		    {
		    	lineno = read_uleb128 (curr, &bytes_read, end);
		    	curr += bytes_read;
		    	SAFE_BYTE_GET_AND_INC (offset, curr, offset_size, end);
		    	//printf(" DW_MACRO_GNU_undef_indirect_alt - lineno : %d macro offset : 0x%lx\n", lineno, (unsigned long) offset);
		    	break;
		    }
		    case DW_MACRO_GNU_transparent_include_alt:
		    {
		    	SAFE_BYTE_GET_AND_INC (offset, curr, offset_size, end);
		    	//printf(" DW_MACRO_GNU_transparent_include_alt - offset : 0x%lx\n", (unsigned long) offset);
		    	break;
		    }
		    default:
		    {
		    	if (extended_ops == NULL || extended_ops[op] == NULL) {
		    		printf(" Unknown macro opcode %02x seen\n", op);
		    		return 0;
		    	} else {
		    		/* Skip over unhandled opcodes.  */
		    		dwarf_vma nargs, n;
		    		unsigned char *desc = extended_ops[op];
		    		nargs = read_uleb128 (desc, &bytes_read, end);
		    		desc += bytes_read;
		    		if (nargs == 0) {
		    			printf (" DW_MACRO_GNU_%02x\n", op);
		    			break;
		    		}
		    		printf(" DW_MACRO_GNU_%02x -", op);
		    		for (n = 0; n < nargs; n++) {
		    			int val;

		    			SAFE_BYTE_GET_AND_INC (val, desc, 1, end);
		    			//curr = read_and_display_attr_value(0, val,
		    			//	       curr, end, 0, 0, offset_size,
		    			//	       version, NULL, 0, NULL, NULL);
		    			//if (n != nargs - 1) {
		    			//	printf (",");
		    			//}
		    		}
		    		printf ("\n");
		    	}
		    	break;
		    }
		    }
		}

	    //结束一轮，可以保存结构了
	    DwfCu *cu = dwfinfo->dwarfGetCuByLineOff(macor->m_line_off);
	    if (cu == NULL) {
	    	delete macor;
	    	macor = NULL;
	    } else {
	    	cu->m_macro = macor;
	    }
	    //printf ("\n");
	    if (macor != NULL) {
	    	//extern int dump_macro_file(dwf_macro_file *macor_file);
	    	//dump_macro_file(&(macor->m_root_file));
	    }
	}

	return 0;
}

bool macro_match(QString *macro_string, char *macro_name)
{
	size_t find_at = macro_string->toStdString().find(macro_name);
	if (string::npos == find_at) {
		return false;
	}

	if (find_at != 0) {
		return false;
	}

	if (macro_string->length() == strlen(macro_name)) {
		return true;
	}

	char chr = macro_string->toStdString().at(strlen(macro_name));
	if (chr == ' ') {
		return true;
	}

	return false;
}

/*return -1:error, 0 notfount, 1 undefine 1 define*/
int get_macro_value(dwf_macro_file *macro_file, char *macro_name, const char **macro_value)
{
	int result;

	QList<dwf_macro_item*>::iterator riter;
	for (riter = macro_file->m_item_list.end(); riter != macro_file->m_item_list.begin(); /*none*/) {
		dwf_macro_item *item = *(--riter);

		switch(item->m_item_type)
		{
		case DWF_MACRO_ITEM_FILE:
		{
			dwf_macro_file *macro_file = (dwf_macro_file *)item;
			result = get_macro_value(macro_file, macro_name, macro_value);
			if (result > 0) {
				return result;
			}
			break;
		}
		case DWF_MACRO_ITEM_DEF:
		{
			dwf_macro_define *macro_define = (dwf_macro_define *)item;
			if (macro_match(&(macro_define->m_defstr), macro_name)) {
				*macro_value = macro_define->m_defstr.toStdString().c_str();
				return 2;
			}
			break;
		}
		case DWF_MACRO_ITEM_UNDEF:
		{
			dwf_macro_undefine *macro_undefine = (dwf_macro_undefine *)item;
			if (macro_match(&(macro_undefine->m_defstr), macro_name)) {
				*macro_value = macro_undefine->m_defstr.toStdString().c_str();
				return 1;
			}
			break;
		}
		case DWF_MACRO_ITEM_DEF_INDIRECT:
		{
			dwf_macro_define_indirect *macor_define_indirect = (dwf_macro_define_indirect *)item;
			if (macro_match(&(macor_define_indirect->m_defstr), macro_name)) {
				*macro_value = macor_define_indirect->m_defstr.toStdString().c_str();
				return 2;
			}
			break;
		}
		case DWF_MACRO_ITEM_UNDEF_INDIRECT:
		{
			dwf_macro_undefine_indirect *macor_undefine_indirect = (dwf_macro_undefine_indirect *)item;
			if (macro_match(&(macor_undefine_indirect->m_defstr), macro_name)) {
				*macro_value = macor_undefine_indirect->m_defstr.toStdString().c_str();
				return 1;
			}
			break;
		}
		}
	}

	return 0;
}
