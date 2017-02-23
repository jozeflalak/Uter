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

#ifndef SRC_UTER_SIGNAL_ARG_H_
#define SRC_UTER_SIGNAL_ARG_H_

class TargetThreadsArg {
public:
	class TargetThreadArg {
	public:
		TargetThreadArg()
		{
			m_pid = 0;
			m_name = "";
			m_in_vdso = false;
			return;
		}
		unsigned int m_pid;
		QString m_name;
		bool m_in_vdso;
	};
	~TargetThreadsArg()
	{
		QList<TargetThreadArg*>::iterator iter;
		for (iter = m_thread_list.begin(); iter != m_thread_list.end(); /*none*/) {
			TargetThreadArg *thread = *iter;
			delete thread;
			m_thread_list.erase(iter++);
		}
		return;
	}

	bool m_success;
	QList<TargetThreadArg*> m_thread_list;
};

/*Get memory from target (block request)*/
class TargetMemGetArg {
public:
	TargetMemGetArg()
	{
		m_addr = 0;
		m_size = 0;
		m_success = false;
	}
	TargetMemGetArg(unsigned long long addr, unsigned int size)
	{
		m_addr = addr;
		m_size = size;
		m_success = false;
		return;
	}
	unsigned long long m_addr;
	unsigned int m_size;
	bool m_success;
	QByteArray m_array;
};

/*Set memory to target (block request)*/
class TargetMemSetArg {
public:
	TargetMemSetArg()
	{
		m_addr = 0;
		m_size = 0;
		m_success = false;
	}
	TargetMemSetArg(unsigned long long addr, unsigned int size)
	{
		m_addr = addr;
		m_size = size;
		m_success = false;
		return;
	}
	unsigned long long m_addr;
	unsigned int m_size;
	QByteArray m_array;
	bool m_success;
};

/*Get register from target (block request)*/
class TargetRegGetArg {
public:
	TargetRegGetArg()
	{
		m_pid = 0;
		m_name = "";
		m_success = false;
		m_value = 0;
		return;
	}
	TargetRegGetArg(unsigned int pid, QString name)
	{
		m_pid = pid;
		m_name = name;
		m_success = false;
		m_value = 0;
		return;
	}
	unsigned int m_pid;
	QString m_name;
	bool m_success;
	unsigned long long m_value;
};

/*Set register to target (block request)*/
class TargetRegSetArg {
public:
	TargetRegSetArg()
	{
		m_pid = 0;
		m_name = "";
		m_success = false;
		m_value = 0;
		return;
	}
	TargetRegSetArg(unsigned int pid, QString name)
	{
		m_pid = pid;
		m_name = name;
		m_success = false;
		m_value = 0;
		return;
	}
	unsigned int m_pid;
	QString m_name;
	unsigned long long m_value;
	bool m_success;
};

/*Function coverage interface*/
class TargetFunCovArg {
public:
	TargetFunCovArg()
	{
		m_mod = "";
		m_cu = "";
		m_fun = "";
		m_opt = "";

		m_err_str = "";
		m_count_hit = 0;
		m_count_line = 0;
		return;
	}
	QString m_mod;
	QString m_cu;
	QString m_fun;
	QString m_opt;
	QString m_err_str;

	QString m_show_str;
	unsigned int m_count_hit;
	unsigned int m_count_line;
};

#endif /*SRC_UTER_SIGNAL_ARG_H_*/
