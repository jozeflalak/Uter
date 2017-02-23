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
#include <tester.hxx>
#include <ui_mainwin.hxx>

namespace TesterUi {

StatusBar::StatusBar(QWidget *parent)
: QWidget(parent)
{
	QMetaObject::Connection conn;

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);
	setLayout(layout);

	m_python_btn = new QPushButton(this);
	m_python_btn->setEnabled(false);
	m_python_btn->setText("Python");
	layout->addWidget(m_python_btn);

	conn = connect(g_tester, SIGNAL(signalStatusChanged(int, int)),
			       this, SLOT(slotStatusChanged(int, int)));
	UTER_ASSERT(conn != NULL);

	slotStatusChanged(-1, true);
	return;
}

void StatusBar::slotStatusChanged(int type/*no use*/, int status)
{
	if (status) {
		m_python_btn->setStyleSheet("background-color:green");
		m_python_btn->setToolTip("Python is ready.");
	} else {
		m_python_btn->setStyleSheet("background-color:red");
		m_python_btn->setToolTip("Python is busy.");
	}
	return;
}

TesterDlg::TesterDlg(QWidget *parent)
: QDialog(parent)
{
	UiStatusBarSetup *statusbar = g_mainwin->statusBarSetup(UiStatusBarSetup::SEQ_TESTER, "PythonStatus");

	m_status_bar = new StatusBar(this);
	statusbar->addWidget(m_status_bar);
	return;
}

} /*namespace TesterUi*/
