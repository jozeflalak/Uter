#coding=utf-8

import uter
import unittest

class test_fun_export_obj(unittest.TestCase):
    def test_export_fun_obj(self):
        fun_obj_0 = uter.export.function("fun_base_0")
        print fun_obj_0
        print fun_obj_0._GetAddress()
        fun_type_0 = fun_obj_0._GetType()
        print fun_type_0
        self.assertEqual(True, fun_obj_0._RetType() == uter.int,"")
        self.assertEqual(True, fun_obj_0._ArgType() == (),"")
        self.assertEqual(True, fun_type_0 != 1,"")
        
        fun_obj_1 = uter.export.function("fun_base_1")
        print fun_obj_1
        print fun_obj_1._GetAddress()
        fun_type_1 = fun_obj_1._GetType()
        print fun_type_1
        self.assertEqual(True, fun_obj_1._RetType() == uter.int,"")
        self.assertEqual(True, fun_obj_1._ArgType() == (uter.int, ),"")
        self.assertEqual(True, fun_type_1 != 1,"")
                
        fun_obj_2 = uter.export.function("fun_base_2")
        print fun_obj_2
        print fun_obj_2._GetAddress()
        fun_type_2 = fun_obj_2._GetType()
        print fun_type_2
        self.assertEqual(True, fun_obj_2._RetType() == uter.int,"")
        self.assertEqual(True, fun_obj_2._ArgType() == (uter.int, uter.int),"")
        self.assertEqual(True, fun_type_2 != 1,"")
        
        fun_obj_3 = uter.export.function("fun_base_3")
        print fun_obj_3
        print fun_obj_3._GetAddress()
        fun_type_3 = fun_obj_3._GetType()
        print fun_type_3
        self.assertEqual(True, fun_obj_3._RetType() == uter.int,"")
        self.assertEqual(True, fun_obj_3._ArgType() == (uter.int, uter.int, uter.int),"")
        self.assertEqual(True, fun_type_3 != 1,"")
        
        fun_obj_s = uter.export.function("fun_base_s")
        print fun_obj_s
        print fun_obj_s._GetAddress()
        fun_type_s = fun_obj_s._GetType()
        print fun_type_s
        self.assertEqual(True, fun_obj_s._RetType() == uter.int,"")
        self.assertEqual(True, fun_obj_s._ArgType() == (uter.int, uter.int, uter.int),"")
        self.assertEqual(True, fun_type_s != 1,"")
        
        fun_obj_v0 = uter.export.function("fun_base_v0")
        print fun_obj_v0
        print fun_obj_v0._GetAddress()
        fun_type_v0 = fun_obj_v0._GetType()
        print fun_type_v0
        self.assertEqual(True, fun_obj_v0._RetType() == uter.void,"")
        self.assertEqual(True, fun_obj_v0._ArgType() == (),"")
        self.assertEqual(True, fun_type_v0 != 1,"")
        
        fun_obj_v1 = uter.export.function("fun_base_v1")
        print fun_obj_v1
        print fun_obj_v1._GetAddress()
        fun_type_v1 = fun_obj_v1._GetType()
        print fun_type_v1
        self.assertEqual(True, fun_obj_v1._RetType() == uter.void,"")
        self.assertEqual(True, fun_obj_v1._ArgType() == (uter.int, ),"")
        self.assertEqual(True, fun_type_v1 != 1,"")
        
        self.assertEqual(True, fun_type_0 == fun_type_0,"")
        self.assertEqual(True, fun_type_0 != fun_type_1,"")
        self.assertEqual(True, fun_type_0 != fun_type_2,"")
        self.assertEqual(True, fun_type_0 != fun_type_3,"")
        self.assertEqual(True, fun_type_0 != fun_type_s,"")
        self.assertEqual(True, fun_type_0 != fun_type_v0,"")
        self.assertEqual(True, fun_type_0 != fun_type_v1,"")
        
        self.assertEqual(True, fun_type_1 != fun_type_0,"")
        self.assertEqual(True, fun_type_1 == fun_type_1,"")
        self.assertEqual(True, fun_type_1 != fun_type_2,"")
        self.assertEqual(True, fun_type_1 != fun_type_3,"")
        self.assertEqual(True, fun_type_1 != fun_type_s,"")
        self.assertEqual(True, fun_type_1 != fun_type_v0,"")
        self.assertEqual(True, fun_type_1 != fun_type_v1,"")
        
        self.assertEqual(True, fun_type_2 != fun_type_0,"")
        self.assertEqual(True, fun_type_2 != fun_type_1,"")
        self.assertEqual(True, fun_type_2 == fun_type_2,"")
        self.assertEqual(True, fun_type_2 != fun_type_3,"")
        self.assertEqual(True, fun_type_2 != fun_type_s,"")
        self.assertEqual(True, fun_type_2 != fun_type_v0,"")
        self.assertEqual(True, fun_type_2 != fun_type_v1,"")
        
        self.assertEqual(True, fun_type_3 != fun_type_0,"")
        self.assertEqual(True, fun_type_3 != fun_type_1,"")
        self.assertEqual(True, fun_type_3 != fun_type_2,"")
        self.assertEqual(True, fun_type_3 == fun_type_3,"")
        self.assertEqual(True, fun_type_3 == fun_type_s,"")
        self.assertEqual(True, fun_type_3 != fun_type_v0,"")
        self.assertEqual(True, fun_type_3 != fun_type_v1,"")
        
        self.assertEqual(True, fun_type_s != fun_type_0,"")
        self.assertEqual(True, fun_type_s != fun_type_1,"")
        self.assertEqual(True, fun_type_s != fun_type_2,"")
        self.assertEqual(True, fun_type_s == fun_type_3,"")
        self.assertEqual(True, fun_type_s == fun_type_s,"")
        self.assertEqual(True, fun_type_s != fun_type_v0,"")
        self.assertEqual(True, fun_type_s != fun_type_v1,"")
        
        self.assertEqual(True, fun_type_v0 != fun_type_0,"")
        self.assertEqual(True, fun_type_v0 != fun_type_1,"")
        self.assertEqual(True, fun_type_v0 != fun_type_2,"")
        self.assertEqual(True, fun_type_v0 != fun_type_3,"")
        self.assertEqual(True, fun_type_v0 != fun_type_s,"")
        self.assertEqual(True, fun_type_v0 == fun_type_v0,"")
        self.assertEqual(True, fun_type_v0 != fun_type_v1,"")
        
        self.assertEqual(True, fun_type_v1 != fun_type_0,"")
        self.assertEqual(True, fun_type_v1 != fun_type_1,"")
        self.assertEqual(True, fun_type_v1 != fun_type_2,"")
        self.assertEqual(True, fun_type_v1 != fun_type_3,"")
        self.assertEqual(True, fun_type_v1 != fun_type_s,"")
        self.assertEqual(True, fun_type_v1 != fun_type_v0,"")
        self.assertEqual(True, fun_type_v1 == fun_type_v1,"")

class test_fun_def_type(unittest.TestCase):
    def test_def_fun_type(self):
        funtype_1 = uter.function(uter.int, (uter.int, uter.char, uter.int))
        print funtype_1
        funobj_1 = funtype_1()
        print funobj_1
        self.assertEqual(True, funtype_1._RetType() == uter.int, "")
        self.assertEqual(True, funtype_1._ArgType() == (uter.int, uter.char, uter.int), "")
        self.assertEqual(True, funtype_1 != 1, "")
        
        funtype_2 = uter.function(uter.void, (uter.int, uter.char, uter.int))
        print funtype_2
        funobj_2 = funtype_2()
        print funobj_2
        self.assertEqual(True, funtype_2._RetType() == uter.void, "")
        self.assertEqual(True, funtype_2._ArgType() == (uter.int, uter.char, uter.int) , "")
        self.assertEqual(True, funtype_2 != 1, "")
        
        funtype_3 = uter.function(uter.void, ())
        print funtype_3
        funobj_3 = funtype_3()
        print funobj_3
        self.assertEqual(True, funtype_3._RetType() == uter.void, "")
        self.assertEqual(True, funtype_3._ArgType() == (), "")
        self.assertEqual(True, funtype_3 != 1, "")
        
        funtype_4 = uter.function(uter.int, (uter.int, uter.char, uter.int))
        print funtype_4
        funobj_4 = funtype_4()
        print funobj_4
        self.assertEqual(True, funtype_4._RetType() == uter.int, "")
        self.assertEqual(True, funtype_4._ArgType() == (uter.int, uter.char, uter.int), "")
        self.assertEqual(True, funtype_4 != 1, "")
        
        self.assertEqual(True, funtype_1 == funtype_1, "")
        self.assertEqual(True, funtype_1 != funtype_2, "")
        self.assertEqual(True, funtype_1 != funtype_3, "")
        self.assertEqual(True, funtype_1 == funtype_4, "")

        self.assertEqual(True, funtype_2 != funtype_1, "")
        self.assertEqual(True, funtype_2 == funtype_2, "")
        self.assertEqual(True, funtype_2 != funtype_3, "")
        self.assertEqual(True, funtype_2 != funtype_4, "")

        self.assertEqual(True, funtype_3 != funtype_1, "")
        self.assertEqual(True, funtype_3 != funtype_2, "")
        self.assertEqual(True, funtype_3 == funtype_3, "")
        self.assertEqual(True, funtype_3 != funtype_4, "")

        self.assertEqual(True, funtype_4 == funtype_1, "")
        self.assertEqual(True, funtype_4 != funtype_2, "")
        self.assertEqual(True, funtype_4 != funtype_3, "")
        self.assertEqual(True, funtype_4 == funtype_4, "")

class test_fun_obj(unittest.TestCase):
    def test_match(self):
        fun_obj_0 = uter.export.function("fun_base_0")
        fun_type_0 = fun_obj_0._GetType()
                
        fun_obj_1 = uter.export.function("fun_base_1")
        fun_type_1 = fun_obj_1._GetType()
                
        fun_obj_2 = uter.export.function("fun_base_2")
        fun_type_2 = fun_obj_2._GetType()
        
        fun_obj_3 = uter.export.function("fun_base_3")
        fun_type_3 = fun_obj_3._GetType()
        
        fun_obj_s = uter.export.function("fun_base_s")
        fun_type_s = fun_obj_s._GetType()
        
        fun_obj_v0 = uter.export.function("fun_base_v0")
        fun_type_v0 = fun_obj_v0._GetType()
                
        fun_obj_v1 = uter.export.function("fun_base_v1")
        fun_type_v1 = fun_obj_v1._GetType()
        
        self.assertEqual(True, not fun_type_0._MatchObj(1), "")
        self.assertEqual(True, fun_type_0._MatchObj(fun_obj_0), "")
        self.assertEqual(True, not fun_type_0._MatchObj(fun_obj_1), "")
        self.assertEqual(True, not fun_type_0._MatchObj(fun_obj_2), "")
        self.assertEqual(True, not fun_type_0._MatchObj(fun_obj_3), "")
        self.assertEqual(True, not fun_type_0._MatchObj(fun_obj_s), "")
        self.assertEqual(True, not fun_type_0._MatchObj(fun_obj_v0), "")
        self.assertEqual(True, not fun_type_0._MatchObj(fun_obj_v1), "")

        self.assertEqual(True, not fun_type_1._MatchObj(1), "")
        self.assertEqual(True, not fun_type_1._MatchObj(fun_obj_0), "")
        self.assertEqual(True, fun_type_1._MatchObj(fun_obj_1), "")
        self.assertEqual(True, not fun_type_1._MatchObj(fun_obj_2), "")
        self.assertEqual(True, not fun_type_1._MatchObj(fun_obj_3), "")
        self.assertEqual(True, not fun_type_1._MatchObj(fun_obj_s), "")
        self.assertEqual(True, not fun_type_1._MatchObj(fun_obj_v0), "")
        self.assertEqual(True, not fun_type_1._MatchObj(fun_obj_v1), "")

        self.assertEqual(True, not fun_type_2._MatchObj(1), "")
        self.assertEqual(True, not fun_type_2._MatchObj(fun_obj_0), "")
        self.assertEqual(True, not fun_type_2._MatchObj(fun_obj_1), "")
        self.assertEqual(True, fun_type_2._MatchObj(fun_obj_2), "")
        self.assertEqual(True, not fun_type_2._MatchObj(fun_obj_3), "")
        self.assertEqual(True, not fun_type_2._MatchObj(fun_obj_s), "")
        self.assertEqual(True, not fun_type_2._MatchObj(fun_obj_v0), "")
        self.assertEqual(True, not fun_type_2._MatchObj(fun_obj_v1), "")

        self.assertEqual(True, not fun_type_3._MatchObj(1), "")
        self.assertEqual(True, not fun_type_3._MatchObj(fun_obj_0), "")
        self.assertEqual(True, not fun_type_3._MatchObj(fun_obj_1), "")
        self.assertEqual(True, not fun_type_3._MatchObj(fun_obj_2), "")
        self.assertEqual(True, fun_type_3._MatchObj(fun_obj_3), "")
        self.assertEqual(True, fun_type_3._MatchObj(fun_obj_s), "")
        self.assertEqual(True, not fun_type_3._MatchObj(fun_obj_v0), "")
        self.assertEqual(True, not fun_type_3._MatchObj(fun_obj_v1), "")
   
        self.assertEqual(True, not fun_type_s._MatchObj(1), "")
        self.assertEqual(True, not fun_type_s._MatchObj(fun_obj_0), "")
        self.assertEqual(True, not fun_type_s._MatchObj(fun_obj_1), "")
        self.assertEqual(True, not fun_type_s._MatchObj(fun_obj_2), "")
        self.assertEqual(True, fun_type_s._MatchObj(fun_obj_3), "")
        self.assertEqual(True, fun_type_s._MatchObj(fun_obj_s), "")
        self.assertEqual(True, not fun_type_s._MatchObj(fun_obj_v0), "")
        self.assertEqual(True, not fun_type_s._MatchObj(fun_obj_v1), "")

        self.assertEqual(True, not fun_type_v0._MatchObj(1), "")
        self.assertEqual(True, not fun_type_v0._MatchObj(fun_obj_0), "")
        self.assertEqual(True, not fun_type_v0._MatchObj(fun_obj_1), "")
        self.assertEqual(True, not fun_type_v0._MatchObj(fun_obj_2), "")
        self.assertEqual(True, not fun_type_v0._MatchObj(fun_obj_3), "")
        self.assertEqual(True, not fun_type_v0._MatchObj(fun_obj_s), "")
        self.assertEqual(True, fun_type_v0._MatchObj(fun_obj_v0), "")
        self.assertEqual(True, not fun_type_v0._MatchObj(fun_obj_v1), "")

        self.assertEqual(True, not fun_type_v1._MatchObj(1), "")
        self.assertEqual(True, not fun_type_v1._MatchObj(fun_obj_0), "")
        self.assertEqual(True, not fun_type_v1._MatchObj(fun_obj_1), "")
        self.assertEqual(True, not fun_type_v1._MatchObj(fun_obj_2), "")
        self.assertEqual(True, not fun_type_v1._MatchObj(fun_obj_3), "")
        self.assertEqual(True, not fun_type_v1._MatchObj(fun_obj_s), "")
        self.assertEqual(True, not fun_type_v1._MatchObj(fun_obj_v0), "")
        self.assertEqual(True, fun_type_v1._MatchObj(fun_obj_v1), "")

class test_point_export_var(unittest.TestCase):
    def test_export_point_var(self):
        funpoint_obj_1 = uter.export.variable("fun_base_point_1")
        print funpoint_obj_1
        fun_obj_1 = uter.export.function("fun_base_fun_1")
        print fun_obj_1
        self.assertEqual(True, funpoint_obj_1._GetValue() == fun_obj_1._GetAddress(), "")
        self.assertEqual(True, funpoint_obj_1._GetFinalType() == fun_obj_1._GetType(), "")
        
        funpoint_obj_2 = uter.export.variable("fun_base_point_2")
        print funpoint_obj_2
        fun_obj_2 = uter.export.function("fun_base_fun_2")
        print fun_obj_2
        self.assertEqual(True, funpoint_obj_2._GetValue() == fun_obj_2._GetAddress(), "")
        self.assertEqual(True, funpoint_obj_2._GetFinalType() == fun_obj_2._GetType(), "")
        
class test_point_def_type(unittest.TestCase):
    def test_def_type_point(self):
        funtype_1 = uter.function(uter.int, (uter.int, uter.char, uter.int))
        print funtype_1
        pointtype_1 = uter.point(funtype_1)
        print pointtype_1
        self.assertEqual(True, pointtype_1._GetFinalType() ==  funtype_1, "")
        
        funtype_2 = uter.function(uter.int, (uter.int, uter.char))
        print funtype_2
        pointtype_2 = uter.point(funtype_2)
        print pointtype_2
        self.assertEqual(True, pointtype_2._GetFinalType() ==  funtype_2, "")
                
        funtype_3 = uter.function(uter.int, ())
        print funtype_3
        pointtype_3 = uter.point(funtype_3)
        print pointtype_3
        self.assertEqual(True, pointtype_3._GetFinalType() ==  funtype_3, "")
        
        funtype_4 = uter.function(uter.void, (uter.int, uter.char, uter.int))
        print funtype_4
        pointtype_4 = uter.point(funtype_4)
        print pointtype_4
        self.assertEqual(True, pointtype_4._GetFinalType() ==  funtype_4, "")
        
        funtype_5 = uter.function(uter.int, (uter.int, uter.char, uter.int))
        print funtype_5
        pointtype_5 = uter.point(funtype_5)
        print pointtype_5
        self.assertEqual(True, pointtype_5._GetFinalType() ==  funtype_5, "")
        
        self.assertEqual(True, pointtype_1 != 1, "")
        self.assertEqual(True, pointtype_1 == pointtype_1, "")
        self.assertEqual(True, pointtype_1 != pointtype_2, "")
        self.assertEqual(True, pointtype_1 != pointtype_3, "")
        self.assertEqual(True, pointtype_1 != pointtype_4, "")
        self.assertEqual(True, pointtype_1 == pointtype_5, "")

        self.assertEqual(True, pointtype_2 != 1, "")
        self.assertEqual(True, pointtype_2 != pointtype_1, "")
        self.assertEqual(True, pointtype_2 == pointtype_2, "")
        self.assertEqual(True, pointtype_2 != pointtype_3, "")
        self.assertEqual(True, pointtype_2 != pointtype_4, "")
        self.assertEqual(True, pointtype_2 != pointtype_5, "")

        self.assertEqual(True, pointtype_3 != 1, "")
        self.assertEqual(True, pointtype_3 != pointtype_1, "")
        self.assertEqual(True, pointtype_3 != pointtype_2, "")
        self.assertEqual(True, pointtype_3 == pointtype_3, "")
        self.assertEqual(True, pointtype_3 != pointtype_4, "")
        self.assertEqual(True, pointtype_3 != pointtype_5, "")
        
        self.assertEqual(True, pointtype_4 != 1, "")
        self.assertEqual(True, pointtype_4 != pointtype_1, "")
        self.assertEqual(True, pointtype_4 != pointtype_2, "")
        self.assertEqual(True, pointtype_4 != pointtype_3, "")
        self.assertEqual(True, pointtype_4 == pointtype_4, "")
        self.assertEqual(True, pointtype_4 != pointtype_5, "")

        self.assertEqual(True, pointtype_5 != 1, "")
        self.assertEqual(True, pointtype_5 == pointtype_1, "")
        self.assertEqual(True, pointtype_5 != pointtype_2, "")
        self.assertEqual(True, pointtype_5 != pointtype_3, "")
        self.assertEqual(True, pointtype_5 != pointtype_4, "")
        self.assertEqual(True, pointtype_5 == pointtype_5, "")
        
class test_point_new(unittest.TestCase):
    def test_def_new(self):
        funtype_1 = uter.function(uter.int, (uter.int, uter.char, uter.int))
        print funtype_1
        pointtype_1 = uter.point(funtype_1)
        print pointtype_1
        pointdata_1 = pointtype_1()
        print pointdata_1
        self.assertEqual(True, pointdata_1._GetValue() == 0, "")
        
        funtype_2 = uter.function(uter.int, (uter.int, uter.char))
        print funtype_2
        pointtype_2 = uter.point(funtype_2)
        print pointtype_2
        pointdata_2 = pointtype_2()
        print pointdata_2
        self.assertEqual(True, pointdata_2._GetValue() == 0, "")
        
        funtype_3 = uter.function(uter.int, ())
        print funtype_3
        pointtype_3 = uter.point(funtype_3)
        print pointtype_3
        pointdata_3 = pointtype_3()
        print pointdata_3
        self.assertEqual(True, pointdata_3._GetValue() == 0, "")
        
        funtype_4 = uter.function(uter.void, (uter.int, uter.char, uter.int))
        print funtype_4
        pointtype_4 = uter.point(funtype_4)
        print pointtype_4
        pointdata_4 = pointtype_4()
        print pointdata_4
        self.assertEqual(True, pointdata_4._GetValue() == 0, "")
        
        funtype_5 = uter.function(uter.int, (uter.int, uter.char, uter.int))
        print funtype_5
        pointtype_5 = uter.point(funtype_5)
        print pointtype_5
        pointdata_5 = pointtype_5()
        print pointdata_5
        self.assertEqual(True, pointdata_5._GetValue() == 0, "")
    
class test_point_obj(unittest.TestCase):
    def test_fun2point(self):
        fun_obj_0 = uter.export.function("fun_base_0")
        funpoint_obj_0 = uter.dopoint(fun_obj_0)
        print funpoint_obj_0
        self.assertEqual(True, funpoint_obj_0._GetValue() == fun_obj_0._GetAddress(), "")
        
        fun_obj_1 = uter.export.function("fun_base_1")
        funpoint_obj_1 = uter.dopoint(fun_obj_1)
        print funpoint_obj_1
        self.assertEqual(True, funpoint_obj_1._GetValue() == fun_obj_1._GetAddress(), "")
        
        fun_obj_2 = uter.export.function("fun_base_2")
        funpoint_obj_2 = uter.dopoint(fun_obj_2)
        print funpoint_obj_2
        self.assertEqual(True, funpoint_obj_2._GetValue() == fun_obj_2._GetAddress(), "")
        
        fun_obj_3 = uter.export.function("fun_base_3")
        funpoint_obj_3 = uter.dopoint(fun_obj_3)
        print funpoint_obj_3
        self.assertEqual(True, funpoint_obj_3._GetValue() == fun_obj_3._GetAddress(), "")
        
        fun_obj_s = uter.export.function("fun_base_s")
        #funpoint_obj_s = uter.dopoint(fun_obj_s)
        #print funpoint_obj_s
        
        #fun_obj_v = uter.export.function("fun_base_v")

class test_call(unittest.TestCase):
    def test_base(self):
        fun_obj_0 = uter.export.function("fun_call_base_0")
        ret_obj_0 = fun_obj_0._Call()
        self.assertEqual(True, ret_obj_0._GetValue() == 1, "")
        
        fun_obj_1 = uter.export.function("fun_call_base_1")
        ret_obj_1 = fun_obj_1._Call(uter.int(5))
        self.assertEqual(True, ret_obj_1._GetValue() == 10, "")
        
        fun_obj_2 = uter.export.function("fun_call_base_2")
        ret_obj_2 = fun_obj_2._Call(uter.int(5), uter.int(6))
        self.assertEqual(True, ret_obj_2._GetValue() == 16, "")
        
        fun_obj_3 = uter.export.function("fun_call_base_3")
        int_obj_1 = uter.int(5)
        int_obj_2 = uter.char(6)
        ret_obj_3 = fun_obj_3._Call(uter.dopoint(int_obj_1), uter.dopoint(int_obj_2))
        self.assertEqual(True, ret_obj_3._GetValue() == 11, "")
        
        fun_obj_4 = uter.export.function("fun_call_base_4")
        ret_obj_4 = fun_obj_4._Call(uter.int(5), uter.char(6))        
        self.assertEqual(True, ret_obj_4._GetValue() == 11, "")
        
    def test_arg_base(self):        
        fun_obj = uter.export.function("fun_call_align_1")
        print fun_obj
        ret_obj = fun_obj._Call(uter.char(6))
        self.assertEqual(True, ret_obj._GetValue() == 6, "")
        
        fun_obj = uter.export.function("fun_call_align_2")
        print fun_obj
        ret_obj = fun_obj._Call(uter.char(6), uter.char(7))
        self.assertEqual(True, ret_obj._GetValue() == 13, "")
        
        fun_obj = uter.export.function("fun_call_align_3")
        print fun_obj
        ret_obj = fun_obj._Call(uter.char(6), uter.int(7), uter.char(8))
        self.assertEqual(True, ret_obj._GetValue() == 21, "")
        
        fun_obj = uter.export.function("fun_call_align_4")
        print fun_obj
        ret_obj = fun_obj._Call(uter.int(6), uter.char(7), uter.int(8))
        self.assertEqual(True, ret_obj._GetValue() == 21, "")
        
        fun_obj = uter.export.function("fun_call_align_5")
        print fun_obj
        ret_obj = fun_obj._Call(uter.short(6))
        self.assertEqual(True, ret_obj._GetValue() == 6, "")
        
        fun_obj = uter.export.function("fun_call_align_6")
        print fun_obj
        ret_obj = fun_obj._Call(uter.short(6), uter.short(7))
        self.assertEqual(True, ret_obj._GetValue() == 13, "")
        
        fun_obj = uter.export.function("fun_call_align_7")
        print fun_obj
        ret_obj = fun_obj._Call(uter.short(6), uter.int(7), uter.short(8))
        self.assertEqual(True, ret_obj._GetValue() == 21, "")
        
        fun_obj = uter.export.function("fun_call_align_8")
        print fun_obj
        ret_obj = fun_obj._Call(uter.int(6), uter.short(7), uter.int(8))
        self.assertEqual(True, ret_obj._GetValue() == 21, "")
        
        fun_obj = uter.export.function("fun_call_align_ll_1")
        print fun_obj
        ret_obj = fun_obj._Call(uter.longlong(8))
        self.assertEqual(True, ret_obj._GetValue() == 8, "")
        
        fun_obj = uter.export.function("fun_call_align_ll_2")
        print fun_obj
        ret_obj = fun_obj._Call(uter.longlong(8), uter.longlong(9))
        self.assertEqual(True, ret_obj._GetValue() == 17, "")
        
        fun_obj = uter.export.function("fun_call_align_ll_3")
        print fun_obj
        ret_obj = fun_obj._Call(uter.longlong(8), uter.int(9), uter.longlong(10))
        self.assertEqual(True, ret_obj._GetValue() == 27, "")
        
        fun_obj = uter.export.function("fun_call_align_ll_4")
        print fun_obj
        ret_obj = fun_obj._Call(uter.int(8), uter.longlong(9), uter.int(10))
        self.assertEqual(True, ret_obj._GetValue() == 27, "")
        
    def test_ret_base(self):        
        fun_obj = uter.export.function("fun_call_ret_1")
        print fun_obj
        ret_obj = fun_obj._Call()
        self.assertEqual(True, ret_obj._GetType() == uter.int and ret_obj._GetValue() == 0x7fffffff, "")
        
        fun_obj = uter.export.function("fun_call_ret_2")
        print fun_obj
        ret_obj = fun_obj._Call()
        self.assertEqual(True, ret_obj._GetType() == uter.char and ret_obj._GetValue() == 120, "")
        
        fun_obj = uter.export.function("fun_call_ret_3")
        print fun_obj
        ret_obj = fun_obj._Call()
        self.assertEqual(True, ret_obj._GetType() == uter.short and ret_obj._GetValue() == 255 * 4, "")
        
        fun_obj = uter.export.function("fun_call_ret_4")
        print fun_obj
        ret_obj = fun_obj._Call()
        self.assertEqual(True, ret_obj._GetType() == uter.longlong and ret_obj._GetValue() == 0xffffffff + 1, "")
        
        fun_obj = uter.export.function("fun_call_ret_5")
        print fun_obj
        ret_obj = fun_obj._Call()
        self.assertEqual(True, ret_obj._GetType() == uter.longlong and ret_obj._GetValue() == 0xffffffff + 0xffffffff, "")
        
        fun_obj = uter.export.function("fun_call_ret_6")
        print fun_obj
        ret_obj = fun_obj._Call()
        self.assertEqual(True, ret_obj == uter.void, "")
        
    def test_arg_st(self):        
        fun_obj = uter.export.function("fun_call_starg_1")
        print fun_obj
        st_type = uter.export.struct("fun_call_starg_st_1")
        st_obj = st_type(5, 6)
        print st_obj
        ret_obj = fun_obj._Call(st_obj)
        self.assertEqual(True, ret_obj._GetValue() == 11, "")
        
        fun_obj = uter.export.function("fun_call_starg_2")
        print fun_obj
        st_type = uter.export.struct("fun_call_starg_st_2")
        st_obj = st_type(5, 6)
        print st_obj
        ret_obj = fun_obj._Call(st_obj)
        self.assertEqual(True, ret_obj._GetValue() == 11, "")
        
        fun_obj = uter.export.function("fun_call_starg_3")
        print fun_obj
        st_type = uter.export.struct("fun_call_starg_st_3")
        st_obj = st_type(5, 6, 7)
        print st_obj
        ret_obj = fun_obj._Call(st_obj)
        self.assertEqual(True, ret_obj._GetValue() == 18, "")
        
        fun_obj = uter.export.function("fun_call_starg_4")
        print fun_obj
        st_type = uter.export.struct("fun_call_starg_st_4")
        st_obj = st_type(5, 6, 7)
        print st_obj
        ret_obj = fun_obj._Call(st_obj)
        self.assertEqual(True, ret_obj._GetValue() == 18, "")
        
        fun_obj = uter.export.function("fun_call_starg_5")
        print fun_obj
        st_type = uter.export.struct("fun_call_starg_st_5")
        st_obj = st_type(9)
        print st_obj
        ret_obj = fun_obj._Call(st_obj)
        self.assertEqual(True, ret_obj._GetValue() == 9, "")
        
        fun_obj = uter.export.function("fun_call_starg_6")
        print fun_obj
        st_type = uter.export.struct("fun_call_starg_st_6")
        st_obj = st_type(4, 5)
        print st_obj
        ret_obj = fun_obj._Call(st_obj, uter.int(6))
        self.assertEqual(True, ret_obj._GetValue() == 15, "")
        
        fun_obj = uter.export.function("fun_call_starg_7")
        print fun_obj
        st_type = uter.export.struct("fun_call_starg_st_7")
        st_obj = st_type(4, 5)
        print st_obj
        ret_obj = fun_obj._Call(uter.int(3), st_obj, uter.int(6))
        self.assertEqual(True, ret_obj._GetValue() == 18, "")
        
        fun_obj = uter.export.function("fun_call_starg_8")
        print fun_obj
        st_type = uter.export.struct("fun_call_starg_st_8")
        st_obj = st_type(4, 5)
        print st_obj
        ret_obj = fun_obj._Call(st_obj, uter.int(6), st_obj)
        self.assertEqual(True, ret_obj._GetValue() == 24, "")
        
        fun_obj = uter.export.function("fun_call_starg_9")
        print fun_obj
        st_type = uter.export.struct("fun_call_starg_st_9")
        st_obj = st_type(5)
        print st_obj
        ret_obj = fun_obj._Call(st_obj, uter.int(6))
        self.assertEqual(True, ret_obj._GetValue() == 11, "")
        
    def test_ret_st(self):        
        fun_obj = uter.export.function("fun_call_stret_1")
        print fun_obj
        st_type = uter.export.struct("fun_call_stret_st_1")
        ret_obj = fun_obj._Call(uter.int(5), uter.int(6))
        self.assertEqual(True, ret_obj._GetType() == st_type and ret_obj.a._GetValue() == 5 and ret_obj.b._GetValue() == 6, "")
        
        fun_obj = uter.export.function("fun_call_stret_2")
        print fun_obj
        st_type = uter.export.struct("fun_call_stret_st_2")
        ret_obj = fun_obj._Call(uter.int(5), uter.int(6))
        self.assertEqual(True, ret_obj._GetType() == st_type and ret_obj.a._GetValue() == 5 and ret_obj.b._GetValue() == 6, "")
        
        fun_obj = uter.export.function("fun_call_stret_3")
        print fun_obj
        st_type = uter.export.struct("fun_call_stret_st_3")
        ret_obj = fun_obj._Call(uter.int(5), uter.int(6))
        self.assertEqual(True, ret_obj._GetType() == st_type and ret_obj.a._GetValue() == 11, "")

    def test_arg_un(self):        
        fun_obj = uter.export.function("fun_call_unarg_1")
        print fun_obj
        un_type = uter.export.union("fun_call_unarg_un_1")
        un_obj = un_type("a", [9])
        print un_obj
        ret_obj = fun_obj._Call(un_obj)
        self.assertEqual(True, ret_obj._GetValue() == 9, "")
        
        fun_obj = uter.export.function("fun_call_unarg_2")
        print fun_obj
        un_type = uter.export.union("fun_call_unarg_un_2")
        un_obj = un_type("a", [9])
        print un_obj
        ret_obj = fun_obj._Call(un_obj)
        self.assertEqual(True, ret_obj._GetValue() == 9, "")
        
        fun_obj = uter.export.function("fun_call_unarg_3")
        print fun_obj
        un_type = uter.export.union("fun_call_unarg_un_3")
        un_obj = un_type("a", [9])
        print un_obj
        ret_obj = fun_obj._Call(un_obj)
        self.assertEqual(True, ret_obj._GetValue() == 9, "")
        
        fun_obj = uter.export.function("fun_call_unarg_4")
        print fun_obj
        un_type = uter.export.union("fun_call_unarg_un_4")
        un_obj = un_type("a", [9])
        print un_obj
        ret_obj = fun_obj._Call(un_obj)
        self.assertEqual(True, ret_obj._GetValue() == 9, "")
        
        fun_obj = uter.export.function("fun_call_unarg_5")
        print fun_obj
        un_type = uter.export.union("fun_call_unarg_un_5")
        un_obj = un_type("a", [9])
        print un_obj
        ret_obj = fun_obj._Call(un_obj, uter.int(2))
        self.assertEqual(True, ret_obj._GetValue() == 11, "")
        
    def test_ret_un(self):        
        fun_obj = uter.export.function("fun_call_unret_1")
        print fun_obj
        un_type = uter.export.union("fun_call_unret_un_1")
        ret_obj = fun_obj._Call(uter.int(4), uter.int(5))
        print ret_obj        
        self.assertEqual(True, ret_obj._GetType() == un_type and ret_obj.a._GetValue() == 9, "")
    
class test_stub(unittest.TestCase):
    @staticmethod
    def stub_base_0_stub():
        print "--------------stub_base_0_stub running--------------"
        ret = uter.int(10)
        return ret
    
    def test_stub_base_0(self):
        fun_stub_obj = uter.export.function("fun_stub_base_0_stub")
        fun_call_obj = uter.export.function("fun_stub_base_0_call")
        
        int_result_obj = fun_call_obj._Call()
        self.assertEqual(True, int_result_obj._GetValue() == 5, "")
        
        fun_stub_obj._Stub(self.stub_base_0_stub)
        int_result_obj = fun_call_obj._Call()
        self.assertEqual(True, int_result_obj._GetValue() == 10, "")
        fun_stub_obj._Stub(None)
        
        int_result_obj = fun_call_obj._Call()
        self.assertEqual(True, int_result_obj._GetValue() == 5, "")
    
    @staticmethod
    def stub_base_1_stub(a):
        print "--------------stub_base_1_stub running--------------"
        v1 = a._GetValue()
        ret = uter.int(v1 + 2)
        return ret
    
    def test_stub_base_1(self):
        fun_stub_obj = uter.export.function("fun_stub_base_1_stub")
        fun_call_obj = uter.export.function("fun_stub_base_1_call")
        
        int_result_obj = fun_call_obj._Call(uter.int(4))
        self.assertEqual(True, int_result_obj._GetValue() == 4, "")
        
        fun_stub_obj._Stub(self.stub_base_1_stub)
        int_result_obj = fun_call_obj._Call(uter.int(4))
        self.assertEqual(True, int_result_obj._GetValue() == 6, "")
        fun_stub_obj._Stub(None)
        
        int_result_obj = fun_call_obj._Call(uter.int(4))
        self.assertEqual(True, int_result_obj._GetValue() == 4, "")

    @staticmethod
    def stub_base_2_stub(a, b):
        print "--------------stub_base_2_stub running--------------"
        v1 = a._GetValue()
        v2 = b._GetValue()
        r = v1 - v2
        ret = uter.int(r)
        return ret
    
    def test_stub_base_2(self):
        fun_stub_obj = uter.export.function("fun_stub_base_2_stub")
        fun_call_obj = uter.export.function("fun_stub_base_2_call")
        
        int_result_obj = fun_call_obj._Call(uter.int(4), uter.int(2))
        self.assertEqual(True, int_result_obj._GetValue() == 6, "")
        
        fun_stub_obj._Stub(self.stub_base_2_stub)
        int_result_obj = fun_call_obj._Call(uter.int(4), uter.int(2))
        self.assertEqual(True, int_result_obj._GetValue() == 2, "")
        fun_stub_obj._Stub(None)
        
        int_result_obj = fun_call_obj._Call(uter.int(4), uter.int(2))
        self.assertEqual(True, int_result_obj._GetValue() == 6, "")
        
    @staticmethod
    def stub_base_3_stub(a):
        print "--------------stub_base_3_stub running--------------"
        v1 = a._GetValue()
        ret = uter.int(v1 + 2)
        return ret
    
    def test_stub_base_3(self):
        fun_stub_obj = uter.export.function("fun_stub_base_3_stub")
        fun_call_obj = uter.export.function("fun_stub_base_3_call")
        
        int_result_obj = fun_call_obj._Call(uter.char(4))
        self.assertEqual(True, int_result_obj._GetValue() == 4, "")
        
        fun_stub_obj._Stub(self.stub_base_3_stub)
        int_result_obj = fun_call_obj._Call(uter.char(4))
        self.assertEqual(True, int_result_obj._GetValue() == 6, "")
        fun_stub_obj._Stub(None)
        
        int_result_obj = fun_call_obj._Call(uter.char(4))
        self.assertEqual(True, int_result_obj._GetValue() == 4, "")

    @staticmethod
    def stub_base_4_stub(a, b):
        print "--------------stub_base_4_stub running--------------"
        v1 = a._GetValue()
        v2 = b._GetValue()
        r = v1 - v2
        ret = uter.int(r)
        return ret
    
    def test_stub_base_4(self):
        fun_stub_obj = uter.export.function("fun_stub_base_4_stub")
        fun_call_obj = uter.export.function("fun_stub_base_4_call")
        
        int_result_obj = fun_call_obj._Call(uter.char(4), uter.char(2))
        self.assertEqual(True, int_result_obj._GetValue() == 6, "")
        
        fun_stub_obj._Stub(self.stub_base_4_stub)
        int_result_obj = fun_call_obj._Call(uter.char(4), uter.char(2))
        self.assertEqual(True, int_result_obj._GetValue() == 2, "")
        fun_stub_obj._Stub(None)
        
        int_result_obj = fun_call_obj._Call(uter.char(4), uter.char(2))
        self.assertEqual(True, int_result_obj._GetValue() == 6, "")
        
    @staticmethod
    def stub_base_5_stub(a, b, c):
        print "--------------stub_base_5_stub running--------------"
        v1 = a._GetValue()
        v2 = b._GetValue()
        v3 = b._GetValue()
        r = v1 - v2- v3
        ret = uter.int(r)
        return ret
    
    def test_stub_base_5(self):
        fun_stub_obj = uter.export.function("fun_stub_base_5_stub")
        fun_call_obj = uter.export.function("fun_stub_base_5_call")
        
        int_result_obj = fun_call_obj._Call(uter.char(8), uter.int(2), uter.char(2))
        self.assertEqual(True, int_result_obj._GetValue() == 12, "")
        
        fun_stub_obj._Stub(self.stub_base_5_stub)
        int_result_obj = fun_call_obj._Call(uter.char(8), uter.int(2), uter.char(2))
        self.assertEqual(True, int_result_obj._GetValue() == 4, "")
        fun_stub_obj._Stub(None)
        
        int_result_obj = fun_call_obj._Call(uter.char(8), uter.int(2), uter.char(2))
        self.assertEqual(True, int_result_obj._GetValue() == 12, "")
        
    @staticmethod
    def stub_base_6_stub(a, b, c):
        print "--------------stub_base_6_stub running--------------"
        v1 = a._GetValue()
        v2 = b._GetValue()
        v3 = b._GetValue()
        r = v1 - v2- v3
        ret = uter.int(r)
        return ret
    
    def test_stub_base_6(self):
        fun_stub_obj = uter.export.function("fun_stub_base_6_stub")
        fun_call_obj = uter.export.function("fun_stub_base_6_call")
        
        int_result_obj = fun_call_obj._Call(uter.int(8), uter.char(2), uter.int(2))
        self.assertEqual(True, int_result_obj._GetValue() == 12, "")
        
        fun_stub_obj._Stub(self.stub_base_6_stub)
        int_result_obj = fun_call_obj._Call(uter.int(8), uter.char(2), uter.int(2))
        self.assertEqual(True, int_result_obj._GetValue() == 4, "")
        fun_stub_obj._Stub(None)
        
        int_result_obj = fun_call_obj._Call(uter.int(8), uter.char(2), uter.int(2))
        self.assertEqual(True, int_result_obj._GetValue() == 12, "")

    @staticmethod
    def stub_base_7_stub(a):
        print "--------------stub_base_7_stub running--------------"
        v1 = a._GetValue()
        ret = uter.int(v1 + 2)
        return ret
    
    def test_stub_base_7(self):
        fun_stub_obj = uter.export.function("fun_stub_base_7_stub")
        fun_call_obj = uter.export.function("fun_stub_base_7_call")
        
        int_result_obj = fun_call_obj._Call(uter.short(4))
        self.assertEqual(True, int_result_obj._GetValue() == 4, "")
        
        fun_stub_obj._Stub(self.stub_base_7_stub)
        int_result_obj = fun_call_obj._Call(uter.short(4))
        self.assertEqual(True, int_result_obj._GetValue() == 6, "")
        fun_stub_obj._Stub(None)
        
        int_result_obj = fun_call_obj._Call(uter.short(4))
        self.assertEqual(True, int_result_obj._GetValue() == 4, "")

    @staticmethod
    def stub_base_8_stub(a, b):
        print "--------------stub_base_8_stub running--------------"
        v1 = a._GetValue()
        v2 = b._GetValue()
        r = v1 - v2
        ret = uter.int(r)
        return ret
    
    def test_stub_base_8(self):
        fun_stub_obj = uter.export.function("fun_stub_base_8_stub")
        fun_call_obj = uter.export.function("fun_stub_base_8_call")
        
        int_result_obj = fun_call_obj._Call(uter.short(4), uter.short(2))
        self.assertEqual(True, int_result_obj._GetValue() == 6, "")
        
        fun_stub_obj._Stub(self.stub_base_8_stub)
        int_result_obj = fun_call_obj._Call(uter.short(4), uter.short(2))
        self.assertEqual(True, int_result_obj._GetValue() == 2, "")
        fun_stub_obj._Stub(None)
        
        int_result_obj = fun_call_obj._Call(uter.short(4), uter.short(2))
        self.assertEqual(True, int_result_obj._GetValue() == 6, "")
        
    @staticmethod
    def stub_base_9_stub(a, b, c):
        print "--------------stub_base_9_stub running--------------"
        v1 = a._GetValue()
        v2 = b._GetValue()
        v3 = b._GetValue()
        r = v1 - v2- v3
        ret = uter.int(r)
        return ret
    
    def test_stub_base_9(self):
        fun_stub_obj = uter.export.function("fun_stub_base_9_stub")
        fun_call_obj = uter.export.function("fun_stub_base_9_call")
        
        int_result_obj = fun_call_obj._Call(uter.short(8), uter.int(2), uter.short(2))
        self.assertEqual(True, int_result_obj._GetValue() == 12, "")
        
        fun_stub_obj._Stub(self.stub_base_9_stub)
        int_result_obj = fun_call_obj._Call(uter.short(8), uter.int(2), uter.short(2))
        self.assertEqual(True, int_result_obj._GetValue() == 4, "")
        fun_stub_obj._Stub(None)
        
        int_result_obj = fun_call_obj._Call(uter.short(8), uter.int(2), uter.short(2))
        self.assertEqual(True, int_result_obj._GetValue() == 12, "")
        
    @staticmethod
    def stub_base_10_stub(a, b, c):
        print "--------------stub_base_10_stub running--------------"
        v1 = a._GetValue()
        v2 = b._GetValue()
        v3 = b._GetValue()
        r = v1 - v2- v3
        ret = uter.int(r)
        return ret
    
    def test_stub_base_10(self):
        fun_stub_obj = uter.export.function("fun_stub_base_10_stub")
        fun_call_obj = uter.export.function("fun_stub_base_10_call")
        
        int_result_obj = fun_call_obj._Call(uter.int(8), uter.short(2), uter.int(2))
        self.assertEqual(True, int_result_obj._GetValue() == 12, "")
        
        fun_stub_obj._Stub(self.stub_base_10_stub)
        int_result_obj = fun_call_obj._Call(uter.int(8), uter.short(2), uter.int(2))
        self.assertEqual(True, int_result_obj._GetValue() == 4, "")
        fun_stub_obj._Stub(None)
        
        int_result_obj = fun_call_obj._Call(uter.int(8), uter.short(2), uter.int(2))
        self.assertEqual(True, int_result_obj._GetValue() == 12, "")

    @staticmethod
    def stub_base_11_stub(a):
        print "--------------stub_base_11_stub running--------------"
        v1 = a._GetValue()
        ret = uter.int(v1 + 2)
        return ret
    
    def test_stub_base_11(self):
        fun_stub_obj = uter.export.function("fun_stub_base_11_stub")
        fun_call_obj = uter.export.function("fun_stub_base_11_call")
        
        int_result_obj = fun_call_obj._Call(uter.longlong(4))
        self.assertEqual(True, int_result_obj._GetValue() == 4, "")
        
        fun_stub_obj._Stub(self.stub_base_11_stub)
        int_result_obj = fun_call_obj._Call(uter.longlong(4))
        self.assertEqual(True, int_result_obj._GetValue() == 6, "")
        fun_stub_obj._Stub(None)
        
        int_result_obj = fun_call_obj._Call(uter.longlong(4))
        self.assertEqual(True, int_result_obj._GetValue() == 4, "")

    @staticmethod
    def stub_base_12_stub(a, b):
        print "--------------stub_base_12_stub running--------------"
        v1 = a._GetValue()
        v2 = b._GetValue()
        r = v1 - v2
        ret = uter.int(r)
        return ret
    
    def test_stub_base_12(self):
        fun_stub_obj = uter.export.function("fun_stub_base_12_stub")
        fun_call_obj = uter.export.function("fun_stub_base_12_call")
        
        int_result_obj = fun_call_obj._Call(uter.longlong(4), uter.longlong(2))
        self.assertEqual(True, int_result_obj._GetValue() == 6, "")
        
        fun_stub_obj._Stub(self.stub_base_12_stub)
        int_result_obj = fun_call_obj._Call(uter.longlong(4), uter.longlong(2))
        self.assertEqual(True, int_result_obj._GetValue() == 2, "")
        fun_stub_obj._Stub(None)
        
        int_result_obj = fun_call_obj._Call(uter.longlong(4), uter.longlong(2))
        self.assertEqual(True, int_result_obj._GetValue() == 6, "")
        
    @staticmethod
    def stub_base_13_stub(a, b, c):
        print "--------------stub_base_13_stub running--------------"
        v1 = a._GetValue()
        v2 = b._GetValue()
        v3 = b._GetValue()
        r = v1 - v2- v3
        ret = uter.int(r)
        return ret
    
    def test_stub_base_13(self):
        fun_stub_obj = uter.export.function("fun_stub_base_13_stub")
        fun_call_obj = uter.export.function("fun_stub_base_13_call")
        
        int_result_obj = fun_call_obj._Call(uter.longlong(8), uter.int(2), uter.longlong(2))
        self.assertEqual(True, int_result_obj._GetValue() == 12, "")
        
        fun_stub_obj._Stub(self.stub_base_13_stub)
        int_result_obj = fun_call_obj._Call(uter.longlong(8), uter.int(2), uter.longlong(2))
        self.assertEqual(True, int_result_obj._GetValue() == 4, "")
        fun_stub_obj._Stub(None)
        
        int_result_obj = fun_call_obj._Call(uter.longlong(8), uter.int(2), uter.longlong(2))
        self.assertEqual(True, int_result_obj._GetValue() == 12, "")
    
    @staticmethod
    def stub_base_14_stub(a, b, c):
        print "--------------stub_base_14_stub running--------------"
        v1 = a._GetValue()
        v2 = b._GetValue()
        v3 = b._GetValue()
        r = v1 - v2- v3
        ret = uter.int(r)
        return ret
    
    def test_stub_base_14(self):
        fun_stub_obj = uter.export.function("fun_stub_base_14_stub")
        fun_call_obj = uter.export.function("fun_stub_base_14_call")
        
        int_result_obj = fun_call_obj._Call(uter.int(8), uter.longlong(2), uter.int(2))
        self.assertEqual(True, int_result_obj._GetValue() == 12, "")
        
        fun_stub_obj._Stub(self.stub_base_14_stub)
        int_result_obj = fun_call_obj._Call(uter.int(8), uter.longlong(2), uter.int(2))
        self.assertEqual(True, int_result_obj._GetValue() == 4, "")
        fun_stub_obj._Stub(None)
        
        int_result_obj = fun_call_obj._Call(uter.int(8), uter.longlong(2), uter.int(2))
        self.assertEqual(True, int_result_obj._GetValue() == 12, "")

    @staticmethod
    def stub_base_v_stub(a):
        print "--------------stub_base_v_stub running--------------"
        obj = uter.depoint(a)
        obj._SetValue(99)
        return uter.void
    
    def test_stub_base_v(self):
        fun_stub_obj = uter.export.function("fun_stub_base_v_stub")
        fun_call_obj = uter.export.function("fun_stub_base_v_call")
        
        int_result_obj = fun_call_obj._Call(uter.int(4))
        self.assertEqual(True, int_result_obj._GetValue() == 55, "")
        
        fun_stub_obj._Stub(self.stub_base_v_stub)
        int_result_obj = fun_call_obj._Call(uter.int(4))
        self.assertEqual(True, int_result_obj._GetValue() == 99, "")
        fun_stub_obj._Stub(None)
        
        int_result_obj = fun_call_obj._Call(uter.int(4))
        self.assertEqual(True, int_result_obj._GetValue() == 55, "")
    
    @staticmethod
    def stub_base_ll_stub(a):
        print "--------------stub_base_ll_stub running--------------"
        ret = uter.longlong(0xffffffff + 8)
        return ret
    
    def test_stub_base_ll(self):
        fun_stub_obj = uter.export.function("fun_stub_base_ll_stub")
        fun_call_obj = uter.export.function("fun_stub_base_ll_call")
        
        int_result_obj = fun_call_obj._Call(uter.int(4))
        self.assertEqual(True, int_result_obj._GetValue() == 0xffffffff + 4, "")
        
        fun_stub_obj._Stub(self.stub_base_ll_stub)
        int_result_obj = fun_call_obj._Call(uter.int(4))
        self.assertEqual(True, int_result_obj._GetValue() == 0xffffffff + 8, "")
        fun_stub_obj._Stub(None)
        
        int_result_obj = fun_call_obj._Call(uter.int(4))
        self.assertEqual(True, int_result_obj._GetValue() == 0xffffffff + 4, "")

    @staticmethod
    def stub_starg_1_stub(x):
        print "--------------stub_starg_1_stub running--------------"
        v = x.a._GetValue()
        return uter.int(v + 4)

    def test_stub_starg_1(self):
        fun_stub_obj = uter.export.function("fun_stub_starg_1_stub")
        fun_call_obj = uter.export.function("fun_stub_starg_1_call")
        
        int_result_obj = fun_call_obj._Call(uter.int(4))
        self.assertEqual(True, int_result_obj._GetValue() == 4, "")
        
        fun_stub_obj._Stub(self.stub_starg_1_stub)
        int_result_obj = fun_call_obj._Call(uter.int(4))
        self.assertEqual(True, int_result_obj._GetValue() == 8, "")
        fun_stub_obj._Stub(None)
        
        int_result_obj = fun_call_obj._Call(uter.int(4))
        self.assertEqual(True, int_result_obj._GetValue() == 4, "")

    @staticmethod
    def stub_starg_2_stub(x):
        print "--------------stub_starg_2_stub running--------------"
        v1 = x.a._GetValue()
        v2 = x.b._GetValue()
        return uter.int(v1 + v2 + 4)

    def test_stub_starg_2(self):
        fun_stub_obj = uter.export.function("fun_stub_starg_2_stub")
        fun_call_obj = uter.export.function("fun_stub_starg_2_call")
        
        int_result_obj = fun_call_obj._Call(uter.int(4), uter.int(5))
        self.assertEqual(True, int_result_obj._GetValue() == 9, "")
        
        fun_stub_obj._Stub(self.stub_starg_2_stub)
        int_result_obj = fun_call_obj._Call(uter.int(4), uter.int(5))
        self.assertEqual(True, int_result_obj._GetValue() == 13, "")
        fun_stub_obj._Stub(None)
        
        int_result_obj = fun_call_obj._Call(uter.int(4), uter.int(5))
        self.assertEqual(True, int_result_obj._GetValue() == 9, "")

    @staticmethod
    def stub_starg_3_stub(x):
        print "--------------stub_starg_3_stub running--------------"
        v1 = x.a._GetValue()
        v2 = x.b._GetValue()
        v3 = x.c._GetValue()
        return uter.int(v1 + v2 + v3 + 4)

    def test_stub_starg_3(self):
        fun_stub_obj = uter.export.function("fun_stub_starg_3_stub")
        fun_call_obj = uter.export.function("fun_stub_starg_3_call")
        
        int_result_obj = fun_call_obj._Call(uter.int(4), uter.int(5), uter.int(6))
        self.assertEqual(True, int_result_obj._GetValue() == 15, "")
        
        fun_stub_obj._Stub(self.stub_starg_3_stub)
        int_result_obj = fun_call_obj._Call(uter.int(4), uter.int(5), uter.int(6))
        self.assertEqual(True, int_result_obj._GetValue() == 19, "")
        fun_stub_obj._Stub(None)
        
        int_result_obj = fun_call_obj._Call(uter.int(4), uter.int(5), uter.int(6))
        self.assertEqual(True, int_result_obj._GetValue() == 15, "")

    @staticmethod
    def stub_starg_4_stub(x, y):
        print "--------------stub_starg_4_stub running--------------"
        v1 = x.a._GetValue()
        v2 = x.b._GetValue()
        v3 = x.c._GetValue()
        v = y._GetValue()
        return uter.int(v1 + v2 + v3 + v + 4)

    def test_stub_starg_4(self):
        fun_stub_obj = uter.export.function("fun_stub_starg_4_stub")
        fun_call_obj = uter.export.function("fun_stub_starg_4_call")
        
        int_result_obj = fun_call_obj._Call(uter.int(4), uter.int(5), uter.int(6), uter.int(7))
        self.assertEqual(True, int_result_obj._GetValue() == 22, "")
        
        fun_stub_obj._Stub(self.stub_starg_4_stub)
        int_result_obj = fun_call_obj._Call(uter.int(4), uter.int(5), uter.int(6), uter.int(7))
        self.assertEqual(True, int_result_obj._GetValue() == 26, "")
        fun_stub_obj._Stub(None)
        
        int_result_obj = fun_call_obj._Call(uter.int(4), uter.int(5), uter.int(6), uter.int(7))
        self.assertEqual(True, int_result_obj._GetValue() == 22, "")
        
    @staticmethod
    def stub_stret_1_stub(x, y, z):
        print "--------------stub_stret_1_stub running--------------"
        v1 = x._GetValue()
        v2 = y._GetValue()
        v3 = z._GetValue()
        sttype = uter.export.struct("fun_stub_stret_st_1")
        stobj = sttype()
        stobj.a._SetValue(v1 + 4)
        stobj.b._SetValue(v2 + 4)
        stobj.c._SetValue(v3 + 4)
        return stobj

    def test_stub_stret_1(self):
        fun_stub_obj = uter.export.function("fun_stub_stret_1_stub")
        fun_call_obj = uter.export.function("fun_stub_stret_1_call")
        
        int_result_obj = fun_call_obj._Call(uter.int(4), uter.int(5), uter.int(6))
        self.assertEqual(True, int_result_obj._GetValue() == 15, "")
        
        fun_stub_obj._Stub(self.stub_stret_1_stub)
        int_result_obj = fun_call_obj._Call(uter.int(4), uter.int(5), uter.int(6))
        self.assertEqual(True, int_result_obj._GetValue() == 27, "")
        fun_stub_obj._Stub(None)
        
        int_result_obj = fun_call_obj._Call(uter.int(4), uter.int(5), uter.int(6))
        self.assertEqual(True, int_result_obj._GetValue() == 15, "")

    @staticmethod
    def stub_unarg_1_stub(x):
        print "--------------stub_unarg_1_stub running--------------"
        v1 = x.a._GetValue()
        return uter.int(v1 + 4)

    def test_stub_unarg_1(self):
        fun_stub_obj = uter.export.function("fun_stub_unarg_1_stub")
        fun_call_obj = uter.export.function("fun_stub_unarg_1_call")
        
        int_result_obj = fun_call_obj._Call(uter.int(4), uter.int(5))
        self.assertEqual(True, int_result_obj._GetValue() == 4, "")
        
        fun_stub_obj._Stub(self.stub_unarg_1_stub)
        int_result_obj = fun_call_obj._Call(uter.int(4), uter.int(5))
        self.assertEqual(True, int_result_obj._GetValue() == 8, "")
        fun_stub_obj._Stub(None)
        
        int_result_obj = fun_call_obj._Call(uter.int(4), uter.int(5))
        self.assertEqual(True, int_result_obj._GetValue() == 4, "")
        
    @staticmethod
    def stub_unret_1_stub(x, y, z):
        print "--------------stub_unret_1_stub running--------------"
        v1 = x._GetValue()
        sttype = uter.export.union("fun_stub_unret_un_1")
        stobj = sttype()
        stobj.a._SetValue(v1 + 4)
        return stobj

    def test_stub_unret_1(self):
        fun_stub_obj = uter.export.function("fun_stub_unret_1_stub")
        fun_call_obj = uter.export.function("fun_stub_unret_1_call")
        
        int_result_obj = fun_call_obj._Call(uter.int(4), uter.int(5), uter.int(6))
        self.assertEqual(True, int_result_obj._GetValue() == 4, "")
        
        fun_stub_obj._Stub(self.stub_unret_1_stub)
        int_result_obj = fun_call_obj._Call(uter.int(4), uter.int(5), uter.int(6))
        self.assertEqual(True, int_result_obj._GetValue() == 8, "")
        fun_stub_obj._Stub(None)
        
        int_result_obj = fun_call_obj._Call(uter.int(4), uter.int(5), uter.int(6))
        self.assertEqual(True, int_result_obj._GetValue() == 4, "")
