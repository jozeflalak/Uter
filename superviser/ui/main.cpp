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
#include <QTime>

#include <target.hxx>
#include <tester.hxx>
#include <ui_mainwin.hxx>
#include <main.hxx>

int register_type()
{
	qRegisterMetaType<QAbstractSocket::SocketState>("");
	return 0;
}

QProcess *g_ptracer_process = NULL;
int start_ptracer(QString app_dir)
{
	const int try_times = 10;
	const unsigned short port_begin = 1024;
	int iloop;
	for (iloop = 0; iloop < try_times; iloop++) {
		QTime time = QTime::currentTime();
		qsrand(time.second()*1000 + time.msec());
		unsigned int rand = (unsigned int)qrand();
		unsigned short port = port_begin + rand % 512;

		QProcess *process = new QProcess();
    	QString ptracer_path = app_dir + QDir::separator() + "ptracer_i386.exe";
    	QStringList arguments;
    	arguments << "-i" << "127.0.0.1";
    	arguments << "-p" << QString::asprintf("%d", port);
    	arguments << "-l" << "1";

    	QString log_path = app_dir + "/../log/";
    	QDir::current().mkdir(log_path);
    	process->setStandardOutputFile(log_path + "ptracer_stdout.txt");
    	process->setStandardErrorFile(log_path + "ptracer_stderr.txt");

    	process->start(ptracer_path, arguments);
		//process->startDetached(ptracer_path, arguments);

    	if (! process->waitForStarted()) {
    		delete process;
    		continue;
    	}

    	const int wait_time = 1000;
    	if (process->waitForFinished(wait_time)) {
    		delete process;
    		continue;
    	}

    	g_ptracer_process = process;
    	break;
    }
    if (g_ptracer_process == NULL) {
    	printf("Ptracer start failed.");
    	return -1;
    }

    //g_ptracer_process->closeWriteChannel();
    //g_ptracer_process->closeReadChannel(QProcess::StandardOutput);
    //g_ptracer_process->closeReadChannel(QProcess::StandardError);
	return 0;
}

int main(int argc, char *argv[])
{
	/*Register data for signal slots*/
	register_type();

	/*Set main thread name*/
	prctl(PR_SET_NAME, (unsigned long)"Target&UI");

	QApplication::addLibraryPath("../lib/Qt5.5.0/plugins");

	/*Create app instance*/
	QApplication app(argc, argv);

	QString app_dir = QApplication::applicationDirPath();
	bool bresult = QResource::registerResource(app_dir + "/Uter.rcc");
	UTER_ASSERT(bresult == true);
    QDir::setCurrent(app_dir);

    /*Set application logo*/
    app.setWindowIcon(QIcon(":/images/app_logo.png"));

    /*Load hibernate configuration*/
    g_ui_main.configLoad();

    /*Local ptracer*/
    start_ptracer(app_dir);

	/*Target object*/
	g_target = new Target();

	/*Tester object with thread*/
	g_tester = new Tester();
	g_tester->start();

	/*Main window object*/
    g_mainwin = new MainWindow();
    g_mainwin->moduleInit();
    g_mainwin->show();

    /*Main loop*/
    return app.exec();
}

UiMain g_ui_main;
int uter_log_level = 0;

QString config_file_name = ".uter/uter_config.xml";

int UiMain::configLoad()
{
	QString root_config_path = QDir::homePath() + QDir::separator() + config_file_name;

	QFile xml_file(root_config_path);
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

	QDomElement domuterconfig = domdoc.firstChildElement("ConfigOfUter");
	if (domuterconfig.isNull()) {
		return 0;
	}

	QDomElement domeditor = domuterconfig.firstChildElement("Edit");
	if (! domeditor.isNull()) {
		QDomElement domlineno = domeditor.firstChildElement("ViewLineNumber");
		if (! domlineno.isNull()) {
			QString value = domlineno.attribute("Enable", "False");
			if (value == "True") {
				m_config_edit_view_line_number = true;
			} else {
				m_config_edit_view_line_number = false;
			}
		}
	}

	QDomElement domproject = domuterconfig.firstChildElement("Project");
	if (! domproject.isNull()) {
		QDomElement domlastprj = domproject.firstChildElement("LastProject");
		if (! domlastprj.isNull()) {
			QString value = domlastprj.attribute("Path", "");
			m_config_project_current_project = value;
		}
	}

	return 0;
}

int UiMain::configSave()
{
	QDomDocument domdoc;

	QDomProcessingInstruction instruction = domdoc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
	domdoc.appendChild(instruction);

	QDomElement domuterconfig = domdoc.createElement("ConfigOfUter");
	domdoc.appendChild(domuterconfig);
	domuterconfig.setAttribute("Version", "1.0");

	QDomElement domeditor = domdoc.createElement("Edit");
	domuterconfig.appendChild(domeditor);

	QDomElement domlineno = domdoc.createElement("ViewLineNumber");
	domeditor.appendChild(domlineno);
	if (m_config_edit_view_line_number) {
		domlineno.setAttribute("Enable", "True");
	} else {
		domlineno.setAttribute("Enable", "False");
	}

	QDomElement domproject = domdoc.createElement("Project");
	domuterconfig.appendChild(domproject);

	QDomElement domlastprj = domdoc.createElement("LastProject");
	domproject.appendChild(domlastprj);
	domlastprj.setAttribute("Path", m_config_project_current_project);

	QString root_config_path = QDir::homePath() + QDir::separator() + config_file_name;
	QFileInfo fineinfo(root_config_path);
	QDir::current().mkdir(fineinfo.path() + QDir::separator());
	QFile xml_file(root_config_path);
	if (! xml_file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
		return 0;
	}

	QTextStream out_stream(&xml_file);
	out_stream.setCodec("UTF-8");
	domdoc.save(out_stream, 4, QDomNode::EncodingFromTextStream);
	xml_file.close();
	return 0;
}
