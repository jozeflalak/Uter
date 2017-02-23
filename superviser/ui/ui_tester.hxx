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

#ifndef SRC_UI_TESTER_HXX_
#define SRC_UI_TESTER_HXX_

#include <ui_public.hxx>

namespace TesterUi {

/*Show tester status on status bar, but just have Python interpreter status now*/
class StatusBar: public QWidget {
	Q_OBJECT
public:
	StatusBar(QWidget *parent = 0);
private slots:
	void slotStatusChanged(int, int);		/*Reply tester status with type(no use) and status*/
private:
	QPushButton *m_python_btn;				/*Python interpreter, button is enough*/
};

/*View of tester on ui, but just have status bar now*/
class TesterDlg: public QDialog {
	Q_OBJECT
public:
	TesterDlg(QWidget *parent = 0);
private:
	StatusBar *m_status_bar;	/*Tester status bar*/
};

} /*namespace TesterUi*/
#endif /*SRC_UI_TESTER_HXX_*/
