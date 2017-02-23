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

#include <qscilexercpp.h>
#include <qscilexerpython.h>
#include <qscilexermakefile.h>
#include <qscilexerhtml.h>

#include <ui_mainwin.hxx>

EditorBase::EditorBase(FileType type, QWidget *parent)
:QsciScintilla(parent)
{
	m_lexer = NULL;
	setType(type);
	return;
}

EditorBase::~EditorBase()
{
	if (m_lexer != NULL) {
		delete m_lexer;
		m_lexer = NULL;
	}
	return;
}

int EditorBase::setType(FileType type)
{
	m_type = type;

	/*Clear old lexer*/
	if (m_lexer != NULL) {
		setLexer(NULL);
		delete m_lexer;
		m_lexer = NULL;
	}

	switch(m_type)
	{
	case FILETYPE_H:
	case FILETYPE_C:
	case FILETYPE_CPP:
	{
		m_lexer = new QsciLexerCPP();
		break;
	}
	case FILETYPE_PY:
	{
		m_lexer = new QsciLexerPython();
		break;
	}
	default:
	{
		break;
	}
	}

	if (m_lexer != NULL) {
		setLexer(m_lexer);
	}
	return 0;
}

int EditorBase::loadFile(QString path, QString onfail)
{
	QFile file(path);
	if (! file.open(QFile::ReadOnly | QFile::Text)) {
		setText(onfail);
		return -1;
	}

	QTextStream in(&file);

	g_ui_main.setWaitCursor();
	setText(in.readAll());
	g_ui_main.restoreCursor();

	file.close();
	return 0;
}

int EditorBase::selectLine(int lineno)
{
	int line_length = lineLength(lineno);
	setSelection(lineno, 0, lineno, line_length);
	return 0;
}

enum {
	EDITOR_MARGINS_LINENO = 0,	/*Line number*/
};

UEditor::UEditor(QString path, QWidget *parent)
:EditorBase(FILETYPE_UNKNOWN, parent)
{
	QMetaObject::Connection conn;

	m_mdi_item = new MdiItem();
	m_mdi_item->setWidget(this);

	m_file_path = "";
	m_file_name = "";

	/*Convert click to doublie click*/
	m_last_click_margin = -1;
	m_last_click_line = -1;
	m_last_click_time.start();

	/*Select slow*/
	m_selected = false;

	/*File changing*/
	m_changing = false;

	/*Margin for line number*/
	setMarginType(EDITOR_MARGINS_LINENO, QsciScintilla::NumberMargin);
	setMarginWidth(EDITOR_MARGINS_LINENO, 0);		/*Hiden*/
	setMarginMarkerMask(EDITOR_MARGINS_LINENO, 0);
	setMarginSensitivity(EDITOR_MARGINS_LINENO, true);

	setTabWidth(4);									/*Tab size*/
	setCaretWidth(50);								/*Cursor size*/
	setCaretForegroundColor (QColor(0, 0, 0, 255));	/*Cursor color*/
	setUtf8(true);									/*Code*/

	conn = connect(this, SIGNAL(marginClicked(int, int, Qt::KeyboardModifiers)),
				   this, SLOT(slotMarginClicked(int, int, Qt::KeyboardModifiers)));
	UTER_ASSERT(conn != NULL);
	conn = connect(this, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(slotPositionChanged(int, int)));
	UTER_ASSERT(conn != NULL);
	conn = connect(this, SIGNAL(modificationChanged(bool)), this, SLOT(slotModificationChanged(bool)));
	UTER_ASSERT(conn != NULL);
	conn = connect(this, SIGNAL(selectionChanged()), this, SLOT(slotSelectionChanged()));
	UTER_ASSERT(conn != NULL);
	conn = connect(this, SIGNAL(textChanged()), this, SLOT(slotTextChanged()));
	UTER_ASSERT(conn != NULL);

	if (path != "") {
		/*Load file*/
		loadFile(path);
	}
	return;
}

UEditor::~UEditor()
{
	/*Remove self from mid item for release ownership before delete mid item*/
	m_mdi_item->setWidget(NULL);
	delete m_mdi_item;
	return;
}

/*Get file type from file name*/
EditorBase::FileType UEditor::getFileTypeByPath(QString name)
{
	if (name.right(2) == ".h" || name.right(4) == ".hxx" || name.right(4) == ".hpp") {
		return FILETYPE_H;
	} else if (name.right(2) == ".c") {
		return FILETYPE_C;
	} else if (name.right(4) == ".cpp" || name.right(4) == ".cxx") {
		return FILETYPE_CPP;
	} else if (name.right(3) == ".py") {
		return FILETYPE_PY;
	} else if (name.right(4) == ".txt") {
		return FILETYPE_TXT;
	} else if (name.right(5) == ".html") {
		return FILETYPE_HTML;
	} else if (name.right(8) == "Makefile" || name.right(3) == ".mk") {
		return FILETYPE_MAKE;
	} else {
		return FILETYPE_UNKNOWN;
	}
}

UEditor *UEditor::createEditByPath(QString path, QWidget *parent)
{
	FileType set_type = getFileTypeByPath(path);
	switch(set_type)
	{
	case FILETYPE_C:
	case FILETYPE_CPP:
	case FILETYPE_H:
	{
		return new EditorCpp(path, parent);
	}
	case FILETYPE_PY:
	{
		return new EditorPy(path, parent);
	}
	default:
	{
		return new UEditor(path, parent);
	}
	}

	return NULL;
}

/*Convert click to doublie click*/
void UEditor::slotMarginClicked(int margin, int line, Qt::KeyboardModifiers state)
{
	/*UEditor line number begin from 0, convert to system line number*/
	line += 1;

	if (m_last_click_margin == margin && m_last_click_line == line
		&& m_last_click_time.elapsed() <= QApplication::doubleClickInterval()) {
		/*Double click*/
		m_last_click_margin = -1;
		m_last_click_line = -1;
		m_last_click_time.restart();

		emit g_mainwin->signalEditMarginDClicked(this, line);
	} else {
		m_last_click_margin = margin;
		m_last_click_line = line;
		m_last_click_time.restart();
	}

	return;
}

void UEditor::slotPositionChanged(int line, int index)
{
	emit signalPositionChanged();
	return;
}

void UEditor::slotModificationChanged(bool status)
{
	flushTitle();
	return;
}

void UEditor::slotSelectionChanged()
{
	if (hasSelectedText() == m_selected) {
		return;
	}

	m_selected = hasSelectedText();
	emit signalSelectChanged();
	return;
}

void UEditor::slotTextChanged()
{
	if (m_changing) {
		return;
	}

	/* beginUndoAction called on loadFile, call endUndoAction after changed to
	 * finish a undo section, and reopen a new undo section with beginUndoAction*/
	endUndoAction();
	beginUndoAction();
	return;
}

void UEditor::initTitle()
{
	m_mdi_item->setTip("(" + m_file_name + ") " + m_file_path);

	switch(m_type)
	{
	case FILETYPE_PY:
	{
		m_mdi_item->setIcon(QIcon(":/images/lang_py.png"));
		break;
	}
	case FILETYPE_C:
	{
		m_mdi_item->setIcon(QIcon(":/images/lang_c.png"));
		break;
	}
	case FILETYPE_CPP:
	{
		m_mdi_item->setIcon(QIcon(":/images/lang_cpp.png"));
		break;
	}
	case FILETYPE_H:
	{
		m_mdi_item->setIcon(QIcon(":/images/lang_h.png"));
		break;
	}
	case FILETYPE_ASMI386:
	{
		break;
	}
	case FILETYPE_TXT:
	{
		m_mdi_item->setIcon(QIcon(":/images/lang_txt.png"));
		break;
	}
	case FILETYPE_HTML:
	{
		m_mdi_item->setIcon(QIcon(":/images/lang_html.png"));
		break;
	}
	case FILETYPE_MAKE:
	{
		m_mdi_item->setIcon(QIcon(":/images/lang_makefile.png"));
		break;
	}
	default:
	{
		m_mdi_item->setIcon(QIcon(":/images/lang_unknown.png"));
		break;
	}
	}

	return;
}

void UEditor::flushTitle()
{
	if (this->isModified()) {
		m_mdi_item->setTitle("*" + m_file_name);
	} else {
		m_mdi_item->setTitle(m_file_name);
	}

	g_mainwin->m_center_mdi->flushTabBar();
	return;
}

int UEditor::loadFile(QString path)
{
	int result;

	/*File name*/
	QFileInfo file_info;
	file_info = QFileInfo(path);
	m_file_path = path;
	m_file_name = file_info.fileName();

	/*Load file*/
	m_changing = true;
	result = EditorBase::loadFile(path, "");
	m_changing = false;
	if (result < 0) {
		return -1;
	}

	setModified(false);

	/*Begin undo section*/
	beginUndoAction();

	/*File type*/
	setType(getFileTypeByPath(m_file_path));
	initTitle();
	flushTitle();

	emit g_mainwin->signalEditFileOpen(this);
	return 0;
}

int UEditor::saveFile(QString path)
{
	if (path == "") {
		path = m_file_path;
	} else {
		if (path != m_file_path) {
			/*File name*/
			QFileInfo file_info;
			file_info = QFileInfo(path);
			m_file_path = path;
			m_file_name = file_info.fileName();

			/*File type*/
			setType(getFileTypeByPath(m_file_path));

			initTitle();
			flushTitle();
		}
	}

	QFile file(path);
	if (! file.open(QFile::ReadWrite | QFile::Text | QFile::Truncate)) {
		return -1;
	}

	QTextStream out(&file);

	g_ui_main.setWaitCursor();
	QString txt = text();
	out << txt;
	/*setText(txt);*/
	g_ui_main.restoreCursor();

	file.close();

	setModified(false);
	flushTitle();
	return 0;
}

int UEditor::setEnableLineNo(bool set)
{
	if (set) {
		setMarginWidth(EDITOR_MARGINS_LINENO, 40);
	} else {
		setMarginWidth(EDITOR_MARGINS_LINENO, 0);
	}
	return 0;
}

enum {
	EDITORCPP_MARGINS_ARROW = 1,			/*Margin arrow*/
	EDITORCPP_MARGINS_BRKPNT = 2,			/*Margin breakpoint*/
};

enum {
	EDITORCPP_MARKS_ARROW = 0,		/*Debug arrow*/
	EDITORCPP_MARKS_BRKPNT_EN = 1,	/*Breakpoint (enable)*/
	EDITORCPP_MARKS_BRKPNT_DIS = 2,	/*Breakpoint (disable)*/
	EDITORCPP_MARKS_COV_YES = 3,	/*Coverage (full)*/
	EDITORCPP_MARKS_COV_PART = 4,	/*Coverage (part)*/
	EDITORCPP_MAKER_COV_NO = 5,		/*Coverage (no)*/
};

QColor EditorCpp::m_cov_color_yes = QColor(0, 0xff, 0, 0x1f);
QColor EditorCpp::m_cov_color_part = QColor(0xff, 0xff, 0, 0x1f);
QColor EditorCpp::m_cov_color_no = QColor(0xff, 0, 0, 0x1f);

EditorCpp::EditorCpp(QString path, QWidget *parent)
: UEditor(path, parent)
{
	/*Arrow mark & arrow margin*/
	QImage arrowimg(":/images/arrow_right.png");
	arrowimg = arrowimg.scaled(20, 20);
	markerDefine(arrowimg, EDITORCPP_MARKS_ARROW);

	setMarginType(EDITORCPP_MARGINS_ARROW, QsciScintilla::SymbolMargin);
	setMarginWidth(EDITORCPP_MARGINS_ARROW, 20);
	setMarginMarkerMask(EDITORCPP_MARGINS_ARROW, 1 << EDITORCPP_MARKS_ARROW);

	/*Breakpoint mark & breakpoint margin*/
	QImage brkpntenimg(":/images/brkpnt_enable.png");
	brkpntenimg = brkpntenimg.scaled(15, 15);
	markerDefine(brkpntenimg, EDITORCPP_MARKS_BRKPNT_EN);

	QImage brkpntdisimg(":/images/brkpnt_disable.png");
	brkpntdisimg = brkpntdisimg.scaled(15, 15);
	markerDefine(brkpntdisimg, EDITORCPP_MARKS_BRKPNT_DIS);

	setMarginType(EDITORCPP_MARGINS_BRKPNT, QsciScintilla::SymbolMargin);
	setMarginWidth(EDITORCPP_MARGINS_BRKPNT, 20);
	setMarginMarkerMask(EDITORCPP_MARGINS_BRKPNT, (1 << EDITORCPP_MARKS_BRKPNT_EN) \
			                                    | (1 << EDITORCPP_MARKS_BRKPNT_DIS));
	setMarginSensitivity(EDITORCPP_MARGINS_BRKPNT, true);

	/* Covearge background without margin,
	 * although Background = SC_MARK_BACKGROUND but we need use Background,
	 * because it is effect override.
	 * !!! setMarkerBackgroundColor argument color must with alpha,
	 * !!! if not, the background will invalid*/
	markerDefine(Background, EDITORCPP_MARKS_COV_YES);
	setMarkerBackgroundColor(m_cov_color_yes, EDITORCPP_MARKS_COV_YES);
	markerDefine(Background, EDITORCPP_MARKS_COV_PART);
	setMarkerBackgroundColor(m_cov_color_part, EDITORCPP_MARKS_COV_PART);
	markerDefine(Background, EDITORCPP_MAKER_COV_NO);
	setMarkerBackgroundColor(m_cov_color_no, EDITORCPP_MAKER_COV_NO);

	return;
}

int EditorCpp::arrowClear()
{
	markerDeleteAll(EDITORCPP_MARKS_ARROW);
	return 0;
}

int EditorCpp::arrowSet(unsigned int line)
{
	/*UEditor line number begin from 0*/
	line -= 1;

	markerAdd(line, EDITORCPP_MARKS_ARROW);
	ensureLineVisible(line);
	return 0;
}

int EditorCpp::brkpntSet(unsigned int line, bool enable)
{
	/*UEditor line number begin from 0*/
	line -= 1;

	markerDelete(line, EDITORCPP_MARKS_BRKPNT_EN);
	markerDelete(line, EDITORCPP_MARKS_BRKPNT_DIS);
	if (enable) {
		markerAdd(line, EDITORCPP_MARKS_BRKPNT_EN);
	} else {
		markerAdd(line, EDITORCPP_MARKS_BRKPNT_DIS);
	}
	return 0;
}

int EditorCpp::brkpntUnSet(unsigned int line)
{
	/*UEditor line number begin from 0*/
	line -= 1;

	markerDelete(line, EDITORCPP_MARKS_BRKPNT_EN);
	markerDelete(line, EDITORCPP_MARKS_BRKPNT_DIS);
	return 0;
}

int EditorCpp::fileCovSet(unsigned int line, unsigned int type)
{
	switch(type)
	{

	case FILECOV_YES:
	{
		markerAdd(line, EDITORCPP_MARKS_COV_YES);
		break;
	}
	case FILECOV_PART:
	{
		markerAdd(line, EDITORCPP_MARKS_COV_PART);
		break;
	}
	case FILECOV_NO:
	{
		markerAdd(line, EDITORCPP_MAKER_COV_NO);
		break;
	}
	case FILECOV_IGNORE:
	{
		/*do nothing*/
		break;
	}
	}

	return 0;
}

int EditorCpp::fileCovClear()
{
	markerDeleteAll(EDITORCPP_MARKS_COV_YES);
	markerDeleteAll(EDITORCPP_MARKS_COV_PART);
	markerDeleteAll(EDITORCPP_MAKER_COV_NO);
	return 0;
}

int EditorCpp::fileCovPrev(int line, bool flag_no, bool flag_part)
{
	unsigned int mask = 0;
	if (flag_no) {
		mask |= 1 << EDITORCPP_MAKER_COV_NO;
	}
	if (flag_part) {
		mask |= 1 << EDITORCPP_MARKS_COV_PART;
	}

	/*Locate line*/
	return markerFindPrevious(line, mask);
}

int EditorCpp::fileCovNext(int line, bool flag_no, bool flag_part)
{
	unsigned int mask = 0;
	if (flag_no) {
		mask |= 1 << EDITORCPP_MAKER_COV_NO;
	}
	if (flag_part) {
		mask |= 1 << EDITORCPP_MARKS_COV_PART;
	}

	/*Locate line*/
	return markerFindNext(line, mask);
}

EditorPy::EditorPy(QString path, QWidget *parent)
: UEditor(path, parent)
{
	QMetaObject::Connection conn;

	m_act_copy2console = new QAction("Copy/Paste to Console", this);

	conn = connect(m_act_copy2console, SIGNAL(triggered()), this, SLOT(slotActCopy2Console()));
	UTER_ASSERT(conn != NULL);
	return;
}

void EditorPy::slotActCopy2Console()
{
	QString select_text = selectedText();
	g_mainwin->m_con_win->pasteToConsole(select_text);
	return;
}

void EditorPy::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu *menu = createStandardContextMenu();
	menu->setAttribute(Qt::WA_DeleteOnClose);		/*Delete it after use*/

	/*Add my action item to standard menu*/
	menu->addSeparator();
	menu->addAction(m_act_copy2console);

	/*Set action status*/
	if (hasSelectedText()) {
		m_act_copy2console->setEnabled(true);
	} else {
		m_act_copy2console->setEnabled(false);
	}

	QPoint mouse_at = QCursor::pos();
	menu->exec(mouse_at);
	return;
}

namespace EditUi {

StatusBar::StatusBar(QWidget *parent)
: QWidget(parent)
{
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);
	setLayout(layout);

	m_label = new QLabel(this);
	m_label->setMargin(0);
	layout->addWidget(m_label);

	return;
}

/*Flush status bar for edit*/
int StatusBar::show(UEditor *edit)
{
	if (edit == NULL) {
		m_label->setText("");
		return 0;
	}

	int line = 0; int index = 0;
	edit->getCursorPosition(&line, &index);

	const char *mode;
	if (edit->overwriteMode()) {
		mode = "Overwrite";
	} else {
		mode = "Insert";
	}

	m_label->setText(QString::asprintf("%s, %d:%d", mode, line + 1, index + 1));
	return 0;
}

FindListDlg::FindListDlg(QWidget *parent)
: QDialog(parent)
{
	QMetaObject::Connection conn;

	/*Modeless windows, but need on the top*/
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

	/* Out layout
	 * ---------------------------
	 *               |  Close
	 *  Edit         |  Goto
	 *               |  Copy to chipboard
	 * ---------------------------*/
	QHBoxLayout *out_layout = new QHBoxLayout(this);
	setLayout(out_layout);

	/*Left layout (edit)*/
	m_list_edit = new EditorBase(EditorBase::FILETYPE_UNKNOWN, this);
	out_layout->addWidget(m_list_edit);
	m_list_edit->setReadOnly(true);

	/*Right layout (Buttons)*/
	QVBoxLayout *btn_layout = new QVBoxLayout(this);
	out_layout->addLayout(btn_layout);
	btn_layout->setAlignment(Qt::AlignTop);

	m_btn_close = new QPushButton("Close", this);
	m_btn_goto = new QPushButton("Goto", this);
	m_btn_clipboard = new QPushButton("Copy to Chipboard", this);
	btn_layout->addWidget(m_btn_close);
	btn_layout->addWidget(m_btn_goto);
	btn_layout->addWidget(m_btn_clipboard);

	m_close_quiet = false;
	m_changing = false;

	conn = connect(m_list_edit, SIGNAL(cursorPositionChanged(int, int)),
			       this, SLOT(slotCursorPositionChanged(int, int)));
	UTER_ASSERT(conn != NULL);

	conn = connect(m_btn_close, SIGNAL(pressed()), this, SLOT(slotBtnClose()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_btn_goto, SIGNAL(pressed()), this, SLOT(slotBtnGoto()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_btn_clipboard, SIGNAL(pressed()), this, SLOT(slotBtnClipBoard()));
	UTER_ASSERT(conn != NULL);
	return;
}

void FindListDlg::slotBtnClose()
{
	close();
	return;
}

void FindListDlg::slotBtnGoto()
{
	int line; int index;
	m_list_edit->getCursorPosition(&line, &index);

	/*Goto edit in mdi, not come back to Find/Replace*/
	m_close_quiet = true;
	close();

	UEditor *edit = g_mainwin->editGetByPath(m_path);
	if (edit == NULL) {
		return;
	}
	g_mainwin->editSetActive(edit);

	/*Find line*/
	QMap<int, int>::iterator iter;
	iter = m_line_map.find(line);
	if (iter == m_line_map.end()) {
		return;
	}
	int line_no = iter.value();

	/*Show line*/
	edit->selectLine(line_no);
	edit->ensureLineVisible(line_no);
	return;
}

void FindListDlg::slotBtnClipBoard()
{
	QString string = m_list_edit->text();
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(string);
	return;
}

void FindListDlg::slotCursorPositionChanged(int line, int index)
{
	if (m_changing) {
		return;
	}

	UEditor *edit = g_mainwin->editGetByPath(m_path);
	if (edit == NULL) {
		return;
	}
	g_mainwin->editSetActive(edit);

	/*Find line*/
	QMap<int, int>::iterator iter;
	iter = m_line_map.find(line);
	if (iter == m_line_map.end()) {
		return;
	}
	int line_no = iter.value();

	/*Show line*/
	edit->selectLine(line_no);
	edit->ensureLineVisible(line_no);
	return;
}

void FindListDlg::showEvent(QShowEvent *event)
{
	m_close_quiet = false;
	return;
}

void FindListDlg::closeEvent(QCloseEvent *event)
{
	if (m_close_quiet) {
		/*If set m_close_quiet, just close it*/
		return;
	}

	/*Emit signal to FindReplace window*/
	emit signalCloseEvent();
	return;
}

int FindListDlg::append(QString linestr, int line_no)
{
	m_changing = true;

	int self_line_no = m_list_edit->lines() - 1;
	m_list_edit->append(linestr);
	m_line_map.insert(self_line_no, line_no);

	m_changing = false;
	return 0;
}

int FindListDlg::clear()
{
	m_list_edit->clear();
	m_line_map.clear();
	return 0;
}

FindReplaceDlg::FindReplaceDlg(QWidget *parent)
: QDialog(parent)
{
	QMetaObject::Connection conn;

	/*Modeless windows, but need on the top*/
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

	/* Out layout
	 * ----------------------------
	 * Find & replace input
	 * ----------------------------
	 * Forward & backward
	 * ----------------------------
	 * Buttons
	 * ----------------------------
	 *                      close
	 * ----------------------------*/
	QVBoxLayout *out_layout = new QVBoxLayout(this);
	setLayout(out_layout);

	/*Input layout*/
	QGridLayout *input_layout = new QGridLayout(this);
	out_layout->addLayout(input_layout);

	QLabel *find_lable = new QLabel("Find:", this);
	QLabel *replace_lable = new QLabel("Replace with:", this);
	m_edit_find = new QLineEdit(this);
	m_edit_replace = new QLineEdit(this);

	input_layout->addWidget(find_lable, 0, 0, 1, 1);
	input_layout->addWidget(m_edit_find, 0, 1, 1, 1);
	input_layout->addWidget(replace_lable, 1, 0, 1, 1);
	input_layout->addWidget(m_edit_replace, 1, 1, 1, 1);

	out_layout->addStretch();

	/*Ward layout*/
	QGroupBox *ward_group = new QGroupBox(this);
	out_layout->addWidget(ward_group);
	QHBoxLayout *ward_layout = new QHBoxLayout(this);
	ward_group->setLayout(ward_layout);
	m_radio_forward = new QRadioButton("Forward", this);
	m_radio_backward = new QRadioButton("Backward", this);
	ward_layout->addWidget(m_radio_forward);
	ward_layout->addWidget(m_radio_backward);

	m_radio_forward->setChecked(true);

	out_layout->addStretch();

	/*Option layout*/
	QGroupBox *opt_group = new QGroupBox(this);
	out_layout->addWidget(opt_group);
	QGridLayout *opt_layout = new QGridLayout(this);
	opt_group->setLayout(opt_layout);
	m_check_case = new QCheckBox("Case sensitive", this);
	m_check_wrapsearch = new QCheckBox("Wrap search", this);
	m_check_wholeword = new QCheckBox("Whole word", this);

	opt_layout->addWidget(m_check_case, 0, 0, 1, 1);
	opt_layout->addWidget(m_check_wrapsearch, 0, 1, 1, 1);
	opt_layout->addWidget(m_check_wholeword, 1, 0, 1, 1);

	out_layout->addStretch();

	/*Buttons layout*/
	QGridLayout *btn_layout = new QGridLayout(this);
	out_layout->addLayout(btn_layout);
	m_btn_find = new QPushButton("Find", this);
	m_btn_list = new QPushButton("List", this);
	m_btn_replace = new QPushButton("Replace", this);
	m_btn_replaceall = new QPushButton("Replace all", this);

	btn_layout->addWidget(m_btn_find, 0, 0, 1, 1);
	btn_layout->addWidget(m_btn_list, 0, 1, 1, 1);
	btn_layout->addWidget(m_btn_replace, 1, 0, 1, 1);
	btn_layout->addWidget(m_btn_replaceall, 1, 1, 1, 1);

	out_layout->addStretch();

	/*Close layout*/
	m_btn_close = new QPushButton("Close", this);
	out_layout->addWidget(m_btn_close, 0, Qt::AlignRight);

	conn = connect(m_btn_find, SIGNAL(pressed()), this, SLOT(slotbtnFindPressed()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_btn_list, SIGNAL(pressed()), this, SLOT(slotbtnListPressed()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_btn_replace, SIGNAL(pressed()), this, SLOT(slotbtnReplacePressed()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_btn_replaceall, SIGNAL(pressed()), this, SLOT(slotbtnReplaceAllPressed()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_btn_close, SIGNAL(pressed()), this, SLOT(slotbtnClosePressed()));
	UTER_ASSERT(conn != NULL);

	/*Find list closed*/
	conn = connect(&m_find_list, SIGNAL(signalCloseEvent()), this, SLOT(slotListClose()));
	UTER_ASSERT(conn != NULL);
	return;
}

void FindReplaceDlg::slotbtnFindPressed()
{
	UEditor *edit = g_mainwin->editGetActive();
	if (edit == NULL) {
		return;
	}

	QString find_string = m_edit_find->text();
	bool case_sensitive = m_check_case->isChecked();
	bool wholeword = m_check_wholeword->isChecked();
	bool wrapsearch = m_check_wrapsearch->isChecked();
	bool forward = m_radio_forward->isChecked();

	if (! forward) {
		/*Adjust selection when backward*/
		if (edit->hasSelectedText()) {
			int lineFrom; int indexFrom; int lineTo; int indexTo;
			edit->getSelection(&lineFrom, &indexFrom, &lineTo, &indexTo);
			edit->setCursorPosition(lineFrom, indexFrom);
		}
	}

	edit->findFirst(find_string, false, case_sensitive, wholeword, wrapsearch, forward);
	return;
}

void FindReplaceDlg::slotbtnListPressed()
{
	UEditor *edit = g_mainwin->editGetActive();
	if (edit == NULL) {
		return;
	}

	QString find_string = m_edit_find->text();

	m_find_list.m_path = edit->m_file_path;
	m_find_list.clear();
	int line_count = edit->lines();
	int iloop;
	for (iloop = 0; iloop < line_count; iloop++) {
		QString line_string = edit->text(iloop);
		if (line_string.indexOf(find_string) < 0) {
			continue;
		}
		m_find_list.append(line_string, iloop);
	}

	/*Close find/replace, show list*/
	close();
	m_find_list.show();
	return;
}

void FindReplaceDlg::slotbtnReplacePressed()
{
	UEditor *edit = g_mainwin->editGetActive();
	if (edit == NULL) {
		return;
	}

	/*Do find*/
	slotbtnFindPressed();

	/*Replace*/
	QString replace_string = m_edit_replace->text();
	edit->replace(replace_string);
	return;
}

void FindReplaceDlg::slotbtnReplaceAllPressed()
{
	UEditor *edit = g_mainwin->editGetActive();
	if (edit == NULL) {
		return;
	}

	QString find_string = m_edit_find->text();
	QString replace_string = m_edit_replace->text();
	bool case_sensitive = m_check_case->isChecked();
	bool wholeword = m_check_wholeword->isChecked();

	edit->setCursorPosition(0, 0);
	while (1) {
		bool bresult = edit->findFirst(find_string, false,
									   case_sensitive, wholeword, false, true);
		if (! bresult) {
			break;
		}
		edit->replace(replace_string);
	}

	return;
}

void FindReplaceDlg::slotbtnClosePressed()
{
	close();
	return;
}

void FindReplaceDlg::slotListClose()
{
	/*Reshow*/
	show();
	return;
}

EditDlg::EditDlg()
{
	QMetaObject::Connection conn;

	/*Toolbar & menu*/
	UiMenuSetup *menu = g_mainwin->menuSetup(UiMenuSetup::SEQ_EDIT, "&Edit");
	UiToolBarSetup *toolbar = g_mainwin->toolBarSetup(UiToolBarSetup::SEQ_EDIT, "&Edit");
	UiStatusBarSetup *statusbar = g_mainwin->statusBarSetup(UiStatusBarSetup::SEQ_EDIT, "EditStatus");

	m_edit_act_undo = new QAction("&Undo", this);
	m_edit_act_undo->setShortcuts(QKeySequence::Undo);
	m_edit_act_undo->setStatusTip("Undo");
    menu->addAction(m_edit_act_undo);

	m_edit_act_redo = new QAction("&Redo", this);
	m_edit_act_redo->setShortcuts(QKeySequence::Redo);
	m_edit_act_redo->setStatusTip("Redo");
    menu->addAction(m_edit_act_redo);

	m_edit_act_undo->setEnabled(false);
	m_edit_act_redo->setEnabled(false);

	menu->addAction(NULL);

    m_edit_act_cut = new QAction("&Cut", this);
    m_edit_act_cut->setShortcuts(QKeySequence::Delete);
    m_edit_act_cut->setStatusTip("Cut");
    m_edit_act_cut->setIcon(QIcon(":/images/edit_cut.png"));
    menu->addAction(m_edit_act_cut);
    toolbar->addAction(m_edit_act_cut);

	m_edit_act_copy = new QAction("&Copy", this);
	m_edit_act_copy->setShortcuts(QKeySequence::Copy);
	m_edit_act_copy->setStatusTip("Copy");
	m_edit_act_copy->setIcon(QIcon(":/images/edit_copy.png"));
    menu->addAction(m_edit_act_copy);
    toolbar->addAction(m_edit_act_copy);

	m_edit_act_paste = new QAction("&Paste", this);
	m_edit_act_paste->setShortcuts(QKeySequence::Paste);
	m_edit_act_paste->setStatusTip("Paste");
	m_edit_act_paste->setIcon(QIcon(":/images/edit_paste.png"));
    menu->addAction(m_edit_act_paste);
    toolbar->addAction(m_edit_act_paste);

    m_edit_act_cut->setEnabled(false);
    m_edit_act_copy->setEnabled(false);
    m_edit_act_paste->setEnabled(false);

    menu->addAction(NULL);

    m_edit_act_delete = new QAction("&Delete", this);
    m_edit_act_delete->setShortcuts(QKeySequence::Delete);
    m_edit_act_delete->setStatusTip("Delete");
    menu->addAction(m_edit_act_delete);

	m_edit_act_selectall = new QAction("&Select All", this);
	m_edit_act_selectall->setStatusTip("Select All");
    menu->addAction(m_edit_act_selectall);

	m_edit_act_delete->setEnabled(false);
	m_edit_act_selectall->setEnabled(false);

	menu->addAction(NULL);

	m_edit_act_gotoline = new QAction("&Goto Line", this);
	m_edit_act_gotoline->setStatusTip("Goto Line");
    menu->addAction(m_edit_act_gotoline);

	m_edit_act_findreplace = new QAction("&Find/Replace", this);
	m_edit_act_findreplace->setStatusTip("Find/Replace");
    menu->addAction(m_edit_act_findreplace);

	m_edit_act_gotoline->setEnabled(false);
	m_edit_act_findreplace->setEnabled(false);

	menu->addAction(NULL);

	m_edit_act_viewlineno = new QAction("&View Line Number", this);
	m_edit_act_viewlineno->setStatusTip("View Line Number");
    menu->addAction(m_edit_act_viewlineno);

	m_edit_act_viewlineno->setCheckable(true);
	if (g_ui_main.m_config_edit_view_line_number) {
		g_mainwin->editSetLineNoEnable(true);
		m_edit_act_viewlineno->setChecked(true);
	} else {
		g_mainwin->editSetLineNoEnable(false);
		m_edit_act_viewlineno->setChecked(false);
	}

	/*Status bar*/
	m_status_bar = new StatusBar(this);
	statusbar->addWidget(m_status_bar);

	conn = connect(m_edit_act_undo, SIGNAL(triggered()), this, SLOT(slotEditUndo()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_edit_act_redo, SIGNAL(triggered()), this, SLOT(slotEditRedo()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_edit_act_cut, SIGNAL(triggered()), this, SLOT(slotEditCut()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_edit_act_copy, SIGNAL(triggered()), this, SLOT(slotEditCopy()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_edit_act_paste, SIGNAL(triggered()), this, SLOT(slotEditPaste()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_edit_act_delete, SIGNAL(triggered()), this, SLOT(slotEditDelete()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_edit_act_selectall, SIGNAL(triggered()), this, SLOT(slotEditSelectAll()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_edit_act_gotoline, SIGNAL(triggered()), this, SLOT(slotEditGotoLine()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_edit_act_findreplace, SIGNAL(triggered()), this, SLOT(slotEditFindReplace()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_edit_act_viewlineno, SIGNAL(triggered()), this, SLOT(slotEditViewLineNo()));
	UTER_ASSERT(conn != NULL);

	conn = connect(g_mainwin->m_center_mdi, SIGNAL(signalActiveChange()), this, SLOT(slotEditActionFlush()));
	UTER_ASSERT(conn != NULL);

	/*Signal about clipboard*/
    QClipboard *clipboard = QApplication::clipboard();
    UTER_ASSERT(clipboard != NULL);
	conn = connect(clipboard, SIGNAL(dataChanged()), this, SLOT(slotEditActionFlush()));
	UTER_ASSERT(conn != NULL);

	return;
}

void EditDlg::slotEditUndo()
{
	UEditor *edit = g_mainwin->editGetActive();
	if (edit == NULL) {
		return;
	}
	edit->undo();
	return;
}

void EditDlg::slotEditRedo()
{
	UEditor *edit = g_mainwin->editGetActive();
	if (edit == NULL) {
		return;
	}
	edit->redo();
	return;
}

void EditDlg::slotEditCut()
{
	UEditor *edit = g_mainwin->editGetActive();
	if (edit == NULL) {
		return;
	}
	edit->cut();
	return;
}

void EditDlg::slotEditCopy()
{
	UEditor *edit = g_mainwin->editGetActive();
	if (edit == NULL) {
		return;
	}
	edit->copy();
	return;
}

void EditDlg::slotEditPaste()
{
	UEditor *edit = g_mainwin->editGetActive();
	if (edit == NULL) {
		return;
	}
	edit->paste();
	return;
}

void EditDlg::slotEditDelete()
{
	UEditor *edit = g_mainwin->editGetActive();
	if (edit == NULL) {
		return;
	}
	/*No delete interface of QsciScintilla*/
	QKeyEvent event(QEvent::KeyPress, Qt::Key_Delete, Qt::NoModifier);
	edit->keyPressEvent(&event);
	return;
}

void EditDlg::slotEditSelectAll()
{
	UEditor *edit = g_mainwin->editGetActive();
	if (edit == NULL) {
		return;
	}
	edit->selectAll(true);
	return;
}

void EditDlg::slotEditGotoLine()
{
	UEditor *edit = g_mainwin->editGetActive();
	if (edit == NULL) {
		return;
	}

	int line_count = edit->lines();
	bool isok;
	int result = QInputDialog::getInt(NULL, "Goto Line", "Line number:",
								/*value*/1, /*min value*/1, /*max value*/line_count,
								/*step*/1, &isok);
	if (isok) {
		edit->setFirstVisibleLine(result - 1);
	}
	return;
}

/*Find & replace on current active edit*/
void EditDlg::slotEditFindReplace()
{
	UEditor *edit = g_mainwin->editGetActive();
	if (edit == NULL) {
		return;
	}

	m_find_replace.show();
	return;
}

/*About view line number*/
void EditDlg::slotEditViewLineNo()
{
	if (m_edit_act_viewlineno->isChecked()) {
		g_mainwin->editSetLineNoEnable(true);
		g_ui_main.m_config_edit_view_line_number = true;
	} else {
		g_mainwin->editSetLineNoEnable(false);
		g_ui_main.m_config_edit_view_line_number = false;
	}
	g_ui_main.configSave();
	return;
}

/* Flush after:
 * O, mdi active item changed
 * O, clipboard status changed
 * O, select ranged changed
 * O, position changed
 * O, insert/overwrite changed
 * O, focus changed
 * */
void EditDlg::slotEditActionFlush()
{
	UEditor *edit;
	QWidget *current_focusat = QApplication::focusWidget();
	if (current_focusat == NULL
    	|| (edit = qobject_cast<UEditor*>(current_focusat)) == NULL) {

		m_edit_act_undo->setEnabled(false);
		m_edit_act_redo->setEnabled(false);

	    m_edit_act_cut->setEnabled(false);
	    m_edit_act_copy->setEnabled(false);
	    m_edit_act_paste->setEnabled(false);

	    m_edit_act_delete->setEnabled(false);
		m_edit_act_selectall->setEnabled(false);

		m_edit_act_gotoline->setEnabled(false);
		m_edit_act_findreplace->setEnabled(false);

		m_status_bar->show(NULL);
		return;
	}

	m_edit_act_undo->setEnabled(true);
	m_edit_act_redo->setEnabled(true);

	/*About select*/
	if (edit->hasSelectedText()) {
	    m_edit_act_cut->setEnabled(true);
	    m_edit_act_copy->setEnabled(true);

	    m_edit_act_delete->setEnabled(true);
	} else {
	    m_edit_act_cut->setEnabled(false);
	    m_edit_act_copy->setEnabled(false);

	    m_edit_act_delete->setEnabled(false);
	}

	/*About clipboard*/
	QClipboard *clipboard = QApplication::clipboard();
	if (clipboard->text().length() != 0) {
		m_edit_act_paste->setEnabled(true);
	} else {
		m_edit_act_paste->setEnabled(false);
	}

	m_edit_act_selectall->setEnabled(true);

	m_edit_act_gotoline->setEnabled(true);
	m_edit_act_findreplace->setEnabled(true);

	m_status_bar->show(edit);
	return;
}

} /*namespace EditUi*/

namespace FileUi {

FileNewDlg::FileNewDlg(QString dir, QWidget *parent)
:QDialog(parent)
{
	QMetaObject::Connection conn;

	setWindowTitle("New file");

	/* Out layout
	 * -----------------------------
	 *         input ware
	 * -----------------------------
	 *         space
	 * -----------------------------
	 *                       Buttons
	 * -----------------------------*/
	QVBoxLayout *top_layout = new QVBoxLayout(this);
	setLayout(top_layout);
	top_layout->setAlignment(Qt::AlignBottom);

	/* Main layout
	 * ---------------------------------
	 * Source folder:     dir input          Browse
	 * Source file:       file input
	 * */
	QGridLayout *main_layout = new QGridLayout(this);
	top_layout->addLayout(main_layout);

	QLabel *label_dir = new QLabel("Source folder:", this);
	QLabel *label_file = new QLabel("Source file:", this);
	m_dir = new QLineEdit(this);
	m_file = new QLineEdit(this);
	m_browse = new QPushButton("Browse...", this);

	main_layout->addWidget(label_dir, 0, 0, 1, 1);
	main_layout->addWidget(m_dir, 0, 1, 1, 1);
	main_layout->addWidget(m_browse, 0, 2, 1, 1);
	main_layout->addWidget(label_file, 1, 0, 1, 1);
	main_layout->addWidget(m_file, 1, 1, 1, 1);

	/*Space*/
	top_layout->addSpacing(100);

	/*Button layout*/
	QHBoxLayout *btn_layout = new QHBoxLayout(this);
	top_layout->addLayout(btn_layout);

	m_ok = new QPushButton("OK", this);
	m_cancel = new QPushButton("Cancel", this);

	btn_layout->addWidget(m_ok);
	btn_layout->addWidget(m_cancel);
	btn_layout->setAlignment(Qt::AlignRight);

	if (dir.isEmpty()) {
		m_dir->setText(QDir::homePath());
	} else {
		m_dir->setText(dir);
	}
	m_result = "";

	setFixedWidth(700);

	conn = connect(m_browse, SIGNAL(pressed()), this, SLOT(slotBtnBrowse()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_ok, SIGNAL(pressed()), this, SLOT(slotBtnOk()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_cancel, SIGNAL(pressed()), this, SLOT(slotBtnCancel()));
	UTER_ASSERT(conn != NULL);

	//--------------------------------------------------------------

	QPushButton *test_btn = new QPushButton(this);
	top_layout->addWidget(test_btn);

	m_test_area = new QMdiArea(this);
	m_test_area->setViewMode(QMdiArea::TabbedView);
	top_layout->addWidget(m_test_area);

	conn = connect(test_btn, SIGNAL(pressed()), this, SLOT(slotBtnTest()));

	return;
}

void FileNewDlg::slotBtnTest()
{
	QTextEdit *edit = new QTextEdit(this);
	QMdiSubWindow *sub = new QMdiSubWindow(this);
	sub->setWidget(edit);
	m_test_area->addSubWindow(sub);
	return;
}

void FileNewDlg::slotBtnOk()
{
	/*TODO:Some check*/
	m_result = m_dir->text() + QDir::separator() + m_file->text();
	close();
	return;
}

void FileNewDlg::slotBtnCancel()
{
	m_result = "";
	close();
	return;
}

void FileNewDlg::slotBtnBrowse()
{
	QString getpath = QFileDialog::getExistingDirectory(this, "Open directory.", m_dir->text(), QFileDialog::ShowDirsOnly);
	if (! getpath.isEmpty()) {
		m_dir->setText(getpath);
	}
	return;
}

FileDlg::FileDlg()
{
	QMetaObject::Connection conn;

	/*Toolbar & menu*/
	UiMenuSetup *menu = g_mainwin->menuSetup(UiMenuSetup::SEQ_FILE, "&File");
	UiToolBarSetup *toolbar = g_mainwin->toolBarSetup(UiToolBarSetup::SEQ_FILE, "&File");

	m_file_act_new = new QAction("&New", this);
	m_file_act_new->setShortcuts(QKeySequence::New);
	m_file_act_new->setStatusTip("Create a new file");
	m_file_act_new->setIcon(QIcon(":/images/file_new.png"));
	menu->addAction(m_file_act_new);
	toolbar->addAction(m_file_act_new);

	m_file_act_open = new QAction("&Open", this);
	m_file_act_open->setShortcuts(QKeySequence::Open);
	m_file_act_open->setStatusTip("Open file");
	m_file_act_open->setIcon(QIcon(":/images/file_open.png"));
	menu->addAction(m_file_act_open);
	toolbar->addAction(m_file_act_open);

	m_file_act_save = new QAction("&Save", this);
	m_file_act_save->setShortcuts(QKeySequence::Save);
	m_file_act_save->setStatusTip("Save file");
	m_file_act_save->setIcon(QIcon(":/images/file_save.png"));
	menu->addAction(m_file_act_save);
	toolbar->addAction(m_file_act_save);

	m_file_act_saveas = new QAction("&Save As", this);
	m_file_act_saveas->setShortcuts(QKeySequence::SaveAs);
	m_file_act_saveas->setStatusTip("Save As ..");
	//m_file_act_saveas->setIcon(QIcon(":/images/file_save.png"));
	menu->addAction(m_file_act_saveas);

	m_file_act_saveall = new QAction("&Save All", this);
	m_file_act_saveall->setStatusTip("Save all files");
	m_file_act_saveall->setIcon(QIcon(":/images/file_saveall.png"));
	menu->addAction(m_file_act_saveall);
	toolbar->addAction(m_file_act_saveall);

	menu->addAction(NULL);

    m_file_act_quit = new QAction("&Exit", this);
    m_file_act_quit->setShortcuts(QKeySequence::Quit);
    m_file_act_quit->setStatusTip("Quit");
	menu->addAction(m_file_act_quit);

    m_file_act_save->setEnabled(false);
    m_file_act_saveas->setEnabled(false);
    m_file_act_saveall->setEnabled(false);

    m_last_open_dir = "";

	conn = connect(m_file_act_new, SIGNAL(triggered()), this, SLOT(slotFileNew()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_file_act_open, SIGNAL(triggered()), this, SLOT(slotFileOpen()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_file_act_save, SIGNAL(triggered()), this, SLOT(slotFileSave()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_file_act_saveas, SIGNAL(triggered()), this, SLOT(slotFileSaveas()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_file_act_saveall, SIGNAL(triggered()), this, SLOT(slotFileSaveall()));
	UTER_ASSERT(conn != NULL);
    conn = connect(m_file_act_quit, SIGNAL(triggered()), this, SLOT(slotFileQuit()));
    UTER_ASSERT(conn != NULL);

	conn = connect(g_mainwin->m_center_mdi, SIGNAL(signalActiveChange()), this, SLOT(slotEditActiveFlush()));
	UTER_ASSERT(conn != NULL);
	return;
}

void FileDlg::slotFileNew()
{
	FileNewDlg newfile(m_last_open_dir, this);
	newfile.exec();
	if (newfile.m_result == "") {
		g_mainwin->statusBar()->showMessage("No file selected.", 2000);
		return;
	}
	QString cleanpath = QDir::cleanPath(newfile.m_result);

	QFileInfo fileinfo(cleanpath);
	if (! fileinfo.exists()) {
		/*Create empty file*/
		QFile file(cleanpath);
		if (! file.open(QFile::ReadWrite | QFile::Text | QFile::Truncate)) {
			g_mainwin->statusBar()->showMessage("Create file failed.", 2000);
			return;
		}
		file.close();
	}

	m_last_open_dir = fileinfo.path();

	/*Open file in mdi*/
	UEditor *editor = g_mainwin->editGetByPath(cleanpath);
	if (editor == NULL) {
		editor = UEditor::createEditByPath(cleanpath);
		g_mainwin->editAddToMdi(editor);
	}
	g_mainwin->editSetActive(editor);
	return;
}

/*Open files*/
void FileDlg::slotFileOpen()
{
	QString open_at;
	if (m_last_open_dir == "") {
		open_at = QDir::homePath();
	} else {
		open_at = m_last_open_dir;
	}

	QString open_filter = "Code&Txt (*.c *.h *.cpp *.hpp *.cxx *.hxx *.py *.txt *.html *.cfg *.ini *.mk Makefile);;AllFile (*.*)";
	QStringList getfilelist = QFileDialog::getOpenFileNames(this, "Open files", open_at, open_filter);
	if (getfilelist.isEmpty()) {
		g_mainwin->statusBar()->showMessage("No file selected.", 2000);
		return;
	}

	QStringList::iterator iter;
	for (iter = getfilelist.begin(); iter != getfilelist.end(); iter++) {
		QString filepath = *iter;
		QString cleanpath = QDir::cleanPath(filepath);

		QFileInfo fineinfo(cleanpath);
		if (! fineinfo.exists()) {
			/*No such file*/
			g_mainwin->statusBar()->showMessage("File not exist.", 2000);
			continue;
		}

		m_last_open_dir = fineinfo.path();

		/*Open file in mdi*/
		UEditor *editor = g_mainwin->editGetByPath(cleanpath);
		if (editor == NULL) {
			editor = UEditor::createEditByPath(cleanpath);
			g_mainwin->editAddToMdi(editor);
		}
		g_mainwin->editSetActive(editor);
	}

	return;
}

void FileDlg::slotFileSave()
{
	UEditor *edit = g_mainwin->editGetActive();
	if (edit == NULL) {
		return;
	}

	edit->saveFile();
	return;
}

void FileDlg::slotFileSaveas()
{
	UEditor *edit = g_mainwin->editGetActive();
	if (edit == NULL) {
		return;
	}

	QString open_at;
	if (m_last_open_dir == "") {
		open_at = QDir::homePath();
	} else {
		open_at = m_last_open_dir;
	}

	QString open_filter = "Code&Txt (*.c *.h *.cpp *.hpp *.cxx *.hxx *.py *.txt *.html *.cfg *.ini *.mk Makefile);;AllFile (*.*)";
	QString filepath = QFileDialog::getSaveFileName(this, "Save files", open_at, open_filter);
	if (filepath.isEmpty()) {
		/*If no file name or do not want override*/
		g_mainwin->statusBar()->showMessage("No file selected.", 2000);
		return;
	}

	QString cleanpath = QDir::cleanPath(filepath);
	edit->saveFile(cleanpath);
	return;
}

void FileDlg::slotFileSaveall()
{
	QList<MdiItem *> mdiitemlist = g_mainwin->m_center_mdi->getItemList();
	QList<MdiItem *>::iterator iter;
	for (iter = mdiitemlist.begin(); iter != mdiitemlist.end(); iter++) {
		MdiItem *mdiitem = *iter;
		UEditor *edit = (UEditor *)mdiitem->widget();
		edit->saveFile();
	}

	return;
}

void FileDlg::slotFileQuit()
{
	QApplication::exit();
	return;
}

/*Some file selected in mdi, flush toolbar & menu*/
void FileDlg::slotEditActiveFlush()
{
	UEditor *edit = g_mainwin->editGetActive();
	if (edit == NULL) {
	    m_file_act_save->setEnabled(false);
	    m_file_act_saveas->setEnabled(false);
	    m_file_act_saveall->setEnabled(false);
		return;
	}

	if (edit->isModified()) {
		m_file_act_save->setEnabled(true);
	} else {
		m_file_act_save->setEnabled(false);
	}

    m_file_act_saveas->setEnabled(true);

    QList<MdiItem *> itemlist = g_mainwin->m_center_mdi->getItemList();
    QList<MdiItem *>::iterator iter;
    for (iter = itemlist.begin(); iter != itemlist.end(); iter++) {
    	MdiItem *mdiitem = *iter;
		UEditor *editor = (UEditor *)mdiitem->widget();
		if (editor->isModified()) {
			break;
		}
    }
    if (iter == itemlist.end()) {
    	m_file_act_saveall->setEnabled(false);
    } else {
    	m_file_act_saveall->setEnabled(true);
    }

	return;
}
} /*namespace FileUi*/
