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
#include <tester.hxx>

#include <ui_mainwin.hxx>

namespace ConUi {

bool pythonSyntaxCheck(QString &input)
{
	/*Python path*/
	char buf[PATH_MAX] = {0};
	int result = readlink("/proc/self/exe", buf, PATH_MAX);
	if (result == -1 || result == PATH_MAX) {
		return false;
	}
	QFileInfo fileinfo(QString::fromLatin1(buf));
	QString python_path = fileinfo.path() + "/../lib/Python-2.7.9/bin/python2.7";

	/*Code file*/
	QString code_path = QDir::tempPath() + QDir::separator() + QUuid::createUuid().toString() + ".py";
	QFile file(code_path);
	if (! file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		return false;
	}
	file.write(input.toLatin1());
	file.close();

	/*!!!!Code file need delete!!!!*/

	/*Execute compile*/
	QProcess process;
	QStringList arguments;
	arguments << "-m" << "py_compile" << code_path;
	process.start(python_path, arguments);

	if (! process.waitForStarted()) {
		/*Start failed*/
		QFile::remove(code_path);
		QFile::remove(code_path + "c");
		return false;
	}

	/*No need write something to process stdin*/
	process.closeWriteChannel();

	/*Wait finished*/
	if (! process.waitForFinished()) {
		QFile::remove(code_path);
		QFile::remove(code_path + "c");
		return false;
	}

	/*Get error output*/
	QByteArray output_array = process.readAllStandardError();

	QFile::remove(code_path);
	QFile::remove(code_path + "c");

	if (output_array.length() == 0) {
		return true;
	} else {
		return false;
	}
}

/*Console mark for arrow*/
enum {
	PROMPT_MARK_ARROW = 1,
};

/*Console margin for arrow*/
enum {
	/*TODO: 24 times failed.*/
	//PROMPT_MARGINS_LINENO = 1,		/*Line number from edit area*/
	//PROMPT_MARGINS_ARROW = 2,
	PROMPT_MARGINS_ARROW = 1,
};

/*Console user list*/
enum {
	USERLIST_ID_CONSOLE = 1,
};

/*No any layout, just a control ware*/
Console::Console(QWidget *parent)
:QsciScintilla(parent)
{
	QMetaObject::Connection conn;

	/*Pop menu in console*/
	m_pop_menu = new QMenu(this);

	m_act_copy = new QAction("Copy", this);
	m_act_cut = new QAction("Cut", this);
	m_act_paste = new QAction("Paste", this);
	m_act_delete = new QAction("Delete", this);

	m_pop_menu->addAction(m_act_copy);
	m_pop_menu->addAction(m_act_cut);
	m_pop_menu->addAction(m_act_paste);
	m_pop_menu->addAction(m_act_delete);

	/*Mark and margin*/
	markerDefine(QsciScintilla::ThreeRightArrows, PROMPT_MARK_ARROW);

	/*TODO:*/
	//setMarginType(PROMPT_MARGINS_LINENO, QsciScintilla::TextMarginRightJustified);
	//setMarginWidth(PROMPT_MARGINS_LINENO, 30);
	//setMarginMarkerMask(PROMPT_MARGINS_LINENO, 0);

	setMarginType(PROMPT_MARGINS_ARROW, QsciScintilla::SymbolMargin);
	setMarginWidth(PROMPT_MARGINS_ARROW, 20);
	setMarginMarkerMask(PROMPT_MARGINS_ARROW, 1 << PROMPT_MARK_ARROW);

	/*Set indicator for tab and space*/
	m_indicator_tab = indicatorDefine(QsciScintilla::FullBoxIndicator);
	UTER_ASSERT(m_indicator_tab != -1);
	setIndicatorForegroundColor(QColor(0, 100, 0, 128), m_indicator_tab);

	m_indicator_space = indicatorDefine(QsciScintilla::FullBoxIndicator);
	UTER_ASSERT(m_indicator_space != -1);
	setIndicatorForegroundColor(QColor(0, 0, 100, 128), m_indicator_space);

	m_indicator_pyfl = indicatorDefine(QsciScintilla::PlainIndicator);
	UTER_ASSERT(m_indicator_pyfl != -1);

	/*File "/home/yaozhongkai/uter/build/release/lib/Python-2.7.9/lib/python2.7/unittest/runner.py", line 151, in run*/
	m_reg_pyfl = new QRegExp("File \"(\\/([0-9a-zA-Z\\_\\-\\.]+))+\", line [0-9]+");
	UTER_ASSERT(m_reg_pyfl->isValid());

	/*Turn off convert tab to space*/
	setTabIndents(false);
	/*Tab width is 4 character*/
	setTabWidth(4);
	/*Big cursor indicator, indicator between character and character, so can not too big*/
	setCaretWidth(40);

	/*Configuration initialization*/
	m_config_auto_submit = true;
	m_config_view_tabspace = false;
	m_history_index = -1;

	m_edit_start = 0;
	m_text_changging = false;

	/*Ready to input*/
	resetCurosr();
	resetPrompt();

	/*Pop menu*/
	conn = connect(m_act_copy, SIGNAL(triggered()), this, SLOT(slotActCopyTrigger()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_act_cut, SIGNAL(triggered()), this, SLOT(slotActCutTrigger()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_act_delete, SIGNAL(triggered()), this, SLOT(slotActDeleteTrigger()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_act_paste, SIGNAL(triggered()), this, SLOT(slotActPasteTrigger()));
	UTER_ASSERT(conn != NULL);

	conn = connect(this, SIGNAL(textChanged()), this, SLOT(slotTextChanged()));
	UTER_ASSERT(conn != NULL);

	conn = connect(this, SIGNAL(userListActivated(int, const QString&)),
			       this, SLOT(slotUserListActivated(int, const QString&)));
	UTER_ASSERT(conn != NULL);
	return;
}

void Console::slotActCopyTrigger()
{
	copy();
	return;
}

void Console::slotActCutTrigger()
{
	int result = cursorInEditArea();
	if (result < 0) {
		return;
	}
	cursorToEditArea();

	cut();

	submitAuto();
	return;
}

void Console::slotActDeleteTrigger()
{
	int result = cursorInEditArea();
	if (result < 0) {
		return;
	}
	cursorToEditArea();

	/*No delete interface of QsciScintilla*/
	QKeyEvent event(QEvent::KeyPress, Qt::Key_Delete, Qt::NoModifier);
	QsciScintilla::keyPressEvent(&event);

	submitAuto();
	return;
}

void Console::slotActPasteTrigger()
{
	int result = cursorInEditArea();
	if (result < 0) {
		return;
	}
	cursorToEditArea();

	paste();

	submitAuto();
	return;
}

void Console::slotTextChanged()
{
	if (m_text_changging) {
		return;
	}

	if (m_config_view_tabspace) {
		setIndicator();
	}
	flushLineNo();
	return;
}

/*Select string in user list, replace current word to string*/
void Console::slotUserListActivated(int id, const QString &string)
{
	if (id != USERLIST_ID_CONSOLE) {
		return;
	}

	int line; int index;
	QsciScintilla::getCursorPosition(&line, &index);

	/*Get line*/
	QString line_string = this->text(line);
	int char_count = line_string.length();

	int before;
	for (before = index; before > 0; before--) {
		/*Backward one by one until not a letter or number (eg: . ( ) / )*/
		QChar ch = line_string[before - 1];
		if (ch.isLetterOrNumber() || ch == '_') {
			continue;
		}
		break;
	}
	if ((string.left(1) == "(" || string.left(1) == "[")
		&& (before > 0 && line_string[before - 1] == '.')) {
		before--;
	}

	int after;
	for (after = index; after <= char_count; after++) {
		/*Forward one by one until not a letter or number (eg: . ( ) / )*/
		QChar ch = line_string[after - 1];
		if (ch.isLetterOrNumber() || ch == '_') {
			continue;
		}
		break;
	}

	/*Remove old*/
	setSelection(line, before, line, after);
	removeSelectedText();

	/*Insert user select string*/
	QsciScintilla::getCursorPosition(&line, &index);
	insertAt(string, line, index);

	/*Set cursor on the end of user select string*/
	setCursorPosition(line, index + string.length());
	return;
}

Console::Cursor Console::getCursorAt()
{
	Cursor ret_cursor;

	if (hasSelectedText()) {
		int lineFrom; int indexFrom; int lineTo; int indexTo;
		getSelection(&lineFrom, &indexFrom, &lineTo, &indexTo);
		ret_cursor.m_start = positionFromLineIndex(lineFrom, indexFrom);
		ret_cursor.m_end = positionFromLineIndex(lineTo, indexTo);
	} else {
		int line; int index;
		QsciScintilla::getCursorPosition(&line, &index);
		ret_cursor.m_start = positionFromLineIndex(line, index);
		ret_cursor.m_end = positionFromLineIndex(line, index);
	}

	return ret_cursor;
}

/*In edit area 1, not in -1, part in 0*/
int Console::cursorInEditArea(Cursor cursor)
{
	if (! cursor.isValid()) {
		cursor = getCursorAt();
	}

	if (cursor.m_start >= m_edit_start) {
		return 1;
	}
	if (cursor.m_end <= m_edit_start) {
		return -1;
	}
	return 0;
}

int Console::cursorToEditArea()
{
	int result = cursorInEditArea();
	if (result > 0) {
		return 0;
	}
	if (result < 0) {
		/*Cursor in show area, move to the end of edit area (or end of all)*/
		int len = length();
		int line; int index;
		lineIndexFromPosition(len, &line, &index);
		setCursorPosition(line, index);
		return 0;
	}

	/*Part of cursor in edit area, adjust selection from edit area start*/
	int lineFrom; int indexFrom;
	lineIndexFromPosition(m_edit_start, &lineFrom, &indexFrom);	/*Reset start to edit area begin*/

	int lineTo; int indexTo;
	getSelection(&lineFrom, &indexFrom, &lineTo, &indexTo);		/*Get selection*/

	setSelection(lineFrom, indexFrom, lineTo, indexTo);			/*Set selection*/
	return 0;
}

/*Reset cursor to edit area*/
int Console::resetCurosr()
{
	int start_line; int start_index;
	lineIndexFromPosition(m_edit_start, &start_line, &start_index);
	setCursorPosition(start_line, start_index);
	return 0;
}

/*Set prompt on line which edit area start at*/
int Console::resetPrompt()
{
	markerDeleteAll(PROMPT_MARK_ARROW);

	int start_line; int start_index;
	lineIndexFromPosition(m_edit_start, &start_line, &start_index);
	markerAdd(start_line, PROMPT_MARK_ARROW);
	return 0;
}


/*Key input*/
void Console::keyPressEvent(QKeyEvent *event)
{
	/*Ctrl+C / Ctrl+V / Ctrl+X*/
	if (event->modifiers() == Qt::ControlModifier) {
		if (event->key() == Qt::Key_C) {
			/*Ctrl+C: Allow copy in show area and edit area*/
			QsciScintilla::keyPressEvent(event);
			return;
		}
		if (event->key() == Qt::Key_V || event->key() == Qt::Key_X) {
			/* Ctrl+V, Ctrl+X: Allow paste/cu in edit area.
			 * If cursor not in edit area, try to adjust in*/
			if (cursorInEditArea() < 0) {
				event->accept();	/*Eat off event, if cursor in show area*/
				return;
			}
			cursorToEditArea();		/*Adjust cursor, to keep away from show area*/
			QsciScintilla::keyPressEvent(event);
			return;
		}
	}

	/*Delete*/
	if (event->key() == Qt::Key_Delete) {
		/*Delete selection or current character*/
		if (cursorInEditArea() < 0) {
			event->accept();
			return;
		}
		cursorToEditArea();
		QsciScintilla::keyPressEvent(event);
		return;
	}
	/*Backspace*/
	if (event->key() == Qt::Key_Backspace) {
		Cursor cursor = getCursorAt();
		if (cursor.isSelected()) {
			/*Delete selection*/
			if (cursorInEditArea() < 0) {
				event->accept();
				return;
			}
			cursorToEditArea();
			QsciScintilla::keyPressEvent(event);
			return;
		} else {
			/*Delete previous character*/
			cursor.m_start--;
			cursor.m_end--;
			if (cursorInEditArea(cursor) < 0) {
				event->accept();
				return;
			}
			QsciScintilla::keyPressEvent(event);
			return;
		}
	}

	/*Tab/Left/Right keep normal*/
	if (event->key() == Qt::Key_Left
		|| event->key() == Qt::Key_Right
		|| event->key() == Qt::Key_Tab) {
		QsciScintilla::keyPressEvent(event);
		return;
	}

	/*Up/Down for history some times*/
	if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down) {
		if (isListActive()) {
			/*If user list popping, keep up/down as normal*/
			QsciScintilla::keyPressEvent(event);
			return;
		}

		int start_line; int start_index;
		lineIndexFromPosition(m_edit_start, &start_line, &start_index);

		int last_line = lines() - 1;
		if (last_line == start_line) {
			/*History alive on only one edit line*/
			historyGet(event->key());
			event->accept();
			return;
		}

		if (event->key() == Qt::Key_Up) {
			int now_line; int now_index;
			QsciScintilla::getCursorPosition(&now_line, &now_index);
			if (now_line == start_line) {
				/*Ignore up on top edit line*/
				event->accept();
				return;
			}
		}

		/*Keep up/down as normal*/
		QsciScintilla::keyPressEvent(event);
		return;
	}

	/*Any valid input, reset history position*/
	m_history_index = -1;

	/*Enter*/
	if (event->key() == Qt::Key_Return) {
		if (isListActive()) {
			/*User list select*/
			QsciScintilla::keyPressEvent(event);
			return;
		}

		int result = submitAuto();
		if (result == 1) {
			/*Submit*/
			return;
		}
		/*Input enter*/
		QsciScintilla::keyPressEvent(event);
		return;
	}

	/*Printable character*/
	QString txt = event->text();
	if (txt.length() == 1) {
		QChar input = txt.at(0);
		if (input.isPrint()) {
			cursorToEditArea();
			QsciScintilla::keyPressEvent(event);
			/*Auto complete after input*/
			autoComplete();
			return;
		}
	}

	/*Eat off event for others key input*/
	event->accept();
	return;
}

/*Pop menu*/
void Console::contextMenuEvent(QContextMenuEvent *event)
{
	QPoint mouse_at = QCursor::pos();		/*Mouse point location*/

	/*Set menu item copy,cut,delete with selection status*/
	Cursor cursor = getCursorAt();
	if (cursor.isSelected()) {
		/*Some selection*/
		m_act_copy->setEnabled(true);
		if (cursorInEditArea() < 0) {
			/*Can not cut/delete in show area*/
			m_act_cut->setEnabled(false);
			m_act_delete->setEnabled(false);
		} else {
			m_act_cut->setEnabled(true);
			m_act_delete->setEnabled(true);
		}
	} else {
		/*No any selection*/
		m_act_copy->setEnabled(false);
		m_act_cut->setEnabled(false);
		m_act_delete->setEnabled(false);
	}

	/*Set menu item paste with clipboard*/
	QClipboard *clipboard = QApplication::clipboard();
	if (clipboard->text().length() == 0) {
		/*No content to paste*/
		m_act_paste->setEnabled(false);
	} else {
		if (cursorInEditArea() < 0) {
			/*Can not paste in show area*/
			m_act_paste->setEnabled(false);
		} else {
			m_act_paste->setEnabled(true);
		}
	}

	/*Pop the menu*/
	m_pop_menu->exec(mouse_at);
	/*Eat off event*/
	event->accept();
	return;
}

/*Disable drop*/
void Console::dropEvent(QDropEvent *event)
{
	event->accept();
	return;
}

/*Append string to show area*/
int Console::appendToShowArea(QColor color, QString &input)
{
	int line; int index;
	QsciScintilla::getCursorPosition(&line, &index);
	int cursor_pos = positionFromLineIndex(line, index);
	if (cursor_pos <= m_edit_start) {
		cursor_pos += input.length();
	}

	int startline; int startindex;
	lineIndexFromPosition(m_edit_start, &startline, &startindex);

	m_text_changging = true;
	insertAt(input, startline, startindex);
	m_edit_start += input.length();
	m_text_changging = false;

	int newline; int newindex;
	lineIndexFromPosition(m_edit_start, &newline, &newindex);

	/*Adjust cursor*/
	lineIndexFromPosition(cursor_pos, &line, &index);
	QsciScintilla::setCursorPosition(line, index);
	ensureLineVisible(line);

	/*After append, mark under-line for python stack*/
	QRegExp m_match = *m_reg_pyfl;
	int iloop;
	for (iloop = startline; iloop < newline; iloop++) {
		QString linetext = text(iloop);			/*Foreach every new line*/
		int pos = 0;
		while ((pos = m_match.indexIn(linetext, pos)) >= 0) {
			int length = m_match.matchedLength();
			/*Under line python file line*/
			fillIndicatorRange(iloop, pos, iloop, pos + length, m_indicator_pyfl);
		    pos += length;
		}
	}

	return 0;
}

/*From edit windows copy and paste to console*/
int Console::pasteToEditArea(QString &string)
{
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(string);

	slotActPasteTrigger();
	return 0;
}

int Console::submitAuto()
{
	if (! m_config_auto_submit) {
		/*No submit*/
		return 0;
	}
	if (hasSelectedText()) {
		/*Some select*/
		return 0;
	}

	int length = this->length();
	UTER_ASSERT(length >= m_edit_start);
	if (m_edit_start == length) {
		/*Empty*/
		return 0;
	}

	int lineStart; int indexStart;
	lineIndexFromPosition(m_edit_start, &lineStart, &indexStart);

	int lineEnd; int indexEnd;
	lineIndexFromPosition(length, &lineEnd, &indexEnd);

	int lineNow; int indexNow;
	QsciScintilla::getCursorPosition(&lineNow, &indexNow);

	/*Break if any valid line after cursor*/
	int iloop;
	for (iloop = lineNow + 1; iloop <= lineEnd; iloop++) {
		QString line = this->text(iloop);
		int count = line.length();
		int jloop;
		for (jloop = 0; jloop < count; jloop++) {
			QString at = line.mid(jloop, 1);
			if (at != " " && at != "\t" && at != "\n") {
				break;
			}
		}
		if (jloop != count) {
			/*Some valid character in line*/
			return 0;
		}
	}

	/*Find the last valid line, and check submit*/
	for (iloop = lineNow; iloop >= lineStart; iloop--) {
		QString line = this->text(iloop);
		if (iloop == lineStart) {
			line = line.mid(indexStart);
		}
		if (UTER_DUMPING()) {
			char *linestr = strdup(line.toStdString().c_str());
			free(linestr);
		}

		int count = line.length();
		int jloop;
		for (jloop = 0; jloop < count; jloop++) {
			QString at = line.mid(jloop, 1);
			if (at != " " && at != "\t" && at != "\n") {
				break;
			}
		}
		if (jloop == count) {
			/*Ignore invalid line*/
			continue;
		}

		/*First valid line*/
		if (line.left(1) != " " && line.left(1) != "\t") {
			return submitManual();
		} else if (iloop == lineEnd) {
			/*No next line*/
			return 0;
		} else {
			/*Check next line*/
			QString next_line = this->text(iloop + 1);
			if (UTER_DUMPING()) {
				char *nextlinestr = strdup(next_line.toStdString().c_str());
				free(nextlinestr);
			}
			if (next_line == "") {
				return submitManual();
			}
		}
		return 0;
	}

	return 0;
}

int Console::submitManual()
{
	if (hasSelectedText()) {
		return 0;
	}

	/*Get cursor at, for recovery*/
	int line; int index;
	QsciScintilla::getCursorPosition(&line, &index);

	/*Select submit text*/
	int length = this->length();
	UTER_ASSERT(length >= m_edit_start);
	if (m_edit_start == length) {
		return 0;
	}

	int lineFrom; int indexFrom;
	lineIndexFromPosition(m_edit_start, &lineFrom, &indexFrom);

	int lineTo; int indexTo;
	lineIndexFromPosition(length, &lineTo, &indexTo);

	setSelection(lineFrom, indexFrom, lineTo, indexTo);
	QString submitit = selectedText();
	if (UTER_DUMPING()) {
		char *submitstr = strdup(submitit.toStdString().c_str());
		free(submitstr);
	}
	if (submitit.right(1) != "\n") {
		submitit += "\n";
	}

	/*Check syntax here*/
	bool bresult = pythonSyntaxCheck(submitit);
	if (! bresult) {
		/*Syntax error*/
		QsciScintilla::setCursorPosition(line, index);
		return 0;
	}

	/*Submit to python*/
	int result = g_tester->pythonRun(TesterJobPython::PYTHON_COMMAND, submitit);
	if (result <= 0) {
		/*Submit failed*/
		QsciScintilla::setCursorPosition(line, index);
		return 0;
	}

	/*Submit success*/
	removeSelectedText();
	/*Adjuest cursor*/
	length = this->length();
	lineIndexFromPosition(length, &line, &index);
	QsciScintilla::setCursorPosition(line, index);

	historyAdd(submitit);
	return 1;
}

int Console::flushLineNo()
{
	clearMarginText(-1);

	int length = this->length();
	UTER_ASSERT(length >= m_edit_start);
	if (m_edit_start == length) {
		return 0;
	}

	int lineFrom; int indexFrom;
	lineIndexFromPosition(m_edit_start, &lineFrom, &indexFrom);

	int lineTo; int indexTo;
	lineIndexFromPosition(length, &lineTo, &indexTo);

	QsciStyle lineno_style;
	int iloop;
	for (iloop = lineFrom; iloop <= lineTo; iloop++) {
		/*TODO:*/
		//setMarginText(iloop,
		//			  QString::asprintf("%d", (iloop - lineFrom) + 1/*start from 1*/),
		//			  lineno_style);
	}
	return 0;
}

int Console::setIndicator()
{
	clearIndicator();

	int length = this->length();
	UTER_ASSERT(length >= m_edit_start);
	if (m_edit_start == length) {
		return 0;
	}

	int lineFrom; int indexFrom;
	lineIndexFromPosition(m_edit_start, &lineFrom, &indexFrom);

	int lineTo; int indexTo;
	lineIndexFromPosition(length, &lineTo, &indexTo);

	int iloop;
	for (iloop = lineFrom; iloop <= lineTo; iloop++) {
		QString line_string = text(iloop);
		int line_count = line_string.length();
		int jloop;
		for (jloop = 0; jloop < line_count; jloop++) {
			QChar qchar = line_string.at(jloop);
			if (qchar == QChar::Tabulation) {
				fillIndicatorRange(iloop, jloop, iloop, jloop + 1, m_indicator_tab);
			} else if (qchar == QChar::Space) {
				fillIndicatorRange(iloop, jloop, iloop, jloop + 1, m_indicator_space);
			} else {
				break;
			}
		}
	}

	return 0;
}

int Console::clearIndicator()
{
	int length = this->length();
	UTER_ASSERT(length >= m_edit_start);
	if (m_edit_start == length) {
		return 0;
	}

	int lineFrom; int indexFrom;
	lineIndexFromPosition(m_edit_start, &lineFrom, &indexFrom);

	int lineTo; int indexTo;
	lineIndexFromPosition(length, &lineTo, &indexTo);

	clearIndicatorRange(0, 0, lineTo, indexTo + 1, m_indicator_tab);
	clearIndicatorRange(0, 0, lineTo, indexTo + 1, m_indicator_space);
	return 0;
}

int Console::historyAdd(QString &submit)
{
	/*split as line*/
	QStringList stmt_list = submit.split("\n", QString::SkipEmptyParts);
	QStringList::iterator iter;
	for (iter = stmt_list.begin(); iter != stmt_list.end(); iter++) {
		QString stmt = *iter;
		/*The last input line is on the history list first, so push on fornt*/
		m_history.push_front(stmt);
	}

	/*Clear if too much*/
	while (m_history.length() > 100) {
		m_history.removeLast();
	}

	m_history_index = -1;
	return 0;
}

int Console::historyGet(int key)
{
	if (m_history.empty()) {
		return 0;
	}

	/*Get history line*/
	int next_history_index;
	if (key == Qt::Key_Up) {
		next_history_index = m_history_index + 1;
	} else if (key == Qt::Key_Down) {
		next_history_index = m_history_index - 1;
	} else {
		UTER_ASSERT(0);
	}
	/*Check range*/
	if (next_history_index < 0 || next_history_index >= m_history.length()) {
		return 0;
	}
	m_history_index = next_history_index;
	QString history_line = m_history[m_history_index];

	/*Remove old*/
	int line; int index;
	QsciScintilla::getCursorPosition(&line, &index);
	int len = lineLength(line);
	setSelection(line, 0, line, len);
	removeSelectedText();

	/*Insert history line*/
	QsciScintilla::getCursorPosition(&line, &index);
	insertAt(history_line, line, index);

	/*Set cursor on the end of history string*/
	setCursorPosition(line, index + history_line.length());
	return 0;
}

/*autoComplete show list, and slotUserListActivated check item*/
int Console::autoComplete()
{
	int line; int index;
	QsciScintilla::getCursorPosition(&line, &index);

	/*Get line*/
	QString line_string = this->text(line);
	if (UTER_DUMPING()) {
		char *linestr = strdup(line_string.toStdString().c_str());
		free(linestr);
	}

	if (line_string.left(1) == "\t" || line_string.left(1) == " ") {
		/*Ignore inside of function.*/
		return 0;
	}

	/*Get input start from*/
	int iloop;
	for (iloop = index - 1; iloop >= 0; iloop--) {
		QChar ch = line_string.at(iloop);
		if (! (ch.isLetterOrNumber() || ch == '_' || ch == '.')) {
			break;
		}
	}
	iloop++;
	if (iloop > index) {
		return 0;
	}

	/*Get input*/
	QString input = line_string.mid(iloop, (index - iloop));
	if (UTER_DUMPING()) {
		char *inputstr = strdup(input.toStdString().c_str());
		free(inputstr);
	}

	if (input.indexOf(".") < 0) {
		/*No dot in it*/
		return 0;
	}

	/*Get user list from python*/
	QStringList list = g_tester->pythonAutoCompletion(input);
	if (list.length() != 0) {
		showUserList(USERLIST_ID_CONSOLE, list);
	}
	return 0;
}

ConsoleDock::ConsoleDock()
:WidgetWithDock(SEQ_CONSOLE, Qt::BottomDockWidgetArea, "Console")
{
	QMetaObject::Connection conn;

	/* Out layout
	 * --------------------------
	 *      tool
	 * --------------------------
	 *      console
	 * --------------------------*/
	QVBoxLayout *console_layout = new QVBoxLayout(this);
	setLayout(console_layout);

	/* Tool layout
	 * -------------------------------------
	 * |  xxxx | xxxx | xxxx | xxxx |
	 * -------------------------------------*/
	QHBoxLayout *tools_layout = new QHBoxLayout(this);
	tools_layout->setAlignment(Qt::AlignLeft);
	console_layout->addLayout(tools_layout);

	/*View tab & space*/
	QCheckBox *view_tabspc_chk = new QCheckBox("View tab/space", this);
	tools_layout->addWidget(view_tabspc_chk);
	view_tabspc_chk->setChecked(false);

	/*Auto submit*/
	QCheckBox *auto_submit_chk = new QCheckBox("Auto submit", this);
	tools_layout->addWidget(auto_submit_chk);
	auto_submit_chk->setChecked(true);

	/*Submit*/
	QPushButton *manual_submit_btn = new QPushButton("Submit", this);
	tools_layout->addWidget(manual_submit_btn);

	/*Log level*/
	QLabel *label = new QLabel("Log level", this);
	tools_layout->addWidget(label);
	QComboBox *m_loglvl_cmb = new QComboBox(this);
	tools_layout->setSpacing(0);
	tools_layout->addWidget(m_loglvl_cmb);
	m_loglvl_cmb->addItem("Info");			/*0*/
	m_loglvl_cmb->addItem("Log");			/*1*/
	m_loglvl_cmb->addItem("Debug");			/*2*/
	m_loglvl_cmb->addItem("Dump");			/*3*/
	m_loglvl_cmb->setMaximumWidth(75);

	/* Console layout */
	m_console = new Console(this);
	console_layout->setSpacing(0);
	console_layout->addWidget(m_console);

	/* Hello message */
	QString hello = "Uter 1.1.0 (default, Oct 14 2015, 16:09:02)\n"
					"Type \"help\", \"copyright\", \"credits\" or \"license\" "
			        "for more information.\n";
	m_console->appendToShowArea(Qt::black, hello);

	/*Some output receiver*/
	conn = connect(&g_ui_main, SIGNAL(signalUterLog(int,QString)), this, SLOT(slotUterLog(int,QString)));
	UTER_ASSERT(conn != NULL);
	conn = connect(&g_ui_main, SIGNAL(signalPythonOutput(int,QString)), this, SLOT(slotPythonOutput(int,QString)));
	UTER_ASSERT(conn != NULL);
	conn = connect(&g_ui_main, SIGNAL(signalTargetOutput(int,QString)), this, SLOT(slotTargetOutput(int,QString)));
	UTER_ASSERT(conn != NULL);

	conn = connect(view_tabspc_chk, SIGNAL(stateChanged(int)), this, SLOT(slotViewTabSpcChanged(int)));
	UTER_ASSERT(conn != NULL);
	conn = connect(auto_submit_chk, SIGNAL(stateChanged(int)), this, SLOT(slotAutoSubmitChanged(int)));
	UTER_ASSERT(conn != NULL);
	conn = connect(manual_submit_btn, SIGNAL(pressed()), this, SLOT(slotSubmitPressed()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_loglvl_cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSetLogLevel(int)));
	UTER_ASSERT(conn != NULL);

	/*Click file name on console(python exception stack)*/
	conn = connect(m_console, SIGNAL(indicatorClicked(int, int, Qt::KeyboardModifiers)),
			       this, SLOT(slotIndicatorClicked(int, int, Qt::KeyboardModifiers)));
	UTER_ASSERT(conn != NULL);

	conn = connect(m_dock, SIGNAL(signalActive()), this, SLOT(slotDockActive()));
	UTER_ASSERT(conn != NULL);

	g_mainwin->dockSetup(this);
	return;
}

void ConsoleDock::slotUterLog(int level, QString log)
{
	switch(level)
	{
	case 0:				/*info / print*/
	{
		m_console->appendToShowArea(Qt::black, log);
		break;
	}
	case 1:				/*log*/
	{
		m_console->appendToShowArea(Qt::blue, log);
		break;
	}
	case 2:				/*debug*/
	case 3:				/*dump*/
	{
		m_console->appendToShowArea(Qt::blue, log);
		break;
	}
	default:
	{
		UTER_ASSERT(0);
	}
	}

	return;
}

void ConsoleDock::slotPythonOutput(int type, QString put)
{
	m_console->appendToShowArea(Qt::black, put);
	return;
}

void ConsoleDock::slotTargetOutput(int type, QString put)
{
	m_console->appendToShowArea(Qt::yellow, put);
	return;
}

void ConsoleDock::slotViewTabSpcChanged(int state)
{
	if (state == Qt::Checked) {
		m_console->m_config_view_tabspace = true;
		m_console->setIndicator();
	} else {
		m_console->m_config_view_tabspace = false;
		m_console->clearIndicator();
	}
	return;
}

void ConsoleDock::slotAutoSubmitChanged(int state)
{
	if (state == Qt::Checked) {
		m_console->m_config_auto_submit = true;
		m_console->submitAuto();
	} else {
		m_console->m_config_auto_submit = false;
	}
	return;
}

void ConsoleDock::slotSubmitPressed()
{
	m_console->submitManual();
	return;
}

void ConsoleDock::slotSetLogLevel(int index)
{
	uter_log_level = index;
	g_target->ctrlLogLevel(uter_log_level);
	return;
}

/*Click file name on console*/
void ConsoleDock::slotIndicatorClicked(int line, int index, Qt::KeyboardModifiers state)
{
	QString linetext = m_console->text(line);

	/*phase 1: Python exception stack file and line*/
	QRegExp pyfileline("File \"((\\/([0-9a-zA-Z\\_\\-\\.]+))+)\", line ([0-9]+)");
	UTER_ASSERT(pyfileline.isValid());
	int pos = 0;
	while ((pos = pyfileline.indexIn(linetext, pos)) >= 0) {
		/*Match one*/
		int length = pyfileline.matchedLength();
		if (index >= pos && index < pos + length) {
			/*String from regex*/
			QString filestr = pyfileline.cap(1);
			QString linestr = pyfileline.cap(pyfileline.captureCount());
			/*Get file path and line number*/
			QString cleanpath = QDir::cleanPath(filestr);
			QFileInfo fileinfo(cleanpath);
			if (! fileinfo.exists()) {
				break;
			}
			int line = linestr.toInt();
			/*Open file*/
			UEditor *editor = g_mainwin->editGetByPath(cleanpath);
			if (editor == NULL) {
				editor = UEditor::createEditByPath(cleanpath);
				g_mainwin->editAddToMdi(editor);
			}
			g_mainwin->editSetActive(editor);
			/*To line*/
			editor->selectLine(line - 1);
			editor->ensureLineVisible(line - 1);
			break;
		}
	    pos += length;
	}

	return;
}

void ConsoleDock::slotDockActive()
{
	/*Move focus to console when dock active*/
	m_console->setFocus();
	return;
}

int ConsoleDock::pasteToConsole(QString &string)
{
	m_console->pasteToEditArea(string);
	return 0;
}

} /*namespace ConUi*/
