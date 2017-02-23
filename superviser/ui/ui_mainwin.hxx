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

#ifndef SRC_UI_MAINWIN_HXX_
#define SRC_UI_MAINWIN_HXX_

#include <QtWidgets/QtWidgets>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QAction>
#include <QtWidgets/QMenu>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QLineEdit>
#include <qsciscintilla.h>
#include <qscistyle.h>

#include <ui_editor.hxx>

#include <ui_coverage.hxx>
#include <ui_mod.hxx>
#include <ui_map.hxx>
#include <ui_mem.hxx>
#include <ui_register.hxx>
#include <ui_thread.hxx>
#include <ui_variable.hxx>
#include <ui_project.hxx>
#include <ui_brkpnt.hxx>
#include <ui_console.hxx>
#include <ui_disasm.hxx>
#include <ui_analyse.hxx>
#include <ui_debug.hxx>
#include <ui_tester.hxx>
#include <ui_supply.hxx>
#include <ui_overview.hxx>

/*Menu setup handle*/
class UiMenuSetup {
public:
	/*Menu setup index*/
	enum UiMenuSeq {
		SEQ_FILE = 0,
		SEQ_PRJ,
		SEQ_EDIT,
		SEQ_DEBUG,
		SEQ_TEST,
		SEQ_COV,
		SEQ_TOOL_EXPLORE,
		SEQ_TOOL_ANALYSE,
		SEQ_WIN,
		SEQ_HELP,
		SEQ_MAX
	};
	UiMenuSetup(UiMenuSeq seq, QStringList &setto)
	{
		m_seq = seq;
		m_setto = setto;
		return;
	}
	int addAction(QAction *action)
	{
		/*UTER_ASSERT(action != NULL);*/
		m_actions.push_back(action);
		return 0;
	}
	UiMenuSeq m_seq;				/*Setup index*/
	QStringList m_setto;			/*Menu location description*/
	QList<QAction*> m_actions;		/*Actions*/
};

/*Toolbar setup handle*/
class UiToolBarSetup {
public:
	/*Toolbar setup index*/
	enum UiToolBarSeq {
		SEQ_FILE = 0,
		SEQ_EDIT,
		SEQ_DEBUG,
		SEQ_TEST,
		SEQ_COV,
		SEQ_TOOL_EXPLORE,
		SEQ_TOOL_ANALYSE,
		SEQ_HELP,
		SEQ_MAX
	};
	UiToolBarSetup(UiToolBarSeq seq, QString name)
	{
		m_seq = seq;
		m_name = name;
		return;
	}
	int addAction(QAction *action)
	{
		UTER_ASSERT(action != NULL);
		m_actions.push_back(action);
		return 0;
	}
	UiToolBarSeq m_seq;				/*Setup index*/
	QString m_name;					/*Toolbar name*/
	QList<QAction*> m_actions;		/*Actions*/
};

/*Statusbar setup handle*/
class UiStatusBarSetup {
public:
	enum UiStatusBarSeq {
		SEQ_EDIT = 0,
		SEQ_TARGET,
		SEQ_TESTER,
		SEQ_MAX
	};
	UiStatusBarSetup(UiStatusBarSeq seq, QString name)
	{
		m_seq = seq;
		m_name = name;
		m_widget = NULL;
		return;
	}
	int addWidget(QWidget *widget)
	{
		UTER_ASSERT(widget != NULL);
		UTER_ASSERT(m_widget == NULL);
		m_widget = widget;
		return 0;
	}
	UiStatusBarSeq m_seq;				/*Setup index*/
	QString m_name;						/*Statusbar name*/
	QWidget *m_widget;					/*Widget*/
};

/*Main windows frame*/
class MainWindow: public QMainWindow
{
	Q_OBJECT
signals:
	void signalEditFileOpen(UEditor*);							/*Edit file open*/
	void signalEditMarginDClicked(UEditor*, unsigned int);		/*Margin of edit file double click*/
private slots:
	void slotMsgToStatBar(unsigned int, QString);				/*Message to status bar with time*/
	void slotStopAction(unsigned int, unsigned int stopby);		/*Stop action*/
	void slotRunAction();										/*Run action*/
	void slotShutAction();										/*Shutdown action*/
	void slotPythonRun();										/*Python begin*/
	void slotPythonFinish();									/*Python finish*/
	void slotEditClose(MdiItem*);								/*Edit close*/
	/*For toolbar and dock ware in top menu*/
	void slotFlushToolbarMenu(bool nouse = true);
	void slotClickToolbarMenu();
	void slotFlushDockMenu(bool nouse = true);
	void slotClickDockMenu();
public:
    MainWindow();
    ~MainWindow();
	void moduleInit();											/*initialize sub ware after g_mainwin ready*/

	int dockSetup(WidgetWithDock *dock);
	UiStatusBarSetup *statusBarSetup(UiStatusBarSetup::UiStatusBarSeq seq, QString name);
	UiToolBarSetup *toolBarSetup(UiToolBarSetup::UiToolBarSeq seq, QString name);
    UiMenuSetup *menuSetup(UiMenuSetup::UiMenuSeq seq, QStringList setto);
    UiMenuSetup *menuSetup(UiMenuSetup::UiMenuSeq seq, QString setto)
    {
    	return menuSetup(seq, QStringList(setto));
    }
private:
	int dockApply();
	QList<WidgetWithDock*> m_dock_item_list;		/*For register and apply*/
	QMap<Qt::DockWidgetArea, WidgetWithDock*> m_dock_first_map;

	int statusBarApply();
	QList<UiStatusBarSetup*> m_statusbar_item_list;	/*For register and apply*/

	class UiToolbarHandl {
	public:
		UiToolbarHandl(QString name, QToolBar *toolbar)
		{
			m_name = name;
			m_toolbar = toolbar;
			return;
		}
		QString m_name;
		QToolBar *m_toolbar;
	};
	QToolBar *toolBarGetHandl(QString name);
	int toolBarApply();
	QList<UiToolBarSetup*> m_toolbar_item_list;		/*For register*/
	QList<UiToolbarHandl*> m_toolbar_handl_list;	/*For apply*/

	class UiMenuHandl {
	public:
		UiMenuHandl(QStringList strlist, QMenu *menu)
		{
			m_strlist = strlist;
			m_menu = menu;
			return;
		}
		QStringList m_strlist;
		QMenu *m_menu;
	};
    QMenu *menuGetHandl(QStringList &setto);
	int menuApply();
	QList<UiMenuSetup*> m_menu_item_list;		/*For register*/
	QList<UiMenuHandl*> m_menu_handl_list;		/*For apply*/
	QMenu *m_window_toolbar_menu;
	QMenu *m_window_dock_menu;
public:
	/*About edit*/
	int editAddToMdi(UEditor *edit);
	UEditor *editGetByPath(QString file);
	UEditor *editGetActive();
	int editClose(UEditor *edit);
	int editSetActive(UEditor *edit);
	int editSetLineNoEnable(bool set);
	MdiFrame *m_center_mdi;
	bool m_enable_lineno;

	friend class DebugDlg;
	friend class UEditor;
	friend class EditDlg;
	friend class FileDlg;

	int set_current_pid(unsigned int pid)
	{
		m_current_pid = pid;
		return 0;
	}
	unsigned int get_current_pid()
	{
		return m_current_pid;
	}
	unsigned int m_current_pid;

    /*UiWidgetWithDock*/
	PrjUi::PrjDock *m_prj_win;
	ConUi::ConsoleDock *m_con_win;
    VarUi::VarDock *m_var_win;
    VarUi::WatchDock *m_watch_win;
    MemUi::MemDock *m_mem_win;
    BrkPntUi::BrkPntDock *m_brkpnt_win;
    RegUi::RegDock *m_reg_win;
    MapUi::MapDock *m_map_win;
    ModUi::ModDock *m_mod_win;
    ThrdUi::ThrdDock *m_thrd_win;
    DisAsmUi::DisAsmDock *m_disasm_win;
    CovUi::CovDock *m_cov_win;
    OverViewUi::OverViewDock *m_overview_win;
    /*QDialog*/
    AnalyseUi::AnalyseDlg *m_analyse;
    FileUi::FileDlg *m_file;
    EditUi::EditDlg *m_edit;
    DebugUi::DebugDlg *m_debug;
    TesterUi::TesterDlg *m_tester;
    SupplyUi::SupplyDlg *m_supply;
};

extern MainWindow *g_mainwin;

#endif /* SRC_UI_MAINWIN_HXX_ */
