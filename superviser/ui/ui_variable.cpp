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

namespace VarUi {

enum VarTreeCaption {
	VAR_COLUMN_NAME = 0,
	VAR_COLUMN_TYPE,
	VAR_COLUMN_VALUE,
	VAR_COLUMN_MAX,
};

static const char *g_var_tree_caption[] = {
	"Name",
	"Type",
	"Value",
};

UTER_ARRAY_CHECK(g_var_tree_caption, VAR_COLUMN_MAX);

Node *Node::makeVarNode(DwfType *input_type, QString &name, unsigned long long array_index)
{
	UTER_ASSERT(input_type != NULL);
	Node *varnode = NULL;

	DwfType *type = input_type->finalType();
	UTER_ASSERT(type != NULL);

	switch(type->m_data_type)
	{
	case DwfType::TYPE_BASE:
	{
		varnode = new NodeBase(type);
		break;
	}
	case DwfType::TYPE_ST:
	{
		varnode = new NodeSt(type);
		break;
	}
	case DwfType::TYPE_UN:
	{
		varnode = new NodeUn(type);
		break;
	}
	case DwfType::TYPE_EN:
	{
		varnode = new NodeEn(type);
		break;
	}
	case DwfType::TYPE_ARRAY:
	{
		varnode = new NodeArray(type);
		((NodeArray *)varnode)->m_index = array_index;
		break;
	}
	case DwfType::TYPE_POINT:
	{
		varnode = new NodePoint(type);
		break;
	}
	default:
	{
		break;
	}
	}
	if (varnode == NULL) {
		return NULL;
	}

	/*Set name*/
	varnode->m_name = name;
	/*Get type description from input type, for keep typedef information*/
	DwfType::typeDesc(input_type, varnode->m_desc_before, varnode->m_desc_after, array_index);

	/*Setup tree item*/
	varnode->m_item = new QTreeWidgetItem();
	varnode->m_item->setData(0, Qt::UserRole, QVariant::fromValue((void *)varnode));
	varnode->m_item->setText(VAR_COLUMN_NAME, varnode->m_name);
	varnode->m_item->setText(VAR_COLUMN_TYPE, varnode->m_desc_before + varnode->m_desc_after);
	varnode->m_item->setText(VAR_COLUMN_VALUE, "");
	/*Set tool tip*/
	if (input_type->m_data_type == DwfType::TYPE_TYPEDEF) {	/*Just support typedef*/
		QString type_tips;
		DwfType *curtype = input_type;
		while(curtype->m_data_type == DwfType::TYPE_TYPEDEF) {
			DwfTypeTypedef *deftype = (DwfTypeTypedef *)curtype;
			DwfType *to_type = deftype->m_type;
			type_tips += deftype->m_name + "->";
			curtype = to_type;
		}
		QString before;
		QString after;
		DwfType::typeDesc(curtype, before, after);
		type_tips += before + after;
		varnode->m_item->setToolTip(VAR_COLUMN_TYPE, type_tips);
	}

	return varnode;
}

int Node::enableNode(unsigned long long addr)
{
	/* If reset address from STAT_LOADED status,
	 * turn status to STAT_ENABLE, because value is stale*/
	m_stat = STAT_ENABLE;
	m_addr = addr;

	QString tip = "Address at:" + g_target->arch0xAddr(m_addr);
	m_item->setToolTip(VAR_COLUMN_NAME, tip);
	return 0;
}

int Node::flushNode()
{
	/*Add stub child if needed*/
	if (m_expandable && m_child_list.empty() && m_stubitem == NULL) {
		m_stubitem = new QTreeWidgetItem();
		m_item->addChild(m_stubitem);
	}

	m_stat = STAT_ENABLE;
	flushData();			/*Variable flush value from target, if failed set status to STAT_ENABLE*/
	setValueToView();		/*Set variable to control ware*/
	return 0;
}

int Node::expandNode() {
	if (! (m_expandable && m_stat == STAT_LOADED)) {
		/*Must be expandable and loaded*/
		return 0;
	}

	if (! m_child_list.empty()) {
		/*Expanded already*/
		return 0;
	}

	if (m_stubitem != NULL) {
		/*Clear stub child*/
		m_item->removeChild(m_stubitem);
		delete m_stubitem;
		m_stubitem = NULL;
	}

	/*Do expand by variable self*/
	expandType();
	return 0;
}

/*Set string to control ware from variable*/
int Node::setValueToView()
{
	m_settingnow = true;			/*setting*/
	if (m_stat == STAT_DISABLE) {
		m_item->setText(VAR_COLUMN_VALUE, "No address information");
		m_item->setTextColor(VAR_COLUMN_VALUE, QColor::fromRgb(255, 0, 0));
	} else if (m_stat == STAT_ENABLE) {
		m_item->setText(VAR_COLUMN_VALUE, "Load failed");
		m_item->setTextColor(VAR_COLUMN_VALUE, QColor::fromRgb(255, 0, 0));
	} else {
		DataFormat format = g_mainwin->m_var_win->m_data_format;
		m_item->setText(VAR_COLUMN_VALUE, getValue(format));
		m_item->setTextColor(VAR_COLUMN_VALUE, QColor::fromRgb(0, 0, 0));
	}
	m_settingnow = false;
	return 0;
}

/*Get user input from control ware*/
QString Node::getInputFromView()
{
	return m_item->text(VAR_COLUMN_VALUE);
}

int Node::expandType()
{
	return 0;
}

int Node::flushData()
{
	m_stat = STAT_LOADED;
	return 0;
}

QString Node::getValue(DataFormat fmt)
{
	return "";
}

int Node::setValue(DataFormat fmt)
{
	return 0;
}

int NodeBase::flushData()
{
	UTER_ASSERT(m_type != NULL);
	UTER_ASSERT(m_type->m_data_type == DwfType::TYPE_BASE);
	DwfTypeBase *type_base = (DwfTypeBase *)m_type;
	const char *shortname = DwfTypeBase::base_desc[type_base->m_base_type].m_short;

	Arch *arch = g_target->archInfoGet();
	if (arch == NULL) {
		UTER_LOG("Get architecture specification failed.");
		return -1;
	}

	ArchBaseType *base = arch->getBaseByName(shortname);
	if (base == NULL) {
		UTER_LOG("Get base type %s failed.", shortname);
		return -1;
	}

	QByteArray array;
	int result = g_target->memoryGet(m_addr, base->m_data_size, array);
	if (result < 0) {
		UTER_LOG("Get base type %s value failed.", shortname);
		return -1;
	}
	g_target->bytesToData(m_value, base->m_data_size, array);

	m_stat = STAT_LOADED;
	return 0;
}

QString NodeBase::getValue(DataFormat fmt)
{
	UTER_ASSERT(m_type != NULL);
	UTER_ASSERT(m_type->m_data_type == DwfType::TYPE_BASE);
	DwfTypeBase *type_base = (DwfTypeBase *)m_type;
	const char *shortname = DwfTypeBase::base_desc[type_base->m_base_type].m_short;

	Arch *arch = g_target->archInfoGet();
	if (arch == NULL) {
		UTER_LOG("Get architecture specification failed.");
		return "";
	}

	ArchBaseType *base = arch->getBaseByName(shortname);
	if (base == NULL) {
		UTER_LOG("Get base type %s failed.", shortname);
		return "";
	}

	switch(type_base->m_base_type)
	{
	case DwfTypeBase::BASE_CHAR:
	case DwfTypeBase::BASE_UCHAR:
	case DwfTypeBase::BASE_SHORT:
	case DwfTypeBase::BASE_USHORT:
	case DwfTypeBase::BASE_INT:
	case DwfTypeBase::BASE_UINT:
	case DwfTypeBase::BASE_LONG:
	case DwfTypeBase::BASE_ULONG:
	case DwfTypeBase::BASE_LONGLONG:
	case DwfTypeBase::BASE_ULONGLONG:
	{
		unsigned long long number = g_target->dataToNum(m_value, base->m_data_size, base->m_signed_flag);
		switch(fmt)
		{
		case FMT_OCT:
		{
			return QString::asprintf("0%llo", number);
		}
		case FMT_HEX:
		{
			return QString::asprintf("0x%llx", number);
		}
		case FMT_DEC:
		case FMT_DFT:
		{
			return QString::asprintf("%lld", number);
		}
		default:
		{
			UTER_ASSERT(0);
		}
		}
		break;
	}
	case DwfTypeBase::BASE_FLOAT:
	case DwfTypeBase::BASE_DOUBLE:
	{
		/*暂不支持*/
		break;
	}
	default:
	{
		break;
	}
	}

	return "";
}

int NodeBase::setValue(DataFormat fmt)
{
	UTER_ASSERT(m_type != NULL);
	UTER_ASSERT(m_type->m_data_type == DwfType::TYPE_BASE);
	DwfTypeBase *type_base = (DwfTypeBase *)m_type;
	const char *shortname = DwfTypeBase::base_desc[type_base->m_base_type].m_short;

	Arch *arch = g_target->archInfoGet();
	if (arch == NULL) {
		UTER_LOG("Get architecture specification failed.");
		return -1;
	}

	ArchBaseType *base = arch->getBaseByName(shortname);
	if (base == NULL) {
		UTER_LOG("Get base type %s failed.", shortname);
		return -1;
	}

	QString input = getInputFromView();

	switch(type_base->m_base_type)
	{
	case DwfTypeBase::BASE_CHAR:
	case DwfTypeBase::BASE_UCHAR:
	case DwfTypeBase::BASE_SHORT:
	case DwfTypeBase::BASE_USHORT:
	case DwfTypeBase::BASE_INT:
	case DwfTypeBase::BASE_UINT:
	case DwfTypeBase::BASE_LONG:
	case DwfTypeBase::BASE_ULONG:
	case DwfTypeBase::BASE_LONGLONG:
	case DwfTypeBase::BASE_ULONGLONG:
	{
		int prefix_base;
		if (input.indexOf("0x") == 0 || input.indexOf("0X") == 0) {
			prefix_base = 16;
		} else if (input.indexOf("0") == 0) {
			prefix_base = 8;
		} else {
			prefix_base = 10;
		}

		unsigned long long input_number;
		bool convert_ok;
		if (base->m_signed_flag) {
			input_number = (unsigned long long)input.toLongLong(&convert_ok, prefix_base);
		} else {
			input_number = (unsigned long long)input.toULongLong(&convert_ok, prefix_base);
		}
		if (! convert_ok) {
			flushData();
			return -1;
		}

		unsigned long long data = g_target->NumToData(input_number, base->m_data_size, base->m_signed_flag);

		QByteArray array;
		g_target->dataToBytes(data, base->m_data_size, array);
		int result = g_target->memorySet(m_addr, base->m_data_size, array);
		if (result < 0) {
			flushData();
			return -1;
		}

		break;
	}
	case DwfTypeBase::BASE_FLOAT:
	case DwfTypeBase::BASE_DOUBLE:
	{
		break;
	}
	default:
	{
		break;
	}
	}

	flushData();
	return 0;
}

int NodeEn::flushData()
{
	UTER_ASSERT(m_type != NULL);
	UTER_ASSERT(m_type->m_data_type == DwfType::TYPE_EN);

	m_settingnow = true;
	m_item->setToolTip(VAR_COLUMN_VALUE, "");
	m_settingnow = false;

	Arch *arch = g_target->archInfoGet();
	if (arch == NULL) {
		UTER_LOG("Get architecture specification failed.");
		return -1;
	}

	const char *shortname = "int";		/*enum same as int*/
	ArchBaseType *base = arch->getBaseByName(shortname);
	if (base == NULL) {
		UTER_LOG("Get base type %s failed.", shortname);
		return -1;
	}

	QByteArray array;
	int result = g_target->memoryGet(m_addr, base->m_data_size, array);
	if (result < 0) {
		UTER_LOG("Get base type %s value failed.", shortname);
		return -1;
	}
	g_target->bytesToData(m_value, base->m_data_size, array);

	/*enum tooltip*/
	m_settingnow = true;
	m_item->setToolTip(VAR_COLUMN_VALUE,
			   QString::asprintf("Enumerator value:%lld",
			   g_target->dataToNum(m_value, base->m_data_size, base->m_signed_flag)));
	m_settingnow = false;

	m_stat = STAT_LOADED;
	return 0;
}

QString NodeEn::getValue(DataFormat fmt)
{
	Arch *arch = g_target->archInfoGet();
	if (arch == NULL) {
		UTER_LOG("Get architecture specification failed.");
		return "";
	}

	const char *shortname = "int";		/*enum same as int*/
	ArchBaseType *base = arch->getBaseByName(shortname);
	if (base == NULL) {
		UTER_LOG("Get base type %s failed.", shortname);
		return "";
	}

	long long number = (long long)g_target->dataToNum(m_value, base->m_data_size, base->m_signed_flag);

	switch(fmt)
	{
	case FMT_OCT:
	{
		return QString::asprintf("0%llo", number);
	}
	case FMT_HEX:
	{
		return QString::asprintf("0x%llx", number);
	}
	case FMT_DEC:
	{
		return QString::asprintf("0x%lld", number);
	}
	case FMT_DFT:
	{
		DwfTypeEn *type_en = (DwfTypeEn *)m_type;
		QList<DwfTypeEnMem*>::iterator iter;
		for (iter = type_en->m_member_list.begin(); iter != type_en->m_member_list.end(); iter++) {
			DwfTypeEnMem *enmem = *iter;
			if (enmem->m_value == number) {
				return enmem->m_name;
			}
		}
		return QString::asprintf("%lld", number);
	}
	default:
	{
		UTER_ASSERT(0);
	}
	}

	return "";
}

int NodeEn::setValue(DataFormat fmt)
{
	Arch *arch = g_target->archInfoGet();
	if (arch == NULL) {
		UTER_LOG("Get architecture specification failed.");
		return -1;
	}

	const char *shortname = "int";		/*enum same as int*/
	ArchBaseType *base = arch->getBaseByName(shortname);
	if (base == NULL) {
		UTER_LOG("Get base type %s failed.", shortname);
		return -1;
	}

	QString input = getInputFromView();

	DwfTypeEn *type_en = (DwfTypeEn *)m_type;
	QList<DwfTypeEnMem*>::iterator iter;
	for (iter = type_en->m_member_list.begin(); iter != type_en->m_member_list.end(); iter++) {
		DwfTypeEnMem *enmem = *iter;
		if (enmem->m_name == input) {
			break;
		}
	}
	unsigned long long input_number;
	if (iter != type_en->m_member_list.end()) {
		DwfTypeEnMem *enmem = *iter;
		input_number = (unsigned long long)enmem->m_value;
	} else {
		int prefix_base;
		if (input.indexOf("0x") == 0 || input.indexOf("0X") == 0) {
			prefix_base = 16;
		} else if (input.indexOf("0") == 0) {
			prefix_base = 8;
		} else {
			prefix_base = 10;
		}

		bool convert_ok;
		input_number = (unsigned long long)input.toLongLong(&convert_ok, prefix_base);
		if (! convert_ok) {
			flushData();
			return -1;
		}
	}

	unsigned long long data = g_target->NumToData(input_number, base->m_data_size, base->m_signed_flag);

	QByteArray array;
	g_target->dataToBytes(data, base->m_data_size, array);
	int result = g_target->memorySet(m_addr, base->m_data_size, array);
	if (result < 0) {
		flushData();
		return -1;
	}

	flushData();
	return 0;
}


int NodeSt::expand_struct(Node *tovar, DwfTypeSt *type_st)
{
	UTER_ASSERT(tovar != NULL);
	UTER_ASSERT(type_st != NULL);
	UTER_ASSERT(tovar->m_child_list.empty());

	/*Foreach all child*/
	QList<DwfTypeStMem*>::iterator iter;
	for (iter = type_st->m_member_list.begin(); iter != type_st->m_member_list.end(); iter++) {
		DwfTypeStMem *mem = *iter;

		/*Create child node(just template, no address)*/
		Node *varnode = makeVarNode(mem->m_type, mem->m_name);
		varnode->m_ext_type = mem;

		tovar->m_item->addChild(varnode->m_item);
		tovar->m_child_list.push_back(varnode);
	}

	return 0;
}

int NodeSt::expandType()
{
	UTER_ASSERT(m_type != NULL);
	UTER_ASSERT(m_type->m_data_type == DwfType::TYPE_ST);
	DwfTypeSt *type_st = (DwfTypeSt *)m_type;
	expand_struct(this, type_st);
	return 0;
}

int NodeSt::flushData()
{
	UTER_ASSERT(m_type != NULL);
	UTER_ASSERT(m_type->m_data_type == DwfType::TYPE_ST);

	QList<Node*>::iterator iter;
	for (iter = m_child_list.begin(); iter != m_child_list.end(); iter++) {
		Node *varnode = *iter;
		UTER_ASSERT(varnode->m_ext_type != NULL);
		UTER_ASSERT(varnode->m_ext_type->m_data_type == DwfType::TYPE_ST_MEM);
		DwfTypeStMem *mem_type = (DwfTypeStMem *)varnode->m_ext_type;

		varnode->enableNode(m_addr + mem_type->m_member_location);
		varnode->flushNode();
	}

	m_stat = STAT_LOADED;
	return 0;
}

int NodeUn::expand_union(Node *tovar, DwfTypeUn *type_un)
{
	UTER_ASSERT(tovar != NULL);
	UTER_ASSERT(type_un != NULL);
	UTER_ASSERT(tovar->m_child_list.empty());

	/*Foreach all child*/
	QList<DwfTypeUnMem*>::iterator iter;
	for (iter = type_un->m_member_list.begin(); iter != type_un->m_member_list.end(); iter++) {
		DwfTypeUnMem *mem = *iter;

		Node *varnode = makeVarNode(mem->m_type, mem->m_name);
		varnode->m_ext_type = mem;

		/*Create child node(just template, no address)*/
		tovar->m_item->addChild(varnode->m_item);
		tovar->m_child_list.push_back(varnode);
	}

	return 0;
}

int NodeUn::expandType()
{
	UTER_ASSERT(m_type != NULL);
	UTER_ASSERT(m_type->m_data_type == DwfType::TYPE_UN);
	DwfTypeUn *type_un = (DwfTypeUn *)m_type;
	expand_union(this, type_un);
	return 0;
}

int NodeUn::flushData()
{
	UTER_ASSERT(m_type != NULL);
	UTER_ASSERT(m_type->m_data_type == DwfType::TYPE_UN);

	QList<Node*>::iterator iter;
	for (iter = m_child_list.begin(); iter != m_child_list.end(); iter++) {
		Node *varnode = *iter;
		UTER_ASSERT(varnode->m_ext_type != NULL);
		UTER_ASSERT(varnode->m_ext_type->m_data_type == DwfType::TYPE_UN_MEM);
		DwfTypeUnMem *mem_type = (DwfTypeUnMem *)varnode->m_ext_type;

		varnode->enableNode(m_addr);
		varnode->flushNode();
	}

	m_stat = STAT_LOADED;
	return 0;
}

int NodeArray::expandType()
{
	UTER_ASSERT(m_type != NULL);
	UTER_ASSERT(m_type->m_data_type == DwfType::TYPE_ARRAY);
	DwfTypeArray *type_array = (DwfTypeArray *)m_type;

	/*Get my bound, my bound is number of child*/
	unsigned long long current_range = type_array->m_range_list.at(m_index);

	/* Index begin from 0, child index is m_index + 1,
	 * if child index out of range, the child is not a sub array*/
	if (m_index + 1 >= type_array->m_range_list.length()) {
		/*Child is final data type, not sub array*/
		DwfType *type = type_array->m_type;
		unsigned long long iloop;
		for (iloop = 0; iloop < current_range; iloop++) {
			QString child_name = m_name + QString::asprintf("[%lld]", iloop);
			Node *varnode = makeVarNode(type, child_name);
			m_item->addChild(varnode->m_item);
			m_child_list.push_back(varnode);
		}
	} else {
		/*Child is sub array*/
		unsigned long long iloop;
		for (iloop = 0; iloop < current_range; iloop++) {
			QString child_name = m_name + QString::asprintf("[%lld]", iloop);
			Node *varnode = makeVarNode(m_type, child_name, m_index + 1);
			m_item->addChild(varnode->m_item);
			m_child_list.push_back(varnode);
		}
	}

	return 0;
}

int NodeArray::flushData()
{
	UTER_ASSERT(m_type != NULL);
	UTER_ASSERT(m_type->m_data_type == DwfType::TYPE_ARRAY);
	DwfTypeArray *type_array = (DwfTypeArray *)m_type;

	/*Get size for child which is sub array(or final data), get final data size first*/
	DwfType *final_type = type_array->m_type->finalType();
	UTER_ASSERT(final_type != NULL);
	/*Get number of final data in sub array*/
	unsigned long long num_final_data = 1;
	unsigned int count = type_array->m_range_list.length();
	unsigned int iloop;
	/*m_index is my index, sub array index begin from m_index + 1*/
	for (iloop = m_index + 1; iloop < count; iloop++) {
		num_final_data *= type_array->m_range_list[iloop];
	}
	unsigned long long byte_size = final_type->m_size * num_final_data;

	unsigned long long byte_offset = 0;
	QList<Node*>::iterator iter;
	for (iter = m_child_list.begin(); iter != m_child_list.end(); iter++) {
		Node *varnode = *iter;
		varnode->enableNode(m_addr + byte_offset);
		varnode->flushNode();
		byte_offset += byte_size;
	}

	m_stat = STAT_LOADED;
	return 0;
}

QString NodeArray::getValue(DataFormat fmt)
{
	return g_target->arch0xAddr(m_addr);
}

int NodePoint::expandType()
{
	UTER_ASSERT(m_type != NULL);
	UTER_ASSERT(m_type->m_data_type == DwfType::TYPE_POINT);
	DwfTypePoint *type_point = (DwfTypePoint *)m_type;

	DwfType *to_type = type_point->m_type;
	UTER_ASSERT(to_type != NULL);

	DwfType *type = to_type->finalType();
	UTER_ASSERT(type != NULL);

	switch(type->m_data_type)
	{
	case DwfType::TYPE_BASE:
	case DwfType::TYPE_EN:
	case DwfType::TYPE_POINT:
	{
		QString name = "*" + m_name;
		/*Not use final type, because keep typedef information*/
		Node *varnode = makeVarNode(to_type, name);
		m_item->addChild(varnode->m_item);
		m_child_list.push_back(varnode);
		break;
	}
	case DwfType::TYPE_ST:
	{
		DwfTypeSt *type_st = (DwfTypeSt *)type;
		NodeSt::expand_struct(this, type_st);
		break;
	}
	case DwfType::TYPE_UN:
	{
		DwfTypeUn *type_un = (DwfTypeUn *)type;
		NodeUn::expand_union(this, type_un);
		break;
	}
	default:
	{
		/*For example: void *, int (x)(), ....*/
		break;
	}
	}

	return 0;
}

int NodePoint::flushData()
{
	UTER_ASSERT(m_type != NULL);
	UTER_ASSERT(m_type->m_data_type == DwfType::TYPE_POINT);
	DwfTypePoint *type_point = (DwfTypePoint *)m_type;

	Arch *arch = g_target->archInfoGet();
	if (arch == NULL) {
		UTER_LOG("Get architecture specification failed.");
		return -1;
	}

	QByteArray array;
	int result = g_target->memoryGet(m_addr, arch->m_pointlen, array);
	if (result < 0) {
		UTER_LOG("Get point value failed.");
		return -1;
	}
	g_target->bytesToData(m_value, arch->m_pointlen, array);

	/*Final type point to*/
	DwfType *to_type = type_point->m_type;
	UTER_ASSERT(to_type != NULL);

	DwfType *type = to_type->finalType();
	UTER_ASSERT(type != NULL);

	switch(type->m_data_type)
	{
	case DwfType::TYPE_BASE:
	case DwfType::TYPE_EN:
	case DwfType::TYPE_POINT:
	case DwfType::TYPE_UN:
	{
		QList<Node*>::iterator iter;
		for (iter = m_child_list.begin(); iter != m_child_list.end(); iter++) {
			Node *varnode = *iter;
			/*my value is your address*/
			varnode->enableNode(m_value);
			varnode->flushNode();
		}
		break;
	}
	case DwfType::TYPE_ST:
	{
		QList<Node*>::iterator iter;
		for (iter = m_child_list.begin(); iter != m_child_list.end(); iter++) {
			Node *varnode = *iter;

			UTER_ASSERT(varnode->m_ext_type != NULL);
			UTER_ASSERT(varnode->m_ext_type->m_data_type == DwfType::TYPE_ST_MEM);
			DwfTypeStMem *mem_type = (DwfTypeStMem *)varnode->m_ext_type;

			/*my value and offset is youe address*/
			varnode->enableNode(m_value + mem_type->m_member_location);
			varnode->flushNode();
		}
		break;
	}
	default:
	{
		break;
	}
	}

	m_stat = STAT_LOADED;
	return 0;
}

QString NodePoint::getValue(DataFormat fmt)
{
	return g_target->arch0xAddr(m_value);
}

int NodePoint::setValue(DataFormat fmt)
{
	Arch *arch = g_target->archInfoGet();
	if (arch == NULL) {
		UTER_LOG("Get architecture specification failed.");
		return -1;
	}

	QString input = getInputFromView();

	int prefix_base;
	if (input.indexOf("0x") == 0 || input.indexOf("0X") == 0) {
		prefix_base = 16;
	} else if (input.indexOf("0") == 0) {
		prefix_base = 8;
	} else {
		prefix_base = 10;
	}

	unsigned long long input_number;
	bool convert_ok;
	input_number = (unsigned long long)input.toULongLong(&convert_ok, prefix_base);
	if (! convert_ok) {
		flushData();
		return -1;
	}

	QByteArray array;
	g_target->dataToBytes(input_number, arch->m_pointlen, array);
	int result = g_target->memorySet(m_addr, arch->m_pointlen, array);
	if (result < 0) {
		flushData();
		return -1;
	}

	flushData();
	return 0;
}

VarDock::VarDock()
:WidgetWithDock(SEQ_VAR, Qt::BottomDockWidgetArea, "Variable")
{
	QMetaObject::Connection conn;

	QVBoxLayout *out_layout = new QVBoxLayout(this);
	setLayout(out_layout);

	/*TODO:toolbar + format select*/

	QSplitter *out_split = new QSplitter(Qt::Horizontal, this);
	out_layout->addWidget(out_split);

	m_treeview = new QTreeWidget(this);
	out_split->addWidget(m_treeview);
	out_split->setStretchFactor(0, 70);

	m_treeview->setColumnCount(VAR_COLUMN_MAX);
	QStringList head_list;
	int iloop;
	for (iloop = 0; iloop < VAR_COLUMN_MAX; iloop++) {
		head_list << g_var_tree_caption[iloop];
	}
	m_treeview->setHeaderLabels(head_list);

	m_info_edit = new QTextEdit(this);
	out_split->addWidget(m_info_edit);

	m_data_format = FMT_DFT;

	conn = connect(m_treeview, SIGNAL(itemExpanded(QTreeWidgetItem*)),
			       this, SLOT(slotItemExpanded(QTreeWidgetItem*)));
	UTER_ASSERT(conn != NULL);

	conn = connect(m_treeview, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
			       this, SLOT(slotItemClicked(QTreeWidgetItem*,int)));
	UTER_ASSERT(conn != NULL);

	conn = connect(m_treeview, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
			       this, SLOT(slotItemChanged(QTreeWidgetItem*,int)));
	UTER_ASSERT(conn != NULL);

	g_mainwin->dockSetup(this);
	return;
}

void VarDock::slotItemExpanded(QTreeWidgetItem *item)
{
	Node *varnode = (Node *)item->data(0, Qt::UserRole).value<void*>();

	/*Expand node, but child may be no address, so need flush*/
	varnode->expandNode();
	/*Flush self and child*/
	varnode->flushNode();
	return;
}

/*Click for edit*/
void VarDock::slotItemClicked(QTreeWidgetItem *item, int column)
{
	Node *varnode = (Node *)item->data(0, Qt::UserRole).value<void*>();

	if (column != VAR_COLUMN_VALUE) {
		/*Just for value column, ignore other columns*/
		return;
	}
	if (! varnode->m_editable) {
		/*Just for editable node*/
		return;
	}
	if (varnode->m_stat != Node::STAT_LOADED) {
		/*Node must be loaded*/
		return;
	}

	/*Set edit*/
	varnode->m_item->setFlags(varnode->m_item->flags() | Qt::ItemIsEditable);
	m_treeview->editItem(varnode->m_item, VAR_COLUMN_VALUE);
	return;
}

/*Value change because edit*/
void VarDock::slotItemChanged(QTreeWidgetItem *item, int column)
{
	Node *varnode = (Node *)item->data(0, Qt::UserRole).value<void*>();

	if (column != VAR_COLUMN_VALUE) {
		/*Just for value column, ignore other columns*/
		return;
	}
	if (varnode->m_settingnow) {
		/*Ignore system change*/
		return;
	}
	if (! (varnode->m_editable && varnode->m_stat != Node::STAT_DISABLE)) {
		/*Editable and good status*/
		return;
	}

	/*Do data set*/
	varnode->setValue(FMT_DFT);

	/*Flush all variables, because last modification may be effect other variable*/
	QList<Node*>::iterator iter;
	for (iter = m_var_list.begin(); iter != m_var_list.end(); iter++) {
		Node *node = *iter;
		node->flushNode();
	}

	return;
}

int VarDock::updateView(ThrdUi::Thrd *task, ThrdUi::Stack *node)
{
	UTER_ASSERT(task != NULL);
	UTER_ASSERT(node != NULL);

	clear();					/*Clear first*/

	Arch *arch = g_target->archInfoGet();
	if (arch == NULL) {
		UTER_LOG("Get architecture specification failed.");
		return -1;
	}

	/*stack at mod*/
	TargetMod *mod = g_target->modGetByAddrIn(node->m_addr);
	if (mod == NULL) {
		UTER_LOG("Get module for stack on failed.");
		return -1;
	}
	unsigned long long offset_addr = node->m_addr - mod->m_add_mkup;
	DwfCu *cu = mod->dwarfGetCuByAddrIn(offset_addr);
	if (cu == NULL) {
		/*If no debug information, return here*/
		return 0;
	}

	/*Get local variables for stack at*/
	DwfLocalVars *varset = cu->getLocalVars(offset_addr);
	if (varset == NULL) {
		return 0;
	}

	QMap<QString, DwfLocalVars::Item*>::iterator iter;
	for (iter = varset->m_var_map.begin(); iter != varset->m_var_map.end(); iter++) {
		DwfLocalVars::Item *var = iter.value();

		DwfType *type = var->m_type;
		UTER_ASSERT(type != NULL);
		UTER_ASSERT(type->m_element_type == DwfItem::DWF_ITEM_TYPE);

		/*Variable address is important*/
		unsigned long long varaddr;
		bool varaddr_ready = true;

		DwfLoc *loc = &(var->m_loc);
		switch(loc->m_loc_type)
		{
		case DwfLoc::LOC_ADDR:
		{
			/*Variable address is number, for example: global variable*/
			varaddr = loc->m_addr;
			break;
		}
		case DwfLoc::LOC_REG:
		case DwfLoc::LOC_REG_OFF:
		{
			/*Variable address register and offset, get register from cfa*/
			unsigned long long regvalue;
			if (loc->m_reg_num == arch->m_stack->m_dwf_no) {
				/*Stack register get direct*/
				regvalue = node->m_stack;
			} else {
				/*Other register value from cfa*/
				DwfRegTable *regtab = node->m_regtab;
				DwfRegTable::Item *entry = regtab->getItemById(loc->m_reg_num);
				if (entry == NULL) {
					varaddr_ready = false;
					break;
				}
				if (entry->m_isexpress) {
					/*TODO:*/
					varaddr_ready = false;
					break;
				}
				if (entry->m_reg_num != DwfLoc::REG_CFA_VAL) {
					varaddr_ready = false;
					break;
				}
				regvalue = node->m_cfa + entry->m_offset;
			}

			/*Register value and offset*/
			varaddr = regvalue + loc->m_offset;
			break;
		}
		case DwfLoc::LOC_CFA_OFF:
		{
			/*Variable address cfa and offset*/
			varaddr = node->m_cfa + loc->m_offset;
			break;
		}
		default:
		{
			varaddr_ready = false;
			break;
		}
		}

		/*Make node, set address if have, flush from memroy*/
		Node *varnode = Node::makeVarNode(type, var->m_name);
		if (varnode == NULL) {
			continue;
		}
		if (varaddr_ready) {
			varnode->enableNode(varaddr);
		}
		varnode->flushNode();

		m_treeview->addTopLevelItem(varnode->m_item);
		m_var_list.push_back(varnode);
	}
	m_treeview->resizeColumnToContents(VAR_COLUMN_NAME);
	m_treeview->resizeColumnToContents(VAR_COLUMN_TYPE);
	m_treeview->resizeColumnToContents(VAR_COLUMN_VALUE);

	delete varset;
	return 0;
}

int VarDock::clear()
{
	/*Clear text*/
	m_info_edit->clear();

	/*Foreach all variable*/
	QList<Node*>::iterator iter;
	for (iter = m_var_list.begin(); iter != m_var_list.end(); /*none*/) {
		Node *varnode = *iter;
		/*Parent remove, child delete*/
		m_treeview->removeItemWidget(varnode->m_item, 0);
		delete varnode;
		m_var_list.erase(iter++);
	}

	return 0;
}

WatchDock::WatchDock()
:WidgetWithDock(SEQ_WATCH, Qt::BottomDockWidgetArea, "Watch")
{
	g_mainwin->dockSetup(this);
	return;
}

} /*namespace VarUi*/
