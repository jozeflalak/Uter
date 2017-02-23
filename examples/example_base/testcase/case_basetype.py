#coding=utf-8

import uter
import unittest

class test_case_class(unittest.TestCase):   
    def test_print(self):
        print uter.char
        print uter.uchar
        print uter.short
        print uter.ushort
        print uter.int
        print uter.uint
        print uter.long
        print uter.ulong
        print uter.longlong
        print uter.ulonglong
        self.assertEqual(True, True, "")
    def test_size(self):
        self.assertEqual(True, uter.char._GetSize() == 1, "")
        self.assertEqual(True, uter.uchar._GetSize() == 1, "")
        self.assertEqual(True, uter.short._GetSize() == 2, "")
        self.assertEqual(True, uter.ushort._GetSize() == 2, "")
        self.assertEqual(True, uter.int._GetSize() == 4, "")
        self.assertEqual(True, uter.uint._GetSize() == 4, "")
        self.assertEqual(True, uter.long._GetSize() == 4, "")
        self.assertEqual(True, uter.ulong._GetSize() == 4, "")
        self.assertEqual(True, uter.longlong._GetSize() == 8, "")
        self.assertEqual(True, uter.ulonglong._GetSize() == 8, "")
    def test_align(self):
        self.assertEqual(True, uter.char._GetAlign() == 1, "")
        self.assertEqual(True, uter.uchar._GetAlign() == 1, "")
        self.assertEqual(True, uter.short._GetAlign() == 2, "")
        self.assertEqual(True, uter.ushort._GetAlign() == 2, "")
        self.assertEqual(True, uter.int._GetAlign() == 4, "")
        self.assertEqual(True, uter.uint._GetAlign() == 4, "")
        self.assertEqual(True, uter.long._GetAlign() == 4, "")
        self.assertEqual(True, uter.ulong._GetAlign() == 4, "")
        self.assertEqual(True, uter.longlong._GetAlign() == 4, "")
        self.assertEqual(True, uter.ulonglong._GetAlign() == 4, "")
    def test_defdesc(self):
        self.assertEqual(True, uter.char._DefDesc() == "char ", "")
        self.assertEqual(True, uter.uchar._DefDesc() == "unsigned char ", "")
        self.assertEqual(True, uter.short._DefDesc() == "short ", "")
        self.assertEqual(True, uter.ushort._DefDesc() == "unsigned short ", "")
        self.assertEqual(True, uter.int._DefDesc() == "int " , "")
        self.assertEqual(True, uter.uint._DefDesc() == "unsigned int ", "")
        self.assertEqual(True, uter.long._DefDesc() == "long ", "")
        self.assertEqual(True, uter.ulong._DefDesc() == "unsigned long ", "")
        self.assertEqual(True, uter.longlong._DefDesc() == "long long ", "")
        self.assertEqual(True, uter.ulonglong._DefDesc() == "unsigned long long ", "")
    def test_match(self):
        self.assertEqual(True, uter.char == uter.char, "")
        self.assertEqual(True, uter.uchar == uter.uchar, "")
        self.assertEqual(True, uter.short == uter.short, "")
        self.assertEqual(True, uter.ushort == uter.ushort, "")
        self.assertEqual(True, uter.int == uter.int, "")
        self.assertEqual(True, uter.uint ==  uter.uint, "")
        self.assertEqual(True, uter.long == uter.long, "")
        self.assertEqual(True, uter.ulong == uter.ulong, "")
        self.assertEqual(True, uter.longlong ==  uter.longlong, "")
        self.assertEqual(True, uter.ulonglong == uter.ulonglong, "")
        self.assertEqual(True, uter.char != uter.uchar, "")
        self.assertEqual(True, uter.uchar != uter.char, "")
        self.assertEqual(True, uter.short != uter.ushort, "")
        self.assertEqual(True, uter.ushort != uter.short, "")
        self.assertEqual(True, uter.int != uter.uint, "")
        self.assertEqual(True, uter.uint != uter.int, "")
        self.assertEqual(True, uter.long != uter.ulong, "")
        self.assertEqual(True, uter.ulong != uter.long, "")
        self.assertEqual(True, uter.longlong != uter.ulonglong, "")
        self.assertEqual(True, uter.ulonglong != uter.longlong, "")
        self.assertEqual(True, uter.char != 1, "")
        self.assertEqual(True, uter.uchar != 1, "")
        self.assertEqual(True, uter.short != 1, "")
        self.assertEqual(True, uter.ushort != 1, "")
        self.assertEqual(True, uter.int != 1, "")
        self.assertEqual(True, uter.uint != 1, "")
        self.assertEqual(True, uter.long != 1, "")
        self.assertEqual(True, uter.ulong != 1, "")
        self.assertEqual(True, uter.longlong != 1, "")
        self.assertEqual(True, uter.ulonglong != 1, "")
    
class test_case_obj_new(unittest.TestCase):
    def test_print(self):
        data = uter.char()
        print data
        data = uter.uchar()
        print data
        data = uter.short()
        print data
        data = uter.ushort()
        print data
        data = uter.int()
        print data
        data = uter.uint()
        print data
        data = uter.long()
        print data
        data = uter.ulong()
        print data
        data = uter.longlong()
        print data
        data = uter.ulonglong()
        print data
        self.assertEqual(True, True, "")
    def test_value_default(self):
        data_char = uter.char()
        data_uchar = uter.uchar()
        data_short = uter.short()
        data_ushort = uter.ushort()
        data_int = uter.int()
        data_uint = uter.uint()
        data_long = uter.long()
        data_ulong = uter.ulong()
        data_longlong = uter.longlong()
        data_ulonglong = uter.ulonglong()
        self.assertEqual(True, data_char._GetValue() == 0, "")
        self.assertEqual(True, data_uchar._GetValue() == 0, "")
        self.assertEqual(True, data_short._GetValue() == 0, "")
        self.assertEqual(True, data_ushort._GetValue() == 0, "")
        self.assertEqual(True, data_int._GetValue() == 0, "")
        self.assertEqual(True, data_uint._GetValue() == 0, "")
        self.assertEqual(True, data_long._GetValue() == 0, "")
        self.assertEqual(True, data_ulong._GetValue() == 0, "")
        self.assertEqual(True, data_longlong._GetValue() == 0, "")
        self.assertEqual(True, data_ulonglong._GetValue() == 0, "")
    def test_value_num(self):
        data_char = uter.char(5)
        data_uchar = uter.uchar(5)
        data_short = uter.short(5)
        data_ushort = uter.ushort(5)
        data_int = uter.int(5)
        data_uint = uter.uint(5)
        data_long = uter.long(5)
        data_ulong = uter.ulong(5)
        data_longlong = uter.longlong(5)
        data_ulonglong = uter.ulonglong(5)
        self.assertEqual(True, data_char._GetValue() == 5, "")
        self.assertEqual(True, data_uchar._GetValue() == 5, "")
        self.assertEqual(True, data_short._GetValue() == 5, "")
        self.assertEqual(True, data_ushort._GetValue() == 5, "")
        self.assertEqual(True, data_int._GetValue() == 5, "")
        self.assertEqual(True, data_uint._GetValue() == 5, "")
        self.assertEqual(True, data_long._GetValue() == 5, "")
        self.assertEqual(True, data_ulong._GetValue() == 5, "")
        self.assertEqual(True, data_longlong._GetValue() == 5, "")
        self.assertEqual(True, data_ulonglong._GetValue() == 5, "")
    def test_value_obj(self):
        data = uter.char(5)
        data_char = uter.char(data)
        data = uter.uchar(5)
        data_uchar = uter.uchar(data)
        data = uter.short(5)
        data_short = uter.short(data)
        data = uter.ushort(5)
        data_ushort = uter.ushort(data)
        data = uter.int(5)
        data_int = uter.int(data)
        data = uter.uint(5)
        data_uint = uter.uint(data)
        data = uter.long(5)
        data_long = uter.long(data)
        data = uter.ulong(5)
        data_ulong = uter.ulong(data)
        data = uter.longlong(5)
        data_longlong = uter.longlong(data)
        data = uter.ulonglong(5)
        data_ulonglong = uter.ulonglong(data)
        self.assertEqual(True, data_char._GetValue() == 5, "")
        self.assertEqual(True, data_uchar._GetValue() == 5, "")
        self.assertEqual(True, data_short._GetValue() == 5, "")
        self.assertEqual(True, data_ushort._GetValue() == 5, "")
        self.assertEqual(True, data_int._GetValue() == 5, "")
        self.assertEqual(True, data_uint._GetValue() == 5, "")
        self.assertEqual(True, data_long._GetValue() == 5, "")
        self.assertEqual(True, data_ulong._GetValue() == 5, "")
        self.assertEqual(True, data_longlong._GetValue() == 5, "")
        self.assertEqual(True, data_ulonglong._GetValue() == 5, "")
class test_case_obj_global(unittest.TestCase):   
    def test_char(self):
        data_obj = uter.export.variable("g_char_0")
        self.assertEqual(True, data_obj._GetValue() == 0, "")
        data_obj = uter.export.variable("g_char_1")
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        data_obj = uter.export.variable("g_char_2")
        self.assertEqual(True, data_obj._GetValue() == 2, "")
        data_obj = uter.export.variable("g_char_min")
        self.assertEqual(True, data_obj._GetValue() == -(2 ** 7), "")
        data_obj = uter.export.variable("g_char_max")
        self.assertEqual(True, data_obj._GetValue() == 2 ** 7 - 1, "")
    def test_uchar(self):
        data_obj = uter.export.variable("g_uchar_0")
        self.assertEqual(True, data_obj._GetValue() == 0, "")
        data_obj = uter.export.variable("g_uchar_1")
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        data_obj = uter.export.variable("g_uchar_2")
        self.assertEqual(True, data_obj._GetValue() == 2, "")
        data_obj = uter.export.variable("g_uchar_min")
        self.assertEqual(True, data_obj._GetValue() == 0, "")
        data_obj = uter.export.variable("g_uchar_max")
        self.assertEqual(True, data_obj._GetValue() == 2 ** 8 - 1, "")
    def test_short(self):
        data_obj = uter.export.variable("g_short_0")
        self.assertEqual(True, data_obj._GetValue() == 0, "")
        data_obj = uter.export.variable("g_short_1")
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        data_obj = uter.export.variable("g_short_2")
        self.assertEqual(True, data_obj._GetValue() == 2, "")
        data_obj = uter.export.variable("g_short_min")
        self.assertEqual(True, data_obj._GetValue() == -(2 ** 15), "")
        data_obj = uter.export.variable("g_short_max")
        self.assertEqual(True, data_obj._GetValue() == 2 ** 15 - 1, "")
    def test_ushort(self):
        data_obj = uter.export.variable("g_ushort_0")
        self.assertEqual(True, data_obj._GetValue() == 0, "")
        data_obj = uter.export.variable("g_ushort_1")
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        data_obj = uter.export.variable("g_ushort_2")
        self.assertEqual(True, data_obj._GetValue() == 2, "")
        data_obj = uter.export.variable("g_ushort_min")
        self.assertEqual(True, data_obj._GetValue() == 0, "")
        data_obj = uter.export.variable("g_ushort_max")
        self.assertEqual(True, data_obj._GetValue() == 2 ** 16 - 1, "")
    def test_int(self):
        data_obj = uter.export.variable("g_int_0")
        self.assertEqual(True, data_obj._GetValue() == 0, "")
        data_obj = uter.export.variable("g_int_1")
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        data_obj = uter.export.variable("g_int_2")
        self.assertEqual(True, data_obj._GetValue() == 2, "")
        data_obj = uter.export.variable("g_int_min")
        self.assertEqual(True, data_obj._GetValue() == -(2 ** 31), "")
        data_obj = uter.export.variable("g_int_max")
        self.assertEqual(True, data_obj._GetValue() == 2 ** 31 - 1, "")
    def test_uint(self):
        data_obj = uter.export.variable("g_uint_0")
        self.assertEqual(True, data_obj._GetValue() == 0, "")
        data_obj = uter.export.variable("g_uint_1")
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        data_obj = uter.export.variable("g_uint_2")
        self.assertEqual(True, data_obj._GetValue() == 2, "")
        data_obj = uter.export.variable("g_uint_min")
        self.assertEqual(True, data_obj._GetValue() == 0, "")
        data_obj = uter.export.variable("g_uint_max")
        self.assertEqual(True, data_obj._GetValue() == 2 ** 32 - 1, "")
    def test_long(self):
        data_obj = uter.export.variable("g_long_0")
        self.assertEqual(True, data_obj._GetValue() == 0, "")
        data_obj = uter.export.variable("g_long_1")
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        data_obj = uter.export.variable("g_long_2")
        self.assertEqual(True, data_obj._GetValue() == 2, "")
        data_obj = uter.export.variable("g_long_min")
        self.assertEqual(True, data_obj._GetValue() == -(2 ** 31), "")
        data_obj = uter.export.variable("g_long_max")
        self.assertEqual(True, data_obj._GetValue() == 2 ** 31 - 1, "")
    def test_ulong(self):
        data_obj = uter.export.variable("g_ulong_0")
        self.assertEqual(True, data_obj._GetValue() == 0, "")
        data_obj = uter.export.variable("g_ulong_1")
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        data_obj = uter.export.variable("g_ulong_2")
        self.assertEqual(True, data_obj._GetValue() == 2, "")
        data_obj = uter.export.variable("g_ulong_min")
        self.assertEqual(True, data_obj._GetValue() == 0, "")
        data_obj = uter.export.variable("g_ulong_max")
        self.assertEqual(True, data_obj._GetValue() == 2 ** 32 - 1, "")
    def test_longlong(self):
        data_obj = uter.export.variable("g_longlong_0")
        self.assertEqual(True, data_obj._GetValue() == 0, "")
        data_obj = uter.export.variable("g_longlong_1")
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        data_obj = uter.export.variable("g_longlong_2")
        self.assertEqual(True, data_obj._GetValue() == 2, "")
        data_obj = uter.export.variable("g_longlong_min")
        self.assertEqual(True, data_obj._GetValue() == -(2 ** 63), "")
        data_obj = uter.export.variable("g_longlong_max")
        self.assertEqual(True, data_obj._GetValue() == 2 ** 63 - 1, "")
    def test_ulonglong(self):
        data_obj = uter.export.variable("g_ulonglong_0")
        self.assertEqual(True, data_obj._GetValue() == 0, "")
        data_obj = uter.export.variable("g_ulonglong_1")
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        data_obj = uter.export.variable("g_ulonglong_2")
        self.assertEqual(True, data_obj._GetValue() == 2, "")
        data_obj = uter.export.variable("g_ulonglong_min")
        self.assertEqual(True, data_obj._GetValue() == 0, "")
        data_obj = uter.export.variable("g_ulonglong_max")
        self.assertEqual(True, data_obj._GetValue() == 2 ** 64 - 1, "")

class test_case_obj(unittest.TestCase):   
    def test_getaddress(self):
        data_char = uter.char()
        data_uchar = uter.uchar()
        data_short = uter.short()
        data_ushort = uter.ushort()
        data_int = uter.int()
        data_uint = uter.uint()
        data_long = uter.long()
        data_ulong = uter.ulong()
        data_longlong = uter.longlong()
        data_ulonglong = uter.ulonglong()
        self.assertEqual(True, data_char._GetAddress() != 0, "")
        self.assertEqual(True, data_uchar._GetAddress() != 0, "")
        self.assertEqual(True, data_short._GetAddress() != 0, "")
        self.assertEqual(True, data_ushort._GetAddress() != 0, "")
        self.assertEqual(True, data_int._GetAddress() != 0, "")
        self.assertEqual(True, data_uint._GetAddress() != 0, "")
        self.assertEqual(True, data_long._GetAddress() != 0, "")
        self.assertEqual(True, data_ulong._GetAddress() != 0, "")
        self.assertEqual(True, data_longlong._GetAddress() != 0, "")
        self.assertEqual(True, data_ulonglong._GetAddress() != 0, "")
    def test_gettype(self):
        data_char = uter.char()
        data_uchar = uter.uchar()
        data_short = uter.short()
        data_ushort = uter.ushort()
        data_int = uter.int()
        data_uint = uter.uint()
        data_long = uter.long()
        data_ulong = uter.ulong()
        data_longlong = uter.longlong()
        data_ulonglong = uter.ulonglong()
        self.assertEqual(True, data_char._GetType() == uter.char, "")
        self.assertEqual(True, data_uchar._GetType() == uter.uchar, "")
        self.assertEqual(True, data_short._GetType() == uter.short, "")
        self.assertEqual(True, data_ushort._GetType() == uter.ushort, "")
        self.assertEqual(True, data_int._GetType() == uter.int, "")
        self.assertEqual(True, data_uint._GetType() == uter.uint, "")
        self.assertEqual(True, data_long._GetType() == uter.long, "")
        self.assertEqual(True, data_ulong._GetType() == uter.ulong, "")
        self.assertEqual(True, data_longlong._GetType() == uter.longlong, "")
        self.assertEqual(True, data_ulonglong._GetType() == uter.ulonglong, "")
    def test_match(self):
        data_char = uter.char()
        data_uchar = uter.uchar()
        data_short = uter.short()
        data_ushort = uter.ushort()
        data_int = uter.int()
        data_uint = uter.uint()
        data_long = uter.long()
        data_ulong = uter.ulong()
        data_longlong = uter.longlong()
        data_ulonglong = uter.ulonglong()
        self.assertEqual(True, uter.char._MatchObj(data_char), "")
        self.assertEqual(True, uter.uchar._MatchObj(data_uchar), "")
        self.assertEqual(True, uter.short._MatchObj(data_short), "")
        self.assertEqual(True, uter.ushort._MatchObj(data_ushort), "")
        self.assertEqual(True, uter.int._MatchObj(data_int), "")
        self.assertEqual(True, uter.uint._MatchObj(data_uint), "")
        self.assertEqual(True, uter.long._MatchObj(data_long), "")
        self.assertEqual(True, uter.ulong._MatchObj(data_ulong), "")
        self.assertEqual(True, uter.longlong._MatchObj(data_longlong), "")
        self.assertEqual(True, uter.ulonglong._MatchObj(data_ulonglong), "")
        self.assertEqual(True, not uter.char._MatchObj(data_uchar), "")
        self.assertEqual(True, not uter.uchar._MatchObj(data_char), "")
        self.assertEqual(True, not uter.short._MatchObj(data_ushort), "")
        self.assertEqual(True, not uter.ushort._MatchObj(data_short), "")
        self.assertEqual(True, not uter.int._MatchObj(data_uint), "")
        self.assertEqual(True, not uter.uint._MatchObj(data_int), "")
        self.assertEqual(True, not uter.long._MatchObj(data_ulong), "")
        self.assertEqual(True, not uter.ulong._MatchObj(data_long), "")
        self.assertEqual(True, not uter.longlong._MatchObj(data_ulonglong), "")
        self.assertEqual(True, not uter.ulonglong._MatchObj(data_longlong), "")
        self.assertEqual(True, not uter.char._MatchObj(1), "")
        self.assertEqual(True, not uter.uchar._MatchObj(1), "")
        self.assertEqual(True, not uter.short._MatchObj(1), "")
        self.assertEqual(True, not uter.ushort._MatchObj(1), "")
        self.assertEqual(True, not uter.int._MatchObj(1), "")
        self.assertEqual(True, not uter.uint._MatchObj(1), "")
        self.assertEqual(True, not uter.long._MatchObj(1), "")
        self.assertEqual(True, not uter.ulong._MatchObj(1), "")
        self.assertEqual(True, not uter.longlong._MatchObj(1), "")
        self.assertEqual(True, not uter.ulonglong._MatchObj(1), "")
    def test_setnum(self):
        data_char = uter.char()
        data_uchar = uter.uchar()
        data_short = uter.short()
        data_ushort = uter.ushort()
        data_int = uter.int()
        data_uint = uter.uint()
        data_long = uter.long()
        data_ulong = uter.ulong()
        data_longlong = uter.longlong()
        data_ulonglong = uter.ulonglong()
        data_char._SetValue(5)
        data_uchar._SetValue(5)
        data_short._SetValue(5)
        data_ushort._SetValue(5)
        data_int._SetValue(5)
        data_uint._SetValue(5)
        data_long._SetValue(5)
        data_ulong._SetValue(5)
        data_longlong._SetValue(5)
        data_ulonglong._SetValue(5)
        self.assertEqual(True, data_char._GetValue() == 5, "")
        self.assertEqual(True, data_uchar._GetValue() == 5, "")
        self.assertEqual(True, data_short._GetValue() == 5, "")
        self.assertEqual(True, data_ushort._GetValue() == 5, "")
        self.assertEqual(True, data_int._GetValue() == 5, "")
        self.assertEqual(True, data_uint._GetValue() == 5, "")
        self.assertEqual(True, data_long._GetValue() == 5, "")
        self.assertEqual(True, data_ulong._GetValue() == 5, "")
        self.assertEqual(True, data_longlong._GetValue() == 5, "")
        self.assertEqual(True, data_ulonglong._GetValue() == 5, "")

class test_case_datarange(unittest.TestCase):
    def test_char(self):
        data_obj = uter.char()
        data_obj._SetValue(1)
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        data_obj._SetValue(-128)
        self.assertEqual(True, data_obj._GetValue() == -128, "")
        data_obj._SetValue(127)
        self.assertEqual(True, data_obj._GetValue() == 127, "")
        data_obj._SetValue(-129)
        self.assertEqual(True, data_obj._GetValue() == 127, "")
        data_obj._SetValue(128)
        self.assertEqual(True, data_obj._GetValue() == -128, "")
    def test_uchar(self):
        data_obj = uter.uchar()
        data_obj._SetValue(1)
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        data_obj._SetValue(0)
        self.assertEqual(True, data_obj._GetValue() == 0, "")
        data_obj._SetValue(255)
        self.assertEqual(True, data_obj._GetValue() == 255, "")
        data_obj._SetValue(-1)
        self.assertEqual(True, data_obj._GetValue() == 255, "")
        data_obj._SetValue(256)
        self.assertEqual(True, data_obj._GetValue() == 0, "")
    def test_short(self):
        data_obj = uter.short()
        data_obj._SetValue(1)
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        data_obj._SetValue(-(2 ** 15))
        self.assertEqual(True, data_obj._GetValue() == -(2 ** 15), "")
        data_obj._SetValue((2 ** 15) - 1)
        self.assertEqual(True, data_obj._GetValue() == (2 ** 15) - 1, "")
        data_obj._SetValue(-(2 ** 15) - 1)
        self.assertEqual(True, data_obj._GetValue() == (2 ** 15) - 1, "")
        data_obj._SetValue(2 ** 15)
        self.assertEqual(True, data_obj._GetValue() == -(2 ** 15), "")
    def test_ushort(self):
        data_obj = uter.ushort()
        data_obj._SetValue(1)
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        data_obj._SetValue(0)
        self.assertEqual(True, data_obj._GetValue() == 0, "")
        data_obj._SetValue((2 ** 16) - 1)
        self.assertEqual(True, data_obj._GetValue() == (2 ** 16) - 1, "")
        data_obj._SetValue(-1)
        self.assertEqual(True, data_obj._GetValue() == (2 ** 16) - 1, "")
        data_obj._SetValue(2 ** 16)
        self.assertEqual(True, data_obj._GetValue() == 0, "")
    def test_int(self):
        data_obj = uter.int()
        data_obj._SetValue(1)
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        data_obj._SetValue(-(2 ** 31))
        self.assertEqual(True, data_obj._GetValue() == -(2 ** 31), "")
        data_obj._SetValue((2 ** 31) - 1)
        self.assertEqual(True, data_obj._GetValue() == (2 ** 31) - 1, "")
        data_obj._SetValue(-(2 ** 31) - 1)
        self.assertEqual(True, data_obj._GetValue() == (2 ** 31) - 1, "")
        data_obj._SetValue(2 ** 31)
        self.assertEqual(True, data_obj._GetValue() == -(2 ** 31), "")
    def test_uint(self):
        data_obj = uter.uint()
        data_obj._SetValue(1)
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        data_obj._SetValue(0)
        self.assertEqual(True, data_obj._GetValue() == 0, "")
        data_obj._SetValue((2 ** 32) - 1)
        self.assertEqual(True, data_obj._GetValue() == (2 ** 32) - 1, "")
        data_obj._SetValue(-1)
        self.assertEqual(True, data_obj._GetValue() == (2 ** 32) - 1, "")
        data_obj._SetValue(2 ** 32)
        self.assertEqual(True, data_obj._GetValue() == 0, "")
    def test_long(self):
        data_obj = uter.long()
        data_obj._SetValue(1)
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        data_obj._SetValue(-(2 ** 31))
        self.assertEqual(True, data_obj._GetValue() == -(2 ** 31), "")
        data_obj._SetValue((2 ** 31) - 1)
        self.assertEqual(True, data_obj._GetValue() == (2 ** 31) - 1, "")
        data_obj._SetValue(-(2 ** 31) - 1)
        self.assertEqual(True, data_obj._GetValue() == (2 ** 31) - 1, "")
        data_obj._SetValue(2 ** 31)
        self.assertEqual(True, data_obj._GetValue() == -(2 ** 31), "")
    def test_ulong(self):
        data_obj = uter.ulong()
        data_obj._SetValue(1)
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        data_obj._SetValue(0)
        self.assertEqual(True, data_obj._GetValue() == 0, "")
        data_obj._SetValue((2 ** 32) - 1)
        self.assertEqual(True, data_obj._GetValue() == (2 ** 32) - 1, "")
        data_obj._SetValue(-1)
        self.assertEqual(True, data_obj._GetValue() == (2 ** 32) - 1, "")
        data_obj._SetValue(2 ** 32)
        self.assertEqual(True, data_obj._GetValue() == 0, "")
    def test_longlong(self):
        data_obj = uter.longlong()
        data_obj._SetValue(1)
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        data_obj._SetValue(-(2 ** 63))
        self.assertEqual(True, data_obj._GetValue() == -(2 ** 63), "")
        data_obj._SetValue((2 ** 63) - 1)
        self.assertEqual(True, data_obj._GetValue() == (2 ** 63) - 1, "")
        data_obj._SetValue(-(2 ** 63) - 1)
        self.assertEqual(True, data_obj._GetValue() == (2 ** 63) - 1, "")
        data_obj._SetValue(2 ** 63)
        self.assertEqual(True, data_obj._GetValue() == -(2 ** 63), "")
    def test_ulonglong(self):
        data_obj = uter.ulonglong()
        data_obj._SetValue(1)
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        data_obj._SetValue(0)
        self.assertEqual(True, data_obj._GetValue() == 0, "")
        data_obj._SetValue((2 ** 64) - 1)
        self.assertEqual(True, data_obj._GetValue() == (2 ** 64) - 1, "")
        data_obj._SetValue(-1)
        self.assertEqual(True, data_obj._GetValue() == (2 ** 64) - 1, "")
        data_obj._SetValue(2 ** 64)
        self.assertEqual(True, data_obj._GetValue() == 0, "")
        
class test_case_left(unittest.TestCase):
    def test_char_left(self):
        data_obj = uter.char()

        data_obj._SetValue(1)
        data_obj._LeftShift(1)
        self.assertEqual(True, data_obj._GetValue() == 2, "")
        
        data_obj._SetValue(1)
        data_obj._LeftShift(2)
        self.assertEqual(True, data_obj._GetValue() == 4, "")
        
        data_obj._SetValue((1 << 7) - 1)
        data_obj._LeftShift(1)
        self.assertEqual(True, data_obj._GetValue() % (1 << 8) == (((1 << 7) - 1) << 1) & ((1 << 8) - 1), "")
        
    def test_uchar_left(self):
        data_obj = uter.uchar()

        data_obj._SetValue(1)
        data_obj._LeftShift(1)
        self.assertEqual(True, data_obj._GetValue() == 2, "")
        
        data_obj._SetValue(1)
        data_obj._LeftShift(2)
        self.assertEqual(True, data_obj._GetValue() == 4, "")
        
        data_obj._SetValue((1 << 8) - 1)
        data_obj._LeftShift(1)
        self.assertEqual(True, data_obj._GetValue() == ((((1 << 8) - 1) << 1) & ((1 << 8) - 1)), "")
        
    def test_short_left(self):
        data_obj = uter.short()

        data_obj._SetValue(1)
        data_obj._LeftShift(1)
        self.assertEqual(True, data_obj._GetValue() == 2, "")
        
        data_obj._SetValue(1)
        data_obj._LeftShift(2)
        self.assertEqual(True, data_obj._GetValue() == 4, "")
        
        data_obj._SetValue((1 << 15) - 1)
        data_obj._LeftShift(1)
        self.assertEqual(True, data_obj._GetValue() % (1 << 16) == (((1 << 15) - 1) << 1) & ((1 << 16) - 1), "")
        
    def test_ushort_left(self):
        data_obj = uter.ushort()

        data_obj._SetValue(1)
        data_obj._LeftShift(1)
        self.assertEqual(True, data_obj._GetValue() == 2, "")
        
        data_obj._SetValue(1)
        data_obj._LeftShift(2)
        self.assertEqual(True, data_obj._GetValue() == 4, "")
        
        data_obj._SetValue((1 << 16) - 1)
        data_obj._LeftShift(1)
        self.assertEqual(True, data_obj._GetValue() == ((((1 << 16) - 1) << 1) & ((1 << 16) - 1)), "")
            
    def test_int_left(self):
        data_obj = uter.int()

        data_obj._SetValue(1)
        data_obj._LeftShift(1)
        self.assertEqual(True, data_obj._GetValue() == 2, "")
        
        data_obj._SetValue(1)
        data_obj._LeftShift(2)
        self.assertEqual(True, data_obj._GetValue() == 4, "")
        
        data_obj._SetValue((1 << 31) - 1)
        data_obj._LeftShift(1)        
        self.assertEqual(True, data_obj._GetValue() % (1 << 32) == (((1 << 31) - 1) << 1) & ((1 << 32) - 1), "")
        
    def test_uint_left(self):
        data_obj = uter.uint()

        data_obj._SetValue(1)
        data_obj._LeftShift(1)
        self.assertEqual(True, data_obj._GetValue() == 2, "")
        
        data_obj._SetValue(1)
        data_obj._LeftShift(2)
        self.assertEqual(True, data_obj._GetValue() == 4, "")
        
        data_obj._SetValue((1 << 32) - 1)
        data_obj._LeftShift(1)
        self.assertEqual(True, data_obj._GetValue() == ((((1 << 32) - 1) << 1) & ((1 << 32) - 1)), "")

    def test_long_left(self):
        data_obj = uter.long()

        data_obj._SetValue(1)
        data_obj._LeftShift(1)
        self.assertEqual(True, data_obj._GetValue() == 2, "")
        
        data_obj._SetValue(1)
        data_obj._LeftShift(2)
        self.assertEqual(True, data_obj._GetValue() == 4, "")
        
        data_obj._SetValue((1 << 31) - 1)
        data_obj._LeftShift(1)
        self.assertEqual(True, data_obj._GetValue() % (1 << 32) == (((1 << 31) - 1) << 1) & ((1 << 32) - 1), "")
        
    def test_ulong_left(self):
        data_obj = uter.ulong()

        data_obj._SetValue(1)
        data_obj._LeftShift(1)
        self.assertEqual(True, data_obj._GetValue() == 2, "")
        
        data_obj._SetValue(1)
        data_obj._LeftShift(2)
        self.assertEqual(True, data_obj._GetValue() == 4, "")
        
        data_obj._SetValue((1 << 32) - 1)
        data_obj._LeftShift(1)
        self.assertEqual(True, data_obj._GetValue() == ((((1 << 32) - 1) << 1) & ((1 << 32) - 1)), "")

    def test_longlong_left(self):
        data_obj = uter.longlong()

        data_obj._SetValue(1)
        data_obj._LeftShift(1)
        self.assertEqual(True, data_obj._GetValue() == 2, "")
        
        data_obj._SetValue(1)
        data_obj._LeftShift(2)
        self.assertEqual(True, data_obj._GetValue() == 4, "")
        
        data_obj._SetValue((1 << 63) - 1)
        data_obj._LeftShift(1)
        self.assertEqual(True, data_obj._GetValue() % (1 << 64) == (((1 << 63) - 1) << 1) & ((1 << 64) - 1), "")
        
    def test_ulonglong_left(self):
        data_obj = uter.ulonglong()

        data_obj._SetValue(1)
        data_obj._LeftShift(1)
        self.assertEqual(True, data_obj._GetValue() == 2, "")
        
        data_obj._SetValue(1)
        data_obj._LeftShift(2)
        self.assertEqual(True, data_obj._GetValue() == 4, "")
        
        data_obj._SetValue((1 << 64) - 1)
        data_obj._LeftShift(1)
        self.assertEqual(True, data_obj._GetValue() == ((((1 << 64) - 1) << 1) & ((1 << 64) - 1)), "")

class test_case_right(unittest.TestCase):
    def test_char_right(self):
        data_obj = uter.char()

        data_obj._SetValue(2)
        data_obj._RightShift(1)
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        
        data_obj._SetValue(4)
        data_obj._RightShift(2)
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        
        data_obj._SetValue(-(1 << 7))
        data_obj._RightShift(1)
        self.assertEqual(True, data_obj._GetValue() % (1 << 8) == (((-(1 << 7)) % (1 << 8)) >> 1), "")
        
    def test_uchar_right(self):
        data_obj = uter.uchar()

        data_obj._SetValue(2)
        data_obj._RightShift(1)
        self.assertEqual(True, data_obj._GetValue() == 1, "") 
        
        data_obj._SetValue(4)
        data_obj._RightShift(2)
        self.assertEqual(True, data_obj._GetValue() == 1, "") 
        
        data_obj._SetValue(0)
        data_obj._RightShift(1)
        self.assertEqual(True, data_obj._GetValue() == 0, "")
        
    def test_short_right(self):
        data_obj = uter.short()

        data_obj._SetValue(2)
        data_obj._RightShift(1)
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        
        data_obj._SetValue(4)
        data_obj._RightShift(2)
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        
        data_obj._SetValue(-(1 << 15))
        data_obj._RightShift(1)
        self.assertEqual(True, data_obj._GetValue() % (1 << 16) == (((-(1 << 15)) % (1 << 16)) >> 1), "")
        
    def test_ushort_right(self):
        data_obj = uter.ushort()

        data_obj._SetValue(2)
        data_obj._RightShift(1)
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        
        data_obj._SetValue(4)
        data_obj._RightShift(2)
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        
        data_obj._SetValue(0)
        data_obj._RightShift(1)
        self.assertEqual(True, data_obj._GetValue() == 0, "")
            
    def test_int_right(self):
        data_obj = uter.int()

        data_obj._SetValue(2)
        data_obj._RightShift(1)
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        
        data_obj._SetValue(4)
        data_obj._RightShift(2)
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        
        data_obj._SetValue(-(1 << 31))
        data_obj._RightShift(1)
        self.assertEqual(True, data_obj._GetValue() % (1 << 32) == (((-(1 << 31)) % (1 << 32)) >> 1), "")
        
    def test_uint_right(self):
        data_obj = uter.uint()

        data_obj._SetValue(2)
        data_obj._RightShift(1)
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        
        data_obj._SetValue(4)
        data_obj._RightShift(2)
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        
        data_obj._SetValue(0)
        data_obj._RightShift(1)
        self.assertEqual(True, data_obj._GetValue() == 0, "")
        
    def test_long_right(self):
        data_obj = uter.long()

        data_obj._SetValue(2)
        data_obj._RightShift(1)
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        
        data_obj._SetValue(4)
        data_obj._RightShift(2)
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        
        data_obj._SetValue(-(1 << 31))
        data_obj._RightShift(1)
        self.assertEqual(True, data_obj._GetValue() % (1 << 32) == (((-(1 << 31)) % (1 << 32)) >> 1), "")
        
    def test_ulong_right(self):
        data_obj = uter.ulong()

        data_obj._SetValue(2)
        data_obj._RightShift(1)
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        
        data_obj._SetValue(4)
        data_obj._RightShift(2)
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        
        data_obj._SetValue(0)
        data_obj._RightShift(1)
        self.assertEqual(True, data_obj._GetValue() == 0, "")
    
    def test_longlong_right(self):
        data_obj = uter.longlong()

        data_obj._SetValue(2)
        data_obj._RightShift(1)
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        
        data_obj._SetValue(4)
        data_obj._RightShift(2)
        self.assertEqual(True, data_obj._GetValue() == 1, "")
        
        data_obj._SetValue(-(1 << 63))
        data_obj._RightShift(1)
        self.assertEqual(True, data_obj._GetValue() % (1 << 64) == (((-(1 << 63)) % (1 << 64)) >> 1), "")
        
    def test_ulonglong_right(self):
        data_obj = uter.ulonglong()

        data_obj._SetValue(2)
        data_obj._RightShift(1)
        self.assertEqual(True, data_obj._GetValue() == 1, "")    
        
        data_obj._SetValue(4)
        data_obj._RightShift(2)
        self.assertEqual(True, data_obj._GetValue() == 1, "")    
        
        data_obj._SetValue(0)
        data_obj._RightShift(1)
        self.assertEqual(True, data_obj._GetValue() == 0, "")    

'''
这个用来测试基类用，测试基类以INT类型为代表
'''
class test_case_baseclass(unittest.TestCase):   
    def test_error_init_1(self):
        exception_happend = False
        try:
            data = uter.int("x")
        except uter.error.ArgError, e:
            exception_happend = True
        self.assertEqual(True, exception_happend, "")

    def test_error_init_2(self):
        exception_happend = False
        z = uter.char(10)
        try:
            data = uter.int(z)
        except uter.error.ArgError, e:
            exception_happend = True
        self.assertEqual(True, exception_happend, "")

    def test_error_init_3(self):
        data = uter.int()
        exception_happend = False
        try:
            data._SetValue("x")
        except uter.error.ArgError, e:
            exception_happend = True
        self.assertEqual(True, exception_happend, "")
        
    def test_error_init_4(self):
        data = uter.int()
        exception_happend = False
        z = uter.char(10)
        try:
            data._SetValue(z)
        except uter.error.ArgError, e:
            exception_happend = True
        self.assertEqual(True, exception_happend, "")
