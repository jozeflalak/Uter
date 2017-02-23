#coding=utf-8

import uter
import unittest

class test_case_export_type(unittest.TestCase):
    def test_wrong(self):        
        except_happen = False
        try:
            datatype = uter.export.struct("st_base_111111")
            print datatype
        except uter.error.NotExist, e:
            except_happen = True
            print e        
        self.assertEqual(True, except_happen, "")
        
    def test_print(self):
        datatype_1 = uter.export.struct("st_base_1")
        print datatype_1
        datatype_2 = uter.export.struct("st_base_2")
        print datatype_2
        datatype_3 = uter.export.struct("st_base_3")
        print datatype_3
        datatype_4 = uter.export.struct("st_base_4")
        print datatype_4
        self.assertEqual(True, True, "")
        
    def test_size(self):
        datatype_1 = uter.export.struct("st_base_1")
        datatype_2 = uter.export.struct("st_base_2")
        datatype_3 = uter.export.struct("st_base_3")
        datatype_4 = uter.export.struct("st_base_4")
        self.assertEqual(True, datatype_1._GetSize() == 4, "")
        self.assertEqual(True, datatype_2._GetSize() == 8, "")
        self.assertEqual(True, datatype_3._GetSize() == 2, "")
        self.assertEqual(True, datatype_4._GetSize() == 8, "")
    def test_align(self):
        datatype_1 = uter.export.struct("st_base_1")
        datatype_2 = uter.export.struct("st_base_2")
        datatype_3 = uter.export.struct("st_base_3")
        datatype_4 = uter.export.struct("st_base_4")
        self.assertEqual(True, datatype_1._GetAlign() == 4, "")
        self.assertEqual(True, datatype_2._GetAlign() == 4, "")
        self.assertEqual(True, datatype_3._GetAlign() == 1, "")
        self.assertEqual(True, datatype_4._GetAlign() == 4, "")
    def test_defdesc(self):
        datatype_1 = uter.export.struct("st_base_1")
        datatype_2 = uter.export.struct("st_base_2")
        datatype_3 = uter.export.struct("st_base_3")
        datatype_4 = uter.export.struct("st_base_4")
        self.assertEqual(True, datatype_1._DefDesc() == "struct st_base_1 ", "")
        self.assertEqual(True, datatype_2._DefDesc() == "struct st_base_2 ", "")
        self.assertEqual(True, datatype_3._DefDesc() == "struct st_base_3 ", "")
        self.assertEqual(True, datatype_4._DefDesc() == "struct st_base_4 ", "")
        
    def test_mem(self):
        datatype_1 = uter.export.struct("st_base_1")
        datatype_2 = uter.export.struct("st_base_2")
        datatype_3 = uter.export.struct("st_base_3")
        datatype_4 = uter.export.struct("st_base_4")
        self.assertEqual(True, datatype_1.m_1 == uter.int, "")
        self.assertEqual(True, datatype_2.m_1 == uter.int, "")
        self.assertEqual(True, datatype_2.m_2 == uter.int, "")
        self.assertEqual(True, datatype_3.m_1 == uter.char, "")
        self.assertEqual(True, datatype_3.m_2 == uter.char, "")
        self.assertEqual(True, datatype_4.m_1 == uter.char, "")
        self.assertEqual(True, datatype_4.m_2 == uter.int, "")
    def test_match(self):
        datatype_1 = uter.export.struct("st_base_1")
        self.assertEqual(True, datatype_1 != 1, "")
        datatype_2 = uter.export.struct("st_base_2")
        self.assertEqual(True, datatype_2 != 1, "")
        datatype_3 = uter.export.struct("st_base_3")
        self.assertEqual(True, datatype_3 != 1, "")
        datatype_4 = uter.export.struct("st_base_4")
        self.assertEqual(True, datatype_4 != 1, "")
        datatype_5 = uter.export.struct("st_base_1")
        self.assertEqual(True, datatype_5 != 1, "")
        
        self.assertEqual(True, datatype_1 == datatype_1 and datatype_2 == datatype_2 and datatype_3 == datatype_3 and datatype_4 == datatype_4 and datatype_5 == datatype_5, "")
        self.assertEqual(True, datatype_1 != datatype_2 and datatype_1 != datatype_3 and datatype_1 != datatype_4 and datatype_1 == datatype_5, "")
        self.assertEqual(True, datatype_2 != datatype_1 and datatype_3 != datatype_1 and datatype_4 != datatype_1 and datatype_5 == datatype_1, "")
        self.assertEqual(True, datatype_2 != datatype_3 and datatype_2 != datatype_4 and datatype_2 != datatype_5, "")
        self.assertEqual(True, datatype_3 != datatype_2 and datatype_4 != datatype_2 and datatype_5 != datatype_2, "")
        self.assertEqual(True, datatype_3 != datatype_4 and datatype_3 != datatype_5, "")
        self.assertEqual(True, datatype_4 != datatype_3 and datatype_5 != datatype_3, "")
        self.assertEqual(True, datatype_4 != datatype_5, "")
        self.assertEqual(True, datatype_5 != datatype_4, "")
        
class test_case_export_var(unittest.TestCase):
    def test_print(self):
        dataobj_1 = uter.export.variable("st_base_1_var")
        print dataobj_1
        dataobj_2 = uter.export.variable("st_base_2_var")
        print dataobj_2
        dataobj_3 = uter.export.variable("st_base_3_var")
        print dataobj_3
        dataobj_4 = uter.export.variable("st_base_4_var")
        print dataobj_4
        self.assertEqual(True, True, "")
    def test_size(self):
        dataobj_1 = uter.export.variable("st_base_1_var")
        self.assertEqual(True, dataobj_1._GetSize() == 4, "")
        dataobj_2 = uter.export.variable("st_base_2_var")
        self.assertEqual(True, dataobj_2._GetSize() == 8, "")
        dataobj_3 = uter.export.variable("st_base_3_var")
        self.assertEqual(True, dataobj_3._GetSize() == 2, "")
        dataobj_4 = uter.export.variable("st_base_4_var")
        self.assertEqual(True, dataobj_4._GetSize() == 8, "")
    def test_align(self):
        dataobj_1 = uter.export.variable("st_base_1_var")
        self.assertEqual(True, dataobj_1._GetAlign() == 4, "")
        dataobj_2 = uter.export.variable("st_base_2_var")
        self.assertEqual(True, dataobj_2._GetAlign() == 4, "")
        dataobj_3 = uter.export.variable("st_base_3_var")
        self.assertEqual(True, dataobj_3._GetAlign() == 1, "")
        dataobj_4 = uter.export.variable("st_base_4_var")
        self.assertEqual(True, dataobj_4._GetAlign() == 4, "")
    def test_gettype(self):
        dataobj_1 = uter.export.variable("st_base_1_var")
        datatype_1 = dataobj_1._GetType()
        print datatype_1
        dataobj_2 = uter.export.variable("st_base_2_var")
        datatype_2 = dataobj_2._GetType()
        print datatype_2
        dataobj_3 = uter.export.variable("st_base_3_var")
        datatype_3 = dataobj_3._GetType()
        print datatype_3
        dataobj_4 = uter.export.variable("st_base_4_var")
        datatype_4 = dataobj_4._GetType()
        print datatype_4
        self.assertEqual(True, True , "")
    def test_mem(self):
        dataobj_1 = uter.export.variable("st_base_1_var")
        print dataobj_1.m_1
        self.assertEqual(True, dataobj_1.m_1._GetType() == uter.int, "")
        
        dataobj_2 = uter.export.variable("st_base_2_var")
        print dataobj_2.m_1
        print dataobj_2.m_2
        self.assertEqual(True, dataobj_2.m_1._GetType() == uter.int, "")
        self.assertEqual(True, dataobj_2.m_2._GetType() == uter.int, "")

        dataobj_3 = uter.export.variable("st_base_3_var")
        print dataobj_3.m_1
        print dataobj_3.m_2
        self.assertEqual(True, dataobj_3.m_1._GetType() == uter.char, "")
        self.assertEqual(True, dataobj_3.m_2._GetType() == uter.char, "")

        dataobj_4 = uter.export.variable("st_base_4_var")
        print dataobj_4.m_1
        print dataobj_4.m_2        
        self.assertEqual(True, dataobj_4.m_1._GetType() == uter.char, "")
        self.assertEqual(True, dataobj_4.m_2._GetType() == uter.int, "")
        
class test_case_def(unittest.TestCase):
    def test_wrong(self):        
        except_happen = False
        try:
            datatype = uter.struct(1)
            print datatype
        except uter.error.ArgError, e:
            except_happen = True
            print e
        self.assertEqual(True, except_happen, "")
        
        except_happen = False
        try:
            datatype = uter.struct("x")
            print datatype
        except uter.error.ArgError, e:
            except_happen = True
            print e
        self.assertEqual(True, except_happen, "")
        
        except_happen = False
        try:
            datatype = uter.struct((1,))
            print datatype
        except uter.error.ArgError, e:
            except_happen = True
            print e
        self.assertEqual(True, except_happen, "")
        
        except_happen = False
        try:
            datatype = uter.struct((uter.int,))
            print datatype
        except uter.error.ArgError, e:
            except_happen = True
            print e
        self.assertEqual(True, except_happen, "")
        
        except_happen = False
        try:
            datatype = uter.struct((uter.int, ""))
            print datatype
        except uter.error.ArgError, e:
            except_happen = True
            print e
        self.assertEqual(True, except_happen, "")
        
        except_happen = False
        try:
            datatype = uter.struct((uter.int, "a"), (uter.char, "a"))
            print datatype
        except uter.error.ArgError, e:
            except_happen = True
            print e
        self.assertEqual(True, except_happen, "")

    def test_create(self):
        datatype_1 = uter.struct((uter.char, "a"), (uter.char, "b"))
        print datatype_1
        datatype_2 = uter.struct((uter.int, "a"), (uter.char, "b"))
        print datatype_2
        datatype_3 = uter.struct((uter.int, "a"), (uter.int, "b"))
        print datatype_3
        datatype_4 = uter.struct((uter.int, "a"), (uter.char, "b"), (uter.int, "c"))
        print datatype_4
        datatype_5 = uter.struct([uter.char, "a"], [uter.char, "b"])
        print datatype_5
        datatype_6 = uter.struct([uter.int, "a"], [uter.char, "b"])
        print datatype_6
        datatype_7 = uter.struct([uter.int, "a"], [uter.int, "b"])
        print datatype_7
        datatype_8 = uter.struct([uter.int, "a"], [uter.char, "b"], [uter.int, "c"])
        print datatype_8
        self.assertEqual(True, True, "")
       
    def test_size(self):
        datatype_1 = uter.struct((uter.char, "a"), (uter.char, "b"))
        datatype_2 = uter.struct((uter.int, "a"), (uter.char, "b"))
        datatype_3 = uter.struct((uter.int, "a"), (uter.int, "b"))
        datatype_4 = uter.struct((uter.int, "a"), (uter.char, "b"), (uter.int, "c"))
        datatype_5 = uter.struct([uter.char, "a"], [uter.char, "b"])
        datatype_6 = uter.struct([uter.int, "a"], [uter.char, "b"])
        datatype_7 = uter.struct([uter.int, "a"], [uter.int, "b"])
        datatype_8 = uter.struct([uter.int, "a"], [uter.char, "b"], [uter.int, "c"])
        self.assertEqual(True, datatype_1._GetSize() == 2, "")
        self.assertEqual(True, datatype_2._GetSize() == 8, "")
        self.assertEqual(True, datatype_3._GetSize() == 8, "")
        self.assertEqual(True, datatype_4._GetSize() == 12, "")
        self.assertEqual(True, datatype_5._GetSize() == 2, "")
        self.assertEqual(True, datatype_6._GetSize() == 8, "")
        self.assertEqual(True, datatype_7._GetSize() == 8, "")
        self.assertEqual(True, datatype_8._GetSize() == 12, "")
        
    def test_align(self):
        datatype_1 = uter.struct((uter.char, "a"), (uter.char, "b"))
        datatype_2 = uter.struct((uter.int, "a"), (uter.char, "b"))
        datatype_3 = uter.struct((uter.int, "a"), (uter.int, "b"))
        datatype_4 = uter.struct((uter.int, "a"), (uter.char, "b"), (uter.int, "c"))
        self.assertEqual(True, datatype_1._GetAlign() == 1, "")
        self.assertEqual(True, datatype_2._GetAlign() == 4, "")
        self.assertEqual(True, datatype_3._GetAlign() == 4, "")
        self.assertEqual(True, datatype_4._GetAlign() == 4, "")
        
    def test_asdef(self):
        datatype = uter.struct((uter.int, "a"), (uter.char, "b"))
        self.assertEqual("struct (no name) ", datatype._DefDesc(), "")
    
    def test_match(self):
        datatype_1 = uter.struct((uter.char, "a"), (uter.char, "b"))
        self.assertEqual(True, datatype_1 != 1, "")
        datatype_2 = uter.struct((uter.int, "a"), (uter.char, "b"))
        self.assertEqual(True, datatype_2 != 1, "")
        datatype_3 = uter.struct((uter.int, "a"), (uter.int, "b"))
        self.assertEqual(True, datatype_3 != 1, "")
        datatype_4 = uter.struct((uter.int, "a"), (uter.char, "b"), (uter.int, "c"))
        self.assertEqual(True, datatype_4 != 1, "")
        datatype_5 = uter.struct((uter.char, "a"), (uter.char, "b"))
        self.assertEqual(True, datatype_5 != 1, "")
        datatype_6 = uter.struct([uter.char, "a"], [uter.char, "b"])
        self.assertEqual(True, datatype_6 != 1, "")
        datatype_7 = uter.struct([uter.int, "a"], [uter.char, "b"])
        self.assertEqual(True, datatype_7 != 1, "")
        datatype_8 = uter.struct([uter.int, "a"], [uter.int, "b"])
        self.assertEqual(True, datatype_8 != 1, "")
        datatype_9 = uter.struct([uter.int, "a"], [uter.char, "b"], [uter.int, "c"])
        self.assertEqual(True, datatype_9 != 1, "")
        datatype_10 = uter.struct([uter.char, "a"], [uter.char, "b"])
        self.assertEqual(True, datatype_10 != 1, "")
        
        self.assertEqual(True, datatype_1 == datatype_1 and datatype_2 == datatype_2 and datatype_3 == datatype_3 and datatype_4 == datatype_4 and datatype_5 == datatype_5, "")
        self.assertEqual(True, datatype_1 != datatype_2 and datatype_1 != datatype_3 and datatype_1 != datatype_4 and datatype_1 == datatype_5, "")
        self.assertEqual(True, datatype_2 != datatype_1 and datatype_3 != datatype_1 and datatype_4 != datatype_1 and datatype_5 == datatype_1, "")
        self.assertEqual(True, datatype_2 != datatype_3 and datatype_2 != datatype_4 and datatype_2 != datatype_5, "")
        self.assertEqual(True, datatype_3 != datatype_2 and datatype_4 != datatype_2 and datatype_5 != datatype_2, "")
        self.assertEqual(True, datatype_3 != datatype_4 and datatype_3 != datatype_5, "")
        self.assertEqual(True, datatype_4 != datatype_3 and datatype_5 != datatype_3, "")
        self.assertEqual(True, datatype_4 != datatype_5, "")
        self.assertEqual(True, datatype_5 != datatype_4, "")
        self.assertEqual(True, datatype_1 == datatype_6 and datatype_2 == datatype_7 and datatype_3 == datatype_8 and datatype_4 == datatype_9 and datatype_5 == datatype_10, "")
        
class test_case_new(unittest.TestCase):   
    def test_new_export_type_def_val(self):
        datatype_1 = uter.export.struct("st_base_1")
        dataobj_1 = datatype_1()
        print dataobj_1
        self.assertEqual(True, dataobj_1.m_1._GetValue() == 0, "")
        
        datatype_2 = uter.export.struct("st_base_2")
        dataobj_2 = datatype_2()
        print dataobj_2
        self.assertEqual(True, dataobj_2.m_1._GetValue() == 0, "")
        self.assertEqual(True, dataobj_2.m_2._GetValue() == 0, "")
        
        datatype_3 = uter.export.struct("st_base_3")
        dataobj_3 = datatype_3()
        print dataobj_3
        self.assertEqual(True, dataobj_3.m_1._GetValue() == 0, "")
        self.assertEqual(True, dataobj_3.m_2._GetValue() == 0, "")
        
        datatype_4 = uter.export.struct("st_base_4")
        dataobj_4 = datatype_4()
        print dataobj_4
        self.assertEqual(True, dataobj_4.m_1._GetValue() == 0, "")
        self.assertEqual(True, dataobj_4.m_2._GetValue() == 0, "")
        
    def test_new_export_type_num_val(self):
        datatype_1 = uter.export.struct("st_base_1")
        dataobj_1 = datatype_1(8)
        print dataobj_1
        self.assertEqual(True, dataobj_1.m_1._GetValue() == 8, "")
        
        datatype_2 = uter.export.struct("st_base_2")
        dataobj_2 = datatype_2(9, 10)
        print dataobj_2
        self.assertEqual(True, dataobj_2.m_1._GetValue() == 9, "")
        self.assertEqual(True, dataobj_2.m_2._GetValue() == 10, "")
        
        datatype_3 = uter.export.struct("st_base_3")
        dataobj_3 = datatype_3(11, 12)
        print dataobj_3
        self.assertEqual(True, dataobj_3.m_1._GetValue() == 11, "") 
        self.assertEqual(True, dataobj_3.m_2._GetValue() == 12, "")
        
        datatype_4 = uter.export.struct("st_base_4")
        dataobj_4 = datatype_4(13, 14)
        print dataobj_4
        self.assertEqual(True, dataobj_4.m_1._GetValue() == 13, "")
        self.assertEqual(True, dataobj_4.m_2._GetValue() == 14, "")
    def test_new_export_type_obj_val(self):
        datatype_1 = uter.export.struct("st_base_1")
        dataobj_1 = datatype_1(uter.int(8))
        print dataobj_1
        self.assertEqual(True, dataobj_1.m_1._GetValue() == 8, "")
        
        datatype_2 = uter.export.struct("st_base_2")
        dataobj_2 = datatype_2(uter.int(9), uter.int(10))
        print dataobj_2
        self.assertEqual(True, dataobj_2.m_1._GetValue() == 9, "")
        self.assertEqual(True, dataobj_2.m_2._GetValue() == 10, "")
        
        datatype_3 = uter.export.struct("st_base_3")
        dataobj_3 = datatype_3(uter.char(11), uter.char(12))
        print dataobj_3
        self.assertEqual(True, dataobj_3.m_1._GetValue() == 11, "")
        self.assertEqual(True, dataobj_3.m_2._GetValue() == 12, "")
        
        datatype_4 = uter.export.struct("st_base_4")
        dataobj_4 = datatype_4(uter.char(13), uter.int(14))
        print dataobj_4
        self.assertEqual(True, dataobj_4.m_1._GetValue() == 13, "")
        self.assertEqual(True, dataobj_4.m_2._GetValue() == 14, "")
        
    def test_new_export_type_wrong(self):        
        except_happen = False
        try:
            datatype = uter.export.struct("st_base_1")
            data = datatype(8, 8)
            val = data.m_1._GetValue()
        except uter.error.ArgError, e:
            except_happen = True
            print e
        self.assertEqual(True, except_happen, "")
        
        except_happen = False
        try:
            datatype = uter.export.struct("st_base_1")
            data = datatype("")
            val = data.m_1._GetValue()
        except uter.error.ArgError, e:
            except_happen = True
            print e
        self.assertEqual(True, except_happen, "")
        
        except_happen = False
        try:
            datatype = uter.export.struct("st_base_1")
            arg = uter.char(9)
            data = datatype(arg)
            val = data.m_1._GetValue()
        except uter.error.ArgError, e:
            except_happen = True
            print e
        self.assertEqual(True, except_happen, "")
      
    def test_new_def_type_def_val(self):
        datatype_1 = uter.struct((uter.char, "a"), (uter.int, "b"))
        dataobj_1 = datatype_1()
        print dataobj_1
        self.assertEqual(True, dataobj_1.a._GetValue() == 0, "")
        
        datatype_2 = uter.struct((uter.char, "a"), (uter.short, "b"), (uter.int, "c"))
        dataobj_2 = datatype_2()
        print dataobj_2
        self.assertEqual(True, dataobj_2.a._GetValue() == 0, "") 
        self.assertEqual(True, dataobj_2.b._GetValue() == 0, "") 
        self.assertEqual(True, dataobj_2.c._GetValue() == 0, "")
        
        datatype_3 = uter.struct((uter.char, "a"), (uter.char, "b"))
        dataobj_3 = datatype_3()
        print dataobj_3
        self.assertEqual(True, dataobj_3.a._GetValue() == 0, "") 
        self.assertEqual(True, dataobj_3.b._GetValue() == 0, "")
        
        datatype_4 = uter.struct((uter.int, "a"), (uter.char, "b"),  (uter.char, "c"), (uter.char, "d"))
        dataobj_4 = datatype_4()
        print dataobj_4
        self.assertEqual(True, dataobj_4.a._GetValue() == 0, "") 
        self.assertEqual(True, dataobj_4.b._GetValue() == 0, "") 
        self.assertEqual(True, dataobj_4.c._GetValue() == 0, "") 
        self.assertEqual(True, dataobj_4.d._GetValue() == 0, "")
        
    def test_new_def_type_num_val(self):
        datatype_1 = uter.struct((uter.char, "a"), (uter.int, "b"))
        dataobj_1 = datatype_1(8, 9)
        print dataobj_1
        self.assertEqual(True, dataobj_1.a._GetValue() == 8, "")
        self.assertEqual(True, dataobj_1.b._GetValue() == 9, "")
        
        datatype_2 = uter.struct((uter.char, "a"), (uter.short, "b"), (uter.int, "c"))
        dataobj_2 = datatype_2(10, 11, 12)
        print dataobj_2
        self.assertEqual(True, dataobj_2.a._GetValue() == 10, "")
        self.assertEqual(True, dataobj_2.b._GetValue() == 11, "")
        self.assertEqual(True, dataobj_2.c._GetValue() == 12, "")
        
        datatype_3 = uter.struct((uter.char, "a"), (uter.char, "b"))
        dataobj_3 = datatype_3(13, 14)
        print dataobj_3
        self.assertEqual(True, dataobj_3.a._GetValue() == 13, "")
        self.assertEqual(True, dataobj_3.b._GetValue() == 14, "")
        
        datatype_4 = uter.struct((uter.int, "a"), (uter.char, "b"),  (uter.char, "c"), (uter.char, "d"))
        dataobj_4 = datatype_4(15, 16, 17, 18)
        print dataobj_4
        self.assertEqual(True, dataobj_4.a._GetValue() == 15, "")
        self.assertEqual(True, dataobj_4.b._GetValue() == 16, "")
        self.assertEqual(True, dataobj_4.c._GetValue() == 17, "")
        self.assertEqual(True, dataobj_4.d._GetValue() == 18, "")
        
    def test_new_def_type_obj_val(self):
        datatype_1 = uter.struct((uter.char, "a"), (uter.int, "b"))
        dataobj_1 = datatype_1(uter.char(8), uter.int(9))
        print dataobj_1
        self.assertEqual(True, dataobj_1.a._GetValue() == 8, "") 
        self.assertEqual(True, dataobj_1.b._GetValue() == 9, "")
        
        datatype_2 = uter.struct((uter.char, "a"), (uter.short, "b"), (uter.int, "c"))
        dataobj_2 = datatype_2(uter.char(10), uter.short(11), uter.int(12))
        print dataobj_2
        self.assertEqual(True, dataobj_2.a._GetValue() == 10, "") 
        self.assertEqual(True, dataobj_2.b._GetValue() == 11, "") 
        self.assertEqual(True, dataobj_2.c._GetValue() == 12, "")
        
        datatype_3 = uter.struct((uter.char, "a"), (uter.char, "b"))
        dataobj_3 = datatype_3(uter.char(13), uter.char(14))
        print dataobj_3
        self.assertEqual(True, dataobj_3.a._GetValue() == 13, "") 
        self.assertEqual(True, dataobj_3.b._GetValue() == 14, "")
        
        datatype_4 = uter.struct((uter.int, "a"), (uter.char, "b"),  (uter.char, "c"), (uter.char, "d"))
        dataobj_4 = datatype_4(uter.int(15), uter.char(16), uter.char(17), uter.char(18))
        print dataobj_4
        self.assertEqual(True, dataobj_4.a._GetValue() == 15, "") 
        self.assertEqual(True, dataobj_4.b._GetValue() == 16, "") 
        self.assertEqual(True, dataobj_4.c._GetValue() == 17, "")
        self.assertEqual(True, dataobj_4.d._GetValue() == 18, "")

    def test_new_def_type_wrong(self):        
        except_happen = False
        try:
            datatype = uter.struct((uter.char, "a"), (uter.int, "b"))
            data = datatype(1)
            val = data.b._GetValue()
        except uter.error.ArgError, e:
            except_happen = True
            print e
        self.assertEqual(True, except_happen, "")
        
        except_happen = False
        try:
            datatype = uter.struct((uter.char, "a"), (uter.int, "b"))
            data = datatype("")
            val = data.b._GetValue()
        except uter.error.ArgError, e:
            except_happen = True
            print e
        self.assertEqual(True, except_happen, "")
        
        except_happen = False
        try:
            datatype = uter.struct((uter.char, "a"), (uter.int, "b"))
            data = datatype(uter.char(5), uter.char(6))
            val = data.b._GetValue()
        except uter.error.ArgError, e:
            except_happen = True
            print e        
        self.assertEqual(True, except_happen, "")
    
class test_case_obj(unittest.TestCase):
    def test_export_var(self):
        dataobj_1 = uter.export.variable("st_base_1_var")
        datatype_1 = uter.export.struct("st_base_1")
        self.assertEqual(True, datatype_1._MatchObj(dataobj_1), "")
        self.assertEqual(True, not datatype_1._MatchObj(4), "")
        
        dataobj_2 = uter.export.variable("st_base_2_var")
        datatype_2 = uter.export.struct("st_base_2")
        self.assertEqual(True, datatype_2._MatchObj(dataobj_2), "")
        self.assertEqual(True, not datatype_2._MatchObj(4), "")
        
        dataobj_3 = uter.export.variable("st_base_3_var")
        datatype_3 = uter.export.struct("st_base_3")
        self.assertEqual(True, datatype_3._MatchObj(dataobj_3), "")
        self.assertEqual(True, not datatype_3._MatchObj(4), "")
        
        dataobj_4 = uter.export.variable("st_base_4_var")
        datatype_4 = uter.export.struct("st_base_4")
        self.assertEqual(True, datatype_4._MatchObj(dataobj_4), "")
        self.assertEqual(True, not datatype_4._MatchObj(4), "")

        self.assertEqual(True, not datatype_1._MatchObj(dataobj_2) and not datatype_1._MatchObj(dataobj_3) and not datatype_1._MatchObj(dataobj_4), "")
        self.assertEqual(True, not datatype_2._MatchObj(dataobj_1) and not datatype_2._MatchObj(dataobj_3) and not datatype_2._MatchObj(dataobj_4), "")
        self.assertEqual(True, not datatype_3._MatchObj(dataobj_1) and not datatype_3._MatchObj(dataobj_2) and not datatype_3._MatchObj(dataobj_4), "")
        self.assertEqual(True, not datatype_4._MatchObj(dataobj_1) and not datatype_4._MatchObj(dataobj_2) and not datatype_4._MatchObj(dataobj_3), "")
    
    def test_export_type_new(self):
        datatype_1 = uter.export.struct("st_base_1")
        dataobj_1 = datatype_1()
        self.assertEqual(True, datatype_1._MatchObj(dataobj_1), "")
        self.assertEqual(True, not datatype_1._MatchObj(4), "")
        
        datatype_2 = uter.export.struct("st_base_2")
        dataobj_2 = datatype_2()
        self.assertEqual(True, datatype_2._MatchObj(dataobj_2), "")
        self.assertEqual(True, not datatype_2._MatchObj(4), "")
        
        datatype_3 = uter.export.struct("st_base_3")
        dataobj_3 = datatype_3()
        self.assertEqual(True, datatype_3._MatchObj(dataobj_3), "")
        self.assertEqual(True, not datatype_3._MatchObj(4), "")
        
        datatype_4 = uter.export.struct("st_base_4")
        dataobj_4 = datatype_4()
        self.assertEqual(True, datatype_4._MatchObj(dataobj_4), "")
        self.assertEqual(True, not datatype_4._MatchObj(4), "")

        self.assertEqual(True, not datatype_1._MatchObj(dataobj_2) and not datatype_1._MatchObj(dataobj_3) and not datatype_1._MatchObj(dataobj_4), "")
        self.assertEqual(True, not datatype_2._MatchObj(dataobj_1) and not datatype_2._MatchObj(dataobj_3) and not datatype_2._MatchObj(dataobj_4), "")
        self.assertEqual(True, not datatype_3._MatchObj(dataobj_1) and not datatype_3._MatchObj(dataobj_2) and not datatype_3._MatchObj(dataobj_4), "")
        self.assertEqual(True, not datatype_4._MatchObj(dataobj_1) and not datatype_4._MatchObj(dataobj_2) and not datatype_4._MatchObj(dataobj_3), "")        
    
    def test_def_type_new(self):
        datatype_1 = uter.struct((uter.char, "a"), (uter.int, "b"))
        dataobj_1 = datatype_1()
        self.assertEqual(True, datatype_1._MatchObj(dataobj_1), "")
        self.assertEqual(True, not datatype_1._MatchObj(4), "")
        
        datatype_2 = uter.struct((uter.char, "a"), (uter.short, "b"), (uter.int, "c"))
        dataobj_2 = datatype_2()
        self.assertEqual(True, datatype_2._MatchObj(dataobj_2), "")
        self.assertEqual(True, not datatype_2._MatchObj(4), "")
        
        datatype_3 = uter.struct((uter.char, "a"), (uter.char, "b"))
        dataobj_3 = datatype_3()
        self.assertEqual(True, datatype_3._MatchObj(dataobj_3), "")
        self.assertEqual(True, not datatype_3._MatchObj(4), "")        
        
        datatype_4 = uter.struct((uter.int, "a"), (uter.char, "b"),  (uter.char, "c"), (uter.char, "d"))
        dataobj_4 = datatype_4()
        self.assertEqual(True, datatype_4._MatchObj(dataobj_4), "")
        self.assertEqual(True, not datatype_4._MatchObj(4), "")
        
        datatype_5 = uter.struct((uter.char, "a"), (uter.int, "b"))
        dataobj_5 = datatype_5()
        self.assertEqual(True, datatype_5._MatchObj(dataobj_5), "")
        self.assertEqual(True, not datatype_5._MatchObj(4), "")
        
        self.assertEqual(True, not datatype_1._MatchObj(dataobj_2) and not datatype_1._MatchObj(dataobj_3) and not datatype_1._MatchObj(dataobj_4) and         datatype_1._MatchObj(dataobj_5), "")
        self.assertEqual(True, not datatype_2._MatchObj(dataobj_1) and not datatype_2._MatchObj(dataobj_3) and not datatype_2._MatchObj(dataobj_4) and not datatype_2._MatchObj(dataobj_5), "")
        self.assertEqual(True, not datatype_3._MatchObj(dataobj_1) and not datatype_3._MatchObj(dataobj_2) and not datatype_3._MatchObj(dataobj_4) and not datatype_3._MatchObj(dataobj_5), "")
        self.assertEqual(True, not datatype_4._MatchObj(dataobj_1) and not datatype_4._MatchObj(dataobj_2) and not datatype_4._MatchObj(dataobj_3) and not datatype_4._MatchObj(dataobj_5), "")
        self.assertEqual(True, datatype_5._MatchObj(dataobj_1) and not datatype_5._MatchObj(dataobj_2) and not datatype_5._MatchObj(dataobj_3) and not datatype_5._MatchObj(dataobj_4), "")
    
class test_case_align(unittest.TestCase):
    def test_size(self):
        datatype_1 = uter.export.struct("st_align_1")
        self.assertEqual(True, datatype_1._GetSize() == 8, "")
        
        datatype_2 = uter.export.struct("st_align_2")
        self.assertEqual(True, datatype_2._GetSize() == 12, "")
        
        datatype_3 = uter.export.struct("st_align_3")
        self.assertEqual(True, datatype_3._GetSize() == 12, "")
        
        datatype_4 = uter.export.struct("st_align_4")
        self.assertEqual(True, datatype_4._GetSize() == 16, "")
        
    def test_align(self):
        datatype_1 = uter.export.struct("st_align_1")
        self.assertEqual(True, datatype_1._GetAlign() == 4, "")
        
        datatype_2 = uter.export.struct("st_align_2")
        self.assertEqual(True, datatype_2._GetAlign() == 4, "")
        
        datatype_3 = uter.export.struct("st_align_3")
        self.assertEqual(True, datatype_3._GetAlign() == 4, "")
        
        datatype_4 = uter.export.struct("st_align_4")
        self.assertEqual(True, datatype_4._GetAlign() == 4, "")
    
class test_case_mem(unittest.TestCase):
    def test_export_type(self):
        datatype_1 = uter.export.struct("st_base_1")
        dataobj_1 = datatype_1()
        dataobj_1.m_1._SetValue(10)
        self.assertEqual(True, dataobj_1.m_1._GetValue() == 10, "")
        
        datatype_2 = uter.export.struct("st_base_2")
        dataobj_2 = datatype_2()
        dataobj_2.m_1._SetValue(10)
        dataobj_2.m_2._SetValue(11)
        self.assertEqual(True, dataobj_2.m_1._GetValue() == 10 and dataobj_2.m_2._GetValue() == 11, "")
        
        datatype_3 = uter.export.struct("st_base_3")
        dataobj_3 = datatype_3()
        dataobj_3.m_1._SetValue(10)
        dataobj_3.m_2._SetValue(11)
        self.assertEqual(True, dataobj_3.m_1._GetValue() == 10 and dataobj_3.m_2._GetValue() == 11, "")
        
        datatype_4 = uter.export.struct("st_base_4")
        dataobj_4 = datatype_4()
        dataobj_4.m_1._SetValue(10)
        dataobj_4.m_2._SetValue(11)
        self.assertEqual(True, dataobj_4.m_1._GetValue() == 10 and dataobj_4.m_2._GetValue() == 11, "")
        
    def test_def_type(self):
        datatype = uter.struct((uter.int, "a"), (uter.char, "b"))
        data = datatype()
        data.a._SetValue(10)
        data.b._SetValue(11)
        self.assertEqual(True, 10 == data.a._GetValue() and 11 == data.b._GetValue(), "")
    
class test_case_complex(unittest.TestCase):
    def test_export_type(self):
        datatype = uter.export.struct("stinst_1_out_1")
        print datatype
        self.assertEqual(True, datatype._GetSize() == 20, "")
        self.assertEqual(True, datatype._GetAlign() == 4, "")
        self.assertEqual(True, datatype._DefDesc() == "struct stinst_1_out_1 ", "")
        
    def test_export_type_1(self):
        datatype = uter.export.struct("stinst_1_out_1")
        datatypem1 = datatype.m_1
        print datatypem1
        datatypein = uter.export.struct("stinst_1_in_1")
        self.assertEqual(True, datatypem1._GetSize() == 8, "")
        self.assertEqual(True, datatypem1._GetAlign() == 4, "")
        self.assertEqual(True, datatypem1._DefDesc() == "struct stinst_1_in_1 ", "")
        self.assertEqual(True, datatypein._MatchType(datatypem1), "")
        self.assertEqual(True, datatypem1._MatchType(datatypein), "")
    
    def test_export_type_2(self):
        datatype = uter.export.struct("stinst_1_out_1")
        datatypem2 = datatype.m_2
        print datatypem2
        datatypein = uter.export.struct("stinst_1_in_2")
        self.assertEqual(True, datatypem2._GetSize() == 8, "")
        self.assertEqual(True, datatypem2._GetAlign() == 4, "")
        self.assertEqual(True, datatypem2._DefDesc() == "struct stinst_1_in_2 ", "")
        self.assertEqual(True, datatypein._MatchType(datatypem2), "")
        self.assertEqual(True, datatypem2._MatchType(datatypein), "")
        
    def test_export_type_3(self):
        datatype = uter.export.struct("stinst_1_out_1")
        datatypem3 = datatype.m_3
        self.assertEqual(uter.int, datatypem3, "")
    
    def test_def_type(self):
        datatypein1 = uter.struct((uter.int, "a"), (uter.int, "b"))
        datatypein2 = uter.struct((uter.char, "a"), (uter.char, "b"))
        datatype = uter.struct((datatypein1, "m_1"), (datatypein2, "m_2"), (uter.int, "m_3"))
        print datatype
        self.assertEqual(True, datatype._GetSize() == 16, "")
        self.assertEqual(True, datatype._GetAlign() == 4, "")
        self.assertEqual(True, datatype._DefDesc() == "struct (no name) ", "")
        
    def test_def_type_1(self):
        datatypein1 = uter.struct((uter.int, "a"), (uter.int, "b"))
        datatypein2 = uter.struct((uter.char, "a"), (uter.char, "b"))
        datatype = uter.struct((datatypein1, "m_1"), (datatypein2, "m_2"), (uter.int, "m_3"))
        datatypem1 = datatype.m_1
        print datatypem1
        self.assertEqual(True, datatypem1._MatchType(datatypein1), "")
        self.assertEqual(True, datatypein1._MatchType(datatypem1), "")
        
    def test_export_var(self):
        data = uter.export.variable("stinst_1_out_1_var")
        print data
        datatype = uter.export.struct("stinst_1_out_1")
        self.assertEqual(True, datatype._MatchObj(data), "")
    
    def test_export_var_1(self):
        data = uter.export.variable("stinst_1_out_1_var")
        m1 = data.m_1
        print m1
        datatype = uter.export.struct("stinst_1_in_1")
        self.assertEqual(True, datatype._MatchObj(m1), "")
        
    def test_export_var_2(self):
        data = uter.export.variable("stinst_1_out_1_var")
        m2 = data.m_2
        print m2
        datatype = uter.export.struct("stinst_1_in_2")
        self.assertEqual(True, datatype._MatchObj(m2), "")
            
    def test_new_01(self):
        datatype = uter.export.struct("stinst_1_out_1")
        data = datatype((1, 2), (3, 4), 5)
        print data
        self.assertEqual(True, data.m_1.m_1._GetValue() == 1, "")
        self.assertEqual(True, data.m_1.m_2._GetValue() == 2, "")
        self.assertEqual(True, data.m_2.m_1._GetValue() == 3, "")
        self.assertEqual(True, data.m_2.m_2._GetValue() == 4, "")
        self.assertEqual(True, data.m_3._GetValue() == 5, "")
        
    def test_new_02(self):
        datatype = uter.export.struct("stinst_1_out_1")
        data1 = uter.char(1)
        data2 = uter.int(2)
        data3 = uter.int(3)
        data4 = uter.char(4)
        data5 = uter.int(5)
        data = datatype((data1, data2), (data3, data4), data5)
        print data
        self.assertEqual(True, data.m_1.m_1._GetValue() == 1, "")
        self.assertEqual(True, data.m_1.m_2._GetValue() == 2, "")
        self.assertEqual(True, data.m_2.m_1._GetValue() == 3, "")
        self.assertEqual(True, data.m_2.m_2._GetValue() == 4, "")
        self.assertEqual(True, data.m_3._GetValue() == 5, "")
                
    def test_mem_01(self):
        datatype = uter.export.struct("stinst_1_out_1")
        data = datatype()
        data.m_1.m_1._SetValue(1)
        data.m_1.m_2._SetValue(2)
        data.m_2.m_1._SetValue(3)
        data.m_2.m_2._SetValue(4)
        data.m_3._SetValue(5)
        self.assertEqual(True, data.m_1.m_1._GetValue() == 1, "")
        self.assertEqual(True, data.m_1.m_2._GetValue() == 2, "")
        self.assertEqual(True, data.m_2.m_1._GetValue() == 3, "")
        self.assertEqual(True, data.m_2.m_2._GetValue() == 4, "")
        self.assertEqual(True, data.m_3._GetValue() == 5, "")

class test_case_pystyle(unittest.TestCase):
    def test_type_dot(self):        
        datatype = uter.export.struct("stname_1")
        print datatype
        print datatype.m_1
        
        except_happen = False
        try:
            m2 = datatype._m_2
            print m2
        except Exception, e:
            except_happen = True
            print e
        self.assertEqual(True, except_happen, "")
        
        except_happen = False
        try:
            m3 = datatype.__m_3
            print m3
        except Exception, e:
            except_happen = True
            print e
        self.assertEqual(True, except_happen, "")
        
        except_happen = False
        try:
            m4 = datatype.__m_4
            print m4
        except Exception, e:
            except_happen = True
            print e        
        self.assertEqual(True, except_happen, "")
    
    def test_type_mem(self):        
        datatype = uter.export.struct("stname_1")
        m1 = datatype._Member("m_1")
        print m1
        m2 = datatype._Member("_m_2")
        print m2
        m3 = datatype._Member("__m_3")
        print m3
        m4 = datatype._Member("___m_4")
        print m4
        
        except_happen = False
        try:
            m4 = datatype._Member("___m_5")
            print m4
        except uter.error.NotExist, e:
            except_happen = True
            print e        
        self.assertEqual(True, except_happen, "")
        
    def test_obj_dot(self):
        data = uter.export.variable("stname_1_var")
        print data
        print data.m_1
        
        except_happen = False
        try:
            m2 = data._m_2
            print m2
        except Exception, e:
            except_happen = True
            print e
        self.assertEqual(True, except_happen, "")
        
        except_happen = False
        try:
            m3 = data.__m_3
            print m3
        except Exception, e:
            except_happen = True
            print e
        self.assertEqual(True, except_happen, "")
        
        except_happen = False
        try:
            m4 = data.__m_4
            print m4
        except Exception, e:
            except_happen = True
            print e
        self.assertEqual(True, except_happen, "")
    
    def test_obj_mem(self):        
        data = uter.export.variable("stname_1_var")
        m1 = data._Member("m_1")
        print m1
        m2 = data._Member("_m_2")
        print m2
        m3 = data._Member("__m_3")
        print m3
        m4 = data._Member("___m_4")
        print m4
        
        except_happen = False
        try:
            m4 = data._Member("___m_5")
            print m4
        except uter.error.NotExist, e:
            except_happen = True
            print e
        self.assertEqual(True, except_happen, "")
