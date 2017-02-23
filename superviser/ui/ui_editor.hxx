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

#ifndef SRC_UI_EDIT_COM_HXX_
#define SRC_UI_EDIT_COM_HXX_

#include <ui_public.hxx>

#include <qsciscintilla.h>
#include <qscistyle.h>

/*For all show window in ui*/
class EditorBase: public QsciScintilla
{
public:
	enum FileType {
		FILETYPE_PY = 1,
		FILETYPE_C,
		FILETYPE_CPP,
		FILETYPE_H,
		FILETYPE_ASMI386,
		FILETYPE_TXT,
		FILETYPE_HTML,
		FILETYPE_MAKE,
		FILETYPE_AUTO,
		FILETYPE_UNKNOWN,
	};

	EditorBase(FileType type = FILETYPE_UNKNOWN, QWidget *parent = 0);
	~EditorBase();
	int setType(FileType type);					/*File type*/
	int loadFile(QString path, QString onfail = "");	/*Load file to edit*/
	int selectLine(int lineno);							/*Select line*/
	FileType m_type;
	QsciLexer *m_lexer;
};

/*Edit window in mdi*/
class UEditor: public EditorBase
{
	Q_OBJECT
public:
	UEditor(QString path = "", QWidget *parent = 0);
	virtual ~UEditor();
signals:
	void signalClose(UEditor *);			/*UEditor close*/
	void signalKeyCtrlF();				/*Ctrl+F press*/
	void signalKeyInsert();				/*Insert press*/
	void signalPositionChanged();		/*Position changed*/
	void signalFocusChanged();			/*Focus changed*/
	void signalSelectChanged();			/*Select changed (slow)*/
private slots:
	void slotMarginClicked(int, int, Qt::KeyboardModifiers);	/*Convert click to doublie click*/
	void slotPositionChanged(int, int);							/*Position changed*/
	void slotModificationChanged(bool);
	void slotSelectionChanged();								/*Select changed (fast)*/
	void slotTextChanged();										/*For undo/redo*/
public:
	static FileType getFileTypeByPath(QString name);
	static UEditor *createEditByPath(QString path = "", QWidget *parent = 0);

	void initTitle();						/*Initialize tile*/
	void flushTitle();						/*Flush title [*]filename */
	int loadFile(QString path);				/*Load file*/
	int saveFile(QString path = "");		/*Save file*/
	int setEnableLineNo(bool set);			/*Enable/disable line number*/

	void keyPressEvent(QKeyEvent *event)
	{
		/*Key Ctrl+F*/
		if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_F) {
			emit signalKeyCtrlF();
			event->accept();
			return;
		}
		EditorBase::keyPressEvent(event);
		/*Key Insert, emit signal after input*/
		if (event->key() == Qt::Key_Insert) {
			emit signalKeyInsert();
		}
		return;
	}

	MdiItem *m_mdi_item;				/*MDI handl*/
	QString m_file_path;				/*File path*/
	QString m_file_name;				/*Short name*/
private:
	void focusInEvent(QFocusEvent *event)
	{
		EditorBase::focusInEvent(event);
		emit signalFocusChanged();
		return;
	}
	void focusOutEvent(QFocusEvent *event)
	{
		EditorBase::focusOutEvent(event);
		emit signalFocusChanged();
		return;
	}

	/*Convert click to doublie click*/
	int m_last_click_margin;
	int m_last_click_line;
	QTime m_last_click_time;

	bool m_selected;			/*Select slow*/
	bool m_changing;			/*Changing*/
public:
	friend class FileDlg;
public:
	/*Set/clear/find coverage*/
	virtual int fileCovSet(unsigned int line, unsigned int type) {return 0;}
	virtual int fileCovClear() {return 0;}
	virtual int fileCovPrev(int line, bool flag_no, bool flag_part) {return 0;}
	virtual int fileCovNext(int line, bool flag_no, bool flag_part) {return 0;}
	/*Set/clear arrow*/
	virtual int arrowSet(unsigned int line) {return 0;}
	virtual int arrowClear() {return 0;}
	/*Set/clear breakpoint*/
	virtual int brkpntSet(unsigned int line, bool enable) {return 0;}
	virtual int brkpntUnSet(unsigned int line) {return 0;}
};

class EditorCpp: public UEditor
{
	Q_OBJECT
public:
	enum {
		FILECOV_IGNORE = 0,
		FILECOV_YES,
		FILECOV_NO,
		FILECOV_PART,
	};
	/*Color for coverage*/
	static QColor m_cov_color_yes;
	static QColor m_cov_color_part;
	static QColor m_cov_color_no;

	EditorCpp(QString path = "", QWidget *parent = 0);

	int fileCovSet(unsigned int line, unsigned int type);
	int fileCovClear();
	int fileCovPrev(int line, bool flag_no, bool flag_part);
	int fileCovNext(int line, bool flag_no, bool flag_part);
	int arrowSet(unsigned int line);
	int arrowClear();
	int brkpntSet(unsigned int line, bool enable);
	int brkpntUnSet(unsigned int line);
};

class EditorPy: public UEditor
{
	Q_OBJECT
public:
	EditorPy(QString path = "", QWidget *parent = 0);
private slots:
	void slotActCopy2Console();
private:
	void contextMenuEvent(QContextMenuEvent *event);
	QAction *m_act_copy2console;
};

namespace EditUi {

class StatusBar: public QWidget {
	Q_OBJECT
public:
	StatusBar(QWidget *parent = 0);
	int show(UEditor *edit);
private:
	QLabel *m_label;
};

class FindListDlg: public QDialog {
	Q_OBJECT
public:
	FindListDlg(QWidget *parent = 0);
signals:
	void signalCloseEvent();
private slots:
	void slotBtnClose();
	void slotBtnGoto();
	void slotBtnClipBoard();

	void slotCursorPositionChanged(int, int);
public:
	int append(QString linestr, int line_no);
	int clear();
	QString m_path;	/*Setup befor show*/
private:
	void showEvent(QShowEvent *event);
	void closeEvent(QCloseEvent *event);

	EditorBase *m_list_edit;

	QPushButton *m_btn_close;
	QPushButton *m_btn_goto;
	QPushButton *m_btn_clipboard;

	bool m_close_quiet;
	bool m_changing;

	QMap<int, int> m_line_map;
};

class FindReplaceDlg: public QDialog {
	Q_OBJECT
public:
	FindReplaceDlg(QWidget *parent = 0);
private slots:
	void slotbtnFindPressed();
	void slotbtnListPressed();
	void slotbtnReplacePressed();
	void slotbtnReplaceAllPressed();

	void slotbtnClosePressed();
	void slotListClose();
public:
	QLineEdit *m_edit_find;
	QLineEdit *m_edit_replace;

	QRadioButton *m_radio_forward;
	QRadioButton *m_radio_backward;

	QCheckBox *m_check_case;
	QCheckBox *m_check_wholeword;
	QCheckBox *m_check_wrapsearch;

	QPushButton *m_btn_find;
	QPushButton *m_btn_list;
	QPushButton *m_btn_replace;
	QPushButton *m_btn_replaceall;

	QPushButton *m_btn_close;

	FindListDlg m_find_list;
};

class EditDlg: public QWidget {
	Q_OBJECT
public:
	EditDlg();
private slots:
	/*Toolbar & menu*/
    void slotEditUndo();
    void slotEditRedo();

    void slotEditCut();
	void slotEditCopy();
	void slotEditPaste();

    void slotEditDelete();
    void slotEditSelectAll();
    void slotEditGotoLine();

    void slotEditFindReplace();
    void slotEditViewLineNo();

	void slotEditActionFlush();
private:
	/*Toolbar & menu*/
	QAction *m_edit_act_undo;
	QAction *m_edit_act_redo;

    QAction *m_edit_act_cut;
    QAction *m_edit_act_copy;
    QAction *m_edit_act_paste;

    QAction *m_edit_act_delete;
    QAction *m_edit_act_selectall;

    QAction *m_edit_act_gotoline;
    QAction *m_edit_act_findreplace;

    QAction *m_edit_act_viewlineno;

    FindReplaceDlg m_find_replace;
    StatusBar *m_status_bar;
};

} /*namespace EditUi*/

namespace FileUi {

class FileNewDlg: public QDialog
{
	Q_OBJECT
public:
	FileNewDlg(QString dir, QWidget *parent = 0);
private slots:
	void slotBtnOk();
	void slotBtnCancel();
	void slotBtnBrowse();

	void slotBtnTest();
public:
	QString m_result;
private:
	QLineEdit *m_dir;
	QLineEdit *m_file;

	QPushButton *m_browse;
	QPushButton *m_ok;
	QPushButton *m_cancel;

	QMdiArea *m_test_area;
};

class FileDlg: public QDialog {
	Q_OBJECT
private slots:
	/*Toolbar & menu*/
	void slotFileNew();
	void slotFileOpen();
	void slotFileSave();
	void slotFileSaveas();
	void slotFileSaveall();
	void slotFileQuit();
	/*Some file select in mdi*/
	void slotEditActiveFlush();
public:
	FileDlg();
private:
	/*Toolbar & menu*/
    QAction *m_file_act_new;
    QAction *m_file_act_open;
    QAction *m_file_act_save;
    QAction *m_file_act_saveas;
    QAction *m_file_act_saveall;
    QAction *m_file_act_quit;
    /*Last opened directory*/
    QString m_last_open_dir;
};

} /*namespace FileUi*/
#endif /*SRC_UI_EDIT_COM_HXX_*/
