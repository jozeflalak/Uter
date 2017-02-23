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

#ifndef SRC_UI_MEMORY_HXX_
#define SRC_UI_MEMORY_HXX_

#include <ui_public.hxx>

namespace MemUi {

/*Address input window*/
class AddrInputDlg: public QDialog {
	Q_OBJECT
private slots:
	void slotOkPressed();
public:
	AddrInputDlg(QWidget *parent);
	int openAddr();

	QLineEdit *m_input_text;
	QPushButton *m_cancel_btn;
	QPushButton *m_ok_btn;

	bool m_ok;
	unsigned long long m_address;
};

class MemAt {
public:
	class Block {
	public:
		enum MemStatus {
			STATUS_CHANGE = 1,
			STATUS_FAILED,
		};
		class Bytes {
		public:
			unsigned char m_data;
			unsigned char m_status;
		};
		const static int MEM_BYTES = 4;			/*Bytes per unit*/
		const static int MEM_COLUMNS = 4;		/*Units per row*/
		const static int MEM_ROWS = 16;			/*Rows per block*/
		const static int BYTES_OF_ROW = MEM_BYTES * MEM_COLUMNS;
		const static int BYTES_OF_BLK = BYTES_OF_ROW * MEM_ROWS;	/*Bytes per block*/
		Block()
		{
			m_startat = 0;
			m_number = BYTES_OF_BLK;
			m_loaded = false;
			Bytes byte = {0, false};
			m_content.fill(byte, BYTES_OF_BLK);
			return;
		}

		int loadData();
		int clearData();
		unsigned long long m_startat;				/*Block start at*/
		unsigned long long m_number;				/*Number of bytes in block*/
		bool m_loaded;								/*Loaded flag*/
		QVector<Bytes> m_content;					/*Block contents*/
	};

	MemAt(unsigned long long addr);
	~MemAt()
	{
		QList<Block*>::iterator iter;
		for (iter = m_blocks.begin(); iter != m_blocks.end(); /*none*/) {
			Block *block = *iter;
			delete block;
			m_blocks.erase(iter++);
		}
		delete m_item;
		return;
	}

	int loadData();
	int clearData();
	unsigned long long m_address;			/*Address of memory at*/
	QList<Block*> m_blocks;					/*Continual block for memory*/
	QListWidgetItem *m_item;				/*Item on memory list*/
};

class MemDock: public WidgetWithDock {
	Q_OBJECT
private slots:
	void slotAddPressed();
	void slotDelPressed();
	void slotDelAllPressed();
	void slotByteViewPressed();
	void slotWordViewPressed();

	void slotListItemClicked(QListWidgetItem *);

	void slotTableItemTextChanged(QTableWidgetItem *);
	void slotTableItemClicked(QTableWidgetItem *);
public:
	MemDock();
	~MemDock()
	{
		clear();
		return;
	}
	int updateOnStop();
	int updateOnShut();
	int updateOnRun();
private:
	int setRow(int row_num, MemAt::Block *block, int index, int len);
	int loadMemView(MemAt *);
	int resizeMemView();
	int clearMemView();
	int clear();

	bool m_data_setting;
	QList<MemAt*> m_memat_list;

	QPushButton *m_btn_add;
	QPushButton *m_btn_del;
	QPushButton *m_btn_delall;

	QRadioButton *m_btn_byteview;
	QRadioButton *m_btn_wordview;

	QListWidget *m_list_monitor;
	QTableWidget *m_table_memory;

	AddrInputDlg *m_addr_input;
};

} /*namespace MemUi*/
#endif /* SRC_UI_MEMORY_HXX_ */
