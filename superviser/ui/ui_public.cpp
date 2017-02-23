

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

#include <ui_public.hxx>

#include <ui_mainwin.hxx>

TabBar::TabBar(QWidget *parent)
:QTabBar(parent)
{
	QMetaObject::Connection conn;

	m_close_this_act = new QAction("Close", this);
	m_close_rigth_act = new QAction("Close Right", this);
	m_close_others_act = new QAction("Close Others", this);
	m_close_all_act = new QAction("Close All", this);

	m_pop_menu = new QMenu(this);
	m_pop_menu->addAction(m_close_this_act);
	m_pop_menu->addAction(m_close_rigth_act);
	m_pop_menu->addAction(m_close_others_act);
	m_pop_menu->addAction(m_close_all_act);

	m_idx_last_pop_on = -1;

	conn = connect(m_close_this_act, SIGNAL(triggered()), this, SLOT(slotCloseThisTriggered()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_close_rigth_act, SIGNAL(triggered()), this, SLOT(slotCloseRightTriggered()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_close_others_act, SIGNAL(triggered()), this, SLOT(slotCloseOthersTriggered()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_close_all_act, SIGNAL(triggered()), this, SLOT(slotCloseAllTriggered()));
	UTER_ASSERT(conn != NULL);
	return;
}

void TabBar::slotCloseThisTriggered()
{
	emit signalTabClose(UITAB_CLOSE_THIS, m_idx_last_pop_on);
	return;
}

void TabBar::slotCloseRightTriggered()
{
	emit signalTabClose(UITAB_CLOSE_RIGHT, m_idx_last_pop_on);
	return;
}

void TabBar::slotCloseOthersTriggered()
{
	emit signalTabClose(UITAB_CLOSE_OTHERS, m_idx_last_pop_on);
	return;
}

void TabBar::slotCloseAllTriggered()
{
	emit signalTabClose(UITAB_CLOSE_ALL, m_idx_last_pop_on);
	return;
}

void TabBar::contextMenuEvent(QContextMenuEvent *event)
{
	QPoint mouse_at = QCursor::pos();

	/*Get index menu pop at*/
	QPoint tab_map = this->mapFromGlobal(mouse_at);
	m_idx_last_pop_on = tabAt(tab_map);
	if (m_idx_last_pop_on < 0) {
		return;
	}

	/*Pop menu*/
	m_pop_menu->exec(mouse_at);
    event->accept();
	return;
}

MdiFrame::MdiFrame(QWidget *parent)
:QWidget(parent)
{
	QMetaObject::Connection conn;

	/* Out layout
	 * --------------------------------
	 *      Tab and more button
	 * --------------------------------
	 *      Qt mdi area
	 *
	 * --------------------------------*/
	QVBoxLayout *out_layout = new QVBoxLayout(this);
	out_layout->setSpacing(0);
	setLayout(out_layout);

	/* Top layout
	 * ---------------------------------
	 *   Tab bar               | more  |
	 * ---------------------------------*/
	QHBoxLayout *top_layout = new QHBoxLayout(this);
	top_layout->setSpacing(0);
	out_layout->addLayout(top_layout, 0);

	m_tab_bar = new TabBar(this);
	top_layout->addWidget(m_tab_bar, 0, Qt::AlignLeft);
	m_tab_bar->setTabsClosable(true);			/*Closeable*/
	m_tab_bar->setUsesScrollButtons(false);		/*Tab scroll xxxx|xxxx|xxxx|<<>>, disabled*/
	m_tab_bar->setElideMode(Qt::ElideNone);		/*Filename -> Filena..., disabled*/
	m_tab_bar->setExpanding(false);				/*|Filename| -> |   Filename   |, disabled*/
	/*Shadow same as visible tab, but it is not visible*/
	m_shadow_bar = new TabBar(this);
	m_shadow_bar->setTabsClosable(true);
	m_shadow_bar->setUsesScrollButtons(false);
	m_shadow_bar->setElideMode(Qt::ElideNone);
	m_shadow_bar->setExpanding(false);
	m_shadow_bar->setVisible(false);

	m_more_btn = new QPushButton("", this);
	m_more_btn->setMinimumWidth(60);
	m_more_btn->setMaximumWidth(60);
	top_layout->addWidget(m_more_btn, 0, Qt::AlignRight);

	/*No layout for more list*/
	m_more_list = new QListWidget(this);
	m_more_list->setVisible(false);
	m_more_list->setResizeMode(QListView::Adjust);

	/*Bottom layout*/
	m_mdi_area = new QMdiArea(this);
	out_layout->addWidget(m_mdi_area, 0);
	/*Enable scroll when needed*/
	m_mdi_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	m_mdi_area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	/*Subwindow mode with custom tab*/
	m_mdi_area->setViewMode(QMdiArea::SubWindowView);

	installEventFilter(this);

	conn = connect(m_tab_bar, SIGNAL(tabBarClicked(int)), this, SLOT(slotTabBarClicked(int)));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_tab_bar, SIGNAL(tabCloseRequested(int)), this, SLOT(slotTabBtnClose(int)));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_tab_bar, SIGNAL(signalTabClose(int, int)), this, SLOT(slotTabMenuClose(int, int)));
	UTER_ASSERT(conn != NULL);

	conn = connect(m_more_btn, SIGNAL(pressed()), this, SLOT(slotMoreBtnPressed()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_more_list, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(slotMoreListClicked(QListWidgetItem*)));
	UTER_ASSERT(conn != NULL);

	flushTabBar();
	return;
}

/*Click tab item for active*/
void MdiFrame::slotTabBarClicked(int index)
{
	if (index >= m_item_list.length()) {
		return;
	}

	MdiItem *item = m_item_list[index];
	setActiveItem(item);
	return;
}

void MdiFrame::slotTabBtnClose(int index)
{
	if (index >= m_item_list.length()) {
		return;
	}

	MdiItem *item = m_item_list[index];
	emit closeItem(item);
	return;
}

/*Pop menu close tab item*/
void MdiFrame::slotTabMenuClose(int type, int index)
{
	if (index >= m_item_list.length()) {
		return;
	}

	switch(type)
	{
	case TabBar::UITAB_CLOSE_THIS:
	{
		MdiItem *cur_item = m_item_list[index];
		emit closeItem(cur_item);
		break;
	}
	case TabBar::UITAB_CLOSE_ALL:
	{
		QList<MdiItem *> copy_list = m_item_list;
		QList<MdiItem *>::iterator iter;
		for (iter = copy_list.begin(); iter != copy_list.end(); iter++) {
			MdiItem *item = *iter;
			emit closeItem(item);
		}
		break;
	}
	case TabBar::UITAB_CLOSE_OTHERS:
	{
		MdiItem *cur_item = m_item_list[index];
		QList<MdiItem *> copy_list = m_item_list;
		QList<MdiItem *>::iterator iter;
		for (iter = copy_list.begin(); iter != copy_list.end(); iter++) {
			MdiItem *item = *iter;
			if (item != cur_item) {
				emit closeItem(item);
			}
		}
		break;
	}
	case TabBar::UITAB_CLOSE_RIGHT:
	{
		MdiItem *cur_item = m_item_list[index];
		QList<MdiItem *> copy_list = m_item_list;
		QList<MdiItem *>::iterator iter;
		bool action = false;
		for (iter = copy_list.begin(); iter != copy_list.end(); iter++) {
			MdiItem *item = *iter;
			if (action) {
				emit closeItem(item);
			}
			if (item == cur_item) {
				action = true;
			}
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

/*Forward subwin signal*/
void MdiFrame::slotSubwinCloseItem(QCloseEvent *closeEvent)
{
	MdiItem *item = dynamic_cast<MdiItem*>(QObject::sender());
	if (item == NULL) {
		return;
	}

	emit closeItem(item);
	return;
}

/*Forward subwin signal*/
void MdiFrame::slotSubwinStateChanged(Qt::WindowStates oldState, Qt::WindowStates newState)
{
	MdiItem *item = dynamic_cast<MdiItem*>(QObject::sender());
	if (item == NULL) {
		return;
	}

	if ((oldState & Qt::WindowMaximized) != (newState & Qt::WindowMaximized)) {
		/*Tabbar show and hiden*/
		flushTabBar();
		return;
	}
	if (newState & Qt::WindowActive) {
		/*Tabbar item selected*/
		flushTabBar();
		return;
	}
	return;
}

void MdiFrame::slotMoreBtnPressed()
{
	if (m_more_list->count() <= 0) {
		return;
	}

	/* ------------------------------o <---Here
	 *                        | More |
	 * -------------------------------
	 *
	 * ------------------------------- */
	QPoint btnpoint = m_more_btn->geometry().bottomRight();

	/*                |-------Here
	 *                V
	 * ---------------o---------------
	 *                | ------------ |
	 * ---------------| ------------ |
	 *                | ------------ |
	 * ------------------------------- */
	QPoint listpoint = btnpoint - QPoint(m_more_list->size().width(), 0);

	/*Pop more list*/
	m_more_list->move(listpoint);
	m_more_list->setVisible(true);
	m_more_list->raise();
	m_more_list->setFocus(Qt::MouseFocusReason);

	/*Select first item*/
	QListWidgetItem *item = m_more_list->item(0);
	if (item == NULL) {
		return;
	}
	item->setSelected(true);

	return;
}

void MdiFrame::slotMoreListClicked(QListWidgetItem *listitem)
{
	/*Hiden more list*/
	m_more_list->setVisible(false);
	/*More list index*/
	int index = listitem->data(Qt::UserRole).value<int>();
	/*Item list index*/
	index = m_tab_bar->count() + index;
	if (index >= m_item_list.length()) {
		return;
	}

	MdiItem *item = m_item_list[index];
	setActiveItem(item);
	return;
}

int MdiFrame::addMdiItem(MdiItem *item)
{
	QMetaObject::Connection conn;

	/*Check exist*/
	QList<MdiItem *>::iterator iter;
	for (iter = m_item_list.begin(); iter != m_item_list.end(); iter++) {
		MdiItem *itemloop = *iter;
		if (itemloop == item) {
			return -1;
		}
	}

	/*Add item to mdi*/
	m_item_list.push_back(item);
	item->setParent(m_mdi_area);
	m_mdi_area->addSubWindow((QMdiSubWindow *)item);

	/*Max size*/
	item->setWindowState(item->windowState() | Qt::WindowMaximized);

	/*Forward subwin signal, subwin close*/
	conn = connect(item, SIGNAL(signalMdiClose(QCloseEvent*)),
			       this, SLOT(slotSubwinCloseItem(QCloseEvent*)));
	UTER_ASSERT(conn != NULL);
	/*Forward subwin signal, subwin active change*/
	conn = connect(item, SIGNAL(windowStateChanged(Qt::WindowStates,Qt::WindowStates)),
			       this, SLOT(slotSubwinStateChanged(Qt::WindowStates, Qt::WindowStates)));
	UTER_ASSERT(conn != NULL);

	flushTabBar();
	return 0;
}

int MdiFrame::removeMdiItem(MdiItem *item)
{
	QList<MdiItem *>::iterator iter;
	for (iter = m_item_list.begin(); iter != m_item_list.end(); iter++) {
		MdiItem *itemloop = *iter;
		if (itemloop == item) {
			break;
		}
	}
	if (iter == m_item_list.end()) {
		return -1;
	}
	MdiItem *act_item = getActiveItem();
	if (act_item == NULL) {
		return -1;
	}

	bool max_new_active = false;
	if (act_item == item && (item->windowState() & Qt::WindowMaximized)) {
		/* If remove the active item with max size,
		 * we need max size the new active item after remove*/
		max_new_active = true;
	}

	/*Remove from mdi*/
	m_mdi_area->removeSubWindow((QMdiSubWindow *)item);
	m_item_list.erase(iter);

	if (max_new_active) {
		MdiItem *new_act_item = getActiveItem();
		if (new_act_item != NULL) {
			new_act_item->setWindowState(new_act_item->windowState() | Qt::WindowMaximized);
		}
	}

	flushTabBar();
	return 0;
}

QList<MdiItem *> MdiFrame::getItemList(unsigned int user_type)
{
	return m_item_list;
}

int MdiFrame::setActiveItem(MdiItem *item)
{
	m_mdi_area->setActiveSubWindow((QMdiSubWindow *)item);
	return 0;
}

MdiItem *MdiFrame::getActiveItem()
{
	return (MdiItem *)m_mdi_area->activeSubWindow();
}

void MdiFrame::resizeEvent(QResizeEvent *event)
{
	/*Do resize first*/
	QWidget::resizeEvent(event);
	/*Flush tab bar*/
	flushTabBar();
	return;
}

bool MdiFrame::eventFilter(QObject *watched, QEvent *event)
{
	if (m_more_list->isVisible()
		&& event->type() == QEvent::MouseButtonPress
		&& watched != m_more_list) {
		/*When more list visible, and mouse press other ware*/
		m_more_list->setVisible(false);
	}

	/*Keep event*/
	return false;
}

int MdiFrame::flushTabBar()
{
	emit signalActiveChange();
redo:
	/*Do clear*/
	m_more_list->setVisible(false);
	m_more_list->clear();
	while (m_tab_bar->count() != 0) {
		m_tab_bar->removeTab(0);
	}
	while (m_shadow_bar->count() != 0) {
		m_shadow_bar->removeTab(0);
	}

	/*If no any item*/
	if (m_item_list.length() == 0) {
		m_tab_bar->setVisible(false);
		m_more_btn->setVisible(false);
		return 0;
	}

	MdiItem *activeitem = (MdiItem *)m_mdi_area->activeSubWindow();

	/*Try add item to shadow tabbar*/
	bool have_active = false;
	int tab_size = 0;
	QList<MdiItem *>::iterator iter;
	for (iter = m_item_list.begin(); iter != m_item_list.end(); iter++, tab_size++) {
		MdiItem *item = *iter;
		m_shadow_bar->addTab(item->m_icon, item->m_title);
		if (m_shadow_bar->sizeHint().width() >= m_mdi_area->size().width() - m_more_btn->size().width()) {
			/*No enough space in frame*/
			break;
		}
		if (activeitem == item) {
			/*Active item in tab*/
			have_active = true;
		}
	}

	/*If active item not in show range, we need adjust index for items(to first of list)*/
	if (! have_active) {
		for (iter = m_item_list.begin(); iter != m_item_list.end(); iter++) {
			MdiItem *item = *iter;
			if (activeitem == item) {
				break;
			}
		}
		if (iter == m_item_list.end()) {
			/*Some wrong*/
			return -1;
		}
		MdiItem *item = *iter;
		m_item_list.erase(iter);
		m_item_list.push_front(item);
		goto redo;
	}

	/*Add item to tab and more list*/
	bool have_maxd = false;
	int iloop = 0;
	for (iter = m_item_list.begin(); iter != m_item_list.end(); iter++, iloop++) {
		MdiItem *item = *iter;

		if (iloop < tab_size) {
			/*Add to tabbar*/
			m_tab_bar->addTab(item->m_icon, item->m_title);
			m_tab_bar->setTabToolTip(iloop, item->m_tip);
			if (activeitem == item) {
				m_tab_bar->setCurrentIndex(iloop);
			}
		} else {
			/*Add to more list*/
			int list_index = m_more_list->count();
			QListWidgetItem *listitem = new QListWidgetItem();
			listitem->setText(item->m_title);
			listitem->setToolTip(item->m_tip);
			listitem->setData(Qt::UserRole, QVariant::fromValue(list_index));
			m_more_list->addItem(listitem);
		}

		Qt::WindowStates stat = item->windowState();
		if (Qt::WindowMaximized & stat) {
			/*Have any max size item*/
			have_maxd = true;
		}
	}

	/*Setup more button show/hiden*/
	if (m_more_list->count() == 0) {
		m_more_btn->setEnabled(false);
		m_more_btn->setText(">>");
	} else {
		m_more_btn->setEnabled(true);
		m_more_btn->setText(QString::asprintf(">>%d", m_more_list->count()));
		m_more_list->adjustSize();
	}

	/*Setup tabbar show/hiden*/
	if (m_item_list.length() == 0 || have_maxd == false) {
		m_tab_bar->setVisible(false);
		m_more_btn->setVisible(false);
	} else {
		m_tab_bar->setVisible(true);
		m_more_btn->setVisible(true);
	}

	return 0;
}

