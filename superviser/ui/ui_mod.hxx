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

#ifndef SRC_UI_MODULE_HXX_
#define SRC_UI_MODULE_HXX_

#include <ui_public.hxx>
#include <ui_editor.hxx>

namespace ModUi {

/*Module detail dialog*/
class ModViewDlg: public QDialog {
	Q_OBJECT
private slots:
	/*Module*/
	void slotModFindBtnPressed();
	void slotModItemClicked(QListWidgetItem *);
	/*Debug*/
	void slotCuFindBtnPressed();
	void slotCuItemSelectChanged();
	void slotDataFindBtnPressed();
	void slotDataItemClicked(QListWidgetItem *);
	void slotOptStateChanged(int state);
	/*Binary*/
	void slotSectionItemChanged(QTableWidgetItem *);
	void slotDynSymFindBtnPressed();
	void slotDynSymItemChanged(QTableWidgetItem *);
	void slotDbgSymFindBtnPressed();
	void slotDbgSymItemChanged(QTableWidgetItem *);
	void slotDynRelocFindBtnPressed();
	void slotDynRelocItemChanged(QTableWidgetItem *);
	/*Menu*/
	void slotActionTriggered();
public:
	ModViewDlg(QWidget *parent);
	int openMod(QString path = "");
private:
	void closeEvent(QCloseEvent *event);
	int clear();
	/*Module*/
	QLineEdit *m_mod_find_edit;
	QPushButton *m_mod_find_btn;
	QListWidget *m_mod_list;

	/*Debug*/
	QLineEdit *m_cu_find_edit;
	QPushButton *m_cu_find_btn;
	QListWidget *m_cu_list;
	bool m_cu_list_changing;

	QLineEdit *m_data_find_edit;
	QPushButton *m_data_find_btn;
	QListWidget *m_data_list;

	QCheckBox *m_data_var;
	QCheckBox *m_data_fun;
	QCheckBox *m_data_type;
	QCheckBox *m_data_type_st;
	QCheckBox *m_data_type_un;
	QCheckBox *m_data_type_en;
	QCheckBox *m_data_type_td;
	bool m_data_opt_changing;

	EditorBase *m_src_sctla;

	/*Binary*/
	int loadBaseInfo();
	int clearBaseInfo();
	QTextEdit *m_bin_base_edit;
	QTableWidget *m_bin_base_table;
	bool m_bin_base_table_changing;

	/*Dynamic symbol*/
	int loadDynSym();
	int clearDynSym();
	QLineEdit *m_bin_dynsym_find_edit;
	QPushButton *m_bin_dynsym_find_btn;
	QRadioButton *m_bin_dynsym_opt_forward;
	QRadioButton *m_bin_dynsym_opt_backward;
	QTableWidget *m_bin_dynsym_table;
	bool m_bin_dynsym_table_changing;

	/*Debug symbols*/
	int loadDbgSym();
	int clearDbgSym();
	QLineEdit *m_bin_dbgsym_find_edit;
	QPushButton *m_bin_dbgsym_find_btn;
	QRadioButton *m_bin_dbgsym_opt_forward;
	QRadioButton *m_bin_dbgsym_opt_backward;
	QTableWidget *m_bin_dbgsym_table;
	bool m_bin_dbgsym_table_changing;

	/*Relocate information*/
	int loadDynReloc();
	int clearDynReloc();
	QLineEdit *m_bin_dynreloc_find_edit;
	QPushButton *m_bin_dynreloc_find_btn;
	QRadioButton *m_bin_dynreloc_opt_forward;
	QRadioButton *m_bin_dynreloc_opt_backward;
	QTableWidget *m_bin_dynreloc_table;
	bool m_bin_dynreloc_table_changing;

	/*Python script*/
	EditorBase *m_python_sctla;

	/*Menu & toolbar*/
	QAction *m_open_act;
	QMenu *m_menu_bar;
	QToolBar *m_tool_bar;
};

class ModDock: public WidgetWithDock {
	Q_OBJECT
private slots:
	void slotCellDoubleClicked(int, int);
public:
	ModDock();
	~ModDock()
	{
		clear();
		return;
	}
	int updateOnStop();
	int updateOnShut();
	int activeByPath(QString path);			/*Active module from map entry*/
private:
	int clear();
	QTableWidget *m_table;					/*Module table*/
	ModViewDlg *m_modview;					/*Module detail dialog*/
};

}; /*namespace ModUi*/
#endif /*SRC_UI_MODULE_HXX_*/
