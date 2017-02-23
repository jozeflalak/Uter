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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ptrace.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <inter_msg.h>
#include <ptracer_arch.h>
#include <ptracer_arch_x86.h>
#include <ptracer_pub.h>
#include <ptracer.h>

const char *PtracerWait::event_desc[] = {
	"EVENT_EXITD",
	"EVENT_KILLED",
	"EVENT_DUMPED",
	"EVENT_STOPED",
	"EVENT_CONTINUED"
};

const char *PtracerThread::status_desc[] =
{
	"STATUS_INIT",
	"STATUS_NEW",
	"STATUS_ATTACH",
	"STATUS_CLONE",
	"STATUS_FORK",
	"STATUS_WAIT",
	"STATUS_STOP",
	"STATUS_RUN",
	"STATUS_PREEND",
	"STATUS_ENDING",
	"STATUS_END",
};

/*PTRACER_OPTION for a thread*/
#define PTRACER_OPTION (PTRACE_O_EXITKILL \
		                | PTRACE_O_TRACECLONE \
					    | PTRACE_O_TRACEEXEC \
					    | PTRACE_O_TRACEEXIT \
					    | PTRACE_O_TRACEFORK \
					    | PTRACE_O_TRACEVFORK \
					    | PTRACE_O_TRACEVFORKDONE)

/*Send a signal to a thread, not support by glibc, so need syscall*/
int tgkill(pid_t pid, pid_t tid, int sig)
{
    return syscall(SYS_tgkill, pid, tid, sig);
}

/*Do waitpid and save wait information to ptracer wait struct*/
int doWait(pid_t pid, PtracerWait *pinfo)
{
	PTRACER_ASSERT(pinfo != NULL);

	int status = 0;
	errno = 0;
    pid_t ret = waitpid(pid, &status, __WALL);
    if (ret <= 0) {
    	return -1;
    }

    pinfo->pid = ret;
    pinfo->status = status;

    if (WIFEXITED(status)) {
    	/*Child exit happened*/
    	pinfo->event = PtracerWait::EVENT_EXITD;
    	pinfo->num = WEXITSTATUS(status);			/*Get child exit code*/
    } else if (WIFSIGNALED(status) && WCOREDUMP(status)) {
    	/*Child terminated by signal and coredump*/
    	pinfo->event = PtracerWait::EVENT_DUMPED;
    	pinfo->num = WTERMSIG(status);				/*Get signal number*/
    } else if (WIFSIGNALED(status)) {
    	/*Child terminated by signal*/
    	pinfo->event = PtracerWait::EVENT_KILLED;
    	pinfo->num = WTERMSIG(status);				/*Get signal number*/
    } else if (WIFSTOPPED(status)) {
    	/*Child stoped*/
    	pinfo->event = PtracerWait::EVENT_STOPED;
    	pinfo->num = WSTOPSIG(status);				/*Get signal number*/
    } else if (WIFCONTINUED(status)) {
    	/*Child continued*/
    	pinfo->event = PtracerWait::EVENT_CONTINUED;
    	pinfo->num = SIGCONT;						/*Get signal number*/
    } else {
    	errno = 0;
    	return -1;
    }

    /*Flag of child stop reason*/
    if (pinfo->event == PtracerWait::EVENT_STOPED) {
    	/*Flag of clone*/
        if (status>>8 == (SIGTRAP | (PTRACE_EVENT_CLONE<<8))) {
        	pinfo->event_clone = true;
        } else {
        	pinfo->event_clone = false;
        }
        /*Flag of exec*/
        if (status>>8 == (SIGTRAP | (PTRACE_EVENT_EXEC<<8))) {
        	pinfo->event_exec = true;
        } else {
        	pinfo->event_exec = false;
        }
        /*Flag of exit*/
        if (status>>8 == (SIGTRAP | (PTRACE_EVENT_EXIT<<8))) {
        	pinfo->event_exit = true;
        } else {
        	pinfo->event_exit = false;
        }
        /*Flag of fork*/
        if (status>>8 == (SIGTRAP | (PTRACE_EVENT_FORK<<8))) {
        	pinfo->event_fork = true;
        } else {
        	pinfo->event_fork = false;
        }
        /*Flag of vfork*/
        if (status>>8 == (SIGTRAP | (PTRACE_EVENT_VFORK<<8))) {
        	pinfo->event_vfork = true;
        } else {
        	pinfo->event_vfork = false;
        }
        /*Flag of vforkdone*/
        if (status>>8 == (SIGTRAP | (PTRACE_EVENT_VFORK_DONE<<8))) {
        	pinfo->event_vforkdone = true;
        } else {
        	pinfo->event_vforkdone = false;
        }
    }

    return 0;
}

/*Thread for wait child event*/
void *ptracer_wait_thread(void *arg)
{
	PTRACER_ASSERT(arg != NULL);
	prctl(PR_SET_NAME, (unsigned long)"Ptracer_WaitPid");
	Ptracer *pt = (Ptracer *)arg;
	pt->wait();		/*never return*/
	exit(0);
}

/*Thread for ptracer work*/
void *ptracer_work_thread(void *arg)
{
	PTRACER_ASSERT(arg != NULL);
	prctl(PR_SET_NAME, (unsigned long)"Ptracer_Work");
	Ptracer *pt = (Ptracer *)arg;
	pt->work();		/*never return*/
	exit(0);
}

Ptracer::Ptracer()
{
	m_pid = 0;
	m_arch = NULL;

	uuid_generate_time(m_inner_msg_uuid);
	m_inner_msg_uuid_str[0]= '\0';
	strcat(m_inner_msg_uuid_str, "/var/tmp/");
	uuid_unparse_lower(m_inner_msg_uuid, m_inner_msg_uuid_str + strlen(m_inner_msg_uuid_str));
    m_inner_main_sockfd = -1;
    m_inner_server_sockfd = -1;
    m_inner_client_sockfd = -1;

    m_child_pipe_stdin = -1;
    m_child_pipe_stdout = -1;

    m_outer_main_sockfd = -1;
    m_outer_server_sockfd = -1;

    m_wait_thread = 0;
    m_work_thread = 0;

    pthread_mutex_init(&m_wait_lock, NULL);
    pthread_cond_init(&m_wait_cond, NULL);
    m_wait_pid = 0;

	return;
}

int Ptracer::init(unsigned int addr, unsigned short port, int loglevel)
{
	int result;

	ptracer_log_level = loglevel;

    /*Outer socket*/
    struct sockaddr_in in_addr;
    m_outer_main_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_outer_main_sockfd < 0) {
		PTRACER_LOG("Create socket failed, result %d, errno %d.", m_outer_main_sockfd, errno);
		goto init_error;
	}
    if (1) {
    	int opt = 1;
    	result = setsockopt(m_outer_main_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    	if (result != 0) {
    		PTRACER_LOG("Set option SO_REUSEADDR failed, result %d, errno %d.", result, errno);
    	}

    	struct linger so_linger;
    	so_linger.l_onoff = 1;
    	so_linger.l_linger = 0;
    	result = setsockopt(m_outer_main_sockfd, SOL_SOCKET, SO_LINGER, &so_linger, sizeof(so_linger));
    	if (result != 0) {
    		PTRACER_LOG("Set option SO_LINGER failed, result %d, errno %d.", result, errno);
    	}
    }
    bzero(&in_addr, sizeof(in_addr));
    in_addr.sin_family = AF_INET;
    in_addr.sin_port = port;
    in_addr.sin_addr.s_addr = addr;
	result = bind(m_outer_main_sockfd, (struct sockaddr *)&in_addr, sizeof(in_addr));
    if (result != 0) {
    	PTRACER_LOG("Bind failed, result %d, errno %d.", result, errno);
    	goto init_error;
    }
    result = listen(m_outer_main_sockfd, 5);
    if (result != 0) {
    	PTRACER_LOG("Listen failed, result %d, errno %d.", result, errno);
    	goto init_error;
    }

    /*Inner socket*/
	struct sockaddr_un un_addr;
	m_inner_main_sockfd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
	if (m_inner_main_sockfd < 0) {
		PTRACER_LOG("Create socket failed, result %d, errno %d.", m_inner_main_sockfd, errno);
		goto init_error;
	}
	un_addr.sun_family = AF_UNIX;
	strcpy(un_addr.sun_path, m_inner_msg_uuid_str);
	result = bind(m_inner_main_sockfd, (struct sockaddr *)&un_addr, sizeof(un_addr));
    if (result != 0) {
    	PTRACER_LOG("Bind failed, result %d, errno %d.", result, errno);
    	goto init_error;
    }
    result = listen(m_inner_main_sockfd, 5);
    if (result != 0) {
    	PTRACER_LOG("Listen failed, result %d, errno %d.", result, errno);
    	goto init_error;
    }

    /*Support X86 only*/
	m_arch = new PtracerArchX86();

    /*Work thread*/
    result = pthread_create(&m_work_thread, NULL, ptracer_work_thread, this);
    if (result != 0) {
    	PTRACER_LOG("Pthread_create failed, result %d, errno %d.", result, errno);
    	goto init_error;
    }

    /*Wait thread*/
    result = pthread_create(&m_wait_thread, NULL, ptracer_wait_thread, this);
    if (result != 0) {
    	PTRACER_LOG("Pthread_create failed, result %d, errno %d.", result, errno);
    	goto init_error;
    }

    return 0;

init_error:
	printf("Ptracer module initialize failed.");
	exit(0);
}

/*Wait thread looper*/
int Ptracer::wait()
{
	/*Configure domain socket*/
	m_inner_client_sockfd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
	if (m_inner_client_sockfd < 0) {
		PTRACER_LOG("Create socket failed, result %d, errno %d.", m_inner_client_sockfd, errno);
		return -1;
	}
	struct sockaddr_un connect_addr;
	connect_addr.sun_family = AF_UNIX;
	strcpy(connect_addr.sun_path, m_inner_msg_uuid_str);
	int result = connect(m_inner_client_sockfd, (struct sockaddr *)&connect_addr, sizeof(connect_addr));
	if(result < -1) {
		PTRACER_LOG("Connect failed, result %d, errno %d.", result, errno);
		return -1;
	}

	/*Loop forever*/
	while (1) {
		/*Wait start*/
		unsigned int m_pid;
		while(1) {
			pthread_mutex_lock(&m_wait_lock);
			if (m_wait_pid == 0) {
	            pthread_cond_wait(&m_wait_cond, &m_wait_lock);
	            pthread_mutex_unlock(&m_wait_lock);
	            continue;
			} else {
				m_pid = m_wait_pid;
				m_wait_pid = 0;
			}
			pthread_mutex_unlock(&m_wait_lock);
			break;
		}

		while(1) {
			PtracerWait info;
		    int result = doWait(-1, &info);
	        if (result < 0) {
	        	PTRACER_DEBUG("No child to wait.");
	            break;
	        }

	        PTRACER_DEBUG("Wait a event with pid = %d, status = 0x%08x, event = %s, num = %d, flags:"
	        			"(%s %s %s %s %s %s ).",
	        		    info.pid, info.status, PtracerWait::event_desc[info.event], info.num,
						info.event_clone?"clone":"", info.event_exec?"exec":"",
						info.event_exit?"exit":"", info.event_fork?"fork":"",
						info.event_vfork?"vfork":"", info.event_vforkdone?"vforkdone":"");
	        /*Send to worker*/
	        ssize_t sendbyts = send(m_inner_client_sockfd, &info, sizeof(info), 0);
	        if (sendbyts != sizeof(info)) {
	        	PTRACER_LOG("Send wait information failed, result %d, errno %d.", sendbyts, errno);
	        	continue;
	        }
		}
	}

	return 0;
}

/*Ptracer work looper*/
int Ptracer::work()
{
	/*Accept from wait thread connect*/
	struct sockaddr_un client_addr;
	socklen_t len = sizeof(client_addr);
    m_inner_server_sockfd = accept(m_inner_main_sockfd, (struct sockaddr *)&client_addr, &len);
    if (m_inner_server_sockfd < 0) {
    	PTRACER_LOG("Accept failed, result %d, errno %d.", m_inner_server_sockfd, errno);
    	return -1;
    }

    int std_buf_size = INTER_MSG_MAX/2;
    char *stdout_msg_buf = (char *)malloc(std_buf_size);
    if (stdout_msg_buf == NULL) {
    	PTRACER_ASSERT(0);
    	return -1;
    }
    char *stderr_msg_buf = (char *)malloc(std_buf_size);
    if (stderr_msg_buf == NULL) {
    	PTRACER_ASSERT(0);
    	return -1;
    }

    /*Receive message buffer*/
    char *out_msg_buf = (char *)malloc(INTER_MSG_MAX);
    if (out_msg_buf == NULL) {
    	PTRACER_ASSERT(0);
    	return -1;
    }
    unsigned int out_msg_buf_off = 0;

    while(1) {
    	ssize_t sizeresult;
    	int nfds;
    	fd_set rfds;

        FD_ZERO(&rfds);
        nfds = 0;

        FD_SET(m_inner_server_sockfd, &rfds);
        if (m_inner_server_sockfd > nfds) {
        	nfds = m_inner_server_sockfd;
        }

        if (m_child_pipe_stdout != -1) {
            FD_SET(m_child_pipe_stdout, &rfds);
            if (m_child_pipe_stdout > nfds) {
            	nfds = m_child_pipe_stdout;
            }
        }

        if (m_outer_server_sockfd == -1) {
        	/*Waitting connect*/
            FD_SET(m_outer_main_sockfd, &rfds);
            if (m_outer_main_sockfd > nfds) {
            	nfds = m_outer_main_sockfd;
            }
        } else {
        	/*Connected*/
            FD_SET(m_outer_server_sockfd, &rfds);
            if (m_outer_server_sockfd > nfds) {
            	nfds = m_outer_server_sockfd;
            }
        }

        int result = select(nfds + 1, &rfds, NULL, NULL, NULL);

        if (FD_ISSET(m_inner_server_sockfd, &rfds)) {
        	/*Wait event*/
        	PtracerWait info;
        	sizeresult = recv(m_inner_server_sockfd, &info, sizeof(info), 0);
        	if (sizeresult != sizeof(info)) {
        		PTRACER_LOG("Recv failed, result %d, errno %d.", sizeresult, errno);
        		return -1;
        	}
        	(void)waitProc(info);
        }

        if (m_child_pipe_stdout != -1) {
        	if (FD_ISSET(m_child_pipe_stdout, &rfds)) {
        		sizeresult = read(m_child_pipe_stdout, stdout_msg_buf, std_buf_size);
    			if (sizeresult < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
    				continue;			/*Receive again*/
    			} else if (sizeresult < 0) {
    				close(m_child_pipe_stdout);
    				m_child_pipe_stdout = -1;
    			} else {
    				string event_head = "event://stdout?";
    				string content(stdout_msg_buf, sizeresult);
    				string encode = msgEncode(content);
    				string event = event_head + encode;
    				sendEvent(event);
    			}
        	}
        }

        if (m_outer_server_sockfd == -1) {
        	/*Waitting connect*/
        	if (FD_ISSET(m_outer_main_sockfd, &rfds)) {
        		struct sockaddr_in client_addr;
        		len = sizeof(client_addr);
        		m_outer_server_sockfd = accept(m_outer_main_sockfd, (struct sockaddr *)&client_addr, &len);
        	    if (m_outer_server_sockfd < 0) {
        	    	PTRACER_LOG("Accept failed, result %d, errno %d.", m_inner_server_sockfd, errno);
        	    }
				out_msg_buf_off = 0;
        	}
        } else {
    		if(0) {
con_failed:
				/*Connection failed*/
				PTRACER_LOG("Connection closed.");
				close(m_outer_server_sockfd);
				m_outer_server_sockfd = -1;
				connClosed();
    		}

        	/*Connected*/
        	if (FD_ISSET(m_outer_server_sockfd, &rfds)) {
        		if (out_msg_buf_off < sizeof(struct ptr_msg)) {
            		sizeresult = recv(m_outer_server_sockfd, out_msg_buf + out_msg_buf_off,
            				          sizeof(struct ptr_msg) - out_msg_buf_off, MSG_DONTWAIT);
            		/*TODO:return value*/
        			if (sizeresult <= 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
        				continue;			/*Receive again*/
        			} else if (sizeresult <= 0) {
        				goto con_failed;
        			} else {
        				out_msg_buf_off += sizeresult;
            			if (out_msg_buf_off < sizeof(struct ptr_msg)) {
            				continue;
            			}
        			}
        		}
        		struct ptr_msg *msg_head = (struct ptr_msg *)out_msg_buf;
        		if (out_msg_buf_off < msg_head->len) {
            		sizeresult = recv(m_outer_server_sockfd, out_msg_buf + out_msg_buf_off,
            				          msg_head->len - out_msg_buf_off, MSG_DONTWAIT);
        			if (sizeresult < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
        				continue;			/*Receive again*/
        			} else if (sizeresult < 0) {
        				goto con_failed;
        			} else {
        				out_msg_buf_off += sizeresult;
            			if (out_msg_buf_off < msg_head->len) {
            				continue;
            			}
        			}
        		}

                if (msg_head->magic != INTER_MSG_MAGIC) {
                	PTRACER_LOG("Message magic error, close connection.");
                	goto con_failed;
                }

                out_msg_buf_off = 0;

               	PTRACER_DEBUG("Received message, code %d, length %d.", msg_head->code, msg_head->len);
               	/*do request*/
               	string request((char *)(msg_head + 1), (msg_head->len - sizeof(struct ptr_msg)));
               	string response = doRequest(request);
               	/*return response*/
               	int length = sizeof(struct ptr_msg) + response.length();
               	struct ptr_msg *msg = (struct ptr_msg *)malloc(length);
               	msg->magic = INTER_MSG_MAGIC;
               	msg->code = PTCR_RESP_DATA;
               	msg->len = length;
               	memcpy((char *)(msg + 1), response.c_str(), response.length());
               	int sizeresult = send(m_outer_server_sockfd, msg, length, 0);
               	free(msg);
               	if (sizeresult != length) {
               		PTRACER_LOG("Send failed, errno %d.", errno);
               		goto con_failed;
               	}

        		pthread_mutex_lock(&m_wait_lock);
        		m_wait_pid = m_pid;
        		pthread_cond_signal(&m_wait_cond);
        		pthread_mutex_unlock(&m_wait_lock);
        	}
        }
    }

	return 0;
}

int Ptracer::postShutDown()
{
	if (m_child_pipe_stdin != -1) {
		close(m_child_pipe_stdin);
		m_child_pipe_stdin = -1;
	}
	if (m_child_pipe_stdout != -1) {
		close(m_child_pipe_stdout);
		m_child_pipe_stdout = -1;
	}

	return 0;
}

/*Child thread exited already*/
int Ptracer::waitExited(PtracerWait &waitinfo, int exit_code)
{
	PTRACER_DEBUG("Wait a exit event for thread %d with exit code %d.", waitinfo.pid, exit_code);
	PtracerThread *pthread = threadGetById(waitinfo.pid);
	if (pthread == NULL) {
		PTRACER_LOG("No thread information for thread %d.", waitinfo.pid);
		return 0;
	}

	switch (pthread->m_status)
	{
	case PtracerThread::STATUS_PREEND:
	{
		/*Normal exit, status from STATUS_PREEND*/
		PTRACER_DEBUG("Exit with code %d for thread %d.", exit_code, waitinfo.pid);
		string event_head = "event://threadexited?";
		string event = event_head + string_format("pid=%d&code=%d", waitinfo.pid, exit_code);
		sendEvent(event);

		/*Clear this thread*/
		threadDelById(pthread->m_tid);
		if (waitinfo.pid == m_pid) {
			m_pid = 0;
			postShutDown();
		}
		break;
	}
	case PtracerThread::STATUS_NEW:
	{
		/*If exec child failed, the forked child exit with error code*/
		PTRACER_DEBUG("Start fail with code %d for thread %d.", exit_code, waitinfo.pid);
		string event_head = "event://startfail?";
		string event = event_head + string_format("pid=%d&code=%d", waitinfo.pid, exit_code);
		sendEvent(event);

		threadDelById(pthread->m_tid);
		PTRACER_ASSERT(m_thread_set.empty());
		m_pid = 0;
		postShutDown();
		break;
	}
	default:
	{
		PTRACER_LOG("Wrong status %s for thread %d, when exit event.",
				     PtracerThread::status_desc[pthread->m_status], waitinfo.pid);
		break;
	}
	}

	return 0;
}

/*Same as waitExited, but reason is killed*/
int Ptracer::waitKilled(PtracerWait &waitinfo, int signal_num)
{
	PTRACER_DEBUG("Wait a kill event for thread %d with signal %d.", waitinfo.pid, signal_num);
	PtracerThread *pthread = threadGetById(waitinfo.pid);
	if (pthread == NULL) {
		PTRACER_LOG("No thread information for thread %d.", waitinfo.pid);
		return 0;
	}

	switch (pthread->m_status)
	{
	case PtracerThread::STATUS_ENDING:
	{
		/*Normal killed, status from STATUS_PREEND->STATUS_ENDING*/
		PTRACER_DEBUG("Killed with signal %d for thread %d.", signal_num, waitinfo.pid);
		string event_head = "event://threadkilled?";
		string event = event_head + string_format("pid=%d&code=%d", waitinfo.pid, signal_num);
		sendEvent(event);

		/*Clear this thread*/
		threadDelById(pthread->m_tid);
		if (waitinfo.pid == m_pid) {
			m_pid = 0;
			postShutDown();
		}
		break;
	}
	case PtracerThread::STATUS_NEW:
	{
		/*Killed a starting child*/
		PTRACER_DEBUG("Start fail with signal %d for thread %d.", signal_num, waitinfo.pid);
		string event_head = "event://startfail?";
		string event = event_head + string_format("pid=%d&code=%d", waitinfo.pid, waitinfo.num);
		sendEvent(event);

		threadDelById(pthread->m_tid);
		PTRACER_ASSERT(m_thread_set.empty());
		m_pid = 0;
		postShutDown();
		break;
	}
	default:
	{
		PTRACER_LOG("Wrong status %s for thread %d, when kill event.",
				     PtracerThread::status_desc[pthread->m_status], waitinfo.pid);
		break;
	}
	}

	return 0;
}

/*A whole clone (new thread start) include two parts: clone thread clone event, new thread stop event*/
int Ptracer::waitStopWithClone(PtracerWait &waitinfo, int child_pid)
{
	PTRACER_DEBUG("Wait a clone event for parent thread %d, child thread is %d.", waitinfo.pid, child_pid);
	PtracerThread *pthread = threadGetById(waitinfo.pid);
	if (pthread == NULL) {
		PTRACER_LOG("No thread information for thread %d.", waitinfo.pid);
		return 0;
	}

	pthread = threadGetById(child_pid);
	if (pthread == NULL) {
		/*Clone event coming first*/
		PtracerThread *new_thread = new PtracerThread();
		new_thread->m_tid = child_pid;
		new_thread->statusSet(PtracerThread::STATUS_CLONE);
	    PTRACER_DEBUG("Create thread info for child thread %d with status %s from parent thread %d",
	    		    child_pid, PtracerThread::status_desc[new_thread->m_status], waitinfo.pid);
	    threadInsert(new_thread);
	} else {
		if (pthread->m_status == PtracerThread::STATUS_WAIT) {
			/*Clone event coming after new thread stop event*/
			pthread->statusSet(PtracerThread::STATUS_STOP);
			long longret = ptrace(PTRACE_SETOPTIONS, child_pid, 0, PTRACER_OPTION);
			if (longret == -1) {
				PTRACER_LOG("PTRACE_SETOPTIONS failed for thread %d, errno %d.", child_pid, errno);
			}
			string event_head = "event://threadstart?";
			string event = event_head + string_format("pid=%d", child_pid);
			sendEvent(event);
		} else {
			PTRACER_LOG("Wrong thread status %s for thread %d when clone event.",
					     PtracerThread::status_desc[pthread->m_status], child_pid);
		}
	}
	/*Parent thread continued after clone event*/
	long longret = ptrace(PTRACE_CONT, waitinfo.pid, 0, /*sig*/0);
	if (longret == -1) {
		PTRACER_LOG("PTRACE_CONT failed for thread %d, errno %d.", waitinfo.pid, errno);
		return 0;
	}
	return 0;
}

/*A whole clone (fork new process start) include two parts: clone thread clone event, new thread stop event*/
int Ptracer::waitStopWithFork(PtracerWait &waitinfo, int child_pid)
{
	PTRACER_DEBUG("Wait a fork event for parent thread %d, child thread is %d.", waitinfo.pid, child_pid);
	PtracerThread *pthread = threadGetById(waitinfo.pid);
	if (pthread == NULL) {
		PTRACER_LOG("No thread information for thread %d.", waitinfo.pid);
		return 0;
	}

	pthread = threadGetById(child_pid);
	if (pthread == NULL) {
		/*Fork event coming first*/
		PtracerThread *new_thread = new PtracerThread();
		new_thread->m_tid = child_pid;
		new_thread->statusSet(PtracerThread::STATUS_FORK);
	    PTRACER_DEBUG("Create thread info for child thread %d with status %s from parent thread %d",
	    		    child_pid, PtracerThread::status_desc[new_thread->m_status], waitinfo.pid);
	    threadInsert(new_thread);
	} else {
		if (pthread->m_status == PtracerThread::STATUS_WAIT) {
			/*Fork event coming after new thread stop event, release the forked process*/
			PTRACER_LOG("Detach thread %d when fork event.", child_pid);
			long longret = ptrace(PTRACE_DETACH, child_pid, 0, 0);
			if (longret == -1) {
				PTRACER_LOG("PTRACE_DETACH failed for thead %d, errno %d.", child_pid, errno);
			}
			threadDelById(pthread->m_tid);
			if (child_pid == m_pid) {
				m_pid = 0;
				postShutDown();
			}
		} else {
			PTRACER_LOG("Wrong thread status %s for thread %d when fork event.",
					    PtracerThread::status_desc[pthread->m_status], child_pid);
		}
	}
	/*Parent thread continued after fork event*/
	long longret = ptrace(PTRACE_CONT, waitinfo.pid, 0, /*sig*/0);
	if (longret == -1) {
		PTRACER_LOG("PTRACE_CONT failed for thread %d, errno %d.", waitinfo.pid, errno);
		return 0;
	}
	return 0;
}

int Ptracer::waitStopWithExec(PtracerWait &waitinfo, /*no use*/int former_pid)
{
	PTRACER_TODO("Exec no support now.");
	return 0;
}

int Ptracer::waitStopWithVfork(PtracerWait &waitinfo, int pid)
{
	PTRACER_TODO("Vfork no support now.");
	return 0;
}

int Ptracer::waitStopWithVforkDone(PtracerWait &waitinfo, int pid)
{
	PTRACER_TODO("Vforkdone no support now.");
	return 0;
}

/*Stop event before exit*/
int Ptracer::waitStopWithExit(PtracerWait &waitinfo, int status)
{
	PTRACER_DEBUG("Wait a stop event with exit for thread %d.", waitinfo.pid);
	PtracerThread *pthread = threadGetById(waitinfo.pid);
	if (pthread == NULL) {
		PTRACER_LOG("No thread information for thread %d.", waitinfo.pid);
		return 0;
	}

	switch(pthread->m_status)
	{
	case PtracerThread::STATUS_STOP:
	case PtracerThread::STATUS_RUN:
	{
		PTRACER_DEBUG("Thread %d will exit with status 0x%08x.", waitinfo.pid, status);
		pthread->statusSet(PtracerThread::STATUS_PREEND);
		string event_head = "event://threadend?";
		string event = event_head + string_format("pid=%d&code=%d", waitinfo.pid, status);
		sendEvent(event);
		break;
	}
	default:
	{
		PTRACER_LOG("Wrong thread status %s for thread %d when a stop event with exit.",
				     PtracerThread::status_desc[pthread->m_status], waitinfo.pid);
		break;
	}
	}
	return 0;
}

/*No event flag and no signal information(errno==22), happened after attach */
int Ptracer::waitStopButNoSiginfo(PtracerWait &waitinfo)
{
	PTRACER_DEBUG("Wait a stop event by no signal information for thread %d.", waitinfo.pid);
	PtracerThread *pthread = threadGetById(waitinfo.pid);
	if (pthread == NULL) {
		PTRACER_LOG("No thread information for thread %d.", waitinfo.pid);
		return 0;
	}

	switch(pthread->m_status)
	{
	case PtracerThread::STATUS_ATTACH:
	{
		/*STATUS_ATTACH to STATUS_STOP*/
		pthread->statusSet(PtracerThread::STATUS_STOP);

		long longret = ptrace(PTRACE_SETOPTIONS, waitinfo.pid, 0, PTRACER_OPTION);
		if (longret == -1) {
			PTRACER_LOG("PTRACE_SETOPTIONS failed for thead %d, errno %d.", waitinfo.pid, errno);
			return 0;
		}

		string event_head = "event://threadstart?";
		string event = event_head + string_format("pid=%d", waitinfo.pid);
		sendEvent(event);

		/*when all thread turn to STATUS_STOP from STATUS_ATTACH, send start ok event*/
		map<unsigned int, PtracerThread*>::iterator iter;
		for (iter = m_thread_set.begin(); iter != m_thread_set.end(); iter++) {
			PtracerThread *thread = iter->second;
			if (thread->m_status != PtracerThread::STATUS_STOP) {
				break;
			}
		}
		if (iter == m_thread_set.end()) {
			string event_head = "event://startok?";
			string event = event_head + string_format("pid=%d", m_pid);
			sendEvent(event);
		}
		break;
	}
	default:
	{
		PTRACER_LOG("Wrong thread status %s for thread %d when stop event by no signal information.",
				     PtracerThread::status_desc[pthread->m_status], waitinfo.pid);
		break;
	}
	}

	return 0;
}

/*A stop signal*/
int Ptracer::waitStopBySignalStop(PtracerWait &waitinfo, siginfo_t &siginfo)
{
	PTRACER_DEBUG("Wait a stop event by stop signal for thread %d.", waitinfo.pid);

	PtracerThread *pthread = threadGetById(waitinfo.pid);
	if (pthread == NULL) {
		/*Child stop event coming before the fork or clone*/
		PTRACER_DEBUG("No thread information for thread %d, create thread information.", waitinfo.pid);
		PtracerThread *new_thread = new PtracerThread();
		new_thread->m_tid = waitinfo.pid;
		new_thread->statusSet(PtracerThread::STATUS_WAIT);		/*wait fork or clone*/
	    PTRACER_DEBUG("Create thread info for child thread %d with status %s from parent thread %d",
	    				waitinfo.pid, PtracerThread::status_desc[new_thread->m_status], waitinfo.pid);
	    threadInsert(new_thread);
		return 0;
	}

	switch(pthread->m_status)
	{
	case PtracerThread::STATUS_CLONE:
	{
		/*Child stop event coming after clone*/
		pthread->statusSet(PtracerThread::STATUS_STOP);
		/*New thread start*/
		long longret = ptrace(PTRACE_SETOPTIONS, waitinfo.pid, 0, PTRACER_OPTION);
		if (longret == -1) {
			PTRACER_LOG("PTRACE_SETOPTIONS failed for thead %d, errno %d.", waitinfo.pid, errno);	//报告错误，保守处理
			return 0;
		}
		string event_head = "event://threadstart?";
		string event = event_head + string_format("pid=%d", waitinfo.pid);
		sendEvent(event);
		break;
	}
	case PtracerThread::STATUS_FORK:
	{
		/*Child stop event coming after fork*/
		PTRACER_LOG("Detach thread %d when fork event.", waitinfo.pid);
		long longret = ptrace(PTRACE_DETACH, waitinfo.pid, 0, 0);
		if (longret == -1) {
			PTRACER_LOG("PTRACE_DETACH failed for thead %d, errno %d.", waitinfo.pid, errno);
			return 0;
		}

		threadDelById(pthread->m_tid);
		if (waitinfo.pid == m_pid) {
			m_pid = 0;
			postShutDown();
		}
		break;
	}
	case PtracerThread::STATUS_RUN:
	{
		if (pthread->m_flag_take_stop) {
			/*Stop by supervisor*/
			PTRACER_DEBUG("Thread %d stoped.", waitinfo.pid);
			pthread->statusSet(PtracerThread::STATUS_STOP);

			string event_head = "event://stop?";
			string event = event_head + string_format("pid=%d", waitinfo.pid);
			sendEvent(event);
		} else {
			/*Others stop signal, for example: user command, Ignore the signal*/
			PTRACER_DEBUG("Ignore stop signed for thread %d, continue the thread.", waitinfo.pid);
			long longret = ptrace(PTRACE_CONT, waitinfo.pid, 0, /*sig*/0);
			if (longret == -1) {
				PTRACER_LOG("PTRACE_CONT failed for thead %d, errno %d.", waitinfo.pid, errno);
				return 0;
			}
		}
		break;
	}
	default:
	{
		PTRACER_LOG("Wrong thread status %s for thread %d when stop event by stop signal.",
				     PtracerThread::status_desc[pthread->m_status], waitinfo.pid);
		break;
	}
	}

	return 0;
}

/*New process start or user command sigtrap*/
int Ptracer::waitStopBySignalTrapUser(PtracerWait &waitinfo, siginfo_t &siginfo)
{
	PTRACER_DEBUG("Wait a stop event by user trap for thread %d.", waitinfo.pid);
	PtracerThread *pthread = threadGetById(waitinfo.pid);
	if (pthread == NULL) {
		PTRACER_LOG("No thread information for thread %d.", waitinfo.pid);
		return 0;
	}

	switch (pthread->m_status)
	{
	case PtracerThread::STATUS_NEW:
	{
		/*New process start ok*/
		PTRACER_DEBUG("Thread %d start.", waitinfo.pid);
		pthread->statusSet(PtracerThread::STATUS_STOP);

		long longret = ptrace(PTRACE_SETOPTIONS, waitinfo.pid, 0, PTRACER_OPTION);
		if (longret == -1) {
			PTRACER_LOG("PTRACE_SETOPTIONS failed for thead %d, errno %d.", waitinfo.pid, errno);
			return 0;
		}

		string threadstart_head = "event://threadstart?";
		string threadstart = threadstart_head + string_format("pid=%d", waitinfo.pid);
		sendEvent(threadstart);

		/*Only on thread*/
		string startok_head = "event://startok?";
		string startok = startok_head + string_format("pid=%d", waitinfo.pid);
		sendEvent(startok);
		break;
	}
	case PtracerThread::STATUS_RUN:
	{
		/*Ignore others sigtrap*/
		PTRACER_DEBUG("Ignore user trap signed for thread %d, continue the thread.", waitinfo.pid);
		long longret = ptrace(PTRACE_CONT, waitinfo.pid, 0, /*sig*/0);
		if (longret == -1) {
			PTRACER_LOG("PTRACE_CONT failed for thread %d, errno %d.", waitinfo.pid, errno);
			return 0;
		}

		break;
	}
	default:
	{
		PTRACER_LOG("Wrong thread status %s for thread %d when stop event by user trap.",
				     PtracerThread::status_desc[pthread->m_status], waitinfo.pid);
		break;
	}
	}

	return 0;
}

/*Break point event*/
int Ptracer::waitStopBySignalTrapBreak(PtracerWait &waitinfo, siginfo_t &siginfo)
{
	PTRACER_DEBUG("Wait a stop event by break trap for thread %d.", waitinfo.pid);
	PtracerThread *pthread = threadGetById(waitinfo.pid);
	if (pthread == NULL) {
		PTRACER_LOG("No thread information for thread %d.", waitinfo.pid);
		return 0;
	}

	switch (pthread->m_status)
	{
	case PtracerThread::STATUS_RUN:
	{
		PTRACER_DEBUG("Thread %d breaked.", waitinfo.pid);
		pthread->statusSet(PtracerThread::STATUS_STOP);

		/*Get breakpint stop at*/
		unsigned long long instr_now;
		int result = m_arch->breakAdjust(waitinfo.pid, &instr_now);
		if (result < 0) {
			PTRACER_LOG("Adjust current instruction for thread %d failed.", waitinfo.pid);
			return 0;
		}

		PtracerBreak *ptrbrk = breakGetByAddr(instr_now);
		if (ptrbrk == NULL) {
			PTRACER_LOG("No breakpoint on 0x%016llx for thread %d break.", instr_now, waitinfo.pid);
		}

		string event_head = "event://break?";
		string event = event_head + string_format("pid=%d&addr=%016llx", waitinfo.pid, instr_now);
		sendEvent(event);
		break;
	}
	default:
	{
		PTRACER_LOG("Wrong thread status %s for thread %d when break trap signal event.",
				     PtracerThread::status_desc[pthread->m_status], waitinfo.pid);
		break;
	}
	}

	return 0;
}

/*Event after step*/
int Ptracer::waitStopBySignalTrapStep(PtracerWait &waitinfo, siginfo_t &siginfo)
{
	PTRACER_DEBUG("Wait a stop event by step trap for thread %d.", waitinfo.pid);
	PtracerThread *pthread = threadGetById(waitinfo.pid);
	if (pthread == NULL) {
		PTRACER_LOG("No thread information for thread %d.", waitinfo.pid);
		return 0;
	}

	switch (pthread->m_status)
	{
	case PtracerThread::STATUS_RUN:
	{
		PTRACER_DEBUG("Thread %d traped.", waitinfo.pid);
		pthread->statusSet(PtracerThread::STATUS_STOP);

		string event_head = "event://step?";
		string event = event_head + string_format("pid=%d&signum=0", waitinfo.pid);
		sendEvent(event);
		break;
	}
	default:
	{
		PTRACER_LOG("Wrong thread status %s for thread %d when stop event by step trap.",
				     PtracerThread::status_desc[pthread->m_status], waitinfo.pid);
		break;
	}
	}

	return 0;
}

/*Event after syscall event*/
int Ptracer::waitStopBySignalTrapSyscall(PtracerWait &waitinfo, siginfo_t &siginfo)
{
	PTRACER_DEBUG("Wait a stop event by syscall trap for thread %d.", waitinfo.pid);
	PtracerThread *pthread = threadGetById(waitinfo.pid);
	if (pthread == NULL) {
		PTRACER_LOG("No thread information for thread %d.", waitinfo.pid);
		return 0;
	}

	switch (pthread->m_status)
	{
	case PtracerThread::STATUS_RUN:
	{
		PTRACER_DEBUG("Thread %d syscalled.", waitinfo.pid);
		pthread->statusSet(PtracerThread::STATUS_STOP);

		string event_head = "event://syscall?";
		string event = event_head + string_format("pid=%d", waitinfo.pid);
		sendEvent(event);
		break;
	}
	default:
	{
		PTRACER_LOG("Wrong thread status %s for thread %d when stop event by syscall trap.",
				PtracerThread::status_desc[pthread->m_status], waitinfo.pid);
		break;
	}
	}

	return 0;
}

/*Othres sigtrap*/
int Ptracer::waitStopBySignalTrapNormal(PtracerWait &waitinfo, siginfo_t &siginfo)
{
	PTRACER_DEBUG("Wait a stop event by normal trap for thread %d.", waitinfo.pid);
	PtracerThread *pthread = threadGetById(waitinfo.pid);
	if (pthread == NULL) {
		PTRACER_LOG("No thread information for thread %d.", waitinfo.pid);
		return 0;
	}
	PTRACER_LOG("No support normal trap.");
	return 0;
}

/*User signal*/
int Ptracer::waitStopBySignalNormal(PtracerWait &waitinfo, siginfo_t &siginfo)
{
	PTRACER_DEBUG("Wait a stop event by normal signal %d for thread %d.", waitinfo.pid, siginfo.si_signo);
	PtracerThread *pthread = threadGetById(waitinfo.pid);
	if (pthread == NULL) {
		PTRACER_LOG("No thread information for thread %d.", waitinfo.pid);
		return 0;
	}

	switch (pthread->m_status)
	{
	case PtracerThread::STATUS_RUN:
	{
		PTRACER_DEBUG("Thread %d signed with signal %d.", waitinfo.pid, siginfo.si_signo);
		pthread->statusSet(PtracerThread::STATUS_STOP);

		string event_head = "event://signal?";
		string event = event_head + string_format("pid=%d&signum=%d", waitinfo.pid, siginfo.si_signo);
		sendEvent(event);
		break;
	}
	default:
	{
		PTRACER_LOG("Wrong thread status %s for thread %d when stop event by normal signal.",
				     PtracerThread::status_desc[pthread->m_status], waitinfo.pid);
		break;
	}
	}

	return 0;
}

/*Difference reason of stop by signal, distribute here*/
int Ptracer::waitStopBySignal(PtracerWait &waitinfo, siginfo_t &siginfo)
{
	PTRACER_DEBUG("Wait a stop event by a signal %d for thread %d.", waitinfo.pid, siginfo.si_signo);
	switch(siginfo.si_signo)
	{
	case SIGSTOP:
	{
		/*A stop signal*/
		PTRACER_DEBUG("Receive a user_send SIGSTOP signal from thread %d.", waitinfo.pid);
		waitStopBySignalStop(waitinfo, siginfo);
		break;
	}
	case SIGTRAP:
	{
		switch(siginfo.si_code)
		{
		case SI_USER:
		{
			/*User send trap signal*/
			PTRACER_DEBUG("Receive a user_send SIGTRAP signal from thread %d.", waitinfo.pid);
			waitStopBySignalTrapUser(waitinfo, siginfo);
			break;
		}
		case SI_KERNEL:
		{
			/*A breakpoint signal*/
			PTRACER_DEBUG("Receive a breakpoint SIGTRAP signal from thread %d.", waitinfo.pid);
			waitStopBySignalTrapBreak(waitinfo, siginfo);
			break;
		}
		case TRAP_TRACE:
		{
			/*A step signal*/
			PTRACER_DEBUG("Receive a step SIGTRAP signal from thread %d.", waitinfo.pid);
			waitStopBySignalTrapStep(waitinfo, siginfo);
			break;
		}
		case TRAP_BRKPT:
		{
			/*A syscall signal*/
			PTRACER_DEBUG("Receive a syscall SIGTRAP signal from thread %d.", waitinfo.pid);
			waitStopBySignalTrapSyscall(waitinfo, siginfo);
			break;
		}
		default:
		{
			PTRACER_LOG("Recevie SIGTRAP signal with unknown code %d from thread %d.", siginfo.si_code, waitinfo.pid);
			waitStopBySignalTrapNormal(waitinfo, siginfo);
			break;
		}
		}

		break;
	}
	default:
	{
		/*A normal signal*/
		PTRACER_DEBUG("Receive a user_send %d signal from pid %d.", siginfo.si_signo, waitinfo.pid);
		waitStopBySignalNormal(waitinfo, siginfo);
		break;
	}
	}

	return 0;
}

/*Do wait event*/
int Ptracer::waitProc(PtracerWait &waitinfo)
{
	PTRACER_ASSERT(waitinfo.event >= 0 && waitinfo.event < PtracerWait::EVENT_MAX);
	PTRACER_DEBUG("Receive wait event %s from thread %d, with code %d, flags:(%s %s %s %s %s %s ).",
				    PtracerWait::event_desc[waitinfo.event], waitinfo.pid, waitinfo.num,
					waitinfo.event_clone?"clone":"", waitinfo.event_exec?"exec":"",
					waitinfo.event_exit?"exit":"", waitinfo.event_fork?"fork":"",
					waitinfo.event_vfork?"vfork":"", waitinfo.event_vforkdone?"vforkdone":"");

	switch(waitinfo.event)
	{
	case PtracerWait::EVENT_EXITD:
	{
		/*Child thread exited*/
		return waitExited(waitinfo, waitinfo.num);
	}
	case PtracerWait::EVENT_KILLED:
	case PtracerWait::EVENT_DUMPED:
	{
		/*Child thread killed or killed and core dump*/
		return waitKilled(waitinfo, waitinfo.num);
	}
	case PtracerWait::EVENT_STOPED:
	{
		/*Child thread stoped, eg:attach, breakpoint, signal, ....*/

		/*Some flag about child stop*/
		if (waitinfo.event_clone) {
			/*Which child cloned, get child pid from PTRACE_GETEVENTMSG*/
		    unsigned long long_var_pid;
		    long longret = ptrace(PTRACE_GETEVENTMSG, waitinfo.pid, 0, &long_var_pid);
		    if (longret == -1) {
		    	PTRACER_LOG("PTRACE_GETEVENTMSG failed for thread %d when clone. errno %d\n", waitinfo.pid, errno);
		    	return 0;
		    }
		    int pid = long_var_pid;
			return waitStopWithClone(waitinfo, pid);
		}

		if (waitinfo.event_exec) {
			/*No support now*/
			return waitStopWithExec(waitinfo, 0);
		}

		if (waitinfo.event_exit) {
			/*Child thread exited, get exit status code form PTRACE_GETEVENTMSG*/
		    unsigned long long_var_stat;
		    long longret = ptrace(PTRACE_GETEVENTMSG, waitinfo.pid, 0, &long_var_stat);
		    if (longret == -1) {
		    	PTRACER_LOG("PTRACE_GETEVENTMSG failed for thread %d when exit. errno %d.", waitinfo.pid, errno);
		    	return 0;
		    }
		    int status = long_var_stat;
			return waitStopWithExit(waitinfo, status);
		}

		if (waitinfo.event_fork) {
			/*Which child forked, get child pid from PTRACE_GETEVENTMSG*/
		    unsigned long long_var_pid;
		    long longret = ptrace(PTRACE_GETEVENTMSG, waitinfo.pid, 0, &long_var_pid);
		    if (longret == -1) {
		    	PTRACER_LOG("PTRACE_GETEVENTMSG failed for thread %d when fork. errno %d.", waitinfo.pid, errno);
		    	return 0;
		    }
		    int pid = long_var_pid;
			return waitStopWithFork(waitinfo, pid);
		}

		if (waitinfo.event_vfork) {
		    unsigned long long_var_pid;
		    long longret = ptrace(PTRACE_GETEVENTMSG, waitinfo.pid, 0, &long_var_pid);
		    if (longret == -1) {
		    	PTRACER_LOG("PTRACE_GETEVENTMSG failed for thread %d when vfork. errno %d.", waitinfo.pid, errno);
		    	return 0;
		    }
		    int pid = long_var_pid;
			return waitStopWithVfork(waitinfo, pid);
		}

		if (waitinfo.event_vforkdone) {
		    unsigned long long_var_pid;
		    long longret = ptrace(PTRACE_GETEVENTMSG, waitinfo.pid, 0, &long_var_pid);
		    if (longret == -1) {
		    	PTRACER_LOG("PTRACE_GETEVENTMSG failed for thread %d when vforkdown. errno %d.", waitinfo.pid, errno);
		    	return 0;
		    }
		    int pid = long_var_pid;
			return waitStopWithVforkDone(waitinfo, pid);
		}

		/*When no any flags*/
		siginfo_t siginfo = {0};
		long longret = ptrace(PTRACE_GETSIGINFO, waitinfo.pid, 0, &siginfo);
		if (longret == -1) {
			if (errno == EINVAL) {		/*#define	EINVAL		22	// Invalid argument */
				/*Stop with on signal information*/
				return waitStopButNoSiginfo(waitinfo);
		    } else {
		    	/*Some mistake*/
		    	PTRACER_LOG("PTRACE_GETSIGINFO failed for thread %d without any flags. errno %d\n", waitinfo.pid, errno);
		    	return 0;
		    }
		}

		/*Stop with signal information*/
		return waitStopBySignal(waitinfo, siginfo);
	}
	case PtracerWait::EVENT_CONTINUED:
	{
		PTRACER_LOG("Wrong wait event with EVENT_CONTINUED.");
		break;
	}
	default:
	{
		PTRACER_ASSERT(0);
	}
	}

	return 0;
}

#if 0

#define ptracer_attach_num  (0x00000001)
#define ptracer_attach_file (0x00000002)
#define ptracer_attach_expr (0x00000003)
struct ptr_req_attach {
	struct ptr_msg head;
	unsigned int num_of_tlv;
	char value[0];
};
struct ptr_resp_attach {
	struct ptr_msg head;
	unsigned int err_no;
	unsigned int pid;
};

/*
 * 对已经启动的进程attach
 * */
int ptracer::request_attach(struct ptr_req_attach *request)
{
	PTRACER_ASSERT(request != NULL);

	PTRACER_DEBUG("Receive request attach.");

	unsigned int return_pid;
	unsigned int err_no;

	int result;
	ptracer_tlv_msg *tlv;

	linux_process *process_info;
	list<linux_thread*>::iterator linux_thread_iter;

	ptracer_thread_map::iterator it;
	struct ptr_resp_attach response = {0};

	unsigned int request_pid = 0;

	//参数分析与检查
	if (request->num_of_tlv != 1) {
		PTRACER_LOG("Error message format, wrong number of tlv.");
		err_no = PTCR_ERRNO_INVALID_ARG;
		return_pid = 0;
		goto finish;
	}

	tlv = (ptracer_tlv_msg *)request->value;	//只有一个tlv
	switch(tlv->type)							//attach 的几种形式
	{
	case ptracer_attach_num:
	{
		request_pid = *((unsigned int *)(tlv->value));
		break;
	}
	case ptracer_attach_file:
	{
		if (tlv->len <= sizeof(struct ptracer_tlv_msg)
			|| tlv->len % INTER_MSG_ALIGN != 0) {
			PTRACER_LOG("Error message format, error length for path.");
			break;
		}

		int path_len = tlv->len - sizeof(struct ptracer_tlv_msg);
		if (tlv->value[path_len - 1] != '\0') {
			PTRACER_LOG("Error message format, no end for path.");
			break;
		}

		request_pid = linux_pid_from_file(tlv->value);
		break;
	}
	case ptracer_attach_expr:
	{
		if (tlv->len <= sizeof(struct ptracer_tlv_msg)
			|| tlv->len % INTER_MSG_ALIGN != 0) {
			PTRACER_LOG("Error message format, error length for path.");
			break;
		}

		int path_len = tlv->len - sizeof(struct ptracer_tlv_msg);
		if (tlv->value[path_len - 1] != '\0') {
			PTRACER_LOG("Error message format, no end for path.");
			break;
		}

		request_pid = linux_pid_from_expr(tlv->value);
		break;
	}
	default:
	{
		PTRACER_LOG("Wrong attach command, code %d.", tlv->type);
		break;
	}
	}

	if (request_pid == 0) {						//检查参数
		PTRACER_LOG("Wrong thread id for attach request.");
		err_no = PTCR_ERRNO_INVALID_ARG;
		return_pid = 0;
		goto finish;
	}

	//环境检查
	if (! status_isshut()) {
		PTRACER_DEBUG("Reject attach request, because running now.");
		return_pid = 0;
		err_no = PTCR_ERRNO_RUNNING;
		goto finish;
	}

	//执行
	/*
	 * 首先向目标进程发送 SIGSTOP，使目标进程暂停，否则运动中的多线程很难控制
	 * 这里发送的信号在attach成功以后被压住，一旦continue还会回来的，不过不用管他，当作无故接收到的stop信号处理。
	 * */
	result = kill(request_pid, SIGSTOP);
	if (result == -1) {
		PTRACER_DEBUG("Kill failed for thread %d, errno %d.", request_pid, errno);
		switch(errno) {
		case EPERM:{err_no = PTCR_ERRNO_NOPERMISON;break;}
		case ESRCH:{err_no = PTCR_ERRNO_NOSUCHPID;break;}
		default:   {err_no = PTCR_ERRNO_UNKONWN;break;}
		}

		return_pid = 0;
		goto finish;
	}

	//为了保险，这里等待一秒，还没想到其他的简单办法
	sleep(1);

	//获取进程以及线程信息，并遍历线程
	process_info = linux_get_process_info(request_pid);
	if (process_info == NULL) {
		PTRACER_LOG("Get process %d information failed.", request_pid);
		err_no = PTCR_ERRNO_UNKONWN;
		return_pid = 0;
		goto finish;
	}

	//对所有线程执行attach
	m_pid = request_pid;
	for(linux_thread_iter = process_info->m_thread_list.begin();
		linux_thread_iter != process_info->m_thread_list.end(); linux_thread_iter++) {
		linux_thread *linux_thread = *linux_thread_iter;

		//对每一个线程执行attach
	    long longret = ptrace(PTRACE_ATTACH, linux_thread->m_tid, 0, 0);
	    if (longret == -1) {
	    	PTRACER_LOG("PTRACE_ATTACH failed for thread %d, errno %d.", linux_thread->m_tid, errno);
	    	break;
	    }

	    ptracer_thread *new_thread = new ptracer_thread();
	    new_thread->m_tid = linux_thread->m_tid;
	    new_thread->init_status(PTRACER_THREAD_ATTACH);
	    thread_insert(new_thread);
	}

	if (linux_thread_iter != process_info->m_thread_list.end()) {	//没有完成全部attach循环，回滚
		ptracer_thread_map::iterator iter;
		for(iter = m_thread_set.begin(); iter != m_thread_set.end(); /*none*/) {
			unsigned int pid = iter->first;
			ptracer_thread *thread = iter->second;
			PTRACER_ASSERT(thread != NULL);
			PTRACER_ASSERT(pid == thread->m_tid);

			long longret = ptrace(PTRACE_DETACH, thread->m_tid, 0, 0);
			if (longret == -1) {
				PTRACER_DEBUG("PTRACE_DETACH failed for thread %d, errno %d.", thread->m_tid, errno);
			}

			delete thread;
			m_thread_set.erase(iter++);
		}

		m_pid = 0;
		PTRACER_ASSERT(status_isshut());

		err_no = PTCR_ERRNO_UNKONWN;
		return_pid = 0;

		delete process_info;
		goto finish;
	}

	delete process_info;

	err_no = PTCR_ERRNO_SUCCESS;
	return_pid = request_pid;

	response.head.code = PTCR_RESP_ATTACH;
	response.head.len = sizeof(response);
	response.pid = return_pid;
	response.err_no = err_no;
	msg_send(&response, response.head.len);

	//不过wait_a_stop_but_no_siginfo通过wait是不会触发了，只能就地调用
	for(it = m_thread_set.begin(); it != m_thread_set.end(); it++) {
		unsigned int pid = it->first;
		ptracer_thread *thread = it->second;
		PTRACER_ASSERT(thread != NULL);
		PTRACER_ASSERT(pid == thread->m_tid);

		struct waitinfo info;
		info.pid = thread->m_tid;
		wait_a_stop_but_no_siginfo(&info);
	}

	return 0;

finish:
	//都是失败返回，因为wait_a_stop_but_no_siginfo
	struct ptr_resp_attach response_finish = {0};
	response_finish.head.code = PTCR_RESP_ATTACH;
	response_finish.head.len = sizeof(response_finish);
	response_finish.pid = return_pid;
	response_finish.err_no = err_no;
	msg_send(&response_finish, response_finish.head.len);
	return 0;
}

struct ptr_req_detach {
	struct ptr_msg head;
	unsigned int pid;
};
struct ptr_resp_detach {
	struct ptr_msg head;
	unsigned int pid;
	unsigned int err_no;
};

/*
 * 对线程detach
 * */
int ptracer::request_detach(struct ptr_req_detach *request)
{
	PTRACER_ASSERT(request != NULL);

	PTRACER_DEBUG("Receive request detach.");

	unsigned int pid;
	unsigned int err_no;

	ptracer_thread *pthread;
	long longret;

	//检查参数
	if (request->pid == 0) {
		PTRACER_LOG("Wrong thread id for detach.");
		err_no = PTCR_ERRNO_INVALID_ARG;
		pid = 0;
		goto finish;
	}

	//环境检查
	if (status_isshut()) {
		PTRACER_DEBUG("Reject detach request, because not running now.");
		err_no = PTCR_ERRNO_NOT_RUN;
		pid = 0;
		goto finish;
	}

	//执行
	pthread = threadGetById(request->pid);
	if (pthread == NULL) {
		PTRACER_LOG("No thread information for thread %d.", request->pid);
		err_no = PTCR_ERRNO_NOSUCHTHREAD;
		pid = 0;
		goto finish;
	}
	PTRACER_ASSERT(pthread->m_tid == request->pid);

	longret = ptrace(PTRACE_DETACH, request->pid, 0, 0);
	if (longret == -1) {
		PTRACER_DEBUG("PTRACE_DETACH failed for thread %d, errno %d.", request->pid, errno);
		//no others
	}

	thread_delete_byid(pthread->m_tid);
	if (m_pid == request->pid) {
		m_pid = 0;
	}

	err_no = PTCR_ERRNO_SUCCESS;
	pid = request->pid;

finish:
	struct ptr_resp_detach response = {0};
	response.head.code = PTCR_RESP_DETACH;
	response.head.len = sizeof(response);
	response.pid = pid;
	response.err_no = err_no;
	msg_send(&response, response.head.len);
	return 0;
}

#endif

int Ptracer::connClosed()
{
	map<unsigned int, PtracerThread*>::iterator iter;
	for(iter = m_thread_set.begin(); iter != m_thread_set.end(); /*none*/) {
		unsigned int pid = iter->first;
		PtracerThread *thread = iter->second;

		long longret = ptrace(PTRACE_DETACH, thread->m_tid, 0, 0);
		if (longret == -1) {
			PTRACER_DEBUG("PTRACE_DETACH failed for thread %d, errno %d.", thread->m_tid, errno);
		}
		tgkill(m_pid, pid, SIGKILL);

		delete thread;
		m_thread_set.erase(iter++);
	}

	return 0;
}

PtracerBreak *Ptracer::breakGetByAddr(const unsigned long long addr)
{
	map<unsigned long long, PtracerBreak*>::iterator iter;
	iter = m_break_set.find(addr);
	if(iter == m_break_set.end()) {
		return NULL;
	} else {
		return iter->second;
	}
}

int Ptracer::breakDelByAddr(const unsigned long long addr)
{
	map<unsigned long long, PtracerBreak*>::iterator it = m_break_set.find(addr);
	if(it == m_break_set.end()) {
		PTRACER_ASSERT(0);
		return -1;
	} else {
		delete it->second;
		m_break_set.erase(it);
		return 0;
	}
}

int Ptracer::breakInsert(PtracerBreak *brkpnt)
{
	PTRACER_ASSERT(brkpnt != NULL);
	/*check first*/
	if(m_break_set.find(brkpnt->m_address) != m_break_set.end()) {
		PTRACER_ASSERT(0);
		return -1;
	}
	m_break_set.insert(map<unsigned long long, PtracerBreak*>::value_type(brkpnt->m_address, brkpnt));
	return 0;
}

PtracerThread *Ptracer::threadGetById(unsigned int pid)
{
	map<unsigned int, PtracerThread*>::iterator it = m_thread_set.find(pid);
	if(it == m_thread_set.end()) {
		return NULL;
	} else {
		PTRACER_ASSERT(it->first == pid);
		PTRACER_ASSERT(it->second != NULL);
		return it->second;
	}

	return NULL;
}

int Ptracer::threadDelById(unsigned int pid)
{
	map<unsigned int, PtracerThread*>::iterator it= m_thread_set.find(pid);
	if(it == m_thread_set.end()) {
		return -1;
	} else {
		PTRACER_ASSERT(it->first == pid);
		PTRACER_ASSERT(it->second != NULL);
		delete it->second;
		m_thread_set.erase(it);
		return 0;
	}
}

int Ptracer::threadInsert(PtracerThread *thread)
{
	PTRACER_ASSERT(thread != NULL);
	PTRACER_ASSERT(thread->m_tid != 0);
	if(m_thread_set.find(thread->m_tid) != m_thread_set.end()) {
		return -1;
	}
	m_thread_set.insert(map<unsigned int, PtracerThread*>::value_type(thread->m_tid, thread));
	return 0;
}

//非运行状态
bool Ptracer::someThreadAlive()
{
	map<unsigned int, PtracerThread*>::iterator iter;
	for (iter = m_thread_set.begin(); iter != m_thread_set.end(); iter++) {
		PtracerThread *thread = iter->second;
		if (thread->m_status == PtracerThread::STATUS_STOP
			|| thread->m_status == PtracerThread::STATUS_WAIT
			|| thread->m_status == PtracerThread::STATUS_PREEND) {
			;
		} else {
			return true;
		}
	}
	return false;
}

string Ptracer::doRequestInit(string &request, string &request_head, string &response_head)
{
	return response_head + "result=success";
}

string Ptracer::doRequestLogLevel(string &request, string &request_head, string &response_head)
{
	string request_content = request.substr(request_head.length());
	map<string,string> kv_map = msgToMap(request_content);
	map<string,string>::iterator kv_iter;
	string value;
	kv_iter = kv_map.find("level");
	if (kv_iter == kv_map.end()) {
		return response_head + "result=failed&reason=no level specified";
	}
	value = kv_iter->second;

	unsigned int level = strtoull(value.c_str(), NULL, 10);
	ptracer_log_level = level;

	PTRACER_LOG("Log test.");
	PTRACER_DEBUG("Debug test.");

	return response_head + "result=success";
}

/*Start a new process with arguments and environment variables*/
string Ptracer::doRequestStartNew(string &request, string &request_head, string &response_head)
{
	string request_content = request.substr(request_head.length());
	list<Ptracer::ListNode> kv_list = msgToList(request_content);
	list<Ptracer::ListNode>::iterator kv_iter;

	string path = "";
	list<string> arg_list;
	list<string> env_list;

	for (kv_iter = kv_list.begin(); kv_iter != kv_list.end(); kv_iter++) {
		Ptracer::ListNode node = *kv_iter;
		if (node.key == "path") {
			path = msgDecode(node.value);
		} else if (node.key == "arg") {
			arg_list.push_back(msgDecode(node.value));
		} else if (node.key == "env") {
			env_list.push_back(msgDecode(node.value));
		} else {
			;
		}
	}
	if (path == "") {
		return response_head + "result=failed&reason=no path specified";
	}

	/*Path is first argument*/
	arg_list.push_front(path);

	if (arg_list.size() > PTRACER_ARG_ENV_MAX) {
		return response_head + "result=failed&reason=too much start arguments";
	}
	if (env_list.size() > PTRACER_ARG_ENV_MAX) {
		return response_head + "result=failed&reason=too much start environment";
	}

	/*Check current threads*/
	if (! m_thread_set.empty()) {
		return response_head + "result=failed&reason=target running";
	}

	int result;

	int pipe_stdin[2];
	int pipe_stdout[2];
	result = pipe(pipe_stdin);
	if (result < 0) {
		return response_head + "result=failed&reason=pipe error";
	}
	result = pipe(pipe_stdout);
	if (result < 0) {
		return response_head + "result=failed&reason=pipe error";
	}

FORK_AGAIN:
	/*Start child*/
	pid_t child = fork();
    if (child == 0) {	/*In child process*/
    	/*Close the terminal for parent*/
        close(pipe_stdin[1]);
        close(pipe_stdout[0]);
        /*Read stdin from pipe_stdin[0], Wrtie stdout to pipe_stdout[1], Wrtie stderr to pipe_stderr[1]*/
        dup2(pipe_stdin[0], STDIN_FILENO);		//0 <- pipe
        dup2(pipe_stdout[1], STDOUT_FILENO);	//1 -> pipe
        dup2(pipe_stdout[1], STDERR_FILENO);	//2 -> pipe
        close(pipe_stdin[0]);
        close(pipe_stdout[1]);

    	/*Trace me*/
        long longret = ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        if (longret == -1) {
        	PTRACER_LOG("PTRACE_TRACEME failed for thead %d, errno = %d\n", 0, errno);
        	exit(PTCR_CHILD_EXIT_E2BIG);
        }

        /*Prefix child process exec*/
    	char *arg_array[PTRACER_ARG_ENV_MAX + 1];
    	char *env_array[PTRACER_ARG_ENV_MAX + 1];
    	int iloop;
    	list<string>::iterator iter;
    	iloop = 0;
    	for (iter = arg_list.begin(); iter != arg_list.end(); iter++) {
    		string tmp = *iter;
    		arg_array[iloop++] = strdup(tmp.c_str());
    	}
    	arg_array[iloop] = NULL;
    	iloop = 0;
    	for (iter = env_list.begin(); iter != env_list.end(); iter++) {
    		string tmp = *iter;
    		env_array[iloop++] = strdup(tmp.c_str());
    	}
    	env_array[iloop] = NULL;

    	/*Child process exec*/
        int result = execvpe(path.c_str(), arg_array, env_array);

        /*Postfix child process exec*/
    	for (iloop = 0; iloop < arg_list.size(); iloop++) {
    		free(arg_array[iloop]);
    	}
    	for (iloop = 0; iloop < env_list.size(); iloop++) {
    		free(env_array[iloop]);
    	}

    	/*If exec failed, exit child.*/
        if (result == -1) {
        	PTRACER_DEBUG("Execvpe failed, errno = %d\n", errno);
        	exit(errno);
        }
    } else if (child == -1 && errno == EAGAIN) {
   		/*In parrent procees, need do fork again*/
    	goto FORK_AGAIN;
    } else if (child == -1) {
    	close(pipe_stdin[0]);
        close(pipe_stdin[1]);
        close(pipe_stdout[0]);
        close(pipe_stdout[1]);
    	/*In parrent process, fork failed*/
		return response_head + "result=failed&reason=fork failed errno " + string_format("%d", errno);
    }

    /*In parent process, close terminal for child*/
    close(pipe_stdin[0]);
    close(pipe_stdout[1]);
    m_child_pipe_stdin = pipe_stdin[1];
    m_child_pipe_stdout = pipe_stdout[0];

    int flags;
    flags = fcntl(m_child_pipe_stdin, F_GETFL);
    fcntl(m_child_pipe_stdin, F_SETFL, flags | O_NONBLOCK);
    flags = fcntl(m_child_pipe_stdout, F_GETFL);
    fcntl(m_child_pipe_stdout, F_SETFL, flags | O_NONBLOCK);

    PtracerThread *new_thread = new PtracerThread();
    new_thread->m_tid = child;
    new_thread->statusSet(PtracerThread::STATUS_NEW);
    threadInsert(new_thread);

    m_pid = child;

    /*Wait start*/
	return response_head + "result=success&pid=" + string_format("%d", child);
}

/*Get pc of pid*/
string Ptracer::doRequestInstrGet(string &request, string &request_head, string &response_head)
{
	int result;

	string request_content = request.substr(request_head.length());
	map<string,string> kv_map = msgToMap(request_content);
	map<string,string>::iterator kv_iter;
	string value;
	kv_iter = kv_map.find("pid");
	if (kv_iter == kv_map.end()) {
		return response_head + "result=failed&reason=no pid specified";
	}
	value = kv_iter->second;
	unsigned int pid = strtoul(value.c_str(), NULL, 10);
	if (pid == 0) {
		return response_head + "result=failed&reason=invalid pid";
	}

	PtracerThread *pthread = threadGetById(pid);
	if (pthread == NULL) {
		return response_head + "result=failed&reason=no such thread";
	}
	if (pthread->m_status != PtracerThread::STATUS_STOP) {
		return response_head + "result=failed&reason=thread running";
	}

	unsigned long long addr;
	result = m_arch->instrAddrGet(pid, &addr);
	if (result != 0) {
		PTRACER_LOG("Get current instruction failed on thread %d.", pid);
		return response_head + "result=failed&reason=get instruction failed";
	}

	PTRACER_DEBUG("Get current instruction on thread %d.", pid);
	return response_head + "result=success&value=" + string_format("%016llx", addr);
}

string Ptracer::doRequestStackGet(string &request, string &request_head, string &response_head)
{
	int result;

	string request_content = request.substr(request_head.length());
	map<string,string> kv_map = msgToMap(request_content);
	map<string,string>::iterator kv_iter;
	string value;
	kv_iter = kv_map.find("pid");
	if (kv_iter == kv_map.end()) {
		return response_head + "result=failed&reason=no pid specified";
	}
	value = kv_iter->second;
	unsigned int pid = strtoul(value.c_str(), NULL, 10);
	if (pid == 0) {
		return response_head + "result=failed&reason=invalid pid";
	}

	PtracerThread *pthread = threadGetById(pid);
	if (pthread == NULL) {
		return response_head + "result=failed&reason=no such thread";
	}
	if (pthread->m_status != PtracerThread::STATUS_STOP) {
		return response_head + "result=failed&reason=thread running";
	}

	unsigned long long addr;
	result = m_arch->stackAddrGet(pid, &addr);
	if (result != 0) {
		PTRACER_LOG("Get current stack failed on thread %d.", pid);
		return response_head + "result=failed&reason=get instruction failed";
	}

	PTRACER_DEBUG("Get current stack on thread %d.", pid);
	return response_head + "result=success&value=" + string_format("%016llx", addr);
}


string Ptracer::doRequestRegGet(string &request, string &request_head, string &response_head)
{
	int result;

	string request_content = request.substr(request_head.length());
	map<string,string> kv_map = msgToMap(request_content);
	map<string,string>::iterator kv_iter;
	string value;
	kv_iter = kv_map.find("pid");
	if (kv_iter == kv_map.end()) {
		return response_head + "result=failed&reason=no pid specified";
	}
	value = kv_iter->second;
	unsigned int pid = strtoul(value.c_str(), NULL, 10);
	if (pid == 0) {
		return response_head + "result=failed&reason=invalid pid";
	}
	kv_iter = kv_map.find("name");
	if (kv_iter == kv_map.end()) {
		return response_head + "result=failed&reason=no name specified";
	}
	string name = kv_iter->second;
	if (name == "") {
		return response_head + "result=failed&reason=invalid name";
	}

	PtracerThread *pthread = threadGetById(pid);
	if (pthread == NULL) {
		return response_head + "result=failed&reason=no such thread";
	}
	if (pthread->m_status != PtracerThread::STATUS_STOP) {
		return response_head + "result=failed&reason=thread running";
	}

	const char *name_str = name.c_str();
	unsigned long long reg_value = 0;
	result = m_arch->regWordGet(pid, name_str, &reg_value);
	if (result != 0) {
		PTRACER_LOG("Get register %s failed on thread %d.", name_str, pid);
		return response_head + "result=failed&reason=get register failed";
	}

	PTRACER_DEBUG("Get register %s on thread %d.", name_str, pid);
	return response_head + "result=success&value=" + string_format("%016llx", reg_value);
}

string Ptracer::doRequestRegSet(string &request, string &request_head, string &response_head)
{
	int result;

	string request_content = request.substr(request_head.length());
	map<string,string> kv_map = msgToMap(request_content);
	map<string,string>::iterator kv_iter;
	string value;
	kv_iter = kv_map.find("pid");
	if (kv_iter == kv_map.end()) {
		return response_head + "result=failed&reason=no pid specified";
	}
	value = kv_iter->second;
	unsigned int pid = strtoul(value.c_str(), NULL, 10);
	if (pid == 0) {
		return response_head + "result=failed&reason=invalid pid";
	}
	kv_iter = kv_map.find("name");
	if (kv_iter == kv_map.end()) {
		return response_head + "result=failed&reason=no name specified";
	}
	string name = kv_iter->second;
	if (name == "") {
		return response_head + "result=failed&reason=invalid name";
	}
	kv_iter = kv_map.find("value");
	if (kv_iter == kv_map.end()) {
		return response_head + "result=failed&reason=no value specified";
	}
	value = kv_iter->second;
	unsigned long long reg_value = strtoull(value.c_str(), NULL, 16);

	PtracerThread *pthread = threadGetById(pid);
	if (pthread == NULL) {
		return response_head + "result=failed&reason=no such thread";
	}
	if (pthread->m_status != PtracerThread::STATUS_STOP) {
		return response_head + "result=failed&reason=thread running";
	}

	const char *name_str = name.c_str();
	result = m_arch->regWordSet(pid, name_str, reg_value);
	if (result != 0) {
		PTRACER_LOG("Set register %s failed for thread %d.", name_str, pid);
		return response_head + "result=failed&reason=set register failed";
	}
	PTRACER_DEBUG("Set register %s on thread %d", name_str, pid);
	return response_head + "result=success";
}

string Ptracer::doRequestMemGet(string &request, string &request_head, string &response_head)
{
	int result;

	string request_content = request.substr(request_head.length());
	map<string,string> kv_map = msgToMap(request_content);
	map<string,string>::iterator kv_iter;
	string value;
	kv_iter = kv_map.find("addr");
	if (kv_iter == kv_map.end()) {
		return response_head + "result=failed&reason=no addr specified";
	}
	value = kv_iter->second;
	unsigned long long addr_value = strtoull(value.c_str(), NULL, 16);
	kv_iter = kv_map.find("len");
	if (kv_iter == kv_map.end()) {
		return response_head + "result=failed&reason=no len specified";
	}
	value = kv_iter->second;
	unsigned int len_value = strtoul(value.c_str(), NULL, 10);

	PtracerThread *pthread = threadGetById(m_pid);
	if (pthread == NULL) {
		return response_head + "result=failed&reason=no such thread";
	}
	if (pthread->m_status != PtracerThread::STATUS_STOP) {
		return response_head + "result=failed&reason=thread running";
	}
	if (someThreadAlive()) {
		return response_head + "result=failed&reason=some thread running";
	}

	string value_string;
	unsigned int iloop;
	for (iloop = 0; iloop < len_value; iloop++) {
		unsigned char char_as_arry = 0;
		result = m_arch->memBytesGet(m_pid, addr_value + iloop, &char_as_arry, 1);
		if (result != 0) {
			return response_head + "result=failed&reason=get memory failed.";
		}
		value_string += string_format("%02x", char_as_arry);
	}

	return response_head + "result=success&value=" + value_string;
}

string Ptracer::doRequestMemSet(string &request, string &request_head, string &response_head)
{
	int result;

	string request_content = request.substr(request_head.length());
	map<string,string> kv_map = msgToMap(request_content);
	map<string,string>::iterator kv_iter;
	string value;
	kv_iter = kv_map.find("addr");
	if (kv_iter == kv_map.end()) {
		return response_head + "result=failed&reason=no addr specified";
	}
	value = kv_iter->second;
	unsigned long long addr_value = strtoull(value.c_str(), NULL, 16);
	kv_iter = kv_map.find("len");
	if (kv_iter == kv_map.end()) {
		return response_head + "result=failed&reason=no len specified";
	}
	value = kv_iter->second;
	unsigned int len_value = strtoul(value.c_str(), NULL, 10);
	kv_iter = kv_map.find("value");
	if (kv_iter == kv_map.end()) {
		return response_head + "result=failed&reason=no value specified";
	}
	string value_str = kv_iter->second;
	if (len_value * 2 != value_str.length()) {
		return response_head + "result=failed&reason=wrong value length";
	}

	PtracerThread *pthread = threadGetById(m_pid);
	if (pthread == NULL) {
		return response_head + "result=failed&reason=no such thread";
	}
	if (pthread->m_status != PtracerThread::STATUS_STOP) {
		return response_head + "result=failed&reason=thread running";
	}
	if (someThreadAlive()) {
		return response_head + "result=failed&reason=some thread running";
	}

	unsigned int iloop;
	for (iloop = 0; iloop < len_value; iloop++) {
		string byte = value_str.substr(iloop * 2, 2);
		unsigned char byte_value = strtoull(byte.c_str(), NULL, 16);
		result = m_arch->memBytesSet(m_pid, addr_value + iloop, &byte_value, 1);
		if (result != 0) {
			return response_head + "result=failed&reason=set memory failed.";
		}
	}

	return response_head + "result=success";
}

string Ptracer::doRequestBreakSet(string &request, string &request_head, string &response_head)
{
	int result;

	string request_content = request.substr(request_head.length());
	map<string,string> kv_map = msgToMap(request_content);
	map<string,string>::iterator kv_iter;
	string value;
	kv_iter = kv_map.find("addr");
	if (kv_iter == kv_map.end()) {
		return response_head + "result=failed&reason=no addr specified";
	}
	value = kv_iter->second;
	unsigned long long addr_value = strtoull(value.c_str(), NULL, 16);

	PtracerThread *pthread = threadGetById(m_pid);
	if (pthread == NULL) {
		return response_head + "result=failed&reason=no such thread";
	}
	if (pthread->m_status != PtracerThread::STATUS_STOP) {
		return response_head + "result=failed&reason=thread running";
	}
	if (someThreadAlive()) {
		return response_head + "result=failed&reason=some thread running";
	}

	PtracerBreak *brkpnt = breakGetByAddr(addr_value);
	if (brkpnt != NULL) {
		return response_head + "result=failed&reason=breakpoint exist";
	}

	brkpnt = new PtracerBreak(addr_value);
	result = m_arch->brkpntSet(m_pid, brkpnt);
	if (result != 0) {
		PTRACER_LOG("Set break failed at 0x%016llx.", addr_value);
		delete brkpnt;
		return response_head + "result=failed&reason=set breakpoint exist";
	}
	breakInsert(brkpnt);

	return response_head + "result=success";
}

string Ptracer::doRequestBreakClear(string &request, string &request_head, string &response_head)
{
	int result;

	string request_content = request.substr(request_head.length());
	map<string,string> kv_map = msgToMap(request_content);
	map<string,string>::iterator kv_iter;
	string value;
	kv_iter = kv_map.find("addr");
	if (kv_iter == kv_map.end()) {
		return response_head + "result=failed&reason=no addr specified";
	}
	value = kv_iter->second;
	unsigned long long addr_value = strtoull(value.c_str(), NULL, 16);

	PtracerThread *pthread = threadGetById(m_pid);
	if (pthread == NULL) {
		return response_head + "result=failed&reason=no such thread";
	}
	if (pthread->m_status != PtracerThread::STATUS_STOP) {
		return response_head + "result=failed&reason=thread running";
	}
	if (someThreadAlive()) {
		return response_head + "result=failed&reason=some thread running";
	}

	if (addr_value == 0) {
		map<unsigned long long, PtracerBreak*>::iterator iter;
		for (iter = m_break_set.begin(); iter != m_break_set.end(); /*none*/) {
			PtracerBreak *brkpnt = iter->second;
			result = m_arch->brkpntClear(m_pid, brkpnt);
			if (result != 0) {
				PTRACER_LOG("Clean break failed at 0x%016llx.", brkpnt->m_address);
			}
			delete brkpnt;
			m_break_set.erase(iter++);
		}
		return response_head + "result=success";
	} else {
		PtracerBreak *brkpnt = breakGetByAddr(addr_value);
		if (brkpnt == NULL) {
			PTRACER_DEBUG("No break at %016llx.", addr_value);
			return response_head + "result=failed&reason=no such breakpoint";
		}
		result = m_arch->brkpntClear(m_pid, brkpnt);
		if (result != 0) {
			PTRACER_LOG("Clean break failed at 0x%016llx.", brkpnt->m_address);
			return response_head + "result=failed&reason=unset breakpint failed";
		}
		breakDelByAddr(addr_value);
		return response_head + "result=success";
	}

	return response_head + "result=success";
}

string Ptracer::doRequestMapGet(string &request, string &request_head, string &response_head)
{
	int result;

	string request_content = request.substr(request_head.length());
	map<string,string> kv_map = msgToMap(request_content);
	map<string,string>::iterator kv_iter;
	string value;
	kv_iter = kv_map.find("pid");
	if (kv_iter == kv_map.end()) {
		return response_head + "result=failed&reason=no pid specified";
	}
	value = kv_iter->second;
	unsigned int pid = strtoul(value.c_str(), NULL, 10);
	if (pid == 0) {
		return response_head + "result=failed&reason=invalid pid";
	}

	PtracerThread *pthread = threadGetById(pid);
	if (pthread == NULL) {
		return response_head + "result=failed&reason=no such thread";
	}
	if (pthread->m_status != PtracerThread::STATUS_STOP) {
		return response_head + "result=failed&reason=thread running";
	}
	if (someThreadAlive()) {
		return response_head + "result=failed&reason=some thread running";
	}

	LinuxMaps mod_list;
	result = mod_list.loadMapInfo(pid);
	if (result < 0) {
		return response_head + "result=failed&reason=get map information failed";
	}

	string value_str;
	list<LinuxMaps::LinuxMap*>::iterator iter;
	for(iter = mod_list.m_map_list.begin(); iter != mod_list.m_map_list.end(); iter++) {
		LinuxMaps::LinuxMap *mod = *iter;
		value_str += string_format("&start=%016llx&end=%016llx&offset=%016llx", mod->m_start, mod->m_end, mod->m_offset);
		value_str += string_format("&flag_r=", mod->m_flag_r?"true":"false");
		value_str += string_format("&flag_w=", mod->m_flag_w?"true":"false");
		value_str += string_format("&flag_x=", mod->m_flag_x?"true":"false");
		value_str += string_format("&flag_p=", mod->m_flag_p?"true":"false");
		value_str += string_format("&major=%d&minor=%d&inode=%d", mod->m_major, mod->m_minor, mod->m_inode);
		value_str += "&path=" + mod->path;
		value_str += "&separator=----";
	}

	return response_head + value_str;
}

string Ptracer::doRequestThreadsGet(string &request, string &request_head, string &response_head)
{
	int result;

	string request_content = request.substr(request_head.length());
	map<string,string> kv_map = msgToMap(request_content);
	map<string,string>::iterator kv_iter;
	string value;
	kv_iter = kv_map.find("pid");
	if (kv_iter == kv_map.end()) {
		return response_head + "result=failed&reason=no pid specified";
	}
	value = kv_iter->second;
	unsigned int pid = strtoul(value.c_str(), NULL, 10);
	if (pid == 0) {
		return response_head + "result=failed&reason=invalid pid";
	}

	PtracerThread *pthread = threadGetById(m_pid);
	if (pthread == NULL) {
		return response_head + "result=failed&reason=no such thread";
	}
	if (pthread->m_status != PtracerThread::STATUS_STOP) {
		return response_head + "result=failed&reason=thread running";
	}
	if (someThreadAlive()) {
		return response_head + "result=failed&reason=some thread running";
	}

	LinuxThreads processinfo;
	result = processinfo.loadThreadInfo(pid);
	if (result < 0) {
		return response_head + "result=failed&reason=get threads information failed";
	}

	string value_str;
	list<LinuxThreads::LinuxThread*>::iterator iter;
	for (iter = processinfo.m_thread_list.begin(); iter != processinfo.m_thread_list.end(); iter++) {
		LinuxThreads::LinuxThread *linuxthread = *iter;
		value_str += string_format("&pid=%d", linuxthread->m_tid);
		value_str += "&name=" + linuxthread->m_name;
		value_str += "&separator=----";
	}

	return response_head + value_str;
}

string Ptracer::doRequestShut(string &request, string &request_head, string &response_head)
{
	int result;

	string request_content = request.substr(request_head.length());
	map<string,string> kv_map = msgToMap(request_content);
	map<string,string>::iterator kv_iter;
	string value;
	kv_iter = kv_map.find("pid");
	if (kv_iter == kv_map.end()) {
		return response_head + "result=failed&reason=no pid specified";
	}
	value = kv_iter->second;
	unsigned int pid = strtoul(value.c_str(), NULL, 10);

	if (pid == 0) {
		/*直接杀主线程*/
		result = kill(m_pid, SIGKILL);
		if (result != 0) {
			PTRACER_DEBUG("kill failed for thread %d, errno %d.", pid, errno);
		}
	} else {
		result = tgkill(m_pid, pid, SIGKILL);
		if (result != 0) {
			PTRACER_DEBUG("kill failed for thread %d, errno %d.", pid, errno);
		}
	}

	return response_head + "result=success";
}

string Ptracer::doRequestStop(string &request, string &request_head, string &response_head)
{
	int result;

	string request_content = request.substr(request_head.length());
	map<string,string> kv_map = msgToMap(request_content);
	map<string,string>::iterator kv_iter;
	string value;
	kv_iter = kv_map.find("pid");
	if (kv_iter == kv_map.end()) {
		return response_head + "result=failed&reason=no pid specified";
	}
	value = kv_iter->second;
	unsigned int pid = strtoul(value.c_str(), NULL, 10);

	if (pid == 0) {
		PTRACER_DEBUG("Stop all thread.");
		map<unsigned int, PtracerThread*>::iterator it;
		for(it = m_thread_set.begin(); it != m_thread_set.end(); it++) {
			PtracerThread *pthread = it->second;
			if (pthread->m_status == PtracerThread::STATUS_RUN) {
				PTRACER_DEBUG("Stop all thread on thread %d.", pthread->m_tid);
				/* 只改标记m_stop_sig_flag，不切状态。这里的m_stop_sig_flag用于表示，发出过STOP信号并期待STOP的意思，
				 * 如果因为tgkill的STOP真的来了，就算操作成功了。如果是因为其他原因原因停止，则也算停止成功，总之停下就行*/
				pthread->m_flag_take_stop = true;
				int result = tgkill(m_pid, pthread->m_tid, SIGSTOP);
				if (result != 0) {
					PTRACER_LOG("tgkill failed for thead %d, errno %d.", pthread->m_tid, errno);
					continue;
				}
			}
		}
		return response_head + "result=success";
	} else {
		PTRACER_DEBUG("Stop thread %d.", pid);
		PtracerThread *pthread = threadGetById(pid);
		if (pthread == NULL) {
			PTRACER_LOG("No thread information for thread %d.", pid);
			return response_head + "result=failed&reason=no such thread";
		}
		if (pthread->m_status != PtracerThread::STATUS_RUN) {
			PTRACER_DEBUG("Wrong status %s for stop operate by thread %d.",
					PtracerThread::status_desc[pthread->m_status], pid);
			return response_head + "result=failed&reason=wrong thread status";
		}
		//向线程发送信号，标记上stop信号是软件自己发送的，当发生停止的时候哦，不会认为是什么用户乱发的。
		pthread->m_flag_take_stop = true;
		int result = tgkill(m_pid, pthread->m_tid, SIGSTOP);
		if (result != 0) {
			PTRACER_DEBUG("tgkill failed for thread %d, errno %d.", pthread->m_tid, errno);
			return response_head + "result=failed&reason=stop thread failed";
		}
		//只改标记m_stop_sig_flag，不切状态
		return response_head + "result=success";
	}

	return response_head + "result=success";
}

string Ptracer::doRequestStep(string &request, string &request_head, string &response_head)
{
	int result;

	string request_content = request.substr(request_head.length());
	map<string,string> kv_map = msgToMap(request_content);
	map<string,string>::iterator kv_iter;
	string value;
	kv_iter = kv_map.find("pid");
	if (kv_iter == kv_map.end()) {
		return response_head + "result=failed&reason=no pid specified";
	}
	value = kv_iter->second;
	unsigned int pid = strtoul(value.c_str(), NULL, 10);

	kv_iter = kv_map.find("signum");
	if (kv_iter == kv_map.end()) {
		return response_head + "result=failed&reason=no singnal number specified";
	}
	value = kv_iter->second;
	unsigned int signum = strtoul(value.c_str(), NULL, 10);

	if (pid == 0) {
		return response_head + "result=failed&reason=wrong thread id";
	}

	PtracerThread *pthread = threadGetById(pid);
	if (pthread == NULL) {
		return response_head + "result=failed&reason=no such thread";
	}

	PTRACER_DEBUG("Step thread %d", pid);

	if (pthread->m_status != PtracerThread::STATUS_STOP) {
		PTRACER_DEBUG("Wrong status %s for step operate by thread %d.",
				PtracerThread::status_desc[pthread->m_status], pid);
		return response_head + "result=failed&reason=wrong thread status";
	}

	long longret = ptrace(PTRACE_SINGLESTEP, pthread->m_tid, 0, signum);
	if (longret == -1) {
		PTRACER_DEBUG("PTRACE_SINGLESTEP failed for thread %d, errno %d.", pthread->m_tid, errno);
		return response_head + "result=failed&reason=step failed";
	}

	if (pthread->m_status == PtracerThread::STATUS_STOP) {
		pthread->statusSet(PtracerThread::STATUS_RUN);
	}

	return response_head + "result=success";
}

string Ptracer::doRequestContinue(string &request, string &request_head, string &response_head)
{
	int result;

	string request_content = request.substr(request_head.length());
	map<string,string> kv_map = msgToMap(request_content);
	map<string,string>::iterator kv_iter;
	string value;
	kv_iter = kv_map.find("pid");
	if (kv_iter == kv_map.end()) {
		return response_head + "result=failed&reason=no pid specified";
	}
	value = kv_iter->second;
	unsigned int pid = strtoul(value.c_str(), NULL, 10);

	kv_iter = kv_map.find("signum");
	if (kv_iter == kv_map.end()) {
		return response_head + "result=failed&reason=no singnal number specified";
	}
	value = kv_iter->second;
	unsigned int signum = strtoul(value.c_str(), NULL, 10);

	if (pid == 0) {
		PTRACER_DEBUG("Continue all thread.");
		map<unsigned int, PtracerThread*>::iterator it;
		for(it = m_thread_set.begin(); it != m_thread_set.end(); it++) {
			PtracerThread *pthread = it->second;
			if (pthread->m_status == PtracerThread::STATUS_STOP
				|| pthread->m_status == PtracerThread::STATUS_PREEND) {
				PTRACER_DEBUG("Continue all thread on thread %d.", pthread->m_tid);
				long longret = ptrace(PTRACE_CONT, pthread->m_tid, 0, signum);
				if (longret == -1) {
					PTRACER_LOG("PTRACE_CONT failed for thread %d, errno %d.", pthread->m_tid, errno);
					continue;
				}
				if (pthread->m_status == PtracerThread::STATUS_STOP) {
					pthread->statusSet(PtracerThread::STATUS_RUN);
				} else {
					pthread->statusSet(PtracerThread::STATUS_ENDING);
				}
			}
		}
		return response_head + "result=success";
	} else {
		PTRACER_DEBUG("Continue thread %d.", pid);
		PtracerThread *pthread = threadGetById(pid);
		if (pthread == NULL) {
			PTRACER_LOG("No thread information for thread %d.", pid);
			return response_head + "result=failed&reason=no such thread";
		}
		if (! (pthread->m_status == PtracerThread::STATUS_STOP
			   || pthread->m_status == PtracerThread::STATUS_PREEND)) {
			PTRACER_DEBUG("Wrong status %s for stop operate by thread %d.",
					PtracerThread::status_desc[pthread->m_status], pid);
			return response_head + "result=failed&reason=wrong thread status";
		}
		long longret = ptrace(PTRACE_CONT, pid, 0, signum);
		if (longret == -1) {
			PTRACER_LOG("PTRACE_CONT failed for thread %d, errno %d.", pid, errno);
			return response_head + "result=failed&reason=continue failed";
		}
		if (pthread->m_status == PtracerThread::STATUS_STOP) {
			pthread->statusSet(PtracerThread::STATUS_RUN);
		} else {
			pthread->statusSet(PtracerThread::STATUS_ENDING);
		}
		return response_head + "result=success";
	}

	return response_head + "result=success";
}

string Ptracer::doRequest(string &request)
{
	PTRACER_ASSERT(request.length() != 0);

	if (       request.find("request://init?") == 0) {
		string request_head = "request://init?";
		string response_head = "response://init?";
		return doRequestInit(request, request_head, response_head);
	} else if (request.find("request://loglvl?") == 0) {
		string request_head = "request://loglvl?";
		string response_head = "response://loglvl?";
		return doRequestLogLevel(request, request_head, response_head);
	} else if (request.find("request://startnew?") == 0) {
		string request_head = "request://startnew?";
		string response_head = "response://startnew?";
		return doRequestStartNew(request, request_head, response_head);
	} else if (request.find("request://instrget?") == 0) {
		string request_head = "request://instrget?";
		string response_head = "response://instrget?";
		return doRequestInstrGet(request, request_head, response_head);
	} else if (request.find("request://stackget?") == 0) {
		string request_head = "request://stackget?";
		string response_head = "response://stackget?";
		return doRequestStackGet(request, request_head, response_head);
	} else if (request.find("request://regget?") == 0) {
		string request_head = "request://regget?";
		string response_head = "response://regget?";
		return doRequestRegGet(request, request_head, response_head);
	} else if (request.find("request://regset?") == 0) {
		string request_head = "request://regset?";
		string response_head = "response://regset?";
		return doRequestRegSet(request, request_head, response_head);
	} else if (request.find("request://memget?") == 0) {
		string request_head = "request://memget?";
		string response_head = "response://memget?";
		return doRequestMemGet(request, request_head, response_head);
	} else if (request.find("request://memset?") == 0) {
		string request_head = "request://memset?";
		string response_head = "response://memset?";
		return doRequestMemSet(request, request_head, response_head);
	} else if (request.find("request://breakset?") == 0) {
		string request_head = "request://breakset?";
		string response_head = "response://breakset?";
		return doRequestBreakSet(request, request_head, response_head);
	} else if (request.find("request://breakclear?") == 0) {
		string request_head = "request://breakclear?";
		string response_head = "response://breakclear?";
		return doRequestBreakClear(request, request_head, response_head);
	} else if (request.find("request://mapget?") == 0) {
		string request_head = "request://mapget?";
		string response_head = "response://mapget?";
		return doRequestMapGet(request, request_head, response_head);
	} else if (request.find("request://threadsget?") == 0) {
		string request_head = "request://threadsget?";
		string response_head = "response://threadsget?";
		return doRequestThreadsGet(request, request_head, response_head);
	} else if (request.find("request://shut?") == 0) {
		string request_head = "request://shut?";
		string response_head = "response://shut?";
		return doRequestShut(request, request_head, response_head);
	} else if (request.find("request://stop?") == 0) {
		string request_head = "request://stop?";
		string response_head = "response://stop?";
		return doRequestStop(request, request_head, response_head);
	} else if (request.find("request://step?") == 0) {
		string request_head = "request://step?";
		string response_head = "response://step?";
		return doRequestStep(request, request_head, response_head);
	} else if (request.find("request://continue?") == 0) {
		string request_head = "request://continue?";
		string response_head = "response://continue?";
		return doRequestContinue(request, request_head, response_head);
	}

	return "response://unknown_command?";
}

int Ptracer::sendEvent(string &event)
{
	int result;
	PTRACER_ASSERT(event.length() != 0);

	struct ptr_msg event_msg_head;
	event_msg_head.magic = INTER_MSG_MAGIC;
	event_msg_head.code = PTCR_EVT_DATA;
	event_msg_head.len = sizeof(struct ptr_msg) + event.length();
	result = send(m_outer_server_sockfd, &event_msg_head, sizeof(struct ptr_msg), 0);
	if (result != sizeof(struct ptr_msg)) {
		PTRACER_LOG("Send failed, errno %d.", errno);
		close(m_outer_server_sockfd);
		m_outer_server_sockfd = -1;
		connClosed();
		return -1;
	}
	result = send(m_outer_server_sockfd, event.c_str(), event.length(), 0);
	if (result != event.length()) {
		PTRACER_LOG("Send failed, errno %d.", errno);
		close(m_outer_server_sockfd);
		m_outer_server_sockfd = -1;
		connClosed();
		return -1;
	}
	return 0;
}

list<Ptracer::ListNode> Ptracer::msgToList(string &content)
{
	list<Ptracer::ListNode> kv_list;

	int pos = 0;
	while(1) {
		string kv_str;
		int find_at = content.find("&", pos);
		if (find_at == string::npos) {
			kv_str = content.substr(pos);
		} else {
			kv_str = content.substr(pos, find_at - pos);
			pos = find_at + 1;
		}

		if (kv_str.length() != 0) {
			int equal_at = kv_str.find("=");
			if (equal_at != string::npos) {
				Ptracer::ListNode node;
				node.key = kv_str.substr(0, equal_at);
				node.value = kv_str.substr(equal_at + 1);
				kv_list.push_back(node);
			}
		}

		if (find_at == string::npos) {
			break;
		}
	}

	return kv_list;
}

map<string,string> Ptracer::msgToMap(string &content)
{
	map<string,string> kv_map;

	int pos = 0;
	while(1) {
		string kv_str;
		int find_at = content.find("&", pos);
		if (find_at == string::npos) {
			kv_str = content.substr(pos);
		} else {
			kv_str = content.substr(pos, find_at - pos);
			pos = find_at + 1;
		}

		if (kv_str.length() != 0) {
			int equal_at = kv_str.find("=");
			if (equal_at != string::npos) {
				string key = kv_str.substr(0, equal_at);
				string value = kv_str.substr(equal_at + 1);
				kv_map.insert(map<string,string>::value_type(key, value));
			}
		}

		if (find_at == string::npos) {
			break;
		}
	}

	return kv_map;
}

string string_replace(string src, string from, string to)
{
	int pos = 0;
	while((pos = src.find(from, pos)) != src.npos) {
		src.replace(pos, from.length(), to);
		pos += to.length();
	}
	return src;
}


string Ptracer::msgEncode(string &input)
{
	string retstring;

	retstring = string_replace(input, string("\\"), string("\\\\"));
	retstring = string_replace(input, string("?"), string("\\?"));
	retstring = string_replace(input, string("="), string("\\="));
	retstring = string_replace(input, string("&"), string("\\&"));

	return retstring;
}

string Ptracer::msgDecode(string &input)
{
	string retstring;

	retstring = string_replace(input, string("\\&"), string("&"));
	retstring = string_replace(input, string("\\="), string("="));
	retstring = string_replace(input, string("\\?"), string("?"));
	retstring = string_replace(input, string("\\\\"), string("\\"));

	return retstring;
}

