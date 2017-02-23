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

#include <Python.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <uter_public.h>
#include <target.hxx>
#include <tester.hxx>
#include <tester_python.h>

#define STACKBYTEUSE(size, align) ((((size) - 1)/(align) + 1) * (align))

int UterMalloc(unsigned int pid, unsigned int len, unsigned long long *point)
{
	UTER_ASSERT(point != NULL);
	int result;
	QByteArray bytes;

	if (len == 0) {
		*point = 0;
		return 0;
	}

	Arch *arch = g_target->archInfoGet();
	if (arch == NULL) {
		UTER_LOG("Get architecture specification failed.");
		return -1;
	}

	unsigned long long malloc_fun_addr;
	unsigned long long free_fun_addr;
	result = g_target->mallocFreeGet(malloc_fun_addr, free_fun_addr);
	if (malloc_fun_addr == 0) {
		UTER_LOG("Get malloc address failed.");
		return -1;
	}

	UTER_DEBUG("Call malloc at 0x%016llx with thread %d, alloc %d bytes",
				malloc_fun_addr, pid, len);

	unsigned long long stack_at;
	result = g_tester->registerGet(pid, arch->m_stack->m_name, stack_at);
	if (result < 0) {
		UTER_LOG("Get stack of thread %d failed.", pid);
		return -1;
	}

	/*Set argument*/
	stack_at -= arch->m_pointlen;
	g_target->dataToBytes(len, arch->m_pointlen, bytes);
	result = g_tester->memorySet(stack_at, arch->m_pointlen, bytes);
	if (result < 0) {
		UTER_LOG("Set call argument failed.");
		return -1;
	}

	unsigned long long esp_when_ret = stack_at;

	RegContext context;
	arch->regContextSave(pid, context);

	/*Push eip*/
	stack_at -= arch->m_pointlen;
	unsigned long long eip_value;
	result = g_tester->registerGet(pid, "eip", eip_value);
	if (result < 0) {
		arch->regContextRestore(pid, context);
		UTER_DEBUG("Set return address failed.");
		return -1;
	}
	g_target->dataToBytes(eip_value, arch->m_pointlen, bytes);
	result = g_tester->memorySet(stack_at, arch->m_pointlen, bytes);
	if (result < 0) {
		arch->regContextRestore(pid, context);
		UTER_DEBUG("Set return address failed.");
		return -1;
	}

	/*Set new eip*/
	result = g_tester->registerSet(pid, "eip", malloc_fun_addr);
	if (result < 0) {
		arch->regContextRestore(pid, context);
		UTER_DEBUG("Set function address failed.");
		return -1;
	}

	/*Save stack before call*/
	result = g_tester->registerSet(pid, arch->m_stack->m_name, stack_at);
	if (result < 0) {
		arch->regContextRestore(pid, context);
		UTER_DEBUG("Save stack register failed.");
		return -1;
	}

	TesterStack interstack(TesterStack::STACK_CALL, pid, esp_when_ret, eip_value);
	g_tester->stackPush(&interstack);

	/*Call and wait return*/
	g_tester->callWaitReturn(&interstack);

	TesterStack *pinterstack = g_tester->stackPop();
	UTER_ASSERT(pinterstack == &interstack);

	UTER_DEBUG("Call malloc at 0x%016llx with thread %d return, alloc %d bytes ",
				malloc_fun_addr, pid, len);

	unsigned long long eax_value;
	result = g_tester->registerGet(pid, "eax", eax_value);
	if (result < 0) {
		arch->regContextRestore(pid, context);
		UTER_DEBUG("Set return value failed.");
		return -1;
	}
	*point = eax_value;

	arch->regContextRestore(pid, context);
	return 0;
}

int UterFree(unsigned int pid, unsigned long long point)
{
	UTER_ASSERT(point != 0);
	int result;
	QByteArray bytes;

	Arch *arch = g_target->archInfoGet();
	if (arch == NULL) {
		UTER_LOG("Get architecture specification failed.");
		return -1;
	}

	unsigned long long malloc_fun_addr;
	unsigned long long free_fun_addr;
	result = g_target->mallocFreeGet(malloc_fun_addr, free_fun_addr);
	if (free_fun_addr == 0) {
		UTER_LOG("Get malloc address failed.");
		return -1;
	}

	UTER_DEBUG("Call free at 0x%016llx with thread %d, free bytes at 0x%016llx.",
				free_fun_addr, pid, point);

	unsigned long long stack_at;
	result = g_tester->registerGet(pid, arch->m_stack->m_name, stack_at);
	if (result < 0) {
		UTER_LOG("Get stack of thread %d failed.", pid);
		return -1;
	}

	/*Set argument*/
	stack_at -= arch->m_pointlen;
	g_target->dataToBytes(point, arch->m_pointlen, bytes);
	result = g_tester->memorySet(stack_at, arch->m_pointlen, bytes);
	if (result < 0) {
		UTER_LOG("Set call argument failed.");
		return -1;
	}

	unsigned long long esp_when_ret = stack_at;

	RegContext context;
	arch->regContextSave(pid, context);

	/*Push eip*/
	stack_at -= arch->m_pointlen;
	unsigned long long eip_value;
	result = g_tester->registerGet(pid, "eip", eip_value);
	if (result < 0) {
		arch->regContextRestore(pid, context);
		UTER_DEBUG("Set return address failed.");
		return -1;
	}
	g_target->dataToBytes(eip_value, arch->m_pointlen, bytes);
	result = g_tester->memorySet(stack_at, arch->m_pointlen, bytes);
	if (result < 0) {
		arch->regContextRestore(pid, context);
		UTER_DEBUG("Set return address failed.");
		return -1;
	}

	/*Set new eip*/
	result = g_tester->registerSet(pid, "eip", free_fun_addr);
	if (result < 0) {
		arch->regContextRestore(pid, context);
		UTER_DEBUG("Set function address failed.");
		return -1;
	}

	/*Save stack before call*/
	result = g_tester->registerSet(pid, arch->m_stack->m_name, stack_at);
	if (result < 0) {
		arch->regContextRestore(pid, context);
		UTER_DEBUG("Save stack register failed.");
		return -1;
	}

	TesterStack interstack(TesterStack::STACK_CALL, pid, esp_when_ret, eip_value);
	g_tester->stackPush(&interstack);

	/*Call and wait return*/
	g_tester->callWaitReturn(&interstack);

	TesterStack *pinterstack = g_tester->stackPop();
	UTER_ASSERT(pinterstack == &interstack);

	UTER_DEBUG("Call free at 0x%016llx with thread %d return, free bytes at 0x%016llx.",
				free_fun_addr, pid, point);

	arch->regContextRestore(pid, context);
	return 0;
}

int TestCall(unsigned int pid, unsigned long long fun_addr, CallFunDesc *fun_desc)
{
	UTER_ASSERT(fun_desc != NULL);
	int result;
	QByteArray bytes;

	Arch *arch = g_target->archInfoGet();
	if (arch == NULL) {
		UTER_LOG("Get architecture specification failed.");
		return -1;
	}

	UTER_DEBUG("Call function 0x%016llx with thread %d.", fun_addr, pid);

	unsigned long long stack_at;
	result = g_tester->registerGet(pid, arch->m_stack->m_name, stack_at);
	if (result < 0) {
		UTER_LOG("Get stack of thread %d failed.", pid);
		return -1;
	}

	/*Set arguments*/
	QList<CallDataDesc*>::iterator iter;
	for (iter = fun_desc->m_args.end(); iter != fun_desc->m_args.begin(); /*none*/) {
		CallDataDesc *arg_desc = *(--iter);
		stack_at -= STACKBYTEUSE(arg_desc->m_len, arch->m_stack_align);
		result = g_tester->memoryGet(arg_desc->m_addr, arg_desc->m_len, bytes);
		if (result < 0) {
			UTER_LOG("Set call argument failed.");
			return -1;
		}
		result = g_tester->memorySet(stack_at, arg_desc->m_len, bytes);
		if (result < 0) {
			UTER_LOG("Set call argument failed.");
			return -1;
		}
	}

	unsigned long long esp_when_ret = stack_at;
	if (fun_desc->m_call_type == CallFunDesc::CALL_RET_POINT) {
		stack_at -= arch->m_pointlen;

		g_target->dataToBytes(fun_desc->m_ret.m_addr, arch->m_pointlen, bytes);
		result = g_tester->memorySet(stack_at, arch->m_pointlen, bytes);
		if (result < 0) {
			UTER_LOG("Set call argument failed.");
			return -1;
		}
	}

	RegContext context;
	arch->regContextSave(pid, context);

	/*Push eip*/
	stack_at -= arch->m_pointlen;
	unsigned long long eip_value;
	result = g_tester->registerGet(pid, "eip", eip_value);
	if (result < 0) {
		arch->regContextRestore(pid, context);
		UTER_DEBUG("Set return address failed.");
		return -1;
	}
	g_target->dataToBytes(eip_value, arch->m_pointlen, bytes);
	result = g_tester->memorySet(stack_at, arch->m_pointlen, bytes);
	if (result < 0) {
		arch->regContextRestore(pid, context);
		UTER_DEBUG("Set return address failed.");
		return -1;
	}

	/*Set new eip*/
	result = g_tester->registerSet(pid, "eip", fun_addr);
	if (result < 0) {
		arch->regContextRestore(pid, context);
		UTER_DEBUG("Set function address failed.");
		return -1;
	}

	/*Save stack before call*/
	result = g_tester->registerSet(pid, arch->m_stack->m_name, stack_at);
	if (result < 0) {
		arch->regContextRestore(pid, context);
		UTER_DEBUG("Save stack register failed.");
		return -1;
	}

	TesterStack interstack(TesterStack::STACK_CALL, pid, esp_when_ret, eip_value);
	g_tester->stackPush(&interstack);

	/*Call and wait return*/
	g_tester->callWaitReturn(&interstack);

	TesterStack *pinterstack = g_tester->stackPop();
	UTER_ASSERT(pinterstack == &interstack);

	UTER_DEBUG("Call function 0x%016llx with thread %d return.", fun_addr, pid);

	/*Do result*/
	if (fun_desc->m_call_type == CallFunDesc::CALL_RET_EAX) {
		unsigned long long eax_value;
		result = g_tester->registerGet(pid, "eax", eax_value);
		if (result < 0) {
			arch->regContextRestore(pid, context);
			UTER_DEBUG("Set return value failed.");
			return -1;
		}
		g_target->dataToBytes(eax_value, arch->m_pointlen, bytes);
		result = g_tester->memorySet(fun_desc->m_ret.m_addr, arch->m_pointlen, bytes);
		if (result < 0) {
			arch->regContextRestore(pid, context);
			UTER_DEBUG("Set return value failed.");
			return -1;
		}
	} else if (fun_desc->m_call_type == CallFunDesc::CALL_RET_EAXEDX)  {
		unsigned long long eax_value;
		result = g_tester->registerGet(pid, "eax", eax_value);
		if (result < 0) {
			arch->regContextRestore(pid, context);
			UTER_DEBUG("Set return value failed.");
			return -1;
		}
		QByteArray eax_bytes;
		g_target->dataToBytes(eax_value, arch->m_pointlen, eax_bytes);

		unsigned long long edx_value;
		result = g_tester->registerGet(pid, "edx", edx_value);
		if (result < 0) {
			arch->regContextRestore(pid, context);
			UTER_DEBUG("Set return value failed.");
			return -1;
		}
		QByteArray edx_bytes;
		g_target->dataToBytes(edx_value, arch->m_pointlen, edx_bytes);

		eax_bytes += edx_bytes;
		result = g_tester->memorySet(fun_desc->m_ret.m_addr, arch->m_pointlen * 2, eax_bytes);
		if (result < 0) {
			arch->regContextRestore(pid, context);
			UTER_DEBUG("Set return value failed.");
			return -1;
		}
	} else if (fun_desc->m_call_type == CallFunDesc::CALL_RET_VOID
			|| fun_desc->m_call_type == CallFunDesc::CALL_RET_POINT) {
		;
	} else {
		UTER_ASSERT(0);
	}

	arch->regContextRestore(pid, context);
	return 0;
}

/*Called in tester/python thread*/
int TestStub(unsigned long long addr, unsigned int tid)
{
	UTER_DEBUG("Stub action at 0x%016llx on thread %d.", addr, tid);
	int result;

	Arch *arch = g_target->archInfoGet();
	if (arch == NULL) {
		UTER_LOG("Get architecture specification failed.");
		return -1;
	}

	g_tester->m_data_lock.lock();
	QMap<unsigned long long, TesterStub*>::iterator stub_iter;
	stub_iter = g_tester->m_stub_map.find(addr);
	if (stub_iter == g_tester->m_stub_map.end()) {
		g_tester->m_data_lock.unlock();
		UTER_LOG("No stub at 0x%016llx for thread %d", addr, tid);
		return -1;
	}
	TesterStub *stub = stub_iter.value();
	g_tester->m_data_lock.unlock();

	PyObject *dict_obj = stub->m_driver_arg_obj;
	UTER_ASSERT(dict_obj != NULL);
	UTER_ASSERT(PyDict_Check(dict_obj));
	PyObject *driver_obj = PyDict_GetItemString(dict_obj, "driver");
	if (driver_obj == NULL || !PyFunction_Check(driver_obj)) {
		UTER_LOG("Stub dict object no function on key named %s.", "driver");
		return -1;
	}

	/*Prepare call*/
	PyObject *call_arg_obj = PyTuple_New(2);
	if (call_arg_obj == NULL) {
		UTER_LOG("Make stub driver argument failed.");
		return -1;
	}

	/*index 0 is stub dict object self*/
	Py_INCREF(dict_obj);
	result = PyTuple_SetItem(call_arg_obj, 0, dict_obj);
	if (result < 0) {
		Py_DECREF(dict_obj);
		Py_DECREF(call_arg_obj);
		UTER_LOG("Make stub driver argument failed.");
		return -1;
	}

	PyObject *arg_list = PyList_New(0);
	if (arg_list == NULL) {
		Py_DECREF(call_arg_obj);
		UTER_LOG("Make stub driver function call argument list failed.");
		return -1;
	}
	/*index 1 is stub function arguments description*/
	result = PyTuple_SetItem(call_arg_obj, 1, arg_list);
	if (result < 0) {
		Py_DECREF(arg_list);
		Py_DECREF(call_arg_obj);
		UTER_LOG("Make stub driver function call argument list failed.");
		return -1;
	}

	unsigned long long stack_at;
	result = g_tester->registerGet(tid, arch->m_stack->m_name, stack_at);
	if (result < 0) {
		Py_DECREF(call_arg_obj);
		UTER_LOG("Get stack of thread %d failed.", tid);
		return -1;
	}

	/*Return address save at*/
	unsigned int pos = stack_at + arch->m_pointlen;

	CallFunDesc *fun_desc = stub->m_fun_desc;

	unsigned long long point_ret_addr = 0;		/*just use for CALL_RET_POINT*/
	if (fun_desc->m_call_type == CallFunDesc::CALL_RET_POINT) {
		QByteArray array;
		result = g_tester->memoryGet(pos, arch->m_pointlen, array);
		if (result < 0) {
			Py_DECREF(call_arg_obj);
			UTER_LOG("Get memory for return point failed.");
			return -1;
		}
		g_target->bytesToData(point_ret_addr, arch->m_pointlen, array);
		pos += arch->m_pointlen;
	}

	QList<CallDataDesc*>::iterator iter;
	for (iter = fun_desc->m_args.begin(); iter != fun_desc->m_args.end(); iter++) {
		CallDataDesc *arg_desc = *iter;

		PyObject *arg_dict = PyDict_New();
		if (arg_dict == NULL) {
			Py_DECREF(call_arg_obj);
			UTER_LOG("Make call argument failed.");
			return -1;
		}
		result = PyList_Append(arg_list, arg_dict);
		Py_DECREF(arg_dict);
    	if (result < 0) {
			Py_DECREF(call_arg_obj);
			UTER_LOG("Make call argument failed.");
			return -1;
    	}

		PyObject *addr_obj = PyLong_FromUnsignedLongLong(pos);
		if (addr_obj == NULL) {
			Py_DECREF(call_arg_obj);
			UTER_LOG("Make call argument failed.");
			return -1;
		}
		result = PyDict_SetItemString(arg_dict, "address", addr_obj);
		Py_DECREF(addr_obj);
        if (result < 0) {
			Py_DECREF(call_arg_obj);
			UTER_LOG("Make call argument failed.");
			return -1;
        }

		pos += STACKBYTEUSE(arg_desc->m_len, arch->m_stack_align);
	}

	QByteArray array;

	TesterStack interstack(TesterStack::STACK_STUB, tid);
	g_tester->stackPush(&interstack);

	/*Call stub driver function, ret_obj with 1 reference*/
	PyObject *call_ret_obj = PyEval_CallObject(driver_obj, call_arg_obj);
	Py_DECREF(call_arg_obj);

	TesterStack *pinterstack = g_tester->stackPop();
	UTER_ASSERT(pinterstack == &interstack);

	if (call_ret_obj == NULL) {
		UTER_DEBUG("Stub call return wrong object.");
		return -1;
	}
	if (!PyDict_Check(call_ret_obj)) {
		Py_DECREF(call_ret_obj);
		UTER_DEBUG("Stub call return wrong object.");
		return -1;
	}

	PyObject *head_obj;
	head_obj = PyDict_GetItemString(call_ret_obj, "head");
	if (head_obj == NULL || !PyString_Check(head_obj)) {
		Py_DECREF(call_ret_obj);
		UTER_DEBUG("Stub call return object no key named %s.", "head");
		return -1;
	}
	char *head_str;
	head_str = PyString_AsString(head_obj);
	if (head_str == NULL) {
		Py_DECREF(call_ret_obj);
		UTER_DEBUG("Stub call return object with unknown head.");
		return -1;
	}

	/*difference type of return object*/
	if (strcmp(head_str, "success") == 0) {
		UTER_DEBUG("Stub running success.");
	} else if (strcmp(head_str, "nomatch") == 0) {
		Py_DECREF(call_ret_obj);
		UTER_LOG("Stub return object not match.");
		return -1;
	} else if (strcmp(head_str, "execption") == 0) {
		Py_DECREF(call_ret_obj);
		UTER_LOG("Stub running execption.");
		return -1;
	} else if (strcmp(head_str, "error") == 0) {
		Py_DECREF(call_ret_obj);
		UTER_LOG("Stub running error.");
		return -1;
	} else {
		Py_DECREF(call_ret_obj);
		UTER_LOG("Stub unknown error.");
		return -1;
	}

	/*return value for head == success*/
	PyObject *retdesc_obj;
	retdesc_obj = PyDict_GetItemString(call_ret_obj, "body");
	if (retdesc_obj == NULL || !PyDict_Check(retdesc_obj)) {
		Py_DECREF(call_ret_obj);
		UTER_DEBUG("Return object body error.");
		return -1;
	}

	/*address*/
	PyObject *ret_addr_obj;
	ret_addr_obj = PyDict_GetItemString(retdesc_obj, "address");
	if (ret_addr_obj == NULL || !PyLong_Check(ret_addr_obj)) {
		Py_DECREF(call_ret_obj);
		UTER_DEBUG("Address wrong of return object.");
		return -1;
	}
	unsigned long long ret_addr = PyLong_AsUnsignedLongLong(ret_addr_obj);

	/*length*/
	PyObject *ret_len_obj;
	ret_len_obj = PyDict_GetItemString(retdesc_obj, "len");
	if (ret_len_obj == NULL || !PyInt_Check(ret_len_obj)) {
		Py_DECREF(call_ret_obj);
		UTER_DEBUG("Length wrong of return object.");
		return -1;
	}
	unsigned int ret_len = PyInt_AsSsize_t(ret_len_obj);

	/*Release return object*/
	Py_DECREF(call_ret_obj);

	/*Do return*/
	unsigned long long eip_val;
	result = g_tester->memoryGet(stack_at, arch->m_pointlen, array);
	if (result < 0) {
		UTER_DEBUG("Get return address failed.");
		return -1;
	}
	g_target->bytesToData(eip_val, arch->m_pointlen, array);
	result = g_tester->registerSet(tid, "eip", eip_val);
	if (result < 0) {
		UTER_DEBUG("Set return failed.");
		return -1;
	}

	stack_at += arch->m_pointlen;

	/*Set return value*/
	if (fun_desc->m_call_type == CallFunDesc::CALL_RET_EAX) {
		unsigned long long eax_val;
		result = g_tester->memoryGet(ret_addr, arch->m_pointlen, array);
		if (result < 0) {
			UTER_DEBUG("Set return value failed.");
			return -1;
		}
		g_target->bytesToData(eax_val, arch->m_pointlen, array);
		result = g_tester->registerSet(tid, "eax", eax_val);
		if (result < 0) {
			UTER_DEBUG("Set return value failed.");
			return -1;
		}
	} else if (fun_desc->m_call_type == CallFunDesc::CALL_RET_EAXEDX) {
		unsigned long long eax_val;
		result = g_tester->memoryGet(ret_addr, arch->m_pointlen, array);
		if (result < 0) {
			UTER_DEBUG("Set return value failed.");
			return -1;
		}
		g_target->bytesToData(eax_val, arch->m_pointlen, array);
		result = g_tester->registerSet(tid, "eax", eax_val);
		if (result < 0) {
			UTER_DEBUG("Set return value failed.");
			return -1;
		}

		unsigned long long edx_val;
		result = g_tester->memoryGet(ret_addr + arch->m_pointlen, arch->m_pointlen, array);
		if (result < 0) {
			UTER_DEBUG("Set return value failed.");
			return -1;
		}
		g_target->bytesToData(edx_val, arch->m_pointlen, array);
		result = g_tester->registerSet(tid, "edx", edx_val);
		if (result < 0) {
			UTER_DEBUG("Set return value failed.");
			return -1;
		}
	} else if (fun_desc->m_call_type == CallFunDesc::CALL_RET_VOID) {
		;
	} else if (fun_desc->m_call_type == CallFunDesc::CALL_RET_POINT) {
		stack_at += arch->m_pointlen;

		result = g_tester->memoryGet(ret_addr, ret_len, array);
		if (result < 0) {
			UTER_DEBUG("Set return value failed.");
			return -1;
		}
		result = g_tester->memorySet(point_ret_addr, ret_len, array);
		if (result < 0) {
			UTER_DEBUG("Set return value failed.");
			return -1;
		}
	} else {
		UTER_ASSERT(0);
	}

	result = g_tester->registerSet(tid, arch->m_stack->m_name, stack_at);
	if (result < 0) {
		UTER_DEBUG("Set return value failed.");
		return -1;
	}

	return 0;
}

/*Get a thread for call*/
unsigned int Tester::callSelectThread()
{
	/*If some thread doing stub, it is the preferred thread for call*/
	m_data_lock.lock();
	QList<TesterStack*>::iterator stack_iter;
	for (stack_iter = m_inter_stack.end(); stack_iter != m_inter_stack.begin(); /*none*/) {
		TesterStack *stack = *(--stack_iter);
		if (stack->m_type == TesterStack::STACK_STUB) {
			unsigned int pid = stack->m_pid;
			m_data_lock.unlock();
			return pid;
		}
	}
	m_data_lock.unlock();

	unsigned int return_pid = UTER_INVALID_PID;
	TargetThreadsArg threads;
	emit signalTargetThreads(&threads);
	if (! threads.m_success) {
		UTER_LOG("Get thread information failed.");
		return return_pid;
	}
	QList<TargetThreadsArg::TargetThreadArg*>::iterator thread_iter;
	for (thread_iter = threads.m_thread_list.begin();
		 thread_iter != threads.m_thread_list.end(); thread_iter++) {
		TargetThreadsArg::TargetThreadArg *info = *thread_iter;
		if (info->m_in_vdso) {
			continue;
		}
		/*Get minimal pid*/
		if (info->m_pid < return_pid) {
			return_pid = info->m_pid;
		}
	}
	return return_pid;
}

Tester::Tester()
:QThread(),
 m_data_lock(QMutex::Recursive)
{
	QMetaObject::Connection conn;

	/*Attach to thread*/
	moveToThread(this);

	m_python_ready = true;
	m_py_output = NULL;

	/*Operate target with signal*/
    conn = connect(this, SIGNAL(signalTargetThreads(TargetThreadsArg*)),
    		       g_target, SLOT(slotThreads(TargetThreadsArg*)),
				   Qt::BlockingQueuedConnection);
    UTER_ASSERT(conn != NULL);
    conn = connect(this, SIGNAL(signalTargetMemGet(TargetMemGetArg*)),
    		       g_target, SLOT(slotMemGet(TargetMemGetArg*)),
				   Qt::BlockingQueuedConnection);
    UTER_ASSERT(conn != NULL);
    conn = connect(this, SIGNAL(signalTargetMemSet(TargetMemSetArg*)),
    		       g_target, SLOT(slotMemSet(TargetMemSetArg*)),
				   Qt::BlockingQueuedConnection);
    UTER_ASSERT(conn != NULL);
    conn = connect(this, SIGNAL(signalTargetRegGet(TargetRegGetArg*)),
    		       g_target, SLOT(slotRegGet(TargetRegGetArg*)),
				   Qt::BlockingQueuedConnection);
    UTER_ASSERT(conn != NULL);
    conn = connect(this, SIGNAL(signalTargetRegSet(TargetRegSetArg*)),
    		       g_target, SLOT(slotRegSet(TargetRegSetArg*)),
				   Qt::BlockingQueuedConnection);
    UTER_ASSERT(conn != NULL);
    conn = connect(this, SIGNAL(signalTargetCovSet(TargetFunCovArg*)),
    		       g_target, SLOT(slotFunCovSet(TargetFunCovArg*)),
				   Qt::BlockingQueuedConnection);
    UTER_ASSERT(conn != NULL);
    conn = connect(this, SIGNAL(signalTargetContinue(unsigned int)),
    		       g_target, SLOT(slotContinue(unsigned int)));
    UTER_ASSERT(conn != NULL);
	return;
}

int Tester::memoryGet(unsigned long long addr, unsigned int size, QByteArray &array)
{
	UTER_ASSERT(size != 0);
	array.clear();
	TargetMemGetArg mem_get_arg;
	mem_get_arg.m_addr = addr;
	mem_get_arg.m_size = size;
	mem_get_arg.m_success = false;
	emit signalTargetMemGet(&mem_get_arg);
	if (! mem_get_arg.m_success) {
		return -1;
	}
	array = mem_get_arg.m_array;
	return 0;
}

int Tester::memorySet(unsigned long long addr, unsigned int size, QByteArray &array)
{
	UTER_ASSERT(size != 0);
	UTER_ASSERT(size == array.size());
	TargetMemSetArg mem_set_arg;
	mem_set_arg.m_addr = addr;
	mem_set_arg.m_size = size;
	mem_set_arg.m_array = array;
	mem_set_arg.m_success = false;
	emit signalTargetMemSet(&mem_set_arg);
	if (! mem_set_arg.m_success) {
		return -1;
	}
	return 0;
}

int Tester::registerGet(unsigned int pid, QString name, unsigned long long &value)
{
	TargetRegGetArg reg_get_arg;
	reg_get_arg.m_pid = pid;
	reg_get_arg.m_name = name;
	reg_get_arg.m_success = false;
	emit signalTargetRegGet(&reg_get_arg);
	if (! reg_get_arg.m_success) {
		return -1;
	}
	value = reg_get_arg.m_value;
	return 0;
}

int Tester::registerSet(unsigned int pid, QString name, unsigned long long value)
{
	TargetRegSetArg reg_set_arg;
	reg_set_arg.m_pid = pid;
	reg_set_arg.m_name = name;
	reg_set_arg.m_value = value;
	reg_set_arg.m_success = false;
	emit signalTargetRegSet(&reg_set_arg);
	if (! reg_set_arg.m_success) {
		return -1;
	}
	return 0;
}

int Tester::pythonStdOut(QString str)
{
	string input_str = str.toStdString();
	const char *input_char = input_str.c_str();
	::write(m_py_pipe_stdout[1], input_char, strlen(input_char));
	::fsync(m_py_pipe_stdout[1]);
	return 0;
}

int Tester::pythonStdErr(QString str)
{
	string input_str = str.toStdString();
	const char *input_char = input_str.c_str();
	::write(m_py_pipe_stderr[1], input_char, strlen(input_char));
	::fsync(m_py_pipe_stderr[1]);
	return 0;
}

int Tester::pythonInit()
{
	int result;

	/*Get application directory*/
	char buf[PATH_MAX] = {0};
	result = readlink("/proc/self/exe", buf, PATH_MAX);
	if (result == -1 || result == PATH_MAX) {
		UTER_ASSERT(0);
		return -1;
	}
	QString app_path = QString::fromLatin1(buf);
	QFileInfo fileinfo(app_path);
	QString app_dir = fileinfo.path();

	/*Make pipe, one end for python interpreter, one end for output thread*/
	result = pipe(m_py_pipe_stdout);
	UTER_ASSERT(result == 0);
	result = pipe(m_py_pipe_stderr);
	UTER_ASSERT(result == 0);

	/*Start thread with pipe handle*/
	m_py_output = new PyOutputTask(m_py_pipe_stdout[0], m_py_pipe_stderr[0]);
	m_py_output->start();

	/*Configure python interpreter initialization*/
	setenv("PYTHONDONTWRITEBYTECODE", "1", 1);
	setenv("PYTHONHOME", (app_dir + "/../lib/Python-2.7.9/").toStdString().c_str(), 1);

	/*Do python interpreter initialization*/
	Py_Initialize();

	/*Change python interpreter output to pipe from stdio*/
	FILE *stdout_file = fdopen(m_py_pipe_stdout[1], "w");
	UTER_ASSERT(stdout_file != NULL);
	PyObject *stdout_obj = PyFile_FromFile(stdout_file, (char *)"<uter-stdout>", (char *)"w", NULL);
	PyFile_SetBufSize(stdout_obj, 0);
	PySys_SetObject((char *)"stdout", stdout_obj);

	FILE *stderr_file = fdopen(m_py_pipe_stderr[1], "w");
	UTER_ASSERT(stderr_file != NULL);
	PyObject *stderr_obj = PyFile_FromFile(stderr_file, (char *)"<uter-stderr>", (char *)"w", NULL);
	PyFile_SetBufSize(stderr_obj, 0);
	PySys_SetObject((char *)"stderr", stderr_obj);

	/*Uter python extern initialization*/
	Py_InitModule("UterExtern", UterPythonExternMethods);

	/*Set module search path*/
	PyRun_SimpleString("import sys");
	PyRun_SimpleString(("sys.path.append(\"" + app_dir + "/../python/\")").toStdString().c_str());

	/*Import critical module*/
	PyRun_SimpleString("import uter");

	return 0;
}

/*Tester thread run*/
void Tester::run()
{
	/*Set thread name*/
	prctl(PR_SET_NAME, (unsigned long)"Tester_Work");
	/*python initialization*/
	pythonInit();
	/*Tester event loop*/
	QThread::exec();
}

void Tester::stackPush(TesterStack *stacknode)
{
	UTER_ASSERT(stacknode != NULL);
	QMutexLocker locker(&m_data_lock);
	m_inter_stack.push_back(stacknode);
	flushStatus();
	return;
}

TesterStack *Tester::stackPop()
{
	QMutexLocker locker(&m_data_lock);
	if (m_inter_stack.empty()) {
		return NULL;
	}
	TesterStack *retval = m_inter_stack.back();
	m_inter_stack.pop_back();
	flushStatus();
	return retval;
}

/*Do python job or stub/call job on interpreter top*/
int Tester::callWaitReturn(TesterStack *stacknode)
{
	UTER_ASSERT(stacknode != NULL);
	UTER_ASSERT(stacknode->m_type == TesterStack::STACK_CALL);

	UTER_DEBUG("Set call return breakpoint at 0x%016llx.", stacknode->m_data.call.m_retto);
	/*Set breakpoint*/
	int result = g_target->breakInsert(stacknode->m_data.call.m_retto, TargetBreak::BREAK_RET);
	if (result != 0) {
		UTER_LOG("Set return breakpoint on address 0x%016llx failed.", stacknode->m_data.call.m_retto);
		return 0;
	}

	/*Continue target*/
	emit signalTargetContinue(0);

	while(1) {
		m_data_lock.lock();
		if (m_job_list.empty()) {
			m_data_lock.unlock();
			/*Event loop on interpreter top, break it when job happened*/
			TesterEventLoop eventloop(this);
			eventloop.exec();
			continue;
		}
		TesterJob *job = m_job_list.front();
		m_job_list.pop_front();
		m_data_lock.unlock();

		/*Do job*/
		if(job->m_job == TesterJob::JOB_PYTHON) {
			TesterJobPython *python_job = (TesterJobPython *)job;
			UTER_DEBUG("Ignore python command on function call, type %d, string %s.",
						python_job->m_type, python_job->m_string.toStdString().c_str());
		} else if (job->m_job == TesterJob::JOB_TARGET) {
			TesterJobTarget *target_job = (TesterJobTarget *)job;
			if (target_job->m_type == TesterJobTarget::TARGET_STUB) {
				TesterStack interstack(TesterStack::STACK_SCRIPT);
				stackPush(&interstack);

				/*Run stub script on function call*/
				TestStub(target_job->m_at_addr, target_job->m_on_pid);

				TesterStack *pinterstack = g_tester->stackPop();
				UTER_ASSERT(pinterstack == &interstack);

				emit signalTargetContinue(0);
			} else if (target_job->m_type == TesterJobTarget::TARGET_RET) {
				/*Call return*/
				delete job;
				break;
			} else {
				UTER_ASSERT(0);
			}
		} else {
			UTER_ASSERT(0);
		}

		delete job;
	}

	/*Remove break point*/
	UTER_DEBUG("Clear call return breakpoint at 0x%016llx.", stacknode->m_data.call.m_retto);
	result = g_target->breakDelete(stacknode->m_data.call.m_retto, TargetBreak::BREAK_RET);
	if (result != 0) {
		UTER_LOG("Remove return breakpoint on address 0x%016llx failed.", stacknode->m_data.call.m_retto);
		/*no return here*/
	}
	return 0;
}

/*Do python job or stub job on interpreter bottom*/
int Tester::jobEventProc()
{
	while(1) {
		m_data_lock.lock();
		if (m_job_list.empty()) {
			m_data_lock.unlock();
			break;
		}
		TesterJob *job = m_job_list.front();
		m_job_list.pop_front();
		m_data_lock.unlock();

		switch(job->m_job)
		{
		case TesterJob::JOB_PYTHON:
		{
			/*Do python job*/
			TesterStack interstack(TesterStack::STACK_SCRIPT);
			stackPush(&interstack);

			emit signalPythonRun();

			TesterJobPython *python_job = (TesterJobPython *)job;
			switch(python_job->m_type)
			{
			case TesterJobPython::PYTHON_COMMAND:
			{
				QStringList lines = python_job->m_string.split("\n");
				QString statements = "";
				QString line = "";
				QStringList::iterator iter;
				for (iter = lines.begin(); iter != lines.end(); iter++) {
					if (*iter == "") {				/*Empty*/
						continue;
					}

					if (line.right(1) == "\\") {	/*Merge to last line*/
						line = *iter;
						statements += line + "\n";
						continue;
					}

					/*Current line*/
					line = *iter;
					if (line.left(1) == "\t" || line.left(1) == " ") {
						statements += line + "\n";
						continue;
					}

					if (statements != "") {
						/*Execute stock*/
						g_tester->pythonStdOut(statements);
						PyRun_SimpleString(statements.toStdString().c_str());
						statements = "";
					}

					statements += line + "\n";
				}
				if (statements != "") {
					/*Run python before next block, output python command first*/
					g_tester->pythonStdOut(statements);
					PyRun_SimpleString(statements.toStdString().c_str());
					statements = "";
				}

				break;
			}
			case TesterJobPython::PYTHON_TESTFILE:
			{
				/*Test a case file*/
				QString script = "uter.run.file(\"" + python_job->m_string + "\")";
				PyRun_SimpleString(script.toStdString().c_str());
				break;
			}
			case TesterJobPython::PYTHON_TESTDIR:
			{
				/*Test a case directory*/
				QString script = "uter.run.dir(\"" + python_job->m_string + "\")";
				PyRun_SimpleString(script.toStdString().c_str());
				break;
			}
			default:
			{
				UTER_ASSERT(0);
			}
			}

			emit signalPythonFinish();

			TesterStack *pinterstack = g_tester->stackPop();
			UTER_ASSERT(pinterstack == &interstack);

			break;
		}
		case TesterJob::JOB_TARGET:
		{
			/*Do target job*/
			TesterJobTarget *target_job = (TesterJobTarget *)job;
			switch(target_job->m_type)
			{
			case TesterJobTarget::TARGET_RET:
			{
				/*No return job on interpreter bottom*/
				UTER_ASSERT(0);
				break;
			}
			case TesterJobTarget::TARGET_STUB:
			{
				TesterStack interstack(TesterStack::STACK_SCRIPT);
				stackPush(&interstack);

				/*Run stub script*/
				TestStub(target_job->m_at_addr, target_job->m_on_pid);

				TesterStack *pinterstack = g_tester->stackPop();
				UTER_ASSERT(pinterstack == &interstack);
				break;
			}
			default:
			{
				UTER_ASSERT(0);
			}
			}

			break;
		}
		default:
		{
			UTER_ASSERT(0);
		}
		}

		delete job;
	}

	return 0;
}

/*
 * Called by ui(target) thread, return:
 * -1 error, 0 not triggerd, 1 triggerd
 * */
int Tester::targetReturnTriggerd(unsigned long long addr, unsigned int tid, unsigned long long stack)
{
	QMutexLocker locker(&m_data_lock);

	if (m_inter_stack.empty()) {
		return 0;
	}
	TesterStack *topnode = m_inter_stack.back();
	if (topnode->m_type != TesterStack::STACK_CALL) {
		return 0;
	}
	if (!(topnode->m_pid == tid && topnode->m_data.call.m_retto == addr
			                    && topnode->m_data.call.m_stack == stack)) {
		return 0;
	}

	/*Target job*/
	TesterJobTarget *target_job = new TesterJobTarget(TesterJobTarget::TARGET_RET, tid, addr);
	m_job_list.push_back(target_job);
	QApplication::postEvent(this, new TesterEvent(TesterEvent::EVENT_JOB_TARGET));
	return 1;
}

/*
 * Called by ui(target) thread, return:
 * -1 error, 0 not triggerd, 1 triggerd
 * */
int Tester::targetStubTriggerd(unsigned long long addr, unsigned int tid)
{
	QMutexLocker locker(&m_data_lock);

	/*Check stub*/
	QMap<unsigned long long, TesterStub*>::iterator iter;
	iter = m_stub_map.find(addr);
	if (iter == m_stub_map.end()) {
		return 0;
	}

	/*Target job*/
	TesterJobTarget *target_job = new TesterJobTarget(TesterJobTarget::TARGET_STUB, tid, addr);
	m_job_list.push_back(target_job);
	QApplication::postEvent(this, new TesterEvent(TesterEvent::EVENT_JOB_TARGET));
	return 1;
}

/*
 * Called by ui(target) thread, return:
 * -1 error, 0 can not run, 1 run it
 * */
int Tester::pythonRun(TesterJobPython::TesterPythonType type, QString &string)
{
	QMutexLocker locker(&m_data_lock);

	if (! m_inter_stack.empty()) {
		/*interpreter busy*/
		return -1;
	}
	if (! m_job_list.empty()) {
		/*interpreter busy*/
		return -1;
	}

	/*Python job*/
	TesterJobPython *python_job = new TesterJobPython(type, string);
	m_job_list.push_back(python_job);
	QApplication::postEvent(this, new TesterEvent(TesterEvent::EVENT_JOB_PYTHON));
	return 1;
}

/*Get python help form input, after no use now, called in ui thread*/
QStringList Tester::pythonAutoCompletion(QString &input)
{
	QStringList ret_list;

	QMutexLocker locker(&m_data_lock);

	if (input.length() == 0) {
		return ret_list;
	}
	QStringList element_list = input.split(".");

	/*Get list from python*/
	ret_list = UterPython_AutoCompletion(element_list);
	return ret_list;
}

int Tester::flushStatus()
{
	int python_ready = m_inter_stack.empty();

	if (m_python_ready != python_ready) {
		m_python_ready = python_ready;
		signalStatusChanged(0, m_python_ready);
	}

	return 0;
}

Tester *g_tester;
