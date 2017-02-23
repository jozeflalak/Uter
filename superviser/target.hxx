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

#ifndef SRC_TARGET_H_
#define SRC_TARGET_H_

#include <QtCore/QThread>
#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtNetwork/QTcpSocket>

#include <uter_public.h>
#include <uter_signal_arg.h>
#include <inter_msg.h>
#include <target_arch.h>
#include <target_bfd.h>
#include <target_coverage.h>
#include <target_dwarf.h>

/*Register value of thread*/
class RegValues {
public:
	class Item {
	public:
		Item()
		{
			m_name = "";
			m_value = 0;
			return;
		}
		QString m_name;					/*Register name*/
		unsigned long long m_value;		/*Register value*/
	};
	RegValues()
	{
		m_pid = 0;
		return;
	}
	~RegValues()
	{
		QList<Item*>::iterator iter;
		for (iter = m_reglist.begin(); iter != m_reglist.end(); /*none*/) {
			Item *regval = *iter;
			delete regval;
			m_reglist.erase(iter++);
		}
		return;
	}
	unsigned int m_pid;
	QList<Item*> m_reglist;
};

/*Stack information of thread*/
class StackInfo {
public:
	class Item {
	public:
		Item()
		{
			m_addr = 0;
			m_stack = 0;
			m_cfa = 0;
			return;
		}
		unsigned long long m_addr;		/*Current address*/
		unsigned long long m_stack;		/*Stack point for node, eg:esp in x86*/
		unsigned long long m_cfa;		/*CFA point for node, get from stack point*/
	};
	StackInfo()
	{
		m_pid = 0;
		return;
	}
	~StackInfo()
	{
		QList<Item*>::iterator iter;
		for (iter = m_stack_list.begin(); iter != m_stack_list.end(); /*none*/) {
			Item *node = *iter;
			delete node;
			m_stack_list.erase(iter++);
		}
		return;
	}
	unsigned int m_pid;
	QList<Item*> m_stack_list;
};

/*Map entry on process, Just support linux now, get from /proc/(pid)/...*/
class TargetMap {
public:
	TargetMap()
	{
		m_start = 0;
		m_end = 0;
		m_flag_r = false;
		m_flag_w = false;
		m_flag_x = false;
		m_flag_p = false;
		m_offset = 0;

		m_major = 0;
		m_minor = 0;
		m_inode = 0;

		m_path = "";
		return;
	}
    bool operator==(const TargetMap& cmp) const
    {
        if (m_start == cmp.m_start
         && m_end == cmp.m_end
		 && m_flag_r == cmp.m_flag_r
		 && m_flag_w == cmp.m_flag_w
		 && m_flag_x == cmp.m_flag_x
		 && m_flag_p == cmp.m_flag_p
		 && m_offset == cmp.m_offset
		 && m_major == cmp.m_major
		 && m_minor == cmp.m_minor
		 && m_inode == cmp.m_inode
		 && m_path == cmp.m_path) {
        	return true;
        }

        return false;
    }
    /*Map entry start address(include) and end address(no inclue)*/
    unsigned long long m_start;
    unsigned long long m_end;
    /*Attribute flag*/
	bool m_flag_r;
	bool m_flag_w;
	bool m_flag_x;
	bool m_flag_p;
	/*Map entry offset on image file*/
	unsigned long long m_offset;
	/*File device number*/
	unsigned int m_major;
	unsigned int m_minor;
	/*File inode number*/
	unsigned int m_inode;
	/*Map file path, empty sometimes*/
	QString m_path;
};

/*Module entry on process, get from TargetMap*/
class TargetMod {
public:
	TargetMod()
	{
		m_name = NULL;
		m_remote_path = "";
		m_local_path = "";

		m_add_mkup = 0;

		m_bfd_file = NULL;

		m_dwarf_fd = -1;
		m_dwarf_dbg = NULL;

		m_dwarf_cie_data = NULL;
	    m_dwarf_cie_count = 0;
	    m_dwarf_fde_data = NULL;
	    m_dwarf_fde_count = 0;
		return;
	}
	~TargetMod()
	{
		if (m_name != NULL) {
			free(m_name);
		}
		return;
	}

/*Base information of module*/
public:
	/*Module name as type char *, for debug show*/
	char *m_name;
	QString m_remote_path;			/*Remote path, file path on target*/
	QString m_local_path;			/*Local path, same file on superviser*/
	DwfAddrRange m_addr_range;		/*Module address range*/
	unsigned long long m_add_mkup;	/*Make up value of address in module, in main program value is 0, in so value is module start*/
	int load();						/*Module information load*/
	int release();					/*Module information release*/
/*BFD information of module*/
public:
	int bfdLoad();
	int bfdRelease();
	BfdSec *bfdGetSecByAddrIn(unsigned long long addr);	/*Get bfd section by program address, not offset address*/
	bfd *m_bfd_file;					/*Keep the bfd handle before release*/
	QList<BfdSec*> m_bfd_sec_list;		/*BFD section informations*/
	/*Symbol table, alive on module which with debug information*/
	QMap<unsigned long long, BfdSym*> m_bfd_dbgsym_addr_map;
	QMap<QString, BfdSym*> m_bfd_dbgsym_str_map;
	QList<BfdSym*> m_bfd_dbgsym_list;
	/*Dynamic symbolic table, alive on so*/
	QMap<unsigned long long, BfdSym*> m_bfd_dynsym_addr_map;
	QMap<QString, BfdSym*> m_bfd_dynsym_str_map;
	QList<BfdSym*> m_bfd_dynsym_list;
	/*Dynamic relocate information*/
	QList<BbdReloc*> m_bfd_dynreloc_list;
/*Dwarf information of module*/
public:
	int dwarfLoad();
	int dwarfRelease();
	DwfCu *dwarfGetCuByPath(QString path);					/*Get cu by path*/
	DwfCu *dwarfGetCuByAddrIn(unsigned long long addr);		/*Get cu by address in it, address is offset*/
	DwfCu *dwarfGetCuByMacroOff(unsigned long long off);	/*TODO:Get cu*/
	DwfCu *dwarfGetCuByLineOff(unsigned long long off);		/*TODO:Get cu*/
	DwfFrameFde *dwarfGetFdeByAddrIn(unsigned long long addr);
	int dwarfInfoLevel();									/*Get debug level: -1 Failed,0 no debug, 1 with debug, 2 with macro*/
	int m_dwarf_fd;											/*File id*/
	Dwarf_Debug m_dwarf_dbg;								/*Dwarf master*/

    /*Keep memory for Fde register table lazy load*/
	Dwarf_Cie *m_dwarf_cie_data;
    Dwarf_Signed m_dwarf_cie_count;
    Dwarf_Fde *m_dwarf_fde_data;
    Dwarf_Signed m_dwarf_fde_count;

    QMap<long long, DwfFrameCie*> m_dwarf_cie_map;			/*Cie index to Cie*/
	QList<DwfFrameFde*> m_dwarf_fde_list;					/*Fde list*/
	QMap<QString, DwfCu*> m_dwarf_cu_map;					/*Cu map*/
};

/*Break point in target*/
class TargetBreak {
public:
	enum TargetBreakType {
		BREAK_STEP = 0x00000001,	/*Step break point*/
		BREAK_DBG = 0x00000002,		/*Debug break point*/
		BREAK_STUB = 0x00000004,	/*Stub break point*/
		BREAK_RET = 0x00000008,		/*Return break point*/
		BREAK_COVFUN = 0x00000010,	/*Function coverage break point*/
		BREAK_COVFILE = 0x00000020,	/*File coverage break point*/
	};
	TargetBreak()
	{
		m_addr = 0;
		m_type_flag = 0;
		m_attached = false;
		m_token = NULL;
		return;
	}
	unsigned long long m_addr;					/*Address of break point at*/
	unsigned int m_type_flag;					/*Break point type flag*/
	bool m_attached;							/*Attached flag*/
	CovToken *m_token;							/*Token alive on coverage break point*/
};

/*Thread in target*/
class TargetThread {
public:
	enum TargetThreadStat {
		STATUS_PAUSE  	= 0x00000001,	/*Pause: User pause or new thread start*/
		STATUS_STOPED 	= 0x00000002,	/*Stop: Stop by breakpoint signal.., see TargetThreadStopBy*/
		STATUS_STEP 	= 0x00000004,	/*Step: Not stepping, just will to step from pause or stop*/
		STATUS_CONTINUE = 0x00000008,	/*Continue: Not running, just will to continue from pause or stop*/
		STATUS_RUNNING  = 0x00000010,	/*Running: Running now*/
		STATUS_RUNSTEP  = 0x00000020,	/*RunStep: Use step jump the break point which triggered*/
		STATUS_MAX
	};
	enum {
		STATUSSET_STOP = STATUS_PAUSE | STATUS_STOPED,			/*Stopping*/
		STATUSSET_GO = STATUS_STEP | STATUS_CONTINUE,			/*Will to run*/
		STATUSSET_NOTRUN = STATUSSET_STOP | STATUSSET_GO,		/*Not run now*/
		STATUSSET_RUNNING = STATUS_RUNNING | STATUS_RUNSTEP,	/*Running*/
	};
	enum TargetThreadStopBy {
		STOPBY_PAUSE,					/*Stop by pause*/
		STOPBY_SIGNAL,					/*Stop by signal*/
		STOPBY_BREAK,					/*Stop by break point*/
		STOPBY_STEPD,					/*Stop by step*/
		STOPBY_SYSCALLD,				/*Step by syscall*/
		STOPBY_MAX,						/*Not stop*/
	};
	TargetThread()
	{
		m_pid = 0;
		m_stat = STATUS_PAUSE;
		m_stop_by = STOPBY_PAUSE;
		m_triggered = false;
		m_brk_addr = 0;
		m_num_of_signal = 0;
		return;
	}
	unsigned int m_pid;					/*Thread id*/
	QString m_name;						/*Thread name*/
	TargetThreadStat m_stat;			/*Thread status*/
	TargetThreadStopBy m_stop_by;		/*Thread stop by*/
	bool m_triggered;					/*Stop triggered flag*/

	unsigned long long m_brk_addr;		/*Break at address if stop by breakpoint*/
	unsigned int m_num_of_signal;		/*Break signal number if stop by signal*/
};

/*Step information for thread*/
class StepInfo {
public:
	enum StepType {STEP_OVER = 0, STEP_IN, STEP_RETURN, STEP_MAX};
	StepInfo()
	{
		m_valid = true;
		m_pid = 0;
		m_type = STEP_MAX;
		m_step_from = 0;
		m_set_brk_at = 0;
		return;
	}
	bool m_valid;						/*Step work is valid*/
	unsigned int m_pid;					/*Which thread stepping*/
	StepType m_type;					/*Step type*/
	unsigned long long m_step_from;		/*Address of step from*/
	unsigned long long m_set_brk_at;	/*Address of break if continue*/
};

/*Target, alive in main thread*/
class Target: public QObject {
	Q_OBJECT
public:
	enum TargetStat {STATUS_DOWN = 0, STATUS_CONNECTED, STATUS_RUNNING, STATUS_MAX,};
	Target();
/*Event signal*/
signals:
	void signalModAdd(TargetMod*);						/*On new module add to target*/
	void signalModDel(TargetMod*);						/*On module remove from target*/
	void signalStopAction(/*pid*/unsigned int, /*stopby*/unsigned int);	/*After debug stop*/
	void signalRunAction(/*no args*/);					/*Before debug running*/
	void signalShutAction(/*no args*/);					/*On debug shutdown*/
	void signalStatChange(int);							/*On target status change*/
	void signalPtracerEvent();							/*Internal event*/
	void signalTryReadyRead();							/*Internal event*/
/*Service slot*/
public slots:
	void slotThreads(TargetThreadsArg*);
	void slotRegGet(TargetRegGetArg*);
	void slotRegSet(TargetRegSetArg*);
	void slotMemGet(TargetMemGetArg*);
	void slotMemSet(TargetMemSetArg*);
	void slotFunCovSet(TargetFunCovArg*);
	void slotContinue(unsigned int);
private slots:
	void slotConError(QAbstractSocket::SocketError socketError);
	void slotConReady();
	void slotConDiscon();
	void slotPtracerEvent();							/*Internal event*/
/*Target control*/
public:
	int ctrlConnect();											/*Connect local ptracer*/
	int ctrlDisconn();											/*Disconnect*/
	int ctrlStartNew(QString path, QList<QString> arglist, QList<QStringList> envlist);
	int ctrlShut();												/*Shutdown target*/
	int ctrlStop();												/*Stop target*/
	int ctrlContinue();											/*Continue target*/
	int ctrlStep(unsigned int pid);								/*Pure step*/
	int ctrlStep(unsigned int pid, StepInfo::StepType type);	/*Step with type*/
	int ctrlLogLevel(unsigned int level);						/*Set debug level to ptracer*/
private:
	int runInit();
	int runStop(unsigned int pid);
	int runContinue(unsigned int pid, unsigned int signum);
	int runStep(unsigned int pid, unsigned int signum);
/*Base information of target*/
public:
	/*Data convert with number in unsigned long long for format*/
	static unsigned long long dataToNum(unsigned long long mem, unsigned int size, int signal);
	static unsigned long long NumToData(unsigned long long data, unsigned int size, int signal);
	/*Data convert with bytes*/
	int dataToBytes(unsigned long long in, int bytesize, QByteArray &bytes);
	int bytesToData(unsigned long long &out, int bytesize, QByteArray &bytes);

	int statSet(TargetStat stat);
	QMutex m_data_lock;
	TargetStat m_status;
	static const char *m_status_desc[];
	unsigned int m_run_pid;								/*Main thread id*/
	bool m_run_flag_starting;							/*Flag for starting*/
	bool m_run_flag_stopping;							/*Flag for stopping*/
	bool m_run_flag_breaking;							/*Flag for breaking, dependent on m_run_flag_stopping*/
/*Target operate*/
public:
	Arch *archInfoGet();
	QString archAddr(unsigned long long addr);
	QString arch0xAddr(unsigned long long addr) {return "0x" + archAddr(addr);}
	QString arch0XAddr(unsigned long long addr) {return "0X" + archAddr(addr);}
	RegValues *pidRegsGet(unsigned int pid);
	StackInfo *pidStackGet(unsigned int pid);
	int pidRegPcGet(unsigned int pid, unsigned long long &pc);
	int pidRegSpGet(unsigned int pid, unsigned long long &stack);
	int pidRegGet(unsigned int pid, QString name, unsigned long long &value);
	int pidRegSet(unsigned int pid, QString name, unsigned long long value);
	int memoryGet(unsigned long long addr, unsigned int size, QByteArray &array);
	int memorySet(unsigned long long addr, unsigned int size, QByteArray &array);
	int breakSet(unsigned long long addr);
	int breakClear(unsigned long long addr);
/*Map and Module*/
public:
	TargetMod *modGetByPath(QString path);				/*Get module by remote path*/
	TargetMod *modGetByAddrIn(unsigned long long addr);	/*Get module by address in*/
	int mapUpdate();									/*Map update from target*/
	int modUpdate();									/*Module update*/
	int mapModClear();
	QList<TargetMap*> m_map_items;
	QMap<QString, TargetMod*> m_mod_items;
/*Malloc and free at*/
public:
	int mallocFreeGet(unsigned long long &malloc, unsigned long long &free);
private:
	int mallocFreeUpdate(TargetMod *mod_file);
	unsigned long long m_malloc_addr;
	unsigned long long m_free_addr;
/*Thread of target*/
public:
	bool threadsStatInRange(unsigned int check);		/*All thread status check*/
	TargetThread *threadGetById(unsigned int pid);		/*Get thread by id*/
	int threadUpdate();									/*Update thread name*/
	int threadClear();
	QMap<unsigned int, TargetThread*> m_thread_set;
	QMap<unsigned int, StepInfo*> m_step_set;			/*Thread step control blocks*/
/*Break point in taret*/
public:
	void breakChanged();
	TargetBreak *breakGetByAddr(unsigned long long addr);
	int breakInsert(unsigned long long addr, TargetBreak::TargetBreakType type, CovToken *token = NULL);
	int breakDelete(unsigned long long addr, TargetBreak::TargetBreakType type);
private:
	int breakAttach();
	int breakDetach();
	QMap<unsigned long long, TargetBreak*> m_break_set;
/*Main job*/
private:
	void deliverStopAction(unsigned int pid, TargetThread::TargetThreadStopBy stopby);
	int deliverStopStep(TargetThread *thread);
	int deliverStopBreak(TargetThread *thread);
	int deliverStop(TargetThread *thread);
	int postStopEvent(TargetThread *thread);
	int doUpdateOnStop();
	int doGoOnProc();
	int doShutDown();
/*Ptracer communication and event*/
private:
	int doEventStartOk(QString &event, QString &event_head);
	int doEventStartFail(QString &event, QString &event_head);
	int doEventThreadStart(QString &event, QString &event_head);
	int doEventThreadEnd(QString &event, QString &event_head);
	int doEventThreadKilled(QString &event, QString &event_head);
	int doEventThreadExited(QString &event, QString &event_head);
	int doEventFork(QString &event, QString &event_head);
	int doEventExec(QString &event, QString &event_head);
	int doEventStop(QString &event, QString &event_head);
	int doEventSignal(QString &event, QString &event_head);
	int doEventBreak(QString &event, QString &event_head);
	int doEventStep(QString &event, QString &event_head);
	int doEventSyscall(QString &event, QString &event_head);
	int doEventStdOut(QString &event, QString &event_head);
	int doEvent(QString &event);
	struct ListNode {QString key;QString value;};
	QList<ListNode> msgToList(QStringRef content);
	QMap<QString,QString> msgToMap(QStringRef content);
	QString msgEncode(QString input);
	QString msgDecode(QString input);
	QString ptracerReq(QString &request);

	QTcpSocket *m_ptracer_socket;
	bool m_ptracer_receiving;
	QByteArray m_ptracer_recv;
	QList<QByteArray> m_ptracer_events;
};

extern Target *g_target;

#endif /* SRC_TARGET_H_ */
