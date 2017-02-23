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

#include <target.hxx>
#include <ui_mainwin.hxx>

namespace DisAsmUi {

enum AsmEditMarks {
	ASM_MARKS_ARROW = 0,
};

enum AsmEditMargins {
	ASM_MARGINS_ARROW = 0,
	ASM_MARGINS_ADDR,
};

DisAsmDock::DisAsmDock()
:WidgetWithDock(SEQ_DASM, Qt::BottomDockWidgetArea, "Disassembly")
{
	QMetaObject::Connection conn;

	QVBoxLayout *out_layout = new QVBoxLayout(this);
	setLayout(out_layout);

	m_asm_scintla = new QsciScintilla(this);
	out_layout->addWidget(m_asm_scintla);

	/*Mark and logo*/
	QImage arrowimg(":/images/arrow_right.png");
	arrowimg = arrowimg.scaled(20, 20);
	m_asm_scintla->markerDefine(arrowimg, ASM_MARKS_ARROW);

	/*Margin for arrow and mark*/
	m_asm_scintla->setMarginType(ASM_MARGINS_ARROW, QsciScintilla::SymbolMargin);
	m_asm_scintla->setMarginWidth(ASM_MARGINS_ARROW, 20);
	m_asm_scintla->setMarginMarkerMask(ASM_MARGINS_ARROW, 1 << ASM_MARKS_ARROW);

	/*Margin for address list*/
	m_asm_scintla->setMarginType(ASM_MARGINS_ADDR, QsciScintilla::TextMargin);
	m_asm_scintla->setMarginWidth(ASM_MARGINS_ADDR, g_target->arch0xAddr(0));
	m_asm_scintla->setMarginMarkerMask(ASM_MARGINS_ADDR, 0);

	/*TODO:High light asm*/

	m_addr = 0;
	m_low = 0xffffffff;
	m_high = 0;

	conn = connect(m_asm_scintla, SIGNAL(SCN_DOUBLECLICK(int, int, int)), this, SLOT(slotScnDoubleClick(int, int, int)));
	UTER_ASSERT(conn != NULL);

	g_mainwin->dockSetup(this);
	return;
}

void DisAsmDock::slotScnDoubleClick(int position, int line, int modifiers)
{
	/*When scintilla is empty, lines() return 1, so max line number is lines() - 1*/
	int last_row_num = m_asm_scintla->lines() - 1;

	if (line == 0) {
		/*Double click first line, expand asm backward*/
		disAsm(0, -1);
		return;
	}

	if (line == last_row_num) {
		/*Double click last line, expand asm forward*/
		disAsm(0, 1);
		return;
	}

	return;
}

/*Disassemble on address, or do expand*/
int DisAsmDock::disAsm(unsigned long long addr, int ext)
{
	/*phase 1: Get address range and line number for insert*/
	int insert_before_row;
	unsigned long long dasm_from;
	unsigned long long dasm_to;

	if (ext != 0) {
		/*Expand existing asm*/
		if (m_low == UTER_INVALID_ADDR && m_high == 0) {
			/*Cannot expand when empty*/
			return 0;
		}
		if (ext < 0) {
			/*Backward expand, insert before line 1 because line 0 is line for double click comment*/
			insert_before_row = 1;
			dasm_from = m_low - 4 * STARTAT_STEP;
			dasm_to = m_low;
		} else {
			/*Forward expand, insert before last line because last line for double click comment*/
			insert_before_row = /*last line*/(m_asm_scintla->lines() - 1);
			dasm_from = m_high;
			dasm_to = m_high + 4 * STARTAT_STEP;
		}
	} else {
		/*Direct disassemble on address*/
		if (! (m_low == UTER_INVALID_ADDR && m_high == 0)) {
			/*When no empty, try expand instead of direct disassemble*/
			if (addr >= m_low && addr < m_high) {
				/*Address disassembled ready*/
				m_addr = addr;
				setArrow(m_addr, true);
				return 0;
			} else if (addr < m_low && addr >= m_low - 4 * STARTAT_STEP) {
				/*Backward expand instead of direct disassemble*/
				insert_before_row = 1;
				dasm_from = m_low - 4 * STARTAT_STEP;
				dasm_to = m_low;
			} else if (addr >= m_high && addr < m_high + 4 * STARTAT_STEP) {
				/*Forward expand instead of direct disassemble*/
				insert_before_row = /*last line*/(m_asm_scintla->lines() - 1);
				dasm_from = m_high;
				dasm_to = m_high + 4 * STARTAT_STEP;
			} else {
				clear();
			}
		}

		m_addr = addr;

		if (m_low == UTER_INVALID_ADDR && m_high == 0) {
			/*Insert 0 line and last line for double chick*/
			m_asm_scintla->append(";  ......Double click here for more......\n");
			m_asm_scintla->append(";  ......Double click here for more......");		/*No \n*/

			insert_before_row = 1;
			dasm_from = m_addr - 2 * STARTAT_STEP;
			dasm_to = m_addr + 2 * STARTAT_STEP;
		}
	}

	/*phase 2: Do disassemble, get module, section, do disassemble*/
	TargetMod *mod = g_target->modGetByAddrIn(m_addr);
	if (mod == NULL) {
		return -1;
	}
	BfdSec *mysec = mod->bfdGetSecByAddrIn(m_addr);
	if (mysec == NULL) {
		return -1;
	}
	BfdAsmSet *asm_range = mysec->dasm(dasm_from, dasm_to);
	if (asm_range == NULL) {
		return -1;
	} else if (asm_range->m_bfd_asm_list.empty()) {
		delete asm_range;
		return 0;
	} else {
		BfdAsm *myasm;
		myasm = asm_range->m_bfd_asm_list.front();
		if (myasm->m_offset < m_low) {
			m_low = myasm->m_offset;
		}
		myasm = asm_range->m_bfd_asm_list.back();
		if (myasm->m_offset + myasm->m_octets > m_high) {
			m_high = myasm->m_offset + myasm->m_octets;
		}
	}

	QsciStyle addrstyle;
	addrstyle.setPaper(QColor(200, 200, 200, 0));

	QList<BfdAsm*>::iterator iter;
	for (iter = asm_range->m_bfd_asm_list.begin(); iter != asm_range->m_bfd_asm_list.end(); iter++) {
		BfdAsm *myasm = *iter;

		int move_down = 0;
		/*Insert empty line with function symbol margin*/
		QMap<unsigned long long, BfdSym*>::iterator iter_sym;
		iter_sym = mod->m_bfd_dynsym_addr_map.find(myasm->m_offset - mysec->m_mod->m_add_mkup);
		if (iter_sym != mod->m_bfd_dynsym_addr_map.end()) {
			/*Find function symbol on bfd information*/
			BfdSym *sym = iter_sym.value();
			move_down++;
			m_asm_scintla->insertAt(sym->m_name + ":\n", insert_before_row, 0);
			m_asm_scintla->setMarginText(insert_before_row, "", addrstyle);
			insert_before_row++;
		} else {
			/*Find function symbol on dwarf information*/
			iter_sym = mod->m_bfd_dbgsym_addr_map.find(myasm->m_offset - mysec->m_mod->m_add_mkup);
			if (iter_sym != mod->m_bfd_dbgsym_addr_map.end()) {
				BfdSym *sym = iter_sym.value();
				move_down++;
				m_asm_scintla->insertAt(sym->m_name + ":\n", insert_before_row, 0);
				m_asm_scintla->setMarginText(insert_before_row, "", addrstyle);
				insert_before_row++;
			}
		}

		/*Move down lines which number greater than or equal to current insert place*/
		move_down++;
		QMap<unsigned long long, unsigned int>::iterator lno_iter;
		for (lno_iter = m_add_lno.begin(); lno_iter != m_add_lno.end(); lno_iter++) {
			if (lno_iter.value() >= insert_before_row) {
				m_add_lno.insert(lno_iter.key(), lno_iter.value() + move_down);
			}
		}
		m_add_lno.insert(myasm->m_offset, insert_before_row);

		/*Insert line*/
		m_asm_scintla->insertAt(myasm->m_asm_code + "\n", insert_before_row, 0);
		m_asm_scintla->setMarginText(insert_before_row, g_target->arch0xAddr(myasm->m_offset), addrstyle);
		insert_before_row++;
	}
	delete asm_range;

	setArrow(addr, ext == 0);		/*Set arrow if not expand*/
	return 0;
}

int DisAsmDock::setArrow(unsigned long long addr, bool showit)
{
	clearArrow();

	/*Get line number from address*/
	QMap<unsigned long long, unsigned int>::iterator row_iter;
	row_iter = m_add_lno.find(addr);
	if (row_iter == m_add_lno.end()) {
		return -1;
	}
	unsigned int arrowat = row_iter.value();

	m_asm_scintla->markerAdd(arrowat, ASM_MARGINS_ARROW);
	if (showit) {
		/*Show arrow in window*/
		m_asm_scintla->setCursorPosition(arrowat, 0);
		m_asm_scintla->ensureLineVisible(arrowat);
	}
	return 0;
}

int DisAsmDock::clearArrow()
{
	m_asm_scintla->markerDeleteAll(ASM_MARGINS_ARROW);
	return 0;
}

int DisAsmDock::clear()
{
	m_asm_scintla->clearMask();
	m_asm_scintla->clearMarginText(-1);
	m_asm_scintla->clear();

	m_addr = 0;
	m_low = UTER_INVALID_ADDR;
	m_high = 0;

	m_add_lno.clear();
	return 0;
}

} /*namespace DisAsmUi*/
