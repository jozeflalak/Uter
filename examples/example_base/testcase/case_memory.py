#coding=utf-8

import uter
import unittest

class test_case_malloc_free(unittest.TestCase):
    def test_mallocfree(self):
        x = uter.memory.malloc(8)
        uter.memory.free(x)
        
        x = uter.memory.malloc(10)
        uter.memory.free(x._GetAddress())
        
        x = uter.memory.malloc(2)
        print x
        self.assertEqual(True, x._GetFinalType() == uter.char, "")
        
class test_case_memory(unittest.TestCase):
    def test_memroy(self):
        x = uter.memory.auto(10)
        print x
        self.assertEqual(True, x._GetFinalType() == uter.char, "")

