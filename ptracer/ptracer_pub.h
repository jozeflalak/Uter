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

#ifndef SRC_PTRACER_PUB_H_
#define SRC_PTRACER_PUB_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <list>
#include <string>
#include <vector>
#include <time.h>
#include <string.h>

using namespace std;

/*Log level：0 no log， 1 log，2 debug*/
extern int ptracer_log_level;

#define PTRACER_DUMP(fmt, ...)  if(ptracer_log_level >= 3) { printf("DUMP:"  fmt "\n", ##__VA_ARGS__); fflush(stdout);}
#define PTRACER_DEBUG(fmt, ...) if(ptracer_log_level >= 2) { printf("DEBUG:" fmt "\n", ##__VA_ARGS__); fflush(stdout);}
#define PTRACER_LOG(fmt, ...)   if(ptracer_log_level >= 1) { printf("LOG:"   fmt "\n", ##__VA_ARGS__); fflush(stdout);}
#define PTRACER_ASSERT(x) if (!(x)) { printf("ASSERT:%s:%d\n", __FILE__, __LINE__); assert(0); }
#define PTRACER_TODO(fmt, ...) do {	printf("!!!!!!!!UTER_TODO,%s:%d:%s:" fmt "!!!!!!!!\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); assert(0); } while(0)

static std::string string_format(const char *fmt,...)
{
#define format_buf_size (1000)
	char buf[format_buf_size];
	va_list valist = NULL;
	va_start(valist, fmt);
	size_t len = vsnprintf(buf, format_buf_size, fmt, valist);
	buf[format_buf_size - 1] = 0;
	va_end(valist);
	return buf;
}

class LinuxThreads {
public:
	class LinuxThread {
	public:
		enum LinuxThreadStatus {
			STATUS_RUNNING = 0,			/*R*/
			STATUS_INTERRUPTIBLE,		/*S*/
			STATUS_UNINTERRUPTIBLE,		/*D*/
			STATUS_STOPPED,				/*t*/
			STATUS_TRACED,				/*T*/
			STATUS_ZOMBIE,				/*Z*/
			STATUS_DEAD,				/*X*/
		};

		LinuxThread()
		{
			m_tid = 0;
			m_stat = STATUS_RUNNING;
			m_name = "";
			return;
		}

		unsigned int m_tid;
		LinuxThreadStatus m_stat;
		string m_name;
	};

	~LinuxThreads()
	{
		list<LinuxThread*>::iterator iter;
		for(iter = m_thread_list.begin(); iter != m_thread_list.end(); /*none*/) {
			LinuxThread *thread = *iter;
			delete thread;
			m_thread_list.erase(iter++);
		}
		return;
	}

	LinuxThread *getThreadInfo(char *thread_path);
	int loadThreadInfo(unsigned int pid);

	list<LinuxThread*> m_thread_list;
};

class LinuxMaps {
public:
	class LinuxMap {
	public:
		LinuxMap()
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
			path = "";
			return;
		}

		unsigned long long m_start;
		unsigned long long m_end;
		bool m_flag_r;		//rooo == true yes; -ooo == false no
		bool m_flag_w;		//owoo == true yes; o-oo == false no
		bool m_flag_x;		//ooxo == true yes; oo-o == false no
		bool m_flag_p;		//ooop == true private; ooos == false share
		unsigned long long m_offset;
		unsigned int m_major;
		unsigned int m_minor;
		unsigned int m_inode;
		string path;
	};

	~LinuxMaps()
	{
		list<LinuxMap*>::iterator iter;
		for(iter = m_map_list.begin(); iter != m_map_list.end(); /*none*/) {
			LinuxMap *mod = *iter;
			delete mod;
			m_map_list.erase(iter++);
		}
		return;
	}

	LinuxMap *getMapInfo(char *line);
	int loadMapInfo(unsigned int pid);

	list<LinuxMap*> m_map_list;
};

/*Get pid from pid file*/
unsigned int linux_pid_from_file(char *path);
/*Get pid from expr*/
unsigned int linux_pid_from_expr(char *expr);

#endif /*SRC_PTRACER_PUB_H_*/

