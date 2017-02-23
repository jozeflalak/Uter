#coding=utf-8

import uter
import unittest

class test_case_base(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        fun_obj = uter.export.function("fun_coverage_base")
        fun_obj._Coverage._On()
    @classmethod
    def tearDownClass(cls):
        fun_obj = uter.export.function("fun_coverage_base")
        covinfo = fun_obj._Coverage._Count()
        fun_obj._Coverage._Off()
        if covinfo["covs"] != covinfo["sum"]:
            raise "Coverage not enough"
    def test_base(self):
        fun_obj = uter.export.function("fun_coverage_base")
        ret_obj = fun_obj._Call()
        self.assertEqual(True, ret_obj._GetValue() == 0, "")
    
class test_case_eq(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        fun_obj = uter.export.function("fun_coverage_if_eq")
        fun_obj._Coverage._On()
    @classmethod
    def tearDownClass(cls):
        fun_obj = uter.export.function("fun_coverage_if_eq")
        covinfo = fun_obj._Coverage._Count()
        fun_obj._Coverage._Off()
        if covinfo["covs"] != covinfo["sum"]:
            raise "Coverage not enough"
    def test_eq(self):
        fun_obj = uter.export.function("fun_coverage_if_eq")
        int_obj = uter.int(5)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 8, "")

        covinfo = fun_obj._Coverage._Count()
        self.assertEqual(True, covinfo["covs"] != covinfo["sum"], "")
        
        int_obj = uter.int(6)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 18, "")

class test_case_neq(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        fun_obj = uter.export.function("fun_coverage_if_neq")
        fun_obj._Coverage._On()
    @classmethod
    def tearDownClass(cls):
        fun_obj = uter.export.function("fun_coverage_if_neq")
        covinfo = fun_obj._Coverage._Count()
        fun_obj._Coverage._Off()
        if covinfo["covs"] != covinfo["sum"]:
            raise "Coverage not enough"
    def test_neq(self):
        fun_obj = uter.export.function("fun_coverage_if_neq")
        
        int_obj = uter.int(6)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 8, "")
        
        covinfo = fun_obj._Coverage._Count()
        self.assertEqual(True, covinfo["covs"] != covinfo["sum"], "")
        
        int_obj = uter.int(5)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 18, "")

class test_case_lt(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        fun_obj = uter.export.function("fun_coverage_if_lt")
        fun_obj._Coverage._On()
    @classmethod
    def tearDownClass(cls):
        fun_obj = uter.export.function("fun_coverage_if_lt")
        covinfo = fun_obj._Coverage._Count()
        fun_obj._Coverage._Off()
        if covinfo["covs"] != covinfo["sum"]:
            raise "Coverage not enough"
    def test_lt(self):
        fun_obj = uter.export.function("fun_coverage_if_lt")
        
        int_obj = uter.int(4)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 8, "")
        
        covinfo = fun_obj._Coverage._Count()
        self.assertEqual(True, covinfo["covs"] != covinfo["sum"], "")
        
        int_obj = uter.int(5)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 18, "")
        
        int_obj = uter.int(6)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 18, "")
        
class test_case_gt(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        fun_obj = uter.export.function("fun_coverage_if_gt")
        fun_obj._Coverage._On()
    @classmethod
    def tearDownClass(cls):
        fun_obj = uter.export.function("fun_coverage_if_gt")
        covinfo = fun_obj._Coverage._Count()
        fun_obj._Coverage._Off()
        if covinfo["covs"] != covinfo["sum"]:
            raise "Coverage not enough"
    def test_gt(self):
        fun_obj = uter.export.function("fun_coverage_if_gt")
        
        int_obj = uter.int(6)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 8, "")
        
        covinfo = fun_obj._Coverage._Count()
        self.assertEqual(True, covinfo["covs"] != covinfo["sum"], "")
        
        int_obj = uter.int(5)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 18, "")
        
        int_obj = uter.int(4)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 18, "")
        
class test_case_le(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        fun_obj = uter.export.function("fun_coverage_if_le")
        fun_obj._Coverage._On()
    @classmethod
    def tearDownClass(cls):
        fun_obj = uter.export.function("fun_coverage_if_le")
        covinfo = fun_obj._Coverage._Count()
        fun_obj._Coverage._Off()
        if covinfo["covs"] != covinfo["sum"]:
            raise "Coverage not enough"
    def test_le(self):
        fun_obj = uter.export.function("fun_coverage_if_le")
        
        int_obj = uter.int(4)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 8, "")

        covinfo = fun_obj._Coverage._Count()
        self.assertEqual(True, covinfo["covs"] != covinfo["sum"], "")

        int_obj = uter.int(5)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 8, "")

        int_obj = uter.int(6)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 18, "")
        
class test_case_ge(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        fun_obj = uter.export.function("fun_coverage_if_ge")
        fun_obj._Coverage._On()
    @classmethod
    def tearDownClass(cls):
        fun_obj = uter.export.function("fun_coverage_if_ge")
        covinfo = fun_obj._Coverage._Count()
        fun_obj._Coverage._Off()
        if covinfo["covs"] != covinfo["sum"]:
            raise "Coverage not enough"
    def test_ge(self):
        fun_obj = uter.export.function("fun_coverage_if_ge")
        
        int_obj = uter.int(6)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 8, "")

        covinfo = fun_obj._Coverage._Count()
        self.assertEqual(True, covinfo["covs"] != covinfo["sum"], "")

        int_obj = uter.int(5)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 8, "")

        int_obj = uter.int(4)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 18, "")
        
class test_case_noelse(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        fun_obj = uter.export.function("fun_coverage_if_noelse")
        fun_obj._Coverage._On()
    @classmethod
    def tearDownClass(cls):
        fun_obj = uter.export.function("fun_coverage_if_noelse")
        covinfo = fun_obj._Coverage._Count()
        fun_obj._Coverage._Off()
        if covinfo["covs"] != covinfo["sum"]:
            raise "Coverage not enough"
    def test_noelse(self):
        fun_obj = uter.export.function("fun_coverage_if_noelse")
        
        int_obj = uter.int(6)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 0, "")

        covinfo = fun_obj._Coverage._Count()
        self.assertEqual(True, covinfo["covs"] != covinfo["sum"], "")

        int_obj = uter.int(5)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 8, "")

class test_case_elseif(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        fun_obj = uter.export.function("fun_coverage_if_elseif")
        fun_obj._Coverage._On()
    @classmethod
    def tearDownClass(cls):
        fun_obj = uter.export.function("fun_coverage_if_elseif")
        covinfo = fun_obj._Coverage._Count()
        fun_obj._Coverage._Off()
        if covinfo["covs"] != covinfo["sum"]:
            raise "Coverage not enough"
    def test_elseif(self):
        fun_obj = uter.export.function("fun_coverage_if_elseif")
        
        int_obj = uter.int(1)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 8, "")

        covinfo = fun_obj._Coverage._Count()
        self.assertEqual(True, covinfo["covs"] != covinfo["sum"], "")

        int_obj = uter.int(2)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 9, "")

        int_obj = uter.int(3)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 10, "")

        int_obj = uter.int(4)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 11, "")
 
        int_obj = uter.int(5)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 12, "")
        
class test_case_switch(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        fun_obj = uter.export.function("fun_coverage_switch")
        fun_obj._Coverage._On()
    @classmethod
    def tearDownClass(cls):
        fun_obj = uter.export.function("fun_coverage_switch")
        covinfo = fun_obj._Coverage._Count()
        fun_obj._Coverage._Off()
        if covinfo["covs"] != covinfo["sum"]:
            raise "Coverage not enough"
    def test_switch(self):
        fun_obj = uter.export.function("fun_coverage_switch")
        
        int_obj = uter.int(1)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 2, "")

        covinfo = fun_obj._Coverage._Count()
        self.assertEqual(True, covinfo["covs"] != covinfo["sum"], "")

        int_obj = uter.int(2)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 3, "")

        int_obj = uter.int(3)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 4, "")

        int_obj = uter.int(4)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 100, "")

class test_case_switch_nobreak(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        fun_obj = uter.export.function("fun_coverage_switch_nobreak")
        fun_obj._Coverage._On()
    @classmethod
    def tearDownClass(cls):
        fun_obj = uter.export.function("fun_coverage_switch_nobreak")
        covinfo = fun_obj._Coverage._Count()
        fun_obj._Coverage._Off()
        if covinfo["covs"] != covinfo["sum"]:
            raise "Coverage not enough"
    def test_switch_nobreak(self):
        fun_obj = uter.export.function("fun_coverage_switch_nobreak")
        
        int_obj = uter.int(1)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 2, "")

        covinfo = fun_obj._Coverage._Count()
        self.assertEqual(True, covinfo["covs"] != covinfo["sum"], "")

        int_obj = uter.int(2)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 7, "")

        int_obj = uter.int(3)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 4, "")

        int_obj = uter.int(4)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 100, "")
        
class test_case_for(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        fun_obj = uter.export.function("fun_coverage_for")
        fun_obj._Coverage._On()
    @classmethod
    def tearDownClass(cls):
        fun_obj = uter.export.function("fun_coverage_for")
        covinfo = fun_obj._Coverage._Count()
        fun_obj._Coverage._Off()
        if covinfo["covs"] != covinfo["sum"]:
            raise "Coverage not enough"
    def test_for(self):
        fun_obj = uter.export.function("fun_coverage_for")
        
        int_obj = uter.int(0)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 0, "")

        covinfo = fun_obj._Coverage._Count()
        self.assertEqual(True, covinfo["covs"] != covinfo["sum"], "")

        int_obj = uter.int(2)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 4, "")
        
class test_case_while(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        fun_obj = uter.export.function("fun_coverage_while")
        fun_obj._Coverage._On()
    @classmethod
    def tearDownClass(cls):
        fun_obj = uter.export.function("fun_coverage_while")
        covinfo = fun_obj._Coverage._Count()
        fun_obj._Coverage._Off()
        if covinfo["covs"] != covinfo["sum"]:
            raise "Coverage not enough"
    def test_while(self):
        fun_obj = uter.export.function("fun_coverage_while")
        
        int_obj = uter.int(0)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 0, "")

        covinfo = fun_obj._Coverage._Count()
        self.assertEqual(True, covinfo["covs"] != covinfo["sum"], "")

        int_obj = uter.int(2)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 4, "")
        
class test_case_dowhile(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        fun_obj = uter.export.function("fun_coverage_dowhile")
        fun_obj._Coverage._On()
    @classmethod
    def tearDownClass(cls):
        fun_obj = uter.export.function("fun_coverage_dowhile")
        covinfo = fun_obj._Coverage._Count()
        fun_obj._Coverage._Off()
        if covinfo["covs"] != covinfo["sum"]:
            raise "Coverage not enough"
    def test_dowhile(self):
        fun_obj = uter.export.function("fun_coverage_dowhile")
        
        int_obj = uter.int(0)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 2, "")

        covinfo = fun_obj._Coverage._Count()
        self.assertEqual(True, covinfo["covs"] == covinfo["sum"], "")

        int_obj = uter.int(2)
        ret_obj = fun_obj._Call(int_obj)
        self.assertEqual(True, ret_obj._GetValue() == 4, "")
        