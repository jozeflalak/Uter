#coding=utf-8

import uter
import unittest

class test_debug_datatype(unittest.TestCase):
	def test_base_base(self):
		var = uter.export.variable("char_in_so")
		self.assertEqual(True, var._GetValue() == -3, "")
		var = uter.export.variable("uchar_in_so")
		self.assertEqual(True, var._GetValue() == 3, "")

		var = uter.export.variable("short_in_so")
		self.assertEqual(True, var._GetValue() == -3, "")
		var = uter.export.variable("ushort_in_so")
		self.assertEqual(True, var._GetValue() == 3, "")

		var = uter.export.variable("int_in_so")
		self.assertEqual(True, var._GetValue() == -3, "")
		var = uter.export.variable("uint_in_so")
		self.assertEqual(True, var._GetValue() == 3, "")

		var = uter.export.variable("long_in_so")
		self.assertEqual(True, var._GetValue() == -3, "")
		var = uter.export.variable("ulong_in_so")
		self.assertEqual(True, var._GetValue() == 3, "")

		var = uter.export.variable("longlong_in_so")
		self.assertEqual(True, var._GetValue() == -3, "")
		var = uter.export.variable("ulonglong_in_so")
		self.assertEqual(True, var._GetValue() == 3, "")
	def test_case_export_st_type(self):
		type = uter.export.struct("st_in_so")
		print type
		self.assertEqual(True, type._GetSize() == 8, "")
		self.assertEqual(True, type._GetAlign() == 4, "")

		obj = type()
		self.assertEqual(True, obj._GetSize() == 8, "")
		self.assertEqual(True, obj._GetAlign() == 4, "")
		self.assertEqual(True, obj.m_1._GetValue() == 0, "")
		self.assertEqual(True, obj.m_2._GetValue() == 0, "")
		obj.m_1._SetValue(3)
		obj.m_2._SetValue(5)
		self.assertEqual(True, obj.m_1._GetValue() == 3, "")
		self.assertEqual(True, obj.m_2._GetValue() == 5, "")
	def test_case_export_st_var(self):
		obj = uter.export.variable("g_st_in_so")
		print obj
		print obj.m_1
		print obj.m_2
		self.assertEqual(True, obj._GetSize() == 8, "")
		self.assertEqual(True, obj._GetAlign() == 4, "")
		self.assertEqual(True, obj.m_1._GetValue() == 3, "")
		self.assertEqual(True, obj.m_2._GetValue() == 5, "")
		
		obj = uter.export.variable("g_noname_st_in_so")
		print obj
		print obj.m_1
		print obj.m_2
		self.assertEqual(True, obj._GetSize() == 8, "")
		self.assertEqual(True, obj._GetAlign() == 4, "")
		self.assertEqual(True, obj.m_1._GetValue() == 3, "")
		self.assertEqual(True, obj.m_2._GetValue() == 5, "")

	def test_case_export_un_type(self):
		type = uter.export.union("un_in_so")
		print type
		print type.m_1
		print type.m_2
		self.assertEqual(True, type._GetSize() == 4, "")
		self.assertEqual(True, type._GetAlign() == 4, "")
		obj = type()
		self.assertEqual(True, obj._GetSize() == 4, "")
		self.assertEqual(True, obj._GetAlign() == 4, "")
		self.assertEqual(True, obj.m_1._GetValue() == 0, "")
		self.assertEqual(True, obj.m_2._GetValue() == 0, "")
		obj.m_2._SetValue(0x22)
		self.assertEqual(True, obj.m_2._GetValue() == 0x22, "")
		if uter.target.endian.isbig():
			self.assertEqual(True, obj.m_1._GetValue() == 0x22000000, "")
		else:
			self.assertEqual(True, obj.m_1._GetValue() == 0x22, "")
	def test_case_export_un_var(self):
		obj = uter.export.variable("g_un_in_so")
		print obj
		print obj.m_1
		print obj.m_2
		self.assertEqual(True, obj._GetSize() == 4, "")
		self.assertEqual(True, obj._GetAlign() == 4, "")
		self.assertEqual(True, obj.m_1._GetValue() == 0x12345678, "")
		if uter.target.endian.isbig():
			self.assertEqual(True, obj.m_2._GetValue() == 0x12, "")
		else:
			self.assertEqual(True, obj.m_2._GetValue() == 0x78, "")

	def test_case_export_en_type(self):
		type = uter.export.enum("en_in_so")
		print type
		self.assertEqual(True, type._GetSize() == 4, "")
		self.assertEqual(True, type._GetAlign() == 4, "")
		self.assertEqual(True, type.M_1 == 1, "")
		self.assertEqual(True, type.M_2 == 2, "")
		self.assertEqual(True, type.M_3 == 3, "")
		
		obj = type()
		self.assertEqual(True, obj._GetSize() == 4, "")
		self.assertEqual(True, obj._GetAlign() == 4, "")
		self.assertEqual(True, obj._GetValue() == 0, "")
		obj._SetValue(type.M_2)
		self.assertEqual(True, obj._GetValue() == type.M_2, "")

	def test_case_export_en_var(self):
		obj = uter.export.variable("g_en_in_so")
		print obj
		self.assertEqual(True, obj.M_1 == 1, "")
		self.assertEqual(True, obj.M_2 == 2, "")
		self.assertEqual(True, obj.M_3 == 3, "")
		self.assertEqual(True, obj._GetValue() == obj.M_2, "")

		obj = uter.export.variable("g_noname_en_in_so")
		print obj
		self.assertEqual(True, obj.M_A == 1, "")
		self.assertEqual(True, obj.M_B == 2, "")
		self.assertEqual(True, obj.M_C == 3, "")
		self.assertEqual(True, obj._GetValue() == obj.M_B, "")
	def test_case_export_typedef_type(self):
		type = uter.export.typedef("typedef_int_in_so")
		print type
		self.assertEqual(True, type._GetSize() == 4, "")
		self.assertEqual(True, type._GetAlign() == 4, "")

		obj = type()
		self.assertEqual(True, obj._GetSize() == 4, "")
		self.assertEqual(True, obj._GetAlign() == 4, "")
		self.assertEqual(True, obj._GetValue() == 0, "")
		obj._SetValue(100)
		self.assertEqual(True, obj._GetValue() == 100, "")

		type = uter.export.typedef("typedef_st_in_so")
		print type
		self.assertEqual(True, type._GetSize() == 8, "")
		self.assertEqual(True, type._GetAlign() == 4, "")

		obj = type()
		self.assertEqual(True, obj._GetSize() == 8, "")
		self.assertEqual(True, obj._GetAlign() == 4, "")
		self.assertEqual(True, obj.m_1._GetValue() == 0, "")
		self.assertEqual(True, obj.m_2._GetValue() == 0, "")
		obj.m_1._SetValue(3)
		obj.m_2._SetValue(5)
		self.assertEqual(True, obj.m_1._GetValue() == 3, "")
		self.assertEqual(True, obj.m_2._GetValue() == 5, "")

	def test_case_export_typedef_var(self):
		obj = uter.export.variable("g_typedef_int_in_so")
		print obj
		self.assertEqual(True, obj._GetSize() == 4, "")
		self.assertEqual(True, obj._GetAlign() == 4, "")
		self.assertEqual(True, obj._GetValue() == 100, "")

		obj = uter.export.variable("g_typedef_st_in_so")
		print obj
		print obj.m_1
		print obj.m_2
		self.assertEqual(True, obj._GetSize() == 8, "")
		self.assertEqual(True, obj._GetAlign() == 4, "")
		self.assertEqual(True, obj.m_1._GetValue() == 5, "")
		self.assertEqual(True, obj.m_2._GetValue() == 7, "")

	def test_case_point(self):
		pvar = uter.export.variable("g_int_point_in_so")
		print pvar
		var = uter.depoint(pvar)
		print var
		self.assertEqual(True, var._GetValue() == 199, "")
		
		pvar = uter.export.variable("g_char_point_in_so")
		self.assertEqual(True, pvar._GetValue() == 0x12345678, "")
		
		pvar = uter.export.variable("g_short_point_in_so")
		self.assertEqual(True, pvar._GetValue() == 0, "")

		pvar = uter.export.variable("g_void_point_in_so")
		self.assertEqual(True, pvar._GetValue() == 0, "")

		pvar = uter.export.variable("g_st_ppoint_in_so")
		var = uter.depoint(uter.depoint(pvar))
		self.assertEqual(True, var.m_1._GetValue() == 3, "")
		self.assertEqual(True, var.m_2._GetValue() == 5, "")

	def test_case_array(self):
		avar = uter.export.variable("array_int_1_in_so")
		print avar
		self.assertEqual(True, avar[0]._GetValue() == 0, "")
		self.assertEqual(True, avar[1]._GetValue() == 0, "")

		avar = uter.export.variable("array_int_2_in_so")
		self.assertEqual(True, avar[0][0]._GetValue() == 0, "")
		self.assertEqual(True, avar[1][1]._GetValue() == 0, "")

		avar = uter.export.variable("array_st_1_in_so")
		print avar
		self.assertEqual(True, avar[0].m_1._GetValue() == 0, "")
		self.assertEqual(True, avar[1].m_2._GetValue() == 0, "")

		avar = uter.export.variable("array_st_2_in_so")
		print avar
		self.assertEqual(True, avar[0][0].m_1._GetValue() == 0, "")
		self.assertEqual(True, avar[1][1].m_2._GetValue() == 0, "")
	
class test_debug_function(unittest.TestCase):
	def test_base_call(self):
		fun = uter.export.function("fun_0_in_so")
		print fun
		obj = fun._Call()
		self.assertEqual(True, obj._GetValue() == 20, "")

		fun = uter.export.function("fun_1_in_so")
		obj = fun._Call(uter.int(5))
		self.assertEqual(True, obj._GetValue() == 25, "")

		fun = uter.export.function("fun_2_in_so")
		obj = fun._Call(uter.int(5), uter.int(10))
		self.assertEqual(True, obj._GetValue() == 35, "")

		fun = uter.export.function("fun_3_in_so")
		obj = fun._Call(uter.int(5), uter.int(10), uter.int(20))
		self.assertEqual(True, obj._GetValue() == 55, "")

		fun = uter.export.function("fun_v_in_so")
		obj = fun._Call()
		self.assertEqual(True, obj == uter.void, "")

		fun = uter.export.function("fun_p_in_so")
		obj = fun._Call(uter.dopoint(uter.int(5)), uter.dopoint(uter.int(10)))
		self.assertEqual(True, obj._GetValue() == 35, "")

		fun = uter.export.function("fun_pp_in_so")
		obj = fun._Call(uter.dopoint(uter.dopoint(uter.int(5))), uter.dopoint(uter.dopoint(uter.int(10))))
		self.assertEqual(True, obj._GetValue() == 35, "")

		fun = uter.export.function("fun_a_int_so")
		x = uter.array(uter.int, 1)()
		x[0]._SetValue(5)
		y = uter.array(uter.int, 1)()
		y[0]._SetValue(10)
		obj = fun._Call(x, y)
		self.assertEqual(True, obj._GetValue() == 35, "")
		
		fun = uter.export.function("fun_aa_int_so")

		fun = uter.export.function("fun_st_in_so")
		sttype = uter.export.struct("st_arg")
		stobj = sttype()
		stobj.x._SetValue(5)
		stobj.y._SetValue(10)
		obj = fun._Call(stobj)
		self.assertEqual(True, obj._GetValue() == 35, "")

		fun = uter.export.function("fun_stp_in_so")
		sttype = uter.export.struct("st_arg")
		stobj = sttype()
		stobj.x._SetValue(5)
		stobj.y._SetValue(10)
		obj = fun._Call(uter.dopoint(stobj))
		self.assertEqual(True, obj._GetValue() == 35, "")
	@staticmethod
	def script_stub_1(x, y):
		return uter.int(x._GetValue() - y._GetValue())
	
	def test_base_stub(self):
		fun_stub = uter.export.function("fun_stub_in_so")
		fun_call = uter.export.function("fun_call_in_so")

		obj = fun_call._Call(uter.int(10), uter.int(5))
		self.assertEqual(True, obj._GetValue() == 15, "")

		fun_stub._Stub(self.script_stub_1)
		obj = fun_call._Call(uter.int(10), uter.int(5))
		self.assertEqual(True, obj._GetValue() == 5, "")

		fun_stub._Stub()
		obj = fun_call._Call(uter.int(10), uter.int(5))
		self.assertEqual(True, obj._GetValue() == 15, "")
        
        
class test_release_datatype(unittest.TestCase):
	def test_symbol(self):
		sym = uter.export.symbol("fun_in_release_so", section = ".text")
		print sym
		sym = uter.export.symbol("var_in_release_so", section = ".data")
		print sym
	def test_type_base(self):
		sym = uter.export.symbol("char_in_release_so", section = ".data")
		var = uter.export.dataobject(uter.char, sym.address())
		self.assertEqual(True, var._GetValue() == -3, "")
		sym = uter.export.symbol("uchar_in_release_so", section = ".data")
		var = uter.export.dataobject(uter.char, sym.address())
		self.assertEqual(True, var._GetValue() == 3, "")

		sym = uter.export.symbol("short_in_release_so", section = ".data")
		var = uter.export.dataobject(uter.char, sym.address())
		self.assertEqual(True, var._GetValue() == -3, "")
		sym = uter.export.symbol("ushort_in_release_so", section = ".data")
		var = uter.export.dataobject(uter.char, sym.address())
		self.assertEqual(True, var._GetValue() == 3, "")

		sym = uter.export.symbol("int_in_release_so", section = ".data")
		var = uter.export.dataobject(uter.char, sym.address())
		self.assertEqual(True, var._GetValue() == -3, "")
		sym = uter.export.symbol("uint_in_release_so", section = ".data")
		var = uter.export.dataobject(uter.char, sym.address())
		self.assertEqual(True, var._GetValue() == 3, "")

		sym = uter.export.symbol("long_in_release_so", section = ".data")
		var = uter.export.dataobject(uter.char, sym.address())
		self.assertEqual(True, var._GetValue() == -3, "")
		sym = uter.export.symbol("ulong_in_release_so", section = ".data")
		var = uter.export.dataobject(uter.char, sym.address())
		self.assertEqual(True, var._GetValue() == 3, "")

		sym = uter.export.symbol("longlong_in_release_so", section = ".data")
		var = uter.export.dataobject(uter.char, sym.address())
		self.assertEqual(True, var._GetValue() == -3, "")
		sym = uter.export.symbol("ulonglong_in_release_so", section = ".data")
		var = uter.export.dataobject(uter.char, sym.address())
		self.assertEqual(True, var._GetValue() == 3, "")
	def test_type_struct(self):
		type = uter.struct(("m_1", uter.int), ("m_2", uter.char))
		sym = uter.export.symbol("g_noname_st_in_release_so", section = ".data")
		var = uter.export.dataobject(type, sym.address())
		self.assertEqual(True, var.m_1._GetValue() == 3, "")
		self.assertEqual(True, var.m_2._GetValue() == 5, "")

		sym = uter.export.symbol("g_st_in_release_so", section = ".data")
		var = uter.export.dataobject(type, sym.address())
		self.assertEqual(True, var.m_1._GetValue() == 3, "")
		self.assertEqual(True, var.m_2._GetValue() == 5, "")
	def test_type_union(self):
		type = uter.union(("m_1", uter.int), ("m_2", uter.char))
		sym = uter.export.symbol("g_noname_un_in_release_so", section = ".data")
		var = uter.export.dataobject(type, sym.address())
		self.assertEqual(True, var.m_1._GetValue() == 0x12345678, "")
		if uter.target.endian.isbig():
			self.assertEqual(True, var.m_2._GetValue() == 0x12, "")
		else:
			self.assertEqual(True, var.m_2._GetValue() == 0x78, "")
	def test_type_enum(self):
		type = uter.enum(("M_A", 1), ("M_B"), ("M_C"))
		sym = uter.export.symbol("g_noname_en_in_release_so", section = ".data")
		var = uter.export.dataobject(type, sym.address())
		self.assertEqual(True, var._GetValue() == type.M_B, "")
		
		type = uter.enum(("M_1", 1), ("M_2"), ("M_3"))
		sym = uter.export.symbol("g_en_in_release_so", section = ".data")
		var = uter.export.dataobject(type, sym.address())
		self.assertEqual(True, var._GetValue() == type.M_2, "")
	def test_type_typedef(self):
		pass
		
class test_release_function(unittest.TestCase):
    def test_base_call(self):
        pass