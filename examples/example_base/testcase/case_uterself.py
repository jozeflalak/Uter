#coding=utf-8

import uter
import unittest

#此文件主要验证uter的结构，比如类的继承关系等。与各个具体类型的测试不重复
class test_case_basetype(unittest.TestCase):   
    def test_all(self):        
        self.assertEqual(True, type(uter.char) is uter._UterMetaClass, "")
        self.assertEqual(True, type(uter.uchar) is uter._UterMetaClass, "")
        self.assertEqual(True, type(uter.short) is uter._UterMetaClass, "")
        self.assertEqual(True, type(uter.ushort) is uter._UterMetaClass, "")
        self.assertEqual(True, type(uter.int) is uter._UterMetaClass, "")
        self.assertEqual(True, type(uter.uint) is uter._UterMetaClass, "")
        self.assertEqual(True, type(uter.long) is uter._UterMetaClass, "")
        self.assertEqual(True, type(uter.ulong) is uter._UterMetaClass, "")
        self.assertEqual(True, type(uter.longlong) is uter._UterMetaClass, "")
        self.assertEqual(True, type(uter.ulonglong) is uter._UterMetaClass, "")
        self.assertEqual(True, type(uter.float) is uter._UterMetaClass, "")
        self.assertEqual(True, type(uter.double) is uter._UterMetaClass, "")
        
        self.assertEqual(True, isinstance(uter.char, uter._UterMetaClass), "")
        self.assertEqual(True, isinstance(uter.uchar, uter._UterMetaClass), "")
        self.assertEqual(True, isinstance(uter.short, uter._UterMetaClass), "")
        self.assertEqual(True, isinstance(uter.ushort, uter._UterMetaClass), "")
        self.assertEqual(True, isinstance(uter.int, uter._UterMetaClass), "")
        self.assertEqual(True, isinstance(uter.uint, uter._UterMetaClass), "")
        self.assertEqual(True, isinstance(uter.long, uter._UterMetaClass), "")
        self.assertEqual(True, isinstance(uter.ulong, uter._UterMetaClass), "")
        self.assertEqual(True, isinstance(uter.longlong, uter._UterMetaClass), "")
        self.assertEqual(True, isinstance(uter.ulonglong, uter._UterMetaClass), "")
        self.assertEqual(True, isinstance(uter.float, uter._UterMetaClass), "")
        self.assertEqual(True, isinstance(uter.double, uter._UterMetaClass), "")
        
        datatype = uter.struct((uter.char, "a"), (uter.char, "b"))
        self.assertEqual(True, type(datatype) is uter._UterMetaClass, "")
        self.assertEqual(True, isinstance(datatype, uter._UterMetaClass), "")
        
        datatype = uter.union((uter.char, "a"), (uter.char, "b"))
        self.assertEqual(True, type(datatype) is uter._UterMetaClass, "")
        self.assertEqual(True, isinstance(datatype, uter._UterMetaClass), "")
        
        datatype = uter.enum(("a", 1), ("b", 3))
        self.assertEqual(True, type(datatype) is uter._UterMetaClass, "")
        self.assertEqual(True, isinstance(datatype, uter._UterMetaClass), "")
        
        datatype = uter.point(uter.int)
        self.assertEqual(True, type(datatype) is uter._UterMetaClass, "")
        self.assertEqual(True, isinstance(datatype, uter._UterMetaClass), "")
        
        datatype = uter.array(uter.int, 8)
        self.assertEqual(True, type(datatype) is uter._UterMetaClass, "")
        self.assertEqual(True, isinstance(datatype, uter._UterMetaClass), "")
        
        datatype = uter.typedef(uter.int, "xxx")
        self.assertEqual(True, type(datatype) is uter._UterMetaClass, "")
        self.assertEqual(True, isinstance(datatype, uter._UterMetaClass), "")

class test_case_stub(unittest.TestCase):
    @staticmethod
    def stub_base_1_stub():
        print "--------------stub_base_1_stub running--------------"
        ret = uter.int(0)
        return ret
    def test_base_1(self):
        fun_stub_obj = uter.export.function("test_stub_without_call_stub")
        fun_call_obj = uter.export.function("test_stub_without_call")
        fun_call_obj._Call()
        fun_stub_obj._Stub(self.stub_base_1_stub)
        fun_call_obj._Call()
        
        #Pleae continue on cmd