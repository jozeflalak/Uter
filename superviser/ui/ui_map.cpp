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

namespace MapUi {

enum MemMapColumn {
	MEMMAP_COLUMM_BEGIN = 0,
	MEMMAP_COLUMM_END,
	MEMMAP_COLUMM_PROPERTY,
	MEMMAP_COLUMM_OFFSET,
	MEMMAP_COLUMM_DEVICE,
	MEMMAP_COLUMM_INODE,
	MEMMAP_COLUMM_PATH,
	MEMMAP_COLUMM_MAX,
};

static const char *g_memmap_view_column[] = {
	"Begin",
	"End",
	"Property",
	"Offset",
	"Device",
	"Inode",
	"Path",
};

UTER_ARRAY_CHECK(g_memmap_view_column, MEMMAP_COLUMM_MAX);

MapDock::MapDock()
:WidgetWithDock(SEQ_MAP, Qt::BottomDockWidgetArea, "Map")
{
	QMetaObject::Connection conn;

	QVBoxLayout *out_layout = new QVBoxLayout(this);
	setLayout(out_layout);

	/*Only one table*/
	m_table = new QTableWidget(this);
	out_layout->addWidget(m_table);

	/*Cannot edit*/
	m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	/*Line select*/
	m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	/*Single select*/
	m_table->setSelectionMode(QAbstractItemView::SingleSelection);
	/*Close caption highlight*/
	m_table->horizontalHeader()->setHighlightSections(false);
	/*No index 1,2,3,4....*/
	m_table->verticalHeader()->setVisible(false);

	/*Set caption*/
	m_table->setColumnCount(MEMMAP_COLUMM_MAX);
	QStringList header;
	int iloop;
	for (iloop = 0; iloop < MEMMAP_COLUMM_MAX; iloop++) {
		header << g_memmap_view_column[iloop];
	}
	m_table->setHorizontalHeaderLabels(header);
	m_table->resizeColumnsToContents();

	conn = connect(m_table, SIGNAL(cellDoubleClicked(int, int)),
			          this, SLOT(slotCellDoubleClicked(int, int)));
	UTER_ASSERT(conn != NULL);

	g_mainwin->dockSetup(this);
	return;
}

void MapDock::slotCellDoubleClicked(int row, int column)
{
	/*Get path from item*/
	QTableWidgetItem *item = m_table->item(row, MEMMAP_COLUMM_PATH);
	if (item == NULL) {
		return;
	}
	QString path = item->text();

	/*Call module*/
	g_mainwin->m_mod_win->activeByPath(path);
	return;
}

int MapDock::updateOnStop()
{
	clear();

	int iloop = 0;
	QList<TargetMap*>::iterator iter;
	for (iter = g_target->m_map_items.begin(); iter != g_target->m_map_items.end(); iter++, iloop++) {
		TargetMap *map = *iter;

		m_table->setRowCount(iloop + 1);

		/*Start & End*/
		m_table->setItem(iloop, MEMMAP_COLUMM_BEGIN, new QTableWidgetItem("[" + g_target->arch0xAddr(map->m_start)));
		m_table->setItem(iloop, MEMMAP_COLUMM_END, new QTableWidgetItem(g_target->arch0xAddr(map->m_end) + ")"));
		/*Property*/
		m_table->setItem(iloop, MEMMAP_COLUMM_PROPERTY, new QTableWidgetItem(QString::asprintf("%s%s%s%s",
				map->m_flag_r?"r":"-", map->m_flag_w?"w":"-", map->m_flag_x?"x":"-", map->m_flag_p?"p":"-")));
		/*Offset*/
		m_table->setItem(iloop, MEMMAP_COLUMM_OFFSET, new QTableWidgetItem(g_target->arch0xAddr(map->m_offset)));
		/*Device*/
		m_table->setItem(iloop, MEMMAP_COLUMM_DEVICE, new QTableWidgetItem(QString::asprintf("%d:%d", map->m_major, map->m_minor)));
		/*Inode*/
		m_table->setItem(iloop, MEMMAP_COLUMM_INODE, new QTableWidgetItem(QString::asprintf("%u", map->m_inode)));
		/*Path*/
		m_table->setItem(iloop, MEMMAP_COLUMM_PATH, new QTableWidgetItem(map->m_path));
	}
	m_table->resizeColumnsToContents();
	return 0;
}

int MapDock::updateOnShut()
{
	clear();
	return 0;
}

int MapDock::clear()
{
	m_table->clearContents();
	m_table->setRowCount(0);
	return 0;
}

} /*namespace MapUi*/
