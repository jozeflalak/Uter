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
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <inter_msg.h>
#include <ptracer_pub.h>

int ptracer_log_level = 0;

/*Get thread information*/
LinuxThreads::LinuxThread *LinuxThreads::getThreadInfo(char *thread_path)
{
	/*Check thread stat*/
	char stat_path[PATH_MAX];
	sprintf(stat_path, "%s/%s", thread_path, "stat");
	int stat_fd = open(stat_path, 0);
	if (stat_fd < 0) {
		return NULL;
	}

	/*Get content from status file*/
	const unsigned int TASK_STAT_BUF = 1024;
	char stat_buf[TASK_STAT_BUF];
	ssize_t sizeresult = read(stat_fd, stat_buf, TASK_STAT_BUF);
	if (sizeresult <= 0) {
		close(stat_fd);
		return NULL;
	}
	close(stat_fd);
	stat_buf[sizeresult] = '\0';

	/*
	 * For example:
	 * 19145 (04_multithreadi) S 8084 19145 8084 34820 19145 1077960704 87 0 0 0 4 1 0
	 * 0 20 0 2 0 76979701 10731520 149 4294967295 134512640 134514832 3214897904 32148
	 * 97208 3078090060 0 0 0 0 3238520631 0 0 17 0 0 0 0 0 0 134520576 134520880 16791
	 * 5520 3214906065 3214906114 3214906114 3214909387 0
	 * */
	/*Name*/
	string str_stat = stat_buf;
	string::size_type findat = str_stat.find(" (");
	if (findat == string::npos) {
		return NULL;
	}
	string str_tid = str_stat.substr(0, findat);
	unsigned int tid = atoi(str_tid.c_str());
	string::size_type name_start = findat + 2;

	findat = str_stat.find(") ");
	if (findat == string::npos) {
		return NULL;
	}
	string str_name = str_stat.substr(name_start, findat - name_start);

	/*Status letter*/
	string str_stw = str_stat.substr(findat + 2, 1);
	char c_stw = str_stw.c_str()[0];
	LinuxThread::LinuxThreadStatus stat;
	switch(c_stw)
	{
	case 'r':
	case 'R':
	{
		stat = LinuxThread::STATUS_RUNNING;
		break;
	}
	case 's':
	case 'S':
	{
		stat = LinuxThread::STATUS_INTERRUPTIBLE;
		break;
	}
	case 'd':
	case 'D':
	{
		stat = LinuxThread::STATUS_UNINTERRUPTIBLE;
		break;
	}
	case 't':
	{
		stat = LinuxThread::STATUS_STOPPED;
		break;
	}
	case 'T':
	{
		stat = LinuxThread::STATUS_TRACED;
		break;
	}
	case 'z':
	case 'Z':
	{
		stat = LinuxThread::STATUS_ZOMBIE;
		break;
	}
	case 'x':
	case 'X':
	{
		stat = LinuxThread::STATUS_DEAD;
		break;
	}
	default:
	{
		return NULL;
		break;
	}
	}

	LinuxThread *thread = new LinuxThread();
	thread->m_tid = tid;
	thread->m_stat = stat;
	thread->m_name = str_name;
	return thread;
}

/*Load thread information on the process by pid*/
int LinuxThreads::loadThreadInfo(unsigned int pid)
{
	int result;

	if (pid == 0) {
		return -1;
	}

	/*Check process dir*/
	char process_path[PATH_MAX];
	sprintf(process_path, "/proc/%d", pid);

	struct stat stbuf;
	result = stat(process_path, &stbuf);
    if (result != 0) {
    	return -1;
    }
    if (!S_ISDIR(stbuf.st_mode)) {
    	return -1;
    }

    /*Check process task dir*/
    char task_path[PATH_MAX];
    sprintf(task_path, "/proc/%d/task", pid);
	result = stat(task_path, &stbuf);
    if (result != 0) {
    	return -1;
    }
    if (!S_ISDIR(stbuf.st_mode)) {
    	return -1;
    }

    /*Foreach process task dir*/
    DIR *dirp = opendir(task_path);
    if (dirp == NULL) {
    	return -1;
    }

    LinuxThreads *return_process = new LinuxThreads();

    struct dirent *dir;
    while((dir = readdir(dirp)) != NULL) {
    	/*Ignore . and .. directory*/
        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {		//特殊的去掉
        	continue;
        }

        /*Check thread task directory*/
		char thread_path[PATH_MAX];
        sprintf(thread_path, "%s/%s", task_path, dir->d_name);
        result = stat(thread_path, &stbuf);
        if (result != 0) {
        	closedir(dirp);
        	delete return_process;
        	return -1;
        }
        if (!S_ISDIR(stbuf.st_mode)) {
        	closedir(dirp);
        	delete return_process;
        	return -1;
        }

        /*Load thread information*/
        LinuxThread *thread = getThreadInfo(thread_path);
        if (thread == NULL) {
        	closedir(dirp);
        	delete return_process;
        	return -1;
        }

        return_process->m_thread_list.push_back(thread);
    }

    closedir(dirp);
    return 0;
}

/*
 * Load a process map entry form a text line.
 * For example:
 * 08048000-08049000 r-xp 00000000 08:03 18484181   /home/yaozhongkai/uter/sub_project/eg_commander/04_multithreading.exe
 * 08049000-0804a000 r--p 00000000 08:03 18484181   /home/yaozhongkai/uter/sub_project/eg_commander/04_multithreading.exe
 * 0804a000-0804b000 rw-p 00001000 08:03 18484181   /home/yaozhongkai/uter/sub_project/eg_commander/04_multithreading.exe
 * 08c0f000-08c30000 rw-p 00000000 00:00 0          [heap]
 * b6d6f000-b6d70000 ---p 00000000 00:00 0
 * b6d70000-b7572000 rw-p 00000000 00:00 0          [stack:4867]
 * b7572000-b771b000 r-xp 00000000 08:01 4326393    /lib/i386-linux-gnu/libc-2.19.so
 * b771b000-b771d000 r--p 001a9000 08:01 4326393    /lib/i386-linux-gnu/libc-2.19.so
 * b771d000-b771e000 rw-p 001ab000 08:01 4326393    /lib/i386-linux-gnu/libc-2.19.so
 * b771e000-b7721000 rw-p 00000000 00:00 0
 * b7721000-b7739000 r-xp 00000000 08:01 4326513    /lib/i386-linux-gnu/libpthread-2.19.so
 * b7739000-b773a000 r--p 00018000 08:01 4326513    /lib/i386-linux-gnu/libpthread-2.19.so
 * b773a000-b773b000 rw-p 00019000 08:01 4326513    /lib/i386-linux-gnu/libpthread-2.19.so
 * b773b000-b773d000 rw-p 00000000 00:00 0
 * b7751000-b7754000 rw-p 00000000 00:00 0
 * b7754000-b7755000 r-xp 00000000 00:00 0          [vdso]
 * b7755000-b7757000 r--p 00000000 00:00 0          [vvar]
 * b7757000-b7777000 r-xp 00000000 08:01 4326369    /lib/i386-linux-gnu/ld-2.19.so
 * b7777000-b7778000 r--p 0001f000 08:01 4326369    /lib/i386-linux-gnu/ld-2.19.so
 * b7778000-b7779000 rw-p 00020000 08:01 4326369    /lib/i386-linux-gnu/ld-2.19.so
 * bfd58000-bfd79000 rw-p 00000000 00:00 0          [stack]
 */
LinuxMaps::LinuxMap *LinuxMaps::getMapInfo(char *line)
{
	if (line == NULL) {
		return NULL;
	}

	LinuxMap *return_mod = new LinuxMap();

	string strline = line;
	string::size_type point = 0;
	string::size_type find_at;
	string stub_str;

	//b771d000-b771e000 rw-p 001ab000 08:01 4326393    /lib/i386-linux-gnu/libc-2.19.so
	find_at = strline.find("-", point);
	if (find_at == string::npos) {
		delete return_mod;
		return NULL;
	}
	stub_str = strline.substr(point, find_at - point);
	point = find_at + 1;
	return_mod->m_start = strtoull(stub_str.c_str(), NULL, 16);

	//b771e000 rw-p 001ab000 08:01 4326393    /lib/i386-linux-gnu/libc-2.19.so
	find_at = strline.find(" ", point);
	if (find_at == string::npos) {
		delete return_mod;
		return NULL;
	}
	stub_str = strline.substr(point, find_at - point);
	point = find_at + 1;
	return_mod->m_end = strtoull(stub_str.c_str(), NULL, 16);

	//rw-p 001ab000 08:01 4326393    /lib/i386-linux-gnu/libc-2.19.so
	stub_str = strline.substr(point, 1);
	point++;
	if (stub_str == "r") {
		return_mod->m_flag_r = true;
	} else if (stub_str == "-") {
		return_mod->m_flag_r = true;
	} else {
		delete return_mod;
		return NULL;
	}

	//w-p 001ab000 08:01 4326393    /lib/i386-linux-gnu/libc-2.19.so
	stub_str = strline.substr(point, 1);
	point++;
	if (stub_str == "w") {
		return_mod->m_flag_w = true;
	} else if (stub_str == "-") {
		return_mod->m_flag_w = true;
	} else {
		delete return_mod;
		return NULL;
	}

	//-p 001ab000 08:01 4326393    /lib/i386-linux-gnu/libc-2.19.so
	stub_str = strline.substr(point, 1);
	point++;
	if (stub_str == "x") {
		return_mod->m_flag_x = true;
	} else if (stub_str == "-") {
		return_mod->m_flag_x = true;
	} else {
		delete return_mod;
		return NULL;
	}

	//p 001ab000 08:01 4326393    /lib/i386-linux-gnu/libc-2.19.so
	stub_str = strline.substr(point, 1);
	point++;
	if (stub_str == "p") {
		return_mod->m_flag_p = true;
	} else if (stub_str == "-") {
		return_mod->m_flag_p = true;
	} else {
		delete return_mod;
		return NULL;
	}

	/*space*/
	point++;

	//001ab000 08:01 4326393    /lib/i386-linux-gnu/libc-2.19.so
	find_at = strline.find(" ", point);
	if (find_at == string::npos) {
		delete return_mod;
		return NULL;
	}
	stub_str = strline.substr(point, find_at - point);
	point = find_at + 1;
	return_mod->m_offset = strtoull(stub_str.c_str(), NULL, 16);

	//08:01 4326393    /lib/i386-linux-gnu/libc-2.19.so
	find_at = strline.find(":", point);
	if (find_at == string::npos) {
		delete return_mod;
		return NULL;
	}
	stub_str = strline.substr(point, find_at - point);
	point = find_at + 1;
	return_mod->m_major = atoi(stub_str.c_str());

	//01 4326393    /lib/i386-linux-gnu/libc-2.19.so
	find_at = strline.find(" ", point);
	if (find_at == string::npos) {
		delete return_mod;
		return NULL;
	}
	stub_str = strline.substr(point, find_at - point);
	point = find_at + 1;
	return_mod->m_minor = atoi(stub_str.c_str());

	//0
	//0          [stack:4867]
	//4326393    /lib/i386-linux-gnu/libc-2.19.so
	find_at = strline.find(" ", point);
	if (find_at == string::npos) {
		delete return_mod;
		return NULL;
	}
	stub_str = strline.substr(point, find_at - point);
	point = find_at + 1;
	return_mod->m_inode = atoi(stub_str.c_str());

	//\n
	//         [stack:4867]\n
	//    /lib/i386-linux-gnu/libc-2.19.so\n
	find_at = strline.find_first_not_of(" ", point);
	if (find_at == string::npos) {
		delete return_mod;
		return NULL;
	}
	stub_str = strline.substr(point, find_at - point);
	point = find_at;
	if (stub_str == "\n") {
		return return_mod;
	}

	find_at = strline.find("\n", point);
	if (find_at == string::npos) {
		delete return_mod;
		return NULL;
	}
	stub_str = strline.substr(point, find_at - point);
	return_mod->path = stub_str;
	return return_mod;
}

/*Load process map from the process by pid*/
int LinuxMaps::loadMapInfo(unsigned int pid)
{
	int result;

	if (pid == 0) {
		return -1;
	}

	/*Check process dir*/
	char process_path[PATH_MAX];
	sprintf(process_path, "/proc/%d", pid);

	struct stat stbuf;
	result = stat(process_path, &stbuf);
    if (result != 0) {
    	return -1;
    }
    if (!S_ISDIR(stbuf.st_mode)) {
    	return -1;
    }

    /*Scan map file*/
    char map_path[PATH_MAX];
    sprintf(map_path, "/proc/%d/maps", pid);
    FILE *file_read = fopen(map_path, "r");
    if (file_read == NULL) {
    	return -1;
    }

	/*Foreach map file line*/
	char read_buf[PATH_MAX * 2];
	while(1) {
		char *strresult = fgets(read_buf, sizeof(read_buf), file_read);
		if (strresult == NULL) {
			if (feof(file_read)) {
				fclose(file_read);
				break;
			} else {
				/*Some mistake*/
				fclose(file_read);
				return -1;
			}
		}

		/*Parse map line*/
		LinuxMap *mod = getMapInfo(strresult);
		if (mod == NULL) {
			fclose(file_read);
			return -1;
		}

		m_map_list.push_back(mod);
	}

	return 0;
}

unsigned int linux_pid_from_file(char *path)
{
	if (path == NULL) {
		return 0;
	}

    FILE *file_read = fopen(path, "r");
    if (file_read == NULL) {
    	return 0;
    }

    const unsigned int PID_BUF = 1024;
    char read_buf[PID_BUF] = {0};
	char *strresult = fgets(read_buf, sizeof(read_buf) - 1, file_read);
	if (strresult == NULL) {
		fclose(file_read);
		return 0;
	}
	fclose(file_read);

	unsigned int pid = strtouq(strresult, NULL, 10);
	return pid;
}

unsigned int linux_pid_from_expr(char *expr)
{
	int result;

	if (expr == NULL) {
		return 0;
	}

	const unsigned int RESULT_BUF = 1024;
	char buffer[RESULT_BUF] = {0};
	/*popen can not timeout*/
	FILE *fp = popen(expr, "r");
	if (fp == NULL) {
		return 0;
	}

	unsigned int ret_pid = 0;
	while(1) {
		char *strresult = fgets(buffer, sizeof(buffer) - 1, fp);
		if (strresult == NULL) {
			break;
		}

		if (ret_pid != 0) {
			ret_pid = 0;
			break;
		} else {
			ret_pid = strtouq(strresult, NULL, 10);
		}
	}
	pclose(fp);

	return ret_pid;
}
