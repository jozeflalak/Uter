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

namespace RegUi {

enum {
	NUM_OF_COLUMM = 5,
};

enum RegColumn {
	REG_COLUMM_NAME = 0,
	REG_COLUMM_VALUE,
	REG_COLUMM_MAX,
};

static const char *g_reg_view_column[] = {
	"Name",
	"Value",
};

UTER_ARRAY_CHECK(g_reg_view_column, REG_COLUMM_MAX);

struct RegDataInfo {
public:
	RegDataInfo()
	{
		m_mon = NULL;
		m_name = "";
		return;
	}
	RegDataInfo(RegMon *mon, QString name)
	{
		m_mon = mon;
		m_name = name;
		return;
	}
	RegMon *m_mon;
	QString m_name;
};

int RegMon::loadData()
{
	RegValues *getregvals = g_target->pidRegsGet(m_pid);
	if (getregvals == NULL) {
		return -1;
	}

	if (m_reg_list.empty()) {
		QList<RegValues::Item*>::iterator get_iter;
		for (get_iter = getregvals->m_reglist.begin(); get_iter != getregvals->m_reglist.end(); get_iter++) {
			RegValues::Item *getitem = *get_iter;

			RegMon::Item *regitem = new RegMon::Item();
			regitem->m_name = getitem->m_name;
			regitem->m_value = getitem->m_value;
			regitem->m_changed = false;

			m_reg_list.append(regitem);
		}
	} else {
		QList<RegValues::Item*>::iterator get_iter;
		QList<RegMon::Item*>::iterator reg_iter;
		for (get_iter = getregvals->m_reglist.begin(), reg_iter = m_reg_list.begin();
			 get_iter != getregvals->m_reglist.end() && reg_iter != m_reg_list.end();
			 get_iter++, reg_iter++) {
			RegValues::Item *getitem = *get_iter;
			RegMon::Item *regitem = *reg_iter;

			if (regitem->m_name != getitem->m_name) {
				/*Some wrong*/
				break;
			}

			/*Compare register value*/
			if (regitem->m_value == getitem->m_value) {
				regitem->m_changed = false;
			} else {
				regitem->m_changed = true;
			}
			regitem->m_value = getitem->m_value;
		}
	}

	delete getregvals;
	return 0;
}

RegDock::RegDock()
:WidgetWithDock(SEQ_REG, Qt::BottomDockWidgetArea, "Register")
{
	QMetaObject::Connection conn;

	QHBoxLayout *out_layout = new QHBoxLayout(this);
	setLayout(out_layout);

	QSplitter *out_split = new QSplitter(Qt::Horizontal, this);
	out_layout->addWidget(out_split);

	/*Thread list*/
	m_list_monitor = new QListWidget(this);
	out_split->addWidget(m_list_monitor);
	out_split->setStretchFactor(0, 30);

	/*Register table*/
	m_table_register = new QTableWidget(this);
	out_split->addWidget(m_table_register);
	out_split->setStretchFactor(1, 70);
	/*Single select*/
	m_table_register->setSelectionMode(QAbstractItemView::SingleSelection);
	/*Take off highlight*/
	m_table_register->horizontalHeader()->setHighlightSections(false);
	/*No index 1,2,3,4....*/
	m_table_register->verticalHeader()->setVisible(false);
	/*Set captions*/
	m_table_register->setColumnCount(REG_COLUMM_MAX * NUM_OF_COLUMM);
	QStringList header;
	int iloop;
	for (iloop = 0; iloop < NUM_OF_COLUMM; iloop++) {
		int jloop;
		for (jloop = 0; jloop < REG_COLUMM_MAX; jloop++) {
			header << g_reg_view_column[jloop];
		}
	}
	m_table_register->setHorizontalHeaderLabels(header);
	m_table_register->resizeColumnsToContents();

	m_data_setting = false;

	conn = connect(m_list_monitor, SIGNAL(itemPressed(QListWidgetItem*)),
			       this, SLOT(slotListItemClicked(QListWidgetItem*)));
	UTER_ASSERT(conn != NULL);

	conn = connect(m_table_register, SIGNAL(itemChanged(QTableWidgetItem*)),
			       this, SLOT(slotTableItemTextChanged(QTableWidgetItem*)));
	UTER_ASSERT(conn != NULL);

	g_mainwin->dockSetup(this);
	return;
}

void RegDock::slotListItemClicked(QListWidgetItem *item)
{
	RegMon *mon = (RegMon *)item->data(Qt::UserRole).value<void*>();
	UTER_ASSERT(mon != NULL);
	/*Show thread register on table*/
	loadRegs(mon);
	return;
}

void RegDock::slotTableItemTextChanged(QTableWidgetItem *item)
{
	if (m_data_setting) {
		return;
	}

	RegDataInfo datainfo = item->data(Qt::UserRole).value<RegDataInfo>();
	RegMon *mon = datainfo.m_mon;
	UTER_ASSERT(mon != NULL);

	/*Find register by name*/
	QList<RegMon::Item*>::iterator iter;
	for (iter = mon->m_reg_list.begin(); iter != mon->m_reg_list.end(); iter++) {
		RegMon::Item *regval = *iter;
		if (regval->m_name == datainfo.m_name) {
			break;
		}
	}
	if (iter == mon->m_reg_list.end()) {
		return;
	}
	RegMon::Item *regval = *iter;

	/*Read user input*/
	QString input = item->text();

	int prefix_base;
	if (input.indexOf("0x") == 0 || input.indexOf("0X") == 0) {
		prefix_base = 16;
	} else if (input.indexOf("0") == 0) {
		prefix_base = 8;
	} else {
		prefix_base = 10;
	}

	unsigned long long input_number;
	bool convert_ok;
	input_number = (unsigned long long)input.toULongLong(&convert_ok, prefix_base);
	if (! convert_ok) {
		goto recovery_value;
	}

	int result;
	result = g_target->pidRegSet(mon->m_pid, regval->m_name, input_number);
	if (result < 0) {
		goto recovery_value;
	}

	regval->m_value = input_number;
	regval->m_changed = false;

recovery_value:
	item->setText(g_target->arch0xAddr(regval->m_value));
	if (regval->m_changed) {
		item->setTextColor(Qt::red);
	} else {
		item->setTextColor(Qt::black);
	}
	return;
}

int RegDock::loadRegs(RegMon *mon)
{
	UTER_ASSERT(mon != NULL);

	m_table_register->clearContents();
	m_table_register->setRowCount(0);

	m_data_setting = true;

	int iloop = 0;
	QList<RegMon::Item*>::iterator iter;
	for (iter = mon->m_reg_list.begin(); iter != mon->m_reg_list.end(); iter++, iloop++) {
		RegMon::Item *regitem = *iter;

		int line = iloop / NUM_OF_COLUMM;
		int column = iloop % NUM_OF_COLUMM;
		m_table_register->setRowCount(line + 1);

		/*Register name*/
		QTableWidgetItem *name_item = new QTableWidgetItem(regitem->m_name);
		name_item->setFlags(name_item->flags()&(~Qt::ItemIsEditable));
		m_table_register->setItem(line, (column * REG_COLUMM_MAX) + REG_COLUMM_NAME, name_item);

		/*Register value*/
		QTableWidgetItem *value_item = new QTableWidgetItem(g_target->arch0xAddr(regitem->m_value));
		if (regitem->m_changed) {
			value_item->setTextColor(Qt::red);
		} else {
			value_item->setTextColor(Qt::black);
		}

		RegDataInfo datainfo(mon, regitem->m_name);
		value_item->setData(Qt::UserRole, QVariant::fromValue(datainfo));
		m_table_register->setItem(line, (column * REG_COLUMM_MAX) + REG_COLUMM_VALUE, value_item);
	}

	m_data_setting = false;
	m_table_register->resizeColumnsToContents();
	return 0;
}

int RegDock::updateOnStop()
{
	/*phase 1: clear thread*/
	QMap<unsigned int, RegMon*>::iterator map_iter;
	for (map_iter = m_mon_map.begin(); map_iter != m_mon_map.end(); /*none*/) {
		RegMon *mon = map_iter.value();

		TargetThread *thread = g_target->threadGetById(mon->m_pid);
		if (thread == NULL) {
			/*Thread is not exist*/
			m_list_monitor->removeItemWidget(mon->m_item);
			delete mon;
			m_mon_map.erase(map_iter++);
		} else {
			mon->loadData();	/*Update registers*/
			map_iter++;
		}
	}

	/*phase 2: add thread*/
	QMap<unsigned int, TargetThread*>::iterator thread_iter;
	for (thread_iter = g_target->m_thread_set.begin(); thread_iter != g_target->m_thread_set.end(); thread_iter++) {
		TargetThread *thread = thread_iter.value();

		QMap<unsigned int, RegMon*>::iterator map_iter;
		map_iter = m_mon_map.find(thread->m_pid);
		if (map_iter == m_mon_map.end()) {
			RegMon *new_mon = new RegMon(thread->m_pid, thread->m_name);
			m_list_monitor->addItem(new_mon->m_item);
			m_mon_map.insert(new_mon->m_pid, new_mon);
			new_mon->loadData();		/*Update registers*/
		}
	}
	m_list_monitor->sortItems();

	/*phase 3:Show register for current thread*/
	QListWidgetItem *item = m_list_monitor->currentItem();
	if (item == NULL) {
		m_table_register->clearContents();
		m_table_register->setRowCount(0);
	} else {
		RegMon *mon = (RegMon *)item->data(Qt::UserRole).value<void*>();
		UTER_ASSERT(mon != NULL);
		/*Show thread register on table*/
		loadRegs(mon);
	}

	return 0;
}

int RegDock::updateOnRun()
{
	/*TODO:*/
	return 0;
}

int RegDock::updateOnShut()
{
	clear();
	return 0;
}

int RegDock::clear()
{
	QMap<unsigned int, RegMon*>::iterator iter;
	for (iter = m_mon_map.begin(); iter != m_mon_map.end(); /*none*/) {
		RegMon *regmon = iter.value();
		delete regmon;
		m_mon_map.erase(iter++);
	}
	m_list_monitor->clear();
	m_table_register->clear();
	return 0;
}

} /*namespace RegUi*/

Q_DECLARE_METATYPE(RegUi::RegDataInfo);
