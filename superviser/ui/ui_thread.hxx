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

#ifndef SRC_UI_THREAD_HXX_
#define SRC_UI_THREAD_HXX_

#include <ui_public.hxx>

namespace ThrdUi {

class Node {
public:
	enum NodeType {
		TYPE_STACK = 0,
		TYPE_THRD,
		TYPE_PROC,
	};
	NodeType m_type;
};

/*Stack node in frame*/
class Stack: public Node {
public:
	Stack()
	{
		m_type = TYPE_STACK;

		m_addr = 0;
		m_stack = 0;
		m_cfa = 0;
		m_regtab = NULL;

		m_file = "";
		m_lineno = 0;
		return;
	}

	/* Thread execute in function, it is stop at address which in function,
	 * or call other function, is this case, is is will execute next instruction after call*/
	unsigned long long m_addr;		/*Current address stop at or after call*/
	unsigned long long m_stack;		/*Stack register value on stack which create by the function call*/
	unsigned long long m_cfa;		/*Cfa register value on stack which create by the function call*/
	DwfRegTable *m_regtab;			/*Register table on current address*/

	QString m_file;					/*Source file on current address*/
	unsigned int m_lineno;			/*Source line on current address*/
};

/*Thread node*/
class Thrd: public Node {
public:
	Thrd()
	{
		m_type = TYPE_THRD;

		m_pid = 0;
		m_desc = "";
		return;
	}
	~Thrd()
	{
		QList<Stack*>::iterator iter;
		for (iter = m_stack_list.begin(); iter != m_stack_list.end(); /*none*/) {
			Stack *stacknode = *iter;
			delete stacknode;
			m_stack_list.erase(iter++);
		}
		return;
	}

	unsigned int m_pid;					/*Thread pid*/
	QString m_desc;						/*Thread name*/
	QList<Stack*> m_stack_list;			/*Thread stack frame*/
};

class Proc: public Node {
public:
	Proc()
	{
		m_type = TYPE_PROC;

		m_pid = 0;
		m_desc = "";
		return;
	}
	unsigned int m_pid;
	QString m_desc;
};

class ThrdDock: public WidgetWithDock {
	Q_OBJECT
public slots:
	void slotTreeItemClicked(QTreeWidgetItem *item, int column);
public:
	ThrdDock();
	~ThrdDock()
	{
		clear();
		return;
	}
	int updateOnStop();
	int updateOnShut();
private:
	int clear();
	QTreeWidget *m_treeview;
	Proc *m_process;
	QMap<unsigned int, Thrd*> m_threads_map;
	unsigned int m_select_pid;
};

} /*namespace NsThrd*/
#endif /*SRC_UI_THREAD_HXX_*/
