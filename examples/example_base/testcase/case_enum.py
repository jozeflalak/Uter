#coding=utf-8

import uter
import unittest

class test_case_export_type(unittest.TestCase):
    def test_type(self):
        datatype_1 = uter.export.enum("en_base_1")
        print datatype_1
        self.assertEqual(True, datatype_1._GetSize() == 4, "")
        self.assertEqual(True, datatype_1._GetAlign() == 4, "")
        self.assertEqual(True, datatype_1._DefDesc() == "enum en_base_1 " , "")
        self.assertEqual(True, datatype_1.EN1_M0 == 0, "")
        self.assertEqual(True, datatype_1.EN1_M1 == 1, "")
        self.assertEqual(True, datatype_1.EN1_M2 == 2, "")
        self.assertEqual(True, datatype_1.EN1_M3 == -1, "")
        self.assertEqual(True, datatype_1.EN1_M4 == 0, "")
        
        datatype_2 = uter.export.enum("en_base_2")
        print datatype_2
        self.assertEqual(True, datatype_1 == datatype_1, "")
        self.assertEqual(True, datatype_2._GetSize() == 4 , "")
        self.assertEqual(True, datatype_2._GetAlign() == 4 , "")
        self.assertEqual(True, datatype_2._DefDesc() == "enum en_base_2 " , "")
        self.assertEqual(True, datatype_2.EN2_M0 == -4, "")
        self.assertEqual(True, datatype_2.EN2_M1 == 5, "")
        self.assertEqual(True, datatype_2.EN2_M2 == 6, "")
        self.assertEqual(True, datatype_2.EN2_M3 == 5, "")
        self.assertEqual(True, datatype_2.EN2_M4 == 6, "")
        
        datatype_3 = uter.export.enum("en_base_1")
        print datatype_3
        self.assertEqual(True, datatype_1 != datatype_2, "")
        self.assertEqual(True, datatype_2 != datatype_1, "")
        self.assertEqual(True, datatype_2 != datatype_3, "")
        self.assertEqual(True, datatype_3 != datatype_2, "")
        self.assertEqual(True, datatype_1 == datatype_3, "") 
        self.assertEqual(True, datatype_3 == datatype_1, "")
        
class test_case_export_var(unittest.TestCase):
    def test_var(self):
        datatype_1 = uter.export.enum("en_base_1")
        data_1 = uter.export.variable("en_base_1_var")
        print data_1
        self.assertEqual(True, data_1._GetSize() == 4, "")
        self.assertEqual(True, data_1._GetAlign() == 4, "")
        self.assertEqual(True, data_1.EN1_M0 == 0, "")
        self.assertEqual(True, data_1.EN1_M1 == 1, "")
        self.assertEqual(True, data_1.EN1_M2 == 2, "")
        self.assertEqual(True, data_1.EN1_M3 == -1, "")
        self.assertEqual(True, data_1.EN1_M4 == 0, "")
        
        datatype_2 = uter.export.enum("en_base_2")
        data_2 = uter.export.variable("en_base_2_var")
        print data_2
        self.assertEqual(True, data_2._GetSize() == 4, "")
        self.assertEqual(True, data_2._GetAlign() == 4, "")
        self.assertEqual(True, data_2.EN2_M0 == -4, "")
        self.assertEqual(True, data_2.EN2_M1 == 5, "")
        self.assertEqual(True, data_2.EN2_M2 == 6, "")
        self.assertEqual(True, data_2.EN2_M3 == 5, "")
        self.assertEqual(True, data_2.EN2_M4 == 6, "")
        
class test_case_def(unittest.TestCase):    
    def test_wrong(self):
        except_happen = False
        try:
            datatype = uter.enum(3)
            print datatype
        except uter.error.ArgError, e:
            except_happen = True
            print e
        self.assertEqual(True, except_happen, "")
        
        except_happen = False
        try:
            datatype = uter.enum(("E1", 4), ("E1", 4), ("E3", ), ("E4", ), ("E5", -5))
            print datatype
        except uter.error.ArgError, e:
            except_happen = True
            print e
        self.assertEqual(True, except_happen, "")

    def test_create(self):
        datatype = uter.enum(("E1", 4), ("E2", 4), ("E3", ), ("E4", ), ("E5", -5))
        print datatype
        self.assertEqual(True, datatype.E1 == 4, "")
        self.assertEqual(True, datatype.E2 == 4, "")
        self.assertEqual(True, datatype.E3 == 5, "")
        self.assertEqual(True, datatype.E4 == 6, "")
        self.assertEqual(True, datatype.E5 == -5, "")

        datatype = uter.enum(("E1", 4), ("E2", 4), "E3", "E4", ("E5", -5))
        print datatype
        self.assertEqual(True, datatype.E1 == 4, "")
        self.assertEqual(True, datatype.E2 == 4, "")
        self.assertEqual(True, datatype.E3 == 5, "")
        self.assertEqual(True, datatype.E4 == 6, "")
        self.assertEqual(True, datatype.E5 == -5, "")
        
        datatype = uter.enum(("E1", 4), ("E2", 4), ("E3"), ("E4"), ("E5", -5))
        print datatype
        self.assertEqual(True, datatype.E1 == 4, "")
        self.assertEqual(True, datatype.E2 == 4, "")
        self.assertEqual(True, datatype.E3 == 5, "")
        self.assertEqual(True, datatype.E4 == 6, "")
        self.assertEqual(True, datatype.E5 == -5, "")
        
        datatype = uter.enum(["E1", 4], ["E2", 4], ["E3"], ["E4"], ["E5", -5])
        print datatype
        self.assertEqual(True, datatype.E1 == 4, "")
        self.assertEqual(True, datatype.E2 == 4, "")
        self.assertEqual(True, datatype.E3 == 5, "")
        self.assertEqual(True, datatype.E4 == 6, "")
        self.assertEqual(True, datatype.E5 == -5, "")
        
        datatype = uter.enum(["E1", 4], ["E2", 4], ["E3", ], ["E4", ], ["E5", -5])
        print datatype
        self.assertEqual(True, datatype.E1 == 4, "")
        self.assertEqual(True, datatype.E2 == 4, "")
        self.assertEqual(True, datatype.E3 == 5, "")
        self.assertEqual(True, datatype.E4 == 6, "")
        self.assertEqual(True, datatype.E5 == -5, "")
        
    def test_atrr(self):
        datatype = uter.enum(("E1", 4), ("E2", 4), ("E3", ), ("E4", ), ("E5", -5))
        print datatype
        self.assertEqual(True, datatype._GetSize() == 4, "")
        self.assertEqual(True, datatype._GetAlign() == 4, "")
        self.assertEqual(True, datatype._DefDesc() == "enum (no name) ", "")
        
    def test_match(self):
        datatype_1 = uter.enum(("E1", 4), ("E2", 4), ("E3", ), ("E4", ), ("E5", -5))
        datatype_2 = uter.enum(("E1", 4), ("E2", 4), ("E3", ), ("E4", ), ("E5", -8))
        datatype_3 = uter.enum(("E1", 4), ("E2", 4), ("E3", ), ("E4", ), ("E5", -5))
        self.assertEqual(True, datatype_1 != datatype_2, "")
        self.assertEqual(True, datatype_2 != datatype_1, "")
        self.assertEqual(True, datatype_2 != datatype_3, "")
        self.assertEqual(True, datatype_3 != datatype_2, "")
        self.assertEqual(True, datatype_1 == datatype_3, "") 
        self.assertEqual(True, datatype_3 == datatype_1, "")
        
class test_case_new(unittest.TestCase):
    def test_export_type_new(self):
        datatype_1 = uter.export.enum("en_base_1")
        datatype_2 = uter.export.enum("en_base_2")
        dataobj_11 = datatype_1()
        print dataobj_11
        dataobj_12 = datatype_1(datatype_1.EN1_M2)
        print dataobj_12
        dataobj_21 = datatype_2()
        print dataobj_21
        dataobj_22 = datatype_2(datatype_2.EN2_M2)
        print dataobj_22
        
        self.assertEqual(True, dataobj_11._GetValue() == 0, "")
        self.assertEqual(True, dataobj_12._GetValue() == datatype_1.EN1_M2, "")
        self.assertEqual(True, dataobj_21._GetValue() == 0, "")
        self.assertEqual(True, dataobj_22._GetValue() == datatype_2.EN2_M2, "")
        
    def test_def_type_new(self):
        datatype_1 = uter.enum(("E1", 4), ("E2", 4), ("E3", ), ("E4", ), ("E5", -5))
        dataobj_1 = datatype_1()
        print dataobj_1
        dataobj_2 = datatype_1(datatype_1.E2)
        self.assertEqual(True, dataobj_1._GetValue() == 0, "")
        self.assertEqual(True, dataobj_2._GetValue() == datatype_1.E2, "")
        
    def test_new_args(self):
        datatype_1 = uter.enum(("E1", 4), ("E2", 4), ("E3", ), ("E4", ), ("E5", -5))
        dataobj_1 = datatype_1()
        dataobj_2 = datatype_1(datatype_1.E2)
        dataobj_3 = datatype_1(dataobj_2)
        self.assertEqual(True, dataobj_1._GetValue() == 0, "")
        self.assertEqual(True, dataobj_2._GetValue() == 4, "")
        self.assertEqual(True, dataobj_3._GetValue() == 4, "")
        
class test_case_obj(unittest.TestCase):
    def test_export(self):
        dataobj_1 = uter.export.variable("en_base_1_var")
        dataobj_2 = uter.export.variable("en_base_2_var")
        datatype_1 = uter.export.enum("en_base_1")
        datatype_2 = uter.export.enum("en_base_2")
        self.assertEqual(True, datatype_1._MatchObj(dataobj_1), "")
        self.assertEqual(True, datatype_2._MatchObj(dataobj_2), "")
        self.assertEqual(True, not datatype_1._MatchObj(dataobj_2), "")
        self.assertEqual(True, not datatype_2._MatchObj(dataobj_1), "")
        self.assertEqual(True, not datatype_1._MatchObj(1), "")
        self.assertEqual(True, not datatype_2._MatchObj(1), "")
    def test_def(self):
        deftype_1 = uter.enum(("E1", 4), ("E2", 4), ("E3", ), ("E4", ), ("E5", -5))
        deftype_2 = uter.enum(("E1", 4), ("E2", 4), ("E3", ), ("E4", ), ("E5", -8))
        deftype_3 = uter.enum(("E1", 4), ("E2", 4), ("E3", ), ("E4", ), ("E5", -8))
        dataen_1 = deftype_1(deftype_1.E2)
        dataen_2 = deftype_2(deftype_2.E2)
        dataen_3 = deftype_3(deftype_3.E2)
        self.assertEqual(True, deftype_1._MatchObj(dataen_1), "")
        self.assertEqual(True, deftype_2._MatchObj(dataen_2), "")
        self.assertEqual(True, not deftype_1._MatchObj(dataen_2), "")
        self.assertEqual(True, not deftype_2._MatchObj(dataen_1), "")
        self.assertEqual(True, not deftype_1._MatchObj(1), "")
        self.assertEqual(True, not deftype_2._MatchObj(1), "")
        self.assertEqual(True, deftype_2._MatchObj(dataen_3), "")
        
class test_case_pystyle(unittest.TestCase):    
    def test_type_dot(self):
        result = True
        datatype = uter.export.enum("enname_1")
        print datatype
        print datatype.ENNAME1
        
        except_happen = False
        try:
            print datatype._ENNAME2
        except Exception, e:
            except_happen = True
            print e
        self.assertEqual(True, except_happen, "")
        
        except_happen = False
        try:
            print datatype.__ENNAME3
        except Exception, e:
            except_happen = True
            print e
        self.assertEqual(True, except_happen, "")
        
        except_happen = False
        try:
            print datatype.___ENNAME4
        except Exception, e:
            except_happen = True
            print e
        self.assertEqual(True, except_happen, "")
        
    def test_type_mem(self):
        datatype = uter.export.enum("enname_1")
        print datatype._Member("ENNAME1")
        print datatype._Member("_ENNAME2")
        print datatype._Member("__ENNAME3")
        print datatype._Member("___ENNAME4")
        except_happen = False
        try:
            print datatype._Member("____ENNAME5")
        except uter.error.NotExist, e:
            except_happen = True
            print e
        self.assertEqual(True, except_happen, "")

        
        