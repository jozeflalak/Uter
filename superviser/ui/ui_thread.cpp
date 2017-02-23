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

namespace ThrdUi {

/*Set text ant tip,in function at file on module*/
static int setupStackDesc(Stack *stacknode, QTreeWidgetItem *item, TargetMod *mod,
		            unsigned long long global_addr, unsigned long long offset_addr)
{
	QString desc = g_target->arch0xAddr(global_addr);
	QString tips;

	DwfCu *cu = mod->dwarfGetCuByAddrIn(offset_addr);
	if (cu == NULL) {
		/*Set item from binary information*/
		tips = "Mod:" + mod->m_remote_path;

		BfdSym *sym_near = NULL;
		QMap<unsigned long long, BfdSym*>::iterator iter;
		for (iter = mod->m_bfd_dynsym_addr_map.begin();
			 iter != mod->m_bfd_dynsym_addr_map.end(); iter++) {
			BfdSym *sym = iter.value();
			/*The highest symbol less than offset address*/
			if (sym->m_addr > offset_addr) {
				continue;
			}
			if (sym_near == NULL) {
				sym_near = sym;
			}
			if (sym->m_addr > sym_near->m_addr) {
				sym_near = sym;
			}
		}
		if (sym_near != NULL) {
			/*In function*/
			desc += QString(" in (") + sym_near->m_name + QString(") ");
		}

		/*On module*/
		QFileInfo file_info(mod->m_remote_path);
		desc += QString(" on (") + file_info.fileName() + QString(") ");
	} else {
		/*Set item from debug information*/
		tips += "Mod:" + mod->m_remote_path + " Cu:" + cu->m_path;

		QList<DwfFun*>::iterator iter_fun;
		for (iter_fun = cu->m_funs.begin(); iter_fun != cu->m_funs.end(); iter_fun++) {
			DwfFun *fun = *iter_fun;
			if (fun->m_add_range.check(offset_addr)) {
				break;
			}
		}
		if (iter_fun != cu->m_funs.end()) {
			DwfFun *fun = *iter_fun;
			/*In function*/
			desc += " in (" + fun->m_name + ")";
		}
		QString file;
		unsigned int line;
		int result = cu->getFileLineByAddrIn(offset_addr, file, line);
		if (result == 0) {
			stacknode->m_file = file;
			stacknode->m_lineno = line;
			/*At file if have*/
			QFileInfo file_info(file);
			desc += QString(" at (") + file_info.fileName() + ":" + QString::asprintf("%d", line) + ")";
		}
		/*On module*/
		QFileInfo file_info(mod->m_remote_path);
		desc += QString(" on (") + file_info.fileName() + QString(")");
	}

	item->setText(0, desc);
	item->setToolTip(0, tips);
	return 0;
}

ThrdDock::ThrdDock()
:WidgetWithDock(SEQ_THRD, Qt::BottomDockWidgetArea, "Thread")
{
	QMetaObject::Connection conn;

	QHBoxLayout *layout = new QHBoxLayout(this);
	setLayout(layout);

	m_treeview = new QTreeWidget(this);
	layout->addWidget(m_treeview);
	m_treeview->setColumnCount(1);
	m_treeview->setHeaderHidden(true);

	m_process = NULL;
	m_select_pid = 0;

	conn = connect(m_treeview, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
			       this, SLOT(slotTreeItemClicked(QTreeWidgetItem*, int)));
	UTER_ASSERT(conn != NULL);

	g_mainwin->dockSetup(this);
	return;
}

void ThrdDock::slotTreeItemClicked(QTreeWidgetItem *item, int column)
{
	Node *node = (Node *)item->data(0, Qt::UserRole).value<void *>();
	UTER_ASSERT(node != NULL);
	Stack *show_stack = NULL;

	switch(node->m_type)
	{
	case Node::TYPE_STACK:
	{
		Stack *stack = (Stack *)node;
		/*Get thread*/
		QTreeWidgetItem *thrd_item = item->parent();
		Thrd *thrd = (Thrd *)thrd_item->data(0, Qt::UserRole).value<void*>();
		UTER_ASSERT(thrd != NULL);
		UTER_ASSERT(thrd->m_type == Node::TYPE_THRD);

		/*Update variable information*/
		g_mainwin->m_var_win->updateView(thrd, stack);
		m_select_pid = thrd->m_pid;
		show_stack = stack;
		break;
	}
	case Node::TYPE_THRD:
	{
		Thrd *thrd = (Thrd *)node;
		/*Get first stack*/
		if (item->childCount() == 0) {
			g_mainwin->m_var_win->clear();
		} else {
			QTreeWidgetItem *stack_item = item->child(0);
			Stack *stack = (Stack *)stack_item->data(0, Qt::UserRole).value<void*>();
			UTER_ASSERT(stack != NULL);
			UTER_ASSERT(stack->m_type == Node::TYPE_STACK);
			g_mainwin->m_var_win->updateView(thrd, stack);
			m_select_pid = thrd->m_pid;
			show_stack = stack;
		}

		break;
	}
	case Node::TYPE_PROC:
	{
		break;
	}
	default:
	{
		UTER_ASSERT(0);
	}
	}

	if (show_stack != NULL && show_stack->m_file != "") {
		/*Get edit handle of file*/
		UEditor *edit = g_mainwin->editGetByPath(show_stack->m_file);
		if (edit == NULL) {
			edit = UEditor::createEditByPath(show_stack->m_file);
			g_mainwin->editAddToMdi(edit);
			g_mainwin->editSetActive(edit);
		}
		/*Show and select line, line number begin from 0 in edit*/
		edit->ensureLineVisible(show_stack->m_lineno - 1);
		edit->selectLine(show_stack->m_lineno - 1);
	}

	return;
}

/* Process
 *    |
 *    |______thread
 *    |         |___stack
 *    |         |___stack
 *    |         |___stack
 *    |
 *    |______thread
 *              |___stack
 *              |___stack
 * */
int ThrdDock::updateOnStop()
{
	clear();

	/*phase 1:Set up process node*/
	TargetThread *main_thread = g_target->threadGetById(g_target->m_run_pid);
	if (main_thread == NULL) {
		/*Some wrong*/
		return 0;
	}

	Proc *uiproc = new Proc();
	uiproc->m_pid = main_thread->m_pid;
	uiproc->m_desc = main_thread->m_name;

	/*Process item*/
	QTreeWidgetItem *process_item = new QTreeWidgetItem();
	process_item->setText(0, main_thread->m_name + QString::asprintf(" (%u)", g_target->m_run_pid));
	process_item->setData(0, Qt::UserRole, QVariant::fromValue((void *)uiproc));
	m_treeview->addTopLevelItem(process_item);
	m_process = uiproc;

	/*phase 2:Check the thread which last selected*/
	TargetThread *thread = g_target->threadGetById(m_select_pid);
	if (thread == NULL) {
		/*Set main thread or event thread for select pid*/
		m_select_pid = g_target->m_run_pid;
	}
	Thrd *uithread_selected = NULL;

	/*phase 3:Setup threads*/
	QMap<unsigned int, TargetThread*>::iterator thrd_iter;
	for (thrd_iter = g_target->m_thread_set.begin();
		 thrd_iter != g_target->m_thread_set.end(); thrd_iter++) {
		TargetThread *thread = thrd_iter.value();

		Thrd *uithread = new Thrd();
		uithread->m_pid = thread->m_pid;
		uithread->m_desc = thread->m_name;

		/*Thread item*/
		QTreeWidgetItem *thread_item = new QTreeWidgetItem();
		thread_item->setText(0, QString::asprintf("%u", thread->m_pid) + " (" + thread->m_name + ")");
		thread_item->setData(0, Qt::UserRole, QVariant::fromValue((void *)uithread));
		process_item->addChild(thread_item);
		m_threads_map.insert(uithread->m_pid, uithread);

		/*Get stack information for thread*/
		StackInfo *stackinfo = g_target->pidStackGet(thread->m_pid);
		if (stackinfo != NULL) {
			QList<StackInfo::Item*>::iterator iter;
			for (iter = stackinfo->m_stack_list.begin(); iter != stackinfo->m_stack_list.end(); iter++) {
				StackInfo::Item *stackitem = *iter;
				TargetMod *mod = g_target->modGetByAddrIn(stackitem->m_addr);
				if (mod == NULL) {
					goto finish;
				}
				unsigned long long offset_addr = stackitem->m_addr - mod->m_add_mkup;
				DwfFrameFde *fde = mod->dwarfGetFdeByAddrIn(offset_addr);
				if (fde == NULL) {
					goto finish;
				}
				DwfRegTable *regtable = fde->getRegTabByAddrIn(offset_addr);
				if (regtable == NULL) {
					goto finish;
				}

				Stack *stacknode = new Stack();
				stacknode->m_addr = stackitem->m_addr;
				stacknode->m_stack = stackitem->m_stack;
				stacknode->m_cfa = stackitem->m_cfa;
				stacknode->m_regtab = regtable;

				/*Stack item*/
				QTreeWidgetItem *stack_item = new QTreeWidgetItem();
				setupStackDesc(stacknode, stack_item, mod, stacknode->m_addr, offset_addr);
				stack_item->setData(0, Qt::UserRole, QVariant::fromValue((void *)stacknode));
				thread_item->addChild(stack_item);
				uithread->m_stack_list.push_back(stacknode);
			}
		finish:
			delete stackinfo;
		}

		if (m_select_pid == thread->m_pid) {
			/*Expand if selected*/
			uithread_selected = uithread;
			thread_item->setExpanded(true);
			thread_item->setSelected(true);
		}
	}
	process_item->setExpanded(true);

	/*phase 4: do select*/
	if (uithread_selected != NULL) {
		if (uithread_selected->m_stack_list.empty()) {
			g_mainwin->m_var_win->clear();
		} else {
			Stack *stacknode = uithread_selected->m_stack_list.front();
			g_mainwin->m_var_win->updateView(uithread_selected, stacknode);
		}
	}
	return 0;
}

int ThrdDock::updateOnShut()
{
	clear();
	return 0;
}

int ThrdDock::clear()
{
	m_treeview->clear();

	if (m_process != NULL) {
		delete m_process;
		m_process = NULL;
	}
	QMap<unsigned int, Thrd*>::iterator iter;
	for (iter = m_threads_map.begin(); iter != m_threads_map.end(); /**/) {
		Thrd *thrd = iter.value();
		delete thrd;
		m_threads_map.erase(iter++);
	}

	g_mainwin->m_var_win->clear();
	return 0;
}

} /*namespace NsThrd*/
