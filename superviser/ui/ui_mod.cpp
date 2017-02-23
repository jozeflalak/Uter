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

namespace ModUi {

struct ModInfo {
public:
	ModInfo()
	{
		m_mod = "";
		return;
	}
	ModInfo(QString mod)
	{
		m_mod = mod;
		return;
	}
	QString m_mod;
};

struct CuInfo {
public:
	CuInfo()
	{
		m_mod = "";
		return;
	}
	CuInfo(QString mod, QString cu)
	{
		m_mod = mod;
		m_cu = cu;
		return;
	}
	QString m_mod;
	QString m_cu;
};

struct DataInfo {
public:
	DataInfo()
	{
		m_cu = "";
		m_path = "";
		m_lineno = 0;
		m_python = "";
		return;
	}
	DataInfo(QString cu, QString path, unsigned int lineno)
	{
		m_cu = cu;
		m_path = path;
		m_lineno = lineno;
		m_python = "";
		return;
	}
	QString m_cu;
	QString m_path;
	unsigned int m_lineno;
	QString m_python;
};

/*Caption of binary section*/
enum ModSectionColumn {
	MOD_BASE_NAME = 0,
	MOD_BASE_OFFSET,
	MOD_BASE_SIZE,
	MOD_BASE_VMA,
	MOD_BASE_LMA,
	MOD_BASE_CODE,
	MOD_BASE_LOAD,
	MOD_BASE_MAX,
};

static const char *g_mod_section_view_column[] = {
	"Name",
	"Offset",
	"Size",
	"VMA",
	"LMA",
	"Code",
	"Load",
};

UTER_ARRAY_CHECK(g_mod_section_view_column, MOD_BASE_MAX);

enum ModDynSymColumn {
	MOD_DYNSYM_NAME = 0,
	MOD_DYNSYM_OFFSET,
	MOD_DYNSYM_ADDRESS,
	MOD_DYNSYM_SECTION,
	MOD_DYNSYM_FLAGS,
	MOD_DYNSYM_MAX,
};

static const char *g_mod_dynsym_view_column[] = {
	"Name",
	"Offset",
	"Address",
	"Section",
	"Flags",
};

UTER_ARRAY_CHECK(g_mod_dynsym_view_column, MOD_DYNSYM_MAX);

enum ModDbgSymColumn {
	MOD_DBGSYM_NAME = 0,
	MOD_DBGSYM_OFFSET,
	MOD_DBGSYM_ADDRESS,
	MOD_DBGSYM_SECTION,
	MOD_DBGSYM_FLAGS,
	MOD_DBGSYM_MAX,
};

static const char *g_mod_dbgsym_view_column[] = {
	"Name",
	"Offset",
	"Address",
	"Section",
	"Flags",
};

UTER_ARRAY_CHECK(g_mod_dbgsym_view_column, MOD_DBGSYM_MAX);

enum ModDynRelocColumn {
	MOD_DYNRELOC_OFFSET = 0,
	MOD_DYNRELOC_TYPE,
	MOD_DYNRELOC_VALUE,
	MOD_DYNRELOC_MAX,
};

static const char *g_mod_dynreloc_view_column[] = {
	"Offset",
	"Type",
	"Value",
};

UTER_ARRAY_CHECK(g_mod_dynreloc_view_column, MOD_DYNRELOC_MAX);

ModViewDlg::ModViewDlg(QWidget *parent)
:QDialog(parent)
{
	QMetaObject::Connection conn;
	QStringList header;
	int iloop;
	QLabel *label;

	setWindowFlags(Qt::Dialog | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
	setWindowTitle("Uter program explorer");

	QHBoxLayout *out_layout = new QHBoxLayout(this);
	setLayout(out_layout);
	QSplitter *out_split = new QSplitter(Qt::Vertical, this);
	out_layout->addWidget(out_split);

	/* Out layout
	 * ---------------------------------------------------
	 *                80%   module/binary/debug
	 * ---------------------------------------------------
	 *                20%   python script
	 * ---------------------------------------------------*/
	/*part of top: top is splitter*/
	QSplitter *top_split = new QSplitter(Qt::Horizontal, this);
	out_split->addWidget(top_split);
	out_split->setStretchFactor(0, 80);

	/*part of bottom: Setup python edit first ......*/
	QWidget *detail_widget = new QWidget(this);
	out_split->addWidget(detail_widget);
	out_split->setStretchFactor(1, 20);

	QVBoxLayout *detail_layout = new QVBoxLayout(this);
	detail_widget->setLayout(detail_layout);

	label = new QLabel("Python script example:", this);
	detail_layout->addWidget(label);

	m_python_sctla = new EditorBase(EditorBase::FILETYPE_PY, this);
	m_python_sctla->setReadOnly(true);
	detail_layout->addWidget(m_python_sctla);

	/* Top layout
	 * -----------------------------
	 * |M   | tab-debug | tab-binary
	 * |o	|-----------------------
	 * |d	|
	 * -----------------------------*/
	QWidget *mod_widget = new QWidget(this);
	top_split->addWidget(mod_widget);
	QVBoxLayout *mod_layout = new QVBoxLayout(this);
	mod_widget->setLayout(mod_layout);

	QTabWidget *tabview = new QTabWidget(this);
	top_split->addWidget(tabview);

	QSplitter *dbg_split = new QSplitter(Qt::Vertical, this);
	tabview->addTab(dbg_split, "Debug Information");

	QTabWidget *bintab = new QTabWidget(this);
	tabview->addTab(bintab, "Binary Information");

	/* Module layout
	 * -----------
	 *   tool
	 * -----------
	 *   list
	 * -----------*/
	QGridLayout *mod_tool_layout = new QGridLayout(this);
	mod_layout->addLayout(mod_tool_layout);

	m_mod_find_edit = new QLineEdit(this);
	m_mod_find_edit->setClearButtonEnabled(true);
	mod_tool_layout->addWidget(m_mod_find_edit, 0, 0, 1, 3);

	m_mod_find_btn = new QPushButton("Find module", this);
	mod_tool_layout->addWidget(m_mod_find_btn, 1, 1, 1, 1);

	label = new QLabel("Please select a module", this);
	label->setAlignment(Qt::AlignCenter);
	mod_tool_layout->addWidget(label, 2, 0, 1, 3);

	m_mod_list = new QListWidget(this);
	mod_layout->addWidget(m_mod_list);

	/* Debug layout
	 * --------------------------------
	 *      Cu + data
	 * --------------------------------
	 *      Source
	 * --------------------------------*/
	QSplitter *cu_and_data_split = new QSplitter(Qt::Horizontal, this);
	dbg_split->addWidget(cu_and_data_split);
	dbg_split->setStretchFactor(0, 80);

	QWidget *info_widget = new QWidget(this);
	dbg_split->addWidget(info_widget);
	dbg_split->setStretchFactor(1, 20);
	QVBoxLayout *info_layout = new QVBoxLayout(this);
	info_widget->setLayout(info_layout);

	/* Cu & Data layout
	 * -----------------------
	 *     cu    |    data
	 * -----------------------*/
	QWidget *cu_widget = new QWidget(this);
	cu_and_data_split->addWidget(cu_widget);
	QVBoxLayout *cu_layout = new QVBoxLayout(this);
	cu_widget->setLayout(cu_layout);

	QWidget *data_widget = new QWidget(this);
	cu_and_data_split->addWidget(data_widget);
	QVBoxLayout *data_layout = new QVBoxLayout(this);
	data_widget->setLayout(data_layout);

	/* Cu layout
	 * ---------------
	 *    tool
	 * ---------------
	 *    list
	 * ---------------*/
	QGridLayout *cu_tool_layout = new QGridLayout(this);
	cu_layout->addLayout(cu_tool_layout);

	m_cu_find_edit = new QLineEdit(this);
	m_cu_find_edit->setClearButtonEnabled(true);
	cu_tool_layout->addWidget(m_cu_find_edit, 0, 0, 1, 3);

	m_cu_find_btn = new QPushButton("Find source file", this);
	cu_tool_layout->addWidget(m_cu_find_btn, 1, 1, 1, 1);

	label = new QLabel("Support multiselect", this);
	label->setAlignment(Qt::AlignCenter);
	cu_tool_layout->addWidget(label, 2, 0, 1, 3);

	m_cu_list = new QListWidget(this);
	m_cu_list->setSelectionMode(QAbstractItemView::ExtendedSelection);
	cu_layout->addWidget(m_cu_list);
	m_cu_list_changing = false;

	/* Data layout
	 * ---------------
	 *    tool
	 * ---------------
	 *    list
	 * ---------------*/
	QGridLayout *data_tool_layout = new QGridLayout(this);
	data_layout->addLayout(data_tool_layout);
	QGridLayout *data_opt_layout = new QGridLayout(this);
	data_tool_layout->addLayout(data_opt_layout, 0, 0, 2, 3);
	data_opt_layout->setSpacing(0);

	m_data_type = new QCheckBox("Data Type", this);
	m_data_var = new QCheckBox("Variable", this);
	m_data_fun = new QCheckBox("Function", this);

	m_data_type_st = new QCheckBox("Struct", this);
	m_data_type_un = new QCheckBox("Union", this);
	m_data_type_en = new QCheckBox("Enum", this);
	m_data_type_td = new QCheckBox("Typedef", this);

	m_data_opt_changing = false;

	data_opt_layout->addWidget(new QLabel("Element:", this), 0, 0);
	data_opt_layout->addWidget(m_data_type, 0, 1);
	data_opt_layout->addWidget(m_data_var, 0, 2);
	data_opt_layout->addWidget(m_data_fun, 0, 3);
	data_opt_layout->addWidget(new QLabel("Type:", this), 1, 0);
	data_opt_layout->addWidget(m_data_type_st, 1, 1);
	data_opt_layout->addWidget(m_data_type_un, 1, 2);
	data_opt_layout->addWidget(m_data_type_en, 1, 3);
	data_opt_layout->addWidget(m_data_type_td, 1, 4);

	m_data_find_edit = new QLineEdit(this);
	m_data_find_edit->setClearButtonEnabled(true);
	data_tool_layout->addWidget(m_data_find_edit, 2, 0, 1, 3);

	m_data_find_btn = new QPushButton("Filter element", this);
	data_tool_layout->addWidget(m_data_find_btn, 3, 1, 1, 1);

	m_data_list = new QListWidget(this);
	data_layout->addWidget(m_data_list);

	/*Source layout*/
	m_src_sctla = new EditorBase(EditorBase::FILETYPE_CPP, this);
	info_layout->addWidget(m_src_sctla);

	/*mark 0 for arrow*/
	QImage arrowimg(":/images/arrow_right.png");
	arrowimg = arrowimg.scaled(20, 20);
	m_src_sctla->markerDefine(arrowimg, 0);

	/*Marker margin, with mask for marker 0*/
	m_src_sctla->setMarginType(0, QsciScintilla::SymbolMargin);
	m_src_sctla->setMarginWidth(0, 20);
	m_src_sctla->setMarginMarkerMask(0, (1 << 0));

	/*Line number margin*/
	m_src_sctla->setMarginType(1, QsciScintilla::NumberMargin);
	m_src_sctla->setMarginWidth(1, 40);
	m_src_sctla->setMarginMarkerMask(1, 0);

	m_src_sctla->setReadOnly(true);

	conn = connect(m_mod_find_btn, SIGNAL(pressed()), this, SLOT(slotModFindBtnPressed()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_mod_list, SIGNAL(itemClicked(QListWidgetItem*)),
			             this, SLOT(slotModItemClicked(QListWidgetItem*)));
	UTER_ASSERT(conn != NULL);

	conn = connect(m_cu_find_btn, SIGNAL(pressed()), this, SLOT(slotCuFindBtnPressed()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_cu_list, SIGNAL(itemSelectionChanged()),
			             this, SLOT(slotCuItemSelectChanged()));
	UTER_ASSERT(conn != NULL);

	conn = connect(m_data_find_btn, SIGNAL(pressed()), this, SLOT(slotDataFindBtnPressed()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_data_list, SIGNAL(itemClicked(QListWidgetItem*)),
			              this, SLOT(slotDataItemClicked(QListWidgetItem*)));
	UTER_ASSERT(conn != NULL);

	conn = connect(m_data_var, SIGNAL(stateChanged(int)), this, SLOT(slotOptStateChanged(int)));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_data_fun, SIGNAL(stateChanged(int)), this, SLOT(slotOptStateChanged(int)));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_data_type, SIGNAL(stateChanged(int)), this, SLOT(slotOptStateChanged(int)));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_data_type_st, SIGNAL(stateChanged(int)), this, SLOT(slotOptStateChanged(int)));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_data_type_un, SIGNAL(stateChanged(int)), this, SLOT(slotOptStateChanged(int)));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_data_type_en, SIGNAL(stateChanged(int)), this, SLOT(slotOptStateChanged(int)));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_data_type_td, SIGNAL(stateChanged(int)), this, SLOT(slotOptStateChanged(int)));
	UTER_ASSERT(conn != NULL);

	/* Binary layout
	 * --------------------------------------------------------------------
	 * Base Information | Dynamic Symbol | Debug Symbol | Dynamic Reloc
	 * --------------------------------------------------------------------*/
	QWidget *base_widget = new QWidget(this);
	bintab->addTab(base_widget, "Base Information");
	QVBoxLayout *base_layout = new QVBoxLayout(this);
	base_widget->setLayout(base_layout);

	/* Base Information layout
	 * ----------------------------------
	 *       File base information
	 * ----------------------------------
	 *       Secion information
	 * ----------------------------------*/
	label = new QLabel("Base information", this);
	base_layout->addWidget(label);
	m_bin_base_edit = new QTextEdit(this);
	m_bin_base_edit->setReadOnly(true);
	base_layout->addWidget(m_bin_base_edit);
	base_layout->setStretchFactor(m_bin_base_edit, 30);
	label = new QLabel("Section information", this);
	base_layout->addWidget(label);
	m_bin_base_table = new QTableWidget(this);
	base_layout->addWidget(m_bin_base_table);
	base_layout->setStretchFactor(m_bin_base_table, 70);
	/*Line select*/
	m_bin_base_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	/*Single select*/
	m_bin_base_table->setSelectionMode(QAbstractItemView::SingleSelection);
	/*Close caption highlight*/
	m_bin_base_table->horizontalHeader()->setHighlightSections(false);
	/*No index 1,2,3,4....*/
	m_bin_base_table->verticalHeader()->setVisible(false);
	/*Set caption*/
	m_bin_base_table->setColumnCount(MOD_BASE_MAX);
	header.clear();
	for (iloop = 0; iloop < MOD_BASE_MAX; iloop++) {
		header << g_mod_section_view_column[iloop];
	}
	m_bin_base_table->setHorizontalHeaderLabels(header);
	m_bin_base_table->resizeColumnsToContents();
	m_bin_base_table_changing = false;

	/* Dynamic Symbol layout
	 * ---------------------------------------
	 *              tool
	 * ---------------------------------------
	 *              list
	 * ---------------------------------------
	 *              flags
	 * ---------------------------------------*/
	QSplitter *dynsym_split = new QSplitter(Qt::Vertical, this);
	bintab->addTab(dynsym_split, "Dynamic Symbol");

	QWidget *dynsym_widget = new QWidget(this);
	dynsym_split->addWidget(dynsym_widget);
	dynsym_split->setStretchFactor(0, 90);

	QVBoxLayout *dynsym_layout = new QVBoxLayout(this);
	dynsym_widget->setLayout(dynsym_layout);

	QGridLayout *dynsym_tool_layout = new QGridLayout(this);
	dynsym_layout->addLayout(dynsym_tool_layout);

	m_bin_dynsym_find_edit = new QLineEdit(this);
	m_bin_dynsym_find_btn = new QPushButton("Find Symbol", this);
	m_bin_dynsym_find_btn->setAutoDefault(true);
	m_bin_dynsym_opt_forward = new QRadioButton("Forward", this);
	m_bin_dynsym_opt_forward->setChecked(true);
	m_bin_dynsym_opt_backward = new QRadioButton("Backward", this);
	dynsym_tool_layout->addWidget(m_bin_dynsym_find_edit, 0, 0, 1, 4);
	dynsym_tool_layout->addWidget(m_bin_dynsym_opt_forward, 1, 1, 1, 1);
	dynsym_tool_layout->addWidget(m_bin_dynsym_opt_backward, 1, 2, 1, 1);
	dynsym_tool_layout->addWidget(m_bin_dynsym_find_btn, 1, 3, 1, 1);

	m_bin_dynsym_table = new QTableWidget(this);
	dynsym_layout->addWidget(m_bin_dynsym_table);
	/*Line select*/
	m_bin_dynsym_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	/*Single select*/
	m_bin_dynsym_table->setSelectionMode(QAbstractItemView::SingleSelection);
	/*Disable high light*/
	m_bin_dynsym_table->horizontalHeader()->setHighlightSections(false);
	/*Set caption*/
	m_bin_dynsym_table->setColumnCount(MOD_DYNSYM_MAX);
	header.clear();
	for (iloop = 0; iloop < MOD_DYNSYM_MAX; iloop++) {
		header << g_mod_dynsym_view_column[iloop];
	}
	m_bin_dynsym_table->setHorizontalHeaderLabels(header);
	m_bin_dynsym_table->resizeColumnsToContents();
	m_bin_dynsym_table_changing = false;

	QTextEdit *dynsym_notes = new QTextEdit(this);
	dynsym_split->addWidget(dynsym_notes);
	dynsym_split->setStretchFactor(1, 10);
	dynsym_notes->setReadOnly(true);
	QString sym_flag_notes;
	sym_flag_notes += "l: The symbol is a local\n";
	sym_flag_notes += "g: The symbol is a global\n";
	sym_flag_notes += "u: The symbol is a unique global\n";
	sym_flag_notes += "!: The symbol is both global and local\n";
	sym_flag_notes += "w: The symbol is weak\n";
	sym_flag_notes += "C: The symbol denotes a constructor\n";
	sym_flag_notes += "W: The symbol is a warning\n";
	sym_flag_notes += "I: The symbol is an indirect reference to another symbol\n";
	sym_flag_notes += "i: The symbol is a function to be evaluated during reloc processing\n";
	sym_flag_notes += "d: The symbol is a debugging symbol\n";
	sym_flag_notes += "D: The symbol is a dynamic symbol\n";
	sym_flag_notes += "F: The symbol is the name of a function\n";
	sym_flag_notes += "f: The symbol is the name of a file\n";
	sym_flag_notes += "O: The symbol is the name of an object\n";
	dynsym_notes->setText(sym_flag_notes);

	/* Debug Symbol layout
	 * ---------------------------------------
	 *              tool
	 * ---------------------------------------
	 *              list
	 * ---------------------------------------
	 *              flags
	 * ---------------------------------------*/
	QSplitter *dbgsym_split = new QSplitter(Qt::Vertical, this);
	bintab->addTab(dbgsym_split, "Debug Symbol");

	QWidget *dbgsym_widget = new QWidget(this);
	dbgsym_split->addWidget(dbgsym_widget);
	dbgsym_split->setStretchFactor(0, 90);

	QVBoxLayout *dbgsym_layout = new QVBoxLayout(this);
	dbgsym_widget->setLayout(dbgsym_layout);

	QGridLayout *dbgsym_tool_layout = new QGridLayout(this);
	dbgsym_layout->addLayout(dbgsym_tool_layout);

	m_bin_dbgsym_find_edit = new QLineEdit(this);
	m_bin_dbgsym_find_btn = new QPushButton("Find Symbol", this);
	m_bin_dbgsym_find_btn->setAutoDefault(true);
	m_bin_dbgsym_opt_forward = new QRadioButton("Forward", this);
	m_bin_dbgsym_opt_forward->setChecked(true);
	m_bin_dbgsym_opt_backward = new QRadioButton("Backward", this);
	dbgsym_tool_layout->addWidget(m_bin_dbgsym_find_edit, 0, 0, 1, 4);
	dbgsym_tool_layout->addWidget(m_bin_dbgsym_opt_forward, 1, 1, 1, 1);
	dbgsym_tool_layout->addWidget(m_bin_dbgsym_opt_backward, 1, 2, 1, 1);
	dbgsym_tool_layout->addWidget(m_bin_dbgsym_find_btn, 1, 3, 1, 1);

	m_bin_dbgsym_table = new QTableWidget(this);
	dbgsym_layout->addWidget(m_bin_dbgsym_table);
	/*Line select*/
	m_bin_dbgsym_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	/*Single select*/
	m_bin_dbgsym_table->setSelectionMode(QAbstractItemView::SingleSelection);
	/*Disable highlight*/
	m_bin_dbgsym_table->horizontalHeader()->setHighlightSections(false);
	/*Set caption*/
	m_bin_dbgsym_table->setColumnCount(MOD_DBGSYM_MAX);
	header.clear();
	for (iloop = 0; iloop < MOD_DBGSYM_MAX; iloop++) {
		header << g_mod_dbgsym_view_column[iloop];
	}
	m_bin_dbgsym_table->setHorizontalHeaderLabels(header);
	m_bin_dbgsym_table->resizeColumnsToContents();
	m_bin_dbgsym_table_changing = false;

	QTextEdit *dbgsym_notes = new QTextEdit(sym_flag_notes, this);
	dbgsym_split->addWidget(dbgsym_notes);
	dbgsym_split->setStretchFactor(1, 10);

	dbgsym_notes->setReadOnly(true);
	dbgsym_notes->setText(sym_flag_notes);

	/* Debug Symbol layout
	 * ---------------------------------------
	 *              tool
	 * ---------------------------------------
	 *              list
	 * ---------------------------------------*/
	QWidget *dynreloc_widget = new QWidget(this);
	bintab->addTab(dynreloc_widget, "Dynamic Reloc");
	QVBoxLayout *dynreloc_layout = new QVBoxLayout(this);
	dynreloc_widget->setLayout(dynreloc_layout);

	QGridLayout *dynreloc_tool_layout = new QGridLayout(this);
	dynreloc_layout->addLayout(dynreloc_tool_layout);

	m_bin_dynreloc_find_edit = new QLineEdit(this);
	m_bin_dynreloc_find_btn = new QPushButton("Find String", this);
	m_bin_dynreloc_find_btn->setAutoDefault(true);
	m_bin_dynreloc_opt_forward = new QRadioButton("Forward", this);
	m_bin_dynreloc_opt_forward->setChecked(true);
	m_bin_dynreloc_opt_backward = new QRadioButton("Backward", this);
	dynreloc_tool_layout->addWidget(m_bin_dynreloc_find_edit, 0, 0, 1, 4);
	dynreloc_tool_layout->addWidget(m_bin_dynreloc_opt_forward, 1, 1, 1, 1);
	dynreloc_tool_layout->addWidget(m_bin_dynreloc_opt_backward, 1, 2, 1, 1);
	dynreloc_tool_layout->addWidget(m_bin_dynreloc_find_btn, 1, 3, 1, 1);

	/*Dynamic relocate table*/
	m_bin_dynreloc_table = new QTableWidget(this);
	dynreloc_layout->addWidget(m_bin_dynreloc_table);
	/*Line select*/
	m_bin_dynreloc_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	/*Single select*/
	m_bin_dynreloc_table->setSelectionMode(QAbstractItemView::SingleSelection);
	/*Disable highlight*/
	m_bin_dynreloc_table->horizontalHeader()->setHighlightSections(false);
	/*Set caption*/
	m_bin_dynreloc_table->setColumnCount(MOD_DYNRELOC_MAX);
	header.clear();
	for (iloop = 0; iloop < MOD_DYNRELOC_MAX; iloop++) {
		header << g_mod_dynreloc_view_column[iloop];
	}
	m_bin_dynreloc_table->setHorizontalHeaderLabels(header);
	m_bin_dynreloc_table->resizeColumnsToContents();
	m_bin_dynreloc_table_changing = false;

	/*Setup menu and toolbar*/
	UiMenuSetup *menu = g_mainwin->menuSetup(UiMenuSetup::SEQ_TOOL_EXPLORE, "&Tools");
	UiToolBarSetup *toolbar = g_mainwin->toolBarSetup(UiToolBarSetup::SEQ_TOOL_EXPLORE, "&Tools");

	m_open_act = new QAction("&Program explorer", this);
	m_open_act->setStatusTip("Open explorer windows");
	m_open_act->setIcon(QIcon(":/images/explorer.png"));
	menu->addAction(m_open_act);
	toolbar->addAction(m_open_act);

	conn = connect(m_bin_base_table, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(slotSectionItemChanged(QTableWidgetItem*)));
	UTER_ASSERT(conn != NULL);

	conn = connect(m_bin_dynsym_find_btn, SIGNAL(pressed()), this, SLOT(slotDynSymFindBtnPressed()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_bin_dynsym_table, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(slotDynSymItemChanged(QTableWidgetItem*)));
	UTER_ASSERT(conn != NULL);

	conn = connect(m_bin_dbgsym_find_btn, SIGNAL(pressed()), this, SLOT(slotDbgSymFindBtnPressed()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_bin_dbgsym_table, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(slotDbgSymItemChanged(QTableWidgetItem*)));
	UTER_ASSERT(conn != NULL);

	conn = connect(m_bin_dynreloc_find_btn, SIGNAL(pressed()),
			       this, SLOT(slotDynRelocFindBtnPressed()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_bin_dynreloc_table, SIGNAL(itemChanged(QTableWidgetItem*)),
			        this, SLOT(slotDynRelocItemChanged(QTableWidgetItem*)));
	UTER_ASSERT(conn != NULL);

    conn = connect(m_open_act, SIGNAL(triggered()), this, SLOT(slotActionTriggered()));
    UTER_ASSERT(conn != NULL);

	closeEvent(NULL);
	return;
}

void ModViewDlg::slotModFindBtnPressed()
{
	QString input = m_mod_find_edit->text();

	m_mod_list->clear();

	QMap<QString, TargetMod*>::iterator iter;
	for (iter = g_target->m_mod_items.begin();
		 iter != g_target->m_mod_items.end(); iter++) {
		TargetMod *mod = iter.value();
		if (mod->m_remote_path.indexOf(input) < 0) {
			/*Ignore no match*/
			continue;
		}

		QFileInfo file_info(mod->m_remote_path);
		ModInfo modinfo(mod->m_remote_path);
		QListWidgetItem *item = new QListWidgetItem();
		item->setText(file_info.fileName() + " (" + mod->m_remote_path + ")");
		item->setToolTip(mod->m_remote_path);
		item->setData(Qt::UserRole, QVariant::fromValue(modinfo));
		m_mod_list->addItem(item);
	}

	m_mod_list->sortItems(Qt::AscendingOrder);
	return;
}

void ModViewDlg::slotModItemClicked(QListWidgetItem *item)
{
	/*Simulate find cu for select module*/
	slotCuFindBtnPressed();

	/*Load binary information for module*/
	loadBaseInfo();
	loadDynSym();
	loadDbgSym();
	loadDynReloc();
	return;
}

void ModViewDlg::slotCuFindBtnPressed()
{
	QString input = m_cu_find_edit->text();

	m_cu_list_changing = true;
	m_cu_list->clear();

	/*Get select module*/
	QList<QListWidgetItem*> select_mods = m_mod_list->selectedItems();
	if (select_mods.empty()) {
		m_cu_list_changing = false;
		return;
	}
	QListWidgetItem *item = select_mods.front();
	ModInfo modinfo = item->data(Qt::UserRole).value<ModInfo>();
	TargetMod *mod = g_target->modGetByPath(modinfo.m_mod);
	if (mod == NULL) {
		m_cu_list_changing = false;
		return;
	}

	/*Foreach cus*/
	QMap<QString, DwfCu*>::iterator iter;
	for (iter = mod->m_dwarf_cu_map.begin(); iter != mod->m_dwarf_cu_map.end(); iter++) {
		DwfCu *cu = iter.value();
		if (cu->m_path.indexOf(input) < 0) {
			/*Ignore no match*/
			continue;
		}

		QFileInfo file_info(cu->m_path);
		CuInfo cuinfo(modinfo.m_mod, cu->m_path);
		QListWidgetItem *item = new QListWidgetItem();
		item->setText(file_info.fileName() + " (" + cu->m_path + ")");
		item->setToolTip(cu->m_path);
		item->setData(Qt::UserRole, QVariant::fromValue(cuinfo));
		m_cu_list->addItem(item);
	}

	m_cu_list->selectAll();
	m_cu_list_changing = false;

	/*Simulate find data*/
	slotDataFindBtnPressed();
	return;
}

void ModViewDlg::slotCuItemSelectChanged()
{
	if (m_cu_list_changing) {
		return;
	}

	/*Simulate find data*/
	slotDataFindBtnPressed();
	return;
}

void ModViewDlg::slotDataFindBtnPressed()
{
	m_data_list->clear();
	m_src_sctla->clear();
	m_python_sctla->clear();

	QString input = m_data_find_edit->text();

	QList<QListWidgetItem*> select_mods = m_mod_list->selectedItems();
	if (select_mods.empty()) {
		return;
	}
	QListWidgetItem *item = select_mods.front();
	ModInfo modinfo = item->data(Qt::UserRole).value<ModInfo>();
	TargetMod *mod = g_target->modGetByPath(modinfo.m_mod);
	if (mod == NULL) {
		return;
	}

	/*Foreach all select cu*/
	QList<QListWidgetItem*> select_cus = m_cu_list->selectedItems();
	QList<QListWidgetItem*>::iterator select_iter;
	for (select_iter = select_cus.begin(); select_iter != select_cus.end(); select_iter++) {
		QListWidgetItem *item = *select_iter;

		CuInfo cuinfo = item->data(Qt::UserRole).value<CuInfo>();
		DwfCu *cu = mod->dwarfGetCuByPath(cuinfo.m_cu);
		if (cu == NULL) {
			continue;
		}
		/*Variable*/
		if (m_data_var->checkState() == Qt::Checked) {
			QList<DwfVar*>::iterator var_iter;
			for (var_iter = cu->m_vars.begin(); var_iter != cu->m_vars.end(); var_iter++) {
				DwfVar *var = *var_iter;
				QString desc = var->varDesc();
				if (desc.indexOf(input) < 0) {
					continue;
				}

				DataInfo datainfo(cu->m_path, cu->getFileNameByNo(var->m_fileno), var->m_lineno);
				datainfo.m_python += var->makeItemScript("my_cu", var->m_name);

				QListWidgetItem *item = new QListWidgetItem(desc);
				item->setData(Qt::UserRole, QVariant::fromValue(datainfo));
				item->setToolTip(cu->m_path + "\n-> " + datainfo.m_path + ":" + QString::asprintf("%u", datainfo.m_lineno));
				m_data_list->addItem(item);
			}
		}
		/*Function*/
		if (m_data_fun->checkState() == Qt::Checked) {
			QList<DwfFun*>::iterator fun_iter;
			for (fun_iter = cu->m_funs.begin(); fun_iter != cu->m_funs.end(); fun_iter++) {
				DwfFun *fun = *fun_iter;
				QString desc = fun->funDesc();
				if (desc.indexOf(input) < 0) {
					continue;
				}

				DataInfo datainfo(cu->m_path, cu->getFileNameByNo(fun->m_fileno), fun->m_lineno);
				datainfo.m_python += fun->makeItemScript("my_cu", fun->m_name);

				QListWidgetItem *item = new QListWidgetItem(desc);
				item->setData(Qt::UserRole, QVariant::fromValue(datainfo));
				item->setToolTip(cu->m_path + "\n-> " + datainfo.m_path + ":" + QString::asprintf("%u", datainfo.m_lineno));
				m_data_list->addItem(item);
			}
		}
		/*Data type*/
		if (m_data_type->checkState() == Qt::Checked) {
			QList<DwfType*>::iterator type_iter;
			if (m_data_type_st->isEnabled() && m_data_type_st->checkState() == Qt::Checked) {
				for (type_iter = cu->m_structs.begin(); type_iter != cu->m_structs.end(); type_iter++) {
					DwfType *type = *type_iter;
					QString desc = DwfType::typeDesc(type);
					if (desc.indexOf(input) < 0) {
						continue;
					}

					DataInfo datainfo(cu->m_path, cu->getFileNameByNo(type->m_fileno), type->m_lineno);
					datainfo.m_python += type->makeItemScript("my_cu", "");

					QListWidgetItem *item = new QListWidgetItem(desc);
					item->setData(Qt::UserRole, QVariant::fromValue(datainfo));
					item->setToolTip(cu->m_path + "\n-> " + datainfo.m_path + ":" + QString::asprintf("%u", datainfo.m_lineno));
					m_data_list->addItem(item);
				}
			}
			if (m_data_type_un->isEnabled() && m_data_type_un->checkState() == Qt::Checked) {
				for (type_iter = cu->m_unions.begin(); type_iter != cu->m_unions.end(); type_iter++) {
					DwfType *type = *type_iter;
					QString desc = DwfType::typeDesc(type);
					if (desc.indexOf(input) < 0) {
						continue;
					}

					DataInfo datainfo(cu->m_path, cu->getFileNameByNo(type->m_fileno), type->m_lineno);
					datainfo.m_python += type->makeItemScript("my_cu", "");

					QListWidgetItem *item = new QListWidgetItem(desc);
					item->setData(Qt::UserRole, QVariant::fromValue(datainfo));
					item->setToolTip(cu->m_path + "\n-> " + datainfo.m_path + ":" + QString::asprintf("%u", datainfo.m_lineno));
					m_data_list->addItem(item);
				}
			}
			if (m_data_type_en->isEnabled() && m_data_type_en->checkState() == Qt::Checked) {
				for (type_iter = cu->m_enums.begin(); type_iter != cu->m_enums.end(); type_iter++) {
					DwfType *type = *type_iter;
					QString desc = DwfType::typeDesc(type);
					if (desc.indexOf(input) < 0) {
						continue;
					}

					DataInfo datainfo(cu->m_path, cu->getFileNameByNo(type->m_fileno), type->m_lineno);
					datainfo.m_python += type->makeItemScript("my_cu", "");

					QListWidgetItem *item = new QListWidgetItem(desc);
					item->setData(Qt::UserRole, QVariant::fromValue(datainfo));
					item->setToolTip(cu->m_path + "\n-> " + datainfo.m_path + ":" + QString::asprintf("%u", datainfo.m_lineno));
					m_data_list->addItem(item);
				}
			}
			if (m_data_type_td->isEnabled() && m_data_type_td->checkState() == Qt::Checked) {
				for (type_iter = cu->m_typedefs.begin(); type_iter != cu->m_typedefs.end(); type_iter++) {
					DwfType *type = *type_iter;
					QString desc = QString("typedef ... ") + DwfType::typeDesc(type);
					if (desc.indexOf(input) < 0) {
						continue;
					}

					DataInfo datainfo(cu->m_path, cu->getFileNameByNo(type->m_fileno), type->m_lineno);
					datainfo.m_python += type->makeItemScript("my_cu", "");

					QListWidgetItem *item = new QListWidgetItem(desc);
					item->setData(Qt::UserRole, QVariant::fromValue(datainfo));
					item->setToolTip(cu->m_path + "\n-> " + datainfo.m_path + ":" + QString::asprintf("%u", datainfo.m_lineno));
					m_data_list->addItem(item);
				}
			}
		}
	}
	m_data_list->sortItems(Qt::AscendingOrder);

	return;
}

void ModViewDlg::slotDataItemClicked(QListWidgetItem *item)
{
	m_src_sctla->clear();
	m_src_sctla->markerDeleteAll(0);
	m_python_sctla->clear();

	DataInfo datainfo = item->data(Qt::UserRole).value<DataInfo>();

	/*Load file and show line*/
	m_src_sctla->loadFile(datainfo.m_path, "/*Can not load source file " + datainfo.m_path + "*/");
	m_src_sctla->setFirstVisibleLine(datainfo.m_lineno - 1);
	m_src_sctla->selectLine(datainfo.m_lineno - 1);

	/*Update python script*/
	QString pyscript;
	QList<QListWidgetItem*> select_mods = m_mod_list->selectedItems();
	if (select_mods.empty()) {
		return;
	}
	QListWidgetItem *mod_item = select_mods.front();
	ModInfo modinfo = mod_item->data(Qt::UserRole).value<ModInfo>();
	if (modinfo.m_mod != "") {
		pyscript += "##################################################\n";
		pyscript += "##Mod\n";
		pyscript += "##################################################\n";
		pyscript += "#Export current select module " + modinfo.m_mod + " and save to variable my_mod\n";
		pyscript += "my_mod = uter.module(uter.path(\"" + modinfo.m_mod + "\"))\n";
		pyscript += "#Export current select module use relative path, but some times may be failed, because match too much modules.\n";
		QFileInfo file_info(modinfo.m_mod);
		pyscript += "my_mod = uter.module(uter.path(\"" + file_info.fileName() + "\"))\n";
	}

	pyscript += "##################################################\n";
	pyscript += "##Cu\n";
	pyscript += "##################################################\n";
	QList<QListWidgetItem*> select_cus = m_cu_list->selectedItems();
	QList<QListWidgetItem*>::iterator select_iter;
	pyscript += "#Export all select source file to variable my_cu with full path.\n";
	for (select_iter = select_cus.begin(); select_iter != select_cus.end(); select_iter++) {
		QListWidgetItem *item = *select_iter;
		CuInfo cuinfo = item->data(Qt::UserRole).value<CuInfo>();
		pyscript += "my_cu = uter.compile_units(my_mod, uter.path(\"" + cuinfo.m_cu + "\"))\n";
	}
	pyscript += "#Export all select source file to variable my_cu with relative path, but some times may be failed, because match too much modules.\n";
	for (select_iter = select_cus.begin(); select_iter != select_cus.end(); select_iter++) {
		QListWidgetItem *item = *select_iter;
		CuInfo cuinfo = item->data(Qt::UserRole).value<CuInfo>();
		QFileInfo file_info(cuinfo.m_cu);
		pyscript += "my_cu = uter.compile_units(my_mod, uter.path(\"" + file_info.fileName() + "\"))\n";
	}

	pyscript += datainfo.m_python;
	m_python_sctla->setText(pyscript);
	m_python_sctla->setCursorPosition(m_python_sctla->lines(), 0);
	return;
}

void ModViewDlg::slotOptStateChanged(int state)
{
	if (m_data_opt_changing) {
		return;
	}

	if (m_data_type->checkState() == Qt::Checked) {
		m_data_type_st->setEnabled(true);
		m_data_type_un->setEnabled(true);
		m_data_type_en->setEnabled(true);
		m_data_type_td->setEnabled(true);
	} else {
		m_data_type_st->setEnabled(false);
		m_data_type_un->setEnabled(false);
		m_data_type_en->setEnabled(false);
		m_data_type_td->setEnabled(false);
	}

	slotDataFindBtnPressed();
	return;
}

/*Module base information and section information*/
int ModViewDlg::loadBaseInfo()
{
	clearBaseInfo();

	QList<QListWidgetItem*> select_mods = m_mod_list->selectedItems();
	if (select_mods.empty()) {
		return -1;
	}
	QListWidgetItem *mod_item = select_mods.front();
	ModInfo modinfo = mod_item->data(Qt::UserRole).value<ModInfo>();
	TargetMod *mod = g_target->modGetByPath(modinfo.m_mod);
	if (mod == NULL) {
		return -1;
	}

	/*Base information*/
	QString base_str;
	base_str += "Module path:" + mod->m_remote_path + "\n";
	base_str += "Local path:" + mod->m_local_path + "\n";
	base_str += "Address range: [" + g_target->arch0xAddr(mod->m_addr_range.m_begin);
	base_str += " - " + g_target->arch0xAddr(mod->m_addr_range.m_begin + mod->m_addr_range.m_offset) + ")\n";
	base_str += "Size:" + QString::asprintf("%lld", mod->m_addr_range.m_offset) + "\n";
	m_bin_base_edit->setText(base_str);

	/*Section information, disable sort before setting*/
	m_bin_base_table_changing = true;
	m_bin_base_table->setSortingEnabled(false);
	int iloop = 0;
	QList<BfdSec*>::iterator iter;
	for (iter = mod->m_bfd_sec_list.begin(); iter != mod->m_bfd_sec_list.end(); iter++, iloop++) {
		BfdSec *sec = *iter;
		m_bin_base_table->setRowCount(iloop + 1);

		/*Section name*/
		QTableWidgetItem *item = new QTableWidgetItem(sec->m_name);
		item->setToolTip(item->text());
		m_bin_base_table->setItem(iloop, MOD_BASE_NAME, item);

		/*Section offset in file*/
		item = new QTableWidgetItem(g_target->arch0xAddr(sec->m_foff));
		item->setToolTip(item->text());
		m_bin_base_table->setItem(iloop, MOD_BASE_OFFSET, item);

		/*Section size*/
		item = new QTableWidgetItem(g_target->arch0xAddr(sec->m_size));
		item->setToolTip(item->text());
		m_bin_base_table->setItem(iloop, MOD_BASE_SIZE, item);

		/*Section vma & lma*/
		item = new QTableWidgetItem(g_target->arch0xAddr(sec->m_vma));
		item->setToolTip(item->text());
		m_bin_base_table->setItem(iloop, MOD_BASE_VMA, item);

		item = new QTableWidgetItem(g_target->arch0xAddr(sec->m_lma));
		item->setToolTip(item->text());
		m_bin_base_table->setItem(iloop, MOD_BASE_LMA, item);

		/*Section is executable*/
		QString bcode = sec->m_sec_code ? "Yes" : "No";
		item = new QTableWidgetItem(bcode);
		item->setToolTip(item->text());
		m_bin_base_table->setItem(iloop, MOD_BASE_CODE, item);

		/*Section is loadable*/
		if (sec->m_sec_load) {
			item = new QTableWidgetItem(g_target->arch0xAddr(mod->m_add_mkup + sec->m_vma));
		} else {
			item = new QTableWidgetItem("No");
		}
		item->setToolTip(item->text());
		m_bin_base_table->setItem(iloop, MOD_BASE_LOAD, item);
	}
	/*Enable sort after setting*/
	m_bin_base_table->setSortingEnabled(true);
	m_bin_base_table_changing = false;
	m_bin_base_table->resizeColumnsToContents();
	return 0;
}

int ModViewDlg::clearBaseInfo()
{
	m_bin_base_edit->clear();
	m_bin_base_table->clearContents();
	m_bin_base_table->setRowCount(0);
	return 0;
}

void ModViewDlg::slotSectionItemChanged(QTableWidgetItem *item)
{
	if (m_bin_base_table_changing) {
		return;
	}

	item->setText(item->toolTip());
	return;
}

int ModViewDlg::loadDynSym()
{
	clearDynSym();

	QList<QListWidgetItem*> select_mods = m_mod_list->selectedItems();
	if (select_mods.empty()) {
		return -1;
	}
	QListWidgetItem *mod_item = select_mods.front();
	ModInfo modinfo = mod_item->data(Qt::UserRole).value<ModInfo>();
	TargetMod *mod = g_target->modGetByPath(modinfo.m_mod);
	if (mod == NULL) {
		return -1;
	}

	/*Disable sort before setting*/
	m_bin_dynsym_table->setSortingEnabled(false);
	m_bin_dynsym_table_changing = true;

	int iloop = 0;
	QList<BfdSym*>::iterator iter;
	for (iter = mod->m_bfd_dynsym_list.begin(); iter != mod->m_bfd_dynsym_list.end(); iter++, iloop++) {
		BfdSym *sym = *iter;
		m_bin_dynsym_table->setRowCount(iloop + 1);

		/*name*/
		QTableWidgetItem *item = new QTableWidgetItem(sym->m_name);
		item->setToolTip(item->text());
		m_bin_dynsym_table->setItem(iloop, MOD_DYNSYM_NAME, item);

		/*Offset*/
		item = new QTableWidgetItem(g_target->arch0xAddr(sym->m_addr));
		item->setToolTip(item->text());
		m_bin_dynsym_table->setItem(iloop, MOD_DYNSYM_OFFSET, item);

		/*Address*/
		item = new QTableWidgetItem(g_target->arch0xAddr(mod->m_add_mkup + sym->m_addr));
		item->setToolTip(item->text());
		m_bin_dynsym_table->setItem(iloop, MOD_DYNSYM_ADDRESS, item);

		/*Section*/
		item = new QTableWidgetItem(sym->m_section);
		item->setToolTip(item->text());
		m_bin_dynsym_table->setItem(iloop, MOD_DYNSYM_SECTION, item);

		/*Flags*/
		QString flags = sym->flagsDesc();
		item = new QTableWidgetItem(flags);
		item->setToolTip(item->text());
		m_bin_dynsym_table->setItem(iloop, MOD_DYNSYM_FLAGS, item);
	}
	m_bin_dynsym_table->setSortingEnabled(true);
	m_bin_dynsym_table_changing = false;
	m_bin_dynsym_table->resizeColumnsToContents();

	return 0;
}

int ModViewDlg::clearDynSym()
{
	m_bin_dynsym_table->clearContents();
	m_bin_dynsym_table->setRowCount(0);
	return 0;
}

/*Find next*/
void ModViewDlg::slotDynSymFindBtnPressed()
{
	if (m_bin_dynsym_table->rowCount() == 0) {
		return;
	}
	QString find_what = m_bin_dynsym_find_edit->text();
	if (find_what == "") {
		return;
	}

	int cur_row = m_bin_dynsym_table->currentRow();
	if (cur_row == -1) {
		/*No current select, so find first match*/
		int iloop;
		for (iloop = 0; iloop < m_bin_dynsym_table->rowCount(); iloop++) {
			QTableWidgetItem *item = m_bin_dynsym_table->item(iloop, MOD_DYNSYM_NAME);
			if (item == NULL) {
				return;
			}
			if (item->text().indexOf(find_what) < 0) {
				continue;
			}
			m_bin_dynsym_table->setCurrentItem(item);
			m_bin_dynsym_table->setFocus();
			return;
		}
		return;
	} else {
		int rowcount = m_bin_dynsym_table->rowCount();
		if (m_bin_dynsym_opt_forward->isChecked()) {
			/*Find forward*/
			int iloop;
			for (iloop = cur_row + 1; iloop % rowcount != cur_row; iloop++) {
				int index = iloop % rowcount;
				QTableWidgetItem *item = m_bin_dynsym_table->item(index, MOD_DYNSYM_NAME);
				if (item == NULL) {
					return;
				}
				if (item->text().indexOf(find_what) < 0) {
					continue;
				}
				m_bin_dynsym_table->setCurrentItem(item);
				m_bin_dynsym_table->setFocus();
				return;
			}
		} else {
			/*Find backward*/
			int iloop;
			for (iloop = cur_row + (rowcount - 1); iloop % rowcount != cur_row; iloop--) {
				int index = iloop % rowcount;
				QTableWidgetItem *item = m_bin_dynsym_table->item(index, MOD_DYNSYM_NAME);
				if (item == NULL) {
					return;
				}
				if (item->text().indexOf(find_what) < 0) {
					continue;
				}
				m_bin_dynsym_table->setCurrentItem(item);
				m_bin_dynsym_table->setFocus();
				return;
			}
		}
	}
	return;
}

void ModViewDlg::slotDynSymItemChanged(QTableWidgetItem *item)
{
	if (m_bin_dynsym_table_changing) {
		return;
	}

	item->setText(item->toolTip());
	return;
}

int ModViewDlg::loadDbgSym()
{
	clearDbgSym();

	QList<QListWidgetItem*> select_mods = m_mod_list->selectedItems();
	if (select_mods.empty()) {
		return -1;
	}
	QListWidgetItem *item = select_mods.front();
	ModInfo modinfo = item->data(Qt::UserRole).value<ModInfo>();
	TargetMod *mod = g_target->modGetByPath(modinfo.m_mod);
	if (mod == NULL) {
		return -1;
	}

	m_bin_dbgsym_table_changing = true;
	m_bin_dbgsym_table->setSortingEnabled(false);

	int iloop = 0;
	QList<BfdSym*>::iterator iter;
	for (iter = mod->m_bfd_dbgsym_list.begin(); iter != mod->m_bfd_dbgsym_list.end(); iter++, iloop++) {
		BfdSym *sym = *iter;
		m_bin_dbgsym_table->setRowCount(iloop + 1);

		/*Name*/
		QTableWidgetItem *item = new QTableWidgetItem(sym->m_name);
		item->setToolTip(item->text());
		m_bin_dbgsym_table->setItem(iloop, MOD_DYNSYM_NAME, item);

		/*Offset*/
		item = new QTableWidgetItem(g_target->arch0xAddr(sym->m_addr));
		item->setToolTip(item->text());
		m_bin_dbgsym_table->setItem(iloop, MOD_DYNSYM_OFFSET, item);

		/*Address*/
		item = new QTableWidgetItem(g_target->arch0xAddr(mod->m_add_mkup + sym->m_addr));
		item->setToolTip(item->text());
		m_bin_dbgsym_table->setItem(iloop, MOD_DYNSYM_ADDRESS, item);

		/*Section*/
		item = new QTableWidgetItem(sym->m_section);
		item->setToolTip(item->text());
		m_bin_dbgsym_table->setItem(iloop, MOD_DYNSYM_SECTION, item);

		/*Flags*/
		QString flags = sym->flagsDesc();
		item = new QTableWidgetItem(flags);
		item->setToolTip(item->text());
		m_bin_dbgsym_table->setItem(iloop, MOD_DYNSYM_FLAGS, item);
	}
	m_bin_dbgsym_table->setSortingEnabled(true);
	m_bin_dbgsym_table_changing = false;
	m_bin_dbgsym_table->resizeColumnsToContents();

	return 0;
}

int ModViewDlg::clearDbgSym()
{
	m_bin_dbgsym_table->clearContents();
	m_bin_dbgsym_table->setRowCount(0);
	return 0;
}

void ModViewDlg::slotDbgSymFindBtnPressed()
{
	if (m_bin_dbgsym_table->rowCount() == 0) {
		return;
	}
	QString find_what = m_bin_dbgsym_find_edit->text();
	if (find_what == "") {
		return;
	}

	int cur_row = m_bin_dbgsym_table->currentRow();
	if (cur_row == -1) {
		int iloop;
		for (iloop = 0; iloop < m_bin_dbgsym_table->rowCount(); iloop++) {
			QTableWidgetItem *item = m_bin_dbgsym_table->item(iloop, MOD_DBGSYM_NAME);
			if (item == NULL) {
				return;
			}
			if (item->text().indexOf(find_what) < 0) {
				continue;
			}
			m_bin_dbgsym_table->setCurrentItem(item);
			m_bin_dbgsym_table->setFocus();
			return;
		}
		return;
	} else {
		int rowcount = m_bin_dbgsym_table->rowCount();
		if (m_bin_dbgsym_opt_forward->isChecked()) {
			int iloop;
			for (iloop = cur_row + 1; iloop % rowcount != cur_row; iloop++) {
				int index = iloop % rowcount;
				QTableWidgetItem *item = m_bin_dbgsym_table->item(index, MOD_DBGSYM_NAME);
				if (item == NULL) {
					return;
				}
				if (item->text().indexOf(find_what) < 0) {
					continue;
				}
				m_bin_dbgsym_table->setCurrentItem(item);
				m_bin_dbgsym_table->setFocus();
				return;
			}
		} else {
			int iloop;
			for (iloop = cur_row + (rowcount - 1); iloop % rowcount != cur_row; iloop--) {
				int index = iloop % rowcount;
				QTableWidgetItem *item = m_bin_dbgsym_table->item(index, MOD_DBGSYM_NAME);
				if (item == NULL) {
					return;
				}
				if (item->text().indexOf(find_what) < 0) {
					continue;
				}
				m_bin_dbgsym_table->setCurrentItem(item);
				m_bin_dbgsym_table->setFocus();
				return;
			}
		}
	}
	return;
}

void ModViewDlg::slotDbgSymItemChanged(QTableWidgetItem *item)
{
	if (m_bin_dbgsym_table_changing) {
		return;
	}

	item->setText(item->toolTip());
	return;
}

int ModViewDlg::loadDynReloc()
{
	clearDynReloc();

	QList<QListWidgetItem*> select_mods = m_mod_list->selectedItems();
	if (select_mods.empty()) {
		return -1;
	}
	QListWidgetItem *item = select_mods.front();
	ModInfo modinfo = item->data(Qt::UserRole).value<ModInfo>();
	TargetMod *mod = g_target->modGetByPath(modinfo.m_mod);
	if (mod == NULL) {
		return -1;
	}

	m_bin_dynreloc_table_changing = true;
	m_bin_dynreloc_table->setSortingEnabled(false);

	int iloop = 0;
	QList<BbdReloc*>::iterator iter;
	for (iter = mod->m_bfd_dynreloc_list.begin(); iter != mod->m_bfd_dynreloc_list.end(); iter++, iloop++) {
		BbdReloc *reloc = *iter;
		m_bin_dynreloc_table->setRowCount(iloop + 1);

		/*Offset*/
		QTableWidgetItem *item = new QTableWidgetItem(g_target->arch0xAddr(reloc->m_offset));
		item->setToolTip(item->text());
		m_bin_dynreloc_table->setItem(iloop, MOD_DYNRELOC_OFFSET, item);

		/*Type*/
		item = new QTableWidgetItem(reloc->m_relocto_name);
		item->setToolTip(item->text());
		m_bin_dynreloc_table->setItem(iloop, MOD_DYNRELOC_TYPE, item);

		/*Offset*/
		item = new QTableWidgetItem(reloc->m_value);
		item->setToolTip(item->text());
		m_bin_dynreloc_table->setItem(iloop, MOD_DYNRELOC_VALUE, item);
	}
	m_bin_dynreloc_table->setSortingEnabled(true);
	m_bin_dynreloc_table_changing = false;
	m_bin_dynreloc_table->resizeColumnsToContents();

	return 0;
}

int ModViewDlg::clearDynReloc()
{
	m_bin_dynreloc_table->clearContents();
	m_bin_dynreloc_table->setRowCount(0);
	return 0;
}

void ModViewDlg::slotDynRelocFindBtnPressed()
{
	if (m_bin_dynreloc_table->rowCount() == 0) {
		return;
	}
	QString find_what = m_bin_dynreloc_find_edit->text();
	if (find_what == "") {
		return;
	}

	int cur_row = m_bin_dynreloc_table->currentRow();
	if (cur_row == -1) {
		int iloop;
		for (iloop = 0; iloop < m_bin_dynreloc_table->rowCount(); iloop++) {
			QTableWidgetItem *item = m_bin_dynreloc_table->item(iloop, MOD_DYNRELOC_VALUE);
			if (item == NULL) {
				return;
			}
			if (item->text().indexOf(find_what) < 0) {
				continue;
			}
			m_bin_dynreloc_table->setCurrentItem(item);
			m_bin_dynreloc_table->setFocus();
			return;
		}
		return;
	} else {
		int rowcount = m_bin_dynreloc_table->rowCount();
		if (m_bin_dynreloc_opt_forward->isChecked()) {
			int iloop;
			for (iloop = cur_row + 1; iloop % rowcount != cur_row; iloop++) {
				int index = iloop % rowcount;
				QTableWidgetItem *item = m_bin_dynreloc_table->item(index, MOD_DYNRELOC_VALUE);
				if (item == NULL) {
					return;
				}
				if (item->text().indexOf(find_what) < 0) {
					continue;
				}
				m_bin_dynreloc_table->setCurrentItem(item);
				m_bin_dynreloc_table->setFocus();
				return;
			}
		} else {
			int iloop;
			for (iloop = cur_row + (rowcount - 1); iloop % rowcount != cur_row; iloop--) {
				int index = iloop % rowcount;
				QTableWidgetItem *item = m_bin_dynreloc_table->item(index, MOD_DYNRELOC_VALUE);
				if (item == NULL) {
					return;
				}
				if (item->text().indexOf(find_what) < 0) {
					continue;
				}
				m_bin_dynreloc_table->setCurrentItem(item);
				m_bin_dynreloc_table->setFocus();
				return;
			}
		}
	}
	return;
}

void ModViewDlg::slotDynRelocItemChanged(QTableWidgetItem *item)
{
	if (m_bin_dynreloc_table_changing) {
		return;
	}

	item->setText(item->toolTip());
	return;
}

void ModViewDlg::slotActionTriggered()
{
	openMod();		/*Do not specify module path*/
	return;
}

int ModViewDlg::openMod(QString path)
{
	showMaximized();

	if (path != "") {
		/*Simulate find module without filter, to list all modules*/
		m_mod_find_edit->setText("");
		slotModFindBtnPressed();

		int count = m_mod_list->count();
		int iloop;
		for (iloop = 0; iloop < count; iloop++) {
			QListWidgetItem *item = m_mod_list->item(iloop);
			ModInfo modinfo = item->data(Qt::UserRole).value<ModInfo>();
			if (modinfo.m_mod == path) {
				break;
			}
		}
		if (iloop == count) {
			/*No module matched*/
			return 0;
		}
		/*Select matched*/
		QListWidgetItem *item = m_mod_list->item(iloop);
		item->setSelected(true);
		m_mod_list->setFocus(Qt::MouseFocusReason);
		/*Simulate select the module*/
		slotModItemClicked(item);
	}

	/*Modeless window, not no call exec()*/
	show();
	return 0;
}

void ModViewDlg::closeEvent(QCloseEvent *event)
{
	/*Do clear on close*/
	clear();
	return;
}

int ModViewDlg::clear()
{
	m_mod_find_edit -> clear();

	m_cu_find_edit->clear();

	m_data_find_edit->clear();

	m_data_opt_changing = true;
	m_data_var->setChecked(true);
	m_data_fun->setChecked(true);
	m_data_type->setChecked(true);
	m_data_type_st->setEnabled(true);
	m_data_type_un->setEnabled(true);
	m_data_type_en->setEnabled(true);
	m_data_type_td->setEnabled(true);
	m_data_type_st->setChecked(true);
	m_data_type_un->setChecked(true);
	m_data_type_en->setChecked(true);
	m_data_type_td->setChecked(true);
	m_data_opt_changing = false;

	m_src_sctla->clear();

	m_bin_base_edit->clear();
	clearBaseInfo();
	m_bin_base_table_changing = false;

	m_bin_dynsym_find_edit->clear();
	m_bin_dynsym_opt_forward->setChecked(true);
	clearDynSym();
	m_bin_dynsym_table_changing = false;

	m_bin_dbgsym_find_edit->clear();
	m_bin_dbgsym_opt_forward->setChecked(true);
	clearDbgSym();
	m_bin_dbgsym_table_changing = false;

	m_bin_dynreloc_find_edit->clear();
	m_bin_dynreloc_opt_forward->setChecked(true);
	clearDynReloc();
	m_bin_dynreloc_table_changing = false;

	m_python_sctla->clear();
	return 0;
}

enum ModColumn {
	MOD_COLUMM_BEGIN = 0,
	MOD_COLUMM_END,
	MOD_COLUMM_DBGINFO,
	MOD_COLUMM_PATH,
	//MOD_COLUMM_REDIRECT,
	MOD_COLUMM_MAX,
};

static const char *g_mod_view_column[] = {
	"Begin",
	"End",
	"DebugInfo",
	"Path",
	//"Redirect",
};

UTER_ARRAY_CHECK(g_mod_view_column, MOD_COLUMM_MAX);

ModDock::ModDock()
:WidgetWithDock(SEQ_MOD, Qt::BottomDockWidgetArea, "Module")
{
	QMetaObject::Connection conn;

	QVBoxLayout *out_layout = new QVBoxLayout(this);
	setLayout(out_layout);

	m_table = new QTableWidget(this);
	out_layout->addWidget(m_table);

	/*Disable edit*/
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
	m_table->setColumnCount(MOD_COLUMM_MAX);
	QStringList header;
	int iloop;
	for (iloop = 0; iloop < MOD_COLUMM_MAX; iloop++) {
		header << g_mod_view_column[iloop];
	}
	m_table->setHorizontalHeaderLabels(header);
	m_table->resizeColumnsToContents();

	/*Module detail dialog*/
	m_modview = new ModViewDlg(this);

	conn = connect(m_table, SIGNAL(cellDoubleClicked(int, int)),
			          this, SLOT(slotCellDoubleClicked(int, int)));
	UTER_ASSERT(conn != NULL);

	g_mainwin->dockSetup(this);
	return;
}

void ModDock::slotCellDoubleClicked(int row, int column)
{
	QTableWidgetItem *item = m_table->item(row, MOD_COLUMM_PATH);
	if (item == NULL) {
		return;
	}

	/*View module in detail dialog*/
	QString path = item->text();
	m_modview->openMod(path);
	return;
}

/*Active module from map*/
int ModDock::activeByPath(QString path)
{
	int count = m_table->rowCount();
	int iloop;
	for (iloop = 0; iloop < count; iloop++) {
		QTableWidgetItem *item = m_table->item(iloop, MOD_COLUMM_PATH);
		if (item == NULL) {
			return 0;
		}
		if (path == item->text()) {
			m_table->setCurrentCell(iloop, QItemSelectionModel::Select);	/*Select line*/
			m_table->setFocus(Qt::MouseFocusReason);						/*Move forcus*/
			m_dock->raise();												/*Show dock*/
			return 1;
		}
	}
	return 0;
}

int ModDock::updateOnStop()
{
	clear();

	int iloop = 0;
	QMap<QString, TargetMod*>::iterator iter;
	for (iter = g_target->m_mod_items.begin();
		 iter != g_target->m_mod_items.end(); iter++, iloop++) {
		TargetMod *mod = iter.value();

		m_table->setRowCount(iloop + 1);

		/*Begin & End*/
		m_table->setItem(iloop, MOD_COLUMM_BEGIN, new QTableWidgetItem("[" + g_target->arch0xAddr(mod->m_addr_range.m_begin)));
		m_table->setItem(iloop, MOD_COLUMM_END, new QTableWidgetItem(g_target->arch0xAddr(mod->m_addr_range.m_begin + mod->m_addr_range.m_offset) + ")"));

		/*Debug information level*/
		if (mod->dwarfInfoLevel() <= 0) {
			m_table->setItem(iloop, MOD_COLUMM_DBGINFO, new QTableWidgetItem("No debug information"));
		} else if (mod->dwarfInfoLevel() <= 1) {
			m_table->setItem(iloop, MOD_COLUMM_DBGINFO, new QTableWidgetItem("Debug information"));
		} else {
			m_table->setItem(iloop, MOD_COLUMM_DBGINFO, new QTableWidgetItem("Debug with macor information"));
		}

		/*Path*/
		m_table->setItem(iloop, MOD_COLUMM_PATH, new QTableWidgetItem(QString(mod->m_remote_path.toStdString().c_str())));

		/*Not support redirect*/
		//m_table->setItem(iloop, MOD_COLUMM_REDIRECT, new QTableWidgetItem(tr("")));
	}
	m_table->resizeColumnsToContents();
	/*Sort by start address*/
	m_table->sortByColumn(MOD_COLUMM_BEGIN, Qt::AscendingOrder);

	return 0;
}

int ModDock::updateOnShut()
{
	clear();
	return 0;
}

int ModDock::clear()
{
	m_table->clearContents();
	m_table->setRowCount(0);
	return 0;
}

}; /*namespace ModUi*/

Q_DECLARE_METATYPE(ModUi::ModInfo);
Q_DECLARE_METATYPE(ModUi::CuInfo);
Q_DECLARE_METATYPE(ModUi::DataInfo);
