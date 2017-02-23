#coding=utf-8

import uter
import unittest

class test_case_export_type(unittest.TestCase):
    def test_export(self):
        datatype_1 = uter.export.typedef("typedef_uint")
        datatype_2 = uter.export.typedef("typedef_st1")
        print datatype_1
        print datatype_2
        cmptype = uter.export.struct("st_for_typedef_1")
        self.assertEqual(True, uter.uint == datatype_1, "")
        self.assertEqual(True, datatype_1 == uter.uint, "")
        self.assertEqual(True, cmptype == datatype_2, "")
        self.assertEqual(True, datatype_2 == cmptype, "")
        
class test_case_export_var(unittest.TestCase):
    def test_export(self):
        data_1 = uter.export.variable("typedef_uint_var")
        data_2 = uter.export.variable("typedef_st1_var")
        print data_1
        print data_2
        cmptype = uter.export.struct("st_for_typedef_1")
        self.assertEqual(True, uter.uint._MatchObj(data_1), "") 
        self.assertEqual(True, cmptype._MatchObj(data_2), "")
        
class test_case_def(unittest.TestCase):
    def test_def(self):
        datatype_1 = uter.typedef(uter.int, "test")
        sttype = uter.struct((uter.int, "a"), (uter.int, "b"))
        datatype_2 = uter.typedef(sttype, "test")
        print datatype_1
        print datatype_2
        self.assertEqual(True, uter.int == datatype_1, "") 
        self.assertEqual(True, datatype_1 == uter.int, "")
        self.assertEqual(True, sttype == datatype_2, "") 
        self.assertEqual(True, datatype_2 == sttype, "")
        
class test_case_obj(unittest.TestCase):
    def test_new(self):
        datatype_1 = uter.export.typedef("typedef_uint")
        sttype = uter.struct((uter.int, "a"), (uter.int, "b"))
        datatype_2 = uter.typedef(sttype, "test")
        data_1 = datatype_1()
        data_2 = datatype_2()
        self.assertEqual(True, uter.uint._MatchObj(data_1), "")
        self.assertEqual(True, datatype_1._MatchObj(data_1), "") 
        self.assertEqual(True, sttype._MatchObj(data_2), "")
        self.assertEqual(True, datatype_2._MatchObj(data_2), "")
                
class test_case_complex(unittest.TestCase):
    def test_export_type(self):
        datatype_1 = uter.export.typedef("typedef_uint_in_def")
        datatype_2 = uter.export.typedef("typedef_st1_in_def")
        print datatype_1
        print datatype_2
        cmptype = uter.export.struct("st_for_typedef_1")
        self.assertEqual(True, uter.uint == datatype_1, "")
        self.assertEqual(True, datatype_1 == uter.uint, "")
        self.assertEqual(True, cmptype == datatype_2, "")
        self.assertEqual(True, datatype_2 == cmptype, "")
        
    def test_export_var(self):
        data_1 = uter.export.variable("typedef_uint_in_def_var")
        data_2 = uter.export.variable("typedef_st1_in_def_var")
        print data_1
        print data_2
        cmptype = uter.export.struct("st_for_typedef_1")
        self.assertEqual(True, uter.uint._MatchObj(data_1), "")
        self.assertEqual(True, cmptype._MatchObj(data_2), "")
        
class test_case_convert(unittest.TestCase):    
    def test_astype(self):
        uintdata = uter.uint(0x12345678)
        print uintdata
        dt = uter.struct((uter.uchar, "a"), (uter.uchar, "b"), (uter.uchar, "c"), (uter.uchar, "d"))
        print dt
        stdata = uintdata._AsType(dt)
        print stdata
        self.assertEqual(True, stdata.a._GetValue() == 0x78, "")
        self.assertEqual(True, stdata.b._GetValue() == 0x56, "")
        self.assertEqual(True, stdata.c._GetValue() == 0x34, "")
        self.assertEqual(True, stdata.d._GetValue() == 0x12, "")
    
    def test_datablock(self):
        datablock = uter.memory.auto(24)
        for item in range(12):
            datablock[item]._SetValue(0x55)
        for item in range(12, 24):
            datablock[item]._SetValue(0x66)
        sttype = uter.struct((uter.uint, "a"), (uter.uint, "b"), (uter.ushort, "c"))
        print sttype
        stdata1 = datablock._AsType(sttype)
        print stdata1
        stdata2 = datablock[sttype._GetSize()]._AsType(sttype)
        print stdata2
        self.assertEqual(True, stdata1.a._GetValue() == 0x55555555, "")
        self.assertEqual(True, stdata1.b._GetValue() == 0x55555555, "")
        self.assertEqual(True, stdata1.c._GetValue() == 0x5555, "")
        self.assertEqual(True, stdata2.a._GetValue() == 0x66666666, "")
        self.assertEqual(True, stdata2.b._GetValue() == 0x66666666, "")
        self.assertEqual(True, stdata2.c._GetValue() == 0x6666, "")
