
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

#ifndef SRC_UI_OVERVIEW_HXX_
#define SRC_UI_OVERVIEW_HXX_

#include <ui_public.hxx>
#include <ui_editor.hxx>

namespace OverViewUi {

class OverViewDock: public WidgetWithDock {
	Q_OBJECT
private slots:
	void slotActiveChange();
public:
	OverViewDock();
	~OverViewDock()
	{
		clear();
		return;
	}
	void flush(UEditor *edit);
	void hide();
private:
	void clear();
	void flushPython(UEditor *edit);

	QTreeWidget *m_treeview;
};

}

#endif /*SRC_UI_OVERVIEW_HXX_*/
