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

#ifndef SRC_UI_REGISTER_HXX_
#define SRC_UI_REGISTER_HXX_

#include <ui_public.hxx>

namespace RegUi {

class RegMon {
public:
	class Item {
	public:
		Item()
		{
			m_name = "";
			m_value = 0;
			m_changed = false;
			return;
		}
		QString m_name;
		unsigned long long m_value;
		bool m_changed;
	};

	RegMon(unsigned int pid, QString name)
	{
		m_pid = pid;
		m_item = new QListWidgetItem(QString::asprintf("%d", m_pid) + " (" + name + ")");
		m_item->setData(Qt::UserRole, QVariant::fromValue((void *)this));
		return;
	}
	~RegMon()
	{
		QList<Item*>::iterator iter;
		for (iter = m_reg_list.begin(); iter != m_reg_list.end(); /*none*/) {
			Item *item = *iter;
			delete item;
			m_reg_list.erase(iter++);
		}
		delete m_item;
		return;
	}
	int loadData();
	unsigned int m_pid;					/*Thread pid*/
	QListWidgetItem *m_item;			/*Item on thread list*/
	QList<Item*> m_reg_list;			/*Register values*/
};

class RegDock: public WidgetWithDock {
	Q_OBJECT
private slots:
	void slotListItemClicked(QListWidgetItem *);
	void slotTableItemTextChanged(QTableWidgetItem *);
public:
	RegDock();
	~RegDock()
	{
		clear();
		return;
	}
	int updateOnStop();
	int updateOnRun();
	int updateOnShut();
private:
	int loadRegs(RegMon *mon);
	int clear();

	QMap<unsigned int, RegMon*> m_mon_map;
	QListWidget *m_list_monitor;				/*Show threads*/
	QTableWidget *m_table_register;				/*Show register value of thread*/
	bool m_data_setting;
};

} /*namespace RegUi*/
#endif /*SRC_UI_REGISTER_HXX_*/
