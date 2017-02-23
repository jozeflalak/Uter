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

#ifndef SRC_UI_DISASSEMBLY_HXX_
#define SRC_UI_DISASSEMBLY_HXX_

#include <qsciscintilla.h>
#include <qscistyle.h>

#include <ui_public.hxx>

namespace DisAsmUi {

class DisAsmDock: public WidgetWithDock {
	Q_OBJECT
private slots:
	void slotScnDoubleClick(int, int, int);
public:
	DisAsmDock();
	int disAsm(unsigned long long addr, int ext = 0);
	int setArrow(unsigned long long addr, bool showit);
	int clearArrow();
	int clear();
private:
	QsciScintilla *m_asm_scintla;						/*Scintilla edit*/
	unsigned long long m_addr;							/*Address in module section*/
	unsigned long long m_low;							/*Address range low*/
	unsigned long long m_high;							/*Address range high*/
	QMap<unsigned long long, unsigned int> m_add_lno;	/*Address to line number*/
};

} /*namespace DisAsmUi*/
#endif /*SRC_UI_DISASSEMBLY_HXX_*/
