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

#ifndef SRC_UI_CONSOLE_HXX_
#define SRC_UI_CONSOLE_HXX_

#include <ui_public.hxx>

#include <qsciscintilla.h>
#include <qscistyle.h>

namespace ConUi {

class Console: public QsciScintilla {
	Q_OBJECT
public:
	class Cursor {
	public:
		Cursor()
		{
			m_start = -1;
			m_end = -1;
			return;
		}
		bool isValid()
		{
			if (m_start == -1 || m_end == -1) {
				return false;
			}
			return true;
		}
		bool isSelected()
		{
			if (! isValid()) {
				return false;
			}
			if (m_start == m_end) {
				return false;
			} else {
				return true;
			}
		}
		int m_start;
		int m_end;
	};

	Console(QWidget *parent = 0);
	~Console()
	{
		delete m_reg_pyfl;
		return;
	}
private slots:
	/*Pop menu*/
	void slotActCopyTrigger();
	void slotActCutTrigger();
	void slotActDeleteTrigger();
	void slotActPasteTrigger();
	void slotTextChanged();								/*Set indicator for tab and space when text changed*/
	void slotUserListActivated(int, const QString&);	/*Select user list item*/
public:
	int appendToShowArea(QColor color, QString &input);
	int pasteToEditArea(QString &string);
	int clearIndicator();
	int setIndicator();
	int flushLineNo();
	int submitAuto();			/*Return value: -1 error, 0 not submit, 1 submit*/
	int submitManual();			/*Return value: -1 error, 0 not submit, 1 submit*/
	bool m_config_auto_submit;
	bool m_config_view_tabspace;
private:
	/*Pop menu in console ware*/
	QMenu *m_pop_menu;
	QAction *m_act_copy;
	QAction *m_act_cut;
	QAction *m_act_paste;
	QAction *m_act_delete;
    int m_indicator_tab;								/*Color tab*/
    int m_indicator_space;								/*Color space*/
	int m_indicator_pyfl;								/*Under line python file line*/
	/* ------------------------------------
	 * |   |       Show area              |
	 * |   |-------------------------------
	 * |   |<--edit start                 |
	 * |>>>|       Edit area              |
	 * ------------------------------------
	 * Prompt in margin, for keep python format.
	 * */
	Cursor getCursorAt();								/*getCursorPosition plus*/
	int cursorInEditArea(Cursor cursor = Cursor());		/*In edit area 1, not in -1, part in 0*/
	int cursorToEditArea();								/*Adjust cursor to edit area(include adjust selection)*/
	int resetCurosr();									/*Reset cursor to edit area(no selection after reset)*/
	int resetPrompt();									/*Reset prompt to edit area start*/
	/* Before using QsciScintilla::getCursorPosition(&line, &index) and length() return 0.
	 * If edit area empty, m_edit_start equal to this->length(),
	 * m_edit_start is not a valid position of text, but cursor can be stay in*/
	int m_edit_start;									/*Edit area start position*/
	bool m_text_changging;								/*Text changed by system*/
	void keyPressEvent(QKeyEvent *event);				/*Key input*/
	void contextMenuEvent(QContextMenuEvent *event);	/*Pop menu in console*/
	void dropEvent(QDropEvent *event);					/*Disable drop*/
	int historyAdd(QString &submit);					/*Add history*/
	int historyGet(int key);							/*Get history*/
	QList<QString> m_history;
	int m_history_index;
	int autoComplete();									/*Auto complete, show user list*/
	QRegExp *m_reg_pyfl;
};

class ConsoleDock: public WidgetWithDock {
	Q_OBJECT
public:
	ConsoleDock();
private slots:
	void slotUterLog(int,QString);					/*Log receiver*/
	void slotPythonOutput(int,QString);					/*Python output receiver*/
	void slotTargetOutput(int,QString);

	/*Tools*/
	void slotViewTabSpcChanged(int);
    void slotAutoSubmitChanged(int);
    void slotSubmitPressed();
    void slotSetLogLevel(int);

    void slotDockActive();										/*Move focus to console when dock active(for use easy)*/
    void slotIndicatorClicked(int, int, Qt::KeyboardModifiers);	/*Click a file path on console(in python exception stack)*/
public:
    int pasteToConsole(QString &string);
private:
    Console *m_console;								/*Console*/
};

} /*namespace ConUi*/
#endif /* SRC_UI_CONSOLE_HXX_ */
