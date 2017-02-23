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

#ifndef SRC_UI_MIN_H_
#define SRC_UI_MIN_H_

#include <QtCore/QObject>
#include <QtGui/QtGui>
#include <QtWidgets/QApplication>

class UiMain: public QObject {
	Q_OBJECT
signals:
	/*Output signal*/
	void signalUterLog(int, QString);				/*log level: 1 log, 2 debug*/
	void signalPythonOutput(int, QString);			/*output type: 1 stdout, 2 stderr*/
	void signalTargetOutput(int, QString);			/*output type: 1 stdout, 2 stderr*/
	void signalMsgToStatBar(unsigned int, QString);	/*set string to status bar with millisecond*/
public:
	static void setWaitCursor()
	{
		QApplication::setOverrideCursor(Qt::WaitCursor);
		return;
	}
	static void restoreCursor()
	{
		QApplication::restoreOverrideCursor();
		return;
	}

	UiMain()
	{
		m_config_edit_view_line_number = false;
		return;
	}

	/*Global configuration*/
	int configLoad();
	int configSave();
	bool m_config_edit_view_line_number;
	QString m_config_project_current_project;
};

extern UiMain g_ui_main;

#endif /*SRC_UI_MIN_H_*/
