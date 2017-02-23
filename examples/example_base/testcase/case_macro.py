#coding=utf-8

import uter
import unittest

#这个先忽略，需要补回来的
@unittest.skipIf(True, "")
class test_case_macro(unittest.TestCase):
    def test_macro(self):
        result = True
        
        macro_obj_1 = uter.export.macro("NUM_MACRO_1")
        print macro_obj_1
        result &= macro_obj_1.number() == 123
        
        macro_obj_2 = uter.export.macro("NUM_MACRO_2")
        print macro_obj_2
        result &= macro_obj_2.number() == 123
        
        macro_obj_3 = uter.export.macro("NUM_MACRO_3")
        print macro_obj_3
        result &= macro_obj_3.number() == 123
        
        macro_obj_4 = uter.export.macro("NUM_MACRO_4")
        print macro_obj_4
        result &= macro_obj_4.number() == 123
        result &= macro_obj_4.numbers() == [123,123]
        
        macro_obj_1 = uter.export.macro("STR_MACRO_1")
        print macro_obj_1
        result &= macro_obj_1.string() == "abc"

        macro_obj_2 = uter.export.macro("STR_MACRO_2")
        print macro_obj_2
        result &= macro_obj_2.string() == "abc"
        
        macro_obj_3 = uter.export.macro("STR_MACRO_3")
        print macro_obj_3
        result &= macro_obj_3.string() == "abc"
        
        macro_obj_4 = uter.export.macro("STR_MACRO_4")
        print macro_obj_4
        result &= macro_obj_4.string() == "abc"
        result &= macro_obj_4.strings() == ["abc","abc"]
        
        macro_obj_5 = uter.export.macro("STR_MACRO_5")
        print macro_obj_5
        result &= macro_obj_5.string() == r'"abc"'
        result &= macro_obj_5.strings() == [r'"abc"',r'"abc"']
        
        macro_obj_1 = uter.export.macro("BIT_MACRO_1")
        print macro_obj_1
        result &= macro_obj_1.number() == 0x00000001
        
        macro_obj_2 = uter.export.macro("BIT_MACRO_2")
        print macro_obj_2
        result &= macro_obj_2.number() == 0x00000002

        macro_obj_3 = uter.export.macro("BIT_MACRO_3")
        print macro_obj_3
        result &= macro_obj_3.number() == 0x00000004
        
        macro_obj_4 = uter.export.macro("BIT_MACRO_4")
        print macro_obj_4
        result &= macro_obj_4.number() == 0x00000008
        
        macro_obj_5 = uter.export.macro("BIT_MACRO_5")
        print macro_obj_5
        result &= macro_obj_5.number() == 0x00000010
        
        macro_obj_1 = uter.export.macro("CMP_MACRO_1(x)")
        print macro_obj_1
        macro_obj_2 = uter.export.macro("CMP_MACRO_2(x)")
        print macro_obj_2
        macro_obj_3 = uter.export.macro("CMP_MACRO_3(x)")
        print macro_obj_3
        macro_obj_4 = uter.export.macro("CMP_MACRO_4(x)")
        print macro_obj_4
        macro_obj_5 = uter.export.macro("CMP_MACRO_5(x)")
        print macro_obj_5
        
        macro_obj_f = uter.export.macro("FUN_MACRO_1(x)")
        print macro_obj_f
        
        macro_obj_1 = uter.export.macro("MACROINMACRO_IN_1")
        print macro_obj_1
        result &= macro_obj_1.number() == 0x12345678
        macro_obj_2 = uter.export.macro("MACROINMACRO_IN_2(x)")
        print macro_obj_2
        macro_obj_3 = uter.export.macro("MACROINMACRO_OUT(x)")
        print macro_obj_3
        
        macro_obj_1 = uter.export.macro("MACRO_WITHUNDEF_1")
        print macro_obj_1
        result &= macro_obj_1.isdefine() == False
        
        macro_obj_1 = uter.export.macro("MACRO_NO_VALUE")
        print macro_obj_1
        result &= macro_obj_1.isdefine() == True
        
        macro_obj_1 = uter.export.macro("__SOME_HEAD_H__")
        print macro_obj_1
        result &= macro_obj_1.isdefine() == True
        
        macro_obj_1 = uter.export.macro("SOME_ENABLE")
        print macro_obj_1
        result &= macro_obj_1.isdefine() == True
        
        '''
        except_happen = False
        try:
            macro_obj_1 = uter.export.macro("SOME_NO_ENABLE")
            print macro_obj_1
        except uter.error.NotExist, e:
            except_happen = True
        result &= except_happen
        '''
        self.assertEqual(True, result, "")

class test_case_cond(unittest.TestCase):
    def isdefined():
        try:
            macro_obj_1 = uter.export.macro("SOME_NO_ENABLE")
            return True
        except uter.error.NotExist, e:
            return False
    def test_1(self):
        int_obj = uter.export.variable("g_some_enable")
        self.assertEqual(True, int_obj._GetValue() == 100, "")
    @unittest.skipIf(isdefined() == False, "")
    def test_2(self):
        int_obj = uter.export.variable("g_some_no_enable")
        self.assertEqual(True, int_obj._GetValue() == 100, "")
        
        