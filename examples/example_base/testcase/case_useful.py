#coding=utf-8

import uter
import unittest

class test_case_call(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        fun_obj = uter.export.function("fun_do_something_1")
        fun_obj._Coverage._On()
     
    @classmethod
    def tearDownClass(cls):
        fun_obj = uter.export.function("fun_do_something_1")
        covinfo = fun_obj._Coverage._Count()
        fun_obj._Coverage._Off()
        if covinfo["covs"] != covinfo["sum"]:
            raise "Coverage not enough"
    
    def test_call(self):
        fun = uter.export.function("fun_do_something_1")
        result = fun._Call(uter.int(-1), uter.int(1))
        self.assertEqual(True, result._GetValue() == -1, "")

        fun = uter.export.function("fun_do_something_1")
        result = fun._Call(uter.int(1), uter.int(-1))
        self.assertEqual(True, result._GetValue() == -1, "")

        fun = uter.export.function("fun_do_something_1")
        result = fun._Call(uter.int(1), uter.int(1))
        self.assertEqual(True, result._GetValue() == 0, "")
        
    @staticmethod
    def stub_1(a, b):
        return uter.int(0)

    def test_stub_1(self):
        fun = uter.export.function("fun_do_something_1")
        stub = uter.export.function("fun_call_by_other")
        stub._Stub(self.stub_1)
        result = fun._Call(uter.int(1), uter.int(1))
        stub._Stub(None)
        self.assertEqual(True, result._GetValue() == 0, "")
        
    @staticmethod
    def stub_2(a, b):
        return uter.int(-1)
    
    def test_stub_2(self):
        fun = uter.export.function("fun_do_something_1")
        stub = uter.export.function("fun_call_by_other")
        stub._Stub(self.stub_2)
        result = fun._Call(uter.int(1), uter.int(1))
        stub._Stub(None)
        self.assertEqual(True, result._GetValue() == 0, "")
    
        
class test_case_list(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        fun_obj = uter.export.function("fun_walk_list_1")
        fun_obj._Coverage._On()
    
    @classmethod
    def tearDownClass(cls):
        fun_obj = uter.export.function("fun_walk_list_1")
        covinfo = fun_obj._Coverage._Count()
        fun_obj._Coverage._Off()
        if covinfo["covs"] != covinfo["sum"]:
            raise "Coverage not enough"
    
    def test_1(self):
        fun_obj = uter.export.function("fun_walk_list_1")
        sttype = uter.export.struct("my_node")
        stptype = uter.point(sttype)
        stpobj = stptype()
        result = fun_obj._Call(stpobj)
        self.assertEqual(True, result._GetValue() == 0, "")
        
    def test_2(self):
        fun_obj = uter.export.function("fun_walk_list_1")
        sttype = uter.export.struct("my_node")
        tmp_list = []
        
        stobj = sttype()
        tmp_list += [stobj]
        stobj.value._SetValue(1)
        stobj.next._SetValue(0)
        addr = stobj._GetAddress()
        
        stobj = sttype()
        tmp_list += [stobj]
        stobj.value._SetValue(2)
        stobj.next._SetValue(addr)
        addr = stobj._GetAddress()
        
        stobj = sttype()
        tmp_list += [stobj]
        stobj.value._SetValue(3)
        stobj.next._SetValue(addr)
        addr = stobj._GetAddress()
        
        stpobj = uter.dopoint(stobj)
        
        result = fun_obj._Call(stpobj)
        self.assertEqual(True, result._GetValue() == 6, "")
    
class test_case_msg(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        fun_obj = uter.export.function("my_msg_proc_1")
        fun_obj._Coverage._On()
    '''
    @classmethod
    def tearDownClass(cls):
        fun_obj = uter.export.function("my_msg_proc_1")
        covinfo = fun_obj._Coverage._Count()
        fun_obj._Coverage._Off()
        if covinfo["covs"] != covinfo["sum"]:
            raise "Coverage not enough"
	'''
    
    def test_msg_1(self):
        #准备
        fun_obj = uter.export.function("my_msg_proc_1")
        sttype = uter.export.struct("my_msg_head")
        psttype = uter.point(sttype)
        pstobj = psttype()
        retobj = fun_obj._Call(pstobj)
        self.assertEqual(True, retobj._GetValue() == -1, "")
    
    def test_msg_2(self):
        #准备
        fun_obj = uter.export.function("my_msg_proc_1")
        sttype = uter.export.struct("my_msg_head")
        stobj = sttype()
        stobj.type._SetValue(100)
        retobj = fun_obj._Call(uter.dopoint(stobj))
        self.assertEqual(True, retobj._GetValue() == -1, "")
    '''
    def test_msg_3(self):
        #准备
        fun_obj = uter.export.function("my_msg_proc_1")
        sttype = uter.export.struct("my_msg_head")
        macro_obj = uter.export.macro("my_msg_type_work")
        work_data_len = 20
        #内存
        memset = work_data_len + sttype._GetSize()
        memarry = uter.memory.auto(memset)
        #头
        stdata = memarry[0]._AsType(sttype)
        stdata.type._SetValue(macro_obj.number())
        stdata.length._SetValue(work_data_len + sttype._GetSize())
        print stdata
        #体
        workdatatype = uter.array(uter.uchar, work_data_len)
        workdata = memarry[sttype._GetSize()]._AsType(workdatatype)
        for item in range(work_data_len):
            workdata[item]._SetValue(item)
        #调
        retobj = fun_obj._Call(uter.dopoint(stdata))
        self.assertEqual(True, retobj._GetValue() == 190, "")
    '''