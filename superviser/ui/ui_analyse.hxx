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

#ifndef SRC_UI_ANALYSE_HXX_
#define SRC_UI_ANALYSE_HXX_

#include <ui_public.hxx>
#include <ui_editor.hxx>

namespace AnalyseUi {

/* Assistant tool for static analysis, Just analyze binary with debug information, include:
 * O, data struct size, data layout, performance ...
 * O, function stack size, complexity ...
 * O, The too work as dialog, not a dock*/
class AnalyseDlg: public QDialog {
	Q_OBJECT
private slots:
	void slotActionTriggered();
	void slotResetPressed();
	void slotObjTreeItemChanged(QTreeWidgetItem*, int);
	void slotTypeListCellClicked(int, int);
public:
	AnalyseDlg(QWidget *parent = 0);
private:
	int modCuTreeLoad();
	int modCuTreeClear();
	QTreeWidget *m_mod_cu_tree;
	bool m_mod_cu_tree_changing;

	int objLoadFromCu();
	int objLoadClear();

	QTreeWidgetItem *objTypeMake(DwfType *type, QString name, unsigned long long global_off);
	int objTypeLoadOnCu(TargetMod *mod, DwfCu *cu, QSet<unsigned int> &match_files);
	int objTypeClear();
	QTableWidget *m_type_table;
	QTreeWidget *m_type_detail_tree;
	EditorBase *m_type_src_edit;

	int objFunLoadOnCu(TargetMod *mod, DwfCu *cu, QSet<unsigned int> &match_files);
	int objFunClear();
	QTableWidget *m_fun_table;
};

} /*namespace AnalyseUi*/
#endif /*SRC_UI_ANALYSE_HXX_*/
