#coding=utf-8

import uter
import unittest

class test_case_export_var(unittest.TestCase):    
    def test_base_1v(self):
        arraydata1 = uter.export.variable("array_int_1")
        arraydata2 = uter.export.variable("array_int_2")
        arraydata5 = uter.export.variable("array_int_5")
        print arraydata1
        print arraydata1._GetType()
        print arraydata2
        print arraydata2._GetType()
        print arraydata5
        print arraydata5._GetType()
        self.assertEqual(True, arraydata1._GetSize() == 4, "")
        self.assertEqual(True, arraydata2._GetSize() == 8, "")
        self.assertEqual(True, arraydata5._GetSize() == 20, "")
        self.assertEqual(True, arraydata1._GetAlign() == 4, "")
        self.assertEqual(True, arraydata2._GetAlign() == 4, "")
        self.assertEqual(True, arraydata5._GetAlign() == 4, "")
        self.assertEqual(True, arraydata1._GetFinalType() == uter.int, "")
        self.assertEqual(True, arraydata2._GetFinalType() == uter.int, "")
        self.assertEqual(True, arraydata5._GetFinalType() == uter.int, "")

    def test_base_2v(self):
        arraydata1 = uter.export.variable("array_int_31")
        arraydata2 = uter.export.variable("array_int_32")
        arraydata5 = uter.export.variable("array_int_35")
        print arraydata1
        print arraydata1._GetType()
        print arraydata2
        print arraydata2._GetType()
        print arraydata5
        print arraydata5._GetType()
        self.assertEqual(True, arraydata1._GetSize() == 12, "")
        self.assertEqual(True, arraydata2._GetSize() == 24, "")
        self.assertEqual(True, arraydata5._GetSize() == 60, "")
        self.assertEqual(True, arraydata1._GetAlign() == 4, "")
        self.assertEqual(True, arraydata2._GetAlign() == 4, "")
        self.assertEqual(True, arraydata5._GetAlign() == 4, "")
        self.assertEqual(True, arraydata1._GetFinalType() == uter.int, "")
        self.assertEqual(True, arraydata2._GetFinalType() == uter.int, "")
        self.assertEqual(True, arraydata5._GetFinalType() == uter.int, "")

    def test_st_1v(self):
        arraydata1 = uter.export.variable("array_st_1")
        arraydata2 = uter.export.variable("array_st_2")
        arraydata5 = uter.export.variable("array_st_5")
        print arraydata1
        print arraydata1._GetType()
        print arraydata2
        print arraydata2._GetType()
        print arraydata5
        print arraydata5._GetType()
        datatype = uter.export.struct("st_for_arry")
        self.assertEqual(True, arraydata1._GetSize() == 8, "")
        self.assertEqual(True, arraydata2._GetSize() == 16, "")
        self.assertEqual(True, arraydata5._GetSize() == 40, "")
        self.assertEqual(True, arraydata1._GetAlign() == 4, "")
        self.assertEqual(True, arraydata2._GetAlign() == 4, "")
        self.assertEqual(True, arraydata5._GetAlign() == 4, "")
        self.assertEqual(True, arraydata1._GetFinalType() == datatype, "")
        self.assertEqual(True, arraydata2._GetFinalType() == datatype, "")
        self.assertEqual(True, arraydata5._GetFinalType() == datatype, "")
        self.assertEqual(True, datatype == arraydata1._GetFinalType(), "")
        self.assertEqual(True, datatype == arraydata2._GetFinalType(), "")
        self.assertEqual(True, datatype == arraydata5._GetFinalType(), "")

    def test_st_2v(self):
        arraydata1 = uter.export.variable("array_st_31")
        arraydata2 = uter.export.variable("array_st_32")
        arraydata5 = uter.export.variable("array_st_35")
        print arraydata1
        print arraydata1._GetType()
        print arraydata2
        print arraydata2._GetType()
        print arraydata5
        print arraydata5._GetType()
        datatype = uter.export.struct("st_for_arry")
        self.assertEqual(True, arraydata1._GetSize() == 24, "")
        self.assertEqual(True, arraydata2._GetSize() == 48, "")
        self.assertEqual(True, arraydata5._GetSize() == 120, "")
        self.assertEqual(True, arraydata1._GetAlign() == 4, "")
        self.assertEqual(True, arraydata2._GetAlign() == 4, "")
        self.assertEqual(True, arraydata5._GetAlign() == 4, "")
        self.assertEqual(True, arraydata1._GetFinalType() == datatype, "")
        self.assertEqual(True, arraydata2._GetFinalType() == datatype, "")
        self.assertEqual(True, arraydata5._GetFinalType() == datatype, "")
        self.assertEqual(True, datatype == arraydata1._GetFinalType(), "")
        self.assertEqual(True, datatype == arraydata2._GetFinalType(), "")
        self.assertEqual(True, datatype == arraydata5._GetFinalType(), "")

class test_case_def(unittest.TestCase):
    def test_def_base(self):
        arraytype_1 = uter.array(uter.int, 5)
        arraytype_2 = uter.array(uter.int, (5))
        arraytype_3 = uter.array(uter.int, (5,))
        arraytype_4 = uter.array(uter.int, (5, 5))
        arraytype_5 = uter.array(uter.int, (5, 5, 4))
        arraytype_6 = uter.array(uter.int, [5])
        arraytype_7 = uter.array(uter.int, [5,])
        arraytype_8 = uter.array(uter.int, [5, 5])
        arraytype_9 = uter.array(uter.int, [5, 5, 4])
        print arraytype_1
        print arraytype_2
        print arraytype_3
        print arraytype_4
        print arraytype_5
        print arraytype_6
        print arraytype_7
        print arraytype_8
        print arraytype_9
        self.assertEqual(True, arraytype_1._GetSize() == 20, "")
        self.assertEqual(True, arraytype_2._GetSize() == 20, "")
        self.assertEqual(True, arraytype_3._GetSize() == 20, "")
        self.assertEqual(True, arraytype_4._GetSize() == 100, "")
        self.assertEqual(True, arraytype_5._GetSize() == 400, "")
        self.assertEqual(True, arraytype_6._GetSize() == 20, "")
        self.assertEqual(True, arraytype_7._GetSize() == 20, "")
        self.assertEqual(True, arraytype_8._GetSize() == 100, "")
        self.assertEqual(True, arraytype_9._GetSize() == 400, "")
        self.assertEqual(True, arraytype_1._GetAlign() == 4, "")
        self.assertEqual(True, arraytype_2._GetAlign() == 4, "")
        self.assertEqual(True, arraytype_3._GetAlign() == 4, "")
        self.assertEqual(True, arraytype_4._GetAlign() == 4, "")
        self.assertEqual(True, arraytype_5._GetAlign() == 4, "")
        self.assertEqual(True, arraytype_6._GetAlign() == 4, "")
        self.assertEqual(True, arraytype_7._GetAlign() == 4, "")
        self.assertEqual(True, arraytype_8._GetAlign() == 4, "")
        self.assertEqual(True, arraytype_9._GetAlign() == 4, "")
        self.assertEqual(True, arraytype_1._GetFinalType() == uter.int, "")
        self.assertEqual(True, arraytype_2._GetFinalType() == uter.int, "")
        self.assertEqual(True, arraytype_3._GetFinalType() == uter.int, "")
        self.assertEqual(True, arraytype_4._GetFinalType() == uter.int, "")
        self.assertEqual(True, arraytype_5._GetFinalType() == uter.int, "")
        self.assertEqual(True, arraytype_6._GetFinalType() == uter.int, "")
        self.assertEqual(True, arraytype_7._GetFinalType() == uter.int, "")
        self.assertEqual(True, arraytype_8._GetFinalType() == uter.int, "")
        self.assertEqual(True, arraytype_9._GetFinalType() == uter.int, "")
        self.assertEqual(True, uter.int == arraytype_1._GetFinalType(), "")
        self.assertEqual(True, uter.int == arraytype_2._GetFinalType(), "")
        self.assertEqual(True, uter.int == arraytype_3._GetFinalType(), "")
        self.assertEqual(True, uter.int == arraytype_4._GetFinalType(), "")
        self.assertEqual(True, uter.int == arraytype_5._GetFinalType(), "")
        self.assertEqual(True, uter.int == arraytype_6._GetFinalType(), "")
        self.assertEqual(True, uter.int == arraytype_7._GetFinalType(), "")
        self.assertEqual(True, uter.int == arraytype_8._GetFinalType(), "")
        self.assertEqual(True, uter.int == arraytype_9._GetFinalType(), "")
        
class test_case_data_and_complex(unittest.TestCase):   
    def test_data_1v(self):
        arraytype = uter.array(uter.int, 5)
        arraydata = arraytype()
        print arraydata
        arraydata[0]._SetValue(1)
        arraydata[1]._SetValue(2)
        arraydata[2]._SetValue(3)
        arraydata[3]._SetValue(4)
        arraydata[4]._SetValue(5)
        self.assertEqual(True, arraydata[0]._GetValue() == 1, "")
        self.assertEqual(True, arraydata[1]._GetValue() == 2, "")
        self.assertEqual(True, arraydata[2]._GetValue() == 3, "")
        self.assertEqual(True, arraydata[3]._GetValue() == 4, "")
        self.assertEqual(True, arraydata[4]._GetValue() == 5, "")
    def test_data_2v(self):
        arraytype = uter.array(uter.int, (5, 2))
        arraydata = arraytype()
        print arraydata
        arraydata[0][0]._SetValue(1)
        arraydata[0][1]._SetValue(2)
        arraydata[1][0]._SetValue(3)
        arraydata[1][1]._SetValue(4)
        arraydata[2][0]._SetValue(5)
        arraydata[2][1]._SetValue(6)
        arraydata[3][0]._SetValue(7)
        arraydata[3][1]._SetValue(8)
        arraydata[4][0]._SetValue(9)
        arraydata[4][1]._SetValue(0)
        self.assertEqual(True, arraydata[0][0]._GetValue() == 1, "")
        self.assertEqual(True, arraydata[0][1]._GetValue() == 2, "")
        self.assertEqual(True, arraydata[1][0]._GetValue() == 3, "")
        self.assertEqual(True, arraydata[1][1]._GetValue() == 4, "")
        self.assertEqual(True, arraydata[2][0]._GetValue() == 5, "")
        self.assertEqual(True, arraydata[2][1]._GetValue() == 6, "")
        self.assertEqual(True, arraydata[3][0]._GetValue() == 7, "")
        self.assertEqual(True, arraydata[3][1]._GetValue() == 8, "")
        self.assertEqual(True, arraydata[4][0]._GetValue() == 9, "")
        self.assertEqual(True, arraydata[4][1]._GetValue() == 0 , "")

    def test_subarry(self):
        arraytype = uter.array(uter.int, (5, 2))
        arraydata = arraytype()
        print arraydata
        print arraydata[0]
        print arraydata[1]
        print arraydata[2]
        print arraydata[3]
        print arraydata[4]
        self.assertEqual(True, True , "")
    