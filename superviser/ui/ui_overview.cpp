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

#include <ui_mainwin.hxx>

namespace OverViewUi {

OverViewDock::OverViewDock()
:WidgetWithDock(SEQ_OVERVIEW, Qt::RightDockWidgetArea, "Overview")
{
	QMetaObject::Connection conn;

	/* Out layout
	 * -----------------------
	 *        Tool
	 * -----------------------
	 *        Tree view
	 * -----------------------*/
	QVBoxLayout *out_layout = new QVBoxLayout(this);
	setLayout(out_layout);

	/* Tool layout */
	QHBoxLayout *tool_layout = new QHBoxLayout(this);
	out_layout->addLayout(tool_layout);

	/* Tree layout*/
	m_treeview = new QTreeWidget();
	out_layout->addWidget(m_treeview);
	m_treeview->setColumnCount(1);
	m_treeview->setHeaderHidden(true);

	conn = connect(g_mainwin->m_center_mdi, SIGNAL(signalActiveChange()), this, SLOT(slotActiveChange()));
	UTER_ASSERT(conn != NULL);

	g_mainwin->dockSetup(this);
	return;
}

void OverViewDock::slotActiveChange()
{
	UEditor *edit = g_mainwin->editGetActive();
	if (edit == NULL) {
		hide();
		return;
	}

	flush(edit);
	return;
}

void OverViewDock::clear()
{
	m_treeview->clear();
	return;
}

void OverViewDock::hide()
{
	clear();
	m_dock->setVisible(false);
	return;
}

void OverViewDock::flushPython(UEditor *edit)
{
	UTER_ASSERT(edit != NULL);

	/*TODO: Make tree node, detect and mark error on editor.*/
	return;
}

void OverViewDock::flush(UEditor *edit)
{
	UTER_ASSERT(edit != NULL);

	clear();
	m_dock->setVisible(true);

	switch(edit->m_type)
	{
	case EditorBase::FILETYPE_PY:
	{
		flushPython(edit);
		break;
	}
	default:
	{
		return;
	}
	}

	return;
}

}
