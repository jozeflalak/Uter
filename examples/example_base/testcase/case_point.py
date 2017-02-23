#coding=utf-8

import uter
import unittest

class test_case_export_var(unittest.TestCase):
    def test_base_base(self):
        pointobj_0 = uter.export.variable("g_p_int_0")
        print pointobj_0
        self.assertEqual(True, pointobj_0._GetValue() == 0, "")
        
        pointobj_1 = uter.export.variable("g_p_int_1")
        print pointobj_1
        self.assertEqual(True, pointobj_1._GetValue() == 0x12345678, "")
        
        pointobj_2 = uter.export.variable("g_p_int_2")
        print pointobj_2
        data = uter.export.variable("g_p_int_2_v")
        self.assertEqual(True, pointobj_2._GetValue() == data._GetAddress(), "")
        self.assertEqual(True, pointobj_2._GetType()._GetFinalType() == uter.int, "")
        
    def test_base_st(self):
        pointobj_0 = uter.export.variable("g_p_st_0")
        print pointobj_0
        self.assertEqual(True, pointobj_0._GetValue() == 0, "")
        
        pointobj_1 = uter.export.variable("g_p_st_1")
        print pointobj_1
        self.assertEqual(True, pointobj_1._GetValue() == 0x12345678, "")
        
        pointobj_2 = uter.export.variable("g_p_st_2")
        print pointobj_2
        data = uter.export.variable("g_p_st_2_v")
        datatype = uter.export.struct("g_p_st_1_t")
        self.assertEqual(True, pointobj_2._GetValue() == data._GetAddress(), "")
        self.assertEqual(True, pointobj_2._GetType()._GetFinalType() == datatype, "")
        
class test_case_def(unittest.TestCase):
    def test_def_base(self):
        pointtype_1 = uter.point(uter.uint)
        print pointtype_1
        self.assertEqual(True, pointtype_1._GetFinalType() == uter.uint, "")
        
        pointtype_2 = uter.point(uter.uint)
        print pointtype_2
        self.assertEqual(True, pointtype_2._GetFinalType() == uter.uint, "")
        
        pointtype_3 = uter.point(uter.uchar)
        print pointtype_3
        self.assertEqual(True, pointtype_3._GetFinalType() == uter.uchar, "")   
        
        self.assertEqual(True, pointtype_1 == pointtype_1, "")
        self.assertEqual(True, pointtype_2 == pointtype_2, "")
        self.assertEqual(True, pointtype_3 == pointtype_3, "")
        self.assertEqual(True, pointtype_1 == pointtype_2, "")
        self.assertEqual(True, pointtype_2 == pointtype_1, "")
        self.assertEqual(True, pointtype_1 != pointtype_3, "")
        self.assertEqual(True, pointtype_3 != pointtype_1, "")
        self.assertEqual(True, pointtype_2 != pointtype_3, "")
        self.assertEqual(True, pointtype_3 != pointtype_2, "")
        
    def test_def_st(self):
        sttype_1 = uter.struct((uter.int, "a"), (uter.int, "b"))
        pointtype_1 = uter.point(sttype_1)
        self.assertEqual(True, pointtype_1._GetFinalType() == sttype_1, "")
        sttype_2 = uter.struct((uter.int, "a"), (uter.int, "b"))
        pointtype_2 = uter.point(sttype_2)
        self.assertEqual(True, pointtype_2._GetFinalType() == sttype_2, "")
        sttype_3 = uter.struct((uter.int, "a"), (uter.char, "b"))
        pointtype_3 = uter.point(sttype_3)
        self.assertEqual(True, pointtype_3._GetFinalType() == sttype_3, "")
        
        self.assertEqual(True, pointtype_1 == pointtype_2, "")
        self.assertEqual(True, pointtype_2 == pointtype_1, "")
        self.assertEqual(True, pointtype_1 != pointtype_3, "")
        self.assertEqual(True, pointtype_3 != pointtype_1, "")
        self.assertEqual(True, pointtype_2 != pointtype_3, "")
        self.assertEqual(True, pointtype_3 != pointtype_2, "")
        
class test_case_new(unittest.TestCase):
    def test_export_type_new(self):
        totype = uter.export.struct("g_p_st_1_t")
        pointtype = uter.point(totype)
        pointdata_1 = pointtype()
        print pointdata_1
        self.assertEqual(True, pointtype._MatchType(pointdata_1._GetType()), "")
        self.assertEqual(True, pointdata_1._GetValue() == 0, "")
        
        pointdata_2 = pointtype(0x12345678)
        print pointdata_2
        self.assertEqual(True, pointdata_2._GetValue() == 0x12345678, "")
        
        pointdata_3 = pointtype(pointdata_2)
        print pointdata_3
        self.assertEqual(True, pointdata_3._GetValue() == 0x12345678, "")
        
    def test_def_type_new(self):
        sttype = uter.struct((uter.int, "a"), (uter.int, "b"))
        pointtype = uter.point(sttype)
        todata = sttype()
        pointdata_1 = pointtype()
        print pointdata_1
        self.assertEqual(True, pointtype._MatchType(pointdata_1._GetType()), "")
        self.assertEqual(True, pointdata_1._GetValue() == 0, "")
        
        pointdata_2 = pointtype(0x12345678)
        print pointdata_2
        self.assertEqual(True, pointdata_2._GetValue() == 0x12345678, "")
        
        pointdata_3 = pointtype(pointdata_2)
        print pointdata_3
        self.assertEqual(True, pointdata_3._GetValue() == 0x12345678, "")
        
class test_case_obj(unittest.TestCase):
    def test_match(self):
        pointdata_1 = uter.export.variable("g_p_st_1")
        datatype_1 = pointdata_1._GetType()
        pointdata_2 = uter.export.variable("g_p_st_2")
        datatype_2 = pointdata_2._GetType()
        self.assertEqual(True, datatype_1._MatchObj(pointdata_1) , "")
        self.assertEqual(True, datatype_2._MatchObj(pointdata_2) , "")
        
    def test_def_val(self):
        sttype = uter.struct((uter.int, "a"), (uter.int, "b"))
        pointtype = uter.point(sttype)
        pointdata = pointtype()
        self.assertEqual(True, pointdata._GetValue() == 0, "")
        
    def test_num_val(self):
        sttype = uter.struct((uter.int, "a"), (uter.int, "b"))
        pointtype = uter.point(sttype)
        pointdata = pointtype()
        pointdata._SetValue(0x12345678)
        self.assertEqual(True, pointdata._GetValue() == 0x12345678, "")
        
class test_case_complex(unittest.TestCase):   
    def test_export_1(self):
        ppoint = uter.export.variable("g_pp_int_0")
        print ppoint
        ppointtype = ppoint._GetType()
        print ppointtype
        pointtype = ppointtype._GetFinalType()
        datatype = pointtype._GetFinalType()
        print datatype
        
        self.assertEqual(True, ppoint._GetValue() == 0, "")
        self.assertEqual(True, datatype == uter.int, "")
        
    def test_export_2(self):
        ppoint = uter.export.variable("g_pp_st_0")
        ppointtype = ppoint._GetType()
        pointtype = ppointtype._GetFinalType()
        datatype = pointtype._GetFinalType()
        print datatype
        
        cmptype = uter.export.struct("g_p_st_1_t")
        
        self.assertEqual(True, ppoint._GetValue() == 0, "")
        self.assertEqual(True, cmptype._MatchType(datatype), "")

    def test_def_1(self):
        sttype = uter.struct((uter.int, "a"), (uter.int, "b"))
        pointtype = uter.point(sttype)
        ppointtype = uter.point(pointtype)
        ppointdata = ppointtype()
        print ppointtype
        print ppointdata
        self.assertEqual(True, True, "")

#通过对象创建指针对象
class test_case_point(unittest.TestCase):
    def test_point(self):
        intdata = uter.int(5)
        pointdata = uter.dopoint(intdata)
        print pointdata
        self.assertEqual(True, pointdata._GetValue() == intdata._GetAddress(), "")
        
class test_case_depoint(unittest.TestCase):
    def test_depoint(self):
        pointobj_2 = uter.export.variable("g_p_int_2")
        dedata = uter.depoint(pointobj_2)
        data = uter.export.variable("g_p_int_2_v")
        self.assertEqual(True, data._GetValue() == dedata._GetValue(), "")
    
class test_case_void(unittest.TestCase):        
    def test_type(self):
        print uter.void        
        self.assertEqual(True, uter.void == uter.void, "")
        self.assertEqual(True, uter.void._GetSize() == 0, "")
        self.assertEqual(True, uter.void._GetAlign() == 0, "")
        self.assertEqual(True, uter.void == uter.void, "")
        self.assertEqual(True, uter.void._MatchType(uter.void), "")
        self.assertEqual(True, uter.void._MatchObj(uter.void), "")
        
    def test_point(self):
        pointtype = uter.point(uter.void)
        print pointtype
        voidtype = pointtype._GetFinalType()
        print voidtype
        pointdata = pointtype()
        print pointdata
        self.assertEqual(True, True, "")

                