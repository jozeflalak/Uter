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

#ifndef SRC_UI_MAP_HXX_
#define SRC_UI_MAP_HXX_

#include <ui_public.hxx>

namespace MapUi {

/*Just update on stop*/
class MapDock: public WidgetWithDock {
	Q_OBJECT
private slots:
	void slotCellDoubleClicked(int, int);
public:
	MapDock();
	~MapDock()
	{
		clear();
		return;
	}
	int updateOnStop();
	int updateOnShut();
private:
	int clear();
	QTableWidget *m_table;				/*Map table*/
};

} /*namespace MapUi*/
#endif /*SRC_UI_MAP_HXX_*/
