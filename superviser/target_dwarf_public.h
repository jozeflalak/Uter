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

#ifndef SRC_TARGET_DWARF_PUBLIC_H_
#define SRC_TARGET_DWARF_PUBLIC_H_

#define DWF_INVALID_OFF (0xffffffffffffffff)
#define DWF_INVALID_SIZE (0xffffffff)
#define DWF_INVALID_ADDR (0xffffffffffffffff)
#define DWF_INVALID_FILELINE (0xffffffffffffffff)

/*Dwarf data range*/
class DwfAddrRange {
public:
	inline DwfAddrRange()
	{
		m_begin = 0;
		m_offset = 0;
		return;
	}
	inline DwfAddrRange(unsigned long long begin, unsigned long long offset)
	{
		m_begin = begin;
		m_offset = offset;
		return;
	}
	inline void set(unsigned long long begin, unsigned long long offset)
	{
		m_begin = begin;
		m_offset = offset;
		return;
	}
	inline unsigned long long low()
	{
		return m_begin;
	}
	inline unsigned long long high()
	{
		return m_begin + m_offset;
	}
	inline bool check(unsigned long long addr)
	{
		if (addr >= m_begin && addr < m_begin + m_offset) {
			return true;
		} else {
			return false;
		}
	}
    bool operator==(const DwfAddrRange& cmp) const
    {
        return (m_begin == cmp.m_begin) && (m_offset == cmp.m_offset);
    }
	unsigned long long m_begin;									/*Address begin(include)*/
	unsigned long long m_offset;								/*Address offset from begin(no include)*/
};

/*Dwarf item*/
class DwfCu;
class DwfItem {
public:
	enum DwfItemType {
		DWF_ITEM_TYPE = 0,
		DWF_ITEM_VAR,
		DWF_ITEM_FUN_PARA,
		DWF_ITEM_FUN,
		DWF_ITEM_MAX
	};
	DwfItem(DwfCu *cu)
	{
		m_element_type = DWF_ITEM_MAX;

		m_cu = cu;
		m_offset = DWF_INVALID_OFF;
		m_fileno = 0;
		m_lineno = 0;
		return;
	}
	DwfItem(DwfCu *cu, unsigned long long offset)
	{
		m_element_type = DWF_ITEM_MAX;

		m_cu = cu;
		m_offset = offset;
		m_fileno = 0;
		m_lineno = 0;
		return;
	}
	virtual ~DwfItem()
	{
		return;
	}
	DwfItemType m_element_type;							/*Dwarf item type, include data type, function, variable*/
	DwfCu *m_cu;										/*Dwarf item from Cu, because source file and line base on Cu*/
	unsigned long long m_offset;						/*Dwarf item offset in module, no in Cu*/
	unsigned long long m_fileno;						/*Dwarf source file number*/
	unsigned long long m_lineno;						/*Dwarf source line number*/
	QString makeItemScript(QString cu, QString name);
};

#endif /*SRC_TARGET_DWARF_PUBLIC_H_*/
