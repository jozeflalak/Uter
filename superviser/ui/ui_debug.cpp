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

namespace DebugUi {

UiTargetStatusBar::UiTargetStatusBar(QWidget *parent)
{
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);
	setLayout(layout);

	m_target_btn = new QPushButton(this);
	m_target_btn->setEnabled(false);
	m_target_btn->setText("Target");
	layout->addWidget(m_target_btn);

	targetStatus(Target::STATUS_DOWN);
	return;
}

int UiTargetStatusBar::targetStatus(int status)
{
	switch(status)
	{
	case Target::STATUS_DOWN:
	{
		m_target_btn->setStyleSheet("background-color:red");
		m_target_btn->setToolTip("Target is down.");
		break;
	}
	case Target::STATUS_CONNECTED:
	{
		m_target_btn->setStyleSheet("background-color:yellow");
		m_target_btn->setToolTip("Target is connected.");
		break;
	}
	case Target::STATUS_RUNNING:
	{
		m_target_btn->setStyleSheet("background-color:green");
		m_target_btn->setToolTip("Target is running.");
		break;
	}
	default:
	{
		m_target_btn->setStyleSheet("background-color:blue");
		m_target_btn->setToolTip("Unknwon.");
		break;
	}
	}
	return 0;
}

DebugDlg::DebugDlg()
{
	QMetaObject::Connection conn;

	/*Status bar*/
	UiStatusBarSetup *statusbar = g_mainwin->statusBarSetup(UiStatusBarSetup::SEQ_TARGET, "TargetStatus");
	m_status_bar = new UiTargetStatusBar(this);
	statusbar->addWidget(m_status_bar);

	/*Menu & toolbar*/
	UiMenuSetup *menu = g_mainwin->menuSetup(UiMenuSetup::SEQ_DEBUG, "&Debug");
	UiToolBarSetup *toolbar = g_mainwin->toolBarSetup(UiToolBarSetup::SEQ_DEBUG, "&Debug");

	m_targetconfig = NULL;

    m_debug_act_con = new QAction("&Connect", this);
    m_debug_act_con->setStatusTip("Connect to target");
    m_debug_act_con->setIcon(QIcon(":/images/ctrl_connect.png"));
    menu->addAction(m_debug_act_con);
    toolbar->addAction(m_debug_act_con);

    m_debug_act_discon = new QAction("&Disconnect", this);
    m_debug_act_discon->setStatusTip("Disconnect from target");
    m_debug_act_discon->setIcon(QIcon(":/images/ctrl_disconnect.png"));
    menu->addAction(m_debug_act_discon);
    toolbar->addAction(m_debug_act_discon);

    m_debug_act_start = new QAction("&Start", this);
    m_debug_act_start->setStatusTip("Start target");
    m_debug_act_start->setIcon(QIcon(":/images/ctrl_start.png"));
    menu->addAction(m_debug_act_start);
    toolbar->addAction(m_debug_act_start);

    m_debug_act_shut = new QAction("&Shut", this);
    m_debug_act_shut->setStatusTip("Shutdown target");
    m_debug_act_shut->setIcon(QIcon(":/images/ctrl_close.png"));
    menu->addAction(m_debug_act_shut);
    toolbar->addAction(m_debug_act_shut);

    m_debug_act_stop = new QAction("&Stop", this);
    m_debug_act_stop->setStatusTip("Stop target");
    m_debug_act_stop->setIcon(QIcon(":/images/ctrl_stop.png"));
    menu->addAction(m_debug_act_stop);
    toolbar->addAction(m_debug_act_stop);

    m_debug_act_cont = new QAction("&Continue", this);
    m_debug_act_cont->setStatusTip("Continue target");
    m_debug_act_cont->setIcon(QIcon(":/images/ctrl_continue.png"));
    menu->addAction(m_debug_act_cont);
    toolbar->addAction(m_debug_act_cont);

    m_debug_act_stepin = new QAction("&Step Into", this);
    m_debug_act_stepin->setStatusTip("Step Into");
    m_debug_act_stepin->setIcon(QIcon(":/images/step_into.png"));
    menu->addAction(m_debug_act_stepin);
    toolbar->addAction(m_debug_act_stepin);

    m_debug_act_stepover = new QAction("&Step Over", this);
    m_debug_act_stepover->setStatusTip("Step Over");
    m_debug_act_stepover->setIcon(QIcon(":/images/step_over.png"));
    menu->addAction(m_debug_act_stepover);
    toolbar->addAction(m_debug_act_stepover);

    m_debug_act_stepreturn = new QAction("&Step Return", this);
    m_debug_act_stepreturn->setStatusTip("Step Return");
    m_debug_act_stepreturn->setIcon(QIcon(":/images/step_return.png"));
    menu->addAction(m_debug_act_stepreturn);
    toolbar->addAction(m_debug_act_stepreturn);

    m_debug_act_stepasm = new QAction("&Step Asm", this);
    m_debug_act_stepasm->setStatusTip("Step Asm");
    m_debug_act_stepasm->setIcon(QIcon(":/images/step_asm.png"));
    menu->addAction(m_debug_act_stepasm);
    toolbar->addAction(m_debug_act_stepasm);

    /*Target config set*/
    conn = connect(g_mainwin->m_prj_win, SIGNAL(signalTargetChange(PrjUi::Config*)),
    		                         this, SLOT(slotTargetChange(PrjUi::Config*)));
    UTER_ASSERT(conn != NULL);

    /*Target status changed*/
    conn = connect(g_target, SIGNAL(signalStatChange(int)), this, SLOT(slotStatChange(int)));
    UTER_ASSERT(conn != NULL);

    /*Menu & toolbar*/
    conn = connect(m_debug_act_con, SIGNAL(triggered()), this, SLOT(slotDebugCon()));
    UTER_ASSERT(conn != NULL);
    conn = connect(m_debug_act_discon, SIGNAL(triggered()), this, SLOT(slotDebugDiscon()));
    UTER_ASSERT(conn != NULL);
    conn = connect(m_debug_act_start, SIGNAL(triggered()), this, SLOT(slotDebugStart()));
    UTER_ASSERT(conn != NULL);
    conn = connect(m_debug_act_shut, SIGNAL(triggered()), this, SLOT(slotDebugShut()));
    UTER_ASSERT(conn != NULL);
    conn = connect(m_debug_act_stop, SIGNAL(triggered()), this, SLOT(slotDebugStop()));
    UTER_ASSERT(conn != NULL);
    conn = connect(m_debug_act_cont, SIGNAL(triggered()), this, SLOT(slotDebugCont()));
    UTER_ASSERT(conn != NULL);
    conn = connect(m_debug_act_stepin, SIGNAL(triggered()), this, SLOT(slotDebugStepIn()));
    UTER_ASSERT(conn != NULL);
    conn = connect(m_debug_act_stepover, SIGNAL(triggered()), this, SLOT(slotDebugStepOver()));
    UTER_ASSERT(conn != NULL);
    conn = connect(m_debug_act_stepreturn, SIGNAL(triggered()), this, SLOT(slotDebugStepReturn()));
    UTER_ASSERT(conn != NULL);
    conn = connect(m_debug_act_stepasm, SIGNAL(triggered()), this, SLOT(slotDebugStepAsm()));
    UTER_ASSERT(conn != NULL);

    flushToolBar();
	return;
}

void DebugDlg::slotDebugCon()
{
	if (m_targetconfig == NULL) {
		return;
	}

	if (m_targetconfig->m_conn.m_con_type == PrjUi::Config::Con::TYPE_LOCAL) {
		g_target->ctrlConnect();
	} else if (m_targetconfig->m_conn.m_con_type == PrjUi::Config::Con::TYPE_TCP) {
		/*TODO:*/
		UTER_ASSERT(0);
	} else {
		UTER_ASSERT(0);
	}

	return;
}

void DebugDlg::slotDebugDiscon()
{
	g_target->ctrlDisconn();
	return;
}

void DebugDlg::slotDebugStart()
{
	if (m_targetconfig == NULL) {
		return;
	}

	if (m_targetconfig->m_start.m_start_type == PrjUi::Config::Start::TYPE_NEW) {
		g_target->ctrlStartNew(m_targetconfig->m_start.m_data_new.m_path,
							   m_targetconfig->m_start.m_data_new.m_arglist,
							   m_targetconfig->m_start.m_data_new.m_envlist);
	} else if (m_targetconfig->m_start.m_start_type == PrjUi::Config::Start::TYPE_ATTACH) {
		/*TODO:*/
		UTER_ASSERT(0);
	} else {
		UTER_ASSERT(0);
	}

	return;
}

void DebugDlg::slotDebugShut()
{
	g_target->ctrlShut();
	return;
}

void DebugDlg::slotDebugStop()
{
	g_target->ctrlStop();
	return;
}

void DebugDlg::slotDebugCont()
{
	g_target->ctrlContinue();
	return;
}

void DebugDlg::slotDebugStepIn()
{
	g_target->ctrlStep(g_mainwin->get_current_pid(), StepInfo::STEP_IN);
	return;
}

void DebugDlg::slotDebugStepOver()
{
	g_target->ctrlStep(g_mainwin->get_current_pid(), StepInfo::STEP_OVER);
	return;
}

void DebugDlg::slotDebugStepReturn()
{
	g_target->ctrlStep(g_mainwin->get_current_pid(), StepInfo::STEP_RETURN);
	return;
}

void DebugDlg::slotDebugStepAsm()
{
	g_target->ctrlStep(g_mainwin->get_current_pid(), StepInfo::STEP_MAX);
	return;
}

void DebugDlg::slotTargetChange(PrjUi::Config *targetconfig)
{
	m_targetconfig = targetconfig;
	flushToolBar();
	return;
}

void DebugDlg::slotStatChange(int status)
{
	m_status_bar->targetStatus(status);
	flushToolBar();
	return;
}

int DebugDlg::cppSrcArrowClean()
{
	QList<MdiItem *> itemlist = g_mainwin->m_center_mdi->getItemList();
	QList<MdiItem *>::iterator iter;
	for (iter = itemlist.begin(); iter != itemlist.end(); iter++) {
		MdiItem *item = *iter;
		UEditor *editor = (UEditor *)item->widget();
		editor->arrowClear();
	}

	return 0;
}

int DebugDlg::cppSrcArrowSet(QString file, unsigned int lineno)
{
	UTER_DEBUG("Set debug arrow to %s:%u", file.toStdString().c_str(), lineno);

	UEditor *editor = g_mainwin->editGetByPath(file);
	if (editor == NULL) {
		editor = UEditor::createEditByPath(file);
		g_mainwin->editAddToMdi(editor);
	}
	g_mainwin->editSetActive(editor);

	editor->arrowSet(lineno);
	return 0;
}

int DebugDlg::flushToolBar()
{
	/*Con/discon/start/shut ...*/
	m_debug_act_con->setEnabled(false);
	m_debug_act_discon->setEnabled(false);
	m_debug_act_start->setEnabled(false);
	m_debug_act_shut->setEnabled(false);

	if (g_target->m_status == Target::STATUS_DOWN) {
		if (m_targetconfig != NULL) {
			/*Shutdown status with valid connection*/
			m_debug_act_con->setEnabled(true);
		}
	} else if (g_target->m_status == Target::STATUS_CONNECTED) {
		/*Connected but not start*/
	    m_debug_act_discon->setEnabled(true);
	    m_debug_act_start->setEnabled(true);
	} else {
		/*Running*/
	    m_debug_act_shut->setEnabled(true);
	}

	if (g_target->m_status == Target::STATUS_RUNNING) {
		if (g_target->threadsStatInRange(TargetThread::STATUSSET_NOTRUN)) {
			m_debug_act_stop->setEnabled(false);
			m_debug_act_cont->setEnabled(true);
			m_debug_act_stepasm->setEnabled(true);

			m_debug_act_stepin->setEnabled(false);
			m_debug_act_stepover->setEnabled(false);
			m_debug_act_stepreturn->setEnabled(false);

			unsigned int pid = g_mainwin->get_current_pid();
			unsigned long long addr;
			int result = g_target->pidRegPcGet(pid, addr);
			if (result != 0) {
				return 0;
			}
			TargetMod *mod = g_target->modGetByAddrIn(addr);
			if (mod == NULL) {
				return 0;
			}
			unsigned long long offset_addr = addr - mod->m_add_mkup;
			DwfCu *cu = mod->dwarfGetCuByAddrIn(offset_addr);
			if (cu == NULL) {
				return 0;
			}
			QString file;
			unsigned int line = 0;
			result = cu->getFileLineByAddrIn(addr, file, line);
			if (result == 0) {
			    m_debug_act_stepin->setEnabled(true);
			    m_debug_act_stepover->setEnabled(true);
			    m_debug_act_stepreturn->setEnabled(true);
			}
		} else {
			m_debug_act_stop->setEnabled(true);
			m_debug_act_cont->setEnabled(false);
			m_debug_act_stepin->setEnabled(false);
			m_debug_act_stepover->setEnabled(false);
			m_debug_act_stepreturn->setEnabled(false);
			m_debug_act_stepasm->setEnabled(false);
		}
	} else {
		m_debug_act_stop->setEnabled(false);
		m_debug_act_cont->setEnabled(false);
		m_debug_act_stepin->setEnabled(false);
		m_debug_act_stepover->setEnabled(false);
		m_debug_act_stepreturn->setEnabled(false);
		m_debug_act_stepasm->setEnabled(false);
	}

	return 0;
}

int DebugDlg::updateOnStop()
{
	flushToolBar();

	unsigned int pid = g_mainwin->get_current_pid();
	unsigned long long addr;
	int result = g_target->pidRegPcGet(pid, addr);
	if (result != 0) {
		return 0;
	}

	/*Disasm*/
	g_mainwin->m_disasm_win->disAsm(addr);

	TargetMod *mod = g_target->modGetByAddrIn(addr);
	if (mod == NULL) {
		return 0;
	}
	unsigned long long offset_addr = addr - mod->m_add_mkup;
	DwfCu *cu = mod->dwarfGetCuByAddrIn(offset_addr);
	if (cu == NULL) {
		return 0;
	}
	QString file;
	unsigned int line = 0;
	result = cu->getFileLineByAddrIn(addr, file, line);
	if (result == 0) {
		cppSrcArrowSet(file, line);
	}

	return 0;
}

int DebugDlg::updateOnShut()
{
	cppSrcArrowClean();

	flushToolBar();
	return 0;
}

int DebugDlg::updateOnRun()
{
	cppSrcArrowClean();
	g_mainwin->m_disasm_win->clearArrow();

	flushToolBar();
	return 0;
}

} /*namespace DebugUi*/
