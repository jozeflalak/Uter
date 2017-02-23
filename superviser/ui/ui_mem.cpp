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
#include <ui_mainwin.hxx>

namespace MemUi {

AddrInputDlg::AddrInputDlg(QWidget *parent)
: QDialog(parent)
{
	QMetaObject::Connection conn;

	/* Out layout
	 * -------------------------------------
	 * label
	 * -------------------------------------
	 *         Input text
	 * -------------------------------------
	 *                      | Cancle | Ok  |
	 * -------------------------------------*/
	QGridLayout *out_layout = new QGridLayout(this);
	setLayout(out_layout);

	QLabel *label = new QLabel("Enter address to monitor", this);
	m_input_text = new QLineEdit(this);
	m_ok_btn = new QPushButton("Ok", this);
	m_cancel_btn = new QPushButton("Cancel", this);

	out_layout->addWidget(label, 0, 0, 1, 2);
	out_layout->addWidget(m_input_text, 1, 0, 1, 4);
	out_layout->addWidget(m_cancel_btn, 2, 2, 1, 1);
	out_layout->addWidget(m_ok_btn, 2, 3, 1, 1);

	m_ok = false;
	m_address = 0;

	conn = connect(m_cancel_btn, SIGNAL(pressed()), this, SLOT(reject()));
	UTER_ASSERT(conn != NULL);

	conn = connect(m_ok_btn, SIGNAL(pressed()), this, SLOT(slotOkPressed()));
	UTER_ASSERT(conn != NULL);
	return;
}

int AddrInputDlg::openAddr()
{
	/*Do clear*/
	m_input_text->clear();
	m_input_text->setText(g_target->arch0xAddr(m_address));
	m_input_text->setFocus();

	m_ok = false;

	/*Modal window*/
	exec();
	return 0;
}

void AddrInputDlg::slotOkPressed()
{
	QString input = m_input_text->text();

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
		return;
	}

	m_ok = true;
	m_address = input_number;

	accept();
	return;
}

/*Load memory for block*/
int MemAt::Block::loadData()
{
	int iloop;
	QByteArray array;
	int result = g_target->memoryGet(m_startat, m_number, array);
	if (result < 0) {
		for (iloop = 0; iloop < BYTES_OF_BLK; iloop++) {
			m_content[iloop].m_status = STATUS_FAILED;
		}
		return 0;
	}

	/*Compare bytes*/
	for (iloop = 0; iloop < m_number; iloop++) {
		unsigned char data = (unsigned char)array[iloop];
		m_content[iloop].m_status = 0;
		if (m_content[iloop].m_data != data) {
			m_content[iloop].m_status = STATUS_CHANGE;
		}
		m_content[iloop].m_data = data;
	}

	if (! m_loaded) {
		m_loaded = true;
		/*Clear flag when first load*/
		for (iloop = 0; iloop < BYTES_OF_BLK; iloop++) {
			m_content[iloop].m_status = 0;
		}
	}

	return 0;
}

int MemAt::Block::clearData()
{
	m_loaded = false;
	return 0;
}

MemAt::MemAt(unsigned long long addr)
{
	m_address = addr;
	m_item = new QListWidgetItem(g_target->arch0xAddr(m_address));
	m_item->setData(Qt::UserRole, QVariant::fromValue((void *)this));

	Block *block = new Block();
	block->m_startat = (m_address / MemAt::Block::BYTES_OF_BLK) * MemAt::Block::BYTES_OF_BLK;
	m_blocks.push_back(block);

	return;
}

/*Load memory for Memory at*/
int MemAt::loadData()
{
	QList<Block*>::iterator iter;
	for (iter = m_blocks.begin(); iter != m_blocks.end(); iter++) {
		Block *block = *iter;
		block->loadData();
	}
	return 0;
}

int MemAt::clearData()
{
	QList<Block*>::iterator iter;
	for (iter = m_blocks.begin(); iter != m_blocks.end(); iter++) {
		Block *block = *iter;
		block->clearData();
	}
	return 0;
}

enum MemViewColumn {
	MEM_VIEW_ADDRESS = 0,
	MOD_VIEW_0_3,
	MOD_VIEW_4_7,
	MOD_VIEW_8_B,
	MOD_VIEW_C_F,
	MOD_VIEW_0,
	MOD_VIEW_1,
	MOD_VIEW_2,
	MOD_VIEW_3,
	MOD_VIEW_4,
	MOD_VIEW_5,
	MOD_VIEW_6,
	MOD_VIEW_7,
	MOD_VIEW_8,
	MOD_VIEW_9,
	MOD_VIEW_A,
	MOD_VIEW_B,
	MOD_VIEW_C,
	MOD_VIEW_D,
	MOD_VIEW_E,
	MOD_VIEW_F,
	MOD_VIEW_DECODE,
	MOD_VIEW_MAX,
};

static const char *g_mem_view_column[] = {
	"Address",
	"0-3",
	"4-7",
	"8-B",
	"C-F",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"",
};

UTER_ARRAY_CHECK(g_mem_view_column, MOD_VIEW_MAX);

struct DataInfo {
public:
	enum ItemType {
		TYPE_WORD = 0,
		TYPE_BYTE,
		TYPE_UP,			/*Backward expand (top)*/
		TYPE_DOWN,			/*Forward expand (bottom)*/
		TYPE_ADDRESS,		/*Address (left)*/
		TYPE_DECOCE,		/*Decode (right)*/
		TYPE_MAX
	};
	DataInfo()
	{
		m_type = TYPE_MAX;
		m_block = NULL;
		m_index = 0;
		return;
	}
	DataInfo(ItemType type)
	{
		m_type = type;
		m_block = NULL;
		m_index = 0;
		return;
	}
	ItemType m_type;				/*Item type*/
	MemAt::Block *m_block;			/*In block*/
	unsigned long long m_index;		/*Index of block*/
};

static int setByte(QTableWidgetItem *item, MemAt::Block *block, int index, int len)
{
	UTER_ASSERT(item != NULL);
	UTER_ASSERT(block != NULL);
	UTER_ASSERT(len == 1);

	item->setText(QString::asprintf("0x%02x", block->m_content[index].m_data));
	item->setTextColor(block->m_content[index].m_status == 0 ? Qt::black : Qt::red);
	return 0;
}

static int setWord(QTableWidgetItem *item, MemAt::Block *block, int index, int len)
{
	UTER_ASSERT(item != NULL);
	UTER_ASSERT(block != NULL);
	UTER_ASSERT(len == MemAt::Block::MEM_BYTES);

	unsigned char bytes[MemAt::Block::MEM_BYTES];
	bool some_wrong = false;
	int iloop;
	for (iloop = 0; iloop < MemAt::Block::MEM_BYTES; iloop++) {
		bytes[iloop] = block->m_content[index + iloop].m_data;
		if (block->m_content[index + iloop].m_status) {
			some_wrong = true;
		}
	}
	unsigned long long data = *((unsigned int *)bytes);
	unsigned long long number = g_target->dataToNum(data, MemAt::Block::MEM_BYTES, false);

	item->setText(QString::asprintf("0x%08llx", number));
	item->setTextColor(some_wrong ? Qt::red : Qt::black);
	return 0;
}

static int setDecode(QTableWidgetItem *item, MemAt::Block *block, int index, int len)
{
	UTER_ASSERT(item != NULL);
	UTER_ASSERT(block != NULL);
	UTER_ASSERT(len == MemAt::Block::MEM_BYTES * MemAt::Block::MEM_COLUMNS);

	QString text;
	int iloop;
	for (iloop = index; iloop < index + len; iloop++) {
		unsigned char ch = block->m_content[iloop].m_data;
		text += QString::asprintf("%c ", ch);
	}
	item->setText(text);
	return 0;
}

MemDock::MemDock()
:WidgetWithDock(SEQ_MEM, Qt::BottomDockWidgetArea, "Memory")
{
	QMetaObject::Connection conn;

	/* Out layout
	 * -------------------------------
	 * |   address  |    memory      |
	 * |    list    |      view      |
	 * -------------------------------
	 * */
	QHBoxLayout *out_layout = new QHBoxLayout(this);
	setLayout(out_layout);
	QSplitter *out_split = new QSplitter(Qt::Horizontal, this);
	out_layout->addWidget(out_split);

	/* Left address layout
	 * ----------------
	 *      tool
	 * ----------------
	 *     address
	 *      list
	 * ----------------*/
	QWidget *left_widget = new QWidget(this);
	out_split->addWidget(left_widget);
	out_split->setStretchFactor(0, 20);
	QVBoxLayout *left_layout = new QVBoxLayout(this);
	left_widget->setLayout(left_layout);

	/*Buttons*/
	QHBoxLayout *btn_layout = new QHBoxLayout(this);
	left_layout->addLayout(btn_layout);

	m_btn_add = new QPushButton(QIcon(":/images/com_add.png"), "", this);
	m_btn_add->setToolTip("Add memory monitor");
	m_btn_del = new QPushButton(QIcon(":/images/com_del.png"), "", this);
	m_btn_del->setToolTip("Delete memory monitor");
	m_btn_delall = new QPushButton(QIcon(":/images/com_delall.png"), "", this);
	m_btn_delall->setToolTip("Delete all memory monitor");

	m_btn_byteview = new QRadioButton("Byte view", this);
	m_btn_wordview = new QRadioButton("Word view", this);

	btn_layout->addWidget(m_btn_byteview);
	btn_layout->addWidget(m_btn_wordview);
	btn_layout->addWidget(m_btn_add);
	btn_layout->addWidget(m_btn_del);
	btn_layout->addWidget(m_btn_delall);
	btn_layout->setAlignment(Qt::AlignRight);

	/*address list*/
	m_list_monitor = new QListWidget(this);
	left_layout->addWidget(m_list_monitor);
	m_data_setting = false;

	/* Right memory view layout
	 * --------------------------
	 *        Big table
	 * --------------------------*/
	m_table_memory = new QTableWidget(this);
	out_split->addWidget(m_table_memory);
	out_split->setStretchFactor(1, 80);

	/*No index 1,2,3,4....*/
	m_table_memory->verticalHeader()->setHidden(true);
	m_table_memory->setColumnCount(MOD_VIEW_MAX);
	QStringList header;
	int iloop;
	for (iloop = 0; iloop < MOD_VIEW_MAX; iloop++) {
		header << g_mem_view_column[iloop];
	}
	m_table_memory->setHorizontalHeaderLabels(header);

	m_btn_byteview->setChecked(true);
	slotByteViewPressed();

	conn = connect(m_btn_add, SIGNAL(pressed()), this, SLOT(slotAddPressed()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_btn_del, SIGNAL(pressed()), this, SLOT(slotDelPressed()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_btn_delall, SIGNAL(pressed()), this, SLOT(slotDelAllPressed()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_btn_byteview, SIGNAL(pressed()), this, SLOT(slotByteViewPressed()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_btn_wordview, SIGNAL(pressed()), this, SLOT(slotWordViewPressed()));
	UTER_ASSERT(conn != NULL);

	conn = connect(m_list_monitor, SIGNAL(itemPressed(QListWidgetItem*)),
			       this, SLOT(slotListItemClicked(QListWidgetItem*)));
	UTER_ASSERT(conn != NULL);

	conn = connect(m_table_memory, SIGNAL(itemChanged(QTableWidgetItem*)),
			       this, SLOT(slotTableItemTextChanged(QTableWidgetItem*)));
	UTER_ASSERT(conn != NULL);

	conn = connect(m_table_memory, SIGNAL(itemClicked(QTableWidgetItem*)),
			       this, SLOT(slotTableItemClicked(QTableWidgetItem*)));
	UTER_ASSERT(conn != NULL);

	m_addr_input = new AddrInputDlg(this);

	g_mainwin->dockSetup(this);
	return;
}

void MemDock::slotAddPressed()
{
	m_addr_input->openAddr();
	if (! m_addr_input->m_ok) {
		return;
	}
	unsigned long long addr = m_addr_input->m_address;

	MemAt *new_memat = new MemAt(addr);
	new_memat->loadData();

	m_memat_list.push_back(new_memat);
	m_list_monitor->addItem(new_memat->m_item);

	/*Select new memory at*/
	m_list_monitor->setCurrentItem(new_memat->m_item);
	loadMemView(new_memat);
	return;
}

void MemDock::slotDelPressed()
{
	QListWidgetItem *current_item = m_list_monitor->currentItem();
	if (current_item == NULL) {
		/*No selected memory at*/
		return;
	}
	MemAt *memat = (MemAt *)current_item->data(Qt::UserRole).value<void*>();
	UTER_ASSERT(memat != NULL);

	QList<MemAt*>::iterator iter;
	for (iter = m_memat_list.begin(); iter != m_memat_list.end(); /*none*/) {
		MemAt *mon = *iter;

		if (memat == mon) {
			m_list_monitor->removeItemWidget(mon->m_item);
			delete mon;
			m_memat_list.erase(iter++);
			clearMemView();

			current_item = m_list_monitor->currentItem();
			if (current_item != NULL) {
				/*Select new memory at*/
				MemAt *mod_item = (MemAt *)current_item->data(Qt::UserRole).value<void*>();
				UTER_ASSERT(mod_item != NULL);
				loadMemView(mod_item);
			}
			break;
		} else {
			iter++;
		}
	}

	return;
}

void MemDock::slotDelAllPressed()
{
	clear();
	return;
}

void MemDock::slotByteViewPressed()
{
	int iloop;
	for (iloop = 0; iloop < MOD_VIEW_MAX; iloop++) {
		if (iloop >= MOD_VIEW_0_3 && iloop <= MOD_VIEW_C_F) {
			/*Word view*/
			m_table_memory->hideColumn(iloop);
		}
		if (iloop >= MOD_VIEW_0 && iloop <= MOD_VIEW_F) {
			/*Byte view*/
			m_table_memory->showColumn(iloop);
		}
	}
	resizeMemView();
	return;
}

void MemDock::slotWordViewPressed()
{
	int iloop;
	for (iloop = 0; iloop < MOD_VIEW_MAX; iloop++) {
		if (iloop >= MOD_VIEW_0_3 && iloop <= MOD_VIEW_C_F) {
			/*Word view*/
			m_table_memory->showColumn(iloop);
		}
		if (iloop >= MOD_VIEW_0 && iloop <= MOD_VIEW_F) {
			/*Byte view*/
			m_table_memory->hideColumn(iloop);
		}
	}
	resizeMemView();
	return;
}

void MemDock::slotListItemClicked(QListWidgetItem *item)
{
	MemAt *mon = (MemAt *)item->data(Qt::UserRole).value<void*>();
	UTER_ASSERT(mon != NULL);
	loadMemView(mon);
	return;
}

void MemDock::slotTableItemClicked(QTableWidgetItem * item)
{
	Arch *arch = g_target->archInfoGet();
	if (arch == NULL) {
		return;
	}
	/*Get memory at*/
	QListWidgetItem *list_item = m_list_monitor->currentItem();
	if (list_item == NULL) {
		return;
	}
	MemAt *memat = (MemAt *)list_item->data(Qt::UserRole).value<void*>();

	DataInfo datainfo = item->data(Qt::UserRole).value<DataInfo>();
	if (datainfo.m_type == DataInfo::TYPE_UP) {
		/*Backward expand*/
		MemAt::Block *first = memat->m_blocks.front();
		if (first->m_startat == 0) {
			return;
		}
		MemAt::Block *new_block = new MemAt::Block();
		new_block->m_startat = first->m_startat - new_block->m_number;
		new_block->loadData();
		memat->m_blocks.push_front(new_block);

		loadMemView(memat);
		return;
	}
	if (datainfo.m_type == DataInfo::TYPE_DOWN) {
		/*Forward expand*/
		MemAt::Block *last = memat->m_blocks.back();
		if (last->m_startat + last->m_number == arch->m_max_addr) {
			return;
		}
		MemAt::Block *new_block = new MemAt::Block();
		new_block->m_startat = last->m_startat + last->m_number;
		new_block->loadData();
		memat->m_blocks.push_back(new_block);

		loadMemView(memat);
		return;
	}

	return;
}

void MemDock::slotTableItemTextChanged(QTableWidgetItem *item)
{
	QByteArray array;
	if (m_data_setting) {
		return;
	}

	DataInfo datainfo = item->data(Qt::UserRole).value<DataInfo>();
	MemAt::Block *block = datainfo.m_block;

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
		goto view_byte;
	}

	int size;
	if (datainfo.m_type == DataInfo::TYPE_WORD) {
		size = MemAt::Block::MEM_BYTES;
	} else if (datainfo.m_type == DataInfo::TYPE_BYTE) {
		size = 1;
	} else {
		goto view_byte;
	}

	g_target->dataToBytes(input_number, size, array);
	int result;
	result = g_target->memorySet(block->m_startat + datainfo.m_index, size, array);
	if (result < 0) {
		goto view_byte;
	}

	int iloop;
	for (iloop = 0; iloop < size; iloop++) {
		block->m_content[datainfo.m_index + iloop].m_status = 0;
		block->m_content[datainfo.m_index + iloop].m_data = array[iloop];
	}

view_byte:
	m_data_setting = true;
	setRow(item->row(), block,
		   datainfo.m_index / MemAt::Block::BYTES_OF_ROW * MemAt::Block::BYTES_OF_ROW,
		   MemAt::Block::BYTES_OF_ROW);
	m_data_setting = false;
	return;
}

int MemDock::updateOnStop()
{
	QList<MemAt*>::iterator iter;
	for (iter = m_memat_list.begin(); iter != m_memat_list.end(); iter++) {
		MemAt *mon = *iter;
		mon->loadData();
	}

	QListWidgetItem *list_item = m_list_monitor->currentItem();
	if (list_item == NULL) {
		/*No selected*/
		clearMemView();
	} else {
		MemAt *mod_item = (MemAt *)list_item->data(Qt::UserRole).value<void*>();
		loadMemView(mod_item);
	}

	return 0;
}

int MemDock::updateOnRun()
{
	return 0;
}

int MemDock::updateOnShut()
{
	/*Clear all block*/
	QList<MemAt*>::iterator memat_iter;
	for (memat_iter = m_memat_list.begin(); memat_iter != m_memat_list.end(); memat_iter++) {
		MemAt *mon = *memat_iter;
		mon->clearData();
	}

	m_list_monitor->setCurrentItem(NULL);
	clearMemView();
	return 0;
}

int MemDock::setRow(int row_num, MemAt::Block *block, int index, int len)
{
	int iloop;
	for (iloop = index; iloop < index + len; iloop++) {
		if (iloop % MemAt::Block::BYTES_OF_ROW == 0) {
			/*Set row start*/
			DataInfo datainfo(DataInfo::TYPE_ADDRESS);
			QTableWidgetItem *item = new QTableWidgetItem();
			item->setText(g_target->arch0xAddr(block->m_startat + iloop));
			item->setFlags(item->flags()&(~Qt::ItemIsEditable));
			item->setData(Qt::UserRole, QVariant::fromValue(datainfo));
			m_table_memory->setItem(row_num, MEM_VIEW_ADDRESS, item);
		}
		if (iloop % MemAt::Block::BYTES_OF_ROW == 0) {
			/*Set row end*/
			DataInfo datainfo(DataInfo::TYPE_DECOCE);
			QTableWidgetItem *item = new QTableWidgetItem();
			setDecode(item, block, iloop, MemAt::Block::BYTES_OF_ROW);
			item->setFlags(item->flags()&(~Qt::ItemIsEditable));
			item->setData(Qt::UserRole, QVariant::fromValue(datainfo));
			m_table_memory->setItem(row_num, MOD_VIEW_DECODE, item);
		}

		if (iloop % MemAt::Block::MEM_BYTES == 0) {
			/*Setup word per MEM_BYTES bytes*/
			DataInfo datainfo(DataInfo::TYPE_WORD);
			datainfo.m_block = block;
			datainfo.m_index = iloop;
			QTableWidgetItem *item = new QTableWidgetItem();
			setWord(item, block, iloop, MemAt::Block::MEM_BYTES);
			item->setData(Qt::UserRole, QVariant::fromValue(datainfo));
			int column = ((iloop / MemAt::Block::MEM_BYTES) % MemAt::Block::MEM_COLUMNS) + MOD_VIEW_0_3;
			m_table_memory->setItem(row_num, column, item);
		}

		/*Setup byte*/
		DataInfo datainfo(DataInfo::TYPE_BYTE);
		datainfo.m_block = block;
		datainfo.m_index = iloop;
		QTableWidgetItem *item = new QTableWidgetItem();
		setByte(item, block, iloop, 1);
		item->setData(Qt::UserRole, QVariant::fromValue(datainfo));
		int column = iloop % (MemAt::Block::MEM_BYTES * MemAt::Block::MEM_COLUMNS) + MOD_VIEW_0;
		m_table_memory->setItem(row_num, column, item);
	}

	return 0;
}

int MemDock::loadMemView(MemAt *mon)
{
	UTER_ASSERT(mon != NULL);
	clearMemView();

	int iloop;

	m_data_setting = true;
	int row_num = 0;				/*Line number*/

	/*Backward expand line*/
	m_table_memory->setRowCount(row_num + 1);
	for (iloop = 0; iloop < MOD_VIEW_MAX; iloop++) {
		QString text;
		if (iloop >= MOD_VIEW_0_3 && iloop <= MOD_VIEW_C_F) {
			text = "......";
		}
		if (iloop >= MOD_VIEW_0 && iloop <= MOD_VIEW_F) {
			text = "...";
		}

		DataInfo datainfo(DataInfo::TYPE_UP);
		QTableWidgetItem *item = new QTableWidgetItem(text);
		item->setToolTip("Click more");
		item->setFlags(item->flags()&(~Qt::ItemIsEditable));
		item->setData(Qt::UserRole, QVariant::fromValue(datainfo));
		m_table_memory->setItem(row_num, iloop, item);
	}

	/*Memory view*/
	QList<MemAt::Block*>::iterator iter;
	for (iter = mon->m_blocks.begin(); iter != mon->m_blocks.end(); iter++) {
		MemAt::Block *block = *iter;
		int row_count = (block->m_number - 1) / MemAt::Block::BYTES_OF_ROW + 1;
		for (iloop = 0; iloop < row_count; iloop++) {
			row_num++;
			m_table_memory->setRowCount(row_num + 1);
			/*Set row with block*/
			setRow(row_num, block, iloop * MemAt::Block::BYTES_OF_ROW, MemAt::Block::BYTES_OF_ROW);
		}
	}

	/*Forward expand*/
	row_num++;
	m_table_memory->setRowCount(row_num + 1);
	for (iloop = 0; iloop < MOD_VIEW_MAX; iloop++) {
		QString text;
		if (iloop >= MOD_VIEW_0_3 && iloop <= MOD_VIEW_C_F) {
			text = "......";
		}
		if (iloop >= MOD_VIEW_0 && iloop <= MOD_VIEW_F) {
			text = "...";
		}

		DataInfo datainfo(DataInfo::TYPE_DOWN);
		QTableWidgetItem *item = new QTableWidgetItem(text);
		item->setToolTip("Click more");
		item->setFlags(item->flags()&(~Qt::ItemIsEditable));
		item->setData(Qt::UserRole, QVariant::fromValue(datainfo));
		m_table_memory->setItem(row_num, iloop, item);
	}
	m_data_setting = false;

	resizeMemView();
	return 0;
}

int MemDock::resizeMemView()
{
	m_table_memory->resizeColumnsToContents();
	int iloop;
	for (iloop = 0; iloop < MOD_VIEW_MAX; iloop++) {
		if (iloop >= MOD_VIEW_0 && iloop <= MOD_VIEW_F) {
			m_table_memory->setColumnWidth(iloop, 50);
		}
	}
	return 0;
}

int MemDock::clearMemView()
{
	m_table_memory->clearContents();
	m_table_memory->setRowCount(0);
	return 0;
}

int MemDock::clear()
{
	QList<MemAt*>::iterator iter;
	for (iter = m_memat_list.begin(); iter != m_memat_list.end(); /*none*/) {
		MemAt *mon = *iter;
		m_list_monitor->removeItemWidget(mon->m_item);
		delete mon;
		m_memat_list.erase(iter++);
	}

	clearMemView();
	return 0;
}

} /*namespace MemUi*/

Q_DECLARE_METATYPE(MemUi::DataInfo);
