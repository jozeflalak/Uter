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

#ifndef SRC_UI_SUPPLY_HXX_
#define SRC_UI_SUPPLY_HXX_

#include <ui_public.hxx>

namespace SupplyUi {

class AboutDlg: public QDialog {
	Q_OBJECT
private slots:
public:
	AboutDlg(QWidget *parent = 0);
private:

};

/*Window and help menu*/
class SupplyDlg: public QDialog {
	Q_OBJECT
private slots:
	void slotActionHelp();
	void slotActionAbout();
public:
	SupplyDlg(QWidget *parent = 0);
private:
	QAction *m_preferences;
};

} /*namespace SupplyUi*/
#endif /*SRC_UI_SUPPLY_HXX_*/
