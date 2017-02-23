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

#ifndef SRC_PTRACER_H_
#define SRC_PTRACER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stddef.h>
#include <signal.h>
#include <uuid/uuid.h>
#include <map>

using namespace std;

#define PTRACER_BREAK_SIZE (4)

struct PtracerWait {
	enum PtracerWaitEvent {
	    EVENT_EXITD = 0,			/*Thread exited*/
		EVENT_KILLED ,				/*Thread killed*/
		EVENT_DUMPED,				/*Thread killed and core dumped*/
		EVENT_STOPED,				/*Thread stoped*/
		EVENT_CONTINUED,			/*Thread continued*/
		EVENT_MAX,
	};

	static const char *event_desc[];

	int pid;						/*Pid of event*/
	int status;						/*Status code of event*/
	PtracerWaitEvent event;			/*Event type*/
    int num;						/*Number of some event, eg signal number, exit code*/

    /*event flag*/
    bool event_clone;
    bool event_exec;
    bool event_exit;
    bool event_fork;
    bool event_vfork;
    bool event_vforkdone;
};

class PtracerThread {
public:
	enum PtracerThreadStatus {
		STATUS_INIT = 0,		/*No use*/
		STATUS_NEW,				/*After fork + PTRACE_TRACEME, wait stop*/
		STATUS_ATTACH,			/*After PTRACE_ATTACH, wait stop*/
		STATUS_CLONE,			/*Thread cloned, wait stop*/
		STATUS_FORK,			/*Thread forked, wait stop*/
		STATUS_WAIT,			/*Thread stop but not known clone or stop*/
		STATUS_STOP,			/*Thread stoped*/
		STATUS_RUN,				/*Thread running*/
		STATUS_PREEND,			/*Before thread end*/
		STATUS_ENDING,			/*Thread ending*/
		STATUS_END,				/*Thread end, no use*/
		STATUS_MAX,
	};

	static const char *status_desc[];

	PtracerThread()
	{
		m_tid = 0;
		m_status = STATUS_INIT;
		m_flag_take_stop = false;
		return;
	}

	int statusSet(PtracerThreadStatus status)
	{
		if (m_status == STATUS_INIT) {
			PTRACER_DEBUG("Create thread information for thread %d with status %s.",
					        m_tid, status_desc[status]);
			m_status = status;
			return 0;
		}

		PTRACER_DEBUG("Change status form %s to %s for thread %d.",
					   status_desc[m_status], status_desc[status], m_tid);
		m_status = status;
		if (m_status == STATUS_STOP && m_flag_take_stop) {
			PTRACER_DEBUG("Stop on signal SIGSTOP send for thread %d.", m_tid);
			m_flag_take_stop = false;
		}
		return 0;
	}

	unsigned int m_tid;				/*Thread pid*/
	PtracerThreadStatus m_status;	/*Thread status*/
	bool m_flag_take_stop;			/*take stop by superviser*/
};

/*Save memory on the break*/
class PtracerBreak {
public:
	PtracerBreak(unsigned long long add)
	{
		m_address = add;
	}
	PtracerBreak()
	{
		m_address = 0;
	}
	unsigned long long m_address;
	char m_backup[PTRACER_BREAK_SIZE];
};

class PtracerArch;
class Ptracer {
public:
	Ptracer();
    int init(unsigned int addr, unsigned short port, int loglevel);
    int wait();
    int work();
private:
    PtracerBreak *breakGetByAddr(const unsigned long long addr);
    int breakDelByAddr(const unsigned long long addr);
    int breakInsert(PtracerBreak *brkpnt);
	map<unsigned long long, PtracerBreak*> m_break_set;

	PtracerThread *threadGetById(unsigned int pid);
	int threadDelById(unsigned int pid);
	int threadInsert(PtracerThread *thread);
	map<unsigned int, PtracerThread*> m_thread_set;

	bool someThreadAlive();

	unsigned int m_pid;					/*Process pid*/
	PtracerArch *m_arch;

	int postShutDown();
    int waitExited(PtracerWait &waitinfo, int exit_code);
    int waitKilled(PtracerWait &waitpinfo, int signal_num);
    int waitStopWithClone(PtracerWait &waitinfo, int child_pid);
    int waitStopWithFork(PtracerWait &waitinfo, int child_pid);
    int waitStopWithExec(PtracerWait &waitinfo, int former_pid);
    int waitStopWithVfork(PtracerWait &waitinfo, int pid);
    int waitStopWithVforkDone(PtracerWait &waitinfo, int pid);
    int waitStopWithExit(PtracerWait &waitinfo, int status);
    int waitStopButNoSiginfo(PtracerWait &waitinfo);
    int waitStopBySignalStop(PtracerWait &waitinfo, siginfo_t &siginfo);
    int waitStopBySignalTrapUser(PtracerWait &waitinfo, siginfo_t &siginfo);
    int waitStopBySignalTrapBreak(PtracerWait &waitinfo, siginfo_t &siginfo);
    int waitStopBySignalTrapStep(PtracerWait &waitinfo, siginfo_t &siginfo);
    int waitStopBySignalTrapSyscall(PtracerWait &waitinfo, siginfo_t &siginfo);
    int waitStopBySignalTrapNormal(PtracerWait &waitinfo, siginfo_t &siginfo);
    int waitStopBySignalNormal(PtracerWait &waitinfo, siginfo_t &siginfo);
    int waitStopBySignal(PtracerWait &waitinfo, siginfo_t &siginfo);
    int waitProc(PtracerWait &waitinfo);

    pthread_t m_wait_thread;			/*Wait thread*/
    pthread_mutex_t m_wait_lock;
    pthread_cond_t m_wait_cond;
    unsigned int m_wait_pid;

    /*Domain socket for wait thread*/
    uuid_t m_inner_msg_uuid;
    char m_inner_msg_uuid_str[128];
    int m_inner_main_sockfd;
    int m_inner_server_sockfd;
    int m_inner_client_sockfd;

    int m_child_pipe_stdin;				/*No use now*/
    int m_child_pipe_stdout;

    string doRequestInit(string &request, string &request_head, string &response_head);
    string doRequestLogLevel(string &request, string &request_head, string &response_head);
    string doRequestStartNew(string &request, string &request_head, string &response_head);
    string doRequestInstrGet(string &request, string &request_head, string &response_head);
    string doRequestStackGet(string &request, string &request_head, string &response_head);
    string doRequestRegGet(string &request, string &request_head, string &response_head);
    string doRequestRegSet(string &request, string &request_head, string &response_head);
    string doRequestMemGet(string &request, string &request_head, string &response_head);
    string doRequestMemSet(string &request, string &request_head, string &response_head);
    string doRequestBreakSet(string &request, string &request_head, string &response_head);
    string doRequestBreakClear(string &request, string &request_head, string &response_head);
    string doRequestMapGet(string &request, string &request_head, string &response_head);
    string doRequestThreadsGet(string &request, string &request_head, string &response_head);
    string doRequestShut(string &request, string &request_head, string &response_head);
    string doRequestStop(string &request, string &request_head, string &response_head);
    string doRequestStep(string &request, string &request_head, string &response_head);
    string doRequestContinue(string &request, string &request_head, string &response_head);
    string doRequest(string &request);
    int sendEvent(string &event);
    struct ListNode {string key;string value;};
    list<ListNode> msgToList(string &content);
    map<string,string> msgToMap(string &content);
    string msgEncode(string &input);
    string msgDecode(string &input);

    int connClosed();

    pthread_t m_work_thread;				/*Work thread*/
    int m_outer_main_sockfd;				/*Work socket*/
    int m_outer_server_sockfd;
};

#endif /* SRC_PTRACER_H_ */
