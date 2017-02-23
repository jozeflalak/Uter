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

#include <ui_mainwin.hxx>

namespace BrkPntUi {

NewFunBrkDlg::NewFunBrkDlg(QWidget *parent)
:QDialog(parent)
{
	QMetaObject::Connection conn;

	/* Out layout
	 * --------------------------------
     *  input ware
     * --------------------------------
     *          space
     * --------------------------------
	 *                     Buttons
	 * --------------------------------*/
	QVBoxLayout *top_layout = new QVBoxLayout(this);
	top_layout->setAlignment(Qt::AlignBottom);
	setLayout(top_layout);

	/* Input layout
	 * ----------------------------------
	 * Source file   |   input    |  browse
	 * Function name |   input
	 * ----------------------------------*/
	QGridLayout *main_layout = new QGridLayout(this);
	top_layout->addLayout(main_layout);

	QLabel *label_file = new QLabel("Source file:", this);
	label_file->setToolTip("Function (especial static) path");
	QLabel *label_name = new QLabel("Function:", this);
	m_file = new QLineEdit(this);
	m_func = new QLineEdit(this);
	m_browse = new QPushButton("Browse...", this);

	main_layout->addWidget(label_file, 0, 0, 1, 1);
	main_layout->addWidget(m_file, 0, 1, 1, 1);
	main_layout->addWidget(m_browse, 0, 2, 1, 1);
	main_layout->addWidget(label_name, 1, 0, 1, 1);
	main_layout->addWidget(m_func, 1, 1, 1, 1);

	top_layout->addSpacing(100);

	/* Button layout
	 * -------------------------------
	 *             |   OK  |  Cancel |
	 * -------------------------------*/
	QHBoxLayout *btn_layout = new QHBoxLayout(this);
	top_layout->addLayout(btn_layout);

	m_ok = new QPushButton("OK", this);
	m_cancel = new QPushButton("Cancel", this);

	btn_layout->addWidget(m_ok);
	btn_layout->addWidget(m_cancel);
	btn_layout->setAlignment(Qt::AlignRight);

	setFixedWidth(700);

	m_func->setFocus();

	m_result_ok = false;
	m_result_file = "";
	m_result_func = "";

	conn = connect(m_browse, SIGNAL(pressed()), this, SLOT(slotBtnBrowse()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_ok, SIGNAL(pressed()), this, SLOT(slotBtnOk()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_cancel, SIGNAL(pressed()), this, SLOT(slotBtnCancel()));
	UTER_ASSERT(conn != NULL);
	return;
}

void NewFunBrkDlg::slotBtnOk()
{
	m_result_file = m_file->text();
	m_result_func = m_func->text();
	if (m_result_func == "") {
		m_result_ok = false;
	} else {
		m_result_ok = true;
	}
	close();
	return;
}

void NewFunBrkDlg::slotBtnCancel()
{
	m_result_ok = false;
	close();
	return;
}

void NewFunBrkDlg::slotBtnBrowse()
{
	QString getpath = QFileDialog::getOpenFileName(this, "Function (especial static) in file.");
	if (! getpath.isEmpty()) {
		m_file->setText(getpath);
	}
	return;
}

/*Remove breakpoint*/
int BrkPnt::modDel(BrkPntDock *win, TargetMod *moditem)
{
	UTER_ASSERT(win != NULL);
	UTER_ASSERT(moditem != NULL);

	QList<BrkApply*>::iterator iter;
	for (iter = m_apply_list.begin(); iter != m_apply_list.end(); /*none*/) {
		BrkApply *apply = *iter;
		/*Check apply in module*/
		if (moditem->m_addr_range.check(apply->m_addr)) {
			m_apply_list.erase(iter++);		/*Detach apply*/
			apply->m_ref_count--;

			if (apply->m_ref_count == 0) {
				/*Release apply*/
				UTER_DEBUG("Remove new breakpoint from 0x%016llx.", apply->m_addr);

				QMap<unsigned long long, BrkApply*>::iterator iter_apply;
				iter_apply = win->m_apply_map.find(apply->m_addr);
				UTER_ASSERT(iter_apply != win->m_apply_map.end());
				win->m_apply_map.erase(iter_apply);
				g_target->breakDelete(apply->m_addr, TargetBreak::BREAK_DBG);
				delete apply;
			} else {
				UTER_DEBUG("Reduce reference breakpoint from 0x%016llx", apply->m_addr);
			}
		} else {
			iter++;
		}
	}
	return 0;
}

int FileBrkPnt::modAdd(BrkPntDock *win, TargetMod *moditem)
{
	UTER_ASSERT(win != NULL);
	UTER_ASSERT(moditem != NULL);
	int ret_val = 0;

	QMap<QString, DwfCu*>::iterator iter_cu;
	for (iter_cu = moditem->m_dwarf_cu_map.begin(); iter_cu != moditem->m_dwarf_cu_map.end(); iter_cu++) {
		DwfCu *cu = iter_cu.value();

		/*Check file use in cu*/
		QMap<QString, unsigned int>::iterator iter_file;
		iter_file = cu->m_file_path_map.find(m_file);
		if (iter_file == cu->m_file_path_map.end()) {
			continue;
		}
		unsigned int fileno = iter_file.value();

		/*Locate line in cu*/
		QList<DwfLine*>::iterator iter_line;
		for (iter_line = cu->m_dwf_line_list.begin();
			 iter_line != cu->m_dwf_line_list.end(); iter_line++) {
			DwfLine *dwfline = *iter_line;
			if (dwfline->m_fileno != fileno) {
				continue;
			}
			if (dwfline->m_lineno < m_line) {
				/*Locate the first line number greater than or equal to double click line*/
				continue;
			}

			/*First line found*/
			unsigned long long brkaddr = moditem->m_add_mkup + dwfline->m_lineaddr;
			UTER_DEBUG("Locate file line %s:%d 0x%016llx, break at 0x%016llx.",
						cu->getFileNameByNo(dwfline->m_fileno).toStdString().c_str(), dwfline->m_lineno,
						dwfline->m_lineaddr, brkaddr);

			/*Apply breakpoint*/
			QMap<unsigned long long, BrkApply*>::iterator iter_apply;
			iter_apply = win->m_apply_map.find(brkaddr);
			if (iter_apply == win->m_apply_map.end()) {
				BrkApply *apply = new BrkApply();
				apply->m_addr = brkaddr;
				apply->m_ref_count++;
				m_apply_list.push_back(apply);

				UTER_DEBUG("Apply new breakpoint to 0x%016llx, from file %s:%d.",
							brkaddr, m_file.toStdString().c_str(), m_line);
				/*Save apply*/
				win->m_apply_map.insert(apply->m_addr, apply);
				g_target->breakInsert(apply->m_addr, TargetBreak::BREAK_DBG);
				ret_val = 2;
			} else {
				UTER_DEBUG("Add reference breakpoint to 0x%016llx, from file %s:%d.",
							brkaddr, m_file.toStdString().c_str(), m_line);
				BrkApply *apply = iter_apply.value();
				apply->m_ref_count++;
				m_apply_list.push_back(apply);
				ret_val = ret_val <= 1 ? 1 : ret_val;
			}
			break;	/*Only one function match in a cu*/
		}
	}

	/*Update margin of editor*/
	UEditor *editor = g_mainwin->editGetByPath(m_file);
	if (editor != NULL) {
		if (m_apply_list.isEmpty()) {
			editor->brkpntSet(m_line, false);
		} else {
			editor->brkpntSet(m_line, true);
		}
	}

	return ret_val;
}

int FileBrkPnt::modDel(BrkPntDock *win, TargetMod *moditem)
{
	int ret_val = BrkPnt::modDel(win, moditem);

	/*Update margin icon of editor*/
	UEditor *editor = g_mainwin->editGetByPath(m_file);
	if (editor != NULL) {
		if (m_apply_list.isEmpty()) {
			editor->brkpntSet(m_line, false);
		} else {
			editor->brkpntSet(m_line, true);
		}
	}

	return ret_val;
}

FileBrkPnt::~FileBrkPnt()
{
	/*Erase margin icon of editor*/
	UEditor *editor = g_mainwin->editGetByPath(m_file);
	if (editor != NULL) {
		editor->brkpntUnSet(m_line);
	}
	return;
}

int FuncBrkPnt::modAdd(BrkPntDock *win, TargetMod *moditem)
{
	UTER_ASSERT(win != NULL);
	UTER_ASSERT(moditem != NULL);
	int ret_val = 0;

	QMap<QString, DwfCu*>::iterator iter_cu;
	for (iter_cu = moditem->m_dwarf_cu_map.begin(); iter_cu != moditem->m_dwarf_cu_map.end(); iter_cu++) {
		DwfCu *cu = iter_cu.value();

		QList<DwfFun*>::iterator iter_func;
		for (iter_func = cu->m_funs.begin(); iter_func != cu->m_funs.end(); iter_func++) {
			DwfFun *fun = *iter_func;
			if (m_file == "") {
				/*No source file name, ignore static function*/
				if (! fun->m_extern) {
					continue;
				}
			} else {
				/*Check file name, so get file name first*/
				QString filename = cu->getFileNameByNo(fun->m_fileno);
				int result = filename.lastIndexOf(m_file);	/*End check*/
				if (result == -1) {
					continue;
				}
				if (result + fun->m_name.length() != filename.length()) {
					continue;
				}
			}

			/*Check function name*/
			if (fun->m_name != m_fun) {
				continue;
			}

			/*Function breakpoint at second statement, first statement is {*/
			QList<DwfLine*>::iterator line_iter;
			for (line_iter = cu->m_dwf_line_list.begin();
				 line_iter != cu->m_dwf_line_list.end(); line_iter++) {
				DwfLine *line = *line_iter;
				if (line->m_lineaddr >= fun->m_add_range.m_begin) {
					/*Get the first statement in function*/
					break;
				}
			}
			if (line_iter == cu->m_dwf_line_list.end()) {
				/*No first statement*/
				UTER_LOG("No any statement for function %s.", m_fun.toStdString().c_str());
				continue;
			}
			unsigned long long brkaddr;
			DwfLine *first_line = *line_iter;
			if (! fun->m_add_range.check(first_line->m_lineaddr)) {
				UTER_LOG("First statement at 0x%016llx out of "
						"function %s with range (%016llx-0x%016llx).",
						first_line->m_lineaddr, m_fun.toStdString().c_str(),
						fun->m_add_range.m_begin, fun->m_add_range.m_offset);
				continue;
			}
			line_iter++;
			if (line_iter == cu->m_dwf_line_list.end()) {
				/*No second statement, use first line instead*/
				brkaddr = moditem->m_add_mkup + first_line->m_lineaddr;
				UTER_LOG("No second statement for function %s, "
						 "use first statement address 0x%016llx, break at 0x%016llx.",
						 m_fun.toStdString().c_str(), first_line->m_lineaddr, brkaddr);
			} else {
				/*Have second statement*/
				DwfLine *second_line = *line_iter;
				if (! fun->m_add_range.check(second_line->m_lineaddr)) {
					/*Second statement out of function, use first line instead*/
					brkaddr = moditem->m_add_mkup + first_line->m_lineaddr;
					UTER_LOG("Second statement out of function %s with range (%016llx-0x%016llx), "
							"use first statement address 0x%016llx, break at 0x%016llx.",
							m_fun.toStdString().c_str(),
							fun->m_add_range.m_begin, fun->m_add_range.m_offset,
							first_line->m_lineaddr, brkaddr);
				} else {
					/*OK*/
					brkaddr = moditem->m_add_mkup + second_line->m_lineaddr;
					UTER_DEBUG("Set breakpoint for function %s, "
							   "at second statement address 0x%016llx, break at 0x%016llx.",
							    m_fun.toStdString().c_str(), second_line->m_lineaddr, brkaddr);
				}
			}

			/*Apply breakpoint*/
			QMap<unsigned long long, BrkApply*>::iterator iter_apply;
			iter_apply = win->m_apply_map.find(brkaddr);
			if (iter_apply == win->m_apply_map.end()) {
				BrkApply *apply = new BrkApply();
				apply->m_addr = brkaddr;
				apply->m_ref_count++;
				m_apply_list.push_back(apply);

				UTER_DEBUG("Apply new breakpoint to 0x%016llx, from function %s.",
							brkaddr, m_fun.toStdString().c_str());
				/*Save apply*/
				win->m_apply_map.insert(apply->m_addr, apply);
				g_target->breakInsert(apply->m_addr, TargetBreak::BREAK_DBG);
				ret_val = 2;
			} else {
				UTER_DEBUG("Add reference breakpoint to 0x%016llx, from function %s.",
							brkaddr, m_fun.toStdString().c_str());
				BrkApply *apply = iter_apply.value();
				apply->m_ref_count++;
				m_apply_list.push_back(apply);
				ret_val = ret_val <= 1 ? 1 : ret_val;
			}
			break;	/*Only one function match in a cu*/
		}
	}
	return ret_val;
}

enum BrkPntColumn {
	COLUMM_NAME = 0,
	COLUMM_APPLY,
	COLUMM_MAX,
};

static const char *g_brkpnt_view_column[] = {
	"Name",
	"Apply",
};

UTER_ARRAY_CHECK(g_brkpnt_view_column, COLUMM_MAX);

BrkPntDock::BrkPntDock()
:WidgetWithDock(SEQ_BRKPNT, Qt::BottomDockWidgetArea, "BreakPoint")
{
	QMetaObject::Connection conn;

	/* Out layout
	 * ----------------------------------
	 *  tool
	 * ----------------------------------
	 *  list
	 * ----------------------------------*/
	QVBoxLayout *top_layout = new QVBoxLayout(this);
	setLayout(top_layout);

	/* Button layout
	 * ----------------------------------
	 * xxxx |  xxxx | xxxx | xxxx |
	 * ----------------------------------*/
	QHBoxLayout *btn_layout = new QHBoxLayout(this);
	top_layout->addLayout(btn_layout);

	QPushButton *btn_addfun = new QPushButton("", this);
	btn_addfun->setIcon(QIcon(":/images/brkpnt_fun.png"));
	btn_addfun->setToolTip("Add breakpoint");
	QPushButton *btn_del = new QPushButton("", this);
	btn_del->setIcon(QIcon(":/images/com_del.png"));
	btn_del->setToolTip("Delete breakpoint");
	QPushButton *btn_delall = new QPushButton("", this);
	btn_delall->setIcon(QIcon(":/images/com_delall.png"));
	btn_delall->setToolTip("Delete all breakpoint");

	btn_layout->addWidget(btn_addfun);
	btn_layout->addWidget(btn_del);
	btn_layout->addWidget(btn_delall);
	btn_layout->setAlignment(Qt::AlignLeft);

	/*List layout*/
	m_brkpnt_show = new QTableWidget(this);
	top_layout->addWidget(m_brkpnt_show);

	/*Set icon size*/
	m_brkpnt_show->setIconSize(QSize(15, 15));
	/*Cannot edit*/
	m_brkpnt_show->setEditTriggers(QAbstractItemView::NoEditTriggers);
	/*Line select*/
	m_brkpnt_show->setSelectionBehavior(QAbstractItemView::SelectRows);
	/*Single select*/
	m_brkpnt_show->setSelectionMode(QAbstractItemView::SingleSelection);
	/*Close caption highlight*/
	m_brkpnt_show->horizontalHeader()->setHighlightSections(false);
	/*No index 1,2,3,4....*/
	m_brkpnt_show->verticalHeader()->setVisible(false);

	/*Set caption*/
	m_brkpnt_show->setColumnCount(COLUMM_MAX);
	QStringList header;
	int iloop;
	for (iloop = 0; iloop < COLUMM_MAX; iloop++) {
		header << g_brkpnt_view_column[iloop];
	}
	m_brkpnt_show->setHorizontalHeaderLabels(header);
	m_brkpnt_show->resizeColumnsToContents();

	/*Some icon*/
	m_icon_brkpnt_enable = QIcon(":/images/brkpnt_enable.png");
	m_icon_brkpnt_disable = QIcon(":/images/brkpnt_disable.png");

	conn = connect(btn_addfun, SIGNAL(pressed()), this, SLOT(slotAddFunPressed()));
	UTER_ASSERT(conn != NULL);
	conn = connect(btn_del, SIGNAL(pressed()), this, SLOT(slotDelPressed()));
	UTER_ASSERT(conn != NULL);
	conn = connect(btn_delall, SIGNAL(pressed()), this, SLOT(slotDelAllPressed()));
	UTER_ASSERT(conn != NULL);
	/*Target event*/
	conn = connect(g_target, SIGNAL(signalModAdd(TargetMod*)), this, SLOT(slotModAdd(TargetMod*)));
	UTER_ASSERT(conn != NULL);
	conn = connect(g_target, SIGNAL(signalModDel(TargetMod*)), this, SLOT(slotModDel(TargetMod*)));
	UTER_ASSERT(conn != NULL);
	/*File open event*/
	conn = connect(g_mainwin, SIGNAL(signalEditFileOpen(UEditor*)), this, SLOT(slotEditFileOpen(UEditor*)));
	UTER_ASSERT(conn != NULL);
	/*Margin double click event*/
	conn = connect(g_mainwin, SIGNAL(signalEditMarginDClicked(UEditor*, unsigned int)),
			       this, SLOT(slotMarginDoubleClicked(UEditor*, unsigned int)));
	UTER_ASSERT(conn != NULL);

	/*Stop on main*/
	FuncBrkPnt *brkpnt = new FuncBrkPnt("", "main");
	usrBrkAdd(brkpnt);
	m_brkpnt_list.push_back((BrkPnt *)brkpnt);

	g_mainwin->dockSetup(this);
	return;
}

void BrkPntDock::slotAddFunPressed()
{
	NewFunBrkDlg newfile(this);
	newfile.exec();
	if (! newfile.m_result_ok) {
		return;
	}

	FuncBrkPnt *brkpnt = new FuncBrkPnt(newfile.m_result_file, newfile.m_result_func);
	usrBrkAdd(brkpnt);
	m_brkpnt_list.push_back((BrkPnt *)brkpnt);
	return;
}

void BrkPntDock::slotDelPressed()
{
	int cur_row = m_brkpnt_show->currentRow();
	if (cur_row < 0) {
		return;
	}
	QTableWidgetItem *item = m_brkpnt_show->item(cur_row, COLUMM_NAME);
	if (item == NULL) {
		return;
	}
	BrkPnt *cur_brkpnt = (BrkPnt *)item->data(Qt::UserRole).value<void*>();

	/*Search breakpoint*/
	QList<BrkPnt*>::iterator iter;
	for (iter = m_brkpnt_list.begin(); iter != m_brkpnt_list.end(); iter++) {
		BrkPnt *brkpnt = *iter;
		if (cur_brkpnt == brkpnt) {
			m_brkpnt_list.erase(iter);
			usrBrkDel(brkpnt);
			delete brkpnt;
			break;
		}
	}
	return;
}

void BrkPntDock::slotDelAllPressed()
{
	QList<BrkPnt*>::iterator iter;
	for (iter = m_brkpnt_list.begin(); iter != m_brkpnt_list.end(); /*none*/) {
		BrkPnt *brkpnt = *iter;
		m_brkpnt_list.erase(iter++);
		usrBrkDel(brkpnt);
		delete brkpnt;
	}
	return;
}

/*Add module*/
void BrkPntDock::slotModAdd(TargetMod *moditem)
{
	UTER_ASSERT(moditem != NULL);
	/*Foreach all breakpoint*/
	QList<BrkPnt*>::iterator iter;
	for (iter = m_brkpnt_list.begin(); iter != m_brkpnt_list.end(); iter++) {
		BrkPnt *brkpnt = *iter;
		brkpnt->modAdd(this, moditem);
		flushIcon(brkpnt);
	}
	return;
}

/*Remove module*/
void BrkPntDock::slotModDel(TargetMod *moditem)
{
	UTER_ASSERT(moditem != NULL);
	/*Foreach all breakpoint*/
	QList<BrkPnt*>::iterator iter;
	for (iter = m_brkpnt_list.begin(); iter != m_brkpnt_list.end(); iter++) {
		BrkPnt *brkpnt = *iter;
		brkpnt->modDel(this, moditem);
		flushIcon(brkpnt);
	}
	return;
}

void BrkPntDock::slotEditFileOpen(UEditor *edit)
{
	UTER_ASSERT(edit != NULL);
	QList<BrkPnt*>::iterator iter;
	for (iter = m_brkpnt_list.begin(); iter != m_brkpnt_list.end(); iter++) {
		BrkPnt *brkpnt = *iter;
		if (brkpnt->m_type == BrkPnt::TYPE_FILE) {
			FileBrkPnt *filebrkpnt = (FileBrkPnt *)brkpnt;
			if (filebrkpnt->m_file != edit->m_file_path) {
				continue;
			}
			/*Show breakpoint in edit*/
			if (filebrkpnt->m_apply_list.isEmpty()) {
				edit->brkpntSet(filebrkpnt->m_line, false);
			} else {
				edit->brkpntSet(filebrkpnt->m_line, true);
			}
		}
	}

	return;
}

void BrkPntDock::slotMarginDoubleClicked(UEditor *edit, unsigned int lineno)
{
	UTER_ASSERT(edit != NULL);
	/*Check breakpoint exist*/
	QList<BrkPnt*>::iterator iter;
	for (iter = m_brkpnt_list.begin(); iter != m_brkpnt_list.end(); iter++) {
		BrkPnt *brkpnt = *iter;
		if (brkpnt->m_type == BrkPnt::TYPE_FILE) {
			FileBrkPnt *filebrkpnt = (FileBrkPnt *)brkpnt;
			if (filebrkpnt->m_file == edit->m_file_path
				&& filebrkpnt->m_line == lineno) {
				break;
			}
		}
	}
	if (iter == m_brkpnt_list.end()) {
		/*No exist breakpoint, add new breakpoint*/
		FileBrkPnt *brkpnt = new FileBrkPnt(edit->m_file_path, lineno);
		usrBrkAdd(brkpnt);
		m_brkpnt_list.push_back((BrkPnt *)brkpnt);
		return;
	} else {
		/*Exist breakpoint, remove the breakpoint*/
		FileBrkPnt *brkpnt = (FileBrkPnt *)(*iter);
		m_brkpnt_list.erase(iter);
		usrBrkDel(brkpnt);
		delete brkpnt;
		return;
	}

	return;
}

int BrkPntDock::usrBrkAdd(BrkPnt *brkpnt)
{
	UTER_ASSERT(brkpnt != NULL);
	bool changed = false;
	/*Foreach module*/
	QMap<QString, TargetMod*>::iterator iter;
	for (iter = g_target->m_mod_items.begin(); iter != g_target->m_mod_items.end(); iter++) {
		TargetMod *moditem = iter.value();
		int result = brkpnt->modAdd(this, moditem);
		if (result == 2) {
			changed = true;
		}
		flushIcon(brkpnt);
	}
	if (changed) {
		g_target->breakChanged();
	}
	int rowcount = m_brkpnt_show->rowCount();
	m_brkpnt_show->setRowCount(rowcount + 1);
	m_brkpnt_show->setItem(rowcount, COLUMM_NAME, brkpnt->m_name_item);
	m_brkpnt_show->setItem(rowcount, COLUMM_APPLY, brkpnt->m_apply_item);
	m_brkpnt_show->resizeColumnsToContents();
	return 0;
}

int BrkPntDock::usrBrkDel(BrkPnt *brkpnt)
{
	UTER_ASSERT(brkpnt != NULL);
	bool changed = false;
	/*Foreach module*/
	QMap<QString, TargetMod*>::iterator iter;
	for (iter = g_target->m_mod_items.begin(); iter != g_target->m_mod_items.end(); iter++) {
		TargetMod *moditem = iter.value();
		int result = brkpnt->modDel(this, moditem);
		if (result == 2) {
			changed = true;
		}
		flushIcon(brkpnt);
	}
	if (changed) {
		g_target->breakChanged();
	}
	int row = brkpnt->m_name_item->row();
	m_brkpnt_show->takeItem(row, COLUMM_NAME);
	m_brkpnt_show->takeItem(row, COLUMM_APPLY);
	m_brkpnt_show->removeRow(row);
	m_brkpnt_show->resizeColumnsToContents();
	return 0;
}

int BrkPntDock::flushIcon(BrkPnt *brkpnt)
{
	UTER_ASSERT(brkpnt != NULL);
	/*Icon of breakpoint on list*/
	if (brkpnt->m_apply_list.isEmpty()) {
		brkpnt->m_name_item->setIcon(m_icon_brkpnt_disable);
		brkpnt->m_apply_item->setText("");
		brkpnt->m_apply_item->setToolTip("");
	} else {
		brkpnt->m_name_item->setIcon(m_icon_brkpnt_enable);
		brkpnt->m_apply_item->setText(QString::asprintf("%d", brkpnt->m_apply_list.length()));
		QString tip;
		QList<BrkApply*>::iterator iter;
		for (iter = brkpnt->m_apply_list.begin(); iter != brkpnt->m_apply_list.end(); iter++) {
			BrkApply *apply = *iter;
			if (iter != brkpnt->m_apply_list.begin()) {
				tip += " | ";
			}
			tip += g_target->arch0xAddr(apply->m_addr);
		}
		brkpnt->m_apply_item->setToolTip(tip);
	}
	m_brkpnt_show->resizeColumnsToContents();
	return 0;
}

} /*namespace BrkPntUi*/
