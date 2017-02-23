#coding=utf-8

import uter
import unittest
                      
class test_case_range(unittest.TestCase):    
    def test_new(self):
        range = uter.range()
        print range
        self.assertEqual(True,  range.mods().__len__() != 0 , "")
        self.assertEqual(True,  range.cus().__len__() != 0 , "")
    def test_mod_path(self):
        range = uter.range(mod_path = "example_base/src/test.exe")
        self.assertEqual(True, range.mods().__len__() == 1, "")
        range = uter.range(mod_path = "example_base/src/testttttttt.exe")
        self.assertEqual(True, range.mods().__len__() == 0, "")
    def test_mod_regex(self):
        range = uter.range(mod_regex = ".*example_base/src/test.exe")
        self.assertEqual(True, range.mods().__len__() == 1, "")
        range = uter.range(mod_regex = ".*example_base/src/testtttttttttttt.exe")
        self.assertEqual(True, range.mods().__len__() == 0, "")
    def test_mod_main(self):
        range = uter.range(mod_regex = ".*test.exe")
        self.assertEqual(True, range.mods().__len__() == 1, "")        
    def test_cu_path(self):
        range = uter.range(mod_regex =  ".*test.exe", cu_path = "src/main.c")
        self.assertEqual(True, range.cus().__len__() == 1, "")
    def test_cu_regex(self):
        range = uter.range(mod_regex =  ".*test.exe", cu_regex = ".*src/main.c")
        self.assertEqual(True, range.cus().__len__() == 1, "")
    
class test_case_export_var(unittest.TestCase):    
    def test_exist(self):
        data = uter.export.variable("export_var")
        print data
        self.assertEqual(True,  True, "")
        
    def test_no_exist(self):
        except_happen = False
        try:
            data = uter.export.variable("export_varrrrrrrr")
        except uter.error.NotExist, e:
            except_happen = True
            print e
        self.assertEqual(True,  except_happen, "")
    
    def test_with_mod(self):
        data = uter.export.variable("export_var", range = uter.range(mod_regex = ".*test.exe"))
        print data
        self.assertEqual(True,  True, "")
        
    def test_with_cu(self):
        data = uter.export.variable("export_var", range = uter.range(cu_regex = ".*src/main.c"))
        print data
        self.assertEqual(True,  True, "")
        
    def test_with_mod_cu(self):
        data = uter.export.variable("export_var", range = uter.range(mod_regex = ".*test.exe", cu_regex = ".*src/main.c"))
        print data
        self.assertEqual(True,  True, "")
    
class test_case_export_fun(unittest.TestCase):
    def test_exist(self):
        fun = uter.export.function("export_fun")
        print fun
        self.assertEqual(True,  True, "")
        
    def test_no_exist(self):
        except_happen = False
        try:
            fun = uter.export.function("export_funnnnnnnn")
        except uter.error.NotExist, e:
            except_happen = True
            print e
        self.assertEqual(True,  except_happen, "")
    
    def test_with_mod(self):
        fun = uter.export.function("export_fun", range = uter.range(mod_regex = ".*test.exe"))
        print fun
        self.assertEqual(True,  True, "")
    
    def test_with_cu(self):
        fun = uter.export.function("export_fun", range = uter.range(cu_regex = ".*src/main.c"))
        print fun
        self.assertEqual(True,  True, "")
        
    def test_with_mod_cu(self):
        fun = uter.export.function("export_fun", range = uter.range(mod_regex = ".*test.exe", cu_path = "src/main.c"))
        print fun
        self.assertEqual(True,  True, "")
                
class test_case_export_var_static(unittest.TestCase):    
    def test_static_exist(self):
        range = uter.range(mod_regex = ".*test.exe", cu_path = "src/main.c")
        data = uter.export.variable("export_var_static", static = True, range = range)
        print data
        self.assertEqual(True,  True, "")
        
    def test_static_not_exist(self):
        except_happen = False
        range = uter.range(mod_regex = ".*test.exe", cu_path = "src/main.c")
        try:
            data = uter.export.variable("export_var_static", range = range)
            print data
        except uter.error.NotExist, e:
            except_happen = True
            print e
        self.assertEqual(True,  except_happen, "")
        
    def test_static_dup_exist(self):
        range = uter.range(mod_regex = ".*test.exe", cu_path = "src/main.c")
        data = uter.export.variable("export_var_static_dup", static = True, range = range)
        print data
        self.assertEqual(True,  True, "")
        
    def test_static_dup_not_exist(self):
        except_happen = False
        range = uter.range(mod_regex = ".*test.exe", cu_path = "src/main.c")
        try:
            data = uter.export.variable("export_var_static_dup", range = range)
            print data
        except uter.error.NotExist, e:
            except_happen = True
            print e
        self.assertEqual(True,  except_happen, "")
        
    def test_static_dup_ambiguous(self):
        except_happen = False
        range = uter.range(mod_regex = ".*test.exe")
        try:
            data = uter.export.variable("export_var_static_dup", static = True, range = range)
            print data
        except uter.error.Ambiguous, e:
            except_happen = True
            print e
        self.assertEqual(True,  except_happen, "")
        
class test_case_export_fun_static(unittest.TestCase):   
    def test_static_exist(self):
        range = uter.range(mod_regex = ".*test.exe", cu_path = "src/main.c")
        fun = uter.export.function("export_fun_static", static = True, range = range)
        print fun
        self.assertEqual(True,  True, "")
        
    def test_static_not_exist(self):
        except_happen = False
        range = uter.range(mod_regex = ".*test.exe", cu_path = "src/main.c")
        try:
            fun = uter.export.function("export_fun_static", range = range)
            print fun
        except uter.error.NotExist, e:
            except_happen = True
            print e
        self.assertEqual(True,  except_happen, "")
        
    def test_static_dup_exist(self):
        range = uter.range(mod_regex = ".*test.exe", cu_path = "src/main.c")
        fun = uter.export.function("export_fun_static_dup", static = True, range = range)
        print fun
        self.assertEqual(True,  True, "")
        
    def test_static_dup_not_exist(self):
        except_happen = False
        range = uter.range(mod_regex = ".*test.exe", cu_path = "src/main.c")
        try:
            fun = uter.export.function("export_fun_static_dup", range = range)
            print fun
        except uter.error.NotExist, e:
            except_happen = True
            print e
        self.assertEqual(True,  except_happen, "")
        
    def test_static_dup_ambiguous(self):
        except_happen = False
        range = uter.range(mod_regex = ".*test.exe")
        try:
            fun = uter.export.function("export_fun_static_dup", static = True, range = range)
            print fun
        except uter.error.Ambiguous, e:
            except_happen = True
            print e
        self.assertEqual(True,  except_happen, "")
        
class test_case_export_type(unittest.TestCase):
    def test_export_exist(self):
        datatype = uter.export.struct("export_type")
        print datatype
        self.assertEqual(True,  True, "")
    
    def test_export_not_exist(self):
        except_happen = False
        try:
            datatype = uter.export.struct("export_typeeeeeee")
            print datatype
        except uter.error.NotExist, e:
            except_happen = True
            print e
        self.assertEqual(True,  except_happen, "")
    
    def test_export_on_head_exist(self):
        datatype = uter.export.struct("export_type_on_h")
        print datatype
        self.assertEqual(True,  True, "")
    
    def test_export_on_head_not_exist(self):
        except_happen = False
        try:
            datatype = uter.export.struct("export_type_on_hhhhhhh")
            print datatype
        except uter.error.NotExist, e:
            except_happen = True
            print e
        self.assertEqual(True,  except_happen, "")
        
    def test_export_dup_ambiguous(self):
        except_happen = False
        try:
            datatype = uter.export.struct("export_type_dup")
            print datatype
        except uter.error.Ambiguous, e:
            except_happen = True
            print e
        self.assertEqual(True,  except_happen, "")
        
    def test_export_dup_exist(self):
        datatype = uter.export.struct("export_type_dup", range = uter.range(cu_path = "main.c"))
        print datatype
        self.assertEqual(True,  True, "")
        
    def test_export_dup_exist(self):
        datatype = uter.export.struct("export_type_dup", range = uter.range(cu_path = "file_1.c"))
        print datatype
        self.assertEqual(True,  True, "")
        