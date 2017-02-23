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

#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <QtNetwork/QHostAddress>
#include <QtCore/QUuid>

#include <uter_public.h>
#include <inter_msg.h>
#include <target.hxx>
#include <tester.hxx>

class ModDesc {
public:
	ModDesc()
	{
		m_path = "";
		m_inode = 0;
		m_start = 0xffffffffffffffff;
		m_stop = 0;
		return;
	}
	QString m_path;
	unsigned int m_inode;
	unsigned long long m_start;
	unsigned long long m_stop;
	DwfAddrRange m_add_range;
};

/*Load everything for module*/
int TargetMod::load()
{
	int result;

	/*
	 * Module vdso is a especial module, simulate by kernel, so it is not in file system.
	 * Support unified interface for system call. vdso module not have dwarf debug information,
	 * but have some ef frame information. Get vdso module by memory read.
	 * */
	if (m_remote_path == "[vdso]") {
		/*Read vdso module one-time*/
		QByteArray array;
		result = g_target->memoryGet(m_addr_range.m_begin, m_addr_range.m_offset, array);
		if (result < 0) {
			UTER_LOG("Read vdso module memory failed.");
			return -1;
		}

		/*Random path for VDSO module on local*/
		m_local_path = QDir::tempPath() + "/uter_vdso_" + QUuid::createUuid().toString() + ".so";
		UTER_DEBUG("Copy vdso to local file %s.", m_local_path.toStdString().c_str());

		QFile file(m_local_path);
		if (! file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
			UTER_LOG("Create vdso module local file %s failed.", m_local_path.toStdString().c_str());
			m_local_path = "";
			return -1;
		}
		file.write(array);
		file.close();
	} else {
		/*TODO: need copy remote file to local, or map local to remote*/
		m_local_path = m_remote_path;
	}

	m_name = strdup(m_remote_path.toStdString().c_str());
	if (m_name == NULL) {
		/*For debug easy*/
		UTER_ASSERT(0);
		goto error_return;
	}

	UTER_DEBUG("Load bfd information for module %s", m_name);
	result = bfdLoad();
	if (result < 0) {
		UTER_LOG("Load bfd information of module %s failed.", m_name);
		goto error_return;
	}

	UTER_DEBUG("Load dwarf information for module %s", m_name);
	result = dwarfLoad();
	if (result < 0) {
		UTER_LOG("Load debug information of module %s failed.", m_name);
		goto error_return;
	}

	UTER_DEBUG("Setup coverage information for module %s", m_name);
	result = CovTokenSet::setTokenSet(this);
	if (result < 0) {
		UTER_LOG("Set coverage information of module %s failed.", m_name);
		goto error_return;
	}

	return 0;

error_return:
	release();
	return -1;
}

int TargetMod::release()
{
	dwarfRelease();

	bfdRelease();

	if (m_remote_path == "[vdso]") {
		QFile::remove(m_local_path);
	}

	if (m_name == NULL) {
		free(m_name);
		m_name = NULL;
	}

	return 0;
}

Target::Target()
:m_data_lock(QMutex::Recursive)
{
	QMetaObject::Connection conn;

	m_status = STATUS_DOWN;

	m_run_pid = 0;
	m_run_flag_starting = false;
	m_run_flag_stopping = false;
	m_run_flag_breaking = false;

	m_malloc_addr = 0;
	m_free_addr = 0;

	m_ptracer_socket = NULL;
	m_ptracer_receiving = false;

    conn = connect(this, SIGNAL(signalPtracerEvent()), this, SLOT(slotPtracerEvent()), Qt::QueuedConnection);
    UTER_ASSERT(conn != NULL);
    conn = connect(this, SIGNAL(signalTryReadyRead()), this, SLOT(slotConReady()), Qt::QueuedConnection);
    UTER_ASSERT(conn != NULL);
	return;
}

void Target::slotThreads(TargetThreadsArg *arg)
{
	UTER_ASSERT(arg != NULL);
	arg->m_success = false;

	QMap<unsigned int, TargetThread*>::iterator thread_iter;
	for (thread_iter = m_thread_set.begin(); thread_iter != m_thread_set.end(); thread_iter++) {
		TargetThread *thread = thread_iter.value();

		unsigned long long instrat;
		int result = pidRegPcGet(thread->m_pid, instrat);
		if (result < 0) {
			UTER_LOG("Get current instructions of thread %d failed.", thread->m_pid);
			return;
		}

		/*Check pc in vdso*/
		TargetMod *mod = modGetByAddrIn(instrat);
		if (mod == NULL) {
			UTER_LOG("Get mod of current instructions 0x%016llx failed.", instrat);
			return;
		}

		TargetThreadsArg::TargetThreadArg *info = new TargetThreadsArg::TargetThreadArg();
		info->m_pid = thread->m_pid;
		info->m_name = thread->m_name;
		info->m_in_vdso = (mod->m_remote_path == "[vdso]");

		arg->m_thread_list.push_back(info);
	}

	arg->m_success = true;
	return;
}

void Target::slotRegGet(TargetRegGetArg *arg)
{
	UTER_ASSERT(arg != NULL);
	UTER_DUMP("Do register get request, thread %d name %s.",
			    arg->m_pid, arg->m_name.toStdString().c_str());
	int result = pidRegGet(arg->m_pid, arg->m_name, arg->m_value);
	if (result < 0) {
		arg->m_success = false;
	} else {
		arg->m_success = true;
	}
	return;
}

void Target::slotRegSet(TargetRegSetArg *arg)
{
	UTER_ASSERT(arg != NULL);
	UTER_DUMP("Do register set request, thread %d name %s value 0x%016llx.",
			    arg->m_pid, arg->m_name.toStdString().c_str(), arg->m_value);
	int result = pidRegSet(arg->m_pid, arg->m_name, arg->m_value);
	if (result < 0) {
		arg->m_success = false;
	} else {
		arg->m_success = true;
	}
	return;
}

void Target::slotMemGet(TargetMemGetArg *arg)
{
	UTER_ASSERT(arg != NULL);
	UTER_DUMP("Do memory get request, address 0x%016llx, length %d.",
				arg->m_addr, arg->m_size);
	int result = memoryGet(arg->m_addr, arg->m_size, arg->m_array);
	if (result < 0) {
		arg->m_success = false;
	} else {
		arg->m_success = true;
	}
	return;
}

void Target::slotMemSet(TargetMemSetArg *arg)
{
	UTER_ASSERT(arg != NULL);
	UTER_ASSERT(arg->m_size == arg->m_array.size());
	UTER_DUMP("Do memory set request, address 0x%016llx, length %d.", arg->m_addr, arg->m_size);
	int result = memorySet(arg->m_addr, arg->m_size, arg->m_array);
	if (result < 0) {
		arg->m_success = false;
	} else {
		arg->m_success = true;
	}
	return;
}

/*Function coverage operate request from tester*/
void Target::slotFunCovSet(TargetFunCovArg *arg)
{
	UTER_ASSERT(arg != NULL);
	TargetMod *mod = g_target->modGetByPath(arg->m_mod);
	if (mod == NULL) {
		arg->m_err_str = QString("No module named ") + arg->m_mod;
		return;
	}
	DwfCu *cu = mod->dwarfGetCuByPath(arg->m_cu);
	if (cu == NULL) {
		arg->m_err_str = QString("No cu named ") + arg->m_cu;
		return;
	}
	QList<DwfFun*>::iterator iter;
	for (iter = cu->m_funs.begin(); iter != cu->m_funs.end(); iter++) {
		DwfFun *fun = *iter;
		if (fun->m_name == arg->m_fun) {
			break;
		}
	}
	if (iter == cu->m_funs.end()) {
		arg->m_err_str = QString("No function named ") + arg->m_fun;
		return;
	}
	DwfFun *fun = *iter;

	if (arg->m_opt == "on") {
		cu->funCovOn(fun);
	} else if (arg->m_opt == "off") {
		cu->funCovOff(fun);
	} else if (arg->m_opt == "reset") {
		cu->funCovReset(fun);
	} else if (arg->m_opt == "show") {
		cu->funCovShow(fun, arg->m_show_str);
	} else if (arg->m_opt == "count") {
		cu->funCovCount(fun, arg->m_count_hit, arg->m_count_line);
	} else {
		arg->m_err_str = "Operate error.";
		return;
	}
	return;
}

void Target::slotContinue(unsigned int)
{
	ctrlContinue();
	return;
}

void Target::slotConError(QAbstractSocket::SocketError socketError)
{
	UTER_LOG("Connection error, description %s.",
				m_ptracer_socket->errorString().toStdString().c_str());
	m_ptracer_socket->close();
	return;
}

void Target::slotConDiscon()
{
	if (m_ptracer_socket == NULL) {
		UTER_ASSERT(0);
		return;
	}

	UTER_DEBUG("Connection closed.");
	/*delete message*/
	m_ptracer_receiving = false;
	m_ptracer_recv.clear();
	m_ptracer_events.clear();
	/*delete socket*/
	m_ptracer_socket->deleteLater();
	m_ptracer_socket = NULL;

	if (m_status >= STATUS_RUNNING) {
		/*Same as end all thread*/
		threadClear();
	}

	if (m_status >= STATUS_CONNECTED) {
		statSet(STATUS_DOWN);
		UTER_PRINT("Disconnect from target.");
		UTER_STBAR(2000, "Disconnect from target.");
	}

	return;
}

/*Read event from ptracer*/
void Target::slotConReady()
{
	if (m_ptracer_socket == NULL) {
		//UTER_ASSERT(0);
		return;
	}

	if (m_ptracer_receiving) {
		/*Ignore when requesting*/
		return;
	}

	QByteArray readbytes = m_ptracer_socket->readAll();
	if (readbytes.length() != 0) {
		m_ptracer_recv += readbytes;
	}

	while(1) {
		if (m_ptracer_recv.length() < sizeof(ptr_msg)) {
			break;
		}
		ptr_msg *msg = (ptr_msg *)m_ptracer_recv.constData();
		if (m_ptracer_recv.length() < msg->len) {
			break;
		}
		if (msg->magic != INTER_MSG_MAGIC) {
			UTER_LOG("Receive message magic error, disconnect from ptracer.");
			m_ptracer_socket->close();
			break;
		}

		/*Cut message from buffer*/
		QByteArray msg_array = m_ptracer_recv.left(msg->len);
		m_ptracer_recv.remove(0, msg->len);
		msg = (ptr_msg *)msg_array.constData();
		if (msg->code == PTCR_EVT_DATA) {
			/*Event message*/
			m_ptracer_events.push_back(msg_array);
			emit signalPtracerEvent();
			UTER_DUMP("Receive event message from ptracer without request, code 0x%08x, length %d.", msg->code, msg->len);
		} else {
			/*Ignore other message*/
			UTER_LOG("Ignore message with code 0x%08x.", msg->code);
		}
	}
	return;
}

void Target::slotPtracerEvent()
{
	while(! m_ptracer_events.empty()) {
		QByteArray msg_array = m_ptracer_events.front();
		m_ptracer_events.pop_front();

		ptr_msg *msg = (ptr_msg *)msg_array.constData();
		UTER_DUMP("Target receive ptracer event message, code %d, length %d.", msg->code, msg->len);

		if (msg->code == PTCR_EVT_DATA) {
			QString event(QLatin1String((char *)(msg + 1), (msg->len - sizeof(struct ptr_msg))));
			UTER_DUMP("Event message: %s.", event.toStdString().c_str());
			doEvent(event);
		} else {
			UTER_LOG("Ignore message with code 0x%08x.", msg->code);
		}
	}

	return;
}

QString Target::ptracerReq(QString &request)
{
	if (m_ptracer_socket == NULL) {
		/*Something wrong*/
		return "";
	}

	/*Send request*/
	struct ptr_msg msg;
	msg.magic = INTER_MSG_MAGIC;
	msg.code = PTCR_REQ_DATA;
	msg.len = sizeof(ptr_msg) + request.length();
	QByteArray writebytes((const char *)&msg, sizeof(ptr_msg));
	writebytes += request.toLatin1();

	int send_off = 0;
	while(send_off < msg.len) {
		qint64 send_bytes = m_ptracer_socket->write(writebytes.constData() + send_off, msg.len - send_off);
		if (send_bytes < 0) {
			UTER_LOG("Send message to ptracer failed, write failed.");
			m_ptracer_socket->close();
			return "";
		}
		if (! m_ptracer_socket->waitForBytesWritten(-1)) {
			UTER_LOG("Send message to ptracer failed, wait failed.");
			m_ptracer_socket->close();
			return "";
		}
		send_off += send_bytes;
	}

	/*Receive response*/
	while(1) {
		m_ptracer_receiving = true;
		bool bresult = m_ptracer_socket->waitForReadyRead(-1);
		m_ptracer_receiving = false;
		if (! bresult) {
			UTER_LOG("Receive message from ptracer failed, wait failed.");
			m_ptracer_socket->close();
			return "";
		}
		QByteArray readbytes = m_ptracer_socket->readAll();
		if (readbytes.length() == 0) {
			UTER_LOG("Receive message from ptracer failed, read failed.");
			m_ptracer_socket->close();
			return "";
		}
		m_ptracer_recv += readbytes;

		while(1) {
			if (m_ptracer_recv.length() < sizeof(ptr_msg)) {
				break;
			}
			ptr_msg *msg = (ptr_msg *)m_ptracer_recv.constData();
			if (m_ptracer_recv.length() < msg->len) {
				break;
			}
			if (msg->magic != INTER_MSG_MAGIC) {
				UTER_LOG("Receive message magic error, disconnect from ptracer.");
				m_ptracer_socket->close();
				return "";
			}

			/*Cut message from buffer*/
			QByteArray msg_array = m_ptracer_recv.left(msg->len);
			m_ptracer_recv.remove(0, msg->len);
			msg = (ptr_msg *)msg_array.constData();
			if (msg->code == PTCR_EVT_DATA) {
				/*Event message*/
				m_ptracer_events.push_back(msg_array);
				emit signalPtracerEvent();
				UTER_DUMP("Receive event message from ptracer when request, code 0x%08x, length %d.", msg->code, msg->len);
			} else if (msg->code == PTCR_RESP_DATA) {
				emit signalTryReadyRead();
				/*Response message*/
				return QString::fromLatin1((char *)(msg + 1), msg->len - sizeof(ptr_msg));
			} else {
				/*Ignore other message*/
				UTER_LOG("Ignore message with code 0x%08x.", msg->code);
			}
		}
	}

	return "";
}

int Target::ctrlConnect()
{
	QMetaObject::Connection conn;
	if (m_status != STATUS_DOWN) {
		UTER_LOG("Can not connect target on status %s.", m_status_desc[m_status]);
		return -1;
	}
	UTER_ASSERT(m_ptracer_socket == NULL);

	m_ptracer_socket = new QTcpSocket(this);
	m_ptracer_socket->connectToHost(QHostAddress(UTER_LOCAL_ADDR), UTER_LOCAL_PORT);
    /*TODO:*/
	UTER_DEBUG("Connect to x.x.x.x:xxx.");

	bool bresult = m_ptracer_socket->waitForConnected(-1);
	if (! bresult) {
		UTER_LOG("Connect to ptracer failed, error:%s.", m_ptracer_socket->errorString().toStdString().c_str());
		m_ptracer_socket->close();
		delete m_ptracer_socket;
    	UTER_PRINT("Connect to target failed.");
    	UTER_STBAR(2000, "Connect to target failed.");
		return -1;
	}

    conn = connect(m_ptracer_socket, SIGNAL(readyRead()), this, SLOT(slotConReady()));
    UTER_ASSERT(conn != NULL);
    conn = connect(m_ptracer_socket, SIGNAL(disconnected()), this, SLOT(slotConDiscon()));
    UTER_ASSERT(conn != NULL);
    conn = connect(m_ptracer_socket, SIGNAL(error(QAbstractSocket::SocketError)),
    		       this, SLOT(slotConError(QAbstractSocket::SocketError)));
    UTER_ASSERT(conn != NULL);

    int result = runInit();
    if (result < 0) {
    	/*Disconnect when initialize failed.*/
    	m_ptracer_socket->close();
    	UTER_PRINT("Initialize target failed.");
    	UTER_STBAR(2000, "Initialize target failed.");
    	return -1;
    }

	statSet(STATUS_CONNECTED);
	UTER_PRINT("Connect to target.");
	UTER_STBAR(2000, "Connect to target.");
	return 0;
}

int Target::ctrlDisconn()
{
	if (m_status != STATUS_CONNECTED) {
		UTER_LOG("Can not disconnect target on status %s.", m_status_desc[m_status]);
		return -1;
	}

	UTER_DEBUG("Disconnect from xxxx.");

	m_ptracer_socket->close();
	return 0;
}

int Target::ctrlStartNew(QString path, QList<QString> arglist, QList<QStringList> envlist)
{
	if (m_status != STATUS_CONNECTED) {
		UTER_LOG("Can not start target on status %s.", m_status_desc[m_status]);
		return -1;
	}

	QString debug_str = "";
	QString request_head = "request://startnew?";
	QString response_head = "response://startnew?";
	QString request = request_head + "&path=" + msgEncode(path);
	debug_str += "Start new process with path: " + path + ", arguments:";
	QList<QString>::iterator arg_iter;
	for (arg_iter = arglist.begin(); arg_iter != arglist.end(); arg_iter++) {
		QString arg = *arg_iter;
		request += "&arg=" + msgEncode(arg);

		if (arg_iter != arglist.begin()) {
			debug_str += " ";
		}
		debug_str += arg;
	}
	debug_str += ", environment:";
	QList<QStringList>::iterator env_iter;
	for (env_iter = envlist.begin(); env_iter != envlist.end(); env_iter++) {
		QStringList env = *env_iter;
		request += "&env=" + msgEncode(env.first() + "=" + env.last());

		if (arg_iter != arglist.begin()) {
			debug_str += " ";
		}
		debug_str += env.first() + "=" + env.last();
	}
	UTER_DEBUG("%s.", debug_str.toStdString().c_str());
	QString response = ptracerReq(request);
	int findat = response.indexOf(response_head);
	if (findat != 0) {
		UTER_LOG("Start target failed, request error.");
		return -1;
	}
	QStringRef response_content = response.midRef(response_head.length());
	QMap<QString,QString> response_map = msgToMap(response_content);
	QMap<QString,QString>::iterator key_iter;
	QString key_value;
	key_iter = response_map.find("result");
	if (key_iter == response_map.end()) {
		UTER_LOG("Start target failed, response error.");
		return -1;
	}
	key_value = key_iter.value();
	if (key_value != "success") {
		UTER_LOG("Start target failed, start error.");
		return -1;
	}
	key_iter = response_map.find("pid");
	if (key_iter == response_map.end()) {
		UTER_LOG("Start target failed, no pid.");
		return -1;
	}
	key_value = key_iter.value();

	m_run_flag_starting = true;
	m_run_pid = key_value.toUInt();
	statSet(STATUS_RUNNING);
	return 0;
}

int Target::ctrlShut()
{
	UTER_DEBUG("Do shutdown.");
	if (m_status != STATUS_RUNNING) {
		UTER_LOG("Can not shutdown target on status %s.", m_status_desc[m_status]);
		return -1;
	}

	QString request_head = "request://shut?";
	QString response_head = "response://shut?";
	QString request = request_head + QString::asprintf("&pid=%d", 0);
	QString response = ptracerReq(request);
	int findat = response.indexOf(response_head);
	if (findat != 0) {
		UTER_LOG("Shutdown target failed, request error.");
		return -1;
	}
	QStringRef response_content = response.midRef(response_head.length());
	QMap<QString,QString> response_map = msgToMap(response_content);
	QMap<QString,QString>::iterator key_iter;
	QString key_value;
	key_iter = response_map.find("result");
	if (key_iter == response_map.end()) {
		UTER_LOG("Shutdown target failed, response error.");
		return -1;
	}
	key_value = key_iter.value();
	if (key_value != "success") {
		UTER_LOG("Shutdown target failed, do shutdown error.");
		return -1;
	}

	return 0;
}

int Target::ctrlStop()
{
	UTER_DEBUG("Do stop.");
	if (m_status != STATUS_RUNNING) {
		UTER_LOG("Can not stop target on status %s.", m_status_desc[m_status]);
		return -1;
	}

	if (threadsStatInRange(TargetThread::STATUSSET_NOTRUN)) {
		UTER_LOG("Can not stop target, all thread stoped already.");
		return -1;
	}

	if (m_run_flag_stopping) {
		UTER_LOG("Can not stop target, all thread stopping now.");
		return -1;
	}

	/*broadcast stop and flag stopping*/
	m_run_flag_stopping = true;
	runStop(0);
	return 0;
}

int Target::ctrlContinue()
{
	UTER_DEBUG("Do continue.");
	if (m_status != STATUS_RUNNING) {
		UTER_LOG("Can not continue target on status %s.", m_status_desc[m_status]);
		return -1;
	}

	if (! threadsStatInRange(TargetThread::STATUSSET_NOTRUN)) {
		UTER_LOG("Can not continue target, some thread running.");
		return -1;
	}

	/*Continue all stopped thread*/
	QMap<unsigned int, TargetThread*>::iterator iter;
	for (iter = m_thread_set.begin(); iter != m_thread_set.end(); iter++) {
		TargetThread *thread = iter.value();
		if (thread->m_stat & TargetThread::STATUSSET_STOP) {
			thread->m_stat = TargetThread::STATUS_CONTINUE;
		}
	}

	doGoOnProc();
	return 0;
}

int Target::ctrlStep(unsigned int pid)
{
	/*Step specific thread, continue other thread*/
	QMap<unsigned int, TargetThread*>::iterator iter;
	for (iter = m_thread_set.begin(); iter != m_thread_set.end(); iter++) {
		TargetThread *thread = iter.value();
		if (thread->m_pid == pid) {
			thread->m_stat = TargetThread::STATUS_STEP;
			continue;
		}
		if (thread->m_stat & TargetThread::STATUSSET_STOP) {
			thread->m_stat = TargetThread::STATUS_CONTINUE;
		}
	}

	doGoOnProc();
	return 0;
}

int Target::ctrlStep(unsigned int pid, StepInfo::StepType type)
{
	UTER_DEBUG("Do step.");
	if (m_status != STATUS_RUNNING) {
		UTER_LOG("Can not step target on status %s.", m_status_desc[m_status]);
		return -1;
	}
	if (! threadsStatInRange(TargetThread::STATUSSET_NOTRUN)) {
		UTER_LOG("Can not step target, some thread running.");
		return -1;
	}
	if (type == StepInfo::STEP_MAX) {
		/*Pure asm step*/
		ctrlStep(pid);
		return 0;
	}

	/*Check stepping already*/
	QMap<unsigned int, StepInfo*>::iterator step_iter;
	step_iter = m_step_set.find(pid);
	if (step_iter != m_step_set.end()) {
		UTER_LOG("Thread %d stepping now.", pid);
		ctrlStep(pid);
		return -1;
	}

	Arch *arch = g_target->archInfoGet();
	if (arch == NULL) {
		UTER_LOG("Get architecture specification failed.");
		ctrlStep(pid);
		return -1;
	}

	/*current instruction at*/
	unsigned long long instr_at;
	int result = pidRegPcGet(pid, instr_at);
	if (result != 0) {
		UTER_LOG("Get current instruction address of thread %d failed.", pid);
		ctrlStep(pid);
		return -1;
	}

	TargetMod *mod = g_target->modGetByAddrIn(instr_at);
	if (mod == NULL) {
		UTER_LOG("Get module of address 0x%016llx failed.", instr_at);
		ctrlStep(pid);
		return -1;
	}
	unsigned long long offset_addr = instr_at - mod->m_add_mkup;
	DwfCu *cu = mod->dwarfGetCuByAddrIn(offset_addr);
	if (cu == NULL) {
		UTER_LOG("Get compilation unit of address 0x%016llx failed.", instr_at);
		ctrlStep(pid);
		return -1;
	}
	unsigned int fileno;
	unsigned int lineno;
	result = cu->getFileLineNoByAddrIn(offset_addr, fileno, lineno);
	if (result != 0) {
		UTER_LOG("Get file line of address 0x%016llx failed.", instr_at);
		ctrlStep(pid);
		return -1;
	}

	/*Make step control block*/
	StepInfo *step = new StepInfo();
	step->m_pid = pid;
	step->m_type = type;
	step->m_step_from = instr_at;
	m_step_set.insert(pid, step);

	if (step->m_type == StepInfo::STEP_RETURN) {
		/* Implement step return, set break point at return address which current function return to
		 * When breakpoint triggered, asm step by step to next code line. so get return address first.
		 * If something wrong, convert to step over*/
		unsigned long long offset_addr = instr_at - mod->m_add_mkup;
		DwfFrameFde *fde = mod->dwarfGetFdeByAddrIn(offset_addr);
		if (fde == NULL) {
			UTER_LOG("Get fde failed.");
			step->m_type = StepInfo::STEP_OVER;
			ctrlStep(pid);
			return -1;
		}
		DwfRegTable *regtable = fde->getRegTabByAddrIn(offset_addr);
		if (regtable == NULL) {
			UTER_LOG("Get register table failed.");
			step->m_type = StepInfo::STEP_OVER;
			ctrlStep(pid);
			return -1;
		}
		DwfRegTable::Item *entry = regtable->getItemById(DwfLoc::REG_CFA_VAL);
		if (entry == NULL) {
			UTER_LOG("Get register on table failed.");
			step->m_type = StepInfo::STEP_OVER;
			ctrlStep(pid);
			return -1;
		}

		if (entry->m_isexpress) {
			/*TODO:not support express now*/
			UTER_LOG("Not support.");
			step->m_type = StepInfo::STEP_OVER;
			ctrlStep(pid);
			return -1;
		}

		/*TODO:i386*/
		unsigned long long cfa_addr;
		if (entry->m_reg_num == 4) {
			unsigned long long esp;
			g_target->pidRegSpGet(step->m_pid, esp);
			cfa_addr = esp + entry->m_offset;
		} else if (entry->m_reg_num == 5) {
			unsigned long long ebp;
			g_target->pidRegGet(step->m_pid, "ebp", ebp);
			cfa_addr = ebp + entry->m_offset;
		} else {
			UTER_LOG("Not support.");
			step->m_type = StepInfo::STEP_OVER;
			ctrlStep(pid);
			return -1;
		}

		QByteArray array;
		result = g_target->memoryGet(cfa_addr - arch->m_pointlen, arch->m_pointlen, array);
		if (result != 0) {
			UTER_LOG("Get step return address failed.");
			step->m_type = StepInfo::STEP_OVER;
			ctrlStep(pid);
			return -1;
		}
		unsigned long long return_to = 0;
		g_target->bytesToData(return_to, arch->m_pointlen, array);

		step->m_set_brk_at = return_to;
		result = g_target->breakInsert(step->m_set_brk_at, TargetBreak::BREAK_STEP);
		if (result < 0) {
			UTER_LOG("Set breakpoint at 0x%016llx for step return failed.", step->m_set_brk_at);
			step->m_type = StepInfo::STEP_OVER;
			ctrlStep(pid);
			return -1;
		}

		ctrlContinue();
		return 0;
	}

	ctrlStep(pid);
	return 0;
}

int Target::ctrlLogLevel(unsigned int level)
{
	UTER_DEBUG("Do set log level to %d.", level);
	if (m_status == STATUS_DOWN) {
		UTER_LOG("Can not set debug level on status %s.", m_status_desc[m_status]);
		return -1;
	}

	QString request_head = "request://loglvl?";
	QString response_head = "response://loglvl?";
	QString request = request_head + "&level=" + QString::asprintf("%d", level);
	QString response = ptracerReq(request);
	int findat = response.indexOf(response_head);
	if (findat != 0) {
		UTER_LOG("Set log level to ptracer failed, request error.");
		return -1;
	}
	QStringRef response_content = response.midRef(response_head.length());
	QMap<QString,QString> response_map = msgToMap(response_content);
	QMap<QString,QString>::iterator iter;
	QString key_value;
	iter = response_map.find("result");
	if (iter == response_map.end()) {
		UTER_LOG("Set log level to ptracer failed, response error.");
		return -1;
	}
	key_value = iter.value();
	if (key_value != "success") {
		UTER_LOG("Set log level to ptracer failed, set error.");
		return -1;
	}

	return 0;
}

int Target::runInit()
{
	QString request_head = "request://init?";
	QString response_head = "response://init?";
	QString request = request_head;
	QString response = ptracerReq(request);
	int findat = response.indexOf(response_head);
	if (findat != 0) {
		UTER_LOG("Initialize ptracer failed, request error.");
		return -1;
	}
	QStringRef response_content = response.midRef(response_head.length());
	QMap<QString,QString> response_map = msgToMap(response_content);
	QMap<QString,QString>::iterator iter;
	QString key_value;
	iter = response_map.find("result");
	if (iter == response_map.end()) {
		UTER_LOG("Initialize ptracer failed, response error.");
		return -1;
	}
	key_value = iter.value();
	if (key_value != "success") {
		UTER_LOG("Initialize ptracer failed, initialize error.");
		return -1;
	}

	return 0;
}

/*
 * Stop running thread. if pid is 0, stop all running thread.
 * If thread stopped, ignore stop request.
 * If thread running do request, request success always.
 * If thread running and stopped by some other reason when do request,
 * trigger the stop reason and ignore stop reguest by flag
 * (ignore as user stop signal, because stop flag clean by triggered stop reason).
 * */
int Target::runStop(unsigned int pid)
{
	/*Stop*/
	QString request_head = "request://stop?";
	QString response_head = "response://stop?";
	QString request = request_head + QString::asprintf("&pid=%d", pid);
	QString response = ptracerReq(request);
	int findat = response.indexOf(response_head);
	if (findat != 0) {
		UTER_LOG("Stop target failed, request error.");
		return -1;
	}
	QStringRef response_content = response.midRef(response_head.length());
	QMap<QString,QString> response_map = msgToMap(response_content);
	QMap<QString,QString>::iterator key_iter;
	QString key_value;
	key_iter = response_map.find("result");
	if (key_iter == response_map.end()) {
		UTER_LOG("Stop target failed, response error.");
		return -1;
	}
	key_value = key_iter.value();
	if (key_value != "success") {
		UTER_LOG("Stop target failed, do stop error.");
		return -1;
	}

	return 0;
}

int Target::runContinue(unsigned int pid, unsigned int signum)
{
	/*Continue*/
	QString request_head = "request://continue?";
	QString response_head = "response://continue?";
	QString request = request_head + QString::asprintf("pid=%d&signum=%d", pid, signum);
	QString response = ptracerReq(request);
	int findat = response.indexOf(response_head);
	if (findat != 0) {
		UTER_LOG("Continue target failed, request error.");
		return -1;
	}
	QStringRef response_content = response.midRef(response_head.length());
	QMap<QString,QString> response_map = msgToMap(response_content);
	QMap<QString,QString>::iterator key_iter;
	QString key_value;
	key_iter = response_map.find("result");
	if (key_iter == response_map.end()) {
		UTER_LOG("Continue target failed, response error.");
		return -1;
	}
	key_value = key_iter.value();
	if (key_value != "success") {
		UTER_LOG("Continue target failed, do stop error.");
		return -1;
	}

	return 0;
}

int Target::runStep(unsigned int pid, unsigned int signum)
{
	/*Step*/
	QString request_head = "request://step?";
	QString response_head = "response://step?";
	QString request = request_head + QString::asprintf("&pid=%d&signum=%d", pid, signum);
	QString response = ptracerReq(request);
	int findat = response.indexOf(response_head);
	if (findat != 0) {
		UTER_LOG("Step target failed, request error.");
		return -1;
	}
	QStringRef response_content = response.midRef(response_head.length());
	QMap<QString,QString> response_map = msgToMap(response_content);
	QMap<QString,QString>::iterator key_iter;
	QString key_value;
	key_iter = response_map.find("result");
	if (key_iter == response_map.end()) {
		UTER_LOG("Step target failed, response error.");
		return -1;
	}
	key_value = key_iter.value();
	if (key_value != "success") {
		UTER_LOG("Step target failed, do stop error.");
		return -1;
	}

	return 0;
}

unsigned long long Target::dataToNum(unsigned long long mem, unsigned int size, int signal)
{
    unsigned long long mask = 0;
    unsigned int iloop;
    for (iloop = size; iloop < sizeof(unsigned long long); iloop++) {
        mask |= (unsigned long long)0xff << 8 * iloop;
    }

    if (signal && (mem & ((unsigned long long)0x1 << ((size - 1) * 8 + 7)))) {
        return mem | mask;
    } else {
        return mem & ~mask;
    }
}

unsigned long long Target::NumToData(unsigned long long data, unsigned int size, int signal)
{
    unsigned long long mask = 0;
    unsigned int iloop;
    for (iloop = size; iloop < sizeof(unsigned long long); iloop++) {
        mask |= (unsigned long long)0xff << 8 * iloop;
    }

    if (signal && (data & (unsigned long long)0x1 << (sizeof(unsigned long long) * 8 - 1))) {
        return (data & ~mask) | ((unsigned long long)0x1 << ((size - 1) * 8 + 7)) ;
    } else {
        return data & ~mask;
    }
}

int Target::dataToBytes(unsigned long long in, int bytesize, QByteArray &bytes)
{
	bytes.clear();
	int iloop;
	for (iloop = 0; iloop < bytesize; iloop++) {		/*TODO:little-endian*/
		unsigned long long t = (in << ((8 - 1 - iloop) * 8)) >> ((8 - 1) * 8);
		/*Force convert to unsigned char*/
		bytes.append((unsigned char)t);
	}
	return 0;
}

int Target::bytesToData(unsigned long long &out, int bytesize, QByteArray &bytes)
{
	UTER_ASSERT(bytesize == bytes.size());
	out = 0;
	int iloop;
	for (iloop = 0; iloop < bytesize; iloop++) {		/*TODO:little-endian*/
		/*Force convert to unsigned char for zero extern*/
		unsigned long long tmp = (unsigned char)(bytes[iloop]);
		out |= tmp << (iloop * 8);
	}
	return 0;
}

const char *Target::m_status_desc[] = {
	"TARGET_STAT_DOWN",
	"TARGET_STAT_CONNECTED",
	"TARGET_STAT_RUNNING",
};

int Target::statSet(TargetStat stat)
{
	QMutexLocker autolock(&m_data_lock);
	UTER_DEBUG("Target status changed, from %s, to %s.", m_status_desc[m_status], m_status_desc[stat]);
	m_status = stat;

	emit signalStatChange(m_status);
	return 0;
}

Arch *Target::archInfoGet()
{
	/*TODO:*/
	return g_uter_arch_master.getArchByType(ARCH_TYPE_I386);
}

QString Target::archAddr(unsigned long long addr)
{
	Arch *arch = archInfoGet();
	if (arch == NULL) {
		return "Error";
	}

	if (arch->m_pointlen == 4) {
		return QString::asprintf("%08llx", addr);
	} else {
		return QString::asprintf("%016llx", addr);
	}
}

/*TODO:i386*/
StackInfo *Target::pidStackGet(unsigned int pid)
{
	int result;
	Arch *arch = archInfoGet();
	if (arch == NULL) {
		UTER_LOG("Get architecture specification failed.");
		return NULL;
	}

	unsigned long long eip;
	unsigned long long esp;
	unsigned long long ebp;

	result = pidRegGet(pid, "eip", eip);
	if (result < 0) {
		UTER_LOG("Get register failed.");
		return NULL;
	}
	result = pidRegGet(pid, "esp", esp);
	if (result < 0) {
		UTER_LOG("Get register failed.");
		return NULL;
	}
	result = pidRegGet(pid, "ebp", ebp);
	if (result < 0) {
		UTER_LOG("Get register failed.");
		return NULL;
	}

	StackInfo *ret_val = new StackInfo();

	/*Foreach level of stack*/
	while(1) {
		TargetMod *mod = modGetByAddrIn(eip);
		if (mod == NULL) {
			UTER_LOG("Get module failed.");
			break;
		}
		unsigned long long offset_addr = eip - mod->m_add_mkup;
		DwfFrameFde *fde = mod->dwarfGetFdeByAddrIn(offset_addr);
		if (fde == NULL) {
			UTER_LOG("Get fde failed.");
			break;
		}
		DwfRegTable *regtable = fde->getRegTabByAddrIn(offset_addr);
		if (regtable == NULL) {
			UTER_LOG("Get register table failed.");
			break;
		}
		DwfRegTable::Item *entry = regtable->getItemById(DwfLoc::REG_CFA_VAL);
		if (entry == NULL) {
			UTER_LOG("Get cfa register in table failed.");
			break;
		}

		if (entry->m_isexpress) {
			/*TODO: not support*/
			break;
		}

		/*TODO:i386*/
		bool use_ebp = false;
		unsigned long long cfa_addr;
		if (entry->m_reg_num == 4) {
			cfa_addr = esp + entry->m_offset;
		} else if (entry->m_reg_num == 5) {
			cfa_addr = ebp + entry->m_offset;
			use_ebp = true;
		} else {
			break;
		}

		/*Save stack information*/
		StackInfo::Item *node = new StackInfo::Item();
		node->m_addr = eip;
		node->m_stack = esp;
		node->m_cfa = cfa_addr;
		ret_val->m_stack_list.push_back(node);

		/*Next stack level*/
		/*eip save address*/
		unsigned long long eip_at = cfa_addr - arch->m_pointlen;

		QByteArray array;
		result = memoryGet(eip_at, arch->m_pointlen, array);
		if (result < 0) {
			UTER_LOG("Get next eip value in memory failed.");
			break;
		}
		bytesToData(eip, arch->m_pointlen, array);		/*new eip is end of current cfa*/

		if (use_ebp) {
			unsigned long long ebp_at = eip_at - arch->m_pointlen;
			result = memoryGet(ebp_at, arch->m_pointlen, array);
			if (result < 0) {
				UTER_LOG("Get next ebp value in memory failed.");
				break;
			}
			bytesToData(ebp, arch->m_pointlen, array);	/*new ebp is end of eip at*/
		}

		esp = cfa_addr;									/*new esp is current cfa*/
	}

	return ret_val;
error_ret:
	delete ret_val;
	return NULL;
}

RegValues *Target::pidRegsGet(unsigned int pid)
{
	Arch *arch = archInfoGet();
	if (arch == NULL) {
		UTER_LOG("Get architecture specification failed.");
		return NULL;
	}

	RegValues *retval = new RegValues();

	int iloop;
	for (iloop = 0; iloop < arch->m_regnum; iloop++) {
		ArchRegister *reg = arch->getRegByIndex(iloop);

		unsigned long long reg_val;
		int result = pidRegGet(pid, reg->m_name, reg_val);
		if (result < 0) {
			UTER_LOG("Get register %s value failed.", reg->m_name.toStdString().c_str());
			goto error_ret;
		}

		RegValues::Item *regval = new RegValues::Item();
		regval->m_name = reg->m_name;
		regval->m_value = reg_val;
		retval->m_reglist.push_back(regval);
	}

	return retval;
error_ret:
	delete retval;
	return NULL;
}

int Target::pidRegPcGet(unsigned int pid, unsigned long long &pc)
{
	if (m_status != STATUS_RUNNING) {
		UTER_LOG("Can not get pc register from target on status %s.", m_status_desc[m_status]);
		return -1;
	}
	if (! threadsStatInRange(TargetThread::STATUSSET_NOTRUN)) {
		UTER_LOG("Can not get pc register from target, some thread running.");
		return -1;
	}

	QString request_head = "request://instrget?";
	QString response_head = "response://instrget?";
	QString request = request_head + "pid=" + QString::asprintf("%d", pid);
	QString response = ptracerReq(request);
	int findat = response.indexOf(response_head);
	if (findat != 0) {
		UTER_LOG("Get threads pc register failed, request failed.");
		return -1;
	}
	QStringRef response_content = response.midRef(response_head.length());
	QMap<QString,QString> response_map = msgToMap(response_content);
	QMap<QString,QString>::iterator iter;
	QString key_value;
	iter = response_map.find("result");
	if (iter == response_map.end()) {
		UTER_LOG("Get threads pc register failed, response failed.");
		return -1;
	}
	key_value = iter.value();
	if (key_value != "success") {
		UTER_LOG("Get threads pc register failed, get regiser failed.");
		return -1;
	}
	iter = response_map.find("value");
	if (iter == response_map.end()) {
		UTER_LOG("Get threads pc register failed, no value.");
		return -1;
	}
	key_value = iter.value();
	pc = key_value.toULongLong(NULL, 16);

	return 0;
}

int Target::pidRegSpGet(unsigned int pid, unsigned long long &stack)
{
	if (m_status != STATUS_RUNNING) {
		UTER_LOG("Can not get sp register from target on status %s.", m_status_desc[m_status]);
		return -1;
	}
	if (! threadsStatInRange(TargetThread::STATUSSET_NOTRUN)) {
		UTER_LOG("Can not get sp register from target, some thread running.");
		return -1;
	}

	QString request_head = "request://stackget?";
	QString response_head = "response://stackget?";
	QString request = request_head + "pid=" + QString::asprintf("%d", pid);
	QString response = ptracerReq(request);
	int findat = response.indexOf(response_head);
	if (findat != 0) {
		UTER_LOG("Get threads sp register failed, request failed.");
		return -1;
	}
	QStringRef response_content = response.midRef(response_head.length());
	QMap<QString,QString> response_map = msgToMap(response_content);
	QMap<QString,QString>::iterator iter;
	QString key_value;
	iter = response_map.find("result");
	if (iter == response_map.end()) {
		UTER_LOG("Get threads stack register failed, response failed.");
		return -1;
	}
	key_value = iter.value();
	if (key_value != "success") {
		UTER_LOG("Get threads stack register failed, get regiser failed.");
		return -1;
	}
	iter = response_map.find("value");
	if (iter == response_map.end()) {
		UTER_LOG("Get threads stack register failed, no value.");
		return -1;
	}
	key_value = iter.value();
	stack = key_value.toULongLong(NULL, 16);

	return 0;
}

int Target::pidRegGet(unsigned int pid, QString name, unsigned long long &value)
{
	if (m_status != STATUS_RUNNING) {
		UTER_LOG("Can not get register from target on status %s.", m_status_desc[m_status]);
		return -1;
	}
	if (! threadsStatInRange(TargetThread::STATUSSET_NOTRUN)) {
		UTER_LOG("Can not get register from target, some thread running.");
		return -1;
	}

	QString request_head = "request://regget?";
	QString response_head = "response://regget?";
	QString request = request_head + "pid=" + QString::asprintf("%d", pid) + "&name=" + name;
	QString response = ptracerReq(request);
	int findat = response.indexOf(response_head);
	if (findat != 0) {
		UTER_LOG("Get register failed, request error.");
		return -1;
	}
	QStringRef response_content = response.midRef(response_head.length());
	QMap<QString,QString> response_map = msgToMap(response_content);
	QMap<QString,QString>::iterator key_iter;
	QString key_value;
	key_iter = response_map.find("result");
	if (key_iter == response_map.end()) {
		UTER_LOG("Get register failed, response error.");
		return -1;
	}
	key_value = key_iter.value();
	if (key_value != "success") {
		UTER_LOG("Get register failed, get register error.");
		return -1;
	}
	key_iter = response_map.find("value");
	if (key_iter == response_map.end()) {
		UTER_LOG("Get register failed, no value.");
		return -1;
	}
	key_value = key_iter.value();
	value = key_value.toULongLong(NULL, 16);

	return 0;
}

int Target::pidRegSet(unsigned int pid, QString name, unsigned long long value)
{
	if (m_status != STATUS_RUNNING) {
		UTER_LOG("Can not set register from target on status %s.", m_status_desc[m_status]);
		return -1;
	}
	if (! threadsStatInRange(TargetThread::STATUSSET_NOTRUN)) {
		UTER_LOG("Can not set register from target, some thread running.");
		return -1;
	}

	QString request_head = "request://regset?";
	QString response_head = "response://regset?";
	QString request = request_head + "pid=" + QString::asprintf("%d", pid)
	                               + "&name=" + name
								   + "&value=" + QString::asprintf("%016llx", value);
	QString response = ptracerReq(request);
	int findat = response.indexOf(response_head);
	if (findat != 0) {
		UTER_LOG("Set register failed, request error.");
		return -1;
	}
	QStringRef response_content = response.midRef(response_head.length());
	QMap<QString,QString> response_map = msgToMap(response_content);
	QMap<QString,QString>::iterator iter;
	QString key_value;
	iter = response_map.find("result");
	if (iter == response_map.end()) {
		UTER_LOG("Set register failed, response error.");
		return -1;
	}
	key_value = iter.value();
	if (key_value != "success") {
		UTER_LOG("Set register failed, get register error.");
		return -1;
	}

	return 0;
}

int Target::memoryGet(unsigned long long addr, unsigned int size, QByteArray &array)
{
	UTER_ASSERT(size != 0);
	array.clear();

	if (m_status != STATUS_RUNNING) {
		UTER_LOG("Can not get memory from target on status %s.", m_status_desc[m_status]);
		return -1;
	}
	if (! threadsStatInRange(TargetThread::STATUSSET_NOTRUN)) {
		UTER_LOG("Can not get memory from target, some thread running.");
		return -1;
	}

	QString request_head = "request://memget?";
	QString response_head = "response://memget?";
	QString request = request_head + "addr=" + QString::asprintf("%016llx", addr)
	                               + "&len=" + QString::asprintf("%d", size);
	QString response = ptracerReq(request);
	int findat = response.indexOf(response_head);
	if (findat != 0) {
		UTER_LOG("Get memory failed, request error.");
		return -1;
	}
	QStringRef response_content = response.midRef(response_head.length());
	QMap<QString,QString> response_map = msgToMap(response_content);
	QMap<QString,QString>::iterator iter;
	QString key_value;
	iter = response_map.find("result");
	if (iter == response_map.end()) {
		UTER_LOG("Get memory failed, response error.");
		return -1;
	}
	key_value = iter.value();
	if (key_value != "success") {
		UTER_LOG("Get memory failed, get memory error.");
		return -1;
	}
	iter = response_map.find("value");
	if (iter == response_map.end()) {
		UTER_LOG("Get memory failed, no value.");
		return -1;
	}
	key_value = iter.value();
	int count = key_value.length();
	if (count != size * 2) {
		UTER_LOG("Get memory failed, error number of bytes.");
		return -1;
	}
	int iloop;
	for (iloop = 0; iloop < count; iloop += 2) {
		QString byte = key_value.mid(iloop, 2);
		unsigned char data = byte.toUInt(NULL, 16);
		array.append(data);
	}

	return 0;
}

int Target::memorySet(unsigned long long addr, unsigned int size, QByteArray &array)
{
	UTER_ASSERT(size != 0);
	UTER_ASSERT(size == array.size());

	if (m_status != STATUS_RUNNING) {
		UTER_LOG("Can not set memory from target on status %s.", m_status_desc[m_status]);
		return -1;
	}
	if (! threadsStatInRange(TargetThread::STATUSSET_NOTRUN)) {
		UTER_LOG("Can not set memory from target, some thread running.");
		return -1;
	}

	QString request_head = "request://memset?";
	QString response_head = "response://memset?";
	QString request = request_head + "addr=" + QString::asprintf("%016llx", addr)
	                               + "&len=" + QString::asprintf("%d", size)
	                               + "&value=";
	int iloop;
	for (iloop = 0; iloop < size; iloop++) {
		unsigned char data = array[iloop];
		request += QString::asprintf("%02x", data);
	}
	QString response = ptracerReq(request);
	int findat = response.indexOf(response_head);
	if (findat != 0) {
		UTER_LOG("Set memory failed, request error.");
		return -1;
	}
	QStringRef response_content = response.midRef(response_head.length());
	QMap<QString,QString> response_map = msgToMap(response_content);
	QMap<QString,QString>::iterator iter;
	QString key_value;
	iter = response_map.find("result");
	if (iter == response_map.end()) {
		UTER_LOG("Set memory failed, response error.");
		return -1;
	}
	key_value = iter.value();
	if (key_value != "success") {
		UTER_LOG("Set memory failed, set memory error.");
		return -1;
	}

	return 0;
}

int Target::breakSet(unsigned long long addr)
{
	QString request_head = "request://breakset?";
	QString response_head = "response://breakset?";
	QString request = request_head + "addr=" + QString::asprintf("%016llx", addr);
	QString response = ptracerReq(request);
	int findat = response.indexOf(response_head);
	if (findat != 0) {
		UTER_LOG("Set breakpoint failed, request failed.");
		return -1;
	}
	QStringRef response_content = response.midRef(response_head.length());
	QMap<QString,QString> response_map = msgToMap(response_content);
	QMap<QString,QString>::iterator iter;
	QString key_value;
	iter = response_map.find("result");
	if (iter == response_map.end()) {
		UTER_LOG("Set breakpoint failed, response failed.");
		return -1;
	}
	key_value = iter.value();
	if (key_value != "success") {
		UTER_LOG("Set breakpoint failed, set breakpoint failed.");
		return -1;
	}

	return 0;
}

int Target::breakClear(unsigned long long addr)
{
	QString request_head = "request://breakclear?";
	QString response_head = "response://breakclear?";
	QString request = request_head + "addr=" + QString::asprintf("%016llx", addr);;
	QString response = ptracerReq(request);
	int findat = response.indexOf(response_head);
	if (findat != 0) {
		UTER_LOG("Clear all breakpoint failed, request failed.");
		return -1;
	}
	QStringRef response_content = response.midRef(response_head.length());
	QMap<QString,QString> response_map = msgToMap(response_content);
	QMap<QString,QString>::iterator key_iter;
	QString key_value;
	key_iter = response_map.find("result");
	if (key_iter == response_map.end()) {
		UTER_LOG("Clear all breakpoint failed, response failed.");
		return -1;
	}
	key_value = key_iter.value();
	if (key_value != "success") {
		UTER_LOG("Clear all breakpoint failed, clear breakpoint failed.");
		return -1;
	}

	return 0;
}

TargetMod *Target::modGetByPath(QString path)
{
	QMap<QString, TargetMod*>::iterator iter = m_mod_items.find(path);
	if (iter == m_mod_items.end()) {
		return NULL;
	}
	return iter.value();
}

TargetMod *Target::modGetByAddrIn(unsigned long long addr)
{
	QMap<QString, TargetMod*>::iterator iter;
	for (iter = m_mod_items.begin(); iter != m_mod_items.end(); iter++) {
		TargetMod *mod = iter.value();
		if (mod->m_addr_range.check(addr)) {
			return mod;
		}
	}
	return NULL;
}

int Target::modUpdate()
{
	QMap<QString, ModDesc*> moddesc_map;
	QMap<QString, ModDesc*>::iterator desc_iter;

	/*phase 1:Scan all map*/
	bool some_error = false;
	QList<TargetMap*>::iterator map_iter;
	for (map_iter = m_map_items.begin(); map_iter != m_map_items.end(); map_iter++) {
		TargetMap *map = *map_iter;

		if (map->m_path == "" || (map->m_inode == 0 && map->m_path != "[vdso]")) {
			/*Ignore no path module, ignore 0 inode module, but use [vdso]*/
			continue;
		}

		if (! (map->m_start < map->m_end)) {
			goto map_scan_failed;
		}

		ModDesc *desc;
		desc_iter = moddesc_map.find(map->m_path);
		if (desc_iter == moddesc_map.end()) {
			/*new module*/
			desc = new ModDesc();
			desc->m_path = map->m_path;
			desc->m_inode = map->m_inode;
			desc->m_start = map->m_start;
			desc->m_stop = map->m_end;
			moddesc_map.insert(desc->m_path, desc);
		} else {
			/*Update address range*/
			desc = *desc_iter;
			if (desc->m_inode != map->m_inode) {
				goto map_scan_failed;
			}
			if (map->m_start < desc->m_start) {
				desc->m_start = map->m_start;
			}
			if (map->m_end > desc->m_stop) {
				desc->m_stop = map->m_end;
			}
		}

		desc->m_add_range.m_begin = desc->m_start;
		desc->m_add_range.m_offset = desc->m_stop - desc->m_start;
	}
	if (0) {
map_scan_failed:
		for (desc_iter = moddesc_map.begin(); desc_iter != moddesc_map.end(); /**/) {
			ModDesc *desc = *desc_iter;
			moddesc_map.erase(desc_iter++);
		}
		return -1;
	}

	/*phase 2:Remove module*/
	QMap<QString, TargetMod*>::iterator mod_iter;
	for (mod_iter = m_mod_items.begin(); mod_iter != m_mod_items.end(); /**/) {
		TargetMod *mod = mod_iter.value();
		for (desc_iter = moddesc_map.begin(); desc_iter != moddesc_map.end(); desc_iter++) {
			ModDesc *desc = *desc_iter;
			if (mod->m_addr_range == desc->m_add_range) {
				break;
			}
		}
		if (desc_iter == moddesc_map.end()) {
			/*Remove! Remove! Remove!*/
			UTER_LOG("Remove module %s", mod->m_remote_path.toStdString().c_str());
			UTER_PRINT("Unload module %s.", mod->m_remote_path.toStdString().c_str());
			UTER_STBAR(2000, "Unload module %s.", mod->m_remote_path.toStdString().c_str());
			emit signalModDel(mod);
			mod->release();
			m_mod_items.erase(mod_iter++);
			delete mod;
		} else {
			mod_iter++;
		}
	}

	/*phase 3:Add module*/
	for (desc_iter = moddesc_map.begin(); desc_iter != moddesc_map.end(); desc_iter++) {
		ModDesc *desc = *desc_iter;
		for (mod_iter = m_mod_items.begin(); mod_iter != m_mod_items.end(); mod_iter++) {
			TargetMod *mod = mod_iter.value();
			if (mod->m_addr_range == desc->m_add_range) {
				break;
			}
		}
		if (mod_iter == m_mod_items.end()) {
			/*Add! Add! Add! */
			TargetMod *mod = new TargetMod();
			mod->m_addr_range = desc->m_add_range;
			mod->m_remote_path = desc->m_path;
			UTER_LOG("Add new module %s", mod->m_remote_path.toStdString().c_str());
			mod->load();		/*Load module, Load module*/

			if (mod->m_dwarf_cu_map.empty()) {	/*Show result*/
				UTER_PRINT("Load module %s%s.", mod->m_name, " (no debug information)");
			} else {
				UTER_PRINT("Load module %s%s.", mod->m_name, "");
			}

			mallocFreeUpdate(mod);
			m_mod_items.insert(mod->m_remote_path, mod);
			emit signalModAdd(mod);
		}
	}

	/*phase 4:clear*/
	for (desc_iter = moddesc_map.begin(); desc_iter != moddesc_map.end(); /**/) {
		ModDesc *desc = *desc_iter;
		moddesc_map.erase(desc_iter++);
	}
	return 0;
}

/*Update map information, result: -1 error, 0 no change, 1 changed*/
int Target::mapUpdate()
{
	QString request_head = "request://mapget?";
	QString response_head = "response://mapget?";
	QString request = request_head + "pid=" + QString::asprintf("%d", m_run_pid);
	QString response = ptracerReq(request);
	int findat = response.indexOf(response_head);
	if (findat != 0) {
		UTER_LOG("Get map information failed, request failed.");
		return -1;
	}
	QStringRef response_content = response.midRef(response_head.length());
	QList<ListNode> response_list = msgToList(response_content);
	QList<ListNode>::iterator list_iter;

	/*new map items*/
	QList<TargetMap*> new_map_items;
	QList<TargetMap*>::iterator iter;

	/*phase 1: parse map message*/
	TargetMap *cur_item = NULL;
	for (list_iter = response_list.begin(); list_iter != response_list.end(); list_iter++) {
		ListNode node = *list_iter;

		if (node.key == "separator" && cur_item != NULL) {
			/*End of a item*/
			new_map_items.push_back(cur_item);
			cur_item = NULL;
			continue;
		}
		if (cur_item == NULL) {
			/*Start new entry*/
			cur_item = new TargetMap();
		}

		if (node.key == "start") {
			cur_item->m_start = node.value.toULongLong(NULL, 16);
			continue;
		} else if (node.key == "end") {
			cur_item->m_end = node.value.toULongLong(NULL, 16);
			continue;
		} else if (node.key == "offset") {
			cur_item->m_offset = node.value.toULongLong(NULL, 16);
			continue;
		} else if (node.key == "flag_r") {
			cur_item->m_flag_r = (node.value == "true");
			continue;
		} else if (node.key == "flag_w") {
			cur_item->m_flag_w = (node.value == "true");
			continue;
		} else if (node.key == "flag_x") {
			cur_item->m_flag_x = (node.value == "true");
			continue;
		} else if (node.key == "flag_p") {
			cur_item->m_flag_p = (node.value == "true");
			continue;
		} else if (node.key == "major") {
			cur_item->m_major = node.value.toUInt(NULL, 10);
			continue;
		} else if (node.key == "minor") {
			cur_item->m_minor = node.value.toUInt(NULL, 10);
			continue;
		} else if (node.key == "inode") {
			cur_item->m_inode = node.value.toUInt(NULL, 10);
			continue;
		} else if (node.key == "path") {
			cur_item->m_path = node.value;
			continue;
		}
	}
	if (cur_item != NULL) {
		/*Ignore no end item*/
		delete cur_item;
		cur_item = NULL;
	}

	/*phase 2: check add*/
	for (iter = new_map_items.begin(); iter != new_map_items.end(); iter++) {
		TargetMap *map = *iter;
		QList<TargetMap*>::iterator find_iter;
		for (find_iter = m_map_items.begin(); find_iter != m_map_items.end(); find_iter++) {
			TargetMap *map_find = *find_iter;
			if (*map == *map_find) {
				break;
			}
		}
		if (find_iter == m_map_items.end()) {
			/*have add*/
			goto map_change;
		}
	}

	/*phase 3: check delete*/
	for (iter = m_map_items.begin(); iter != m_map_items.end(); iter++) {
		TargetMap *map = *iter;
		QList<TargetMap*>::iterator find_iter;
		for (find_iter = new_map_items.begin(); find_iter != new_map_items.end(); find_iter++) {
			TargetMap *map_find = *find_iter;
			if (*map == *map_find) {
				break;
			}
		}
		if (find_iter == new_map_items.end()) {
			/*have delete*/
			goto map_change;
		}
	}

	/*no map change*/
	for (iter = new_map_items.begin(); iter != new_map_items.end(); /*none*/) {
		TargetMap *map = *iter;
		delete map;
		new_map_items.erase(iter++);
	}
	return 0;

map_change:
	/*switch map information*/
	for (iter = m_map_items.begin(); iter != m_map_items.end(); /*none*/) {
		TargetMap *map = *iter;
		delete map;
		m_map_items.erase(iter++);
	}
	m_map_items = new_map_items;
	return 1;
}

int Target::mapModClear()
{
	QList<TargetMap*>::iterator map_iter;
	for (map_iter = m_map_items.begin(); map_iter != m_map_items.end(); /**/) {
		TargetMap *map = *map_iter;
		delete map;
		m_map_items.erase(map_iter++);
	}

	QMap<QString, TargetMod*>::iterator mod_iter;
	for (mod_iter = m_mod_items.begin(); mod_iter != m_mod_items.end(); /**/) {
		TargetMod *mod = mod_iter.value();
		/*Remove! Remove! Remove!*/
		emit signalModDel(mod);
		delete mod;
		m_mod_items.erase(mod_iter++);
	}

	return 0;
}

int Target::mallocFreeGet(unsigned long long &malloc, unsigned long long &free)
{
	QMutexLocker autolock(&m_data_lock);
	malloc = m_malloc_addr;
	free = m_free_addr;
	return 1;
}

int Target::mallocFreeUpdate(TargetMod *mod_file)
{
	UTER_ASSERT(mod_file != NULL);
	/*Check glibc so module with regex*/
	QRegExp regex("/(.*)/libc-(.*)\\.so");
	UTER_ASSERT(regex.isValid());
	int result = regex.indexIn(mod_file->m_remote_path);
	if (result < 0) {
		/*Other module*/
		return 0;
	}

	/*Get malloc and free*/
	BfdSym *sym;
	QMap<QString, BfdSym*>::iterator sym_iter;
	sym_iter = mod_file->m_bfd_dynsym_str_map.find("malloc");
	if (sym_iter == mod_file->m_bfd_dynsym_str_map.end()) {
		m_malloc_addr = 0;
		m_free_addr = 0;
		return -1;
	}
	sym = sym_iter.value();
	m_malloc_addr = mod_file->m_add_mkup + sym->m_addr;

	sym_iter = mod_file->m_bfd_dynsym_str_map.find("free");
	if (sym_iter == mod_file->m_bfd_dynsym_str_map.end()) {
		m_malloc_addr = 0;
		m_free_addr = 0;
		return -1;
	}
	sym = sym_iter.value();
	m_free_addr = mod_file->m_add_mkup + sym->m_addr;

	UTER_PRINT("Get malloc entry at %s, free entry at %s.",
				g_target->arch0xAddr(m_malloc_addr).toStdString().c_str(),
				g_target->arch0xAddr(m_free_addr).toStdString().c_str());
	return 1;
}

TargetBreak *Target::breakGetByAddr(unsigned long long addr)
{
	QMap<unsigned long long, TargetBreak*>::iterator iter;
	iter = m_break_set.find(addr);
	if (iter == m_break_set.end()) {
		return NULL;
	}
	TargetBreak *breakpnt =  iter.value();
	if (breakpnt->m_type_flag == 0) {
		/*No any type on*/
		return NULL;
	}
	return breakpnt;
}

void Target::breakChanged()
{
	if (threadsStatInRange(TargetThread::STATUSSET_NOTRUN) || m_run_flag_stopping) {
		/*all thread no running or stopping now*/
		return;
	}

	/*Do stop and set flag for breakpoint update*/
	ctrlStop();
	m_run_flag_breaking = true;
	return;
}

/*Insert breakpoint, argument token needed on coverage type*/
int Target::breakInsert(unsigned long long addr, TargetBreak::TargetBreakType type, CovToken *token)
{
	UTER_DEBUG("Insert break at 0x%016llx.", addr);

	TargetBreak *breakpnt;
	QMap<unsigned long long, TargetBreak*>::iterator iter = m_break_set.find(addr);
	if (iter == m_break_set.end()) {
		breakpnt = new TargetBreak;
		breakpnt->m_addr = addr;
		m_break_set.insert(breakpnt->m_addr, breakpnt);
	} else {
		breakpnt = iter.value();
	}

	/*breakpoint type duplication check*/
	UTER_ASSERT(!(breakpnt->m_type_flag & type));
	breakpnt->m_type_flag |= type;

	/*Coverage type need associate token*/
	if (type == TargetBreak::BREAK_COVFUN || type == TargetBreak::BREAK_COVFILE) {
		UTER_ASSERT(token != NULL);
		if (breakpnt->m_token == NULL) {
			breakpnt->m_token = token;
		} else {
			UTER_ASSERT(breakpnt->m_token == token);
		}
	}

	return 0;
}

int Target::breakDelete(unsigned long long addr, TargetBreak::TargetBreakType type)
{
	UTER_DEBUG("Delete break at 0x%016llx.", addr);

	TargetBreak *breakpnt;
	QMap<unsigned long long, TargetBreak*>::iterator iter = m_break_set.find(addr);
	if (iter == m_break_set.end()) {
		UTER_ASSERT(0);
		return -1;
	} else {
		breakpnt = iter.value();
	}

	UTER_ASSERT(breakpnt->m_type_flag & type);
	breakpnt->m_type_flag &= ~type;

	/*Clear token*/
	if (! (breakpnt->m_type_flag & (TargetBreak::BREAK_COVFUN | TargetBreak::BREAK_COVFILE))) {
		breakpnt->m_token = NULL;
	}

	/*Clear breakpoint on attach and detach*/
	return 0;
}

int Target::breakAttach()
{
	QMap<unsigned long long, TargetBreak*>::iterator iter;
	for (iter = m_break_set.begin(); iter != m_break_set.end(); /*none*/) {
		TargetBreak *thebreak = iter.value();
		/*Clear empty breakpoint*/
		if (thebreak->m_type_flag == 0) {
			delete thebreak;
			m_break_set.erase(iter++);
			continue;
		}

		UTER_DUMP("Attach break to ptracer 0x%016llx.", thebreak->m_addr);
		int result = breakSet(thebreak->m_addr);
		if (result < 0) {
			UTER_LOG("Set breakpoint 0x%016llx failed.", thebreak->m_addr);
		}

		thebreak->m_attached = true;
		iter++;
	}

	return 0;
}

int Target::breakDetach()
{
	UTER_DEBUG("Detach all breaks to ptracer.");
	breakClear(0);

	QMap<unsigned long long, TargetBreak*>::iterator iter;
	for (iter = m_break_set.begin(); iter != m_break_set.end(); /*none*/) {
		TargetBreak *thebreak = iter.value();

		/*Clear empty breakpoint*/
		if (thebreak->m_type_flag == 0) {
			delete thebreak;
			m_break_set.erase(iter++);
			continue;
		}

		thebreak->m_attached = false;
		iter++;
	}

	return 0;
}

bool Target::threadsStatInRange(unsigned int check)
{
	QMap<unsigned int, TargetThread*>::iterator iter;
	for (iter = m_thread_set.begin(); iter != m_thread_set.end(); iter++) {
		TargetThread *thread = iter.value();
		if (!(thread->m_stat & check)) {
			return false;
		}
	}
	return true;
}

TargetThread *Target::threadGetById(unsigned int pid)
{
	QMap<unsigned int, TargetThread*>::iterator iter = m_thread_set.find(pid);
	if(iter == m_thread_set.end()) {
		return NULL;
	} else {
		UTER_ASSERT(iter.key() == pid);
		UTER_ASSERT(iter.value() != NULL);
		return iter.value();
	}
	return NULL;
}

/*Just update thread name*/
int Target::threadUpdate()
{
	QString request_head = "request://threadsget?";
	QString response_head = "response://threadsget?";
	QString request = request_head;
	QString response = ptracerReq(request);
	int findat = response.indexOf(response_head);
	if (findat != 0) {
		UTER_LOG("Get threads information failed, request failed.");
		return -1;
	}
	QStringRef response_content = response.midRef(response_head.length());
	QList<ListNode> response_list = msgToList(response_content);
	QList<ListNode>::iterator list_iter;
	unsigned int pid = 0;
	QString name;
	for (list_iter = response_list.begin(); list_iter != response_list.end(); list_iter++) {
		ListNode node = *list_iter;
		if (node.key == "separator" && pid != 0) {
			/*End of section*/
			QMutexLocker autolock(&m_data_lock);
			TargetThread *tgt_thread = threadGetById(pid);
			if (tgt_thread != NULL) {
				tgt_thread->m_name = name;
			}
			pid = 0;
			name = "";
			continue;
		}
		if (node.key == "pid") {
			pid = node.value.toUInt(NULL, 10);
			continue;
		}
		if (node.key == "name") {
			name = node.value;
			continue;
		}
	}

	return 0;
}

int Target::threadClear()
{
	QMap<unsigned int, TargetThread*>::iterator thrd_iter;
	for (thrd_iter = m_thread_set.begin(); thrd_iter != m_thread_set.end(); /*none*/) {
		TargetThread *thrd = thrd_iter.value();
		delete thrd;
		m_thread_set.erase(thrd_iter++);
	}

	doShutDown();
	return 0;
}

void Target::deliverStopAction(unsigned int pid, TargetThread::TargetThreadStopBy stopby)
{
	/*When some stop event delivered, all step control block useless*/
	/*TODO:控制和断电的清除问题*/
	QMap<unsigned int, StepInfo*>::iterator step_iter;
	for (step_iter = m_step_set.begin(); step_iter != m_step_set.end(); step_iter++) {
		StepInfo *step = step_iter.value();
		step->m_valid = false;
	}

	emit signalStopAction(pid, stopby);
	return;
}

int Target::deliverStopStep(TargetThread *thread)
{
	int result;
	UTER_ASSERT(thread != NULL);

	QMap<unsigned int, StepInfo*>::iterator step_iter;
	step_iter = m_step_set.find(thread->m_pid);
	if (step_iter == m_step_set.end()) {
		/*some mistake, do step action immediately*/
		deliverStopAction(thread->m_pid, TargetThread::STOPBY_STEPD);
		return 0;
	}
	StepInfo *step = step_iter.value();
	if (! step->m_valid) {
		m_step_set.erase(step_iter);
		delete step;
		ctrlContinue();
		return 0;
	}

	/*Get instruction at now*/
	unsigned long long inst_at;
	result = pidRegPcGet(thread->m_pid, inst_at);
	if (result != 0) {
		goto delctrl_and_emitstop;
	}

	/*Get source file line for now*/
	TargetMod *mod;
	mod = g_target->modGetByAddrIn(inst_at);
	if (mod == NULL) {
		goto delctrl_and_emitstop;
	}
	unsigned long long offset_addr;
	offset_addr = inst_at - mod->m_add_mkup;
	DwfCu *cu;
	cu = mod->dwarfGetCuByAddrIn(offset_addr);
	if (cu == NULL) {
		goto delctrl_and_emitstop;
	}
	unsigned int fileno;
	unsigned int lineno;
	result = cu->getFileLineNoByAddrIn(offset_addr, fileno, lineno);
	if (result != 0) {
		goto delctrl_and_emitstop;
	}

	/*Check function call happened*/
	DwfFun *fun;
	fun = cu->getFunByAddr(offset_addr);
	if (fun == NULL) {
		/* No function call happened, so some step type is same.
		 * step by step to different source file line*/
		TargetMod *from_mod = g_target->modGetByAddrIn(step->m_step_from);
		if (from_mod == NULL || from_mod != mod) {
			goto delctrl_and_emitstop;
		}
		unsigned long long from_offset_addr;
		from_offset_addr = step->m_step_from - from_mod->m_add_mkup;
		DwfCu *from_cu;
		from_cu = from_mod->dwarfGetCuByAddrIn(from_offset_addr);
		if (from_cu == NULL) {
			goto delctrl_and_emitstop;
		}
		unsigned int from_fileno;
		unsigned int from_lineno;
		result = from_cu->getFileLineNoByAddrIn(from_offset_addr, from_fileno, from_lineno);
		if (result != 0) {
			goto delctrl_and_emitstop;
		}

		if (fileno != from_fileno || lineno != from_lineno) {
			/*Source file line changed, goto is normally*/
			goto delctrl_and_emitstop;
		}

		/*Still on the same file line, keep step*/
		ctrlStep(thread->m_pid);
		return 0;
	} else {
		/*Function call happened, switch step type*/
		if (step->m_type == StepInfo::STEP_IN) {
			/* Set breakpoint on address which is the second statement in function.
			 * because first statement is {. when the breakpoint triggered, step in finished*/
			DwfLine *second_line = NULL;
			QList<DwfLine*>::iterator line_iter;
			for (line_iter = cu->m_dwf_line_list.begin();
				 line_iter != cu->m_dwf_line_list.end(); line_iter++) {
				DwfLine *line = *line_iter;
				if (line->m_lineaddr >= offset_addr) {
					/*found first statement, get next statement*/
					line_iter++;
					if (line_iter != cu->m_dwf_line_list.end()) {
						/*found second statement*/
						second_line = *line_iter;
					}
					break;
				}
			}
			if (second_line == NULL) {
				/*no second statement*/
				goto delctrl_and_emitstop;
			}

			step->m_set_brk_at = mod->m_add_mkup + second_line->m_lineaddr;
			result = g_target->breakInsert(step->m_set_brk_at, TargetBreak::BREAK_STEP);
			if (result < 0) {
				UTER_LOG("Set breakpoint for second statement on address 0x%016llx failed.",
						 step->m_set_brk_at);
				goto delctrl_and_emitstop;
			}

			ctrlContinue();
			return 0;
		} else if (step->m_type == StepInfo::STEP_OVER) {
			/* We need come back to address which after call instruction first.
			 * so set breakpoint on return address which is the function-call return to.
			 * when breakpoint triggered, keep step by step to next line as usually
			 * Get return to address by stack register first*/
			Arch *arch = g_target->archInfoGet();
			if (arch == NULL) {
				UTER_LOG("Get architecture specification failed.");
				goto delctrl_and_emitstop;
			}

			unsigned long long stack;
			result = g_target->pidRegSpGet(thread->m_pid, stack);
			if (result != 0) {
				UTER_LOG("Get stack register of thread %d failed.", thread->m_pid);
				goto delctrl_and_emitstop;
			}

			QByteArray array;
			result = g_target->memoryGet(stack, arch->m_pointlen, array);
			if (result != 0) {
				UTER_LOG("Get return address of thread %d failed.", thread->m_pid);
				goto delctrl_and_emitstop;
			}
			unsigned long long return_to = 0;
			g_target->bytesToData(return_to, arch->m_pointlen, array);

			step->m_set_brk_at = return_to;
			result = g_target->breakInsert(step->m_set_brk_at, TargetBreak::BREAK_STEP);
			if (result < 0) {
				UTER_LOG("Set breakpoint on return address 0x%016llx failed.",
						  step->m_set_brk_at);
				goto delctrl_and_emitstop;
			}

			ctrlContinue();
			return 0;
		} else {
			UTER_ASSERT(0);
		}
	}

	return 0;
/*End step*/
delctrl_and_emitstop:
	m_step_set.erase(step_iter);
	delete step;
	deliverStopAction(thread->m_pid, TargetThread::STOPBY_STEPD);
	return 0;
}

int Target::deliverStopBreak(TargetThread *thread)
{
	int result;
	UTER_ASSERT(thread != NULL);

	TargetBreak *brkpnt = breakGetByAddr(thread->m_brk_addr);
	if (brkpnt == NULL) {
		UTER_DEBUG("Break at 0x%016llx but no breakpoint.", thread->m_brk_addr);
		ctrlContinue();
		return 0;
	}
	UTER_DEBUG("Break at 0x%016llx with flag 0x%08x.", thread->m_brk_addr, brkpnt->m_type_flag);

	/* phase 1: return breakpoint process, return breakpoint preferential. for example:
	 * some thread pause on at breakpoint but not triggered now, then set breakpoint and call.
	 * When return breakpoint triggered, it is just meaningful call, no association about
	 * breakpoint for debug. So when return breakpoint triggered, ignore other breakpoint flag,
	 * for restore environment of call.*/
	/*TODO:step 穿过断点与STUB等断点叠加问题，或者其他类型断点，主要是triggerd 标志别清掉什么的影响*/
	if (brkpnt->m_type_flag & TargetBreak::BREAK_RET) {
		unsigned long long eip_value = thread->m_brk_addr;
		unsigned long long esp_value = 0;
		result = pidRegSpGet(thread->m_pid, esp_value);
		if (result < 0) {
			UTER_LOG("Get stack register of thread %d failed.", thread->m_pid);
			return -1;
		}
		result = g_tester->targetReturnTriggerd(eip_value, thread->m_pid, esp_value);
		if (result == 1) {
			/*Breakpoint triggered*/
			return 0;
		}
	}

	/*phase 2: process coverage breakpoint if we have*/
	if (brkpnt->m_type_flag & (TargetBreak::BREAK_COVFUN | TargetBreak::BREAK_COVFILE)) {
		TargetBreak *brkpnt = breakGetByAddr(thread->m_brk_addr);
		if (brkpnt == NULL) {
			goto end_of_converage;
		}
		CovToken *token = brkpnt->m_token;
		if (token == NULL) {
			goto end_of_converage;
		}
		if (brkpnt->m_type_flag & TargetBreak::BREAK_COVFUN) {
			token->m_hit_for_fun = true;
		}
		if (brkpnt->m_type_flag & TargetBreak::BREAK_COVFILE) {
			token->m_hit_for_file = true;
		}
	}
end_of_converage:

	/*phase 3: breakpoint for source step(advanced step)*/
	if (brkpnt->m_type_flag & TargetBreak::BREAK_STEP) {
		QMap<unsigned int, StepInfo*>::iterator step_iter;
		step_iter = m_step_set.find(thread->m_pid);
		if (step_iter == m_step_set.end()) {
			/*No step control block*/
			goto end_of_step;
		}
		StepInfo *step = step_iter.value();
		if (! step->m_valid) {
			/*Step control block invalid*/
			m_step_set.erase(step_iter);
			delete step;
			goto end_of_step;
		}
		UTER_ASSERT(step->m_set_brk_at == thread->m_brk_addr);
		/*Switch step type*/
		if (step->m_type == StepInfo::STEP_OVER) {
			/* STEP_OVER do step by step to next line usually, but if current instruction
			 * is the first instruction in function, it is means function called happened.
			 * we need come back to address which after call instruction first.
			 * so set breakpoint on return address which is the function-call return to.
			 * when breakpoint triggered, keep step by step to next line as usually*/
			TargetMod *mod = g_target->modGetByAddrIn(thread->m_brk_addr);
			if (mod == NULL) {
				goto stepover_delctrl_emitstop;
			}
			unsigned long long offset_addr;
			offset_addr = thread->m_brk_addr - mod->m_add_mkup;
			DwfCu *cu;
			cu = mod->dwarfGetCuByAddrIn(offset_addr);
			if (cu == NULL) {
				goto stepover_delctrl_emitstop;
			}
			unsigned int fileno;
			unsigned int lineno;
			result = cu->getFileLineNoByAddr(offset_addr, fileno, lineno);
			if (result == 0) {
				goto stepover_delctrl_emitstop;
			}
			g_target->breakDelete(step->m_set_brk_at, TargetBreak::BREAK_STEP);
			step->m_set_brk_at = 0;
			/*Step by step to different source line*/
			step->m_step_from = thread->m_brk_addr;
			ctrlStep(thread->m_pid);
			return 0;
		stepover_delctrl_emitstop:
			/*If have some mistake, do step action immediately*/
			g_target->breakDelete(step->m_set_brk_at, TargetBreak::BREAK_STEP);
			step->m_set_brk_at = 0;
			m_step_set.erase(step_iter);
			delete step;
			deliverStopAction(thread->m_pid, TargetThread::STOPBY_STEPD);
			return 0;
		} else if (step->m_type == StepInfo::STEP_IN) {
			/* STEP_IN do step by step to next line usually, but if current instruction
			 * is the first instruction in function, it is means function called happened.
			 * so set breakpoint on address which is the second statement in function.
			 * because first statement is {. when the breakpoint triggered, step in finished*/
			g_target->breakDelete(step->m_set_brk_at, TargetBreak::BREAK_STEP);
			step->m_set_brk_at = 0;
			m_step_set.erase(step_iter);
			delete step;
			deliverStopAction(thread->m_pid, TargetThread::STOPBY_STEPD);
			return 0;
		} else if (step->m_type == StepInfo::STEP_RETURN) {
			/* STEP_RETURN set breakpoint on address which is the function-call return to usually,
			 * when the breakpoint triggered, step return finished*/
			g_target->breakDelete(step->m_set_brk_at, TargetBreak::BREAK_STEP);
			step->m_set_brk_at = 0;
			m_step_set.erase(step_iter);
			delete step;
			deliverStopAction(thread->m_pid, TargetThread::STOPBY_STEPD);
			return 0;
		} else {
			UTER_ASSERT(0);
		}
	}
end_of_step:

	/*phase 4: debug breakpoint*/
	if (brkpnt->m_type_flag & TargetBreak::BREAK_DBG) {
		deliverStopAction(thread->m_pid, TargetThread::STOPBY_BREAK);
		return 0;
	}

	/* phase 5: stub breakpoint is last process. we need check arguments before function stub*/
	/*TODO:调试断点叠加STUB的问题*/
	if (brkpnt->m_type_flag & TargetBreak::BREAK_STUB) {
		result = g_tester->targetStubTriggerd(thread->m_brk_addr, thread->m_pid);
		if (result == 1) {
			return 0;
		}
	}

	/*No breakpoint worked, continue running*/
	ctrlContinue();
	return 0;
}

int Target::deliverStop(TargetThread *thread)
{
	if (thread == NULL) {
		/*deliver pause stop*/
		UTER_DEBUG("Stop happened on thread %d (main thread)", m_run_pid);
		deliverStopAction(m_run_pid, TargetThread::STOPBY_PAUSE);
		return 0;
	}

	switch(thread->m_stop_by)
	{
	case TargetThread::STOPBY_BREAK:
	{
		/*deliver breakpoint stop, some breakpoint use for system*/
		UTER_DEBUG("Break happened on thread %d at address 0x%016llx",
					thread->m_pid, thread->m_brk_addr);
		deliverStopBreak(thread);
		break;
	}
	case TargetThread::STOPBY_STEPD:
	{
		/*deliver step stop, some step use for system*/
		UTER_DEBUG("Step happened on thread %d", thread->m_pid);
		deliverStopStep(thread);
		break;
	}
	case TargetThread::STOPBY_SIGNAL:
	{
		UTER_DEBUG("Signal happened on thread %d by signal %d",
					thread->m_pid, thread->m_num_of_signal);
		deliverStopAction(thread->m_pid, TargetThread::STOPBY_SIGNAL);
		break;
	}
	case TargetThread::STOPBY_SYSCALLD:
	{
		UTER_DEBUG("Syscall happened on thread %d", thread->m_pid);
		deliverStopAction(thread->m_pid, TargetThread::STOPBY_SYSCALLD);
		break;
	}
	default:
	{
		UTER_ASSERT(0);
		break;
	}
	}

	return 0;
}

/*After any stop event*/
int Target::postStopEvent(TargetThread *thread)
{
	UTER_ASSERT(thread != NULL);
	UTER_ASSERT(thread->m_stat & TargetThread::STATUSSET_STOP);

	thread->m_triggered = false;

	if (! threadsStatInRange(TargetThread::STATUSSET_NOTRUN)) {
		/*Some thread running, so try to stop other thread*/
		if (thread->m_stop_by != TargetThread::STOPBY_PAUSE && (! m_run_flag_stopping)) {
			/*broadcast stop and flag stopping*/
			runStop(0);
			m_run_flag_stopping = true;
		}
		return 0;
	}

	/*Clear flag when all thread stoped*/
	m_run_flag_stopping = false;

	/*Remove breakpoint from target*/
	breakDetach();

	/*Update breakpoint when changed*/
	if (m_run_flag_breaking) {
		m_run_flag_breaking = false;
		/*When all thread paused explain stop by breakpoint change*/
		if (threadsStatInRange(TargetThread::STATUS_PAUSE)) {
			ctrlContinue();
			return 0;
		}
	}

	/*Update some information about target*/
	doUpdateOnStop();

	/*Deliver pause stop*/
	if (threadsStatInRange(TargetThread::STATUS_PAUSE)) {
		deliverStop(NULL);
		return 0;
	}

	/*Scan stop event*/
	QMap<unsigned int, TargetThread*>::iterator iter;
	for (iter = m_thread_set.begin(); iter != m_thread_set.end(); iter++) {
		TargetThread *thread = iter.value();
		if (thread->m_stop_by != TargetThread::STOPBY_PAUSE && (! thread->m_triggered)) {
			thread->m_triggered = true;
			/*Trigger one event per time*/
			deliverStop(thread);
			return 0;
		}
	}

	return 0;
}

/*It is complex about multi-thread*/
int Target::doGoOnProc()
{
	UTER_ASSERT(threadsStatInRange(TargetThread::STATUSSET_GO));

	/*phase 1:Trigger remain stop event*/
	QMap<unsigned int, TargetThread*>::iterator thrd_iter;
	for (thrd_iter = m_thread_set.begin(); thrd_iter != m_thread_set.end(); thrd_iter++) {
		TargetThread *thread = thrd_iter.value();
		/*STOPBY_PAUSE not trigger again and trigger by flag*/
		if ((thread->m_stop_by != TargetThread::STOPBY_PAUSE) && (! thread->m_triggered)) {
			thread->m_triggered = true;
			/*Trigger one event per time*/
			deliverStop(thread);
			return 0;
		}
	}

	/* phase 2: through the triggered breakpoint by step without breakpoint apply (the thread pc still at the breakpoint).
	 * If not do this, the breakpoint will triggered again.
	 * In this process other thread keep stop, because no breakpoint apply*/
	QSet<unsigned int> break_on_way_pids;
	for (thrd_iter = m_thread_set.begin(); thrd_iter != m_thread_set.end(); thrd_iter++) {
		TargetThread *thread = thrd_iter.value();
		UTER_ASSERT(thread->m_stat & TargetThread::STATUSSET_GO);
		if (thread->m_stop_by == TargetThread::STOPBY_BREAK) {
			unsigned long long now = 0;
			int result = pidRegPcGet(thread->m_pid, now);
			if (now == thread->m_brk_addr) {
				/*Thread pc still at the breakpoint, so need through. (breakpoint on the way)*/
				break_on_way_pids.insert(thread->m_pid);
			}
		}
	}

	if (! break_on_way_pids.empty()) {
		QSet<unsigned int>::iterator pid_iter;
		for (pid_iter = break_on_way_pids.begin(); pid_iter != break_on_way_pids.end(); pid_iter++) {
			thrd_iter = m_thread_set.find(*pid_iter);
			if (thrd_iter == m_thread_set.end()) {
				continue;
			}
			TargetThread *thread = thrd_iter.value();

			unsigned int num_of_signal = 0;
			if (thread->m_stop_by == TargetThread::STOPBY_SIGNAL) {
				num_of_signal = thread->m_num_of_signal;
			}
			/*through breakpoint with step*/
			runStep(thread->m_pid, num_of_signal);

			if (thread->m_stat == TargetThread::STATUS_STEP) {
				/*When step triggered, keep step event process*/
				thread->m_stat = TargetThread::STATUS_RUNNING;
				thread->m_stop_by = TargetThread::STOPBY_MAX;
			} else if (thread->m_stat == TargetThread::STATUS_CONTINUE) {
				/*When step triggered, do continue*/
				thread->m_stat = TargetThread::STATUS_RUNSTEP;
				thread->m_stop_by = TargetThread::STOPBY_MAX;
			} else {
				UTER_ASSERT(0);
			}
		}

		emit signalRunAction();
		return 0;
	}

	/*phase 3: Do continue*/
	/*Apply break point to target*/
	breakAttach();

	for (thrd_iter = m_thread_set.begin(); thrd_iter != m_thread_set.end(); thrd_iter++) {
		TargetThread *thread = thrd_iter.value();

		unsigned int num_of_signal = 0;
		if (thread->m_stop_by == TargetThread::STOPBY_SIGNAL) {
			num_of_signal = thread->m_num_of_signal;
		}

		if (thread->m_stat == TargetThread::STATUS_STEP) {
			/*Step*/
			runStep(thread->m_pid, num_of_signal);
			thread->m_stat = TargetThread::STATUS_RUNNING;
			thread->m_stop_by = TargetThread::STOPBY_MAX;
		} else if (thread->m_stat == TargetThread::STATUS_CONTINUE) {
			/*Continue*/
			runContinue(thread->m_pid, num_of_signal);
			thread->m_stat = TargetThread::STATUS_RUNNING;
			thread->m_stop_by = TargetThread::STOPBY_MAX;
		} else {
			UTER_ASSERT(0);
		}
	}

	emit signalRunAction();
	return 0;
}

/*Try to shutdown*/
int Target::doShutDown()
{
	if (! m_thread_set.empty()) {
		/*Have thread, can not shutdown*/
		return 0;
	}

	/*Release module*/
	mapModClear();

	m_run_pid = 0;
	statSet(STATUS_CONNECTED);

	emit signalShutAction();
	return 0;
}

int Target::doUpdateOnStop()
{
	/*Flush thread name*/
	threadUpdate();
	/*Update map*/
	int result = mapUpdate();
	if (result > 0) {
		/*Update module*/
		modUpdate();
	}

	return 0;
}

int Target::doEventStartOk(QString &event, QString &event_head)
{
	QStringRef event_content = event.midRef(event_head.length());
	QMap<QString,QString> kv_map = msgToMap(event_content);
	QMap<QString,QString>::iterator kv_iter;
	kv_iter = kv_map.find("pid");
	if (kv_iter == kv_map.end()) {
		return -1;
	}
	QString value = kv_iter.value();
	unsigned int pid = value.toUInt(NULL, 10);
	if (pid == 0) {
		UTER_LOG("Start ok event with wrong thread id %d.", pid);
		return -1;
	}

	if (! m_run_flag_starting) {
		UTER_LOG("Ignore start ok event on not starting status.");
		return -1;
	}
	if (m_thread_set.empty()) {
		UTER_LOG("Start ok but no any thread.");
		return -1;
	}
	if (m_run_pid != pid) {
		UTER_LOG("Start ok event with wrong process id %d for process %d.", pid, m_run_pid);
		return -1;
	}
	TargetThread *thrd = threadGetById(m_run_pid);
	if (thrd == NULL) {
		UTER_LOG("No thread with process id %d.", m_run_pid);
		return -1;
	}

	/*Start target finish*/
	m_run_flag_starting = false;
	UTER_PRINT("Target start with pid %d.", m_run_pid);
	UTER_STBAR(2000, "Target start with pid %d.", m_run_pid);

	/*Start ok is a stop event too*/
	doUpdateOnStop();

	/*All thread with STATUS_PAUSE + STOPBY_PAUSE, so continue smoothly*/
	ctrlContinue();
	return 0;
}

int Target::doEventStartFail(QString &event, QString &event_head)
{
	QStringRef event_content = event.midRef(event_head.length());
	QMap<QString,QString> kv_map = msgToMap(event_content);
	QMap<QString,QString>::iterator kv_iter;
	kv_iter = kv_map.find("pid");
	if (kv_iter == kv_map.end()) {
		return -1;
	}
	QString value = kv_iter.value();
	unsigned int pid = value.toUInt(NULL, 10);
	kv_iter = kv_map.find("code");
	if (kv_iter == kv_map.end()) {
		return -1;
	}
	value = kv_iter.value();
	unsigned int code = value.toUInt(NULL, 10);
	if (pid == 0) {
		UTER_LOG("Start fail event with wrong thread id %d.", pid);
		return -1;
	}

	if (! m_run_flag_starting) {
		UTER_LOG("Ignore start fail event on not starting status.");
		return -1;
	}
	if (m_run_pid != pid) {
		UTER_LOG("Start fail event with wrong process id %d for process %d.", pid, m_run_pid);
		return -1;
	}
	UTER_DEBUG("Start fail event for process %d.", m_run_pid);

	/*Clear*/
	m_run_flag_starting = false;
	threadClear();
	return 0;
}

int Target::doEventThreadStart(QString &event, QString &event_head)
{
	QStringRef event_content = event.midRef(event_head.length());
	QMap<QString,QString> kv_map = msgToMap(event_content);
	QMap<QString,QString>::iterator kv_iter;
	kv_iter = kv_map.find("pid");
	if (kv_iter == kv_map.end()) {
		return -1;
	}
	QString value = kv_iter.value();
	unsigned int pid = value.toUInt(NULL, 10);
	if (pid == 0) {
		UTER_LOG("Thread start event with wrong thread id %d.", pid);
		return -1;
	}

	TargetThread *thread = threadGetById(pid);
	if (thread != NULL) {
		UTER_LOG("Thread %d start already.", thread->m_pid);
		return -1;
	}

	thread = new TargetThread();
	thread->m_pid = pid;
	m_thread_set.insert(thread->m_pid, thread);
	UTER_PRINT("Start thread %d.", thread->m_pid);
	UTER_STBAR(2000, "Start thread %d.", thread->m_pid);

	if (m_run_flag_starting) {
		/*Thread start on starting, keep STATUS_PAUSE + STOPBY_PAUSE, wait start ok*/
		UTER_DEBUG("Create thread %d, suspend it on starting.", thread->m_pid);
		thread->m_stat = TargetThread::STATUS_PAUSE;
		thread->m_stop_by = TargetThread::STOPBY_PAUSE;
	} else {
		/*Thread start after starting, continue the thread*/
		UTER_DEBUG("Create thread %d, continue the thread.", thread->m_pid);
		UTER_PRINT("Start thread %d.", thread->m_pid);
		UTER_STBAR(2000, "Start thread %d.", thread->m_pid);

		runContinue(pid, 0);
		thread->m_stat = TargetThread::STATUS_RUNNING;
		thread->m_stop_by = TargetThread::STOPBY_MAX;
	}

	return 0;
}

int Target::doEventThreadEnd(QString &event, QString &event_head)
{
	QStringRef event_content = event.midRef(event_head.length());
	QMap<QString,QString> kv_map = msgToMap(event_content);
	QMap<QString,QString>::iterator kv_iter;
	kv_iter = kv_map.find("pid");
	if (kv_iter == kv_map.end()) {
		return -1;
	}
	QString value = kv_iter.value();
	unsigned int pid = value.toUInt(NULL, 10);
	if (pid == 0) {
		UTER_LOG("Thread end with wrong thread id %d.", pid);
		return -1;
	}

	TargetThread *thread = threadGetById(pid);
	if (thread == NULL) {
		UTER_LOG("No thread information for thread %d.", thread->m_pid);
		return -1;
	}

	/*Thread will be die, so do continue and wait it exited or killed*/
	runContinue(pid, 0);
	thread->m_stat = TargetThread::STATUS_RUNNING;
	thread->m_stop_by = TargetThread::STOPBY_MAX;
	return 0;
}

int Target::doEventThreadKilled(QString &event, QString &event_head)
{
	QStringRef event_content = event.midRef(event_head.length());
	QMap<QString,QString> kv_map = msgToMap(event_content);
	QMap<QString,QString>::iterator kv_iter;
	kv_iter = kv_map.find("pid");
	if (kv_iter == kv_map.end()) {
		return -1;
	}
	QString value = kv_iter.value();
	unsigned int pid = value.toUInt(NULL, 10);
	if (pid == 0) {
		UTER_LOG("Thread killed with wrong thread id %d.", pid);
		return -1;
	}

	QMap<unsigned int, TargetThread*>::iterator iter;
	iter = m_thread_set.find(pid);
	if (iter == m_thread_set.end()) {
		UTER_LOG("No thread information for thread %d.", pid);
		return -1;
	}
	/*delete thread*/
	TargetThread *thread = iter.value();
	m_thread_set.erase(iter);
	delete thread;

	doShutDown();
	return 0;
}

int Target::doEventThreadExited(QString &event, QString &event_head)
{
	QStringRef event_content = event.midRef(event_head.length());
	QMap<QString,QString> kv_map = msgToMap(event_content);
	QMap<QString,QString>::iterator kv_iter;
	kv_iter = kv_map.find("pid");
	if (kv_iter == kv_map.end()) {
		return -1;
	}
	QString value = kv_iter.value();
	unsigned int pid = value.toUInt(NULL, 10);
	if (pid == 0) {
		UTER_LOG("Thread exit with wrong thread id %d.", pid);
		return -1;
	}

	QMap<unsigned int, TargetThread*>::iterator iter;
	iter = m_thread_set.find(pid);
	if (iter == m_thread_set.end()) {
		UTER_LOG("No thread information for thread %d.", pid);
		return -1;
	}
	/*delete thread*/
	TargetThread *thread = iter.value();
	m_thread_set.erase(iter);
	delete thread;

	doShutDown();
	return 0;
}

int Target::doEventFork(QString &event, QString &event_head)
{
	UTER_LOG("No support now, doEventFork.");
	return 0;
}

int Target::doEventExec(QString &event, QString &event_head)
{
	UTER_LOG("No support now, doEventExec.");
	return 0;
}

int Target::doEventStop(QString &event, QString &event_head)
{
	QStringRef event_content = event.midRef(event_head.length());
	QMap<QString,QString> kv_map = msgToMap(event_content);
	QMap<QString,QString>::iterator kv_iter;
	kv_iter = kv_map.find("pid");
	if (kv_iter == kv_map.end()) {
		return -1;
	}
	QString value = kv_iter.value();
	unsigned int pid = value.toUInt(NULL, 10);
	if (pid == 0) {
		UTER_LOG("Thread stop with wrong thread id %d.", pid);
		return -1;
	}

	TargetThread *thread = threadGetById(pid);
	if (thread == NULL) {
		UTER_LOG("No thread info for thread %d.", pid);
		return -1;
	}

	thread->m_stat = TargetThread::STATUS_PAUSE;
	thread->m_stop_by = TargetThread::STOPBY_PAUSE;
	postStopEvent(thread);
	return 0;
}

int Target::doEventSignal(QString &event, QString &event_head)
{
	QStringRef event_content = event.midRef(event_head.length());
	QMap<QString,QString> kv_map = msgToMap(event_content);
	QMap<QString,QString>::iterator kv_iter;
	kv_iter = kv_map.find("pid");
	if (kv_iter == kv_map.end()) {
		return -1;
	}
	QString value = kv_iter.value();
	unsigned int pid = value.toUInt(NULL, 10);
	kv_iter = kv_map.find("signum");
	if (kv_iter == kv_map.end()) {
		return -1;
	}
	value = kv_iter.value();
	unsigned int signal_num = value.toUInt(NULL, 10);

	if (pid == 0) {
		UTER_LOG("Thread signed with wrong thread id %d.", pid);
		return -1;
	}

	TargetThread *thread = threadGetById(pid);
	if (thread == NULL) {
		UTER_LOG("No thread info for thread %d.", pid);
		return -1;
	}

	thread->m_stat = TargetThread::STATUS_STOPED;
	thread->m_stop_by = TargetThread::STOPBY_SIGNAL;
	thread->m_num_of_signal = signal_num;
	postStopEvent(thread);
	return 0;
}

int Target::doEventBreak(QString &event, QString &event_head)
{
	QStringRef event_content = event.midRef(event_head.length());
	QMap<QString,QString> kv_map = msgToMap(event_content);
	QMap<QString,QString>::iterator kv_iter;
	kv_iter = kv_map.find("pid");
	if (kv_iter == kv_map.end()) {
		return -1;
	}
	QString value = kv_iter.value();
	unsigned int pid = value.toUInt(NULL, 10);
	kv_iter = kv_map.find("addr");
	if (kv_iter == kv_map.end()) {
		return -1;
	}
	value = kv_iter.value();
	unsigned long long addr = value.toULongLong(NULL, 16);

	if (pid == 0) {
		UTER_LOG("Thread break with wrong thread id %d.", pid);
		return -1;
	}

	TargetThread *thread = threadGetById(pid);
	if (thread == NULL) {
		UTER_LOG("No thread info for thread %d.", pid);
		return -1;
	}

	thread->m_stat = TargetThread::STATUS_STOPED;
	thread->m_stop_by = TargetThread::STOPBY_BREAK;
	thread->m_brk_addr = addr;
	postStopEvent(thread);
	return 0;
}

int Target::doEventStep(QString &event, QString &event_head)
{
	QStringRef event_content = event.midRef(event_head.length());
	QMap<QString,QString> kv_map = msgToMap(event_content);
	QMap<QString,QString>::iterator kv_iter;
	kv_iter = kv_map.find("pid");
	if (kv_iter == kv_map.end()) {
		return -1;
	}
	QString value = kv_iter.value();
	unsigned int pid = value.toUInt(NULL, 10);

	if (pid == 0) {
		UTER_LOG("Thread step with wrong thread id %d.", pid);
		return -1;
	}

	TargetThread *thread = threadGetById(pid);
	if (thread == NULL) {
		UTER_LOG("No thread info for thread %d.", pid);
		return -1;
	}

	/* Some times, threads need through a triggered breakpoint,
	 * do step with STATUS_RUNNING or STATUS_RUNSTEP,
	 * when step triggered, STATUS_RUNNING threads do step normally,
	 * STATUS_RUNSTEP threads turn status to STATUS_PAUSE,
	 * if all threads not running, do continue for continue ,
	 * or continue for trigger event which happened when through a breakpoint*/
	if (thread->m_stat == TargetThread::STATUS_RUNSTEP) {
		/*As STATUS_PAUSE*/
		thread->m_stat = TargetThread::STATUS_PAUSE;
		thread->m_stop_by = TargetThread::STOPBY_PAUSE;
		thread->m_triggered = false;
		/*When all through breakpoint thread stopped, continue target*/
		bool bresult = threadsStatInRange(TargetThread::STATUSSET_NOTRUN);
		if (bresult) {
			ctrlContinue();
		}
		return 0;
	}

	thread->m_stat = TargetThread::STATUS_STOPED;
	thread->m_stop_by = TargetThread::STOPBY_STEPD;
	postStopEvent(thread);
	return 0;
}

int Target::doEventSyscall(QString &event, QString &event_head)
{
	QStringRef event_content = event.midRef(event_head.length());
	QMap<QString,QString> kv_map = msgToMap(event_content);
	QMap<QString,QString>::iterator kv_iter;
	kv_iter = kv_map.find("pid");
	if (kv_iter == kv_map.end()) {
		return -1;
	}
	QString value = kv_iter.value();
	unsigned int pid = value.toUInt(NULL, 10);

	if (pid == 0) {
		UTER_LOG("Thread step with wrong thread id %d.", pid);
		return -1;
	}

	if (pid == 0) {
		UTER_LOG("Thread step with wrong thread id %d.", pid);
		return 0;
	}

	TargetThread *thread = threadGetById(pid);
	if (thread == NULL) {
		UTER_LOG("No thread info for thread %d.", pid);
		return 0;
	}

	thread->m_stat = TargetThread::STATUS_STOPED;
	thread->m_stop_by = TargetThread::STOPBY_SYSCALLD;
	postStopEvent(thread);
	return 0;
}

int Target::doEventStdOut(QString &event, QString &event_head)
{
	QString event_content = event.mid(event_head.length());
	QString decode = msgDecode(event_content);
	emit g_ui_main.signalTargetOutput(1, decode);
	return 0;
}

int Target::doEvent(QString &event)
{
	if (event.indexOf("event://startok?") == 0) {
		QString event_head = "event://startok?";
		return doEventStartOk(event, event_head);
	} else if (event.indexOf("event://startfail?") == 0) {
		QString event_head = "event://startfail?";
		return doEventStartFail(event, event_head);
	} else if (event.indexOf("event://threadstart?") == 0) {
		QString event_head = "event://threadstart?";
		return doEventThreadStart(event, event_head);
	} else if (event.indexOf("event://threadend?") == 0) {
		QString event_head = "event://threadend?";
		return doEventThreadEnd(event, event_head);
	} else if (event.indexOf("event://threadkilled?") == 0) {
		QString event_head = "event://threadkilled?";
		return doEventThreadKilled(event, event_head);
	} else if (event.indexOf("event://threadexited?") == 0) {
		QString event_head = "event://threadexited?";
		return doEventThreadExited(event, event_head);
	} else if (event.indexOf("event://fork?") == 0) {
		QString event_head = "event://fork?";
		return doEventFork(event, event_head);
	} else if (event.indexOf("event://exec?") == 0) {
		QString event_head = "event://exec?";
		return doEventExec(event, event_head);
	} else if (event.indexOf("event://stop?") == 0) {
		QString event_head = "event://stop?";
		return doEventStop(event, event_head);
	} else if (event.indexOf("event://signal?") == 0) {
		QString event_head = "event://signal?";
		return doEventSignal(event, event_head);
	} else if (event.indexOf("event://break?") == 0) {
		QString event_head = "event://break?";
		return doEventBreak(event, event_head);
	} else if (event.indexOf("event://step?") == 0) {
		QString event_head = "event://step?";
		return doEventStep(event, event_head);
	} else if (event.indexOf("event://syscall?") == 0) {
		QString event_head = "event://syscall?";
		return doEventSyscall(event, event_head);
	} else if (event.indexOf("event://stdout?") == 0) {
		QString event_head = "event://stdout?";
		return doEventStdOut(event, event_head);
	} else {
		UTER_LOG("Unknown event %s.", event.toStdString().c_str());
		return 0;
	}

	return 0;
}

/*Convert string to kv list*/
QList<Target::ListNode> Target::msgToList(QStringRef content)
{
	QList<Target::ListNode> kv_list;

	QVector<QStringRef> kv_vector = content.split("&", QString::SkipEmptyParts);
	QVector<QStringRef>::Iterator iter;
	for (iter = kv_vector.begin(); iter != kv_vector.end(); iter++) {
		QStringRef kv = *iter;
		int find_at = kv.indexOf("=");
		if (find_at < 0) {
			continue;
		}
		Target::ListNode listnode;
		listnode.key = kv.mid(0, find_at).toString();
		listnode.value = kv.mid(find_at + 1).toString();
		kv_list.push_back(listnode);
	}

	return kv_list;
}

/*Convert string to kv map*/
QMap<QString,QString> Target::msgToMap(QStringRef content)
{
	QMap<QString,QString> kv_map;

	QVector<QStringRef> kv_vector = content.split("&", QString::SkipEmptyParts);
	QVector<QStringRef>::Iterator iter;
	for (iter = kv_vector.begin(); iter != kv_vector.end(); iter++) {
		QStringRef kv = *iter;
		int find_at = kv.indexOf("=");
		if (find_at < 0) {
			continue;
		}
		QString key = kv.mid(0, find_at).toString();
		QString value = kv.mid(find_at + 1).toString();
		kv_map.insert(key, value);
	}

	return kv_map;
}

/*Encode message*/
QString Target::msgEncode(QString input)
{
	QString retstring;
	retstring = input.replace("\\", "\\\\");
	retstring = input.replace("?", "\\?");
	retstring = input.replace("=", "\\=");
	retstring = input.replace("&", "\\&");
	return retstring;
}

/*Decode message*/
QString Target::msgDecode(QString input)
{
	QString retstring;
	retstring = input.replace("\\&", "&");
	retstring = input.replace("\\=", "=");
	retstring = input.replace("\\?", "?");
	retstring = input.replace("\\\\", "\\");
	return retstring;
}

Target *g_target = NULL;
