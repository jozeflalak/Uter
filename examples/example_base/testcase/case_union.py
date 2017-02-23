#coding=utf-8

import uter
import unittest

class test_case_export_type(unittest.TestCase):
    def test_print(self):
        datatype_1 = uter.export.union("un_base_1")
        datatype_2 = uter.export.union("un_base_2")
        print datatype_1
        print datatype_2
        self.assertEqual(True, True, "")
    def test_size(self):
        datatype_1 = uter.export.union("un_base_1")
        datatype_2 = uter.export.union("un_base_2")
        self.assertEqual(True, datatype_1._GetSize() == 4, "")
        self.assertEqual(True, datatype_2._GetSize() == 4, "")
    def test_align(self):
        datatype_1 = uter.export.union("un_base_1")
        datatype_2 = uter.export.union("un_base_2")
        self.assertEqual(True, datatype_1._GetAlign() == 4, "")
        self.assertEqual(True, datatype_2._GetAlign() == 4, "")
    def test_defdesc(self):
        datatype_1 = uter.export.union("un_base_1")
        datatype_2 = uter.export.union("un_base_2")
        self.assertEqual(True, datatype_1._DefDesc() == "union un_base_1 ", "") 
        self.assertEqual(True, datatype_2._DefDesc() == "union un_base_2 ", "")
    
    def test_mem(self):
        datatype_1 = uter.export.union("un_base_1")
        datatype_2 = uter.export.union("un_base_2")
        self.assertEqual(True, datatype_1.m_1 == uter.uint, "")
        self.assertEqual(True, datatype_2.m_1 == uter.uint, "")
        self.assertEqual(True, datatype_2.m_2 == uter.uchar, "")
        
    def test_match(self):
        datatype_1 = uter.export.union("un_base_1")
        self.assertEqual(True, datatype_1 != 1, "")
        datatype_2 = uter.export.union("un_base_2")
        self.assertEqual(True, datatype_1 != 2, "")
        datatype_3 = uter.export.union("un_base_1")
        self.assertEqual(True, datatype_1 != 3, "")
        
        self.assertEqual(True, datatype_1 == datatype_1 and datatype_2 == datatype_2 and datatype_3 == datatype_3, "")
        self.assertEqual(True, datatype_1 != datatype_2 and datatype_1 == datatype_3, "")
        self.assertEqual(True, datatype_2 != datatype_1 and datatype_3 == datatype_1, "")
        self.assertEqual(True, datatype_2 != datatype_3, "")
        self.assertEqual(True, datatype_3 != datatype_2, "")
        
class test_case_export_var(unittest.TestCase):
    def test_print(self):
        dataobj_1 = uter.export.variable("un_base_1_var")
        dataobj_2 = uter.export.variable("un_base_2_var")
        print dataobj_1
        print dataobj_2
        self.assertEqual(True, True, "")
    def test_size(self):
        dataobj_1 = uter.export.variable("un_base_1_var")
        dataobj_2 = uter.export.variable("un_base_2_var")
        self.assertEqual(True, dataobj_1._GetSize() == 4, "")
        self.assertEqual(True, dataobj_2._GetSize() == 4, "")
    def test_align(self):
        dataobj_1 = uter.export.variable("un_base_1_var")
        dataobj_2 = uter.export.variable("un_base_2_var")
        self.assertEqual(True, dataobj_1._GetAlign() == 4, "")
        self.assertEqual(True, dataobj_2._GetAlign() == 4, "")
    def test_mem(self):
        dataobj_1 = uter.export.variable("un_base_1_var")
        dataobj_2 = uter.export.variable("un_base_2_var")
        self.assertEqual(True, dataobj_1.m_1._GetType() == uter.uint, "")
        self.assertEqual(True, dataobj_2.m_1._GetType() == uter.uint, "")
        self.assertEqual(True, dataobj_2.m_2._GetType() == uter.uchar, "")
        
class test_case_def(unittest.TestCase):
    def test_wrong(self):        
        except_happen = False
        try:
            datatype = uter.union(1)
            print datatype
        except uter.error.ArgError, e:
            except_happen = True
            print e
        self.assertEqual(True, except_happen, "")
        
        except_happen = False
        try:
            datatype = uter.union("x")
            print datatype
        except uter.error.ArgError, e:
            except_happen = True
            print e
        self.assertEqual(True, except_happen, "")
        
        except_happen = False
        try:
            datatype = uter.union((1,))
            print datatype
        except uter.error.ArgError, e:
            except_happen = True
            print e
        self.assertEqual(True, except_happen, "")
        
        except_happen = False
        try:
            datatype = uter.union((uter.int,))
            print datatype
        except uter.error.ArgError, e:
            except_happen = True
            print e
        self.assertEqual(True, except_happen, "")
        
        except_happen = False
        try:
            datatype = uter.union((uter.int, ""))
            print datatype
        except uter.error.ArgError, e:
            except_happen = True
            print e
        self.assertEqual(True, except_happen, "")
        
        except_happen = False
        try:
            datatype = uter.union((uter.int, "a"), (uter.char, "a"))
            print datatype
        except uter.error.ArgError, e:
            except_happen = True
            print e
        self.assertEqual(True, except_happen, "")

    def test_create(self):
        datatype_1 = uter.union((uter.int, "a"), (uter.char, "b"))
        datatype_2 = uter.union([uter.int, "a"], [uter.char, "b"])
        print datatype_1
        print datatype_2
        self.assertEqual(True, True, "")

    def test_size(self):
        datatype = uter.union((uter.int, "a"), (uter.char, "b"))
        self.assertEqual(4, datatype._GetSize(), "")
        
    def test_align(self):
        datatype = uter.union((uter.int, "a"), (uter.char, "b"))
        self.assertEqual(4, datatype._GetAlign(), "")
    
    def test_asdef(self):
        datatype = uter.union((uter.int, "a"), (uter.char, "b"))
        self.assertEqual("union (no name) ", datatype._DefDesc(), "")
        
    def test_match(self):
        datatype_1 = uter.union((uter.int, "a"), (uter.char, "b"))
        self.assertEqual(True, datatype_1 != 1, "")
        datatype_2 = uter.union((uter.int, "a"), (uter.int, "b"))
        self.assertEqual(True, datatype_2 != 1, "")
        datatype_3 = uter.union((uter.int, "a"), (uter.char, "b"))
        self.assertEqual(True, datatype_3 != 1, "")
        datatype_4 = uter.union([uter.int, "a"], [uter.char, "b"])
        self.assertEqual(True, datatype_4 != 1, "")
        datatype_5 = uter.union([uter.int, "a"], [uter.int, "b"])
        self.assertEqual(True, datatype_5 != 1, "")
        datatype_6 = uter.union([uter.int, "a"], [uter.char, "b"])
        self.assertEqual(True, datatype_6 != 1, "")
        
        self.assertEqual(True, datatype_1 == datatype_1 and datatype_2 == datatype_2 and datatype_3 == datatype_3, "")
        self.assertEqual(True, datatype_1 != datatype_2 and datatype_1 == datatype_3, "")
        self.assertEqual(True, datatype_2 != datatype_1 and datatype_3 == datatype_1, "")
        self.assertEqual(True, datatype_2 != datatype_3, "")
        self.assertEqual(True, datatype_3 != datatype_2, "")
        self.assertEqual(True, datatype_1 ==  datatype_4, "")
        self.assertEqual(True, datatype_2 ==  datatype_5, "")
        self.assertEqual(True, datatype_3 ==  datatype_6, "")
        
class test_case_new(unittest.TestCase):
    def test_export_type_new_def_val(self):
        datatype_1 = uter.export.union("un_base_1")
        datatype_2 = uter.export.union("un_base_2")
        dataobj_1 = datatype_1()
        dataobj_2 = datatype_2()
        print dataobj_1
        print dataobj_2
        self.assertEqual(True, dataobj_1.m_1._GetValue() == 0, "")
        self.assertEqual(True, dataobj_2.m_1._GetValue() == 0, "")
        self.assertEqual(True, dataobj_2.m_2._GetValue() == 0, "")
        
    def test_export_type_new_mem_noval(self):
        datatype_1 = uter.export.union("un_base_1")
        datatype_2 = uter.export.union("un_base_2")
        dataobj_1 = datatype_1("m_1")
        dataobj_2 = datatype_2("m_1")
        print dataobj_1
        print dataobj_2
        self.assertEqual(True, dataobj_1.m_1._GetValue() == 0, "")
        self.assertEqual(True, dataobj_2.m_1._GetValue() == 0, "")
        self.assertEqual(True, dataobj_2.m_2._GetValue() == 0, "")
        
    def test_export_type_new_mem_eval(self):
        datatype_1 = uter.export.union("un_base_1")
        datatype_2 = uter.export.union("un_base_2")
        dataobj_1 = datatype_1("m_1", ())
        dataobj_21 = datatype_2("m_1", ())
        dataobj_22 = datatype_2("m_2", ())
        print dataobj_1
        print dataobj_21
        print dataobj_22
        self.assertEqual(True, dataobj_1.m_1._GetValue() == 0, "")
        self.assertEqual(True, dataobj_21.m_1._GetValue() == 0, "") 
        self.assertEqual(True, dataobj_21.m_2._GetValue() == 0, "")
        self.assertEqual(True, dataobj_22.m_1._GetValue() == 0, "") 
        self.assertEqual(True, dataobj_22.m_2._GetValue() == 0, "")
    def test_export_type_new_mem_num(self):
        datatype_1 = uter.export.union("un_base_1")
        datatype_2 = uter.export.union("un_base_2")
        dataobj_1 = datatype_1("m_1", (8, ))
        dataobj_21 = datatype_2("m_1", (8, ))
        dataobj_22 = datatype_2("m_2", (8, ))
        print dataobj_1
        print dataobj_21
        print dataobj_22
        self.assertEqual(True, dataobj_1.m_1._GetValue() == 8, "")
        self.assertEqual(True, dataobj_21.m_1._GetValue() == 8, "") 
        self.assertEqual(True, dataobj_21.m_2._GetValue() == 8, "")
        self.assertEqual(True, dataobj_22.m_1._GetValue() == 8, "") 
        self.assertEqual(True, dataobj_22.m_2._GetValue() == 8, "")
    def test_export_type_new_mem_obj(self):
        datatype_1 = uter.export.union("un_base_1")
        datatype_2 = uter.export.union("un_base_2")
        arg = uter.uint(9)
        dataobj_1 = datatype_1("m_1", (arg, ))
        dataobj_21 = datatype_2("m_1", (arg, ))
        arg = uter.uchar(9)
        dataobj_22 = datatype_2("m_2", (arg, ))
        print dataobj_1
        print dataobj_21
        print dataobj_22
        self.assertEqual(True, dataobj_1.m_1._GetValue() == 9, "")
        self.assertEqual(True, dataobj_21.m_1._GetValue() == 9, "") 
        self.assertEqual(True, dataobj_21.m_2._GetValue() == 9, "")
        self.assertEqual(True, dataobj_22.m_1._GetValue() == 9, "") 
        self.assertEqual(True, dataobj_22.m_2._GetValue() == 9, "")
    def test_export_type_new_maxval(self):
        datatype_2 = uter.export.union("un_base_2")
        dataobj_2 = datatype_2("m_1", (2 ** 32 - 1, ))
        print dataobj_2
        self.assertEqual(True, dataobj_2.m_1._GetValue() == 2 ** 32 - 1, "") 
        self.assertEqual(True, dataobj_2.m_2._GetValue() == 255, "")
        
    def test_export_type_new_wrong(self):                
        datatype = uter.export.union("un_base_1")
        except_happen = False
        try:
            data = datatype("m_111111", (9, ))
        except uter.error.ArgError, e:
            except_happen = True
            print e
        self.assertEqual(True, except_happen, "")
        
        datatype = uter.export.union("un_base_1")
        except_happen = False
        try:
            arg = uter.int(9)
            data = datatype("m_1", (arg, ))
        except uter.error.ArgError, e:
            except_happen = True
            print e
        self.assertEqual(True, except_happen, "")

    def test_def_type_new_def_val(self):
        datatype = uter.union((uter.uint, "a"), (uter.uchar, "b"))
        data = datatype()
        print data
        self.assertEqual(True, data.a._GetValue() == 0, "") 
        self.assertEqual(True, data.b._GetValue() == 0, "")
        
    def test_def_type_new_mem_noval(self):
        datatype = uter.union((uter.uint, "a"), (uter.uchar, "b"))
        data = datatype("a")
        print data
        self.assertEqual(True, data.a._GetValue() == 0, "")
        self.assertEqual(True, data.b._GetValue() == 0, "")
        
    def test_def_type_new_mem_eval(self):
        datatype = uter.union((uter.uint, "a"), (uter.uchar, "b"))
        data = datatype("a", ())
        print data
        self.assertEqual(True, data.a._GetValue() == 0, "")
        self.assertEqual(True, data.b._GetValue() == 0, "")
        
    def test_def_type_new_mem_num(self):
        datatype = uter.union((uter.uint, "a"), (uter.uchar, "b"))
        data_a = datatype("a", (4, ))
        data_b = datatype("b", (4, ))
        print data_a
        print data_b
        self.assertEqual(True, data_a.a._GetValue() == 4, "") 
        self.assertEqual(True, data_a.b._GetValue() == 4, "")
        self.assertEqual(True, data_b.a._GetValue() == 4, "")
        self.assertEqual(True, data_b.b._GetValue() == 4, "")
        
    def test_def_type_new_mem_obj(self):
        datatype = uter.union((uter.uint, "a"), (uter.uchar, "b"))
        arg = uter.uint(4)
        data_a = datatype("a", (arg, ))
        arg = uter.uchar(4)
        data_b = datatype("b", (arg, ))
        print data_a
        print data_b
        self.assertEqual(True, data_a.a._GetValue() == 4, "") 
        self.assertEqual(True, data_a.b._GetValue() == 4, "")
        self.assertEqual(True, data_b.a._GetValue() == 4, "")
        self.assertEqual(True, data_b.b._GetValue() == 4, "")

    def test_def_type_new_maxval(self):
        datatype = uter.union((uter.uint, "a"), (uter.uchar, "b"))
        data = datatype("a", (2 ** 32 - 1, ))
        print data
        self.assertEqual(True, data.a._GetValue() == 2 ** 32 - 1, "")
        self.assertEqual(True, data.b._GetValue() == 255, "")

    def test_def_type_new_wrong(self):             
        datatype = uter.union((uter.uint, "a"), (uter.uchar, "b"))
        except_happen = False
        try:
            data = datatype("aaaaaa", (1, ))
        except uter.error.ArgError, e:
            except_happen = True
            print e
        self.assertEqual(True, except_happen, "")

class test_case_obj(unittest.TestCase):
    def test_export_var(self):
        dataobj_1 = uter.export.variable("un_base_1_var")
        dataobj_2 = uter.export.variable("un_base_2_var")
        datatype_1 = uter.export.union("un_base_1")
        datatype_2 = uter.export.union("un_base_2")
        self.assertEqual(True, not datatype_1._MatchObj(4), "") 
        self.assertEqual(True, not datatype_2._MatchObj(4), "")
        self.assertEqual(True, datatype_1._MatchObj(dataobj_1), "")
        self.assertEqual(True, not datatype_1._MatchObj(dataobj_2), "")
        self.assertEqual(True, not datatype_2._MatchObj(dataobj_1), "")
        self.assertEqual(True, datatype_2._MatchObj(dataobj_2), "")
                
    def test_new_export_type(self):
        datatype_1 = uter.export.union("un_base_1")
        datatype_2 = uter.export.union("un_base_2")
        dataobj_1 = datatype_1()
        dataobj_2 = datatype_2()
        self.assertEqual(True, not datatype_1._MatchObj(4), "") 
        self.assertEqual(True, not datatype_2._MatchObj(4), "")
        self.assertEqual(True, datatype_1._MatchObj(dataobj_1), "")
        self.assertEqual(True, not datatype_1._MatchObj(dataobj_2), "")
        self.assertEqual(True, not datatype_2._MatchObj(dataobj_1), "")
        self.assertEqual(True, datatype_2._MatchObj(dataobj_2), "")
        
    def test_new_def_type(self):
        datatype_1 = uter.union((uter.int, "a"), (uter.int, "b"))
        datatype_2 = uter.union((uter.char, "a"), (uter.char, "b"))
        dataobj_1 = datatype_1()
        dataobj_2 = datatype_2()
        self.assertEqual(True, not datatype_1._MatchObj(4), "") 
        self.assertEqual(True, not datatype_2._MatchObj(4), "")
        self.assertEqual(True, datatype_1._MatchObj(dataobj_1), "")
        self.assertEqual(True, not datatype_1._MatchObj(dataobj_2), "")
        self.assertEqual(True, not datatype_2._MatchObj(dataobj_1), "")
        self.assertEqual(True, datatype_2._MatchObj(dataobj_2), "")
        
class test_case_mem(unittest.TestCase):
    def test_base(self):
        datatype_1 = uter.export.union("un_base_1")
        datatype_2 = uter.export.union("un_base_2")
        dataobj_1 = datatype_1()
        dataobj_2 = datatype_2()
        dataobj_1.m_1._SetValue(10)
        dataobj_2.m_1._SetValue(10)
        dataobj_2.m_2._SetValue(11)
        self.assertEqual(True, dataobj_1.m_1._GetValue() == 10, "")
        self.assertEqual(True, dataobj_2.m_1._GetValue() == 11, "")
        self.assertEqual(True, dataobj_2.m_2._GetValue() == 11, "")
        
    def test_def(self):
        datatype = uter.union((uter.int, "a"), (uter.char, "b"))
        data = datatype()
        data.a._SetValue(10)
        data.b._SetValue(11)
        self.assertEqual(True, 11 == data.a._GetValue(), "")
        self.assertEqual(True, 11 == data.b._GetValue(), "")
        
class test_case_complex(unittest.TestCase):
    def test_export_type(self):
        datatype = uter.export.union("someinun_1_out_1")
        print datatype
        self.assertEqual(True, datatype._GetSize() == 8, "")
        self.assertEqual(True, datatype._GetAlign() == 4, "")
        self.assertEqual(True, datatype._DefDesc() == "union someinun_1_out_1 ", "")
        
    def test_export_type_1(self):
        datatype = uter.export.union("someinun_1_out_1")
        datatypem1 = datatype.m_1
        print datatypem1
        datatypein = uter.export.struct("someinun_1_in_1")
        self.assertEqual(True, datatypem1._GetSize() == 8, "")
        self.assertEqual(True, datatypem1._GetAlign() == 4, "")
        self.assertEqual(True, datatypem1._DefDesc() == "struct someinun_1_in_1 ", "")
        self.assertEqual(True, datatypein._MatchType(datatypem1), "")        
        
    def test_export_type_2(self):
        datatype = uter.export.union("someinun_1_out_1")
        datatypem2 = datatype.m_2
        print datatypem2
        datatypein = uter.export.union("someinun_1_in_2")
        self.assertEqual(True, datatypem2._GetSize() == 4, "")
        self.assertEqual(True, datatypem2._GetAlign() == 4, "")
        self.assertEqual(True, datatypem2._DefDesc() == "union someinun_1_in_2 ", "")
        self.assertEqual(True, datatypein._MatchType(datatypem2), "")

    def test_export_type_3(self):
        datatype = uter.export.union("someinun_1_out_1")
        datatypem3 = datatype.m_3
        print datatypem3
        datatypein = uter.export.struct("someinun_1_in_3")
        self.assertEqual(True, datatypem3._GetSize() == 8, "")
        self.assertEqual(True, datatypem3._GetAlign() == 1, "")
        self.assertEqual(True, datatypem3._DefDesc() == "struct someinun_1_in_3 ", "")
        self.assertEqual(True, datatypein._MatchType(datatypem3), "")
    
    def test_export_type_4(self):
        datatype = uter.export.union("someinun_1_out_1")
        datatypem4 = datatype.m_4
        self.assertEqual(uter.int, datatypem4, "")
        
    def test_def_type(self):
        datatypein1 = uter.struct((uter.int, "a"), (uter.int, "b"))
        datatypein2 = uter.int
        datatypein3 = uter.struct((uter.char, "a"), (uter.char, "b"), (uter.char, "c"), (uter.char, "d"))
        datatype = uter.union((datatypein1, "m_1"), (datatypein2, "m_2"), (datatypein3, "m_3"), (uter.int, "m_4"))
        print datatype
        self.assertEqual(True, datatype._GetSize() == 8, "")
        self.assertEqual(True, datatype._GetAlign() == 4, "")
        self.assertEqual(True, datatype._DefDesc() == "union (no name) ", "")
        
    def test_def_type(self):
        datatypein1 = uter.struct((uter.int, "a"), (uter.int, "b"))
        datatypein2 = uter.int
        datatypein3 = uter.struct((uter.char, "a"), (uter.char, "b"), (uter.char, "c"), (uter.char, "d"))
        datatype = uter.union((datatypein1, "m_1"), (datatypein2, "m_2"), (datatypein3, "m_3"), (uter.int, "m_4"))
        datatypem1 = datatype.m_1
        print datatypem1
        datatypem2 = datatype.m_2
        print datatypem2
        self.assertEqual(True, datatypem1._MatchType(datatypein1), "")
        self.assertEqual(True, datatypem2._MatchType(datatypein2), "")
        
    def test_export_var(self):
        data = uter.export.variable("someinun_1_out_1_var")
        datatype = uter.export.union("someinun_1_out_1")
        m1 = data.m_1
        m2 = data.m_2
        m3 = data.m_3
        datatype1 = uter.export.struct("someinun_1_in_1")
        datatype2 = uter.export.union("someinun_1_in_2")
        datatype3 = uter.export.struct("someinun_1_in_3")
        self.assertEqual(True, datatype._MatchObj(data), "")
        self.assertEqual(True, datatype1._MatchObj(m1), "")
        self.assertEqual(True, datatype2._MatchObj(m2), "")
        self.assertEqual(True, datatype3._MatchObj(m3), "")
    
    def test_new(self):
        datatype = uter.export.union("someinun_1_out_1")
        data11 = datatype("m_1", (2, 3))
        print data11
        self.assertEqual(True, data11.m_1.m_1._GetValue() == 2, "")
        self.assertEqual(True, data11.m_1.m_2._GetValue() == 3, "")
        data12 = datatype("m_1", (uter.uchar(2), uter.uint(3)))
        print data12
        self.assertEqual(True, data12.m_1.m_1._GetValue() == 2, "")
        self.assertEqual(True, data12.m_1.m_2._GetValue() == 3, "")
        data20 = datatype("m_2", ("m_1", (10, )))
        print data20
        self.assertEqual(True, data20.m_2.m_1._GetValue() == 10, "")
        data30 = datatype("m_3", (1, 2, 3, 4, 5, 6, 7, 8))
        print data30
        self.assertEqual(True, data30.m_3.m_1._GetValue() == 1, "")
        self.assertEqual(True, data30.m_3.m_2._GetValue() == 2, "")
        self.assertEqual(True, data30.m_3.m_3._GetValue() == 3, "")
        self.assertEqual(True, data30.m_3.m_4._GetValue() == 4, "")
        self.assertEqual(True, data30.m_3.m_5._GetValue() == 5, "")
        self.assertEqual(True, data30.m_3.m_6._GetValue() == 6, "")
        self.assertEqual(True, data30.m_3.m_7._GetValue() == 7, "")
        self.assertEqual(True, data30.m_3.m_8._GetValue() == 8, "")
        
class test_case_pystyle(unittest.TestCase):
    def test_type_dot(self):
        datatype = uter.export.union("unname_1")
        print datatype
        m1 = datatype.m_1
        print m1
        
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
            m4 = datatype.___m_4
            print m4
        except Exception, e:
            except_happen = True
            print e
        self.assertEqual(True, except_happen, "")
        
    def test_type_mem(self):
        datatype = uter.export.union("unname_1")
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
        result = True
        data = uter.export.variable("unname_1_var")
        print data
        m1 = data.m_1
        print m1
        
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
            m3 = data.__m_3
            print m3
        except Exception, e:
            except_happen = True
            print e
        self.assertEqual(True, except_happen, "")
        
    def test_obj_mem(self):
        data = uter.export.variable("unname_1_var")
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
       
        