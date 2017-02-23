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

#ifndef SRC_UI_DEBUG_HXX_
#define SRC_UI_DEBUG_HXX_

#include <ui_public.hxx>
#include <ui_project.hxx>

namespace DebugUi {

class UiTargetStatusBar: public QWidget {
	Q_OBJECT
public:
	UiTargetStatusBar(QWidget *parent = 0);
	int targetStatus(int);
private:
	QPushButton *m_target_btn;
};

class DebugDlg: public QDialog {
	Q_OBJECT
private slots:
	/*Debug menu & toolbar*/
	void slotDebugCon();
	void slotDebugDiscon();
	void slotDebugStart();
	void slotDebugShut();
	void slotDebugStop();
	void slotDebugCont();
	void slotDebugStepIn();
	void slotDebugStepOver();
	void slotDebugStepReturn();
	void slotDebugStepAsm();

	void slotTargetChange(PrjUi::Config*);	/*Target set*/
	void slotStatChange(int);				/*Target status changed*/
public:
	DebugDlg();
	int updateOnStop();
	int updateOnShut();
	int updateOnRun();
private:
	int flushToolBar();
    int cppSrcArrowClean();
    int cppSrcArrowSet(QString file, unsigned int lineno);

    PrjUi::Config *m_targetconfig;

    /*Debug menu & toolbar*/
    QAction *m_debug_act_con;
    QAction *m_debug_act_discon;
    QAction *m_debug_act_start;
    QAction *m_debug_act_shut;
    QAction *m_debug_act_stop;
    QAction *m_debug_act_cont;
    QAction *m_debug_act_stepin;
    QAction *m_debug_act_stepover;
    QAction *m_debug_act_stepreturn;
    QAction *m_debug_act_stepasm;

    /*Status bar*/
    UiTargetStatusBar *m_status_bar;
};

} /*namespace DebugUi*/
#endif /*SRC_UI_DEBUG_HXX_*/
