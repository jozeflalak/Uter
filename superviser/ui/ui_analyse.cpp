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

namespace AnalyseUi {

class ObjTreeItem: public QTreeWidgetItem
{
public:
	enum ObjType {
		OBJTYPE_ROOT = 0,
		OBJTYPE_MOD,
		OBJTYPE_CU,
		OBJTYPE_FILE,
	};

	ObjTreeItem(ObjType type)
	{
		m_type = type;
		setCheckState(0, Qt::Checked);
		switch(m_type)
		{
		case OBJTYPE_MOD:
		{
			setIcon(0, QIcon(":/images/elmt_mod.png"));
			break;
		}
		case OBJTYPE_CU:
		{
			setIcon(0, QIcon(":/images/elmt_cu.png"));
			break;
		}
		case OBJTYPE_FILE:
		{
			setIcon(0, QIcon(":/images/elmt_file.png"));
			break;
		}
		}
		return;
	}
	ObjType m_type;
	QString m_name;		/*Some name, module name, cu name ....*/
};

enum TypeColumn {
	TYPE_COLUMN_DESC = 0,
	TYPE_COLUMN_SIZE,
	TYPE_COLUMN_ALIGN,
	TYPE_COLUMN_FILELINE,
	TYPE_COLUMM_MAX,
};

static const char *g_type_column[] = {
	"Define",
	"Size",
	"Alignment",
	"File&Line",
};

UTER_ARRAY_CHECK(g_type_column, TYPE_COLUMM_MAX);

enum TypeDetailColumn {
	TYPE_DETAIL_MEMBER = 0,
	TYPE_DETAIL_SIZE,
	TYPE_DETAIL_ALIGN,
	TYPE_DETAIL_OFFSET,
	TYPE_DETAIL_OFFSET_ON_PARENT,
	TYPE_DETAIL_MAX,
};

static const char *g_type_detail_column[] = {
	"Member",
	"Size",
	"Alignment",
	"Offset",
	"Offset on parent",
};

UTER_ARRAY_CHECK(g_type_detail_column, TYPE_DETAIL_MAX);

enum FunColumn {
	FUN_COLUMN_DESC = 0,
	FUN_COLUMN_ADDR,
	FUN_COLUMN_LEN,
	FUN_COLUMN_STACKSIZE,
	FUN_COLUMN_FILELINE,
	FUN_COLUMM_MAX,
};

static const char *g_fun_column[] = {
	"Define",
	"Address",
	"Length",
	"Stack size",
	"File&Line",
};

UTER_ARRAY_CHECK(g_fun_column, FUN_COLUMM_MAX);

struct ListDataInfo {
public:
	ListDataInfo()
	{
		m_mod = "";
		m_cu = "";
		m_offset = 0;
		return;
	}
	ListDataInfo(QString mod, QString cu, unsigned long long offset)
	{
		m_mod = mod;
		m_cu = cu;
		m_offset = offset;
		return;
	}
	QString m_mod;
	QString m_cu;
	unsigned long long m_offset;
};

AnalyseDlg::AnalyseDlg(QWidget *parent)
:QDialog(parent)
{
	QMetaObject::Connection conn;
	QStringList heads;
	int iloop;
	QLabel *label;

	setWindowTitle("Uter program analyse");

	/* Out layout
	 * ------------------------------
	 *   Object  |      Object
	 *    Tree   |       Tab
	 *           |      View
	 * ------------------------------*/
	QVBoxLayout *out_layout = new QVBoxLayout(this);
	setLayout(out_layout);
	QSplitter *top_split = new QSplitter(Qt::Horizontal, this);
	out_layout->addWidget(top_split);

	QWidget *left_widget = new QWidget(this);
	top_split->addWidget(left_widget);
	top_split->setStretchFactor(0, 30);

	QTabWidget *right_tab = new QTabWidget(this);
	top_split->addWidget(right_tab);
	top_split->setStretchFactor(1, 70);

	/* Left layout
	 * ------------------------
	 *      Tools
	 * ------------------------
	 *      Object tree
	 * ------------------------*/
	QVBoxLayout *left_layout = new QVBoxLayout(this);
	left_widget->setLayout(left_layout);

	QHBoxLayout *left_tool_layout = new QHBoxLayout(this);
	left_layout->addLayout(left_tool_layout);
	left_tool_layout->setAlignment(Qt::AlignRight);
	QPushButton *reset_btn = new QPushButton("Reset object tree", this);
	left_tool_layout->addWidget(reset_btn);

	m_mod_cu_tree = new QTreeWidget(this);
	left_layout->addWidget(m_mod_cu_tree);
	m_mod_cu_tree->setHeaderHidden(true);
	m_mod_cu_tree_changing = false;

	/* Rigth layout
	 * ----------------------------
	 * | Type  |  Function  |
	 * ----------------------------
	 *
	 * ----------------------------*/
	QSplitter *type_split = new QSplitter(Qt::Horizontal, this);
	right_tab->addTab(type_split, "Type");

	/* Type layout
	 * --------------------------------
	 *    Type     |      Detail
	 *    list     |-------------------
	 *             |      Source
	 * ---------------------------------*/
	QWidget *type_left_widget = new QWidget(this);
	type_split->addWidget(type_left_widget);
	QVBoxLayout *type_left_layout = new QVBoxLayout(this);
	type_left_widget->setLayout(type_left_layout);

	m_type_table = new QTableWidget(this);
	type_left_layout->addWidget(m_type_table);

	/*Cannot edit*/
	m_type_table->setEditTriggers(QTableWidget::NoEditTriggers);
	/*Select line*/
	m_type_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	/*Single select*/
	m_type_table->setSelectionMode(QAbstractItemView::SingleSelection);
	/*Close caption highlight*/
	m_type_table->horizontalHeader()->setHighlightSections(false);
	/*No index 1,2,3,4....*/
	m_type_table->verticalHeader()->setHidden(true);

	/*Set caption*/
	m_type_table->setColumnCount(TYPE_COLUMM_MAX);
	heads.clear();
	for (iloop = 0; iloop < TYPE_COLUMM_MAX; iloop++) {
		heads << g_type_column[iloop];
	}
	m_type_table->setHorizontalHeaderLabels(heads);
	m_type_table->resizeColumnsToContents();

	/*Right side*/
	QSplitter *type_right_split = new QSplitter(Qt::Vertical, this);
	type_split->addWidget(type_right_split);

	QWidget *type_right_up_widget = new QWidget(this);
	type_right_split->addWidget(type_right_up_widget);
	type_right_split->setStretchFactor(0, 55);

	QWidget *type_right_down_widget = new QWidget(this);
	type_right_split->addWidget(type_right_down_widget);
	type_right_split->setStretchFactor(0, 45);

	/*Right up*/
	QVBoxLayout *type_right_up_layout = new QVBoxLayout(this);
	type_right_up_widget->setLayout(type_right_up_layout);
	label = new QLabel("Detail:", this);
	type_right_up_layout->addWidget(label);

	m_type_detail_tree = new QTreeWidget(this);
	type_right_up_layout->addWidget(m_type_detail_tree);

	m_type_detail_tree->setColumnCount(TYPE_DETAIL_MAX);
	heads.clear();
	for (iloop = 0; iloop < TYPE_DETAIL_MAX; iloop++) {
		heads << g_type_detail_column[iloop];
	}
	m_type_detail_tree->setHeaderLabels(heads);

	/*Right down*/
	QVBoxLayout *type_right_down_layout = new QVBoxLayout(this);
	type_right_down_widget->setLayout(type_right_down_layout);
	label = new QLabel("Source:", this);
	type_right_down_layout->addWidget(label);

	m_type_src_edit = new EditorBase(EditorBase::FILETYPE_CPP, this);
	type_right_down_layout->addWidget(m_type_src_edit);
	m_type_src_edit->setReadOnly(true);

	/* Function layout
	 * ------------------------------
	 *    a table
	 * ------------------------------*/
	QSplitter *fun_split = new QSplitter(Qt::Horizontal, this);
	right_tab->addTab(fun_split, "Function");

	QWidget *fun_left_widget = new QWidget(this);
	fun_split->addWidget(fun_left_widget);
	QVBoxLayout *fun_left_layout = new QVBoxLayout(this);
	fun_left_widget->setLayout(fun_left_layout);

	m_fun_table = new QTableWidget(this);
	fun_left_layout->addWidget(m_fun_table);

	/*Can not edit*/
	m_fun_table->setEditTriggers(QTableWidget::NoEditTriggers);
	/*Select line*/
	m_fun_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	/*Single select*/
	m_fun_table->setSelectionMode(QAbstractItemView::SingleSelection);
	/*Close caption highlight*/
	m_fun_table->horizontalHeader()->setHighlightSections(false);
	/*No index 1,2,3,4....*/
	m_fun_table->verticalHeader()->setHidden(true);

	/*Set caption*/
	m_fun_table->setColumnCount(FUN_COLUMM_MAX);
	heads.clear();
	for (iloop = 0; iloop < FUN_COLUMM_MAX; iloop++) {
		heads << g_fun_column[iloop];
	}
	m_fun_table->setHorizontalHeaderLabels(heads);
	m_fun_table->resizeColumnsToContents();

    conn = connect(reset_btn, SIGNAL(pressed()), this, SLOT(slotResetPressed()));
    UTER_ASSERT(conn != NULL);

    conn = connect(m_mod_cu_tree, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(slotObjTreeItemChanged(QTreeWidgetItem*, int)));
    UTER_ASSERT(conn != NULL);
    conn = connect(m_type_table, SIGNAL(cellClicked(int, int)), this, SLOT(slotTypeListCellClicked(int, int)));
    UTER_ASSERT(conn != NULL);

	/*Menu & toolbar*/
	UiMenuSetup *menu = g_mainwin->menuSetup(UiMenuSetup::SEQ_TOOL_ANALYSE, "&Tools");
	UiToolBarSetup *toolbar = g_mainwin->toolBarSetup(UiToolBarSetup::SEQ_TOOL_ANALYSE, "&Tools");

	QAction *open_act = new QAction("&Program analyse", this);
	open_act->setStatusTip("Open analyse windows");
	open_act->setIcon(QIcon(":/images/analyse.png"));
	menu->addAction(open_act);
	toolbar->addAction(open_act);

    conn = connect(open_act, SIGNAL(triggered()), this, SLOT(slotActionTriggered()));
    UTER_ASSERT(conn != NULL);
	return;
}

void AnalyseDlg::slotActionTriggered()
{
	modCuTreeLoad();
	setWindowState(Qt::WindowMaximized);
	exec();		/*Modal window*/
	return;
}

void AnalyseDlg::slotResetPressed()
{
	modCuTreeLoad();
	return;
}

static void flush_child(QTreeWidgetItem *item, Qt::CheckState checkstat)
{
	int count = item->childCount();
	int iloop;
	for (iloop = 0; iloop < count; iloop++) {
		QTreeWidgetItem *child = item->child(iloop);
		/*Set state for self*/
		child->setCheckState(0, checkstat);
		/*Set state for child*/
		flush_child(child, checkstat);
	}
	return;
}

static void flush_parent(QTreeWidgetItem *item)
{
	/*Get parent*/
	QTreeWidgetItem *parent = item->parent();
	if (parent == NULL) {
		return;
	}
	int count = parent->childCount();

	unsigned int check = 0;
	unsigned int uncheck = 0;
	unsigned int partcheck = 0;
	int iloop;
	for (iloop = 0; iloop < count; iloop++) {
		QTreeWidgetItem *child = parent->child(iloop);
		Qt::CheckState checkstat = child->checkState(0);
		if (checkstat == Qt::Checked) {
			check++;
		} else if (checkstat == Qt::Unchecked) {
			uncheck++;
		} else {
			partcheck++;
		}
	}
	/*Set state for parent*/
	if (partcheck != 0 || (check != 0 && uncheck != 0)) {
		parent->setCheckState(0, Qt::PartiallyChecked);
	} else if (check == 0) {
		parent->setCheckState(0, Qt::Unchecked);
	} else {
		parent->setCheckState(0, Qt::Checked);
	}
	/*Set state for parent`s parent*/
	flush_parent(parent);
	return;
}

void AnalyseDlg::slotObjTreeItemChanged(QTreeWidgetItem *item, int column)
{
	if (m_mod_cu_tree_changing) {
		return;
	}

	m_mod_cu_tree_changing = true;

	/*Qt::Unchecked or Qt::Checked */
	Qt::CheckState checkstat = item->checkState(0);

	/*Set state for self*/
	item->setCheckState(0, checkstat);
	/*Set state for child*/
	flush_child(item, checkstat);
	/*Set state for parent*/
	flush_parent(item);

	m_mod_cu_tree_changing = false;
	objLoadFromCu();
	return;
}

int AnalyseDlg::modCuTreeLoad()
{
	modCuTreeClear();

	m_mod_cu_tree_changing = true;

	/*Get source file*/
	QSet<QString> prjfiles = g_mainwin->m_prj_win->prjFileGet();

	ObjTreeItem *root_item = new ObjTreeItem(ObjTreeItem::OBJTYPE_ROOT);
	root_item->setText(0, "tree:module/compilation unit/file");
	m_mod_cu_tree->addTopLevelItem(root_item);

	unsigned int mod_check = 0;
	unsigned int mod_uncheck = 0;
	unsigned int mod_partcheck = 0;
	QMap<QString, TargetMod*>::iterator mod_iter;
	for (mod_iter = g_target->m_mod_items.begin();
		 mod_iter != g_target->m_mod_items.end(); mod_iter++) {
		TargetMod *mod = mod_iter.value();

		ObjTreeItem *mod_item = new ObjTreeItem(ObjTreeItem::OBJTYPE_MOD);
		mod_item->setText(0, mod->m_remote_path);
		mod_item->setToolTip(0, mod->m_remote_path);
		mod_item->m_name = mod->m_remote_path;
		root_item->addChild(mod_item);

		unsigned int cu_check = 0;
		unsigned int cu_uncheck = 0;
		unsigned int cu_partcheck = 0;
		QMap<QString, DwfCu*>::iterator cu_iter;
		for (cu_iter = mod->m_dwarf_cu_map.begin();
			 cu_iter != mod->m_dwarf_cu_map.end(); cu_iter++) {
			DwfCu *cu = cu_iter.value();

			ObjTreeItem *cu_item = new ObjTreeItem(ObjTreeItem::OBJTYPE_CU);
			cu_item->setText(0, cu->m_path);
			cu_item->setToolTip(0, cu->m_path);
			cu_item->m_name = cu->m_path;
			mod_item->addChild(cu_item);

			unsigned int file_check = 0;
			unsigned int file_uncheck = 0;
			QMap<QString, unsigned int>::iterator file_iter;
			for (file_iter = cu->m_file_path_map.begin();
				 file_iter != cu->m_file_path_map.end(); file_iter++) {
				QString path = file_iter.key();

				ObjTreeItem *file_item = new ObjTreeItem(ObjTreeItem::OBJTYPE_FILE);
				file_item->setText(0, path);
				file_item->setToolTip(0, path);
				file_item->m_name = path;
				cu_item->addChild(file_item);

				m_mod_cu_tree->expandItem(file_item);

				/*Check file in project*/
				QSet<QString>::iterator prjfile_iter;
				prjfile_iter = prjfiles.find(path);
				if (prjfile_iter == prjfiles.end()) {
					file_item->setCheckState(0, Qt::Unchecked);
					file_uncheck++;
				} else {
					file_item->setCheckState(0, Qt::Checked);
					file_check++;
				}
			}
			/*Set cu check status*/
			if (file_check != 0 && file_uncheck != 0) {
				cu_item->setCheckState(0, Qt::PartiallyChecked);
				cu_partcheck++;
				m_mod_cu_tree->expandItem(cu_item);
			} else if (file_check == 0) {
				cu_item->setCheckState(0, Qt::Unchecked);
				cu_uncheck++;
			} else {
				cu_item->setCheckState(0, Qt::Checked);
				cu_check++;
				m_mod_cu_tree->expandItem(cu_item);
			}
		}

		/*Set mod check status*/
		if (cu_partcheck != 0 || (cu_check != 0 && cu_uncheck != 0)) {
			mod_item->setCheckState(0, Qt::PartiallyChecked);
			mod_partcheck++;
			m_mod_cu_tree->expandItem(mod_item);
		} else if (cu_check == 0) {
			mod_item->setCheckState(0, Qt::Unchecked);
			mod_uncheck++;
		} else {
			mod_item->setCheckState(0, Qt::Checked);
			mod_check++;
			m_mod_cu_tree->expandItem(mod_item);
		}
	}

	/*Set root check status*/
	if (mod_partcheck != 0 || (mod_check != 0 && mod_uncheck != 0)) {
		root_item->setCheckState(0, Qt::PartiallyChecked);
		m_mod_cu_tree->expandItem(root_item);
	} else if (mod_check == 0) {
		root_item->setCheckState(0, Qt::Unchecked);
	} else {
		root_item->setCheckState(0, Qt::Checked);
		m_mod_cu_tree->expandItem(root_item);
	}

	m_mod_cu_tree_changing = false;

	objLoadFromCu();
	return 0;
}

int AnalyseDlg::modCuTreeClear()
{
	m_mod_cu_tree_changing = true;
	m_mod_cu_tree->clear();
	m_mod_cu_tree_changing = false;
	return 0;
}

int AnalyseDlg::objLoadFromCu()
{
	objLoadClear();

	if (m_mod_cu_tree->topLevelItemCount() != 1) {
		return 0;
	}
	QTreeWidgetItem *rootitem = m_mod_cu_tree->topLevelItem(0);
	if (rootitem == NULL) {
		return 0;
	}

	int mod_count = rootitem->childCount();
	int iloop;
	for (iloop = 0; iloop < mod_count; iloop++) {
		ObjTreeItem *mod_item = (ObjTreeItem *)rootitem->child(iloop);
		if (mod_item->checkState(0) == Qt::Unchecked) {
			continue;
		}
		TargetMod *mod = g_target->modGetByPath(mod_item->m_name);
		if (mod == NULL) {
			continue;
		}

		int cu_count = mod_item->childCount();
		int jloop;
		for (jloop = 0; jloop < cu_count; jloop++) {
			ObjTreeItem *cu_item = (ObjTreeItem *)mod_item->child(jloop);
			if (cu_item->checkState(0) == Qt::Unchecked) {
				continue;
			}
			DwfCu *cu = mod->dwarfGetCuByPath(cu_item->m_name);
			if (cu == NULL) {
				continue;
			}

			/*Match files*/
			QSet<unsigned int> match_files;
			QSet<unsigned int>::iterator match_iter;

			int file_count = cu_item->childCount();
			int kloop;
			for (kloop = 0; kloop < file_count; kloop++) {
				ObjTreeItem *file_item = (ObjTreeItem *)cu_item->child(kloop);
				if (file_item->checkState(0) == Qt::Unchecked) {
					continue;
				}

				QMap<QString, unsigned int>::iterator dwf_file_iter;
				dwf_file_iter = cu->m_file_path_map.find(file_item->m_name);
				if (dwf_file_iter == cu->m_file_path_map.end()) {
					/*Some wrong*/
					continue;
				}

				match_files.insert(dwf_file_iter.value());
			}

			/*Load data type and function*/
			objTypeLoadOnCu(mod, cu, match_files);
			objFunLoadOnCu(mod, cu, match_files);
		}
	}

	return 0;
}

int AnalyseDlg::objLoadClear()
{
	objTypeClear();
	objFunClear();
	return 0;
}

/*Make tree item in data type datail*/
QTreeWidgetItem *AnalyseDlg::objTypeMake(DwfType *type, QString name, unsigned long long global_off)
{
	DwfType *final_type = type->finalType();

	QString type_before;
	QString type_after;
	DwfType::typeDesc(type, type_before, type_after);
	QString type_desc;
	if (type_before.right(0) == "*") {
		type_desc = type_before + name + type_after;
	} else {
		type_desc = type_before + " " + name + type_after;
	}

	/*Data type item*/
	QTreeWidgetItem *ret_item = new QTreeWidgetItem();
	ret_item->setText(TYPE_DETAIL_MEMBER, type_desc);
	ret_item->setText(TYPE_DETAIL_SIZE, QString::asprintf("%u", final_type->dataSize()));
	ret_item->setText(TYPE_DETAIL_ALIGN, QString::asprintf("%u", final_type->dataAlign()));
	ret_item->setText(TYPE_DETAIL_OFFSET, g_target->arch0xAddr(global_off));
	ret_item->setText(TYPE_DETAIL_OFFSET_ON_PARENT, g_target->arch0xAddr(0));	/*上级节点会修改的*/

	/*Child data type items*/
	switch(final_type->m_data_type)
	{
	case DwfType::TYPE_ST:
	{
		unsigned long long last_member_stopat = 0;
		DwfTypeSt *type_st = (DwfTypeSt *)final_type;
		QList<DwfTypeStMem*>::iterator iter;
		for (iter = type_st->m_member_list.begin(); iter != type_st->m_member_list.end(); iter++) {
			DwfTypeStMem *st_mem = *iter;
			DwfType *mem_type = st_mem->m_type;
			if (st_mem->m_member_location > last_member_stopat) {
				/*Alignment space*/
				QTreeWidgetItem *space_item = new QTreeWidgetItem();
				space_item->setText(TYPE_DETAIL_MEMBER, "/*Alignment space*/");
				space_item->setText(TYPE_DETAIL_SIZE, QString::asprintf("%llu", st_mem->m_member_location - last_member_stopat));
				space_item->setText(TYPE_DETAIL_ALIGN, "--");
				space_item->setText(TYPE_DETAIL_OFFSET, "--");
				space_item->setText(TYPE_DETAIL_OFFSET_ON_PARENT, "--");
				ret_item->addChild(space_item);
			}
			/*Child*/
			QTreeWidgetItem *mem_item = objTypeMake(mem_type, st_mem->m_name, global_off + st_mem->m_member_location);
			mem_item->setText(TYPE_DETAIL_OFFSET_ON_PARENT, g_target->arch0xAddr(st_mem->m_member_location));
			ret_item->addChild(mem_item);
			last_member_stopat = st_mem->m_member_location + mem_type->dataSize();
		}
		if (last_member_stopat < type_st->dataSize()) {
			/*Alignment space at end*/
			QTreeWidgetItem *space_item = new QTreeWidgetItem();
			space_item->setText(TYPE_DETAIL_MEMBER, "/*Alignment space*/");
			space_item->setText(TYPE_DETAIL_SIZE, QString::asprintf("%llu", type_st->dataSize() - last_member_stopat));
			space_item->setText(TYPE_DETAIL_ALIGN, "--");
			space_item->setText(TYPE_DETAIL_OFFSET, "--");
			space_item->setText(TYPE_DETAIL_OFFSET_ON_PARENT, "--");
			ret_item->addChild(space_item);
		}
		break;
	}
	case DwfType::TYPE_UN:
	{
		DwfTypeUn *type_un = (DwfTypeUn *)final_type;
		QList<DwfTypeUnMem*>::iterator iter;
		for (iter = type_un->m_member_list.begin(); iter != type_un->m_member_list.end(); iter++) {
			DwfTypeUnMem *un_mem = *iter;
			DwfType *mem_type = un_mem->m_type;

			QTreeWidgetItem *mem_item = objTypeMake(mem_type, un_mem->m_name, global_off);
			mem_item->setText(TYPE_DETAIL_OFFSET_ON_PARENT, g_target->arch0xAddr(0));
			ret_item->addChild(mem_item);
		}
		break;
	}
	default:
	{
		break;
	}
	}

	return ret_item;
}

void AnalyseDlg::slotTypeListCellClicked(int row, int column)
{
	QTableWidgetItem *item = m_type_table->item(row, TYPE_COLUMN_DESC);
	if (item == NULL) {
		return;
	}
	ListDataInfo datainfo = item->data(Qt::UserRole).value<ListDataInfo>();

	TargetMod *mod = g_target->modGetByPath(datainfo.m_mod);
	if (mod == NULL) {
		return;
	}
	DwfCu *cu = mod->dwarfGetCuByPath(datainfo.m_cu);
	if (cu == NULL) {
		return;
	}
	DwfType *type = cu->getTypeOnOff(datainfo.m_offset);
	if (type == NULL) {
		return;
	}

	/*Load data type detail*/
	m_type_detail_tree->clear();
	QTreeWidgetItem *data_type_root_item = objTypeMake(type, "", 0);
	m_type_detail_tree->addTopLevelItem(data_type_root_item);
	m_type_detail_tree->expandAll();
	m_type_detail_tree->resizeColumnToContents(TYPE_DETAIL_MEMBER);

	/*Load source file*/
	QString path = cu->getFileNameByNo(type->m_fileno);
	m_type_src_edit->clear();
	m_type_src_edit->loadFile(path, "");
	m_type_src_edit->setFirstVisibleLine(type->m_lineno - 1);
	m_type_src_edit->selectLine(type->m_lineno - 1);

	return;
}

int AnalyseDlg::objTypeLoadOnCu(TargetMod *mod, DwfCu *cu, QSet<unsigned int> &match_files)
{
	int type_index = m_type_table->rowCount();

	QMap<unsigned long long, DwfItem*>::iterator item_iter;
	for (item_iter = cu->m_elements.begin(); item_iter != cu->m_elements.end(); item_iter++) {
		DwfItem *dwfitem = item_iter.value();
		if (dwfitem->m_element_type != DwfItem::DWF_ITEM_TYPE) {
			continue;
		}
		QSet<unsigned int>::iterator match_iter;
		match_iter = match_files.find(dwfitem->m_fileno);
		if (match_iter == match_files.end()) {
			/*Type not match*/
			continue;
		}

		DwfType *dwftype = (DwfType *)dwfitem;
		if (dwftype->m_data_type == DwfType::TYPE_ST
		    || dwftype->m_data_type == DwfType::TYPE_UN) {
			/*Only support struct and union*/
			m_type_table->setRowCount(type_index + 1);

			ListDataInfo datainfo(mod->m_remote_path, cu->m_path, dwftype->m_offset);

			/*Type description*/
			QString before;
			QString after;
			DwfType::typeDesc(dwftype, before, after);
			QString desc = before + after;

			QTableWidgetItem *item_desc = new QTableWidgetItem(desc);
			item_desc->setToolTip(desc);
			item_desc->setData(Qt::UserRole, QVariant::fromValue(datainfo));
			m_type_table->setItem(type_index, TYPE_COLUMN_DESC, item_desc);

			/*Type length*/
			QString size = QString::asprintf("%u", dwftype->dataSize());
			QTableWidgetItem *item_size = new QTableWidgetItem(size);
			item_size->setToolTip(size);
			m_type_table->setItem(type_index, TYPE_COLUMN_SIZE, item_size);

			/*Type align*/
			QString align = QString::asprintf("%u", dwftype->dataAlign());
			QTableWidgetItem *item_align = new QTableWidgetItem(align);
			item_align->setToolTip(align);
			m_type_table->setItem(type_index, TYPE_COLUMN_ALIGN, item_align);

			/*Type file and line*/
			QString fileline = cu->getFileNameByNo(dwfitem->m_fileno)
					           + QString::asprintf(":%llu", dwfitem->m_lineno);
			QTableWidgetItem *item_fileline = new QTableWidgetItem(fileline);
			item_fileline->setToolTip(fileline);
			m_type_table->setItem(type_index, TYPE_COLUMN_FILELINE, item_fileline);

			type_index++;
		}
	}
	m_type_table->resizeColumnsToContents();

	return 0;
}

int AnalyseDlg::objTypeClear()
{
	m_type_table->clearContents();
	m_type_table->setRowCount(0);

	m_type_detail_tree->clear();
	m_type_src_edit->clear();
	return 0;
}

int AnalyseDlg::objFunLoadOnCu(TargetMod *mod, DwfCu *cu, QSet<unsigned int> &match_files)
{
	UTER_ASSERT(mod != NULL);
	UTER_ASSERT(cu != NULL);

	/*Table line number*/
	int fun_index = m_fun_table->rowCount();

	QList<DwfFun*>::iterator fun_iter;
	for (fun_iter = cu->m_funs.begin(); fun_iter != cu->m_funs.end(); fun_iter++) {
		DwfFun *dwffun = *fun_iter;

		QSet<unsigned int>::iterator match_iter;
		match_iter = match_files.find(dwffun->m_fileno);
		if (match_iter == match_files.end()) {
			/*Function not match*/
			continue;
		}

		m_fun_table->setRowCount(fun_index + 1);

		/*Function name*/
		QString desc = dwffun->m_name;
		QTableWidgetItem *item_desc = new QTableWidgetItem(desc);
		item_desc->setToolTip(desc);
		m_fun_table->setItem(fun_index, FUN_COLUMN_DESC, item_desc);

		/*Function address*/
		QString address = g_target->arch0xAddr(dwffun->m_add_range.m_begin);
		QTableWidgetItem *item_addr = new QTableWidgetItem(address);
		item_addr->setToolTip(address);
		m_fun_table->setItem(fun_index, FUN_COLUMN_ADDR, item_addr);

		/*Function length*/
		QString length = g_target->arch0xAddr(dwffun->m_add_range.m_offset);
		QTableWidgetItem *item_size = new QTableWidgetItem(length);
		item_size->setToolTip(length);
		m_fun_table->setItem(fun_index, FUN_COLUMN_LEN, item_size);

		/*Function stack*/
		QString stack;
		stack = "NA";
		QTableWidgetItem *item_stack = new QTableWidgetItem(stack);
		item_size->setToolTip(stack);
		m_fun_table->setItem(fun_index, FUN_COLUMN_STACKSIZE, item_stack);

		/*Function file and line*/
		QString fileline = cu->getFileNameByNo(dwffun->m_fileno)
				           + QString::asprintf(":%llu", dwffun->m_lineno);
		QTableWidgetItem *item_fileline = new QTableWidgetItem(fileline);
		item_fileline->setToolTip(fileline);
		m_fun_table->setItem(fun_index, FUN_COLUMN_FILELINE, item_fileline);

		fun_index++;
	}
	m_fun_table->resizeColumnsToContents();
	return 0;
}

int AnalyseDlg::objFunClear()
{
	m_fun_table->clearContents();
	m_fun_table->setRowCount(0);
	return 0;
}

} /*namespace AnalyseUi*/

Q_DECLARE_METATYPE(AnalyseUi::ListDataInfo);
