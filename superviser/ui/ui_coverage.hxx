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

#ifndef SRC_UI_COVERAGE_HXX_
#define SRC_UI_COVERAGE_HXX_

#include <ui_public.hxx>
#include <ui_editor.hxx>

namespace CovUi {

class CovNode {
public:
	enum NodeType {
		TYPE_TEXT_CU = 0,
		TYPE_TEXT_MOD,
		TYPE_TEXT_ROOT,
		TYPE_TEXT_NA,
		TYPE_PROG_FUN,
		TYPE_PROG_FILE,
		TYPE_PROG_CU,
		TYPE_PROG_MOD,
		TYPE_MAX
	};

	CovNode()
	{
		m_type = TYPE_MAX;
		return;
	}
	NodeType m_type;
};

/*Program function coverage(Terminal node)*/
class ProgCovFun: public CovNode {
public:
	ProgCovFun(QString name)
	{
		m_type = TYPE_PROG_FUN;
		m_name = name;
		m_lineno = 0;
		token_hit = 0;
		token_num = 0;
		m_count_hit = 0;
		m_count_num = 0;
		return;
	}

	QString m_name;							/*Function name*/
	unsigned int m_lineno;					/*Function line number ??? */
	unsigned int token_hit;					/*Token hit*/
	unsigned int token_num;					/*Token count*/
	unsigned int m_count_hit;				/*Statement hit*/
	unsigned int m_count_num;				/*Statement count*/
};

/*Program file coverage*/
class ProgCovFile: public CovNode {
public:
	ProgCovFile(QString path)
	{
		m_type = TYPE_PROG_FILE;
		m_path = path;
		m_count_hit = 0;
		m_count_num = 0;
		return;
	}
	~ProgCovFile()
	{
		QList<ProgCovFun*>::iterator iter;
		for (iter = m_fun_list.begin(); iter != m_fun_list.end(); /*none*/) {
			ProgCovFun *progfun = *iter;
			delete progfun;
			m_fun_list.erase(iter++);
		}
		return;
	}
	QString m_path;						/*Source file path*/
	QList<ProgCovFun*> m_fun_list;		/*Function list*/
	unsigned int m_count_hit;			/*Statement hit*/
	unsigned int m_count_num;			/*Statement count*/
};

/*Program cu coverage*/
class ProgCovCu: public CovNode {
public:
	ProgCovCu(QString path)
	{
		m_type = TYPE_PROG_CU;
		m_path = path;

		m_count_hit = 0;
		m_count_num = 0;
		return;
	}
	~ProgCovCu()
	{
		QMap<QString, ProgCovFile*>::iterator iter;
		for (iter = m_file_map.begin(); iter != m_file_map.end(); /*none*/) {
			ProgCovFile *progfile = iter.value();
			delete progfile;
			m_file_map.erase(iter++);
		}
		return;
	}
	QString m_path;							/*Cu file path*/
	QMap<QString, ProgCovFile*> m_file_map;	/*File list*/
	unsigned int m_count_hit;				/*Statement hit*/
	unsigned int m_count_num;				/*Statement count*/
};

/*Program mod coverage*/
class ProgCovMod: public CovNode {
public:
	ProgCovMod(QString path)
	{
		m_type = TYPE_PROG_MOD;
		m_path = path;
		return;
	}
	~ProgCovMod()
	{
		QList<ProgCovCu*>::iterator iter;
		for (iter = m_cu_list.begin(); iter != m_cu_list.end(); /*none*/) {
			ProgCovCu *progcu = *iter;
			delete progcu;
			m_cu_list.erase(iter++);
		}
		return;
	}
	QString m_path;						/*Module path*/
	QList<ProgCovCu*> m_cu_list;		/*Cu list*/
};

/*Coverage count of source line*/
class CovLine {
public:
	CovLine()
	{
		m_line_no = 0;
		m_hit = 0;
		m_num = 0;
		return;
	}
	unsigned int m_line_no;			/*Line number*/
	unsigned int m_hit;				/*Statement hit*/
	unsigned int m_num;				/*Statement count*/
};

/*File coverage in cu*/
class TextCovCu: public CovNode {
public:
	TextCovCu(QString cu_path)
	{
		m_type = TYPE_TEXT_CU;
		m_cu = cu_path;

		m_count_hit = 0;
		m_count_num = 0;
		return;
	}
	~TextCovCu()
	{
		QMap<unsigned int, CovLine*>::iterator iter;
		for (iter = m_linemap.begin(); iter != m_linemap.end(); /*none*/) {
			CovLine *covline = iter.value();
			delete covline;
			m_linemap.erase(iter++);
		}
		return;
	}
	QMap<unsigned int, CovLine*> m_linemap;		/*File line coverage*/
	QString m_cu;								/*Name of cu which the file in*/
	unsigned int m_count_hit;					/*Statement hit*/
	unsigned int m_count_num;					/*Statement count*/
};

/*File coverage in module*/
class TextCovMod: public CovNode {
public:
	TextCovMod(QString mod_path)
	{
		m_type = TYPE_TEXT_MOD;
		m_mod = mod_path;

		m_count_hit = 0;
		m_count_num = 0;
		return;
	}
	~TextCovMod()
	{
		QMap<unsigned int, CovLine*>::iterator iter;
		for (iter = m_linemap.begin(); iter != m_linemap.end(); /*none*/) {
			CovLine *covline = iter.value();
			delete covline;
			m_linemap.erase(iter++);
		}

		QMap<QString, TextCovCu*>::iterator iter_cu;
		for (iter_cu = m_cu_map.begin(); iter_cu != m_cu_map.end(); /*none*/) {
			TextCovCu *cucov = iter_cu.value();
			delete cucov;
			m_cu_map.erase(iter_cu++);
		}

		return;
	}
	QMap<QString, TextCovCu*> m_cu_map;			/*Cu sets*/
	QMap<unsigned int, CovLine*> m_linemap;		/*File line coverage*/
	QString m_mod;								/*Module path*/
	unsigned int m_count_hit;					/*Statement hit*/
	unsigned int m_count_num;					/*Statement count*/
};

class TextCovRoot: public CovNode {
public:
	TextCovRoot(QString file_path)
	{
		m_type = TYPE_TEXT_ROOT;
		m_file = file_path;

		m_count_hit = 0;
		m_count_num = 0;
		return;
	}
	~TextCovRoot()
	{
		QMap<unsigned int, CovLine*>::iterator iter;
		for (iter = m_linemap.begin(); iter != m_linemap.end(); /*none*/) {
			CovLine *covline = iter.value();
			delete covline;
			m_linemap.erase(iter++);
		}

		QMap<QString, TextCovMod*>::iterator iter_mod;
		for (iter_mod = m_mod_map.begin(); iter_mod != m_mod_map.end(); /*none*/) {
			TextCovMod *modcov = iter_mod.value();
			delete modcov;
			m_mod_map.erase(iter_mod++);
		}

		return;
	}
	QMap<unsigned int, CovLine*> m_linemap;		/*File line coverage*/
	QMap<QString, TextCovMod*> m_mod_map;		/*Modules sets*/
	QString m_file;								/*File path*/
	unsigned int m_count_hit;					/*Statement hit*/
	unsigned int m_count_num;					/*Statement count*/
};

class TextCovFile: public CovNode {
public:
	TextCovFile(QString filename)
	{
		m_type = TYPE_TEXT_NA;

		m_name = filename;
		m_root_covinfo = NULL;
		return;
	}
	~TextCovFile()
	{
		if (m_root_covinfo != NULL) {
			delete m_root_covinfo;
			m_root_covinfo = NULL;
		}
		return;
	}
	QString m_name;							/*File name*/
	TextCovRoot *m_root_covinfo;			/*File text coverage*/
};

class ApplyCov {
public:
	ApplyCov()
	{
		m_addr = 0;
		m_ref_count = 0;
		m_token = NULL;
		return;
	}
	unsigned long long m_addr;				/*Breakpoint address*/
	unsigned int m_ref_count;				/*Reference count*/
	CovToken *m_token;						/*Token*/
};

class CovDock: public WidgetWithDock {
	Q_OBJECT
private slots:
	/*Tools*/
	void slotStartPressed();
	void slotStopPressed();
	void slotClearPressed();
	void slotUpdatePressed();
	void slotCheckStateChanged(int);
	/*Menu & toolbar*/
	void slotCovNoPrevPressed();
	void slotCovNoNextPressed();
	void slotCovPartPrevPressed();
	void slotCovPartNextPressed();
	/*Text & program view*/
	void slotTextTreeItemClicked(QTreeWidgetItem*, int);
	void slotProgTreeItemClicked(QTreeWidgetItem*, int);
	void slotProgTableItemClicked(QTreeWidgetItem*, int);
	/*Target event*/
	void slotModAdd(TargetMod*);
	void slotModDel(TargetMod*);
	/*Project event*/
	void slotAddPrjFile(QString path);
	void slotDelPrjFile(QString path);

	void slotEditFileOpen(UEditor *);			/*File open event*/
public:
	CovDock();
	~CovDock();
private:
	TextCovCu *makeTextCovCu(DwfCu *cu, QString &file, QMap<CovToken*, unsigned long long> &digintoken);
	TextCovMod *makeTextCovMod(TargetMod *mod, QString &file, QMap<CovToken*, unsigned long long> &digintoken);
	TextCovRoot *makeTextCovRoot(QString &file, QMap<CovToken*, unsigned long long> &digintoken);
	ProgCovFun *makeProgCovFun(DwfCu *cu, DwfFun *fun, QMap<CovToken*, unsigned long long> &digintoken);
	ProgCovCu *makeProgCovCu(DwfCu *cu, QMap<CovToken*, unsigned long long> &digintoken);
	ProgCovMod *makeProgCovMod(TargetMod *mod, QMap<CovToken*, unsigned long long> &digintoken);

	int fileAttach(TextCovFile *covfile);							/*Enable coverage file*/
	int fileDetach(TextCovFile *covfile);							/*Disable coverage file*/
	int fileModSetup(TextCovFile *covfile, TargetMod *moditem);		/*Enable coverage file on module*/
	int fileModTearDown(TextCovFile *covfile, TargetMod *moditem);	/*Disable coverage file on module*/
	int applyAdd(unsigned long long addr, CovToken *token);			/*Apply breakpoint to address with token*/
	int applyDel(unsigned long long addr, CovToken *token);			/*Delete breakkpoint from address*/

	void flushEditor(UEditor *edit = NULL);						/*Flush coverage to editor*/
	int textViewFlush();
	int progViewFlush();
	int progTableFlush(ProgCovCu *progcu);

	QMap<QString, TextCovFile*> m_files;				/*Coverage file*/
	QMap<unsigned long long, ApplyCov*> m_apply_map;	/*Apply breakpoints*/
	QList<ProgCovMod*> m_progcov_list;					/*Program coverage module*/
	/*Tools*/
	bool m_starting;
	bool m_showing;
	QPushButton *m_start_btn;
	QPushButton *m_stop_btn;
	QPushButton *m_clear_btn;
	QPushButton *m_update_btn;
	QCheckBox *m_show_check;
	/*Frame*/
	QTabWidget *m_top_tab;
	QWidget *m_text_view;
	QWidget *m_prog_view;
	/*Text view*/
	QTreeWidget *m_text_cov_tree;
	EditorBase *m_text_cov_edit;
	/*Program view*/
	QTreeWidget *m_prog_cov_tree;
	QTreeWidget *m_prog_detail_tree;
	/*Menu & toolbar*/
	QAction *m_prev_no_act;
	QAction *m_next_no_act;
	QAction *m_prev_part_act;
	QAction *m_next_part_act;
};

} /*namespace CovUi*/
#endif /* SRC_UI_COVERAGE_HXX_ */
