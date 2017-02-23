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
#include <main.hxx>
#include <ui_mainwin.hxx>

MainWindow::MainWindow()
{
	QMetaObject::Connection conn;

	setWindowTitle("Uter IDE");

	m_center_mdi = new MdiFrame(this);
	setCentralWidget(m_center_mdi);
	m_enable_lineno = false;

	/*Edit close*/
	conn = connect(m_center_mdi, SIGNAL(closeItem(MdiItem*)), this, SLOT(slotEditClose(MdiItem*)));
	UTER_ASSERT(conn != NULL);

	conn = connect(&g_ui_main, SIGNAL(signalMsgToStatBar(unsigned int,QString)),
			       this, SLOT(slotMsgToStatBar(unsigned int,QString)));
	UTER_ASSERT(conn != NULL);

	conn = connect(g_target, SIGNAL(signalStopAction(unsigned int, unsigned int)),
			       this, SLOT(slotStopAction(unsigned int, unsigned int)));
	UTER_ASSERT(conn != NULL);
	conn = connect(g_target, SIGNAL(signalRunAction()), this, SLOT(slotRunAction()));
	UTER_ASSERT(conn != NULL);
	conn = connect(g_target, SIGNAL(signalShutAction()), this, SLOT(slotShutAction()));
	UTER_ASSERT(conn != NULL);

	conn = connect(g_tester, SIGNAL(signalPythonRun()), this, SLOT(slotPythonRun()));
	UTER_ASSERT(conn != NULL);
	conn = connect(g_tester, SIGNAL(signalPythonFinish()), this, SLOT(slotPythonFinish()));
	UTER_ASSERT(conn != NULL);

    /*Show statusbar*/
    statusBar()->showMessage("", 2000);

    setMinimumSize(500, 500);
    setWindowState(Qt::WindowMaximized);

	m_window_toolbar_menu = NULL;
	m_window_dock_menu = NULL;

	m_prj_win = NULL;
	m_con_win = NULL;
	m_var_win = NULL;
	m_watch_win = NULL;
	m_mem_win = NULL;
	m_reg_win = NULL;
	m_map_win = NULL;
	m_mod_win = NULL;
	m_thrd_win = NULL;
	m_disasm_win = NULL;
	m_brkpnt_win = NULL;
	m_cov_win = NULL;

	m_analyse = NULL;
	m_file = NULL;
	m_edit = NULL;
	m_debug = NULL;
	m_tester = NULL;
	m_supply = NULL;

    /*TODO:*/
    m_current_pid = 0;
    return;
}

MainWindow::~MainWindow()
{
	QList<UiMenuSetup*>::iterator menu_item_iter;
	for (menu_item_iter = m_menu_item_list.begin();
		 menu_item_iter != m_menu_item_list.end(); /*none*/) {
		UiMenuSetup *item = *menu_item_iter;
		delete item;
		m_menu_item_list.erase(menu_item_iter++);
	}

	QList<UiMenuHandl*>::iterator menu_handl_iter;
	for (menu_handl_iter = m_menu_handl_list.begin();
		 menu_handl_iter != m_menu_handl_list.end(); /*none*/) {
		UiMenuHandl *handl = *menu_handl_iter;
		delete handl;
		m_menu_handl_list.erase(menu_handl_iter++);
	}

	QList<UiToolBarSetup*>::iterator toolbar_item_iter;
	for (toolbar_item_iter = m_toolbar_item_list.begin();
			toolbar_item_iter != m_toolbar_item_list.end(); /*none*/) {
		UiToolBarSetup *item = *toolbar_item_iter;
		delete item;
		m_toolbar_item_list.erase(toolbar_item_iter++);
	}

	QList<UiToolbarHandl*>::iterator toolbar_handl_iter;
	for (toolbar_handl_iter = m_toolbar_handl_list.begin();
			toolbar_handl_iter != m_toolbar_handl_list.end(); /*none*/) {
		UiToolbarHandl *handl = *toolbar_handl_iter;
		delete handl;
		m_toolbar_handl_list.erase(toolbar_handl_iter++);
	}

	return;
}

void MainWindow::moduleInit()
{
	m_prj_win = new PrjUi::PrjDock();
	m_con_win = new ConUi::ConsoleDock();
    m_var_win = new VarUi::VarDock();
    m_watch_win = new VarUi::WatchDock();
	m_mem_win = new MemUi::MemDock();
	m_reg_win = new RegUi::RegDock();
	m_map_win = new MapUi::MapDock();
	m_mod_win = new ModUi::ModDock();
	m_thrd_win = new ThrdUi::ThrdDock();
	m_disasm_win = new DisAsmUi::DisAsmDock();
	m_brkpnt_win = new BrkPntUi::BrkPntDock();
    m_cov_win = new CovUi::CovDock();
    m_overview_win = new OverViewUi::OverViewDock();

	m_analyse = new AnalyseUi::AnalyseDlg();
	m_file = new FileUi::FileDlg();
	m_edit = new EditUi::EditDlg();
	m_debug = new DebugUi::DebugDlg();
	m_tester = new TesterUi::TesterDlg();
	m_supply = new SupplyUi::SupplyDlg();

	statusBarApply();
	dockApply();
	toolBarApply();
	menuApply();		/*After dock and toolbar, because dock and toolbar item in menu*/

	/*Show console dock*/
	m_con_win->m_dock->raise();
	/*Hiden by default*/
	m_overview_win->m_dock->setVisible(false);

    /*Try to load project*/
	m_prj_win->m_project_path = g_ui_main.m_config_project_current_project;
	m_prj_win->prjClear();
	m_prj_win->prjLoadXml();
	return;
}

void MainWindow::slotStopAction(unsigned int pid, unsigned int stopby)
{
	set_current_pid(pid);

	m_map_win->updateOnStop();
	m_mod_win->updateOnStop();
	m_mem_win->updateOnStop();
	m_reg_win->updateOnStop();
	m_thrd_win->updateOnStop();
	m_debug->updateOnStop();
	return;
}

void MainWindow::slotRunAction()
{
	m_mem_win->updateOnRun();
	m_reg_win->updateOnRun();

	m_debug->updateOnRun();
	return;
}

void MainWindow::slotShutAction()
{
	m_map_win->updateOnShut();
	m_mod_win->updateOnShut();
	m_mem_win->updateOnShut();
	m_reg_win->updateOnShut();
	m_thrd_win->updateOnShut();
	m_debug->updateOnShut();

	return;
}

void MainWindow::slotPythonRun()
{
	return;
}

void MainWindow::slotPythonFinish()
{
	if (g_target->m_status != Target::STATUS_RUNNING) {
		/*Not running*/
		return;
	}
	if (! g_target->threadsStatInRange(TargetThread::STATUSSET_NOTRUN)) {
		/*Not stoped*/
		return;
	}

	slotShutAction();
	return;
}

void MainWindow::slotMsgToStatBar(unsigned int timeout, QString info)
{
    statusBar()->showMessage("Console::" + info, timeout);
	return;
}

int MainWindow::dockSetup(WidgetWithDock *dock)
{
	/*Check exit*/
	QList<WidgetWithDock*>::iterator iter;
	for (iter = m_dock_item_list.begin(); iter != m_dock_item_list.end(); iter++) {
		WidgetWithDock *item = *iter;
		if (item->m_seq == dock->m_seq) {
			break;
		}
	}
	if (iter != m_dock_item_list.end()) {
		UTER_ASSERT(0);
		return -1;
	}

	/*Setup dock*/
	int count = m_dock_item_list.count();
	int iloop;
	for (iloop = 0; iloop < count; iloop++) {
		WidgetWithDock *item = m_dock_item_list[iloop];
		if (dock->m_seq < item->m_seq) {
			m_dock_item_list.insert(iloop, dock);
			break;
		}
	}
	if (iloop == count) {
		m_dock_item_list.insert(count, dock);
	}
	return 0;
}

int MainWindow::dockApply()
{
	/*Keep tab style when multi-dock*/
	setDockOptions(dockOptions() | ForceTabbedDocks);
	/*Set dock tab at south, same as eclipse*/
	setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::South);

	QList<WidgetWithDock*>::iterator iter;
	for (iter = m_dock_item_list.begin(); iter != m_dock_item_list.end(); iter++) {
		WidgetWithDock *item = *iter;

		addDockWidget(item->m_area, item->m_dock);

		QMap<Qt::DockWidgetArea, WidgetWithDock*>::iterator first_iter;
		first_iter = m_dock_first_map.find(item->m_area);
		if (first_iter == m_dock_first_map.end()) {
			m_dock_first_map.insert(item->m_area, item);
		} else {
			/*Merge to tab, if not first dock on such side*/
			WidgetWithDock *first = first_iter.value();
			tabifyDockWidget(first->m_dock, item->m_dock);
		}
	}

	return 0;
}

UiStatusBarSetup *MainWindow::statusBarSetup(UiStatusBarSetup::UiStatusBarSeq seq, QString name)
{
	/*Check exist*/
	QList<UiStatusBarSetup*>::iterator iter;
	for (iter = m_statusbar_item_list.begin();
		 iter != m_statusbar_item_list.end(); iter++) {
		UiStatusBarSetup *item = *iter;
		if (item->m_seq == seq) {
			break;
		}
	}
	if (iter != m_statusbar_item_list.end()) {
		UTER_ASSERT(0);
		return NULL;
	}

	/*Setup statusbar*/
	int count = m_statusbar_item_list.length();
	int iloop;
	for (iloop = 0; iloop < count; iloop++) {
		UiStatusBarSetup *item = m_statusbar_item_list[iloop];
		if (seq < item->m_seq) {
			UiStatusBarSetup *statusbar = new UiStatusBarSetup(seq, name);
			m_statusbar_item_list.insert(iloop, statusbar);
			return statusbar;
		}
	}
	if (iloop == count) {
		UiStatusBarSetup *statusbar = new UiStatusBarSetup(seq, name);
		m_statusbar_item_list.insert(count, statusbar);
		return statusbar;
	}

	UTER_ASSERT(0);
	return NULL;
}

int MainWindow::statusBarApply()
{
	QStatusBar *statusbar = statusBar();

	QList<UiStatusBarSetup*>::iterator iter;
	for (iter = m_statusbar_item_list.begin(); iter != m_statusbar_item_list.end(); /*none*/) {
		UiStatusBarSetup *item = *iter;
		statusbar->insertPermanentWidget(-1, item->m_widget);
		delete item;
		m_statusbar_item_list.erase(iter++);
	}

	return 0;
}

UiToolBarSetup *MainWindow::toolBarSetup(UiToolBarSetup::UiToolBarSeq seq, QString name)
{
	/*Check exist*/
	QList<UiToolBarSetup*>::iterator iter;
	for (iter = m_toolbar_item_list.begin(); iter != m_toolbar_item_list.end(); iter++) {
		UiToolBarSetup *setup = *iter;
		if (setup->m_seq == seq) {
			break;
		}
	}
	if (iter != m_toolbar_item_list.end()) {
		return *iter;
	}

	/*Setup*/
	UiToolBarSetup *new_setup = new UiToolBarSetup(seq, name);
	int count = m_toolbar_item_list.count();
	int iloop;
	for (iloop = 0; iloop < count; iloop++) {
		UiToolBarSetup *setup = m_toolbar_item_list[iloop];
		if (seq < setup->m_seq) {
			m_toolbar_item_list.insert(iloop, new_setup);
			break;
		}
	}
	if (iloop == count) {
		m_toolbar_item_list.insert(count, new_setup);
	}
	return new_setup;
}

/*Get toolbar handle*/
QToolBar *MainWindow::toolBarGetHandl(QString name)
{
	/*Get exist*/
	QList<UiToolbarHandl*>::iterator iter;
	for (iter = m_toolbar_handl_list.begin(); iter != m_toolbar_handl_list.end(); iter++) {
		UiToolbarHandl *handl = *iter;
		if (handl->m_name == name) {
			return handl->m_toolbar;
		}
	}

	/*Make new*/
	QToolBar *toolbar = addToolBar(name);
	toolbar->setIconSize(QSize(20,20));

	UiToolbarHandl *handl = new UiToolbarHandl(name, toolbar);
	m_toolbar_handl_list.push_back(handl);
	return toolbar;
}

int MainWindow::toolBarApply()
{
	QList<UiToolBarSetup*>::iterator iter;
	for (iter = m_toolbar_item_list.begin(); iter != m_toolbar_item_list.end(); iter++) {
		UiToolBarSetup *item = *iter;
		QToolBar *toolbar = toolBarGetHandl(item->m_name);
		QList<QAction*>::iterator act_iter;
		for (act_iter = item->m_actions.begin(); act_iter != item->m_actions.end(); act_iter++) {
			QAction *act = *act_iter;
			toolbar->addAction(act);
		}
	}
	return 0;
}

UiMenuSetup *MainWindow::menuSetup(UiMenuSetup::UiMenuSeq seq, QStringList setto)
{
	/*Check exist*/
	QList<UiMenuSetup*>::iterator iter;
	for (iter = m_menu_item_list.begin(); iter != m_menu_item_list.end(); iter++) {
		UiMenuSetup *setup = *iter;
		if (setup->m_seq == seq) {
			break;
		}
	}
	if (iter != m_menu_item_list.end()) {
		return *iter;
	}

	/*New handle*/
	UiMenuSetup *new_setup = new UiMenuSetup(seq, setto);
	int count = m_menu_item_list.count();
	int iloop;
	for (iloop = 0; iloop < count; iloop++) {
		UiMenuSetup *setup = m_menu_item_list[iloop];
		if (seq < setup->m_seq) {
			m_menu_item_list.insert(iloop, new_setup);
			break;
		}
	}
	if (iloop == count) {
		m_menu_item_list.insert(count, new_setup);
	}
	return new_setup;
}

/*Get QMenu for apply*/
QMenu *MainWindow::menuGetHandl(QStringList &setto)
{
	/*Check exist*/
	QList<UiMenuHandl*>::iterator set_iter;
	for (set_iter = m_menu_handl_list.begin(); set_iter != m_menu_handl_list.end(); set_iter++) {
		UiMenuHandl *menuat = *set_iter;
		if (menuat->m_strlist == setto) {
			return menuat->m_menu;
		}
	}

	int count = setto.count();
	if (count == 1) {
		/* Head level: OOOO.xxx.xxx.xxx */
		QString name = setto.at(0);
		QMenu *new_menu = menuBar()->addMenu(name);	/*Add to top level*/

		UiMenuHandl *menuat = new UiMenuHandl(setto, new_menu);
		m_menu_handl_list.push_back(menuat);
		return new_menu;
	} else {
		/* Get the short one(up level), ooo.ooo.ooo.xxx */
		QStringList sub = setto.mid(0, count - 1);
		QMenu *up_menu = menuGetHandl(sub);

		QString name = setto.at(count - 1);			/*Last level name*/
		QMenu *new_menu = up_menu->addMenu(name);	/*Add to up level*/

		UiMenuHandl *menuat = new UiMenuHandl(setto, new_menu);
		m_menu_handl_list.push_back(menuat);
		return new_menu;
	}
}

int MainWindow::menuApply()
{
	QMetaObject::Connection conn;

	/*phase 1: apply all setup menu*/
	QList<UiMenuSetup*>::iterator iter;
	for (iter = m_menu_item_list.begin(); iter != m_menu_item_list.end(); iter++) {
		UiMenuSetup *item = *iter;
		QMenu *to_menu = menuGetHandl(item->m_setto);
		/*Add action*/
		QList<QAction*>::iterator act_iter;
		for (act_iter = item->m_actions.begin(); act_iter != item->m_actions.end(); act_iter++) {
			QAction *act = *act_iter;
			if (act == NULL) {
				to_menu->addSeparator();
			} else {
				to_menu->addAction(act);
			}
		}
	}

	/*phase 2: Window menu*/
	QStringList windows_menu;
	windows_menu << "&Window";
	QMenu *window_menu = menuGetHandl(windows_menu);

	m_window_toolbar_menu = new QMenu("Toolbar", this);
	m_window_dock_menu = new QMenu("Dock", this);
	QList<QAction*> window_actions = window_menu->actions();
	if (window_actions.empty()) {
		window_menu->addMenu(m_window_toolbar_menu);
		window_menu->addMenu(m_window_dock_menu);
	} else {
		window_menu->insertMenu(window_actions.front(), m_window_toolbar_menu);
		window_menu->insertMenu(window_actions.front(), m_window_dock_menu);
	}

	/*phase 3: Toolbar item in menu*/
	QList<UiToolbarHandl*>::iterator toolbar_iter;
	for (toolbar_iter = m_toolbar_handl_list.begin();
		 toolbar_iter != m_toolbar_handl_list.end(); toolbar_iter++) {
		UiToolbarHandl *handl = *toolbar_iter;
		QToolBar *toolbar = handl->m_toolbar;

		QAction *action = new QAction(handl->m_name, this);
		action->setCheckable(true);
		action->setData(QVariant::fromValue(toolbar));

		m_window_toolbar_menu->addAction(action);

		/*For visibility and select change*/
		conn = connect(action, SIGNAL(triggered()), this, SLOT(slotClickToolbarMenu()));
		UTER_ASSERT(conn != NULL);
		conn = connect(toolbar, SIGNAL(visibilityChanged(bool)), this, SLOT(slotFlushToolbarMenu(bool)));
		UTER_ASSERT(conn != NULL);
	}
	slotFlushToolbarMenu();

	/*phase 4: Dock item in menu*/
	QList<WidgetWithDock*>::iterator dock_iter;
	for (dock_iter = m_dock_item_list.begin();
		 dock_iter != m_dock_item_list.end(); dock_iter++) {
		DockPlus *dock = (*dock_iter)->m_dock;

		QAction *action = new QAction((*dock_iter)->m_name, this);
		action->setCheckable(true);
		action->setData(QVariant::fromValue(dock));

		m_window_dock_menu->addAction(action);

		conn = connect(action, SIGNAL(triggered()), this, SLOT(slotClickDockMenu()));
		UTER_ASSERT(conn != NULL);
		conn = connect(dock, SIGNAL(visibilityChanged(bool)), this, SLOT(slotFlushDockMenu(bool)));
		UTER_ASSERT(conn != NULL);
	}
	slotFlushDockMenu();

	return 0;
}

void MainWindow::slotFlushToolbarMenu(bool nouse)
{
	QList<QAction*> action_list = m_window_toolbar_menu->actions();
	QList<QAction*>::iterator action_iter;
	for (action_iter = action_list.begin(); action_iter != action_list.end(); action_iter++) {
		QAction *menu_action = *action_iter;
		QToolBar *toolbar = menu_action->data().value<QToolBar*>();
		QAction *toolbar_action = toolbar->toggleViewAction();
		if (toolbar_action->isChecked()) {
			menu_action->setChecked(true);
		} else {
			menu_action->setChecked(false);
		}
	}

	return;
}

void MainWindow::slotClickToolbarMenu()
{
	QAction *menu_action = dynamic_cast<QAction*>(QObject::sender());
	if (menu_action == NULL) {
		return;
	}

	QToolBar *toolbar = menu_action->data().value<QToolBar*>();
	QAction *toolbar_action = toolbar->toggleViewAction();
	if (menu_action->isChecked()) {
		toolbar->setVisible(true);
	} else {
		toolbar->setVisible(false);
	}

	return;
}

void MainWindow::slotFlushDockMenu(bool nouse)
{
	QList<QAction*> action_list = m_window_dock_menu->actions();
	QList<QAction*>::iterator action_iter;
	for (action_iter = action_list.begin(); action_iter != action_list.end(); action_iter++) {
		QAction *menu_action = *action_iter;

		DockPlus *dock = menu_action->data().value<DockPlus*>();
		if (dock->isVisible()) {
			menu_action->setChecked(true);
		} else {
			menu_action->setChecked(false);
		}
	}

	return;
}

void MainWindow::slotClickDockMenu()
{
	QAction *menu_action = dynamic_cast<QAction*>(QObject::sender());
	if (menu_action == NULL) {
		return;
	}

	DockPlus *dock = menu_action->data().value<DockPlus*>();
	if (menu_action->isChecked()) {
		dock->setVisible(true);
	} else {
		dock->setVisible(false);
	}

	return;
}

/*When item close*/
void MainWindow::slotEditClose(MdiItem *item)
{
	UTER_ASSERT(item != NULL);

	/*TODO: Just support editor */
	UEditor *edit = (UEditor *)item->widget();

	editClose(edit);
	return;
}

/*Add to mdi*/
int MainWindow::editAddToMdi(UEditor *edit)
{
	UTER_ASSERT(edit != NULL);
	QMetaObject::Connection conn;

	m_center_mdi->addMdiItem(edit->m_mdi_item);
	edit->m_mdi_item->show();

	if (m_enable_lineno) {
		edit->setEnableLineNo(true);
	} else {
		edit->setEnableLineNo(false);
	}

	/*Signal to edit/find/replace ware*/
	conn = connect(edit, SIGNAL(signalKeyCtrlF()), this->m_edit, SLOT(slotEditFindReplace()));
	UTER_ASSERT(conn != NULL);
	conn = connect(edit, SIGNAL(signalKeyInsert()), this->m_edit, SLOT(slotEditActionFlush()));
	UTER_ASSERT(conn != NULL);
	conn = connect(edit, SIGNAL(signalPositionChanged()), this->m_edit, SLOT(slotEditActionFlush()));
	UTER_ASSERT(conn != NULL);
	conn = connect(edit, SIGNAL(signalFocusChanged()), this->m_edit, SLOT(slotEditActionFlush()));
	UTER_ASSERT(conn != NULL);
	conn = connect(edit, SIGNAL(signalSelectChanged()), this->m_edit, SLOT(slotEditActionFlush()));
	UTER_ASSERT(conn != NULL);
	return 0;
}

UEditor *MainWindow::editGetByPath(QString file)
{
	QList<MdiItem *> itemlist = m_center_mdi->getItemList();
	QList<MdiItem *>::iterator iter;
	for (iter = itemlist.begin(); iter != itemlist.end(); iter++) {
		MdiItem *item = *iter;
		/*TODO: Just support editor */
		UEditor *editor = (UEditor *)item->widget();
		if (editor->m_file_path == file) {
			return editor;
		}
	}
	return NULL;
}

UEditor *MainWindow::editGetActive()
{
	MdiItem *item = m_center_mdi->getActiveItem();
	if (item == NULL) {
		return NULL;
	}
	/*TODO: Just support editor */
	UEditor *editor = (UEditor *)item->widget();
	return editor;
}

int MainWindow::editClose(UEditor *edit)
{
	UTER_ASSERT(edit != NULL);

	/*Remove from mdi*/
	m_center_mdi->removeMdiItem(edit->m_mdi_item);
	/*Delete editor (later)*/
	edit->deleteLater();
	return 0;
}

int MainWindow::editSetActive(UEditor *edit)
{
	m_center_mdi->setActiveItem(edit->m_mdi_item);
	return 0;
}

int MainWindow::editSetLineNoEnable(bool set)
{
	m_enable_lineno = set;

	QList<MdiItem *> itemlist = m_center_mdi->getItemList();
	QList<MdiItem *>::iterator iter;
	for (iter = itemlist.begin(); iter != itemlist.end(); iter++) {
		MdiItem *mdiitem = *iter;
		UEditor *editor = (UEditor *)mdiitem->widget();
		if (m_enable_lineno) {
			editor->setEnableLineNo(true);
		} else {
			editor->setEnableLineNo(false);
		}
	}

	return 0;
}

MainWindow *g_mainwin = NULL;
