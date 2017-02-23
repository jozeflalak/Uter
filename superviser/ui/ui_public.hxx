
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

#ifndef SRC_UI_PUBLIC_HXX_
#define SRC_UI_PUBLIC_HXX_

#include <QtWidgets/QtWidgets>
#include <target.hxx>

/*Convert active event to active signal*/
class DockPlus: public QDockWidget
{
	Q_OBJECT
signals:
	void signalActive();
public:
	DockPlus(const QString &title, QWidget *parent = 0)
	:QDockWidget(title, parent)
	{
		return;
	}
	bool event(QEvent *event)
	{
		if (event->type() == QEvent::WindowActivate) {
			emit signalActive();
		}
		return QDockWidget::event(event);
	}
};

/*Widget for use, Dock for setup*/
class WidgetWithDock: public QWidget
{
	Q_OBJECT
public:
	enum UiDockSeq {
		SEQ_PRJ = 0,
		SEQ_CONSOLE,
		SEQ_NEW,
		SEQ_VAR,
		SEQ_WATCH,
		SEQ_THRD,
		SEQ_REG,
		SEQ_DASM,
		SEQ_MEM,
		SEQ_MAP,
		SEQ_MOD,
		SEQ_BRKPNT,
		SEQ_COV,
		SEQ_OVERVIEW,
		SEQ_MAX
	};

	WidgetWithDock(UiDockSeq seq, Qt::DockWidgetArea area, QString name, QWidget *parent = 0)
	{
		m_seq = seq;
		m_area = area;
		m_name = name;

		m_dock = new DockPlus(m_name, parent);
		m_dock->setWidget(this);
		return;
	}

	UiDockSeq m_seq;				/*Index*/
	Qt::DockWidgetArea m_area;		/*Dock location*/
	QString m_name;					/*Dock name*/
	DockPlus *m_dock;				/*Dock ware*/
};

/*Use on top of MdiFrame*/
class TabBar: public QTabBar {
	Q_OBJECT
public:
	TabBar(QWidget *parent = 0);
signals:
	void signalTabClose(int, int);						/*Emit on tab close*/
public slots:
	void slotCloseThisTriggered();
	void slotCloseRightTriggered();
	void slotCloseOthersTriggered();
	void slotCloseAllTriggered();
public:
	enum UiTabCloseType {
		UITAB_CLOSE_THIS = 0,
		UITAB_CLOSE_ALL,
		UITAB_CLOSE_OTHERS,
		UITAB_CLOSE_RIGHT,
	};
private:
	void contextMenuEvent(QContextMenuEvent *event);	/*Pop menu event*/
	QMenu *m_pop_menu;
	QAction *m_close_this_act;
	QAction *m_close_rigth_act;
	QAction *m_close_others_act;
	QAction *m_close_all_act;

	int m_idx_last_pop_on;								/*The index menu last pop on*/
};

class MdiItem: public QMdiSubWindow {
	Q_OBJECT
signals:
	void signalMdiClose(QCloseEvent *closeEvent);		/*Emit on item close*/
public:
	MdiItem(QWidget *parent = 0, unsigned int user_type = 0)
	:QMdiSubWindow(parent)
	{
		m_user_type = user_type;

		m_title = "";
		m_tip = "";
		return;
	}
public:
	int setIcon(QIcon icon)
	{
		m_icon = icon;
		setWindowIcon(m_icon);
		return 0;
	}
	int setTitle(QString title)
	{
		m_title = title;
		setWindowTitle(m_title);
		return 0;
	}
	int setTip(QString tip)
	{
		m_tip = tip;
		setWhatsThis(m_tip);
		return 0;
	}

	unsigned int m_user_type;
	QIcon m_icon;
	QString m_title;
	QString m_tip;

private:
	void closeEvent(QCloseEvent *closeEvent)
	{
		emit signalMdiClose(closeEvent);
		return;
	}
};

/*MdiFrame include tab and Qt mdi area*/
class MdiFrame: public QWidget {
	Q_OBJECT
public:
	MdiFrame(QWidget *parent = 0);
signals:
	/*Any close/delete/active/... processed by slot, MdiFrame just emit signal*/
	void closeItem(MdiItem *item);			/*Item close*/
	void signalActiveChange();				/*Item active change*/
private slots:
	void slotTabBarClicked(int);			/*Click on tab item*/
	void slotTabBtnClose(int);				/*Click close icon on tab item*/
	void slotTabMenuClose(int, int);		/*Pop menu on tab time*/

	void slotMoreBtnPressed();						/*Press more button*/
	void slotMoreListClicked(QListWidgetItem *);	/*Click more list*/

	void slotSubwinCloseItem(QCloseEvent *);							/*Forward subwin signal*/
	void slotSubwinStateChanged(Qt::WindowStates, Qt::WindowStates);	/*Forward subwin signal*/
public:
	/*Item management*/
	int addMdiItem(MdiItem *item);
	int removeMdiItem(MdiItem *item);
	int setActiveItem(MdiItem *item);
	MdiItem *getActiveItem();
	QList<MdiItem *> getItemList(unsigned int user_type = 0);

	int flushTabBar();
private:
	void resizeEvent(QResizeEvent *event);				/*Frame resize, adjust tab item*/
	bool eventFilter(QObject *watched, QEvent *event);	/*Mouse move out of more list*/
	/* ----------------------------------------------
	 * | 1xx | 2xx | 3xx | 4xx | 5xx | 6xx | 7xx |more|
	 * ------------------------------------------------
	 *                                     |   8xx    |
	 *           Qt MDI                    |   9xx    |
	 *                                     |  10xx    |
	 * -----------------------------------------------*/
	TabBar *m_tab_bar;						/*Tab bar*/
	TabBar *m_shadow_bar;					/*Tab bar(shadow)*/
	QPushButton *m_more_btn;				/*More button*/
	QListWidget *m_more_list;				/*More list*/
	QMdiArea *m_mdi_area;					/*Qt Mdi*/
	QList<MdiItem *> m_item_list;			/*Item list*/
};

#endif /*SRC_UI_PUBLIC_HXX_*/
