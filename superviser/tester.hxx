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

#ifndef SRC_TESTER_H_
#define SRC_TESTER_H_

#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QList>
#include <QtCore/QWaitCondition>
#include <QtNetwork/QLocalSocket>

#include <uter_public.h>
#include <uter_signal_arg.h>

/*Data description for function call and stub, include arguments and return*/
class CallDataDesc {
public:
	CallDataDesc()
	{
		m_addr = 0;
		m_desc = "";
		m_len = 0;
		m_align = 0;
		return;
	}
	unsigned long long m_addr;		/*Alive for call, useless for stub*/
	QString m_desc;					/*Data type*/
	unsigned int m_len;				/*Data length*/
	unsigned int m_align;			/*Data aligned length*/
};

/*Description for function call and stub*/
class CallFunDesc {
public:
	enum ReturnType {
		CALL_RET_EAX = 0,		/*Return char,short,int data*/
		CALL_RET_EAXEDX,		/*Return longlong data*/
		CALL_RET_VOID,			/*Return void*/
		CALL_RET_POINT,			/*Return point*/
		CALL_RET_MAX
	};
	CallFunDesc()
	{
		m_call_type = CALL_RET_MAX;
		return;
	}
	~CallFunDesc()
	{
		QList<CallDataDesc*>::iterator iter;
		for (iter = m_args.begin(); iter != m_args.end(); /*none*/) {
			CallDataDesc *desc = *iter;
			delete desc;
			m_args.erase(iter++);
		}
		return;
	}
	ReturnType m_call_type;				/*Return type*/
	CallDataDesc m_ret;					/*Return data description*/
	QList<CallDataDesc*> m_args;		/*Arguments data description*/
};

/*
 * Thread for python output, python interpreter output stdout and stderr to a standard file id.
 * The python thread can not read output message timely, so use the special thread.
 * The thread make socket from pipe handle which connect to the python stdout and stderr,
 * read the local socket and send message to UI.
 * */
class PyOutputTask: public QThread {
	Q_OBJECT
public:
	PyOutputTask(int stdout, int stderr)
	:QThread()
	{
		moveToThread(this);
		m_py_pipe_stdout = stdout;
		m_py_pipe_stderr = stderr;
		m_py_lsock_stdout = NULL;
		m_py_lsock_stderr = NULL;
	}
	void run()
	{
		int result;
		/*Set thread name*/
		prctl(PR_SET_NAME, (unsigned long)"Tester_PythonOut");
		/*Local socket*/
		m_py_lsock_stdout = new QLocalSocket(this);
		m_py_lsock_stderr = new QLocalSocket(this);
		/*Set pipe handle to local socket*/
		result = m_py_lsock_stdout->setSocketDescriptor(m_py_pipe_stdout,
				                     QLocalSocket::ConnectedState, QLocalSocket::ReadOnly);
		UTER_ASSERT(result == true);
		result = m_py_lsock_stderr->setSocketDescriptor(m_py_pipe_stderr,
				                     QLocalSocket::ConnectedState, QLocalSocket::ReadOnly);
		UTER_ASSERT(result == true);

		QMetaObject::Connection conn;
	    conn = connect(m_py_lsock_stdout, SIGNAL(readyRead()), this, SLOT(slotPyStdoutReady()));
	    UTER_ASSERT(conn != NULL);
	    conn = connect(m_py_lsock_stderr, SIGNAL(readyRead()), this, SLOT(slotPyStderrReady()));
	    UTER_ASSERT(conn != NULL);
	    /*Thread main loop*/
		QThread::exec();
	}
private slots:
	void slotPyStdoutReady()
	{
		while(1) {
			QByteArray line = m_py_lsock_stdout->readLine();
			if (line.isEmpty()) {
				return;
			}
			emit g_ui_main.signalPythonOutput(1, line);
		}
		return;
	}
	void slotPyStderrReady()
	{
		while(1) {
			QByteArray line = m_py_lsock_stderr->readLine();
			if (line.isEmpty()) {
				return;
			}
			emit g_ui_main.signalPythonOutput(2, line);
		}
		return;
	}
private:
	/*Pipe socket*/
	int m_py_pipe_stdout;
	int m_py_pipe_stderr;
	/*Local socket*/
    QLocalSocket *m_py_lsock_stdout;
    QLocalSocket *m_py_lsock_stderr;
};

/*Stub register table*/
typedef struct _object PyObject;
class TesterStub {
public:
	TesterStub()
	{
		m_address = 0;
		m_fun_desc = NULL;
		m_driver_arg_obj = NULL;
		return;
	}
	unsigned long long m_address;			/*Stub address*/
	CallFunDesc *m_fun_desc;				/*Stub function description*/
	PyObject *m_driver_arg_obj;				/*Stub register argument, keep it by reference count*/
};

/*Tester running stack*/
class TesterStack {
public:
	enum TesterStackType {
		STACK_SCRIPT = 1,
		STACK_CALL,
		STACK_STUB,
		STACK_MAX
	};
	TesterStack()
	{
		memset(&m_data, 0, sizeof(m_data));
		m_type = STACK_MAX;
		m_pid = 0;
		return;
	}
	TesterStack(TesterStack::TesterStackType type, unsigned int pid,
			    unsigned long long stack, unsigned long long retto)
	{
		UTER_ASSERT(type == TesterStack::STACK_CALL);
		memset(&m_data, 0, sizeof(m_data));
		m_type = type;
		m_pid = pid;
		m_data.call.m_stack = stack;
		m_data.call.m_retto = retto;
		return;
	}
	TesterStack(TesterStack::TesterStackType type, unsigned int pid)
	{
		UTER_ASSERT(type == TesterStack::STACK_STUB);
		memset(&m_data, 0, sizeof(m_data));
		m_type = type;
		m_pid = pid;
		return;
	}
	TesterStack(TesterStack::TesterStackType type)
	{
		UTER_ASSERT(type == TesterStack::STACK_SCRIPT);
		memset(&m_data, 0, sizeof(m_data));
		m_type = type;
		m_pid = 0;
		return;
	}
	TesterStackType m_type;						/*Stack node type*/
	unsigned int m_pid;							/*Pid*/
	union {
		struct {
			unsigned long long m_retto;			/*Return to address, if eip same as it, the call may be return*/
			unsigned long long m_stack;			/*Stack address, check stack point when break at return to address*/
		} call;
	} m_data;
};

/*
 * TesterEvent and TesterEventLoop, actualize a event loop on the top of stack.
 * For example: tester use python script running call, it is can not return to stack bottom and receive event,
 * so use TesterEvent and TesterEventLoop process event on the stack top, and break when receive custom event.
 * */
class TesterEvent: public QEvent {
public:
	enum TesterEventType {
		EVENT_JOB_PYTHON = QEvent::User + 1,
		EVENT_JOB_TARGET,
		EVENT_MAX,
	};

	TesterEvent(TesterEventType type)
	: QEvent((QEvent::Type)type)
	{
		return;
	}
};
class TesterEventLoop: public QEventLoop {
public:
	TesterEventLoop(QObject *obj)
	{
		m_obj = obj;
		m_obj->installEventFilter(this);
		return;
	}
	~TesterEventLoop()
	{
		m_obj->removeEventFilter(this);
		return;
	}
	bool eventFilter(QObject *obj, QEvent *event)
	{
		/*Break with QEventLoop::exit when custom event*/
	    if(event->type() == (QEvent::Type)TesterEvent::EVENT_JOB_PYTHON
	    	|| event->type() == (QEvent::Type)TesterEvent::EVENT_JOB_TARGET) {
	    	QEventLoop::exit(0);
	    	return true;
	    }
		return QEventLoop::eventFilter(obj, event);
	}

	QObject *m_obj;
};

/*TesterJob*/
class TesterJob {
public:
	enum TesterJobType {
		JOB_TARGET = 1,
		JOB_PYTHON,
		JOB_MAX,
	};
	TesterJob(TesterJob::TesterJobType job)
	{
		m_job = job;
		return;
	}
	virtual ~TesterJob()
	{
		return;
	}
	TesterJobType m_job;
};

/*Tester job for target event*/
class TesterJobTarget: public TesterJob {
public:
	enum TesterTargetType {
		TARGET_STUB = 0,
		TARGET_RET,
		TARGET_MAX
	};
	TesterJobTarget(TesterJobTarget::TesterTargetType type,
				 unsigned int on_pid, unsigned long long at_addr)
	: TesterJob(TesterJob::JOB_TARGET)
	{
		m_type = type;
		m_on_pid = on_pid;
		m_at_addr = at_addr;
		return;
	}
	TesterTargetType m_type;
	unsigned int m_on_pid;
	unsigned long long m_at_addr;
};

/*Tester job for python event*/
class TesterJobPython: public TesterJob {
public:
	enum TesterPythonType {
		PYTHON_COMMAND = 0,
		PYTHON_TESTFILE,
		PYTHON_TESTDIR,
		PYTHON_MAX,
	};

	TesterJobPython(TesterPythonType type, QString &string)
	: TesterJob(TesterJob::JOB_PYTHON)
	{
		m_type = type;
		m_string = string;
		return;
	}
	TesterPythonType m_type;
	QString m_string;
};

/*Tester work with customized thread*/
class Tester: public QThread {
	Q_OBJECT
public:
	Tester();
signals:
	void signalTargetThreads(TargetThreadsArg*);
	void signalTargetMemGet(TargetMemGetArg*);
	void signalTargetMemSet(TargetMemSetArg*);
	void signalTargetRegGet(TargetRegGetArg*);
	void signalTargetRegSet(TargetRegSetArg*);
	void signalTargetCovSet(TargetFunCovArg*);
	void signalTargetContinue(unsigned int);
	void signalProjectCaseDirs(QList<QString>*);				/*Get project information from UI*/
	void signalStatusChanged(/*no use*/int, /*status*/int);		/*Tester status change notification*/
	void signalPythonRun();										/*Python script execute begin*/
	void signalPythonFinish();									/*Python script execute finish*/
public:
	/*Encapsulation of block signal*/
	int memoryGet(unsigned long long addr, unsigned int size, QByteArray &array);
	int memorySet(unsigned long long addr, unsigned int size, QByteArray &array);
	int registerGet(unsigned int pid, QString name, unsigned long long &value);
	int registerSet(unsigned int pid, QString name, unsigned long long value);
	void run();												/*Override QThread run*/
	void stackPush(TesterStack *stacknode);					/*Stack push*/
	TesterStack *stackPop();								/*Stack pop*/
	int callWaitReturn(TesterStack *stacknode);				/*Call function in target and wait to return*/
	int targetReturnTriggerd(unsigned long long addr, unsigned int tid, unsigned long long stack);		/*Use for trigger return*/
	int targetStubTriggerd(unsigned long long addr, unsigned int tid);									/*Use for trigger stub*/
	int pythonRun(TesterJobPython::TesterPythonType type, QString &string);								/*Run python command*/
	QStringList pythonAutoCompletion(QString &input);
	unsigned int callSelectThread();						/*Get thread in to call*/
private:
	/*Process event on the bottom*/
	int jobEventProc();
	bool event(QEvent *event)
	{
	    if(event->type() == (QEvent::Type)TesterEvent::EVENT_JOB_PYTHON
	    	|| event->type() == (QEvent::Type)TesterEvent::EVENT_JOB_TARGET) {
	    	jobEventProc();
	    	return true;
	    }
	    return QObject::event(event);
	}

	QMutex m_data_lock;
	QMap<unsigned long long, TesterStub*> m_stub_map;			/*Stub register table*/
	QList<TesterStack*> m_inter_stack;							/*Call and Stub Stack*/
	QList<TesterJob*> m_job_list;								/*Job query*/

	int flushStatus();
	bool m_python_ready;
/*Python output*/
public:
	/*Python output interface, write message to pipe[1]*/
	int pythonStdOut(QString str);
	int pythonStdErr(QString str);
private:
	int pythonInit();
    /*stdout and stderr pipe, pipe[0] for read, pipe[1] for write*/
    int m_py_pipe_stdout[2];
    int m_py_pipe_stderr[2];
    /*python output thread, read message from pipe[0]*/
    PyOutputTask *m_py_output;

	friend PyObject *UterPythonExtern_Stub(PyObject *self, PyObject *args, PyObject *kw);
	friend PyObject *UterPythonExtern_StubAdmin(PyObject *self, PyObject *args, PyObject *kw);
	friend int TestStub(unsigned long long addr, unsigned int tid);
};

extern Tester *g_tester;

int UterMalloc(unsigned int pid, unsigned int len, unsigned long long *point);
int UterFree(unsigned int pid, unsigned long long point);
int TestCall(unsigned int pid, unsigned long long fun_addr, CallFunDesc *fun_desc);
int TestStub(unsigned long long addr, unsigned int tid);
int TestStub(unsigned long long addr, unsigned int tid);

#endif /* SRC_TESTER_H_ */
