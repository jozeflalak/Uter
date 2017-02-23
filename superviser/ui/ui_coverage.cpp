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

namespace CovUi {

enum TextViewColumn {
	TEXTVIEW_COLUMM_ELEMENT = 0,
	TEXTVIEW_COLUMM_COVERAGE,
	TEXTVIEW_COLUMM_LINES,
	TEXTVIEW_COLUMM_PERCENT,
	TEXTVIEW_COLUMM_MAX,
};

static const char *g_text_view_column[] = {
	"Element",
	"Coveraged",
	"Statements",
	"Percent",
};

UTER_ARRAY_CHECK(g_text_view_column, TEXTVIEW_COLUMM_MAX);

enum ProgViewColumn {
	PROGVIEW_COLUMM_ELEMENT = 0,
	PROGVIEW_COLUMM_MAX,
};

static const char *g_prog_view_column[] = {
	"Element",
};

UTER_ARRAY_CHECK(g_prog_view_column, PROGVIEW_COLUMM_MAX);

enum ProgTableColumn {
	PROGDETAIL_COLUMM_FILE = 0,
	PROGDETAIL_COLUMM_NAME,
	PROGDETAIL_COLUMM_COVERAGE,
	PROGDETAIL_COLUMM_LINES,
	PROGDETAIL_COLUMM_PERCENT,
	PROGDETAIL_COLUMM_LINENO,
	PROGDETAIL_COLUMM_MAX,
};

static const char *g_prog_table_column[] = {
	"File",
	"Function",
	"Coveraged",
	"Statements",
	"Percent",
	"Line No.",
};

UTER_ARRAY_CHECK(g_prog_table_column, PROGDETAIL_COLUMM_MAX);

enum CovMark {
	MARKS_COV_YES = 1,
	MARKS_COV_PART = 2,
	MARKS_COV_NO = 3,
};

CovDock::CovDock()
:WidgetWithDock(SEQ_COV, Qt::BottomDockWidgetArea, "Coverage")
{
	QMetaObject::Connection conn;

	/* Out layout
	 * --------------------------------------
	 * |      tools
	 * --------------------------------------
	 * | Text View | Program View
	 * --------------------------------------
	 * |
	 * --------------------------------------*/
	QVBoxLayout *out_layout = new QVBoxLayout(this);
	setLayout(out_layout);

	QHBoxLayout *tool_layout = new QHBoxLayout(this);
	out_layout->addLayout(tool_layout);
	tool_layout->setAlignment(Qt::AlignLeft);

	m_top_tab = new QTabWidget(this);
	out_layout->addWidget(m_top_tab);

	/* Tool layout
	 * --------------------------------------
	 * On | Off | Clear | Update | .......
	 * --------------------------------------*/
	m_start_btn = new QPushButton(QIcon(":/images/cov_on.png"), "", this);
	m_start_btn->setToolTip("Start file coverage");
	m_stop_btn = new QPushButton(QIcon(":/images/cov_off.png"), "", this);
	m_stop_btn->setToolTip("Stop file coverage");
	m_clear_btn = new QPushButton(QIcon(":/images/cov_clear.png"), "", this);
	m_clear_btn->setToolTip("Clear file coverage");
	m_update_btn = new QPushButton(QIcon(":/images/cov_update.png"), "", this);
	m_update_btn->setToolTip("Show file coverage");

	m_show_check = new QCheckBox("Show in editor", this);

	tool_layout->addWidget(m_start_btn);
	tool_layout->addWidget(m_stop_btn);
	tool_layout->addWidget(m_clear_btn);
	tool_layout->addWidget(m_update_btn);
	tool_layout->addWidget(m_show_check);

	m_start_btn->setEnabled(true);
	m_stop_btn->setEnabled(false);
	m_clear_btn->setEnabled(false);
	m_update_btn->setEnabled(false);
	m_show_check->setEnabled(false);
	m_show_check->setChecked(true);

	m_starting = false;
	m_showing = true;

	/* Text View Layout
	 * -----------------------------------
	 *  Object tree   |    Text view
	 * -----------------------------------*/
	QSplitter *textview_split = new QSplitter(Qt::Horizontal, this);
	m_top_tab->addTab(textview_split, "Text view");
	m_text_view = textview_split;

	QWidget *text_left_widget = new QWidget(this);
	textview_split->addWidget(text_left_widget);
	textview_split->setStretchFactor(0, 60);

	QWidget *text_right_widget = new QWidget(this);
	textview_split->addWidget(text_right_widget);
	textview_split->setStretchFactor(1, 40);

	/*Left - Text coverage object tree*/
	QVBoxLayout *text_left_layout = new QVBoxLayout(this);
	text_left_widget->setLayout(text_left_layout);
	m_text_cov_tree = new QTreeWidget(this);
	text_left_layout->addWidget(m_text_cov_tree);
	QStringList labels;
	int iloop;
	for (iloop = 0; iloop < TEXTVIEW_COLUMM_MAX; iloop++) {
		labels << g_text_view_column[iloop];
	}
	m_text_cov_tree->setHeaderLabels(labels);

	/*Right - Text coverage text view*/
	QVBoxLayout *text_right_layout = new QVBoxLayout(this);
	text_right_widget->setLayout(text_right_layout);
	QLabel *label = new QLabel("View coverage in this range:", this);
	text_right_layout->addWidget(label);
	m_text_cov_edit = new EditorBase(EditorBase::FILETYPE_CPP, this);
	text_right_layout->addWidget(m_text_cov_edit);
	/*Read only*/
	m_text_cov_edit->setReadOnly(true);
	/*Margin for line number*/
	m_text_cov_edit->setMarginType(0, QsciScintilla::NumberMargin);
	m_text_cov_edit->setMarginWidth(0, 40);
	m_text_cov_edit->setMarginMarkerMask(0, 0);
	/*Marker for background without margin*/
	m_text_cov_edit->markerDefine(QsciScintilla::Background, MARKS_COV_YES);
	m_text_cov_edit->setMarkerBackgroundColor(EditorCpp::m_cov_color_yes, MARKS_COV_YES);
	m_text_cov_edit->markerDefine(QsciScintilla::Background, MARKS_COV_PART);
	m_text_cov_edit->setMarkerBackgroundColor(EditorCpp::m_cov_color_part, MARKS_COV_PART);
	m_text_cov_edit->markerDefine(QsciScintilla::Background, MARKS_COV_NO);
	m_text_cov_edit->setMarkerBackgroundColor(EditorCpp::m_cov_color_no, MARKS_COV_NO);

	/* Program View Layout
	 * -------------------------------------
	 *  Object tree   | Coverage table
	 * -------------------------------------*/
	QSplitter *progview_split = new QSplitter(Qt::Horizontal, this);
	m_top_tab->addTab(progview_split, "Program view");
	m_prog_view = progview_split;

	QWidget *prog_left_widget = new QWidget(this);
	progview_split->addWidget(prog_left_widget);
	progview_split->setStretchFactor(0, 50);

	QWidget *prog_rigth_widget = new QWidget(this);
	progview_split->addWidget(prog_rigth_widget);
	progview_split->setStretchFactor(1, 50);

	/*Left - Program coverage object tree*/
	QVBoxLayout *prog_left_layout = new QVBoxLayout(this);
	prog_left_widget->setLayout(prog_left_layout);
	m_prog_cov_tree = new QTreeWidget(this);
	prog_left_layout->addWidget(m_prog_cov_tree);
	labels.clear();
	for (iloop = 0; iloop < PROGVIEW_COLUMM_MAX; iloop++) {
		labels << g_prog_view_column[iloop];
	}
	m_prog_cov_tree->setHeaderLabels(labels);

	/*Right - Program covearge table*/
	QVBoxLayout *prog_rigth_layout = new QVBoxLayout(this);
	prog_rigth_widget->setLayout(prog_rigth_layout);
	m_prog_detail_tree = new QTreeWidget(this);
	prog_rigth_layout->addWidget(m_prog_detail_tree);
	labels.clear();
	for (iloop = 0; iloop < PROGDETAIL_COLUMM_MAX; iloop++) {
		labels << g_prog_table_column[iloop];
	}
	m_prog_detail_tree->setHeaderLabels(labels);

	/*Tool signal*/
	conn = connect(m_start_btn, SIGNAL(pressed()), this, SLOT(slotStartPressed()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_stop_btn, SIGNAL(pressed()), this, SLOT(slotStopPressed()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_clear_btn, SIGNAL(pressed()), this, SLOT(slotClearPressed()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_update_btn, SIGNAL(pressed()), this, SLOT(slotUpdatePressed()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_show_check, SIGNAL(stateChanged(int)), this, SLOT(slotCheckStateChanged(int)));
	UTER_ASSERT(conn != NULL);

	conn = connect(m_text_cov_tree, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(slotTextTreeItemClicked(QTreeWidgetItem*, int)));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_prog_cov_tree, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(slotProgTreeItemClicked(QTreeWidgetItem*, int)));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_prog_detail_tree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(slotProgTableItemClicked(QTreeWidgetItem*, int)));
	UTER_ASSERT(conn != NULL);

    /*Menu and toolbar*/
	UiMenuSetup *menu = g_mainwin->menuSetup(UiMenuSetup::SEQ_COV, "&Coverage");
	UiToolBarSetup *toolbar = g_mainwin->toolBarSetup(UiToolBarSetup::SEQ_COV, "&Coverage");

	m_prev_no_act = new QAction("&Previous no coverage", this);
	m_prev_no_act->setStatusTip("Previous no coverage");
	m_prev_no_act->setIcon(QIcon(":/images/cov_prev_no.png"));
	menu->addAction(m_prev_no_act);
	toolbar->addAction(m_prev_no_act);

    m_next_no_act = new QAction("&Next no coverage", this);
    m_next_no_act->setStatusTip("Next no coverage");
    m_next_no_act->setIcon(QIcon(":/images/cov_next_no.png"));
	menu->addAction(m_next_no_act);
	toolbar->addAction(m_next_no_act);

	m_prev_part_act = new QAction("&Previous part coverage", this);
	m_prev_part_act->setStatusTip("Previous part coverage");
	m_prev_part_act->setIcon(QIcon(":/images/cov_prev_part.png"));
	menu->addAction(m_prev_part_act);
	toolbar->addAction(m_prev_part_act);

    m_next_part_act = new QAction("&Next part coverage", this);
    m_next_part_act->setStatusTip("Next part coverage");
    m_next_part_act->setIcon(QIcon(":/images/cov_next_part.png"));
	menu->addAction(m_next_part_act);
	toolbar->addAction(m_next_part_act);

	/*Menu & toolbar event*/
    conn = connect(m_prev_no_act, SIGNAL(triggered()), this, SLOT(slotCovNoPrevPressed()));
    UTER_ASSERT(conn != NULL);
    conn = connect(m_next_no_act, SIGNAL(triggered()), this, SLOT(slotCovNoNextPressed()));
    UTER_ASSERT(conn != NULL);
    conn = connect(m_prev_part_act, SIGNAL(triggered()), this, SLOT(slotCovPartPrevPressed()));
    UTER_ASSERT(conn != NULL);
    conn = connect(m_next_part_act, SIGNAL(triggered()), this, SLOT(slotCovPartNextPressed()));
    UTER_ASSERT(conn != NULL);

	/*Target event*/
	conn = connect(g_target, SIGNAL(signalModDel(TargetMod*)), this, SLOT(slotModDel(TargetMod*)));
	UTER_ASSERT(conn != NULL);
	conn = connect(g_target, SIGNAL(signalModAdd(TargetMod*)), this, SLOT(slotModAdd(TargetMod*)));
	UTER_ASSERT(conn != NULL);
	/*Project event*/
	conn = connect(g_mainwin->m_prj_win, SIGNAL(signalAddPrjFile(QString)), this, SLOT(slotAddPrjFile(QString)));
	UTER_ASSERT(conn != NULL);
	conn = connect(g_mainwin->m_prj_win, SIGNAL(signalDelPrjFile(QString)), this, SLOT(slotDelPrjFile(QString)));
	UTER_ASSERT(conn != NULL);
	/*File open event*/
	conn = connect(g_mainwin, SIGNAL(signalEditFileOpen(UEditor*)), this, SLOT(slotEditFileOpen(UEditor*)));
	UTER_ASSERT(conn != NULL);

    g_mainwin->dockSetup(this);
	return;
}

CovDock::~CovDock()
{
	QMap<QString, TextCovFile*>::iterator file_iter;
	for (file_iter = m_files.begin(); file_iter != m_files.end(); /*none*/) {
		TextCovFile *txtfile = file_iter.value();
		delete txtfile;
		m_files.erase(file_iter++);
	}

	QMap<unsigned long long, ApplyCov*>::iterator apply_iter;
	for (apply_iter = m_apply_map.begin(); apply_iter != m_apply_map.end(); /*none*/) {
		ApplyCov *apply = apply_iter.value();
		delete apply;
		m_apply_map.erase(apply_iter++);
	}

	QList<ProgCovMod*>::iterator progcov_iter;
	for (progcov_iter = m_progcov_list.begin(); progcov_iter != m_progcov_list.end(); /*none*/) {
		ProgCovMod *progmod = *progcov_iter;
		delete progmod;
		m_progcov_list.erase(progcov_iter++);
	}

	return;
}

void CovDock::slotStartPressed()
{
	m_starting = true;
	m_start_btn->setEnabled(false);
	m_stop_btn->setEnabled(true);
	m_clear_btn->setEnabled(true);
	m_update_btn->setEnabled(true);
	m_show_check->setEnabled(true);

	/*Attach all source file*/
	QMap<QString, TextCovFile*>::iterator iter;
	for (iter = m_files.begin(); iter != m_files.end(); iter++) {
		TextCovFile *covfile = iter.value();
		fileAttach(covfile);
	}

	/*Flush coverage*/
	slotUpdatePressed();
	return;
}

void CovDock::slotStopPressed()
{
	m_starting = false;
	m_start_btn->setEnabled(true);
	m_stop_btn->setEnabled(false);
	m_clear_btn->setEnabled(false);
	m_update_btn->setEnabled(false);
	m_show_check->setEnabled(false);

	/*Detach all source file*/
	QMap<QString, TextCovFile*>::iterator iter;
	for (iter = m_files.begin(); iter != m_files.end(); iter++) {
		TextCovFile *covfile = iter.value();
		fileDetach(covfile);
	}
	UTER_ASSERT(m_apply_map.empty());

	/*Flush with flushEditor, because slotUpdatePressed can not work on disable coverage*/
	flushEditor();
	return;
}

void CovDock::slotClearPressed()
{
	if (! m_starting) {
		return;
	}

	/*Clear all apply token*/
	QMap<unsigned long long, ApplyCov*>::iterator iter;
	for (iter = m_apply_map.begin(); iter != m_apply_map.end(); iter++) {
		ApplyCov *apply = iter.value();
		apply->m_token->m_hit_for_file = 0;
	}

	/*Flush coverage*/
	slotUpdatePressed();
	return;
}

void CovDock::slotUpdatePressed()
{
	if (! m_starting) {
		return;
	}

	/* Coverage breakpoint on the end of token, no any coverage count before breakpoint trigger.
	 * so get current instruction address in token of all threads, to adjust coverage for clean token*/
	QMap<CovToken*, unsigned long long> digintoken;
	QMap<unsigned int, TargetThread*>::iterator thrd_iter;
	for (thrd_iter = g_target->m_thread_set.begin(); thrd_iter != g_target->m_thread_set.end(); thrd_iter++) {
		TargetThread *thrd = thrd_iter.value();
		unsigned long long inst_at;
		int result = g_target->pidRegPcGet(thrd->m_pid, inst_at);
		if (result != 0) {
			continue;
		}
		TargetMod *mod = g_target->modGetByAddrIn(inst_at);
		if (mod == NULL) {
			continue;
		}
		unsigned long long offset_addr = inst_at - mod->m_add_mkup;
		DwfCu *cu = mod->dwarfGetCuByAddrIn(offset_addr);
		if (cu == NULL) {
			continue;
		}
		QList<DwfFun*>::iterator fun_iter;
		for (fun_iter = cu->m_funs.begin(); fun_iter != cu->m_funs.end(); fun_iter++) {
			DwfFun *dwffun = *fun_iter;
			if (dwffun->m_add_range.check(offset_addr)) {
				break;
			}
		}
		if (fun_iter == cu->m_funs.end()) {
			break;
		}
		DwfFun *dwffun = *fun_iter;
		CovToken *token = dwffun->m_cov_token.getTokenByAddr(offset_addr);
		if (token == NULL) {
			continue;
		}
		QMap<CovToken*, unsigned long long>::iterator digin_iter;
		digin_iter = digintoken.find(token);
		if (digin_iter == digintoken.end()) {
			digintoken.insert(token, offset_addr);
		} else {
			/*Record farthest location*/
			if (offset_addr > digin_iter.value()) {
				digintoken.insert(token, offset_addr);
			}
		}
	}

	/*Flush file coverage*/
	QMap<QString, TextCovFile*>::iterator text_iter;
	for (text_iter = m_files.begin(); text_iter != m_files.end(); text_iter++) {
		TextCovFile *covfile = text_iter.value();
		/*Clear old*/
		if (covfile->m_root_covinfo != NULL) {
			delete covfile->m_root_covinfo;
			covfile->m_root_covinfo = NULL;
		}
		/*Get new*/
		covfile->m_root_covinfo = makeTextCovRoot(covfile->m_name, digintoken);
	}

	/*Flush program coverage, clear old first*/
	QList<ProgCovMod *>::iterator prog_iter;
	for (prog_iter = m_progcov_list.begin(); prog_iter != m_progcov_list.end(); /*none*/) {
		ProgCovMod *progmod = *prog_iter;
		delete progmod;
		m_progcov_list.erase(prog_iter++);
	}
	QMap<QString, TargetMod*>::iterator mod_iter;
	for (mod_iter = g_target->m_mod_items.begin(); mod_iter != g_target->m_mod_items.end(); mod_iter++) {
		TargetMod *mod = mod_iter.value();
		ProgCovMod *progmod = makeProgCovMod(mod, digintoken);
		if (progmod == NULL) {
			/*Normal when no debug information module*/
			continue;
		}
		m_progcov_list.push_back(progmod);
	}

	/*Flush to user interface*/
	flushEditor();
	return;
}

void CovDock::slotCheckStateChanged(int state)
{
	if (! m_starting) {
		return;
	}

	if (state == Qt::Checked) {
		m_showing = true;
		flushEditor();				/*Do flush*/
	} else {
		m_showing = false;
		/*Do clear*/
		QMap<QString, TextCovFile*>::iterator iter;
		for (iter = m_files.begin(); iter != m_files.end(); iter++) {
			TextCovFile *covfile = iter.value();
			UEditor *edit = g_mainwin->editGetByPath(covfile->m_name);
			if (edit == NULL) {
				continue;
			}
			edit->fileCovClear();
		}
	}

	return;
}

static int getFindFrom(UEditor *edit)
{
	int lineFrom; int indexFrom; int lineTo; int indexTo;
	edit->getSelection(&lineFrom, &indexFrom, &lineTo, &indexTo);
	if (lineFrom < 0 || indexFrom < 0 || lineTo < 0 || indexTo  < 0) {
		/*No select new, so return cursor at line*/
		int line; int index;
		edit->getCursorPosition(&line, &index);
		if (line < 0 || index < 0) {
			return -1;
		}
		return line;
	} else {
		/*Return select begin*/
		return lineFrom;
	}
}

void CovDock::slotCovNoPrevPressed()
{
	if (! (m_starting && m_showing)) {
		return;
	}

	UEditor *edit = g_mainwin->editGetActive();
	if (edit == NULL) {
		return;
	}
	QMap<QString, TextCovFile*>::iterator iter;
	iter = m_files.find(edit->m_file_path);
	if (iter == m_files.end()) {
		return;
	}

	int line_no = getFindFrom(edit);
	if (line_no < 0) {
		return;
	} else if (line_no == 0) {
		/*Find from the last line*/
		line_no = edit->lines() - 1;
	} else {
		/*Find from the previous line*/
		line_no -= 1;
	}
	int to_line_no = edit->fileCovPrev(line_no, true, false);
	if (to_line_no < 0) {
		return;
	}
	/*Select the find line*/
	edit->setSelection(to_line_no, 0, to_line_no, edit->lineLength(to_line_no));
	edit->ensureLineVisible(line_no);
	return;
}

void CovDock::slotCovNoNextPressed()
{
	if (! (m_starting && m_showing)) {
		return;
	}

	UEditor *edit = g_mainwin->editGetActive();
	if (edit == NULL) {
		return;
	}
	QMap<QString, TextCovFile*>::iterator iter;
	iter = m_files.find(edit->m_file_path);
	if (iter == m_files.end()) {
		return;
	}

	int line_no = getFindFrom(edit);
	if (line_no < 0) {
		return;
	} else if (line_no == edit->lines() - 1) {
		/*Find from the first line*/
		line_no = 0;
	} else {
		/*Find from the next line*/
		line_no += 1;
	}
	int to_line_no = edit->fileCovNext(line_no, true, false);
	if (to_line_no < 0) {
		return;
	}
	/*Select the find line*/
	edit->setSelection(to_line_no, 0, to_line_no, edit->lineLength(to_line_no));
	edit->ensureLineVisible(line_no);
	return;
}

void CovDock::slotCovPartPrevPressed()
{
	if (! (m_starting && m_showing)) {
		return;
	}

	UEditor *edit = g_mainwin->editGetActive();
	if (edit == NULL) {
		return;
	}
	QMap<QString, TextCovFile*>::iterator iter;
	iter = m_files.find(edit->m_file_path);
	if (iter == m_files.end()) {
		return;
	}

	int line_no = getFindFrom(edit);
	if (line_no < 0) {
		return;
	} else if (line_no == 0) {
		/*Find from the last line*/
		line_no = edit->lines() - 1;
	} else {
		/*Find from the previous line*/
		line_no -= 1;
	}
	int to_line_no = edit->fileCovPrev(line_no, true, true);
	if (to_line_no < 0) {
		return;
	}
	/*Select the find line*/
	edit->setSelection(to_line_no, 0, to_line_no, edit->lineLength(to_line_no));
	edit->ensureLineVisible(line_no);
	return;
}

void CovDock::slotCovPartNextPressed()
{
	if (! (m_starting && m_showing)) {
		return;
	}

	UEditor *edit = g_mainwin->editGetActive();
	if (edit == NULL) {
		return;
	}
	QMap<QString, TextCovFile*>::iterator iter;
	iter = m_files.find(edit->m_file_path);
	if (iter == m_files.end()) {
		return;
	}

	int line_no = getFindFrom(edit);
	if (line_no < 0) {
		return;
	} else if (line_no == edit->lines() - 1) {
		/*Find from the first line*/
		line_no = 0;
	} else {
		/*Find from the next line*/
		line_no += 1;
	}
	int to_line_no = edit->fileCovNext(line_no, true, true);
	if (to_line_no < 0) {
		return;
	}
	/*Select the find line*/
	edit->setSelection(to_line_no, 0, to_line_no, edit->lineLength(to_line_no));
	edit->ensureLineVisible(line_no);
	return;
}

TextCovCu *CovDock::makeTextCovCu(DwfCu *cu, QString &file, QMap<CovToken*, unsigned long long> &digintoken)
{
	/*Check file in cu*/
	QMap<QString, unsigned int>::iterator iter_source;
	iter_source = cu->m_file_path_map.find(file);
	if (iter_source == cu->m_file_path_map.end()) {
		return NULL;
	}
	unsigned int fileno = iter_source.value();

	TextCovCu *cucov = new TextCovCu(cu->m_path);

	/*Foreach all statement*/
	QMap<unsigned long long, DwfLine*>::iterator line_iter;
	for (line_iter = cu->m_dwf_line_map.begin();
		 line_iter != cu->m_dwf_line_map.end(); line_iter++) {
		DwfLine *line = line_iter.value();
		if (line->m_fileno != fileno) {
			/*No about this file*/
			continue;
		}
		unsigned long long addr = line_iter.key();	/*Statement address*/
		/*The token which the statement in*/
		QList<DwfFun*>::iterator iter_func;
		for (iter_func = cu->m_funs.begin(); iter_func != cu->m_funs.end(); iter_func++) {
			DwfFun *fun = *iter_func;
			if (fun->m_add_range.check(addr)) {
				break;
			}
		}
		if (iter_func == cu->m_funs.end()) {
			continue;
		}
		DwfFun *fun = *iter_func;
		CovToken *token = fun->m_cov_token.getTokenByAddr(addr);
		if (token == NULL) {
			continue;
		}

		bool cov_hit = token->m_hit_for_file;
		if (! cov_hit) {
			/*Make up when token clean*/
			QMap<CovToken*, unsigned long long>::iterator dig_iter;
			dig_iter = digintoken.find(token);
			if (dig_iter != digintoken.end()) {
				unsigned long long walk_to = dig_iter.value();
				if (addr < walk_to) {
					cov_hit = true;
				}
			}
		}

		/*Get source line*/
		CovLine *cu_covline = NULL;
		QMap<unsigned int, CovLine*>::iterator iter_covline;
		iter_covline = cucov->m_linemap.find(line->m_lineno);
		if (iter_covline == cucov->m_linemap.end()) {
			cu_covline = new CovLine();
			cu_covline->m_line_no = line->m_lineno;
			cucov->m_linemap.insert(cu_covline->m_line_no, cu_covline);
		} else {
			cu_covline = iter_covline.value();
		}

		/*Source line count*/
		cu_covline->m_num++;
		if (cov_hit) {
			cu_covline->m_hit++;
		}
		/*Merge line to cu*/
		cucov->m_count_hit += cu_covline->m_hit;
		cucov->m_count_num += cu_covline->m_num;
	}

	return cucov;
}

/*Text coverage for module (for a source file)*/
TextCovMod *CovDock::makeTextCovMod(TargetMod *mod, QString &file, QMap<CovToken*, unsigned long long> &digintoken)
{
	TextCovMod *modcov = NULL;

	QMap<QString, DwfCu*>::iterator iter_cu;
	for (iter_cu = mod->m_dwarf_cu_map.begin(); iter_cu != mod->m_dwarf_cu_map.end(); iter_cu++) {
		DwfCu *cu = iter_cu.value();

		TextCovCu *cucov = makeTextCovCu(cu, file, digintoken);
		if (cucov == NULL) {
			/*Normal if uncorrelated module*/
			continue;
		}
		if (modcov == NULL) {
			modcov = new TextCovMod(mod->m_remote_path);
		}
		modcov->m_cu_map.insert(cucov->m_cu, cucov);
		/*Merge cu coverage to module*/
		QMap<unsigned int, CovLine*>::iterator iter_line;
		for (iter_line = cucov->m_linemap.begin(); iter_line != cucov->m_linemap.end(); iter_line++) {
			CovLine *cu_covline = iter_line.value();
			/*Get coverage line on module*/
			CovLine *mod_covline;
			QMap<unsigned int, CovLine*>::iterator iter_line_count;
			iter_line_count = modcov->m_linemap.find(cu_covline->m_line_no);
			if (iter_line_count == modcov->m_linemap.end()) {
				mod_covline = new CovLine();
				mod_covline->m_line_no = cu_covline->m_line_no;
				modcov->m_linemap.insert(mod_covline->m_line_no, mod_covline);
			} else {
				mod_covline = iter_line_count.value();
			}
			/*Merge line to line*/
			mod_covline->m_hit += cu_covline->m_hit;
			mod_covline->m_num += cu_covline->m_num;
			/*Merge line to module*/
			modcov->m_count_hit += cu_covline->m_hit;
			modcov->m_count_num += cu_covline->m_num;
		}
	}

	return modcov;
}

/*Text coverage for root (for a source file)*/
TextCovRoot *CovDock::makeTextCovRoot(QString &file, QMap<CovToken*, unsigned long long> &digintoken)
{
	TextCovRoot *rootcov = NULL;

	QMap<QString, TargetMod*>::iterator mod_iter;
	for (mod_iter = g_target->m_mod_items.begin(); mod_iter != g_target->m_mod_items.end(); mod_iter++) {
		TargetMod *moditem = mod_iter.value();

		TextCovMod *modcov = makeTextCovMod(moditem, file, digintoken);
		if (modcov == NULL) {
			/*Normal if uncorrelated module or no debug module*/
			continue;
		}
		if (rootcov == NULL) {
			rootcov = new TextCovRoot(file);
		}
		rootcov->m_mod_map.insert(modcov->m_mod, modcov);
		/*Merge module coverage to root*/
		QMap<unsigned int, CovLine*>::iterator iter_line;
		for (iter_line = modcov->m_linemap.begin(); iter_line != modcov->m_linemap.end(); iter_line++) {
			CovLine *mod_line = iter_line.value();
			/*Get coverage line on root*/
			CovLine *root_covline;
			QMap<unsigned int, CovLine*>::iterator iter_line_count;
			iter_line_count = rootcov->m_linemap.find(mod_line->m_line_no);
			if (iter_line_count == rootcov->m_linemap.end()) {
				root_covline = new CovLine();
				root_covline->m_line_no = mod_line->m_line_no;
				rootcov->m_linemap.insert(root_covline->m_line_no, root_covline);
			} else {
				root_covline = iter_line_count.value();
			}
			/*Merge line to line*/
			root_covline->m_hit += mod_line->m_hit;
			root_covline->m_num += mod_line->m_num;
			/*Merge line to root*/
			rootcov->m_count_hit += mod_line->m_hit;
			rootcov->m_count_num += mod_line->m_num;
		}
	}

	return rootcov;
}

/*Program coverage for function*/
ProgCovFun *CovDock::makeProgCovFun(DwfCu *cu, DwfFun *fun, QMap<CovToken*, unsigned long long> &digintoken)
{
	ProgCovFun *progfun = new ProgCovFun(fun->m_name);
	progfun->m_lineno = fun->m_lineno;

	/*Token count*/
	QMap<unsigned long long, CovToken*>::iterator token_iter;
	for (token_iter = fun->m_cov_token.m_utertoken_map.begin();
			token_iter != fun->m_cov_token.m_utertoken_map.end(); token_iter++) {
		CovToken *token = token_iter.value();
		if (token->m_hit_for_file) {
			progfun->token_hit++;
		}
		progfun->token_num++;
	}

	/*Statement count*/
	QList<DwfLine*>::iterator line_iter;
	for (line_iter = cu->m_dwf_line_list.begin();
		 line_iter != cu->m_dwf_line_list.end(); line_iter++) {
		DwfLine *dwfline = *line_iter;
		/*Check statement in function*/
		if (! fun->m_add_range.check(dwfline->m_lineaddr)) {
			continue;
		}
		/*The token which the statement in*/
		CovToken *token = fun->m_cov_token.getTokenByAddr(dwfline->m_lineaddr);
		if (token == NULL) {
			continue;
		}
		bool cov_hit = token->m_hit_for_file;
		if (! cov_hit) {
			/*Make up when token clean*/
			QMap<CovToken*, unsigned long long>::iterator dig_iter;
			dig_iter = digintoken.find(token);
			if (dig_iter != digintoken.end()) {
				unsigned long long digto_to = dig_iter.value();
				if (dwfline->m_lineaddr < digto_to) {
					cov_hit = true;
				}
			}
		}
		/*Function statement coverage count*/
		progfun->m_count_num++;
		if (cov_hit) {
			progfun->m_count_hit++;
		}
	}

	return progfun;
}

/*Program coverage for cu*/
ProgCovCu *CovDock::makeProgCovCu(DwfCu *cu, QMap<CovToken*, unsigned long long> &digintoken)
{
	ProgCovCu *progcu = new ProgCovCu(cu->m_path);
	QList<DwfFun*>::iterator fun_iter;
	for (fun_iter = cu->m_funs.begin(); fun_iter != cu->m_funs.end(); fun_iter++) {
		DwfFun *dwffun = *fun_iter;
		ProgCovFun *progfun = makeProgCovFun(cu, dwffun, digintoken);
		if (progfun == NULL) {
			continue;
		}
		/*Get the function at file*/
		QString filename = cu->getFileNameByNo(dwffun->m_fileno);
		ProgCovFile *progfile;
		QMap<QString, ProgCovFile*>::iterator file_iter;
		file_iter = progcu->m_file_map.find(filename);
		if (file_iter == progcu->m_file_map.end()) {
			progfile = new ProgCovFile(filename);
			progcu->m_file_map.insert(filename, progfile);
		} else {
			progfile = file_iter.value();
		}
		/*Merge function to file*/
		progfile->m_fun_list.push_back(progfun);
		progfile->m_count_hit += progfun->m_count_hit;
		progfile->m_count_num += progfun->m_count_num;
		/*Merge function to cu*/
		progcu->m_count_hit += progfun->m_count_hit;
		progcu->m_count_num += progfun->m_count_num;
	}

	return progcu;
}

/*Program coverage for module*/
ProgCovMod *CovDock::makeProgCovMod(TargetMod *mod, QMap<CovToken*, unsigned long long> &digintoken)
{
	ProgCovMod *progmod = NULL;
	QMap<QString, TextCovFile*>::iterator file_iter;
	for (file_iter = m_files.begin(); file_iter != m_files.end(); file_iter++) {
		TextCovFile *covfile = file_iter.value();
		/*Use cu which with same name of coverage file*/
		DwfCu *cu = mod->dwarfGetCuByPath(covfile->m_name);
		if (cu == NULL) {
			continue;
		}
		ProgCovCu *progcu = makeProgCovCu(cu, digintoken);
		if (progcu == NULL) {
			continue;
		}
		if (progmod == NULL) {
			progmod = new ProgCovMod(mod->m_remote_path);
		}
		progmod->m_cu_list.push_back(progcu);
	}
	return progmod;
}

void CovDock::flushEditor(UEditor *edit)
{
	if (! (m_starting && m_showing)) {
		/*Do clear on not start & show*/
		QMap<QString, TextCovFile*>::iterator iter;
		for (iter = m_files.begin(); iter != m_files.end(); iter++) {
			TextCovFile *covfile = iter.value();
			UEditor *edit = g_mainwin->editGetByPath(covfile->m_name);
			if (edit == NULL) {
				continue;
			}
			edit->fileCovClear();
		}

		m_text_cov_tree->clear();
		m_text_cov_edit->clear();
		m_prog_cov_tree->clear();
		m_prog_detail_tree->clear();
		return;
	}

	/*Flush edit*/
	QMap<QString, TextCovFile*>::iterator iter;
	for (iter = m_files.begin(); iter != m_files.end(); iter++) {
		TextCovFile *covfile = iter.value();

		if (edit == NULL) {
			/*Flush any opening coverage file*/
			edit = g_mainwin->editGetByPath(covfile->m_name);
			if (edit == NULL) {
				continue;
			}
		} else {
			/*Flush specify coverage file*/
			if (covfile->m_name != edit->m_file_path) {
				continue;
			}
		}

		/*Clear old*/
		edit->fileCovClear();
		/*Set new, flush every line*/
		int linecount = edit->lines();
		int iloop;
		for (iloop = 0; iloop < linecount; iloop++) {
			if (covfile->m_root_covinfo == NULL) {
				edit->fileCovSet(iloop, EditorCpp::FILECOV_IGNORE);
				continue;
			}
			/*Get line coverage*/
			QMap<unsigned int, CovLine*>::iterator iter;
			iter = covfile->m_root_covinfo->m_linemap.find(iloop + 1);
			if (iter == covfile->m_root_covinfo->m_linemap.end()) {
				edit->fileCovSet(iloop, EditorCpp::FILECOV_IGNORE);
				continue;
			}
			CovLine *covline = iter.value();
			/*Set line coverage*/
			if (covline->m_num == 0) {
				edit->fileCovSet(iloop, EditorCpp::FILECOV_IGNORE);
			} else if (covline->m_num == covline->m_hit) {
				edit->fileCovSet(iloop, EditorCpp::FILECOV_YES);
			} else if (covline->m_hit == 0) {
				edit->fileCovSet(iloop, EditorCpp::FILECOV_NO);
			} else {
				edit->fileCovSet(iloop, EditorCpp::FILECOV_PART);
			}
		}
	}

	/*Flush tab view*/
	textViewFlush();
	progViewFlush();
	return;
}

int CovDock::textViewFlush()
{
	m_text_cov_tree->clear();
	m_text_cov_edit->clear();

	QMap<QString, TextCovFile*>::iterator iter;
	for (iter = m_files.begin(); iter != m_files.end(); iter++) {
		TextCovFile *covfile = iter.value();

		TextCovRoot *rootcov = covfile->m_root_covinfo;
		if (rootcov == NULL) {
			/*No coverage information for file*/
			QTreeWidgetItem *file_item = new QTreeWidgetItem();
			file_item->setIcon(0, QIcon(":/images/warning.png"));
			file_item->setText(TEXTVIEW_COLUMM_ELEMENT, covfile->m_name);
			file_item->setToolTip(TEXTVIEW_COLUMM_ELEMENT, "Source file " + covfile->m_name + " no coverage information.");
			file_item->setText(TEXTVIEW_COLUMM_COVERAGE, "NA");
			file_item->setText(TEXTVIEW_COLUMM_LINES, "NA");
			file_item->setText(TEXTVIEW_COLUMM_PERCENT, "NA");
			file_item->setData(0, Qt::UserRole, QVariant::fromValue((void *)covfile));
			m_text_cov_tree->addTopLevelItem(file_item);
			continue;
		}

		/*File coverage root item*/
		QTreeWidgetItem *root_item = new QTreeWidgetItem();
		root_item->setIcon(0, QIcon(":/images/elmt_file.png"));
		root_item->setText(TEXTVIEW_COLUMM_ELEMENT, rootcov->m_file);
		root_item->setToolTip(TEXTVIEW_COLUMM_ELEMENT, "Source file " + rootcov->m_file);
		root_item->setText(TEXTVIEW_COLUMM_COVERAGE, QString::asprintf("%u", rootcov->m_count_hit));
		root_item->setText(TEXTVIEW_COLUMM_LINES, QString::asprintf("%u", rootcov->m_count_num));
		root_item->setText(TEXTVIEW_COLUMM_PERCENT, QString::asprintf("%0.2f%%", ((float)rootcov->m_count_hit) / rootcov->m_count_num));
		root_item->setData(0, Qt::UserRole, QVariant::fromValue((void *)rootcov));
		m_text_cov_tree->addTopLevelItem(root_item);

		/*The modules file in*/
		QMap<QString, TextCovMod*>::iterator iter_mod;
		for (iter_mod = rootcov->m_mod_map.begin(); iter_mod != rootcov->m_mod_map.end(); iter_mod++) {
			TextCovMod *modcov = iter_mod.value();

			QTreeWidgetItem *mod_item = new QTreeWidgetItem();
			mod_item->setIcon(0, QIcon(":/images/elmt_mod.png"));
			mod_item->setText(TEXTVIEW_COLUMM_ELEMENT, modcov->m_mod);
			mod_item->setToolTip(TEXTVIEW_COLUMM_ELEMENT, "Source file " + rootcov->m_file + " alive on module " + modcov->m_mod);
			mod_item->setText(TEXTVIEW_COLUMM_COVERAGE, QString::asprintf("%u", modcov->m_count_hit));
			mod_item->setText(TEXTVIEW_COLUMM_LINES, QString::asprintf("%u", modcov->m_count_num));
			mod_item->setText(TEXTVIEW_COLUMM_PERCENT, QString::asprintf("%0.2f%%", ((float)modcov->m_count_hit) / modcov->m_count_num));
			mod_item->setData(0, Qt::UserRole, QVariant::fromValue((void *)modcov));
			root_item->addChild(mod_item);

			/*The cu file in*/
			QMap<QString, TextCovCu*>::iterator iter_cu;
			for (iter_cu = modcov->m_cu_map.begin(); iter_cu != modcov->m_cu_map.end(); iter_cu++) {
				TextCovCu *cucov = iter_cu.value();

				QTreeWidgetItem *cu_item = new QTreeWidgetItem();
				cu_item->setIcon(0, QIcon(":/images/elmt_cu.png"));
				cu_item->setText(TEXTVIEW_COLUMM_ELEMENT, cucov->m_cu);
				if (rootcov->m_file == cucov->m_cu) {
					cu_item->setToolTip(TEXTVIEW_COLUMM_ELEMENT, "Source file " + rootcov->m_file + " is compilation unit " + cucov->m_cu);
				} else {
					cu_item->setToolTip(TEXTVIEW_COLUMM_ELEMENT, "Source file " + rootcov->m_file + " alive in compilation unit " + cucov->m_cu);
				}
				cu_item->setText(TEXTVIEW_COLUMM_COVERAGE, QString::asprintf("%u", cucov->m_count_hit));
				cu_item->setText(TEXTVIEW_COLUMM_LINES, QString::asprintf("%u", cucov->m_count_num));
				cu_item->setText(TEXTVIEW_COLUMM_PERCENT, QString::asprintf("%0.2f%%", ((float)cucov->m_count_hit) / cucov->m_count_num));
				cu_item->setData(0, Qt::UserRole, QVariant::fromValue((void *)cucov));
				mod_item->addChild(cu_item);
			}
		}
	}

	//m_text_cov_tree->expandAll();
	m_text_cov_tree->resizeColumnToContents(TEXTVIEW_COLUMM_ELEMENT);
	return 0;
}

void CovDock::slotTextTreeItemClicked(QTreeWidgetItem *item, int column)
{
	CovNode *node = (CovNode *)item->data(0, Qt::UserRole).value<void *>();
	UTER_ASSERT(node != NULL);

	m_text_cov_edit->clear();

	QString path;
	QMap<unsigned int, CovLine*> *linemap = NULL;

	switch(node->m_type)
	{
	case CovNode::TYPE_TEXT_NA:
	{
		TextCovFile *covfile = (TextCovFile *)node;
		break;
	}
	case CovNode::TYPE_TEXT_ROOT:
	{
		TextCovRoot *rootcov = (TextCovRoot *)node;

		path = rootcov->m_file;
		linemap = &(rootcov->m_linemap);
		break;
	}
	case CovNode::TYPE_TEXT_MOD:
	{
		TextCovMod *modcov = (TextCovMod *)node;

		item = item->parent();
		node = (CovNode *)item->data(0, Qt::UserRole).value<void *>();
		UTER_ASSERT(node != NULL);
		UTER_ASSERT(node->m_type == CovNode::TYPE_TEXT_ROOT);
		TextCovRoot *rootcov = (TextCovRoot *)node;

		path = rootcov->m_file;
		linemap = &(modcov->m_linemap);
		break;
	}
	case CovNode::TYPE_TEXT_CU:
	{
		TextCovCu *cucov = (TextCovCu *)node;
		item = item->parent();
		node = (CovNode *)item->data(0, Qt::UserRole).value<void *>();
		UTER_ASSERT(node != NULL);
		UTER_ASSERT(node->m_type == CovNode::TYPE_TEXT_MOD);
		TextCovMod *modcov = (TextCovMod *)node;

		item = item->parent();
		node = (CovNode *)item->data(0, Qt::UserRole).value<void *>();
		UTER_ASSERT(node != NULL);
		UTER_ASSERT(node->m_type == CovNode::TYPE_TEXT_ROOT);
		TextCovRoot *rootcov = (TextCovRoot *)node;

		path = rootcov->m_file;
		linemap = &(cucov->m_linemap);
		break;
	}
	default:
	{
		break;
	}
	}

	if (path == "" || linemap == NULL) {
		m_text_cov_edit->setText("/*No coverage information.*/");
		return;
	}
	int result = m_text_cov_edit->loadFile(path, "/*Can not load source file " + path + "*/");
	if (result < 0) {
		return;
	}

	/*Set coverage to edit*/
	int iloop;
	int count = m_text_cov_edit->lines();
	for (iloop = 0; iloop < count; iloop++) {
		QMap<unsigned int, CovLine*>::iterator iter;
		iter = linemap->find(iloop + 1);	/*dwarf line number form 1*/
		if (iter == linemap->end()) {
			continue;
		}
		CovLine *covline = iter.value();
		if (covline->m_hit == 0) {
			m_text_cov_edit->markerAdd(iloop, MARKS_COV_NO);
		} else if (covline->m_hit == covline->m_num) {
			m_text_cov_edit->markerAdd(iloop, MARKS_COV_YES);
		} else {
			m_text_cov_edit->markerAdd(iloop, MARKS_COV_PART);
		}
	}

	return;
}


int CovDock::progViewFlush()
{
	m_prog_cov_tree->clear();
	m_prog_detail_tree->clear();

	/*Foreach module*/
	QList<ProgCovMod*>::iterator progmod_iter;
	for (progmod_iter = m_progcov_list.begin(); progmod_iter != m_progcov_list.end(); progmod_iter++) {
		ProgCovMod *progmod = *progmod_iter;

		QTreeWidgetItem *mod_item = new QTreeWidgetItem();
		mod_item->setIcon(0, QIcon(":/images/elmt_mod.png"));
		mod_item->setText(PROGVIEW_COLUMM_ELEMENT, progmod->m_path);
		mod_item->setToolTip(PROGVIEW_COLUMM_ELEMENT, "Module:" + progmod->m_path);
		mod_item->setData(0, Qt::UserRole, QVariant::fromValue((void *)progmod));
		m_prog_cov_tree->addTopLevelItem(mod_item);

		/*Foreach cu*/
		QList<ProgCovCu*>::iterator progcu_iter;
		for (progcu_iter = progmod->m_cu_list.begin();
			 progcu_iter != progmod->m_cu_list.end(); progcu_iter++) {
			ProgCovCu *progcu = *progcu_iter;

			QTreeWidgetItem *cu_item = new QTreeWidgetItem();
			cu_item->setIcon(0, QIcon(":/images/elmt_cu.png"));
			cu_item->setText(PROGVIEW_COLUMM_ELEMENT, progcu->m_path);
			cu_item->setToolTip(PROGVIEW_COLUMM_ELEMENT, "Cu:" + progcu->m_path);
			cu_item->setData(0, Qt::UserRole, QVariant::fromValue((void *)progcu));
			mod_item->addChild(cu_item);
		}
	}

	//m_prog_cov_tree->expandAll();
	m_prog_cov_tree->resizeColumnToContents(PROGVIEW_COLUMM_ELEMENT);
	return 0;
}

void CovDock::slotProgTreeItemClicked(QTreeWidgetItem *item, int column)
{
	CovNode *node = (CovNode *)item->data(0, Qt::UserRole).value<void *>();
	UTER_ASSERT(node != NULL);

	switch(node->m_type)
	{
	case CovNode::TYPE_PROG_MOD:
	{
		ProgCovMod *progmod = (ProgCovMod*)node;
		m_prog_detail_tree->clear();
		break;
	}
	case CovNode::TYPE_PROG_CU:
	{
		ProgCovCu *progcu = (ProgCovCu*)node;
		progTableFlush(progcu);
		break;
	}
	default:
	{
		break;
	}
	}

	return;
}

static int progTableColor(QTreeWidgetItem *item, unsigned int hit, unsigned int num)
{
	int iloop;
	for (iloop = 0; iloop < PROGDETAIL_COLUMM_MAX; iloop++) {
		if (hit == 0) {
			item->setBackgroundColor(iloop, EditorCpp::m_cov_color_no);
		} else if (hit != num) {
			item->setBackgroundColor(iloop, EditorCpp::m_cov_color_part);
		} else {
			item->setBackgroundColor(iloop, EditorCpp::m_cov_color_yes);
		}
	}
	return 0;
}

int CovDock::progTableFlush(ProgCovCu *progcu)
{
	m_prog_detail_tree->clear();

	/*Foreach file*/
	QMap<QString, ProgCovFile*>::iterator file_iter;
	for (file_iter = progcu->m_file_map.begin(); file_iter != progcu->m_file_map.end(); file_iter++) {
		ProgCovFile *progfile = file_iter.value();

		QFileInfo fileinfo(progfile->m_path);
		QTreeWidgetItem *file_item = new QTreeWidgetItem();
		file_item->setIcon(0, QIcon(":/images/elmt_file.png"));
		file_item->setText(PROGDETAIL_COLUMM_FILE, fileinfo.fileName());
		progTableColor(file_item, progfile->m_count_hit, progfile->m_count_num);
		QMap<QString, TextCovFile*>::iterator txt_file_iter;
		txt_file_iter = m_files.find(progfile->m_path);
		if (txt_file_iter == m_files.end()) {
			file_item->setToolTip(PROGDETAIL_COLUMM_FILE, progfile->m_path + " not a project file.");
			file_item->setIcon(0, QIcon(":/images/cannot.png"));
		} else {
			file_item->setToolTip(PROGDETAIL_COLUMM_FILE, progfile->m_path);
			file_item->setIcon(0, QIcon(":/images/elmt_file.png"));
		}
		file_item->setData(0, Qt::UserRole, QVariant::fromValue((void *)progfile));
		m_prog_detail_tree->addTopLevelItem(file_item);

		/*Foreach function*/
		QList<ProgCovFun*>::iterator fun_iter;
		for (fun_iter = progfile->m_fun_list.begin(); fun_iter != progfile->m_fun_list.end(); fun_iter++) {
			ProgCovFun* progfun = *fun_iter;

			QTreeWidgetItem *fun_item = new QTreeWidgetItem();
			fun_item->setText(PROGDETAIL_COLUMM_NAME, progfun->m_name);
			fun_item->setToolTip(PROGDETAIL_COLUMM_NAME, progfun->m_name);
			fun_item->setText(PROGDETAIL_COLUMM_COVERAGE, QString::asprintf("%u", progfun->m_count_hit));
			fun_item->setText(PROGDETAIL_COLUMM_LINES, QString::asprintf("%u", progfun->m_count_num));
			fun_item->setText(PROGDETAIL_COLUMM_PERCENT, QString::asprintf("%0.2f%%", ((float)progfun->m_count_hit) / progfun->m_count_num));
			fun_item->setText(PROGDETAIL_COLUMM_LINENO, QString::asprintf("%u", progfun->m_lineno));
			progTableColor(fun_item, progfun->m_count_hit, progfun->m_count_num);
			fun_item->setData(0, Qt::UserRole, QVariant::fromValue((void *)progfun));
			file_item->addChild(fun_item);
		}
	}
	m_prog_detail_tree->expandAll();
	m_prog_detail_tree->resizeColumnToContents(PROGDETAIL_COLUMM_NAME);

	return 0;
}

void CovDock::slotProgTableItemClicked(QTreeWidgetItem *item, int column)
{
	CovNode *node = (CovNode *)item->data(0, Qt::UserRole).value<void *>();
	UTER_ASSERT(node != NULL);

	switch(node->m_type)
	{
	case CovNode::TYPE_PROG_FILE:
	{
		ProgCovFile *progfile= (ProgCovFile*)node;
		break;
	}
	case CovNode::TYPE_PROG_FUN:
	{
		ProgCovFun *progfun = (ProgCovFun*)node;

		item = item->parent();
		node = (CovNode *)item->data(0, Qt::UserRole).value<void *>();
		UTER_ASSERT(node != NULL);
		UTER_ASSERT(node->m_type == CovNode::TYPE_PROG_FILE);
		ProgCovFile *progfile = (ProgCovFile *)node;

		/*Locate cu by left tree*/
		item = m_prog_cov_tree->currentItem();
		if (item == NULL) {
			return;
		}
		node = (CovNode *)item->data(0, Qt::UserRole).value<void *>();
		UTER_ASSERT(node != NULL);
		UTER_ASSERT(node->m_type == CovNode::TYPE_PROG_CU);
		ProgCovCu *progcu = (ProgCovCu *)node;

		item = item->parent();
		node = (CovNode *)item->data(0, Qt::UserRole).value<void *>();
		UTER_ASSERT(node != NULL);
		UTER_ASSERT(node->m_type == CovNode::TYPE_PROG_MOD);
		ProgCovMod *progmod = (ProgCovMod *)node;

		/*Show it in text view (jump to)*/

		/*Foreach text coverage file root*/
		int rootcount = m_text_cov_tree->topLevelItemCount();
		int iloop;
		for (iloop = 0; iloop < rootcount; iloop++) {
			QTreeWidgetItem *rootitem = m_text_cov_tree->topLevelItem(iloop);
			CovNode *rootnode = (CovNode *)rootitem->data(0, Qt::UserRole).value<void *>();
			UTER_ASSERT(rootnode->m_type == CovNode::TYPE_TEXT_NA || rootnode->m_type == CovNode::TYPE_TEXT_ROOT);
			if (rootnode->m_type != CovNode::TYPE_TEXT_ROOT) {
				continue;
			}
			/*Check file name in text coverage root*/
			TextCovRoot *txtroot = (TextCovRoot *)rootnode;
			if (txtroot->m_file != progfile->m_path) {
				continue;
			}

			/*Foreach text coverage file in modules*/
			int modcount = rootitem->childCount();
			int jloop;
			for (jloop = 0; jloop < modcount; iloop++) {
				QTreeWidgetItem *moditem = rootitem->child(jloop);
				CovNode *modnode = (CovNode *)moditem->data(0, Qt::UserRole).value<void *>();
				UTER_ASSERT(modnode->m_type == CovNode::TYPE_TEXT_MOD);
				/*Check module name in text coverage file in module*/
				TextCovMod *txtmod = (TextCovMod *)modnode;
				if (txtmod->m_mod != progmod->m_path) {
					continue;
				}

				/*Foreach text coverage file in cus*/
				int cucount = moditem->childCount();
				int kloop;
				for (kloop = 0; kloop < cucount; kloop++) {
					QTreeWidgetItem *cuitem = moditem->child(kloop);
					CovNode *cunode = (CovNode *)cuitem->data(0, Qt::UserRole).value<void *>();
					UTER_ASSERT(cunode->m_type == CovNode::TYPE_TEXT_CU);
					TextCovCu *txtcu = (TextCovCu *)cunode;
					if (txtcu->m_cu != progcu->m_path) {
						/*Check module name in text coverage file in cu*/
						continue;
					}

					/*Simulate point cu item, and show function*/
					m_top_tab->setCurrentWidget(m_text_view);
					m_text_cov_tree->setCurrentItem(cuitem);
					cuitem->setSelected(true);
					slotTextTreeItemClicked(cuitem, 0);
					m_text_cov_edit->setFirstVisibleLine(progfun->m_lineno - 1);
					m_text_cov_edit->selectLine(progfun->m_lineno - 1);

					break;
				}

				break;
			}

			break;
		}

		break;
	}
	default:
	{
		break;
	}
	}

	return;
}

void CovDock::slotEditFileOpen(UEditor *edit)
{
	if (! (m_starting && m_showing)) {
		return;
	}
	/*Flush coverage to edit*/
	flushEditor(edit);
	return;
}

int CovDock::applyAdd(unsigned long long addr, CovToken *token)
{
	QMap<unsigned long long, ApplyCov*>::iterator apply_iter;
	apply_iter = m_apply_map.find(addr);
	if (apply_iter != m_apply_map.end()) {
		ApplyCov *apply = apply_iter.value();
		apply->m_ref_count++;
		return 0;
	}
	ApplyCov *apply = new ApplyCov();
	apply->m_addr = addr;
	apply->m_ref_count++;
	apply->m_token = token;
	m_apply_map.insert(addr, apply);
	/*Set breakpoint*/
	g_target->breakInsert(apply->m_addr, TargetBreak::BREAK_COVFILE, apply->m_token);
	return 1;
}

int CovDock::applyDel(unsigned long long addr, CovToken *token)
{
	QMap<unsigned long long, ApplyCov*>::iterator apply_iter;
	apply_iter = m_apply_map.find(addr);
	if (apply_iter != m_apply_map.end()) {
		ApplyCov *apply = apply_iter.value();
		apply->m_ref_count--;
		UTER_ASSERT(apply->m_token == token);
		if (apply->m_ref_count == 0) {
			/*Remove breakpoint*/
			g_target->breakDelete(apply->m_addr, TargetBreak::BREAK_COVFILE);
			m_apply_map.erase(apply_iter);
			delete apply;
			return 1;
		}
		return 0;
	}
	UTER_ASSERT(0);
	return -1;
}

int CovDock::fileModSetup(TextCovFile *covfile, TargetMod *moditem)
{
	UTER_ASSERT(covfile != NULL);
	UTER_ASSERT(moditem != NULL);

	int ret_val = 0;

	QMap<QString, DwfCu*>::iterator iter_cu;
	for (iter_cu = moditem->m_dwarf_cu_map.begin(); iter_cu != moditem->m_dwarf_cu_map.end(); iter_cu++) {
		DwfCu *cu = iter_cu.value();
		if (cu->m_path == covfile->m_name) {
			/*Check cu path same to coverage file, monitor all of the cu (For program coverage)*/
			QList<DwfFun*>::iterator fun_iter;
			for (fun_iter = cu->m_funs.begin(); fun_iter != cu->m_funs.end(); fun_iter++) {
				DwfFun *dwffun = *fun_iter;
				QMap<unsigned long long, CovToken*>::iterator token_iter;
				for (token_iter = dwffun->m_cov_token.m_utertoken_map.begin();
					 token_iter != dwffun->m_cov_token.m_utertoken_map.end(); token_iter++) {
					CovToken *token = token_iter.value();
					int result = applyAdd(token->m_addr, token);
					if (result > 0) {
						ret_val = 2;	/*Breakpoint changed*/
					}
				}
			}
			continue;
		}

		/*If file compiled in cu*/
		QMap<QString, unsigned int>::iterator iter_source;
		iter_source = cu->m_file_path_map.find(covfile->m_name);
		if (iter_source == cu->m_file_path_map.end()) {
			continue;
		}
		unsigned int fileno = iter_source.value();

		/*Monitor all line in this file*/
		QMap<unsigned long long, DwfLine*>::iterator line_iter;
		for (line_iter = cu->m_dwf_line_map.begin();
			 line_iter != cu->m_dwf_line_map.end(); line_iter++) {
			DwfLine *line = line_iter.value();
			unsigned long long addr = line_iter.key();	/*Statement address*/
			if (line->m_fileno != fileno) {
				continue;
			}

			/*Get token for statement address*/
			QList<DwfFun*>::iterator iter_func;
			for (iter_func = cu->m_funs.begin(); iter_func != cu->m_funs.end(); iter_func++) {
				DwfFun *fun = *iter_func;
				if (fun->m_add_range.check(addr)) {
					break;
				}
			}
			if (iter_func == cu->m_funs.end()) {
				continue;
			}
			DwfFun *fun = *iter_func;
			CovToken *token = fun->m_cov_token.getTokenByAddr(addr);
			if (token == NULL) {
				continue;
			}

			int result = applyAdd(token->m_addr, token);
			if (result > 0) {
				ret_val = 2;	/*Breakpoint changed*/
			}
		}
	}

	return ret_val;
}

int CovDock::fileModTearDown(TextCovFile *covfile, TargetMod *moditem)
{
	UTER_ASSERT(covfile != NULL);
	UTER_ASSERT(moditem != NULL);

	int ret_val = 0;

	QMap<QString, DwfCu*>::iterator iter_cu;
	for (iter_cu = moditem->m_dwarf_cu_map.begin(); iter_cu != moditem->m_dwarf_cu_map.end(); iter_cu++) {
		DwfCu *cu = iter_cu.value();
		if (cu->m_path == covfile->m_name) {
			/*Check cu path same to coverage file, monitor all of the cu (For program coverage)*/
			QList<DwfFun*>::iterator fun_iter;
			for (fun_iter = cu->m_funs.begin(); fun_iter != cu->m_funs.end(); fun_iter++) {
				DwfFun *dwffun = *fun_iter;
				QMap<unsigned long long, CovToken*>::iterator token_iter;
				for (token_iter = dwffun->m_cov_token.m_utertoken_map.begin();
					 token_iter != dwffun->m_cov_token.m_utertoken_map.end(); token_iter++) {
					CovToken *token = token_iter.value();
					int result = applyDel(token->m_addr, token);
					if (result > 0) {
						ret_val = 2;	/*Breakpoint changed*/
					}
				}
			}
			continue;
		}

		/*If file compiled in cu*/
		QMap<QString, unsigned int>::iterator iter_source;
		iter_source = cu->m_file_path_map.find(covfile->m_name);
		if (iter_source == cu->m_file_path_map.end()) {
			continue;
		}
		unsigned int fileno = iter_source.value();

		/*Monitor all line in this file*/
		QMap<unsigned long long, DwfLine*>::iterator line_iter;
		for (line_iter = cu->m_dwf_line_map.begin();
			 line_iter != cu->m_dwf_line_map.end(); line_iter++) {
			DwfLine *line = line_iter.value();
			unsigned long long addr = line_iter.key();	/*Statement address*/
			if (line->m_fileno != fileno) {
				continue;
			}

			/*Get token for statement address*/
			QList<DwfFun*>::iterator iter_func;
			for (iter_func = cu->m_funs.begin(); iter_func != cu->m_funs.end(); iter_func++) {
				DwfFun *fun = *iter_func;
				if (fun->m_add_range.check(addr)) {
					break;
				}
			}
			if (iter_func == cu->m_funs.end()) {
				continue;
			}
			DwfFun *fun = *iter_func;
			CovToken *token = fun->m_cov_token.getTokenByAddr(addr);
			if (token == NULL) {
				continue;
			}

			int result = applyDel(token->m_addr, token);
			if (result > 0) {
				ret_val = 2;		/*Breakpoint changed*/
			}
		}
	}

	return ret_val;
}

void CovDock::slotModAdd(TargetMod *moditem)
{
	UTER_ASSERT(moditem != NULL);
	if (! m_starting) {
		return;
	}
	QMap<QString, TextCovFile*>::iterator iter;
	for (iter = m_files.begin(); iter != m_files.end(); iter++) {
		TextCovFile *covfile = iter.value();
		/*When signal/slot triggered, target stopping now, so no need call breakChanged*/
		fileModSetup(covfile, moditem);
	}
	return;
}

void CovDock::slotModDel(TargetMod *moditem)
{
	UTER_ASSERT(moditem != NULL);
	if (! m_starting) {
		return;
	}
	QMap<QString, TextCovFile*>::iterator iter;
	for (iter = m_files.begin(); iter != m_files.end(); iter++) {
		TextCovFile *covfile = iter.value();
		/*When signal/slot triggered, target stopping now, so no need call breakChanged*/
		fileModTearDown(covfile, moditem);
	}
	return;
}

int CovDock::fileAttach(TextCovFile *covfile)
{
	UTER_ASSERT(covfile != NULL);
	bool change = false;
	QMap<QString, TargetMod*>::iterator iter;
	for (iter = g_target->m_mod_items.begin(); iter != g_target->m_mod_items.end(); iter++) {
		TargetMod *moditem = iter.value();
		int result = fileModSetup(covfile, moditem);
		if (result == 2) {
			change = true;
		}
	}
	if (change) {
		g_target->breakChanged();
	}
	return 0;
}

int CovDock::fileDetach(TextCovFile *covfile)
{
	UTER_ASSERT(covfile != NULL);
	bool change = false;
	QMap<QString, TargetMod*>::iterator iter;
	for (iter = g_target->m_mod_items.begin(); iter != g_target->m_mod_items.end(); iter++) {
		TargetMod *moditem = iter.value();
		int result = fileModTearDown(covfile, moditem);
		if (result == 2) {
			change = true;
		}
	}
	if (change) {
		g_target->breakChanged();
	}
	return 0;
}

void CovDock::slotAddPrjFile(QString path)
{
	QMap<QString, TextCovFile*>::iterator iter_file;
	iter_file = m_files.find(path);
	if (iter_file != m_files.end()) {
		return;
	}
	TextCovFile *covfile = new TextCovFile(path);
	m_files.insert(path, covfile);
	if (m_starting) {
		/*If coverage start*/
		fileAttach(covfile);
	}
	return;
}

void CovDock::slotDelPrjFile(QString path)
{
	QMap<QString, TextCovFile*>::iterator iter_file;
	iter_file = m_files.find(path);
	if (iter_file == m_files.end()) {
		return;
	}
	TextCovFile *covfile = iter_file.value();
	if (m_starting) {
		/*If coverage start*/
		fileDetach(covfile);
	}
	m_files.erase(iter_file);
	delete covfile;
	return;
}

} /*namespace CovUi*/
