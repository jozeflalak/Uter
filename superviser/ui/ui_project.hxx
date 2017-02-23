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

#ifndef SRC_UI_PROJECT_HXX_
#define SRC_UI_PROJECT_HXX_

#include <ui_public.hxx>

namespace PrjUi {

class Config {
public:
	class Con {
	public:
		enum ConType {
			TYPE_LOCAL = 1,
			TYPE_TCP,
			TYPE_MAX,
		};

		Con()
		{
			m_con_type = TYPE_MAX;
			return;
		}
		ConType m_con_type;
		struct {
		} m_data_local;
		struct {
		} m_data_tcp;
		struct {
		} m_data_console;
	};

	class Start {
	public:
		enum StartType {
			TYPE_NEW = 1,
			TYPE_ATTACH,
			TYPE_MAX,
		};

		Start()
		{
			m_start_type = TYPE_MAX;

			m_data_new.m_envreplace = false;
			return;
		}
		StartType m_start_type;

		struct {
			QString m_path;
			QList<QString> m_arglist;
			QList<QStringList> m_envlist;
			bool m_envreplace;
		} m_data_new;

		struct {

		} m_data_attach;
	};

	Config()
	{
		m_alive = false;
		return;
	}
	bool m_alive;
	QString m_name;
	Con m_conn;
	Start m_start;
};

class ConfigDlg: public QDialog {
	Q_OBJECT
public:
	ConfigDlg(QString name, QWidget *parent = 0);
private slots:
	void slotTargetNameChanged(const QString &);
	void slotConTypeIndexChanged(int);
	void slotConTcpIpChanged(const QString &);
	void slotConTcpPortChanged(const QString &);
	void slotStartTypeIndexChanged(int);
	void slotStartNewPathChanged(const QString &);
	void slotStartNewArgsChanged();
	void slotStartNewEnvsChanged();
	void slotStartNewPathBrowse();
	void slotOkPressed();
	void slotCancelPressed();
private:
	void doCheck();
	void doLoad(QString name);
	int argsToList(QString &args, QList<QString> &list);
	int argsFromList(QString &args, QList<QString> &list);
	int envsToList(QString &envs, QList<QStringList> &list);
	int envsFromList(QString &envs, QList<QStringList> &list);

	/*----------------------------------------------------------*/
	/* Target Name */
	QString m_name;
	QLineEdit *m_target_name;
	/*----------------------------------------------------------*/
	/* Connection type */
	QComboBox *m_conn_type_combo;
	QList<QWidget*> m_conn_detail_widgets;
	/*--------------------------------*/
	QWidget *m_con_local_widget;
	/*--------------------------------*/
	QWidget *m_con_tcp_widget;
	QLineEdit *m_con_tcp_ip;
	QLineEdit *m_con_tcp_port;
	/*----------------------------------------------------------*/
	/* Start parameter */
	QComboBox *m_start_type_combo;
	QList<QWidget*> m_start_detail_widgets;
	/*--------------------------------*/
	QWidget *m_start_new_widget;
	QLineEdit *m_start_new_path;
	QTextEdit *m_start_new_args;
	QTextEdit *m_start_new_envs;
	QRadioButton *m_start_new_envs_append;
	QRadioButton *m_start_new_envs_replace;
	/*--------------------------------*/
	QWidget *m_start_attach_widget;
	/*----------------------------------------------------------*/

	QLabel *m_info;
	QPushButton *m_ok;
	QPushButton *m_cancel;
};

class CaseNewDlg: public QDialog {
	Q_OBJECT
public:
	CaseNewDlg(QString dir, QWidget *parent = 0);
private slots:
	void slotBtnOk();
	void slotBtnCancel();
	void slotCaseNameChange(const QString &);
public:
	QString m_result;
private:
	QLineEdit *m_dir;
	QLineEdit *m_file;
	QLabel *m_info;

	QPushButton *m_ok;
	QPushButton *m_cancel;
};

class PrjNewDlg: public QDialog {
	Q_OBJECT
public:
	PrjNewDlg(QString dir, QWidget *parent = 0);
private slots:
	void slotBtnBrowse();
	void slotBtnOk();
	void slotBtnCancel();
	void slotPrjDirNameChange(const QString &);
public:
	QString m_result;
private:
	QLineEdit *m_dir;
	QLineEdit *m_file;
	QLabel *m_info;

	QPushButton *m_browse;
	QPushButton *m_ok;
	QPushButton *m_cancel;
};

/*Tree with menu signal*/
class PrjTree: public QTreeWidget {
	Q_OBJECT
public:
	PrjTree(QWidget *parent = 0);
signals:
	void signalContextMenu(QTreeWidgetItem *);
private:
	void contextMenuEvent(QContextMenuEvent *event);
};

class PrjDock: public WidgetWithDock {
	Q_OBJECT
public:
	PrjDock();
signals:
	void signalAddPrjFile(QString);				/*Source event (add)*/
	void signalDelPrjFile(QString);				/*Source event (move)*/
	void signalTargetChange(PrjUi::Config*);	/*Target event*/
private slots:
	void slotProjectCaseDirs(QList<QString>*);
	void slotItemChanged(QTreeWidgetItem *, int);
	void slotItemDoubleClicked(QTreeWidgetItem*, int);
	void slotItemClicked(QTreeWidgetItem *, int);
	void slotContextMenu(QTreeWidgetItem *);
	/*Pop menu action (target)*/
	void slotActTargetNew();
	void slotActTargetConfig();
	void slotActTargetRemove();
	/*Pop menu action (source)*/
	void slotActSrcFileAdd();
	void slotActSrcFileRemove();
	/*Pop menu action (case)*/
	void slotActCaseRootRun();
	void slotActCaseDirAdd();
	void slotActCaseDirRun();
	void slotActCaseDirRemove();
	void slotActCaseFileNew();
	void slotActCaseFileRun();
	void slotActCaseFileDelete();
	/*Toolbar & menu*/
	void slotActTestCurrentCase();
	void slotActTestAllCase();
	void slotNewProject();
	void slotOpenProject();
public:
	QSet<QString> prjFileGet();
	int prjLoadXml(QString path = "");
	int prjSaveXml(QString path = "");
	int prjClear();

	QString m_project_path;
private:
	/*Toolbar & menu*/
    QAction *m_project_act_new;
    QAction *m_project_act_open;
    QAction *m_test_act_case;
    QAction *m_test_act_allcase;

    bool m_tree_change;						/*Changing*/
    PrjTree *m_tree_root;					/*Big tree*/
    QTreeWidgetItem *m_target_node;			/*Sub tree for target*/
	QTreeWidgetItem *m_file_node;			/*Sub tree for source*/
	QTreeWidgetItem *m_case_node;			/*Sub tree for case*/
	QMenu *m_target_root_menu;				/*Pop menu on target root*/
	QMenu *m_target_file_menu;				/*Pop menu on target file*/
	QMenu *m_src_root_menu;					/*Pop menu on source root*/
	QMenu *m_src_file_menu;					/*Pop menu on source file*/
	QMenu *m_case_root_menu;				/*Pop menu on case root*/
	QMenu *m_case_dir_menu;					/*Pop menu on case directory*/
	QMenu *m_case_file_menu;				/*Pop menu on case file*/

	QString m_menu_string_arg_1;			/*Pop menu argument: for directory some times*/
	QString m_menu_string_arg_2;			/*Pop menu argument: for file name some times*/

	int targetTreeFlush();
	int targetTreeClear();
	QMap<QString,Config*> m_target_map;		/*Target configurations*/

	int fileTreeFlush();
	int fileTreeClear();
	QSet<QString> m_srcfiles;				/*Source files*/

	int caseTreeFlush();
	int caseTreeClear();
	QSet<QString> m_casedirs;				/*Case directory*/

	friend class ConfigDlg;
};

} /*namespace PrjUi*/
#endif /* SRC_UI_PROJECT_HXX_ */
