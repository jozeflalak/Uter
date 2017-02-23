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

#ifndef SRC_UI_BREAKPOINT_HXX_
#define SRC_UI_BREAKPOINT_HXX_

#include <ui_public.hxx>
#include <ui_editor.hxx>

namespace BrkPntUi {

class NewFunBrkDlg: public QDialog
{
	Q_OBJECT
private slots:
	void slotBtnOk();
	void slotBtnCancel();
	void slotBtnBrowse();
public:
	NewFunBrkDlg(QWidget *parent);
	bool m_result_ok;
	QString m_result_file;
	QString m_result_func;
private:
	QLineEdit *m_file;
	QLineEdit *m_func;

	QPushButton *m_browse;
	QPushButton *m_ok;
	QPushButton *m_cancel;
};

class BrkApply {
public:
	BrkApply()
	{
		m_addr = 0;
		m_ref_count = 0;
		return;
	}
	unsigned long long m_addr;					/*Breakpoint apply address*/
	unsigned int m_ref_count;					/*Reference count*/
};

class BrkPntDock;
class BrkPnt {
public:
	enum BrkPntType {
		TYPE_FILE = 1,
		TYPE_FUNC,
		TYPE_MAX,
	};
	BrkPnt()
	{
		m_type = TYPE_MAX;
		m_name_item = new QTableWidgetItem();
		m_name_item->setData(Qt::UserRole, QVariant::fromValue((void *)this));
		m_apply_item = new QTableWidgetItem();
		m_apply_item->setData(Qt::UserRole, QVariant::fromValue((void *)this));
		return;
	}
	virtual ~BrkPnt()
	{
		delete m_name_item;
		delete m_apply_item;
		return;
	}
	virtual int modAdd(BrkPntDock *win, TargetMod *moditem){return 0;};
	virtual int modDel(BrkPntDock *win, TargetMod *moditem);
	BrkPntType m_type;						/*Point type*/
	QTableWidgetItem *m_name_item;			/*Breakpoint table item*/
	QTableWidgetItem *m_apply_item;			/*Breakpoint table item*/
	QList<BrkApply*> m_apply_list;			/*Apply breakpoints from this breakpoint*/
};

class FileBrkPnt:public BrkPnt {
public:
	FileBrkPnt(QString file, unsigned int line)
	{
		m_type = TYPE_FILE;
		m_file = file;
		m_line = line;

		QFileInfo info = QFileInfo(m_file);
		m_name_item->setText(info.fileName() + QString::asprintf(" [line:%d]", m_line));
		m_name_item->setToolTip(m_file + QString::asprintf(" [line:%d]", m_line));
		return;
	}
	~FileBrkPnt();
	int modAdd(BrkPntDock *win, TargetMod *moditem);
	int modDel(BrkPntDock *win, TargetMod *moditem);
	QString m_file;										/*File name*/
	unsigned int m_line;								/*Line number*/
};

class FuncBrkPnt: public BrkPnt {
public:
	FuncBrkPnt(QString file, QString func)
	{
		m_type = TYPE_FUNC;
		m_file = file;
		m_fun = func;

		QFileInfo info = QFileInfo(m_file);
		m_name_item->setText(info.fileName() + " [function:" + m_fun + "]");
		m_name_item->setToolTip(m_file + " [function:" + m_fun + "]");
		return;
	}
	int modAdd(BrkPntDock *win, TargetMod *moditem);
	QString m_file;									/*File name*/
	QString m_fun;									/*Function name*/
};

class BrkPntDock: public WidgetWithDock {
	Q_OBJECT
private slots:
	void slotAddFunPressed();
	void slotDelPressed();
	void slotDelAllPressed();
	/*Target event*/
	void slotModAdd(TargetMod*);
	void slotModDel(TargetMod*);

	void slotEditFileOpen(UEditor*);							/*File open event*/
	void slotMarginDoubleClicked(UEditor*, unsigned int);	/*Margin double click event*/
public:
	BrkPntDock();
	~BrkPntDock()
	{
		QList<BrkPnt*>::iterator brk_iter;
		for (brk_iter = m_brkpnt_list.begin(); brk_iter != m_brkpnt_list.end(); /*none*/) {
			BrkPnt *brk = *brk_iter;
			delete brk;
			m_brkpnt_list.erase(brk_iter++);
		}

		QMap<unsigned long long, BrkApply*>::iterator apply_iter;
		for (apply_iter = m_apply_map.begin(); apply_iter != m_apply_map.end(); /*none*/) {
			BrkApply *apply = apply_iter.value();
			delete apply;
			m_apply_map.erase(apply_iter++);
		}
		return;
	}
	QMap<unsigned long long, BrkApply*> m_apply_map;	/*Apply breakpoint map*/
	QIcon m_icon_brkpnt_enable;
	QIcon m_icon_brkpnt_disable;
private:
	int flushIcon(BrkPnt *brkpnt);
	int usrBrkAdd(BrkPnt *brkpnt);						/*Breakpoint add*/
	int usrBrkDel(BrkPnt *brkpnt);						/*Breakpoint remove*/
	QList<BrkPnt*> m_brkpnt_list;						/*Breakpoint list*/
	QTableWidget *m_brkpnt_show;						/*Breakpoint view*/
};

} /*namespace BrkPntUi*/
#endif /* SRC_UI_BREAKPOINT_HXX_ */
