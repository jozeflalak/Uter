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

#ifndef SRC_UI_VARIABLE_HXX_
#define SRC_UI_VARIABLE_HXX_

#include <ui_public.hxx>
#include <ui_thread.hxx>

namespace VarUi {

/*Not support binary*/
enum DataFormat {
	FMT_OCT = 0,/*8:*/
	FMT_HEX,	/*16:*/
	FMT_DEC,	/*10:*/
	FMT_DFT,	/*default:*/
	FMT_MAX,
};

/*
 * Show variable detail with different type of node on three, node information include name,type,value.
 * node with some attribute, see follow table:
 * data_type               type_show             value_show               expand                     others
 * ------------------------------------------------------------------------------------------------------
 * base type               char,int,...          Yes(value)               No                         NA
 * named enum              enum xxxx             Yes(enum value)          No                         Show first equal enum value
 * no name enum            enum {...}            Yes(enum value)          No                         Show first equal enun value
 * named struct            struct xxxx           No({...})                Yes(expand member)
 * no name struct          struct {...}          No({...})                Yes(expand member)
 * named union             union xxxx            No({...})                Yes(expand member)
 * no name union           union {...}           No({...})                Yes(expand member)
 * base type point         char *,int *,...      Yes(address)             Yes(expand *name as base type)
 * named enum point        enum xxxx *           Yes(address)             Yes(expand *name as enum)
 * no name enum point      enum {...} *          Yes(address)             Yes(expand *name as enum)
 * named struct point      struct xxxx *         Yes(address)             Yes(expand member of struct point to)
 * no name struct point    struct {...} *        Yes(address)             Yes(expand member of struct point to)
 * named union point       union xxxx *          Yes(address)             Yes(expand member of union point to)
 * no name union point     union {...} *         Yes(address)             Yes(expand member of union point to)
 * void point              void *                Yes(address)             No
 * array                   int [4],...           Yes(address of array)    Yes(Sub array until final type)
 * function point          int (*)(int,int)      Yes(address)             No
 * point by point          int **,...            Yes(address)             Yes(Sub point until final type)
 * */

/*parent class of variable node*/
class Node {
public:
	enum NodeStat {
		STAT_DISABLE = 0,				/*Step 1: Create new template node*/
		STAT_ENABLE,					/*Step 2: Enable node by variable address*/
		STAT_LOADED,					/*Step 3: Load information from memory*/
		STAT_MAX,
	};

	Node()
	{
		m_item = NULL;
		m_stubitem = NULL;
		m_type = NULL;
		m_ext_type = NULL;
		m_desc_before = "";
		m_desc_after = "";
		m_name = "";

		m_editable = false;
		m_expandable = false;

		m_stat = STAT_DISABLE;
		m_addr = 0;

		m_settingnow = false;
		return;
	}
	virtual ~Node()
	{
		/* As parent, do removeChild child item of child, but not delete child item,
		 * child item delete by child*/
		QList<Node*>::iterator iter;
		for (iter = m_child_list.begin(); iter != m_child_list.end(); /*none*/) {
			Node *varnode = *iter;
			m_item->removeChild(varnode->m_item);
			delete varnode;
			m_child_list.erase(iter++);
		}

		/*delete sub item*/
		if (m_stubitem != NULL) {
			m_item->removeChild(m_stubitem);
			delete m_stubitem;
			m_stubitem = NULL;
		}

		/*Delete self item*/
		delete m_item;
		m_item = NULL;
		return;
	}

	QTreeWidgetItem *m_item;			/*Tree node item, new/free by self, parent do addChild/removeChild*/
	QTreeWidgetItem *m_stubitem;		/*Keep show "+" otherwise show "-" before expand for expandable node*/
	DwfType *m_type;					/*Variable data type*/
	DwfType *m_ext_type;				/*Use for struct->DwfTypeStMem/union->DwfTypeUnMem*/
	QString m_desc_before;				/*Variable type description*/
	QString m_desc_after;				/*Variable type description*/
	QString m_name;						/*Variable name*/
	bool m_expandable;					/*Expandable, char/int/long == false, struct/union == true*/
	bool m_editable;					/*Editable, char/int/long == true, struct/union == false*/
	bool m_settingnow;					/*Setting by system now*/
	NodeStat m_stat;					/*Node status*/
	unsigned long long m_addr;			/*Variable address*/
	QList<Node*> m_child_list;			/*Child list when expanded*/

	/* Make node from DWARF type with variable name and array index, return node is a template,
	 * invalid node because without address information, need set address and flush as follow steps*/
	static Node *makeVarNode(DwfType *input_type, QString &name, unsigned long long array_index = 0);
	int enableNode(unsigned long long addr);	/*Enable node by set address, need flush node as follow steps*/
	int flushNode();							/*Flush node after enable with valid address*/
	int expandNode();							/*Expand child with node type, need flush self after*/
	QString getInputFromView();					/*Get user input after change*/
	int setValueToView();						/*Set variable to view*/
	virtual int expandType();					/*Expand type*/
	virtual int flushData();					/*Flush data*/
	virtual QString getValue(DataFormat fmt);	/*Get from memory as format*/
	virtual int setValue(DataFormat fmt);		/*Set to memory as format*/
};

class NodeBase: public Node {
public:
	NodeBase(DwfType *type)
	{
		UTER_ASSERT(type != NULL);
		UTER_ASSERT(type->m_data_type == DwfType::TYPE_BASE);
		m_type = type;

		m_editable = true;
		m_expandable = false;

		m_value = 0;
		return;
	}

	int flushData();
	QString getValue(DataFormat fmt);
	int setValue(DataFormat fmt);
private:
	unsigned long long m_value;			/*value of base type*/
};

class NodeEn: public Node {
public:
	NodeEn(DwfType *type)
	{
		UTER_ASSERT(type != NULL);
		UTER_ASSERT(type->m_data_type == DwfType::TYPE_EN);
		m_type = type;

		m_editable = true;
		m_expandable = false;

		m_value = 0;
		return;
	}

	int flushData();
	QString getValue(DataFormat fmt);
	int setValue(DataFormat fmt);
private:
	unsigned long long m_value;			/*value of enum*/
};

class NodeSt: public Node {
public:
	NodeSt(DwfType *type)
	{
		UTER_ASSERT(type != NULL);
		UTER_ASSERT(type->m_data_type == DwfType::TYPE_ST);
		m_type = type;

		m_editable = false;
		m_expandable = true;
		return;
	}

	/*Use for expand struct and struct point*/
	static int expand_struct(Node *tovar, DwfTypeSt *type_st);
	int expandType();
	int flushData();
	QString getValue(DataFormat fmt) {return "{...}";}
};

class NodeUn: public Node {
public:
	NodeUn(DwfType *type)
	{
		UTER_ASSERT(type != NULL);
		UTER_ASSERT(type->m_data_type == DwfType::TYPE_UN);
		m_type = type;

		m_editable = false;
		m_expandable = true;
		return;
	}

	/*Use for expand union and union point*/
	static int expand_union(Node *tovar, DwfTypeUn *type_un);
	int expandType();
	int flushData();
	QString getValue(DataFormat fmt) {return "{...}";}
};

class NodeArray: public Node {
public:
	NodeArray(DwfType *type)
	{
		UTER_ASSERT(type != NULL);
		UTER_ASSERT(type->m_data_type == DwfType::TYPE_ARRAY);
		m_type = type;

		m_editable = false;
		m_expandable = true;

		m_index = 0;
		return;
	}

	int expandType();
	int flushData();
	QString getValue(DataFormat fmt);
	/* Array bound index, eg: int var[2][3][4],
	 * index == 0 variable is int xxxx[2][3][4]
	 * index == 1 variable is int xxxx[3][4]
	 * index == 2 variable is int xxxx[4]
	 * array final data is int*/
	unsigned int m_index;
};

class NodePoint: public Node {
public:
	NodePoint(DwfType *type)
	{
		UTER_ASSERT(type != NULL);
		UTER_ASSERT(type->m_data_type == DwfType::TYPE_POINT);
		m_type = type;

		m_editable = true;
		m_expandable = true;

		m_value = 0;

		DwfTypePoint *point_type = (DwfTypePoint *)type;

		DwfType *to_type = point_type->m_type;
		UTER_ASSERT(to_type != NULL);

		DwfType *final_type = to_type->finalType();
		UTER_ASSERT(final_type != NULL);

		/*Some point type not support expand*/
		if (final_type->m_data_type == DwfType::TYPE_VOID
			|| final_type->m_data_type == DwfType::TYPE_SUBRT) {
			m_expandable = false;
		}

		return;
	}

	int expandType();
	int flushData();
	QString getValue(DataFormat fmt);
	int setValue(DataFormat fmt);
private:
	unsigned long long m_value;			/*point value, the address point to*/
};

class VarDock: public WidgetWithDock {
	Q_OBJECT
public slots:
	void slotItemExpanded(QTreeWidgetItem *item);
	void slotItemClicked(QTreeWidgetItem *item, int column);
	void slotItemChanged(QTreeWidgetItem *item, int column);
public:
	VarDock();
	~VarDock()
	{
		clear();
		return;
	}
	int updateView(ThrdUi::Thrd *task, ThrdUi::Stack *node);	/*Set variables from stack in task*/
	int clear();												/*Clear view*/
	DataFormat m_data_format;
private:
	QTreeWidget *m_treeview;									/*Variable tree view*/
	QTextEdit *m_info_edit;										/*Description text of current variable*/
	QList<Node*> m_var_list;									/*All variables now*/
};

class WatchDock: public WidgetWithDock {
	Q_OBJECT
public:
	WatchDock();
private:
};

} /*namespace VarUi*/
#endif /* SRC_UI_VARIABLE_HXX_ */
