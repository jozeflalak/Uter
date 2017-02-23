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

#include <QtXml/QDomDocument>
#include <target.hxx>
#include <tester.hxx>

#include <ui_mainwin.hxx>

namespace PrjUi {

ConfigDlg::ConfigDlg(QString name, QWidget *parent)
: QDialog(parent)
{
	QMetaObject::Connection conn;
	m_name = name;

	/* Top layout
	 * --------------------------------
	 * Target name: xxxx
	 * --------------------------------
	 * Connection
	 *   xxxxxxxxxxxxxxxx
	 * --------------------------------
	 * Start type
	 *   xxxxxxxxxxxxxxxx
	 * --------------------------------
	 * Error information
	 * --------------------------------
	 *                      Buttons
	 * --------------------------------*/
	QVBoxLayout *top_layout = new QVBoxLayout(this);
	setLayout(top_layout);

	QString groupbox_style = "QGroupBox{border:solid;border-width:2px;border-color:white}";

	/* Name layout
	 * ---------------------------
	 * Target Name:
	 * xxxxxxxxxxxxxxxxxxxx
	 * ---------------------------*/
	QGroupBox *name_group = new QGroupBox("", this);
	top_layout->addWidget(name_group);
	name_group->setStyleSheet(groupbox_style);
	QVBoxLayout *name_layout = new QVBoxLayout(this);
	name_group->setLayout(name_layout);

	QLabel *target_name_label = new QLabel("Target Name:", this);
	m_target_name = new QLineEdit(this);
	name_layout->addWidget(target_name_label);
	name_layout->addWidget(m_target_name);

	m_target_name->setText(m_name);

	/* Connection layout
	 * --------------------------------------
	 * Connection type:xxxx
	 * --------------------------------------
	 * About this type:local/tcp/console/...
	 * --------------------------------------*/
	QGroupBox *conn_group = new QGroupBox("", this);
	top_layout->addWidget(conn_group);
	conn_group->setStyleSheet(groupbox_style);
	QVBoxLayout *conn_layout = new QVBoxLayout(this);
	conn_group->setLayout(conn_layout);

	/*Connection type*/
	QHBoxLayout *conn_type_layout = new QHBoxLayout(this);
	conn_type_layout->setAlignment(Qt::AlignLeft);
	conn_layout->addLayout(conn_type_layout);

	QLabel *conn_type_label = new QLabel("Connection type:", this);
	m_conn_type_combo = new QComboBox(this);
	conn_type_layout->addWidget(conn_type_label);
	conn_type_layout->addWidget(m_conn_type_combo);

	/* Local connection layout
	 * ---------------------------------
	 *             Empty
	 * ---------------------------------*/
	m_con_local_widget = new QWidget(this);	/*empty widget*/
	m_conn_type_combo->addItem("Local", QVariant::fromValue(m_con_local_widget));
	conn_layout->addWidget(m_con_local_widget);
	m_conn_detail_widgets.push_back(m_con_local_widget);

	/* Tcp connection layout
	 * ----------------------------------
	 * IP: x.x.x.x  Port: xxxx
	 * ----------------------------------*/
	m_con_tcp_widget = new QWidget(this);
	m_conn_type_combo->addItem("Tcp", QVariant::fromValue(m_con_tcp_widget));
	conn_layout->addWidget(m_con_tcp_widget);
	m_conn_detail_widgets.push_back(m_con_tcp_widget);

	QHBoxLayout *m_con_tcp_layout = new QHBoxLayout(this);
	m_con_tcp_widget->setLayout(m_con_tcp_layout);
	m_con_tcp_layout->setAlignment(Qt::AlignLeft);

	QLabel *conn_type_tcp_ip_label = new QLabel("IP:", this);
	conn_type_tcp_ip_label->setFixedWidth(30);
	QLabel *conn_type_tcp_port_label = new QLabel("Port:", this);
	conn_type_tcp_port_label->setFixedWidth(50);
	m_con_tcp_ip = new QLineEdit(this);
	m_con_tcp_ip->setFixedWidth(150);
	m_con_tcp_port = new QLineEdit(this);
	m_con_tcp_port->setFixedWidth(60);
	m_con_tcp_layout->addWidget(conn_type_tcp_ip_label);
	m_con_tcp_layout->addWidget(m_con_tcp_ip);
	m_con_tcp_layout->addWidget(conn_type_tcp_port_label);
	m_con_tcp_layout->addWidget(m_con_tcp_port);

	/* Start layout
	 * --------------------------------
	 * Start type: xxxx
	 * --------------------------------
	 * About this type:new/attach/...
	 * --------------------------------*/
	QGroupBox *start_group = new QGroupBox("", this);
	top_layout->addWidget(start_group);
	start_group->setStyleSheet(groupbox_style);

	QVBoxLayout *start_layout = new QVBoxLayout(this);
	start_group->setLayout(start_layout);

	/*Start type*/
	QHBoxLayout *start_type_layout = new QHBoxLayout(this);
	start_layout->addLayout(start_type_layout);
	start_type_layout->setAlignment(Qt::AlignLeft);

	QLabel *start_type_label = new QLabel("Start type:", this);
	m_start_type_combo = new QComboBox(this);
	start_type_layout->addWidget(start_type_label);
	start_type_layout->addWidget(m_start_type_combo);

	/* Start new layout
	 * ---------------------------------
	 * Path: xxxxxxxxxx     Browse
	 * ---------------------------------
	 * Start arguments:
	 *  xxxxxxxxxxxxxxxxxxxxxxx
	 *  xxxxxxxxxxxxxxxxxxxxxxx
	 * ----------------------------------
	 * Start environment:
	 *  xxxxxxxxxxxxxxxxxxxxxxx
	 *  xxxxxxxxxxxxxxxxxxxxxxx
	 *  environment option
	 * ----------------------------------*/
	m_start_new_widget = new QWidget(this);
	m_start_type_combo->addItem("New", QVariant::fromValue(m_start_new_widget));
	start_layout->addWidget(m_start_new_widget);
	m_start_detail_widgets.push_back(m_start_new_widget);

	QVBoxLayout *start_new_layout = new QVBoxLayout(this);
	m_start_new_widget->setLayout(start_new_layout);

	/*Path*/
	QHBoxLayout *start_new_path_layout = new QHBoxLayout(this);
	start_new_layout->addLayout(start_new_path_layout);
	QLabel *start_new_path_label = new QLabel("Path:", this);
	m_start_new_path = new QLineEdit(this);
	QPushButton *start_new_path_browse = new QPushButton("Browse(Local)", this);
	start_new_path_layout->addWidget(start_new_path_label);
	start_new_path_layout->addWidget(m_start_new_path);
	start_new_path_layout->addWidget(start_new_path_browse);

	/*Start arguments*/
	QLabel *m_start_args_label = new QLabel("Start arguments:", this);
	m_start_new_args = new QTextEdit(this);
	m_start_new_args->setFixedHeight(50);
	start_new_layout->addWidget(m_start_args_label);
	start_new_layout->addWidget(m_start_new_args);

	/*Start environments*/
	QLabel *m_start_envs_label = new QLabel("Start envirnments(\"key=value\"split by \";\"):", this);
	m_start_new_envs = new QTextEdit(this);
	m_start_new_envs->setFixedHeight(50);
	start_new_layout->addWidget(m_start_envs_label);
	start_new_layout->addWidget(m_start_new_envs);

	QHBoxLayout *m_start_envs_radio_layout = new QHBoxLayout(this);
	start_new_layout->addLayout(m_start_envs_radio_layout);

	m_start_new_envs_append = new QRadioButton("Append native environment", this);
	m_start_new_envs_replace = new QRadioButton("Replace native environment", this);
	m_start_envs_radio_layout->addWidget(m_start_new_envs_append);
	m_start_envs_radio_layout->addWidget(m_start_new_envs_replace);
	m_start_new_envs_append->setChecked(true);
	m_start_new_envs_append->setEnabled(false);
	m_start_new_envs_replace->setEnabled(false);

	/* Start attach layout
	 * ---------------------------------
	 * TODO:
	 * ---------------------------------*/
	m_start_attach_widget = new QWidget(this);
	m_start_type_combo->addItem("Attach", QVariant::fromValue(m_start_attach_widget));
	start_layout->addWidget(m_start_attach_widget);
	m_start_detail_widgets.push_back(m_start_attach_widget);

	/* Error information layout
	 * --------------------------------
	 * Error information label
	 * --------------------------------*/
	m_info = new QLabel("", this);
	top_layout->addWidget(m_info);
	QPalette palet;
	palet.setColor(QPalette::WindowText, Qt::red);
	m_info->setPalette(palet);

	/* Button layout
	 * -------------------------------
	 *                | Ok  | Cancel |
	 * -------------------------------*/
	QHBoxLayout *button_layout = new QHBoxLayout(this);
	top_layout->addLayout(button_layout);
	button_layout->setAlignment(Qt::AlignRight);
	m_ok = new QPushButton("OK", this);
	m_cancel = new QPushButton("Cancel", this);
	button_layout->addWidget(m_ok);
	button_layout->addWidget(m_cancel);

	setFixedWidth(700);

    conn = connect(m_target_name, SIGNAL(textChanged(const QString&)), this, SLOT(slotTargetNameChanged(const QString&)));
    UTER_ASSERT(conn != NULL);
    conn = connect(m_conn_type_combo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotConTypeIndexChanged(int)));
    UTER_ASSERT(conn != NULL);
    conn = connect(m_con_tcp_ip, SIGNAL(textChanged(const QString&)), this, SLOT(slotConTcpIpChanged(const QString &)));
    UTER_ASSERT(conn != NULL);
    conn = connect(m_con_tcp_port, SIGNAL(textChanged(const QString&)), this, SLOT(slotConTcpPortChanged(const QString &)));
    UTER_ASSERT(conn != NULL);
    conn = connect(m_start_type_combo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotStartTypeIndexChanged(int)));
    UTER_ASSERT(conn != NULL);
    conn = connect(m_start_new_path, SIGNAL(textChanged(const QString&)), this, SLOT(slotStartNewPathChanged(const QString&)));
    UTER_ASSERT(conn != NULL);
    conn = connect(m_start_new_args, SIGNAL(textChanged()), this, SLOT(slotStartNewArgsChanged()));
    UTER_ASSERT(conn != NULL);
    conn = connect(m_start_new_envs, SIGNAL(textChanged()), this, SLOT(slotStartNewEnvsChanged()));
    UTER_ASSERT(conn != NULL);
    conn = connect(start_new_path_browse, SIGNAL(pressed()), this, SLOT(slotStartNewPathBrowse()));
    UTER_ASSERT(conn != NULL);
    conn = connect(m_ok, SIGNAL(pressed()), this, SLOT(slotOkPressed()));
    UTER_ASSERT(conn != NULL);
    conn = connect(m_cancel, SIGNAL(pressed()), this, SLOT(slotCancelPressed()));
    UTER_ASSERT(conn != NULL);

	slotConTypeIndexChanged(0);		/*Select first*/
	slotStartTypeIndexChanged(0);	/*Select first*/
	doCheck();						/*Do first check*/

	if (m_name == "") {
		setWindowTitle("New target");
	} else {
		setWindowTitle("Configure target");
		doLoad(m_name);				/*Load target configuration*/
	}
	return;
}

void ConfigDlg::slotTargetNameChanged(const QString &context)
{
	doCheck();
	return;
}

void ConfigDlg::slotConTypeIndexChanged(int index)
{
	QWidget *set_widget = m_conn_type_combo->itemData(index).value<QWidget*>();
	if (set_widget == NULL) {
		return;
	}

	QList<QWidget*>::iterator iter;
	for (iter = m_conn_detail_widgets.begin(); iter != m_conn_detail_widgets.end(); iter++) {
		QWidget *widget = *iter;
		if (set_widget == widget) {
			widget->setVisible(true);
		} else {
			widget->setVisible(false);
		}
	}

	doCheck();
	return;
}

void ConfigDlg::slotConTcpIpChanged(const QString &context)
{
	doCheck();
	return;
}

void ConfigDlg::slotConTcpPortChanged(const QString &context)
{
	doCheck();
	return;
}

void ConfigDlg::slotStartTypeIndexChanged(int index)
{
	QWidget *set_widget = m_start_type_combo->itemData(index).value<QWidget*>();
	if (set_widget == NULL) {
		return;
	}

	QList<QWidget*>::iterator iter;
	for (iter = m_start_detail_widgets.begin(); iter != m_start_detail_widgets.end(); iter++) {
		QWidget *widget = *iter;
		if (set_widget == widget) {
			widget->setVisible(true);
		} else {
			widget->setVisible(false);
		}
	}

	doCheck();
	return;
}

void ConfigDlg::slotStartNewPathChanged(const QString &context)
{
	doCheck();
	return;
}

void ConfigDlg::slotStartNewArgsChanged()
{
	doCheck();
	return;
}

void ConfigDlg::slotStartNewEnvsChanged()
{
	QString envs_string = m_start_new_envs->toPlainText();
	if (envs_string == "") {
		m_start_new_envs_append->setEnabled(false);
		m_start_new_envs_replace->setEnabled(false);
	} else {
		m_start_new_envs_append->setEnabled(true);
		m_start_new_envs_replace->setEnabled(true);
	}
	doCheck();
	return;
}

void ConfigDlg::slotStartNewPathBrowse()
{
	QString opendir;
	if (m_start_new_path->text() == "") {
		opendir = QDir::homePath();
	} else {
		QFileInfo fileinfo(m_start_new_path->text());
		opendir = fileinfo.path();
	}

	QString getpath = QFileDialog::getOpenFileName(this, "Set local target file", opendir);
	if (getpath == "") {
		return;
	}

	m_start_new_path->setText(getpath);
	return;
}

void ConfigDlg::slotOkPressed()
{
	/*Build configuration*/
	Config *config = new Config();

	QWidget *contype_widget = m_conn_type_combo->currentData(Qt::UserRole).value<QWidget*>();
	if (contype_widget == m_con_local_widget) {
		config->m_conn.m_con_type = Config::Con::TYPE_LOCAL;
	} else if (contype_widget == m_con_tcp_widget) {
		config->m_conn.m_con_type = Config::Con::TYPE_TCP;
		/*TODO:*/
		UTER_ASSERT(0);
	} else {
		UTER_ASSERT(0);
	}

	QWidget *starttype_widget = m_start_type_combo->currentData(Qt::UserRole).value<QWidget*>();
	if (starttype_widget == m_start_new_widget) {
		config->m_start.m_start_type = Config::Start::TYPE_NEW;
		/*Start new path*/
		config->m_start.m_data_new.m_path = m_start_new_path->text();
		/*Start new arguments*/
		QString args_string = m_start_new_args->toPlainText();
		if (args_string != "") {
			QList<QString> arglist;
			int result = argsToList(args_string, arglist);
			if (result < 0) {
				delete config;
				return;
			}
			config->m_start.m_data_new.m_arglist = arglist;
		}
		/*Start new environment*/
		QString envs_string = m_start_new_envs->toPlainText();
		if (envs_string != "") {
			int result = envsToList(envs_string, config->m_start.m_data_new.m_envlist);
			if (result < 0) {
				delete config;
				return;
			}
		}
		if (m_start_new_envs_replace->isChecked()) {
			config->m_start.m_data_new.m_envreplace = true;
		} else {
			config->m_start.m_data_new.m_envreplace = false;
		}
	} else if (starttype_widget == m_start_attach_widget) {
		config->m_start.m_start_type = Config::Start::TYPE_ATTACH;
		/*TODO:*/
		UTER_ASSERT(0);
	} else {
		UTER_ASSERT(0);
	}

	/*Delete old target, if named configuration*/
	bool old_alive = false;
	QMap<QString,Config*>::iterator config_iter;
	if (m_name != "") {
		config_iter = g_mainwin->m_prj_win->m_target_map.find(m_name);
		if (config_iter != g_mainwin->m_prj_win->m_target_map.end()) {
			Config *old_target = config_iter.value();
			old_alive = old_target->m_alive;
			delete old_target;
			g_mainwin->m_prj_win->m_target_map.erase(config_iter);
		}
	}

	/*Setup configuration*/
	QString target_name = m_target_name->text();
	config_iter = g_mainwin->m_prj_win->m_target_map.find(target_name);
	if (config_iter == g_mainwin->m_prj_win->m_target_map.end()) {
		config->m_name = target_name;
		config->m_alive = old_alive;

		g_mainwin->m_prj_win->m_target_map.insert(target_name, config);
		if (old_alive) {
			emit g_mainwin->m_prj_win->signalTargetChange(config);
		}
	}

	/*Flush is easy*/
	g_mainwin->m_prj_win->targetTreeClear();
	g_mainwin->m_prj_win->targetTreeFlush();
	g_mainwin->m_prj_win->prjSaveXml();
	close();
	return;
}

void ConfigDlg::slotCancelPressed()
{
	close();
	return;
}

void ConfigDlg::doCheck()
{
	m_info->setText("");
	m_ok->setEnabled(false);

	/*Check name*/
	QString target_name = m_target_name->text();
	if (target_name == "") {
		m_info->setText("No target name");
		return;
	}

	QMap<QString,Config*>::iterator config_iter;
	config_iter = g_mainwin->m_prj_win->m_target_map.find(target_name);
	if (config_iter != g_mainwin->m_prj_win->m_target_map.end()) {
		if (config_iter.key() != m_name) {
			/* m_name == "" or m_name == "somename"
			 * If target name already exist, and it is not my self*/
			m_info->setText("Target with this name already exists");
			return;
		}
	}

	/*Connection check*/
	QWidget *contype_widget = m_conn_type_combo->currentData(Qt::UserRole).value<QWidget*>();
	if (contype_widget == m_con_local_widget) {
		/*No any check*/;
	} else if (contype_widget == m_con_tcp_widget) {
		m_info->setText("Tcp connection not support now.");
		return;

		QString ip_string = m_con_tcp_ip->text();
		/*TODO:ip check*/

		QString port_string = m_con_tcp_port->text();
		/*TODO:port check*/
	} else {
		UTER_ASSERT(0);
	}

	/*Start check*/
	QWidget *starttype_widget = m_start_type_combo->currentData(Qt::UserRole).value<QWidget*>();
	if (starttype_widget == m_start_new_widget) {
		QString path_string = m_start_new_path->text();
		if (path_string == "") {
			m_info->setText("No start path");
			return;
		}

		QString args_string = m_start_new_args->toPlainText();
		if (args_string != "") {
			QList<QString> arglist;
			int result = argsToList(args_string, arglist);
			if (result < 0) {
				m_info->setText("Arguments format error");
				return;
			}
		}

		QString envs_string = m_start_new_envs->toPlainText();
		if (envs_string != "") {
			QList<QStringList> envlist;
			int result = envsToList(envs_string, envlist);
			if (result < 0) {
				m_info->setText("Environment format error");
				return;
			}
		}
	} else if (starttype_widget == m_start_attach_widget) {
		;
	} else {
		UTER_ASSERT(0);
	}

	m_ok->setEnabled(true);
	return;
}

/*Load configuration from project*/
void ConfigDlg::doLoad(QString name)
{
	QMap<QString,Config*>::iterator config_iter;
	config_iter = g_mainwin->m_prj_win->m_target_map.find(name);
	if (config_iter == g_mainwin->m_prj_win->m_target_map.end()) {
		return;
	}

	Config *config = config_iter.value();

	/*Load name*/
	m_target_name->setText(config->m_name);

	/*Load connection*/
	QWidget *conn_widget = NULL;
	if (config->m_conn.m_con_type == Config::Con::TYPE_LOCAL) {
		conn_widget = m_con_local_widget;
	} else if (config->m_conn.m_con_type == Config::Con::TYPE_TCP) {
		conn_widget = m_con_tcp_widget;
		/*TODO:*/
		UTER_ASSERT(0);
	} else {
		UTER_ASSERT(0);
	}
	int iloop;
	int count = m_conn_type_combo->count();
	for (iloop = 0; iloop < count; iloop++) {
		QWidget *contype_widget = m_conn_type_combo->itemData(iloop, Qt::UserRole).value<QWidget*>();
		if (contype_widget == conn_widget) {
			m_conn_type_combo->setCurrentIndex(iloop);
			break;
		}
	}

	/*Load start*/
	QWidget *start_widget = NULL;
	if (config->m_start.m_start_type == Config::Start::TYPE_NEW) {
		start_widget = m_start_new_widget;
		/*Start new path*/
		m_start_new_path->setText(config->m_start.m_data_new.m_path);
		/*Start new arguments*/
		QString args;
		argsFromList(args, config->m_start.m_data_new.m_arglist);
		m_start_new_args->setText(args);
		/*Start new environments*/
		QString envs;
		envsFromList(envs, config->m_start.m_data_new.m_envlist);
		m_start_new_envs->setText(envs);
		if (config->m_start.m_data_new.m_envreplace) {
			m_start_new_envs_replace->setChecked(true);
		} else {
			m_start_new_envs_append->setChecked(true);
		}
	} else if (config->m_start.m_start_type == Config::Start::TYPE_ATTACH) {
		start_widget = m_start_attach_widget;
		/*TODO:*/
		UTER_ASSERT(0);
	} else {
		UTER_ASSERT(0);
	}
	count = m_start_type_combo->count();
	for (iloop = 0; iloop < count; iloop++) {
		QVariant starttype_datainfo = m_start_type_combo->itemData(iloop, Qt::UserRole);
		QWidget *starttype_widget = starttype_datainfo.value<QWidget*>();
		if (starttype_widget == conn_widget) {
			m_start_type_combo->setCurrentIndex(iloop);
			break;
		}
	}

	return;
}

/*Scan arguments string*/
int ConfigDlg::argsToList(QString &args, QList<QString> &list)
{
	bool slash = false;			/*Set true after \ */
	bool quote = false;			/*Set true after " */
	bool have_quote = false;	/*Set true between " */
	QString lastarg = "";

	int iloop;
	int count = args.count();
	for (iloop = 0; iloop < count; iloop++) {
		QChar qchar = args.at(iloop);
		if (slash) {
			/*Some char after \, eg: \" \\ \x \i */
			slash = false;
			lastarg += qchar;
			continue;
		}
		if (qchar == '\\') {
			/*Char \ , turn slash*/
			slash = true;
			continue;
		}
		if (qchar == '\"') {
			/*Begin " or end ", turn quote*/
			quote = !quote;
			have_quote = true;
			continue;
		}
		if (quote) {
			/*Char between " and " */
			lastarg += qchar;
			continue;
		}
		/*Out of "" */
		if (qchar == ' ') {
			/*End of a argument with char space, eg: "" xxxxx */
			if (have_quote || lastarg != "") {
				list.push_back(lastarg);
				lastarg = "";
				have_quote = false;
			}
			continue;
		} else {
			/*Other char*/
			lastarg += qchar;
		}
	}
	/*End whit \ or " */
	if (slash || quote) {
		return -1;
	}
	/*Last argument*/
	if (have_quote || lastarg != "") {
		list.push_back(lastarg);
		lastarg = "";
		have_quote = false;
	}
	return 0;
}

int ConfigDlg::argsFromList(QString &args, QList<QString> &list)
{
	QList<QString>::iterator arg_iter;
	for (arg_iter = list.begin(); arg_iter != list.end(); arg_iter++) {
		QString arg = *arg_iter;
		arg = arg.replace("\\", "\\\\");	/*Convert \ to \\ */
		arg = arg.replace("\"", "\\\"");	/*Convert " to \" */
		/*Empty argument*/
		if (arg == "") {
			arg = "\"\"";
		}
		/*Space in argument*/
		if (arg.indexOf(" ") >= 0) {
			arg = "\"" + arg + "\"";
		}
		args += arg + " ";
	}
	return 0;
}

int ConfigDlg::envsToList(QString &envs, QList<QStringList> &list)
{
	bool slash = false;			/*Set true after \ */
	bool quote = false;			/*Set true after " */
	QString lastword = "";
	QString key = "";
	QString value = "";

	int iloop;
	int count = envs.count();
	for (iloop = 0; iloop < count; iloop++) {
		QChar qchar = envs.at(iloop);
		if (slash) {
			/*Some char after \, eg: \" \\ \x \i */
			slash = false;
			lastword += qchar;
			continue;
		}
		if (qchar == '\\') {
			/*Char \ , turn slash*/
			slash = true;
			continue;
		}
		if (qchar == '\"') {
			/*Begin " or end ", turn quote*/
			quote = !quote;
			continue;
		}
		if (quote) {
			/*Char between " and " */
			lastword += qchar;
			continue;
		}
		/*Out of "" */
		if (qchar == ' ') {
			/*Ignore space*/
			continue;
		}
		if (qchar == '=') {
			if (key != "") {
				/*Too much key*/
				return -1;
			}
			key = lastword;
			lastword = "";
			if (key == "") {
				/*Empty key*/
				return -1;
			}
			continue;
		}
		if (qchar == ';') {
			if (key == "") {
				/*No key*/
				continue;
			}
			if (value != "") {
				/*Too much value*/
				return -1;
			}
			value = lastword;
			lastword = "";
			if (value == "") {
				/*Value empty*/
				//return -1;
			}

			QStringList node;
			node.push_back(key);
			node.push_back(value);
			key = "";
			value = "";

			list.push_back(node);
			continue;
		}
		/*Other char*/
		lastword += qchar;
	}
	/*End whit \ or " */
	if (slash || quote) {
		return -1;
	}
	/*Last key & value*/
	value = lastword;
	lastword = "";

	if (key == "") {
		/*No key*/
		return 0;
	}

	QStringList node;
	node.push_back(key);
	node.push_back(value);

	list.push_back(node);
	return 0;
}

int ConfigDlg::envsFromList(QString &envs, QList<QStringList> &list)
{
	QList<QStringList>::iterator env_iter;
	for (env_iter = list.begin(); env_iter != list.end(); env_iter++) {
		QStringList env = *env_iter;

		if (env_iter != list.begin()) {
			envs += ";";
		}

		QString arg = env.first();
		arg = arg.replace("\\", "\\\\");	/*Convert \ to \\ */
		arg = arg.replace("\"", "\\\"");	/*Convert " to \" */
		if (arg == "") {
			/*Empty key*/
			arg = "\"\"";
		}
		if (arg.indexOf(" ") >= 0) {
			/*Space in key*/
			arg = "\"" + arg + "\"";
		}
		envs += arg;

		envs += "=";

		arg = env.last();
		arg = arg.replace("\\", "\\\\");	/*Convert \ to \\ */
		arg = arg.replace("\"", "\\\"");	/*Convert \ to \\ */
		if (arg == "") {
			/*Empty value*/
			arg = "\"\"";
		}
		if (arg.indexOf(" ") >= 0) {
			/*Space in value*/
			arg = "\"" + arg + "\"";
		}
		envs += arg;
	}
	return 0;
}

PrjNewDlg::PrjNewDlg(QString dir, QWidget *parent)
{
	QMetaObject::Connection conn;

	setWindowTitle("New project");

	/* Top layout
	 * --------------------------------
	 * Folder : xxxxxxxxxxxxxx   Browse
	 * File : xxxxxxxxxxxxxxx
	 * --------------------------------
	 *                  | OK  | Cancel |
	 * --------------------------------*/
	QVBoxLayout *top_layout = new QVBoxLayout(this);
	setLayout(top_layout);
	top_layout->setAlignment(Qt::AlignBottom);

	/* Main layout */
	QGridLayout *main_layout = new QGridLayout(this);
	top_layout->addLayout(main_layout);

	QLabel *label_dir = new QLabel("Folder:", this);
	QLabel *label_file = new QLabel("File:", this);
	m_browse = new QPushButton("Browse", this);
	m_dir = new QLineEdit(this);
	m_file = new QLineEdit(this);
	m_info = new QLabel("", this);
	QPalette palet;
	palet.setColor(QPalette::WindowText, Qt::red);
	m_info->setPalette(palet);

	main_layout->addWidget(label_dir, 0, 0, 1, 1);
	main_layout->addWidget(m_dir, 0, 1, 1, 1);
	main_layout->addWidget(m_browse, 0, 2, 1, 1);
	main_layout->addWidget(label_file, 1, 0, 1, 1);
	main_layout->addWidget(m_file, 1, 1, 1, 1);
	main_layout->addWidget(m_info, 2, 1, 1, 1);

	m_dir->setText(dir);

	/* Button layout */
	QHBoxLayout *btn_layout = new QHBoxLayout(this);
	top_layout->addLayout(btn_layout);

	m_ok = new QPushButton("OK", this);
	m_cancel = new QPushButton("Cancel", this);

	btn_layout->addWidget(m_ok);
	btn_layout->addWidget(m_cancel);
	btn_layout->setAlignment(Qt::AlignRight);

	m_result = "";

	setFixedWidth(700);

	conn = connect(m_browse, SIGNAL(pressed()), this, SLOT(slotBtnBrowse()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_ok, SIGNAL(pressed()), this, SLOT(slotBtnOk()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_cancel, SIGNAL(pressed()), this, SLOT(slotBtnCancel()));
	UTER_ASSERT(conn != NULL);

	conn = connect(m_dir, SIGNAL(textChanged(const QString&)),
			         this, SLOT(slotPrjDirNameChange(const QString&)));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_file, SIGNAL(textChanged(const QString&)),
			         this, SLOT(slotPrjDirNameChange(const QString&)));
	UTER_ASSERT(conn != NULL);

	slotPrjDirNameChange("");
	return;
}

void PrjNewDlg::slotBtnBrowse()
{
	QString getpath = QFileDialog::getExistingDirectory(this, "Open directory.", m_dir->text(), QFileDialog::ShowDirsOnly);
	if (! getpath.isEmpty()) {
		m_dir->setText(getpath);
	}
	return;
}

void PrjNewDlg::slotBtnOk()
{
	m_result = m_dir->text() + QDir::separator() + m_file->text();
	close();
	return;
}

void PrjNewDlg::slotBtnCancel()
{
	m_result = "";
	close();
	return;
}

void PrjNewDlg::slotPrjDirNameChange(const QString &nouse)
{
	m_ok->setEnabled(false);
	m_info->setText("");

	QString path = m_dir->text() + QDir::separator() + m_file->text();
	QString cleanpath = QDir::cleanPath(path);
	QFileInfo fileinfo(cleanpath);

	if (fileinfo.suffix() != "xml") {
		m_info->setText("Project file need a xml file, eg xxx.xml .");
		return;
	}

	if (fileinfo.exists()) {
		m_info->setText("File exist.");
		return;
	}

	m_ok->setEnabled(true);
	return;
}

CaseNewDlg::CaseNewDlg(QString dir, QWidget *parent)
: QDialog(parent)
{
	QMetaObject::Connection conn;

	setWindowTitle("New test case file");

	/* Top layout
	 * -----------------------------------
	 * Case folder: xxxxxxxxxxxxxxxxxxx
	 * Case file: xxxxxxxxxxxxxx
	 * -----------------------------------
	 *                     | Ok | Cancel |
	 * -----------------------------------*/
	QVBoxLayout *top_layout = new QVBoxLayout(this);
	setLayout(top_layout);
	top_layout->setAlignment(Qt::AlignBottom);

	/* Main layout */
	QGridLayout *main_layout = new QGridLayout(this);
	top_layout->addLayout(main_layout);

	QLabel *label_dir = new QLabel("Case folder:", this);
	QLabel *label_file = new QLabel("Case file:", this);
	m_dir = new QLineEdit(this);
	m_file = new QLineEdit(this);
	m_info = new QLabel("", this);
	QPalette palet;
	palet.setColor(QPalette::WindowText, Qt::red);
	m_info->setPalette(palet);

	main_layout->addWidget(label_dir, 0, 0, 1, 1);
	main_layout->addWidget(m_dir, 0, 1, 1, 1);
	main_layout->addWidget(label_file, 1, 0, 1, 1);
	main_layout->addWidget(m_file, 1, 1, 1, 1);
	main_layout->addWidget(m_info, 2, 1, 1, 1);

	m_dir->setText(dir);
	m_dir->setEnabled(false);

	/*Space*/
	top_layout->addSpacing(100);

	/* Button layout */
	QHBoxLayout *btn_layout = new QHBoxLayout(this);
	top_layout->addLayout(btn_layout);

	m_ok = new QPushButton("OK", this);
	m_cancel = new QPushButton("Cancel", this);
	m_ok->setEnabled(false);

	btn_layout->addWidget(m_ok);
	btn_layout->addWidget(m_cancel);
	btn_layout->setAlignment(Qt::AlignRight);

	m_result = "";

	setFixedWidth(700);

	conn = connect(m_ok, SIGNAL(pressed()), this, SLOT(slotBtnOk()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_cancel, SIGNAL(pressed()), this, SLOT(slotBtnCancel()));
	UTER_ASSERT(conn != NULL);
	conn = connect(m_file, SIGNAL(textChanged(const QString&)),
			         this, SLOT(slotCaseNameChange(const QString&)));
	UTER_ASSERT(conn != NULL);
	return;
}

void CaseNewDlg::slotBtnOk()
{
	m_result = m_dir->text() + QDir::separator() + m_file->text();
	close();
	return;
}

void CaseNewDlg::slotBtnCancel()
{
	m_result = "";
	close();
	return;
}

void CaseNewDlg::slotCaseNameChange(const QString &nouse)
{
	m_ok->setEnabled(false);
	m_info->setText("");

	QString path = m_dir->text() + QDir::separator() + m_file->text();
	QString cleanpath = QDir::cleanPath(path);
	QFileInfo fileinfo(cleanpath);

	if (fileinfo.suffix() != "py") {
		m_info->setText("Case file name need a python source file, eg xxx.py .");
		return;
	}
	if (fileinfo.exists()) {
		m_info->setText("File exist.");
		return;
	}

	m_ok->setEnabled(true);
	return;
}

/*Project tree node*/
struct NodeInfo {
public:
	enum NodeType {
		NODETYPE_ROOT = 0,
		NODETYPE_TARGETROOT,	/*Target root*/
		NODETYPE_TARGETFILE,	/*Target root*/
		NODETYPE_PRJROOT,		/*File root*/
		NODETYPE_PRJFILE,		/*File file*/
		NODETYPE_CASEROOT,		/*Case root*/
		NODETYPE_CASEDIR,		/*Case dir*/
		NODETYPE_CASEFILE,		/*Case file*/
		NODETYPE_MAX
	};

	NodeInfo(NodeType type)
	{
		m_nodetype = type;
		return;
	}
	NodeInfo()
	{
		m_nodetype = NODETYPE_MAX;
		return;
	}
	NodeType m_nodetype;
	QString m_string_info;			/*String parameter for node*/
};

PrjTree::PrjTree(QWidget *parent)
: QTreeWidget(parent)
{
	return;
}

void PrjTree::contextMenuEvent(QContextMenuEvent *event)
{
	QPoint mouse_at = QCursor::pos();
	QPoint point_map = this->mapFromGlobal(mouse_at);
	QTreeWidgetItem *item = itemAt(point_map);
	if (item == NULL) {
		return;
	}

	/*Emit menu signal*/
	emit signalContextMenu(item);
    event->accept();
	return;
}

PrjDock::PrjDock()
:WidgetWithDock(SEQ_PRJ, Qt::LeftDockWidgetArea, "Project")
{
	QMetaObject::Connection conn;

	/* Top layout (A big tree)
	 * --------------------------------------
	 * PrjTree(Root)
	 *    |
	 *    |--Target sub tree
	 *    |
	 *    |--File sub tree
	 *    |
	 *    |--Case sub tree
	 *    |
	 * --------------------------------------*/
	QVBoxLayout *top_layout = new QVBoxLayout(this);
	setLayout(top_layout);

	/*Big tree*/
	m_tree_change = false;
	m_tree_root = new PrjTree(this);
	top_layout->addWidget(m_tree_root);
	m_tree_root->setColumnCount(1);
	m_tree_root->setHeaderHidden(true);

	/*Tree for all project (Root tree)*/
	NodeInfo root_nodeinf(NodeInfo::NODETYPE_ROOT);
	root_nodeinf.m_string_info = "";

	QTreeWidgetItem *project_node = new QTreeWidgetItem();
	project_node->setText(0, "Project");
	project_node->setIcon(0, QIcon(":/images/project.png"));
	project_node->setData(0, Qt::UserRole, QVariant::fromValue(root_nodeinf));
	m_tree_root->addTopLevelItem(project_node);

	/*Target tree (Sub tree)*/
	NodeInfo target_nodeinf(NodeInfo::NODETYPE_TARGETROOT);
	target_nodeinf.m_string_info = "";

	m_target_node = new QTreeWidgetItem();
	m_target_node->setText(0, "Targets");
	m_target_node->setIcon(0, QIcon(":/images/targets.png"));
	m_target_node->setData(0, Qt::UserRole, QVariant::fromValue(target_nodeinf));
	project_node->addChild(m_target_node);

	/*File tree (Sub tree)*/
	NodeInfo prj_nodeinf(NodeInfo::NODETYPE_PRJROOT);
	prj_nodeinf.m_string_info = "";

	m_file_node = new QTreeWidgetItem();
	m_file_node->setText(0, "Project files");
	m_file_node->setIcon(0, QIcon(":/images/elmt_files.png"));
	m_file_node->setData(0, Qt::UserRole, QVariant::fromValue(prj_nodeinf));
	project_node->addChild(m_file_node);

	/*Case tree (Sub tree)*/
	NodeInfo case_nodeinf(NodeInfo::NODETYPE_CASEROOT);
	case_nodeinf.m_string_info = "";

	m_case_node = new QTreeWidgetItem();
	m_case_node->setText(0, "Test cases");
	m_case_node->setIcon(0, QIcon(":/images/test_allcase.png"));
	m_case_node->setData(0, Qt::UserRole, QVariant::fromValue(case_nodeinf));
	project_node->addChild(m_case_node);

	m_tree_root->expandAll();

	/*Pop menu for target root*/
	m_target_root_menu = new QMenu(this);
	QAction *act_target_root_new = new QAction("New target", this);
	m_target_root_menu->addAction(act_target_root_new);

	/*Pop menu for target*/
	m_target_file_menu = new QMenu(this);
	QAction *act_target_file_config = new QAction("Configure target", this);
	m_target_file_menu->addAction(act_target_file_config);
	QAction *act_target_file_remove = new QAction("Remove target", this);
	m_target_file_menu->addAction(act_target_file_remove);

	/*Pop menu for file root*/
	m_src_root_menu = new QMenu(this);
	QAction *act_src_root_add = new QAction("Add file", this);
	m_src_root_menu->addAction(act_src_root_add);

	/*Pop menu for file*/
	m_src_file_menu = new QMenu(this);
	QAction *act_src_file_remove = new QAction("Remove file", this);
	m_src_file_menu->addAction(act_src_file_remove);

	/*Pop menu for case root*/
	m_case_root_menu = new QMenu(this);
	QAction *act_case_root_run = new QAction("Run all case directory", this);
	m_case_root_menu->addAction(act_case_root_run);
	QAction *act_case_root_add = new QAction("Add case directory", this);
	m_case_root_menu->addAction(act_case_root_add);

	/*Pop menu for case dir*/
	m_case_dir_menu = new QMenu(this);
	QAction *act_case_dir_run = new QAction("Run case directory", this);
	m_case_dir_menu->addAction(act_case_dir_run);
	QAction *act_case_dir_new = new QAction("New case file", this);
	m_case_dir_menu->addAction(act_case_dir_new);
	QAction *act_case_dir_remove = new QAction("Remove case directory", this);
	m_case_dir_menu->addAction(act_case_dir_remove);

	/*Pop menu for case file*/
	m_case_file_menu = new QMenu(this);
	QAction *act_case_file_run = new QAction("Run case file", this);
	m_case_file_menu->addAction(act_case_file_run);
	QAction *act_case_file_delete = new QAction("Delete case file", this);
	m_case_file_menu->addAction(act_case_file_delete);

	/*Menu & toolbar (project)*/
	UiMenuSetup *menu = g_mainwin->menuSetup(UiMenuSetup::SEQ_PRJ, "&Project");
	m_project_act_new = new QAction("&New project", this);
	menu->addAction(m_project_act_new);
	m_project_act_open = new QAction("&Open project", this);
	menu->addAction(m_project_act_open);

	/*Menu & toolbar (case)*/
	menu = g_mainwin->menuSetup(UiMenuSetup::SEQ_TEST, "&Test");
	UiToolBarSetup *toolbar = g_mainwin->toolBarSetup(UiToolBarSetup::SEQ_TEST, "&Test");

	m_test_act_case = new QAction("&Run Case", this);
    m_test_act_case->setStatusTip("Run Case");
    m_test_act_case->setIcon(QIcon(":/images/test_case.png"));
    menu->addAction(m_test_act_case);
    toolbar->addAction(m_test_act_case);

    m_test_act_allcase = new QAction("&Run All Case", this);
    m_test_act_allcase->setStatusTip("Run All Case");
    m_test_act_allcase->setIcon(QIcon(":/images/test_allcase.png"));
    menu->addAction(m_test_act_allcase);
    toolbar->addAction(m_test_act_allcase);

    m_test_act_case->setEnabled(false);

    /*Supply case directory list to tester*/
    conn = connect(g_tester, SIGNAL(signalProjectCaseDirs(QList<QString>*)),
    		       this, SLOT(slotProjectCaseDirs(QList<QString>*)), Qt::BlockingQueuedConnection);
    UTER_ASSERT(conn != NULL);

    /*Tree signal*/
    conn = connect(m_tree_root, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(slotItemChanged(QTreeWidgetItem*, int)));
    UTER_ASSERT(conn != NULL);
    conn = connect(m_tree_root, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(slotItemClicked(QTreeWidgetItem*, int)));
    UTER_ASSERT(conn != NULL);
    conn = connect(m_tree_root, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(slotItemDoubleClicked(QTreeWidgetItem*, int)));
    UTER_ASSERT(conn != NULL);
    conn = connect(m_tree_root, SIGNAL(signalContextMenu(QTreeWidgetItem*)), this, SLOT(slotContextMenu(QTreeWidgetItem*)));
    UTER_ASSERT(conn != NULL);

    /*Target root & item*/
    conn = connect(act_target_root_new, SIGNAL(triggered()), this, SLOT(slotActTargetNew()));
    UTER_ASSERT(conn != NULL);
    conn = connect(act_target_file_config, SIGNAL(triggered()), this, SLOT(slotActTargetConfig()));
    UTER_ASSERT(conn != NULL);
    conn = connect(act_target_file_remove, SIGNAL(triggered()), this, SLOT(slotActTargetRemove()));
    UTER_ASSERT(conn != NULL);

    /*File root & item*/
    conn = connect(act_src_root_add, SIGNAL(triggered()), this, SLOT(slotActSrcFileAdd()));
    UTER_ASSERT(conn != NULL);
    conn = connect(act_src_file_remove, SIGNAL(triggered()), this, SLOT(slotActSrcFileRemove()));
    UTER_ASSERT(conn != NULL);

    /*Case root & dir & file*/
    conn = connect(act_case_root_run, SIGNAL(triggered()), this, SLOT(slotActCaseRootRun()));
    UTER_ASSERT(conn != NULL);
    conn = connect(act_case_root_add, SIGNAL(triggered()), this, SLOT(slotActCaseDirAdd()));
    UTER_ASSERT(conn != NULL);
    conn = connect(act_case_dir_run, SIGNAL(triggered()), this, SLOT(slotActCaseDirRun()));
    UTER_ASSERT(conn != NULL);
    conn = connect(act_case_dir_remove, SIGNAL(triggered()), this, SLOT(slotActCaseDirRemove()));
    UTER_ASSERT(conn != NULL);
    conn = connect(act_case_dir_new, SIGNAL(triggered()), this, SLOT(slotActCaseFileNew()));
    UTER_ASSERT(conn != NULL);
    conn = connect(act_case_file_run, SIGNAL(triggered()), this, SLOT(slotActCaseFileRun()));
    UTER_ASSERT(conn != NULL);
    conn = connect(act_case_file_delete, SIGNAL(triggered()), this, SLOT(slotActCaseFileDelete()));
    UTER_ASSERT(conn != NULL);

    /*Menu & toolbar (project)*/
    conn = connect(m_project_act_new, SIGNAL(triggered()), this, SLOT(slotNewProject()));
    UTER_ASSERT(conn != NULL);
    conn = connect(m_project_act_open, SIGNAL(triggered()), this, SLOT(slotOpenProject()));
    UTER_ASSERT(conn != NULL);

    /*Menu & toolbar (case)*/
    conn = connect(m_test_act_case, SIGNAL(triggered()), this, SLOT(slotActTestCurrentCase()));
    UTER_ASSERT(conn != NULL);
    conn = connect(m_test_act_allcase, SIGNAL(triggered()), this, SLOT(slotActTestAllCase()));
    UTER_ASSERT(conn != NULL);

    g_mainwin->dockSetup(this);
	return;
}

void PrjDock::slotProjectCaseDirs(QList<QString> *casedirs)
{
	UTER_ASSERT(casedirs != NULL);

	QSet<QString>::iterator iter;
	for (iter = m_casedirs.begin(); iter != m_casedirs.end(); iter++) {
		QString dir = *iter;
		casedirs->push_back(dir);
	}

	return;
}

/*识别出check state变化并响应*/
void PrjDock::slotItemChanged(QTreeWidgetItem *item, int column)
{
	NodeInfo nodeinf = item->data(0, Qt::UserRole).value<NodeInfo>();
	switch (nodeinf.m_nodetype)
	{
	case NodeInfo::NODETYPE_TARGETFILE:
	{
		if (m_tree_change) {
			return;
		}

		QString targetname = nodeinf.m_string_info;
		QMap<QString,Config*>::iterator iter;
		iter = m_target_map.find(targetname);
		if (iter == m_target_map.end()) {
			return;
		}
		Config *targetconfig = iter.value();

		if (item->checkState(0) == Qt::Checked && !targetconfig->m_alive) {
			/*Select new*/
			for (iter = m_target_map.begin(); iter != m_target_map.end(); iter++) {
				Config *looptargetconfig = iter.value();
				looptargetconfig->m_alive = false;
			}
			targetconfig->m_alive = true;
			int count = m_target_node->childCount();
			int iloop;
			for (iloop = 0; iloop < count; iloop++) {
				QTreeWidgetItem *loopchild = m_target_node->child(iloop);
				m_tree_change = true;
				loopchild->setCheckState(0, Qt::Unchecked);
				m_tree_change = false;
			}
			m_tree_change = true;
			item->setCheckState(0, Qt::Checked);
			m_tree_change = false;
			emit signalTargetChange(targetconfig);

			prjSaveXml();
		} else if (item->checkState(0) == Qt::Unchecked && targetconfig->m_alive) {
			/*Select none*/
			for (iter = m_target_map.begin(); iter != m_target_map.end(); iter++) {
				Config *looptargetconfig = iter.value();
				looptargetconfig->m_alive = false;
			}
			int count = m_target_node->childCount();
			int iloop;
			for (iloop = 0; iloop < count; iloop++) {
				QTreeWidgetItem *loopchild = m_target_node->child(iloop);
				m_tree_change = true;
				loopchild->setCheckState(0, Qt::Unchecked);
				m_tree_change = false;
			}
			emit signalTargetChange(NULL);

			prjSaveXml();
		}

		break;
	}
	default:
	{
		break;
	}
	}

	return;
}

void PrjDock::slotItemDoubleClicked(QTreeWidgetItem *item, int column)
{
	NodeInfo nodeinf = item->data(0, Qt::UserRole).value<NodeInfo>();
	switch(nodeinf.m_nodetype)
	{
	case NodeInfo::NODETYPE_ROOT:
	{
		break;
	}
	case NodeInfo::NODETYPE_TARGETROOT:
	{
		break;
	}
	case NodeInfo::NODETYPE_TARGETFILE:
	{
		ConfigDlg targetconfig(nodeinf.m_string_info, this);
		targetconfig.exec();
		break;
	}
	case NodeInfo::NODETYPE_PRJROOT:
	{
		break;
	}
	case NodeInfo::NODETYPE_PRJFILE:
	{
		QFileInfo fileinfo(nodeinf.m_string_info);
		if (! fileinfo.exists()) {
			return;
		}
		UEditor *editor = g_mainwin->editGetByPath(nodeinf.m_string_info);
		if (editor == NULL) {
			editor = UEditor::createEditByPath(nodeinf.m_string_info);
			g_mainwin->editAddToMdi(editor);
		}
		g_mainwin->editSetActive(editor);
		break;
	}
	case NodeInfo::NODETYPE_CASEROOT:
	{
		break;
	}
	case NodeInfo::NODETYPE_CASEDIR:
	{
		break;
	}
	case NodeInfo::NODETYPE_CASEFILE:
	{
		QFileInfo fileinfo(nodeinf.m_string_info);
		if (! fileinfo.exists()) {
			return;
		}
		UEditor *editor = g_mainwin->editGetByPath(nodeinf.m_string_info);
		if (editor == NULL) {
			editor = UEditor::createEditByPath(nodeinf.m_string_info);
			g_mainwin->editAddToMdi(editor);
		}
		g_mainwin->editSetActive(editor);
		break;
	}
	default:
	{
		break;
	}
	}

	return;
}

/*Click tree node*/
void PrjDock::slotItemClicked(QTreeWidgetItem *item, int column)
{
	m_test_act_case->setEnabled(false);

	NodeInfo nodeinf = item->data(0, Qt::UserRole).value<NodeInfo>();
	switch(nodeinf.m_nodetype)
	{
	case NodeInfo::NODETYPE_CASEFILE:
	{
		m_test_act_case->setEnabled(true);
		break;
	}
	case NodeInfo::NODETYPE_CASEROOT:
	case NodeInfo::NODETYPE_CASEDIR:
	{
		break;
	}
	default:
	{
		break;
	}
	}
	return;
}

/*Tree node context menu event to context menu signal*/
void PrjDock::slotContextMenu(QTreeWidgetItem *item)
{
	QPoint mouse_at = QCursor::pos();

	m_menu_string_arg_1 = "";
	m_menu_string_arg_2 = "";

	NodeInfo nodeinf = item->data(0, Qt::UserRole).value<NodeInfo>();
	switch(nodeinf.m_nodetype)
	{
	case NodeInfo::NODETYPE_ROOT:
	{
		break;
	}
	case NodeInfo::NODETYPE_TARGETROOT:
	{
		m_target_root_menu->exec(mouse_at);
		break;
	}
	case NodeInfo::NODETYPE_TARGETFILE:
	{
		m_menu_string_arg_1 = nodeinf.m_string_info;

		m_target_file_menu->exec(mouse_at);
		break;
	}
	case NodeInfo::NODETYPE_PRJROOT:
	{
		m_src_root_menu->exec(mouse_at);
		break;
	}
	case NodeInfo::NODETYPE_PRJFILE:
	{
		m_menu_string_arg_1 = nodeinf.m_string_info;

		m_src_file_menu->exec(mouse_at);
		break;
	}
	case NodeInfo::NODETYPE_CASEROOT:
	{
		m_case_root_menu->exec(mouse_at);
		break;
	}
	case NodeInfo::NODETYPE_CASEDIR:
	{
		m_menu_string_arg_1 = nodeinf.m_string_info;

		m_case_dir_menu->exec(mouse_at);
		break;
	}
	case NodeInfo::NODETYPE_CASEFILE:
	{
		m_menu_string_arg_2 = nodeinf.m_string_info;

		item = item->parent();
		nodeinf = item->data(0, Qt::UserRole).value<NodeInfo>();
		m_menu_string_arg_1 = nodeinf.m_string_info;

		m_case_file_menu->exec(mouse_at);
		break;
	}
	default:
	{
		break;
	}
	}

	return;
}

void PrjDock::slotActTargetNew()
{
	ConfigDlg targetconfig("", this);
	targetconfig.exec();
	return;
}

void PrjDock::slotActTargetConfig()
{
	ConfigDlg targetconfig(m_menu_string_arg_1, this);
	targetconfig.exec();
	return;
}

void PrjDock::slotActTargetRemove()
{
	QMap<QString,Config*>::iterator iter;
	iter = m_target_map.find(m_menu_string_arg_1);
	if (iter == m_target_map.end()) {
		return;
	}
	Config *targetconfig = iter.value();
	if (targetconfig->m_alive) {
		emit signalTargetChange(NULL);
	}
	delete targetconfig;
	m_target_map.erase(iter);

	/*Flush is easy*/
	targetTreeClear();
	targetTreeFlush();
	prjSaveXml();
	return;
}

void PrjDock::slotActSrcFileAdd()
{
	static QString last_dir;
	QString dir;
	if (last_dir != "") {
		dir = last_dir;
	} else {
		dir = QDir::homePath();
	}

	QString open_filter = "SourceFile (*.c *.h *.cpp *.hpp *.cxx *.hxx);;AllFile (*.*)";
	QStringList filelist = QFileDialog::getOpenFileNames(this, "Open source file", dir, open_filter);
	if (filelist.empty()) {
		return;
	}

	bool flush = false;
	QStringList::iterator file_iter;
	for (file_iter = filelist.begin(); file_iter != filelist.end(); file_iter++) {
		QString file = *file_iter;
		QFileInfo fileinfo(file);
		last_dir = fileinfo.path();
		QString cleanpath = QDir::cleanPath(file);

		QSet<QString>::iterator src_iter = m_srcfiles.find(cleanpath);
		if (src_iter != m_srcfiles.end()) {
			continue;
		}

		m_srcfiles.insert(cleanpath);
		emit signalAddPrjFile(cleanpath);
		flush = true;
	}

	if (flush) {
		/*Flush is easy*/
		fileTreeClear();
		fileTreeFlush();
		prjSaveXml();
	}

	return;
}

void PrjDock::slotActSrcFileRemove()
{
	QSet<QString>::iterator src_iter;
	src_iter = m_srcfiles.find(m_menu_string_arg_1);
	if (src_iter == m_srcfiles.end()) {
		return;
	}
	QString path = *src_iter;
	m_srcfiles.erase(src_iter);
	emit signalDelPrjFile(path);

	/*Flush is easy*/
	fileTreeClear();
	fileTreeFlush();
	prjSaveXml();
	return;
}

void PrjDock::slotActCaseRootRun()
{
	QString empty = "";
	g_tester->pythonRun(TesterJobPython::PYTHON_TESTDIR, empty);
	return;
}

/*Pop menu add case directory (Pop it in root node)*/
void PrjDock::slotActCaseDirAdd()
{
	QString homedir = QDir::homePath();
	QString getdir = QFileDialog::getExistingDirectory(this, "Add case directory", homedir, QFileDialog::ShowDirsOnly);
	if (getdir == "") {
		return;
	}

	QString cleanpath = QDir::cleanPath(getdir);

	QSet<QString>::iterator casedir_iter;
	casedir_iter = m_casedirs.find(cleanpath);
	if (casedir_iter != m_casedirs.end()) {
		QMessageBox::information(this, "Add directory failed", "Case directory added already");
		return;
	}

	m_casedirs.insert(cleanpath);
	/*Flush is easy*/
	caseTreeClear();
	caseTreeFlush();
	prjSaveXml();
	return;
}

void PrjDock::slotActCaseDirRun()
{
	g_tester->pythonRun(TesterJobPython::PYTHON_TESTDIR, m_menu_string_arg_1);
	return;
}

/*Pop menu remove case directory*/
void PrjDock::slotActCaseDirRemove()
{
	QSet<QString>::iterator dir_iter;
	dir_iter = m_casedirs.find(m_menu_string_arg_1);
	if (dir_iter == m_casedirs.end()) {
		return;
	}
	m_casedirs.erase(dir_iter);
	/*Flush is easy*/
	caseTreeClear();
	caseTreeFlush();
	prjSaveXml();
	return;
}

/*Pop menu new file in case directory (Pop it in case directory node)*/
void PrjDock::slotActCaseFileNew()
{
	/*Get path*/
	CaseNewDlg newcase(m_menu_string_arg_1, this);
	newcase.exec();

	if (newcase.m_result == "") {
		g_mainwin->statusBar()->showMessage("No case file created.", 2000);
		return;
	}

	QString cleanpath = QDir::cleanPath(newcase.m_result);

	QFileInfo fileinfo(cleanpath);
	if (fileinfo.exists()) {
		g_mainwin->statusBar()->showMessage("File " + newcase.m_result + " exist.", 2000);
		return;
	}

	/*Create file*/
	QFile file(cleanpath);
	if (! file.open(QFile::ReadWrite | QFile::Text | QFile::Truncate)) {
		g_mainwin->statusBar()->showMessage("Create file failed.", 2000);
		return;
	}
	file.close();

	/*Open in editor*/
	UEditor *editor = g_mainwin->editGetByPath(cleanpath);
	if (editor == NULL) {
		editor = UEditor::createEditByPath(cleanpath);
		g_mainwin->editAddToMdi(editor);
	}
	g_mainwin->editSetActive(editor);

	int dir_count = m_case_node->childCount();
	int iloop;
	for (iloop = 0; iloop < dir_count; iloop++) {
		QTreeWidgetItem *dir_item = m_case_node->child(iloop);
		NodeInfo dir_nodeinf = dir_item->data(0, Qt::UserRole).value<NodeInfo>();
		UTER_ASSERT(dir_nodeinf.m_nodetype == NodeInfo::NODETYPE_CASEDIR);
		/*Locate case directory*/
		if (dir_nodeinf.m_string_info == m_menu_string_arg_1) {

			NodeInfo nodeinf(NodeInfo::NODETYPE_CASEFILE);
			nodeinf.m_string_info = fileinfo.filePath();

			QTreeWidgetItem *file_item = new QTreeWidgetItem();
			file_item->setText(0, fileinfo.fileName());
			file_item->setToolTip(0, fileinfo.filePath());
			file_item->setIcon(0, QIcon(":/images/test_case.png"));
			file_item->setData(0, Qt::UserRole, QVariant::fromValue(nodeinf));
			dir_item->addChild(file_item);

			dir_item->sortChildren(0, Qt::AscendingOrder);
			dir_item->setExpanded(true);
			file_item->setSelected(true);
			break;
		}
	}

	return;
}

/*Pop menu run case file*/
void PrjDock::slotActCaseFileRun()
{
	g_tester->pythonRun(TesterJobPython::PYTHON_TESTFILE, m_menu_string_arg_2);
	return;
}

/*Pop menu delete case file*/
void PrjDock::slotActCaseFileDelete()
{
	int dir_count = m_case_node->childCount();
	int iloop;
	for (iloop = 0; iloop < dir_count; iloop++) {
		QTreeWidgetItem *dir_item = m_case_node->child(iloop);
		NodeInfo dir_nodeinf = dir_item->data(0, Qt::UserRole).value<NodeInfo>();
		UTER_ASSERT(dir_nodeinf.m_nodetype == NodeInfo::NODETYPE_CASEDIR);

		/*Locate action case directory*/
		if (dir_nodeinf.m_string_info == m_menu_string_arg_1) {
			int file_count = dir_item->childCount();
			int jloop;
			for (jloop = 0; jloop < file_count; jloop++) {
				QTreeWidgetItem *file_item = dir_item->child(jloop);
				NodeInfo file_nodeinf = file_item->data(0, Qt::UserRole).value<NodeInfo>();
				UTER_ASSERT(file_nodeinf.m_nodetype == NodeInfo::NODETYPE_CASEFILE);
				/*Locate action case file*/
				if (file_nodeinf.m_string_info == m_menu_string_arg_2) {
					/*Delete file*/
					if (QFile::remove(file_nodeinf.m_string_info)) {
						dir_item->removeChild(file_item);
						delete file_item;

						/*Close editor*/
						UEditor *editor = g_mainwin->editGetByPath(m_menu_string_arg_2);
						if (editor != NULL) {
							g_mainwin->editClose(editor);
						}
					}
					break;
				}
			}
			break;
		}
	}

	return;
}

/*For toolbar*/
void PrjDock::slotActTestCurrentCase()
{
	QTreeWidgetItem *item = m_tree_root->currentItem();
	NodeInfo nodeinf = item->data(0, Qt::UserRole).value<NodeInfo>();
	switch(nodeinf.m_nodetype)
	{
	case NodeInfo::NODETYPE_CASEFILE:
	{
		g_tester->pythonRun(TesterJobPython::PYTHON_TESTFILE, nodeinf.m_string_info);
		break;
	}
	default:
	{
		break;
	}
	}

	return;
}

/*For toolbar*/
void PrjDock::slotActTestAllCase()
{
	QString empty = "";
	g_tester->pythonRun(TesterJobPython::PYTHON_TESTDIR, empty);
	return;
}

void PrjDock::slotNewProject()
{
	/*Get path*/
	QString dir;
	if (m_project_path == "") {
		dir = QDir::homePath();
	} else {
		QFileInfo fileinfo(m_project_path);
		dir = fileinfo.path();
	}

	PrjNewDlg newprj(dir, this);
	newprj.exec();

	if (newprj.m_result == "") {
		g_mainwin->statusBar()->showMessage("No project file created.", 2000);
		return;
	}

	/*Load project*/
	m_project_path = newprj.m_result;

	QFile xml_file(m_project_path);
	xml_file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
	xml_file.close();

	prjClear();
	prjLoadXml();

	g_ui_main.m_config_project_current_project = m_project_path;
	g_ui_main.configSave();
	return;
}

void PrjDock::slotOpenProject()
{
	/*Get path*/
	QString dir;
	if (m_project_path == "") {
		dir = QDir::homePath();
	} else {
		QFileInfo fileinfo(m_project_path);
		dir = fileinfo.path();
	}

	QString open_filter = "XML (*.xml);;AllFile (*.*)";
	QString getfile = QFileDialog::getOpenFileName(this, "Open files", dir, open_filter);
	if (getfile == "") {
		g_mainwin->statusBar()->showMessage("No file selected.", 2000);
		return;
	}

	m_project_path = getfile;

	prjClear();
	prjLoadXml();

	g_ui_main.m_config_project_current_project = m_project_path;
	g_ui_main.configSave();
	return;
}

QSet<QString> PrjDock::prjFileGet()
{
	return m_srcfiles;
}

/*Load project from file*/
int PrjDock::prjLoadXml(QString path)
{
	if (path == "") {
		path = m_project_path;
	}

	QFile xml_file(path);
	if (! xml_file.open(QIODevice::ReadOnly)) {
		/*Use default configuration*/
		return 0;
	}

	QDomDocument domdoc;
	if (! domdoc.setContent(&xml_file)) {
		xml_file.close();
		return 0;
	}
	xml_file.close();

	/*Load project from xml*/
	QDomElement domuterroot = domdoc.firstChildElement("ProjectOfUter");
	if (domuterroot.isNull()) {
		return 0;
	}

	/*Load targets*/
	QDomElement domtargets = domuterroot.firstChildElement("Targets");
	QDomElement domtarget = domtargets.firstChildElement("Target");
	for (; !domtarget.isNull(); domtarget = domtarget.nextSiblingElement("Target")) {
		Config *config = new Config();

		config->m_name = domtarget.attribute("Name", "");
		if (config->m_name == "") {
			continue;
		}

		if (domtarget.attribute("Alive", "False") == "True") {
			config->m_alive = true;
		}

		QDomElement domcon = domtarget.firstChildElement("Connect");
		if (! domcon.isNull()) {
			QString type_string = domcon.attribute("Type", "");
			if (type_string == "Local") {
				config->m_conn.m_con_type = Config::Con::TYPE_LOCAL;
			} else if (type_string == "Tcp") {
				/*TODO:*/
				UTER_ASSERT(0);
			} else if (type_string == "Console") {
				/*TODO:*/
				UTER_ASSERT(0);
			} else {
				;
			}
		}

		QDomElement domstart = domtarget.firstChildElement("Start");
		if (! domstart.isNull()) {
			QString type_string = domstart.attribute("Type", "");
			if (type_string == "New") {
				config->m_start.m_start_type = Config::Start::TYPE_NEW;
				QDomElement dompath = domstart.firstChildElement("Path");
				config->m_start.m_data_new.m_path = dompath.attribute("Value", "");

				QDomElement domargs = domstart.firstChildElement("Arguments");
				QDomElement domarg = domargs.firstChildElement("Argument");
				for (; !domarg.isNull(); domarg = domarg.nextSiblingElement("Argument")) {
					QString arg = domarg.attribute("Value", "");
					config->m_start.m_data_new.m_arglist.push_back(arg);
				}

				QDomElement domenvs = domstart.firstChildElement("Environments");
				QDomElement domenv = domenvs.firstChildElement("Environment");
				for (; !domenv.isNull(); domenv = domenv.nextSiblingElement("Environment")) {
					QStringList node;
					node.push_back(domenv.attribute("Key", ""));
					node.push_back(domenv.attribute("Value", ""));
					config->m_start.m_data_new.m_envlist.push_back(node);
				}

				if (domenvs.attribute("Replace", "False") == "True") {
					config->m_start.m_data_new.m_envreplace = true;
				} else {
					config->m_start.m_data_new.m_envreplace = false;
				}
			} else if (type_string == "Attach") {
				/*TODO:*/
				UTER_ASSERT(0);
			} else {
				;
			}
		}

		QMap<QString,Config*>::iterator target_iter;
		target_iter = m_target_map.find(config->m_name);
		if (target_iter == m_target_map.end()) {

			m_target_map.insert(config->m_name, config);
			if (config->m_alive) {
				emit signalTargetChange(config);
			}
		}
	}
	targetTreeFlush();

	/*Load project files*/
	QDomElement domsrcfiles = domuterroot.firstChildElement("SourceFile");
	QDomElement domsrcfile = domsrcfiles.firstChildElement("File");
	for (; !domsrcfile.isNull(); domsrcfile = domsrcfile.nextSiblingElement("File")) {
		QString path = domsrcfile.attribute("Path", "");
		if (path == "") {
			continue;
		}

		QSet<QString>::iterator iter = m_srcfiles.find(path);
		if (iter == m_srcfiles.end()) {
			m_srcfiles.insert(path);
			emit signalAddPrjFile(path);
		}
	}
	fileTreeFlush();

	/*Load case dirs*/
	QDomElement domcasedirs = domuterroot.firstChildElement("CaseDir");
	QDomElement domcasedir = domcasedirs.firstChildElement("Dir");
	for (; !domcasedir.isNull(); domcasedir = domcasedir.nextSiblingElement("Dir")) {
		QString path = domcasedir.attribute("Path", "");
		if (path == "") {
			continue;
		}

		QSet<QString>::iterator iter = m_casedirs.find(path);
		if (iter == m_casedirs.end()) {
			m_casedirs.insert(path);
		}
	}
	caseTreeFlush();
	return 0;
}

int PrjDock::prjSaveXml(QString path)
{
	if (path == "") {
		path = m_project_path;
	}

	/*File head*/
	QDomDocument domdoc;

	QDomProcessingInstruction instruction = domdoc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
	domdoc.appendChild(instruction);

	QDomElement domuterroot = domdoc.createElement("ProjectOfUter");
	domdoc.appendChild(domuterroot);
	domuterroot.setAttribute("Version", "1.0");

	/*Save targets*/
	QDomElement domtargets = domdoc.createElement("Targets");
	domuterroot.appendChild(domtargets);
	QMap<QString,Config*>::iterator target_iter;
	for (target_iter = m_target_map.begin(); target_iter != m_target_map.end(); target_iter++) {
		Config *config = target_iter.value();
		QDomElement domtarget = domdoc.createElement("Target");
		domtargets.appendChild(domtarget);
		domtarget.setAttribute("Name", config->m_name);
		if (config->m_alive) {
			domtarget.setAttribute("Alive", "True");
		}

		QDomElement domconn = domdoc.createElement("Connect");
		domtarget.appendChild(domconn);
		if (config->m_conn.m_con_type == Config::Con::TYPE_LOCAL) {
			domconn.setAttribute("Type", "Local");
		} else if (config->m_conn.m_con_type == Config::Con::TYPE_TCP) {
			/*TODO:*/
			UTER_ASSERT(0);
		} else {
			UTER_ASSERT(0);
		}

		QDomElement domstart = domdoc.createElement("Start");
		domtarget.appendChild(domstart);
		if (config->m_start.m_start_type == Config::Start::TYPE_NEW) {
			domstart.setAttribute("Type", "New");
			QDomElement dompath = domdoc.createElement("Path");
			domstart.appendChild(dompath);
			dompath.setAttribute("Value", config->m_start.m_data_new.m_path);

			QDomElement domargs = domdoc.createElement("Arguments");
			domstart.appendChild(domargs);
			QList<QString>::iterator arg_iter;
			for (arg_iter = config->m_start.m_data_new.m_arglist.begin();
				 arg_iter != config->m_start.m_data_new.m_arglist.end(); arg_iter++) {
				QString env_string = *arg_iter;
				QDomElement domarg = domdoc.createElement("Argument");
				domargs.appendChild(domarg);
				domarg.setAttribute("Value", env_string);
			}

			QDomElement domenvs = domdoc.createElement("Environments");
			domstart.appendChild(domenvs);
			QList<QStringList>::iterator env_iter;
			for (env_iter = config->m_start.m_data_new.m_envlist.begin();
				 env_iter != config->m_start.m_data_new.m_envlist.end(); env_iter++) {
				QStringList env_node = *env_iter;
				QDomElement domenv = domdoc.createElement("Environment");
				domenvs.appendChild(domenv);
				domenv.setAttribute("Key", env_node.first());
				domenv.setAttribute("Value", env_node.last());
			}

			if (config->m_start.m_data_new.m_envreplace) {
				domenvs.setAttribute("Replace", "True");
			} else {
				domenvs.setAttribute("Replace", "False");
			}
		} else if (config->m_start.m_start_type == Config::Start::TYPE_ATTACH) {
			/*TODO:*/
			UTER_ASSERT(0);
		} else {
			UTER_ASSERT(0);
		}
	}

	/*Save files*/
	QDomElement domsrcfiles = domdoc.createElement("SourceFile");
	domuterroot.appendChild(domsrcfiles);
	QSet<QString>::iterator srcfile_iter;
	for (srcfile_iter = m_srcfiles.begin(); srcfile_iter != m_srcfiles.end(); srcfile_iter++) {
		QString path = *srcfile_iter;
		QDomElement domsrcfile = domdoc.createElement("File");
		domsrcfiles.appendChild(domsrcfile);
		domsrcfile.setAttribute("Path", path);
	}

	/*Save case dirs*/
	QDomElement domcasedir = domdoc.createElement("CaseDir");
	domuterroot.appendChild(domcasedir);
	QSet<QString>::iterator casedir_iter;
	for (casedir_iter = m_casedirs.begin(); casedir_iter != m_casedirs.end(); casedir_iter++) {
		QDomElement domdir = domdoc.createElement("Dir");
		domcasedir.appendChild(domdir);
		domdir.setAttribute("Path", *casedir_iter);
	}

	/*Save file*/
	QFileInfo fineinfo(path);
	QDir::current().mkdir(fineinfo.path() + QDir::separator());
	QFile xml_file(path);
	if (! xml_file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
		return 0;
	}

	QTextStream out_stream(&xml_file);
	out_stream.setCodec("UTF-8");
	domdoc.save(out_stream, 4, QDomNode::EncodingFromTextStream);
	xml_file.close();
	return 0;
}

int PrjDock::prjClear()
{
	targetTreeClear();
	fileTreeClear();
	caseTreeClear();

	QMap<QString,Config*>::iterator target_iter;
	for (target_iter = m_target_map.begin(); target_iter != m_target_map.end(); /*none*/) {
		Config *config = target_iter.value();
		delete config;
		m_target_map.erase(target_iter++);
	}
	emit signalTargetChange(NULL);

	QSet<QString>::iterator srcfile_iter;
	for (srcfile_iter = m_srcfiles.begin(); srcfile_iter != m_srcfiles.end(); /*none*/) {
		QString path = *srcfile_iter;
		emit signalDelPrjFile(path);
		m_srcfiles.erase(srcfile_iter++);
	}

	QSet<QString>::iterator casedir_iter;
	for (casedir_iter = m_casedirs.begin(); casedir_iter != m_casedirs.end(); /*none*/) {
		m_casedirs.erase(casedir_iter++);
	}

	return 0;
}

/*Remove & delete all child*/
static int clear_node_child(QTreeWidgetItem *item)
{
	while (item->childCount() != 0) {
		QTreeWidgetItem *child_item = item->child(0);
		clear_node_child(child_item);

		item->removeChild(child_item);
		delete child_item;
	}

	return 0;
}

int PrjDock::targetTreeFlush()
{
	QMap<QString,Config*>::iterator iter;
	for (iter = m_target_map.begin(); iter != m_target_map.end(); iter++) {
		Config *targetconfig = iter.value();

		NodeInfo nodeinf(NodeInfo::NODETYPE_TARGETFILE);
		nodeinf.m_string_info = targetconfig->m_name;

		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setText(0, targetconfig->m_name);
		item->setToolTip(0, targetconfig->m_name);
		item->setIcon(0, QIcon(":/images/target.png"));
		item->setData(0, Qt::UserRole, QVariant::fromValue(nodeinf));
		if (targetconfig->m_alive) {
			item->setCheckState(0, Qt::Checked);
		} else {
			item->setCheckState(0, Qt::Unchecked);
		}

		m_target_node->addChild(item);
	}
	m_target_node->sortChildren(0, Qt::AscendingOrder);
	return 0;
}

int PrjDock::targetTreeClear()
{
	clear_node_child(m_target_node);
	return 0;
}

int PrjDock::fileTreeFlush()
{
	QSet<QString>::iterator iter;
	for (iter = m_srcfiles.begin(); iter != m_srcfiles.end(); iter++) {
		QFileInfo fileinfo(*iter);

		NodeInfo nodeinf(NodeInfo::NODETYPE_PRJFILE);
		nodeinf.m_string_info = fileinfo.filePath();

		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setText(0, fileinfo.fileName());
		item->setToolTip(0, fileinfo.filePath());
		item->setIcon(0, QIcon(":/images/elmt_file.png"));
		item->setData(0, Qt::UserRole, QVariant::fromValue(nodeinf));
		m_file_node->addChild(item);

		if (! fileinfo.exists()) {
			QFont font = item->font(0);
			font.setUnderline(true);
			item->setFont(0, font);

			item->setToolTip(0, "No such file:" + fileinfo.filePath());
		}
	}
	m_file_node->sortChildren(0, Qt::AscendingOrder);
	return 0;
}

int PrjDock::fileTreeClear()
{
	clear_node_child(m_file_node);
	return 0;
}

int PrjDock::caseTreeFlush()
{
	QSet<QString>::iterator dir_iter;
	for (dir_iter = m_casedirs.begin(); dir_iter != m_casedirs.end(); dir_iter++) {
		QDir dir(*dir_iter);

		NodeInfo nodeinf(NodeInfo::NODETYPE_CASEDIR);
		nodeinf.m_string_info = dir.path();

		QTreeWidgetItem *dir_item = new QTreeWidgetItem();
		dir_item->setText(0, dir.dirName());
		dir_item->setToolTip(0, dir.path());
		dir_item->setIcon(0, QIcon(":/images/file_open.png"));
		dir_item->setData(0, Qt::UserRole, QVariant::fromValue(nodeinf));
		m_case_node->addChild(dir_item);

		dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
		QFileInfoList file_info_list = dir.entryInfoList();
		QFileInfoList::iterator file_iter;
		for (file_iter = file_info_list.begin(); file_iter != file_info_list.end(); file_iter++) {
			QFileInfo fileinfo = *file_iter;
			if (fileinfo.suffix() != "py") {
				continue;
			}

			NodeInfo nodeinf(NodeInfo::NODETYPE_CASEFILE);
			nodeinf.m_string_info = fileinfo.filePath();

			QTreeWidgetItem *file_item = new QTreeWidgetItem();
			file_item->setText(0, fileinfo.fileName());
			file_item->setToolTip(0, fileinfo.filePath());
			file_item->setIcon(0, QIcon(":/images/test_case.png"));
			file_item->setData(0, Qt::UserRole, QVariant::fromValue(nodeinf));
			dir_item->addChild(file_item);
		}

		dir_item->sortChildren(0, Qt::AscendingOrder);
		dir_item->setExpanded(true);
	}
	m_case_node->sortChildren(0, Qt::AscendingOrder);
	m_case_node->setExpanded(true);
	return 0;
}

int PrjDock::caseTreeClear()
{
	clear_node_child(m_case_node);
	return 0;
}

} /*namespace PrjUi*/

Q_DECLARE_METATYPE(PrjUi::NodeInfo);
