#coding=utf-8

import __builtin__
import sys
import imp
import os
import os.path
import re
import inspect
import copy
import unittest
import UterExtern

'''
(internal class)
Uter meta class, uter data type print and == use it, when call type() return this meta class
this class use for uter data type
'''
class _UterMetaClass(__builtin__.type):
    #print uter data type
    def __str__(self):
        sys_str = __builtin__.type.__str__(self)
        def_str = self._TypeDefDesc()
        type_str = self._TypeDesc("type")
        #If no typedef, not print information about typedef 
        if __builtin__.len(def_str) == 0:
            return sys_str + "\n"  + type_str
        else:
            return sys_str + "\n"  + def_str + "\n"  + type_str
    #For use in uter data type compare 
    def __eq__(self, check_type):
        if isinstance(check_type, _UterMetaClass):
            return self._MatchType(check_type)
        else:
            return False
    #For use in uter data type compare 
    def __ne__(self, check_type):
        if isinstance(check_type, _UterMetaClass):
            return not self._MatchType(check_type)
        else:
            return True
        
'''
(internal class)
The uter top call, this class bind meta calss _UterMetaClass, so child class bind meta class too.
this class use for uter data object
'''
class _UterObject(object):
    __metaclass__ =  _UterMetaClass
    def __init__(self):
        _exp_info = None
        return None
    #Print uter data object
    def __str__(self):
        sys_str = __builtin__.type.__str__(self)
        def_str = self._TypeDefDesc()
        type_str = self._TypeDesc("instance")
        obj_str = self._ObjDesc()
        if __builtin__.len(def_str) == 0:
            return sys_str + "\n" + type_str + "\n" + obj_str
        else:
            return sys_str + "\n" + def_str + "\n" + type_str + "\n" + obj_str

class error(object):
        #Uter exception, use __metaclass__ too.
    class Base(BaseException):   
        __metaclass__ =  _UterMetaClass
        def __init__(self, desc):
            self.desc = desc
            return None
        def __str__(self):
            return self.desc
    #Execption about argument error
    class ArgError(Base):
        def __init__(self, desc):
            super(error.ArgError,self).__init__(desc)
            return None
    #Execption about some object not exist
    class NotExist(Base):
        def __init__(self, desc):
            super(error.NotExist,self).__init__(desc)
            return None
    #Execption about some object too much
    class Ambiguous(Base):
        def __init__(self, desc):
            super(error.Ambiguous,self).__init__(desc)
            return None
    #Execption about some operate error
    class OperateError(Base):
        def __init__(self, desc):
            super(error.OperateError,self).__init__(desc)
            return None
    #Execption about others
    class Unknown(Base):
        def __init__(self, desc):
            super(error.Unknown,self).__init__(desc)
            return None

            
class internal(object):
    class utils(object):
        #Make 10 base number string, fill 0 if no enough length
        @staticmethod
        def _NumStr(num, size = 0):
            num_str = __builtin__.str(num)
            if num < 0 or __builtin__.len(num_str) >= size:
                return num_str
            else:
                return "0" * (size - __builtin__.len(num_str)) + num_str
    
        #Make 16 base number string
        @staticmethod
        def _HexStr(address):
            hex_val = hex(address)
            return hex_val
    
        #Make 16 base number string, fill 0 to target bit size
        @staticmethod
        def _AddrStr(address):
            info_dict = UterExtern.TargetInfo(nouse = None)
            point_len = info_dict["types"]["point"]["size"]
            if point_len ==  4:
                if address > 0xffffffff or address < 0:
                    raise error.Unknown("Out of range on 32bit address.")
                hex_val = hex(address)
                if hex_val[-1] == "L":
                    hex_val = hex_val[:-1]
                if __builtin__.len(hex_val) == 8 + 2:
                    return hex_val
                else:
                    return "0x" + "0" * (8 - (__builtin__.len(hex_val) - 2)) + hex_val[2:]
            elif point_len == 8:
                if address > 0xffffffffffffffff or address < 0:
                    raise error.Unknown("Out of range on 64bit address")
                hex_val = hex(address)
                if hex_val[-1] == "L":
                    hex_val = hex_val[:-1]
                if __builtin__.len(hex_val) == 16 + 2:
                    return hex_val
                else:
                    return "0x" + "0" * (16 - (__builtin__.len(hex_val) - 2)) + hex_val[2:]
            else:
                raise error.Unknown("Unkonwn point size.")
        
    class memory(object):
        '''
        Record memory alloc information, when this object lost reference, release memory on target with __del__ method.
        '''
        class UterAlloc(object):
            def __init__(self, addr, len):
                self.addr = addr
                self.len = len
                return None
            def __del__(self):
                #Todo
                #UterExtern.Free(addr = __builtin__.long(self.addr))
                return None
        class UterBytesBits(object):
            #Init
            def __init__(self, address, size):
                if not (__builtin__.type(address) == __builtin__.int or __builtin__.type(address) == __builtin__.long):
                    raise error.ArgError("Argument address should python int/long.")
                if not (__builtin__.type(size) == __builtin__.int or __builtin__.type(size) == __builtin__.long):
                    raise error.ArgError("Argument size should python int/long.")
                if size <= 0:
                    raise error.ArgError("Argument size should greater then 0.")
                self._data_address = __builtin__.long(address)
                self._data_bytes = [0] * size
                return None
            #Read bytes from target memory
            def Read(self):
                byte_values = UterExtern.GetMemory(addr = self._data_address, len = __builtin__.len(self._data_bytes))
                if __builtin__.len(byte_values) != __builtin__.len(self._data_bytes):
                    raise error.OperateError("Get memory with wrong length, need %d but %d."  % (__builtin__.len(self._data_bytes), __builtin__.len(byte_values)))
                self._data_bytes = byte_values
                return None
            #Write bytes to target memory
            def Write(self):
                UterExtern.SetMemory(addr = self._data_address, bytes = __builtin__.tuple(self._data_bytes))
                return None
            #Get bytes tuple by host byte order
            def GetHostBytes(self):
                ret_tuple = ()
                for item in __builtin__.range(__builtin__.len(self._data_bytes)):
                    byte = self._data_bytes[item]
                    ret_tuple += (byte,)
                return ret_tuple
            #Set bytes tuple or list by host byte order
            def SetHostBytes(self, bytes):
                if __builtin__.type(bytes) == __builtin__.tuple:
                    pass
                elif __builtin__.type(bytes) == __builtin__.list:
                    bytes = __builtin__.tuple(bytes)
                else:
                    raise error.ArgError("Argument should python list/tuple.")
                if __builtin__.len(bytes) != __builtin__.len(self._data_bytes):
                    raise error.ArgError("Wrong number of bytes, need %d but %d."  % (__builtin__.len(self._data_bytes), __builtin__.len(bytes))) 
                for item in __builtin__.range(__builtin__.len(bytes)):
                    byteloop = bytes[item]
                    if __builtin__.type(byteloop) != __builtin__.int:
                        raise error.ArgError("byte %d should a python int."  % item)
                #Foreach bytes to _data_bytes
                for item in __builtin__.range(__builtin__.len(bytes)):
                    byteloop = bytes[item]
                    self._data_bytes[item] = byteloop
                return None    
            #Get bytes tuple by data order
            def GetDataBytes(self):
                ret_tuple = ()
                if target.endian.isbig():
                    for item in __builtin__.range(__builtin__.len(self._data_bytes)):
                        byte = self._data_bytes[item]
                        ret_tuple = (byte,) + ret_tuple
                else:
                    for item in __builtin__.range(__builtin__.len(self._data_bytes)):
                        byte = self._data_bytes[item]
                        #Reverse order
                        ret_tuple = ret_tuple + (byte,)
                return ret_tuple
            #Set bytes tuple or list by data order
            def SetDataBytes(self, bytes):
                if __builtin__.type(bytes) == __builtin__.tuple:
                    pass
                elif __builtin__.type(bytes) == __builtin__.list:
                    bytes = __builtin__.tuple(bytes)
                else:
                    raise error.ArgError("Argument should python list/tuple.")
                if __builtin__.len(bytes) != __builtin__.len(self._data_bytes):
                    raise error.ArgError("Wrong number of bytes, need %d but %d." % (__builtin__.len(self._data_bytes), __builtin__.len(bytes))) 
                for item in __builtin__.range(__builtin__.len(bytes)):
                    byteloop = bytes[item]
                    if __builtin__.type(byteloop) != __builtin__.int:
                        raise error.ArgError("byte %d should a python int." % item)
                #Foreach bytes to _data_bytes
                if target.endian.isbig():
                    for item in __builtin__.range(__builtin__.len(bytes)):
                        byteloop = bytes[item]
                        self._data_bytes[__builtin__.len(self._data_bytes)  - 1 - item] = bytes[item]
                else:
                    for item in __builtin__.range(__builtin__.len(bytes)):
                        byteloop = bytes[item]
                        self._data_bytes[item] = bytes[item]
                return None
            #print data bytes to console
            def PrintDataBytes(self):
                bytes = self.GetDataBytes()
                print bytes
            #Get data bits tuple by data order
            def GetDataBits(self):
                bytes = self.GetDataBytes()
                ret_tuple = ()
                for item in __builtin__.range(__builtin__.len(bytes)):
                    byteloop = bytes[item]
                    for itembit in __builtin__.range(8):
                        bitvalue = (byteloop & (1 << itembit)) >> itembit
                        ret_tuple += (bitvalue, )
                return ret_tuple
            #Set data bits tuple or list by data order 
            def SetDataBits(self, bits):
                if __builtin__.type(bits) == __builtin__.tuple:
                    pass
                elif __builtin__.type(bits) == __builtin__.list:
                    bits = __builtin__.tuple(bits)
                else:
                    raise error.ArgError("Argument should python list/tuple.")
                if __builtin__.len(bits) != __builtin__.len(self._data_bytes) * 8:
                    raise error.ArgError("Wrong number of bytes, need %d but %d." % (__builtin__.len(self._data_bytes), __builtin__.len(bits))) 
                for item in __builtin__.range(__builtin__.len(bits)):
                    bitloop = bits[item]
                    if __builtin__.type(bitloop) != __builtin__.int:
                        raise error.ArgError("bit %d should a python int." % item)
                #Write
                bytes = ()
                value = 0
                for item in __builtin__.range(__builtin__.len(bits)):
                    bitloop = bits[item]
                    biton = item % 8
                    value += bitloop << biton
                    if biton == 7:
                        bytes += (value,)
                        value = 0
                self.SetDataBytes(bytes)
                return None
            #print bits by data order to console
            def PrintDataBits(self):
                bits = self.GetDataBits()
                print bits
                return None
            #Left shift
            def DataLeft(self, num):
                if not (__builtin__.type(num) == __builtin__.int or __builtin__.type(num) == __builtin__.long):
                    raise error.ArgError("Argument should python int/long.")
                #Get bits
                bits = self.GetDataBits()
                if num > __builtin__.len(bits):
                    num = __builtin__.len(bits)
                #Adjust bits
                bits = bits[:-num]
                bits = (0, ) * num + bits
                #Set bits
                self.SetDataBits(bits)
                return None
            #Right shift
            def DataRight(self, num):
                if not (__builtin__.type(num) == __builtin__.int or __builtin__.type(num) == __builtin__.long):
                    raise error.ArgError("Argument should python int/long.")
                #Get bits
                bits = self.GetDataBits()
                if num > __builtin__.len(bits):
                    num = __builtin__.len(bits)
                #Adjust bits
                bits = bits[num:]
                bits = bits + (0, ) * num
                #Set bits
                self.SetDataBits(bits)
                return None
    class datatype(object):
        DataTypeList = "uter.char, uter.uchar, uter.short, uter.ushort,     " \
                                              + "uter.int, uter.uint, uter.long, uter.ulong, " \
                                              + "uter.longlong, uter.ulonglong, uter.float, uter.double, " \
                                              + "uter.struct(), uter.union(), uter.enum(), uter.array(), uter.point(), uter.typedef()"
        class TypedefBase(object):
            #Create a new class, shell of __builtin__.type
            @staticmethod
            def _CreateClass(desc, parrents):
                if __builtin__.type(desc) != __builtin__.str:
                    raise error.ArgError("Argument desc should python str.")
                if __builtin__.type(parrents) == __builtin__.tuple:
                    pass
                elif __builtin__.type(parrents) == __builtin__.list:
                    parrents = __builtin__.tuple(parrents)
                else:
                    parrents = (parrents, )
                #parrents should be template type
                for item in __builtin__.range(__builtin__.len(parrents)):
                    parrentloop = parrents[item]
                    if not internal.datatype.TypedefBase._IsTemplateType(parrentloop):
                        raise error.ArgError("Argument parrents should uter data type or data type tuple.")
                #Create new class
                dict_for_class = {}
                dict_for_class["_def_desc"] = desc
                dict_for_class["_def_parrents"] = parrents
                new_class = type(desc, parrents, dict_for_class)
                return new_class
            #Attach dict to the new class
            @staticmethod
            def _AttachDict(new_class, dict_for_class):
                if __builtin__.type(dict_for_class) != __builtin__.dict:
                    raise error.ArgError("Argument dict for class should python dict.")
                
                #_def_desc and _def_parrents in zhe class already
                dict_for_class["_def_desc"] = getattr(new_class, "_def_desc", None)
                dict_for_class["_def_parrents"] = getattr(new_class, "_def_parrents", None)
                dict_for_class["_def_created"] = None                                     #create flag, use for _IsTemplateType
                dict_for_class["_def_typedef"] = ()                                             #typedef clean
                dict_for_class["_def_dictcopy"] = None
                dict_for_class["_def_dictcopy"] = dict_for_class.copy()       #backup dict_for_class
                dict_for_class["_exp_info"] = None                                            #export information clean
                #Set attribute to the class
                for item in dict_for_class.keys():
                    setattr(new_class, item, dict_for_class[item])
                return new_class
            #Create new type class and attach dict to the new class
            @staticmethod
            def _CreateType(desc, parrents, dict_for_class):
                new_class = internal.datatype.TypedefBase._CreateClass(desc, parrents)
                return internal.datatype.TypedefBase._AttachDict(new_class, dict_for_class)
            #Check template type
            @staticmethod
            def _IsTemplateType(check_type):
                if not isinstance(check_type, _UterMetaClass):
                    return False
                if not issubclass(check_type, _UterObject):
                    return False
                if hasattr(check_type, "_def_created"):
                    return False
                return True
            #Check the type create form template type
            @staticmethod
            def _IsCreateType(check_type):
                if not isinstance(check_type, _UterMetaClass):
                    return False
                if not issubclass(check_type, _UterObject):
                    return False
                if not hasattr(check_type, "_def_created"):
                    return False
                return True
            #Copy current type class to a new type class
            @classmethod
            def _CopyType(cls):
                #The dict_for_class backup
                dict_for_class = cls._def_dictcopy
                #_def_typedef may be modified after backup, so _def_typedef copy from self class
                dict_for_class["_def_typedef"] = cls._def_typedef
                dict_for_class["_def_dictcopy"] = None
                dict_for_class["_def_dictcopy"] = dict_for_class.copy()         #re copy the dict_for_class
                dict_for_class["_exp_info"] = None                                              #export information clean
                new_class = type(cls._def_desc, cls._def_parrents, dict_for_class)
                return new_class
            #Return data type typedef information, return "" when no any typedef.
            @classmethod
            def _TypeDefDesc(cls):
                return_str = ""
                for item in __builtin__.range(__builtin__.len(cls._def_typedef)):
                    typedefloop = cls._def_typedef[item]
                    return_str += "Typedef as %s " % typedefloop
                return return_str
        
        @staticmethod 
        def TypedefClassMake(totype, name):
            if not internal.datatype.TypedefBase._IsCreateType(totype):
                raise error.ArgError("Argument totype should a uter data type.")
            if not __builtin__.type(name) == __builtin__.str:
                raise error.ArgError("Argument name should a python str.")
            #Copy totype to a new type, and rebuild typedef section
            newtype = totype._CopyType()
            newtype._def_typedef = (name, ) + newtype._def_typedef
            return newtype

        class DataTypeObjectInitArg(object):
            def __init__(self, name, address, allocfrom, inited):
                self.name = name
                self.address = address
                self.allocfrom = allocfrom
                self.inited = inited
                return None
        
        class DataTypeBase(TypedefBase, _UterObject):
            #Check the give data object match current data type class
            @classmethod
            def _MatchObj(cls, check_obj):
                if not isinstance(check_obj, internal.datatype.DataTypeBase):
                    return False
                return cls == check_obj._GetType()
            #Get type (data type class) of data object
            def _GetType(self):
                return __builtin__.type(self)
            #Get address for data object
            def _GetAddress(self):
                return self._data_address
            #A data object convert to other data object
            def _AsType(self, as_type):
                if not internal.datatype.TypedefBase._IsCreateType(as_type):
                    raise error.ArgError("Argument should type of: %s." % internal.datatype.DataTypeList) 
                #as_type is convert to data type, create a new data object use as_type
                init_arg = internal.datatype.DataTypeObjectInitArg("(no name)", self._data_address, self._data_allocfrom, True)
                return internal.datatype.DataTypeNew(as_type, init_arg)
            
            #New
            @classmethod
            def __new__(cls, *args, **kwargs):
                instance = super(internal.datatype.DataTypeBase, cls).__new__(cls, *args, **kwargs)
                instance._init_arg = None
                return instance
            #Init
            def __init__(self):
                if self._init_arg == None:
                    size = self._GetSize()
                    self._data_name  = "(no name)"
                    self._data_address = UterExtern.Malloc(len = __builtin__.int(size))
                    self._data_allocfrom = internal.memory.UterAlloc(self._data_address, size)
                    self._data_inited = False
                elif isinstance(self._init_arg, internal.datatype.DataTypeObjectInitArg):
                    self._data_name = self._init_arg.name
                    self._data_address = self._init_arg.address
                    self._data_allocfrom = self._init_arg.allocfrom
                    self._data_inited = self._init_arg.inited
                else:
                    raise error.Unknown("") 
                return None
        @staticmethod 
        def DataTypeNew(typecls, init_arg = None, *args, **kwargs):
            instance = typecls.__new__(*args, **kwargs)
            instance._init_arg = init_arg
            instance.__init__(*args, **kwargs)
            return instance
            
        #Be parent for basetype and point, enum  
        class ByteTypeBase(DataTypeBase):
            #Byte type data size
            @classmethod
            def _GetSize(cls):
                info_dict = UterExtern.TargetInfo(nouse = None)
                return info_dict["types"][cls.__base_type_name__]["size"]
            #Byte type data align
            @classmethod
            def _GetAlign(cls):
                info_dict = UterExtern.TargetInfo(nouse = None)
                return info_dict["types"][cls.__base_type_name__]["align"]
            #Copy give byte data object to self
            def _CopyValue(self, from_obj):
                if not self._MatchObj(from_obj):
                    raise error.ArgError("Argument should uter %s type." % self.__base_type_name__)
                self._SetValue(from_obj._GetValue())
                return None
            #Get unsigned value
            def _GetValueWithUnsigned(self):
                #Get date info
                info_dict = UterExtern.TargetInfo(nouse = None)
                data_size = info_dict["types"][self.__base_type_name__]["size"]
                data_signed = info_dict["types"][self.__base_type_name__]["signed"]
                #Load memory
                mem = internal.memory.UterBytesBits(self._data_address, data_size)
                mem.Read()
                #Convert to bytes
                off_values = mem.GetDataBytes()
                #Convert to unsigned value
                unsigned_value = 0 
                for item in __builtin__.range(__builtin__.len(off_values)):
                    unsigned_value +=off_values[item] << (8 * item)
                #If signed data and out of range, adjust it.
                if data_signed and unsigned_value >= (1 << (8 * data_size - 1)):
                    my_value = unsigned_value - (1 << (8 * data_size))
                else:
                    my_value = unsigned_value
                return my_value, unsigned_value
            #Get value
            def _GetValue(self):
                val,uval =  self._GetValueWithUnsigned()
                return val
            #Set value
            def _SetValue(self, value):
                if not (__builtin__.type(value) == __builtin__.int or __builtin__.type(value) == __builtin__.long):
                    raise error.ArgError("Argument should python int/long.")
                info_dict = UterExtern.TargetInfo(nouse = None)
                data_size = info_dict["types"][self.__base_type_name__]["size"]
                #Convert to unsigned data
                unsigned_value = value % (1 << (8 * data_size))
                #Convert to bytes
                off_values = ()
                for item in __builtin__.range(0, data_size):            
                    byte_val = __builtin__.int((unsigned_value  & (0xff << (item * 8))) >> (item * 8))
                    off_values += (byte_val,)
                #Save to memory
                mem = internal.memory.UterBytesBits(self._data_address, data_size)
                mem.SetDataBytes(off_values)
                mem.Write()
                return None
            def _LeftShift(self, bits):
                if not (__builtin__.type(bits) == __builtin__.int or __builtin__.type(bits) == __builtin__.long):
                    raise error.ArgError("Argument should python int/long.")
                info_dict = UterExtern.TargetInfo(nouse = None)
                data_size = info_dict["types"][self.__base_type_name__]["size"]
                mem = internal.memory.UterBytesBits(self._data_address, data_size)
                mem.Read()
                mem.DataLeft(bits)
                mem.Write()
                return None
            def _RightShift(self, bits):
                if not (__builtin__.type(bits) == __builtin__.int or __builtin__.type(bits) == __builtin__.long):
                    raise error.ArgError("Argument should python int/long.")
                info_dict = UterExtern.TargetInfo(nouse = None)
                data_size = info_dict["types"][self.__base_type_name__]["size"]
                mem = internal.memory.UterBytesBits(self._data_address, data_size)
                mem.Read()
                mem.DataRight(bits)
                mem.Write()
                return None
            def __init__(self, init_value = None):
                super(internal.datatype.ByteTypeBase, self).__init__()
                if not self._data_inited:
                    if init_value == None:
                        self._SetValue(0)
                    elif __builtin__.type(init_value) == __builtin__.int or __builtin__.type(init_value) == __builtin__.long:
                        self._SetValue(init_value)
                    elif isinstance(init_value, internal.datatype.ByteTypeBase):
                        self._CopyValue(init_value)
                    else:
                        raise error.ArgError("Argument should python int/long or uter byte data object."  )
                    #
                return None

        class BaseTemplate(ByteTypeBase):
            #Type define variable description
            @classmethod
            def _DefDesc(cls, stars = "", name = ""):
                info_dict = UterExtern.TargetInfo(nouse = None)
                basetype = info_dict["types"][cls.__base_type_name__]["daily"]
                template = basetype + ' {stars}{name}'
                return template.format(stars = stars, name = name)
            #Compare the give type
            @classmethod
            def _MatchType(cls, check_type, cmp_list = None):
                if cls is internal.datatype.BaseTemplate or check_type is internal.datatype.BaseTemplate:
                    return cls is check_type
                elif isinstance(check_type, _UterMetaClass) and issubclass(check_type, internal.datatype.BaseTemplate):
                    return cls.__base_type_name__ == check_type.__base_type_name__
                else:
                    return False
            #Type description, when print data type.
            @classmethod
            def _TypeDesc(cls, what = "type"):
                info_dict = UterExtern.TargetInfo(nouse = None)
                return_str = ""
                return_str += "Uter base %s:%s\n" % (what,cls.__base_type_name__.lower())
                return_str += "size:%d\n" % cls._GetSize()
                return_str += "C/C++ description:%s" % info_dict["types"][cls.__base_type_name__]["daily"] 
                return return_str;
            #Object description, when print data object.
            def _ObjDesc(self):
                return_str = ""
                return_str += "address:%s\n"  % internal.utils._AddrStr(self._data_address)
                val,uval =  self._GetValueWithUnsigned()
                return_str += "value:%d           (hex:%s)" % (val, internal.utils._HexStr(uval))
                return return_str
            def __init__(self, init_value = None):
                super(internal.datatype.BaseTemplate, self).__init__(init_value)
                return None
        
        class PointTemplate(ByteTypeBase):
            __base_type_name__ = "ulong"
            @classmethod
            def _DefDesc(cls, stars = "", name = ""):
                return cls._type_datatype._DefDesc(stars = stars + "*", name = name)
            @classmethod
            def _MatchType(cls, check_type, cmp_list = None):
                if cls is internal.datatype.PointTemplate or check_type is internal.datatype.PointTemplate:
                    return cls is check_type
                if not (isinstance(check_type, _UterMetaClass) and issubclass(check_type, internal.datatype.PointTemplate)):
                    return False
                #Detail
                return cls._type_datatype._MatchType(check_type._type_datatype, cmp_list)
            #Get point to data type
            @classmethod
            def _GetFinalType(cls):
                return cls._type_datatype
            @classmethod
            def _TypeDesc(cls, what = "type"):
                return_str = ""
                return_str += "Uter point %s to %s: \n" % (what, cls._type_datatype._DefDesc("", ""))
                return_str += "size:%d\n" % cls._GetSize()
                return_str += "C/C++ description: %s" % cls._type_datatype._DefDesc("*", "") 
                return return_str
            def _ObjDesc(self):
                return_str = ""
                return_str += "address(point variable on):%s\n"  % internal.utils._AddrStr(self._data_address)
                val,uval =  self._GetValueWithUnsigned()
                return_str += "value(point to):%d           (hex:%s)" % (val, internal.utils._HexStr(uval))
                return return_str
            def __init__(self, init_value = None):       
                super(internal.datatype.PointTemplate, self).__init__(init_value)
                return None
        
        @staticmethod 
        def PointClassMake(datatype):
            dict_for_class = {}
            dict_for_class["_type_datatype"] = datatype
            return internal.datatype.TypedefBase._CreateType("class for uter point type", internal.datatype.PointTemplate, dict_for_class)
        
        class EnumTemplate(ByteTypeBase):
            #enum same as int some times
            __base_type_name__ = "int"
            @classmethod
            def _DefDesc(cls, stars = "", name = ""):
                template = "enum %s {stars}{name}" % cls._type_name
                return template.format(stars = stars, name = name)
            @classmethod
            def _MatchType(cls, check_type, cmp_list = None):
                if cls is internal.datatype.EnumTemplate or check_type is internal.datatype.EnumTemplate:
                    return cls is check_type
                if not (isinstance(check_type, _UterMetaClass) and issubclass(check_type, internal.datatype.EnumTemplate)):
                    return False
                #Detail
                if __builtin__.len(cls._type_mem_tuple) != __builtin__.len(check_type._type_mem_tuple):
                    return False
                for item in __builtin__.range(__builtin__.len(cls._type_mem_tuple)):
                    my_mem_dict = cls._type_mem_tuple[item]
                    check_mem_dict = check_type._type_mem_tuple[item]
                    if not (my_mem_dict["name"] == check_mem_dict["name"] and my_mem_dict["value"] == check_mem_dict["value"]) :
                        return False
                else:
                    return True
            @classmethod
            def _Member(cls, name):
                #Use _type_mem_dict
                if __builtin__.type(name) != __builtin__.str:
                    raise error.ArgError("Argument should python str.")
                if cls._type_mem_dict.has_key(name):
                    return cls._type_mem_dict[name]["value"]
                else:
                    raise error.NotExist("No such member named %s." % name)
                return None
            @classmethod
            def _TypeDesc(cls, what = "type"):
                return_str = ""
                return_str += "Uter enum %s:%s\n" % (what, cls._type_name)
                return_str += "size:%d\n" % cls._GetSize()
                return_str += "C/C++ description: \n"
                return_str += "enum %s {\n"  % cls._type_name
                for type_mem_item in __builtin__.range(__builtin__.len(cls._type_mem_tuple)):
                    type_mem_dict = cls._type_mem_tuple[type_mem_item]
                    type_mem_name = type_mem_dict["name"]
                    type_mem_value = type_mem_dict["value"]
                    return_str +=  "%s  =  %d, \n" % (type_mem_name, type_mem_value)
                return_str += "};"
                return return_str
            def _ObjDesc(self):
                return_str = ""
                return_str += "address:%s\n" % internal.utils._AddrStr(self._data_address)
                val,uval =  self._GetValueWithUnsigned()
                return_str += "value:%d            (hex:%s) \n" % (val, internal.utils._HexStr(uval))
                return_str += "value math enumerator:"
                for type_mem_item in __builtin__.range(__builtin__.len(self._type_mem_tuple)):
                    type_mem_dict = self._type_mem_tuple[type_mem_item]
                    type_mem_name = type_mem_dict["name"]
                    type_mem_value = type_mem_dict["value"]
                    if type_mem_value == val:
                        return_str +=  "%s,  " % type_mem_name
                return return_str
            def __init__(self, init_value = None):
                super(internal.datatype.EnumTemplate, self).__init__(init_value)
                return None
        
        @staticmethod 
        def EnumClassMake(name, members):
            if __builtin__.type(name) != __builtin__.str:
                raise error.ArgError("Argument name should python str.")
            if __builtin__.type(members) == __builtin__.tuple:
                pass
            elif __builtin__.type(members) == __builtin__.list:
                members = __builtin__.tuple(members)
            else:
                raise error.ArgError("Argument members should python tuple.")
            #Data sets
            dict_for_class = {}
            type_mem_dict = {}
            type_mem_tuple = ()
            #Foreach members
            for item in __builtin__.range(__builtin__.len(members)):
                memloop = members[item]
                #Check first
                if __builtin__.type(memloop) != __builtin__.dict:
                    raise error.ArgError("Memeber %d should python dict." % (item + 1))
                if not memloop.has_key("name"):
                    raise error.ArgError("Memeber %d no name specifed." % (item + 1))
                mem_name = memloop["name"]
                if __builtin__.type(mem_name) != __builtin__.str:
                    raise error.ArgError("Memeber %d name should  python str." % (item + 1))
                    
                if not memloop.has_key("value"):
                    raise error.ArgError("Memeber %d no value specifed." % (item + 1))
                mem_value = memloop["value"]
                
                #prefix with _
                if mem_name[0] == "_":
                    mem_pystyle = False
                else:
                    mem_pystyle = True
                #Member name check
                if type_mem_dict.has_key(mem_name):
                    raise error.ArgError("Duplicate member name % s." % mem_name)
                #Create and save member information.
                tmp_dict = {"name":mem_name, "value":mem_value, "pystyle":mem_pystyle}
                type_mem_dict[mem_name] = tmp_dict
                type_mem_tuple += (tmp_dict, )
                if mem_pystyle:
                    dict_for_class[mem_name] = mem_value
            #Class dict key for enum type
            dict_for_class["_type_mem_dict"] = type_mem_dict
            dict_for_class["_type_mem_tuple"] = type_mem_tuple
            dict_for_class["_type_name"] = name
            #Create type class
            return internal.datatype.TypedefBase._CreateType("class for uter enum type", internal.datatype.EnumTemplate, dict_for_class) 

        class StructTemplate(DataTypeBase):
            @classmethod
            def _GetSize(cls):
                return cls._type_size
            @classmethod
            def _GetAlign(cls):
                return cls._type_align
            @classmethod
            def _DefDesc(cls, stars = "", name = ""):
                template = "struct " + cls._type_name + ' {stars}{name}'
                return template.format(stars = stars, name = name)
            @classmethod
            def _MatchType(cls, check_type, cmp_list = None):
                if cls is internal.datatype.StructTemplate or check_type is internal.datatype.StructTemplate:
                    return cls is check_type
                if not (isinstance(check_type, _UterMetaClass) and issubclass(check_type, internal.datatype.StructTemplate)):
                    return False
                if cmp_list == None:
                    cmp_list = []
                else:
                    #Foreach cmp_list
                    for item in __builtin__.range(__builtin__.len(cmp_list)):
                        cmp_dict = cmp_list[item]
                        if cls is cmp_dict["self"] and check_type is cmp_dict["check"]:
                            #If type checked, return pass
                            return True
                    cmp_list += [{"self":cls, "check":check_type}]
                #Detail
                if __builtin__.len(cls._type_mem_tuple) != __builtin__.len(check_type._type_mem_tuple):
                    return False
                for item in __builtin__.range(__builtin__.len(cls._type_mem_tuple)):
                    my_mem_type = cls._type_mem_tuple[item]["type"]
                    check_mem_type = check_type._type_mem_tuple[item]["type"]
                    if not my_mem_type._MatchType(check_mem_type, cmp_list):
                        return False
                else:
                    return True
            @classmethod
            def _Member(cls, name):
                if not (__builtin__.type(name) ==  __builtin__.str):
                    raise error.ArgError("Argument should python str.")
                if cls._type_mem_dict.has_key(name):
                    return cls._type_mem_dict[name]["type"]
                else:
                    raise error.NotExist("No such member named  %s." % name)
                return None
            @classmethod
            def _TypeDesc(cls, what = "type"):
                return_str = ""
                return_str += "Uter struct %s:%s\n"  % (what, cls._type_name)
                return_str += "size:%d\n" % cls._type_size
                return_str += "C/C++ description: \n"
                return_str += "struct %s {" % cls._type_name
                last_end = 0
                for type_mem_item in __builtin__.range(__builtin__.len(cls._type_mem_tuple)):
                    type_mem_dict = cls._type_mem_tuple[type_mem_item]
                    type_mem_name = type_mem_dict["name"]
                    type_mem_type = type_mem_dict["type"]
                    type_mem_off = type_mem_dict["offset"]
                    if not type_mem_off == last_end:
                        return_str += "    /*%s-%s*/" % (internal.utils._NumStr(last_end, 2), internal.utils._NumStr(type_mem_off - 1, 2))
                    return_str += "\n/*%s-%s*/"  % (internal.utils._NumStr(type_mem_off, 2), internal.utils._NumStr(type_mem_off + type_mem_type._GetSize() - 1, 2) )
                    return_str += "  %s;" % (type_mem_type._DefDesc("", type_mem_name))
                    last_end = type_mem_off + type_mem_type._GetSize()
                if not cls._type_size == last_end:
                    return_str += "    /*%s-%s*/" % (internal.utils._NumStr(last_end, 2), internal.utils._NumStr(cls._type_size - 1, 2))
                return_str += "\n};"
                return return_str
            def _ObjDesc(self):
                return_str = ""
                return_str += "address:%s\n"  % internal.utils._AddrStr(self._data_address)
                return_str += "value:\n"
                return_str += "struct %s {" % self._type_name
                last_end = 0
                for type_mem_item in __builtin__.range(__builtin__.len(self._data_mem_tuple )):
                    data_mem_dict = self._data_mem_tuple[type_mem_item]
                    data_mem_name = data_mem_dict["name"]
                    data_mem_type = data_mem_dict["type"]
                    data_mem_off = data_mem_dict["offset"]
                    data_mem_obj = data_mem_dict["obj"]
                    if not data_mem_off == last_end:
                        return_str += "    /*%s-%s*/"  % (internal.utils._NumStr(last_end, 2), internal.utils._NumStr(data_mem_off - 1, 2))
                    return_str += "\n/*%s-%s*/"  % (internal.utils._NumStr(data_mem_off, 2), internal.utils._NumStr(data_mem_off + data_mem_type._GetSize() - 1, 2))
                    return_str += "  %s;" % data_mem_type._DefDesc("", data_mem_name)
                    if isinstance(data_mem_obj, internal.datatype.BaseTemplate):
                        return_str += "  -->%d  (hex:%s)" % (data_mem_obj._GetValue(), internal.utils._HexStr(data_mem_obj._GetValue()))
                    last_end = data_mem_off + data_mem_type._GetSize()
                if not self._type_size == last_end:
                    return_str += "    /*%s-%s*/" % (internal.utils._NumStr(last_end, 2), internal.utils._NumStr(self._type_size - 1, 2))
                return_str += "\n};"
                return return_str
            def _Obj_Member(self, name):
                if not (__builtin__.type(name) ==  __builtin__.str):
                    raise error.ArgError("Argument should python str.")
                if self._data_mem_dict.has_key(name):
                    return self._data_mem_dict[name]["obj"]
                else:
                    raise error.NotExist("No such member named  %s." % name)
                return None
            def __init__(self, *args):
                super(internal.datatype.StructTemplate, self).__init__()
                #Turn _Member to _Obj_Member
                self._Member = self._Obj_Member
                #Init object data
                self._data_mem_tuple = ()
                self._data_mem_dict = {}
                #Check tuple arguments
                if not self._data_inited:
                    if not (__builtin__.len(args) == 0 or __builtin__.len(args) == __builtin__.len(self._type_mem_tuple)):
                        raise error.ArgError("Init argument number should zero or %d but %d." % (__builtin__.len(self._type_mem_tuple), __builtin__.len(args)))
                    #
                #
                for item in __builtin__.range(__builtin__.len(self._type_mem_tuple)):
                    type_mem_dict = self._type_mem_tuple[item]
                    
                    type_mem_name = type_mem_dict["name"]
                    type_mem_type = type_mem_dict["type"]
                    type_mem_off = type_mem_dict["offset"]
                    type_mem_aspy = type_mem_dict["pystyle"]
                    
                    name = type_mem_name
                    addr = self._data_address + type_mem_off
                    allocfrom = self._data_allocfrom
                    
                    if self._data_inited:
                        #Initialized already
                        init_arg = internal.datatype.DataTypeObjectInitArg(name, addr, allocfrom, True)
                        data_mem_obj = internal.datatype.DataTypeNew(type_mem_type, init_arg)
                    elif __builtin__.len(args) == 0:
                        #Need initialize, but no arguments
                        init_arg = internal.datatype.DataTypeObjectInitArg(name, addr, allocfrom, False)
                        data_mem_obj = internal.datatype.DataTypeNew(type_mem_type, init_arg)
                    else:
                        mem_arg = args[item]
                        if __builtin__.type(mem_arg) == __builtin__.tuple:
                            init_arg = internal.datatype.DataTypeObjectInitArg(name, addr, allocfrom, False)
                            data_mem_obj = internal.datatype.DataTypeNew(type_mem_type, init_arg, *mem_arg)
                        elif __builtin__.type(mem_arg) == __builtin__.list:
                            mem_arg = __builtin__.tuple(mem_arg)
                            init_arg = internal.datatype.DataTypeObjectInitArg(name, addr, allocfrom, False)
                            data_mem_obj = internal.datatype.DataTypeNew(type_mem_type, init_arg, *mem_arg)
                        else:
                            if not isinstance(type_mem_type, internal.datatype.ByteTypeBase):
                                print "Argument for struct member should be in a python list/tuple."
                            mem_arg = (mem_arg, )
                            init_arg = internal.datatype.DataTypeObjectInitArg(name, addr, allocfrom, False)
                            data_mem_obj = internal.datatype.DataTypeNew(type_mem_type, init_arg, *mem_arg)
                        #
                    #Save member data object
                    self._data_mem_dict[type_mem_name] = {"obj":data_mem_obj}
                    self._data_mem_tuple += ({"name":type_mem_name, "type":type_mem_type,  "offset":type_mem_off, "obj":data_mem_obj},)
                    if type_mem_aspy:
                        setattr(self, type_mem_name, data_mem_obj) 
                return None
        
        @staticmethod 
        def StructClassMake(name, size, members, ret_obj = None): 
            #If no any valid arguments, create a empty struct type class
            if name == None and size == None and members == None and ret_obj == None:
                return internal.datatype.TypedefBase._CreateClass("class for uter struct type", internal.datatype.StructTemplate)
            
            #Check
            if __builtin__.type(name) != __builtin__.str:
                raise error.ArgError("Argument name should python str")
            if __builtin__.type(size) != __builtin__.int:
                raise error.ArgError("Argument size should python int.")
            if __builtin__.type(members) == __builtin__.tuple:
                pass
            elif __builtin__.type(members) == __builtin__.list:
                members = __builtin__.tuple(members)
            else:
                raise error.ArgError("Argument members should python tuple.")
            #Data sets
            dict_for_class = {}
            type_mem_dict = {}                                             
            type_mem_tuple = ()
            #Foreach members
            cur_pos = max_align = 0
            for item in __builtin__.range(__builtin__.len(members)):
                memloop = members[item]
                
                if __builtin__.type(memloop) != __builtin__.dict:
                    raise error.ArgError("Member %d description should python dict." % (item + 1))
                
                if not memloop.has_key("type"):
                    raise error.ArgError("Member %d description no index named \"type\"." % (item + 1))
                mem_type = memloop["type"]
                if not internal.datatype.TypedefBase._IsCreateType(mem_type):
                    raise error.ArgError("Member %d description type should uter data type." % (item + 1))
                
                if not memloop.has_key("name"):
                    raise error.ArgError("Member %d description no index named \"name\"." % (item + 1))
                mem_name = memloop["name"]
                if not __builtin__.type(mem_name) == __builtin__.str:
                    raise error.ArgError("Member %d description name should python str." % (item + 1))
                
                if not memloop.has_key("offset"):
                    raise error.ArgError("Member %d description no index named \"offset\"." % (item + 1))
                mem_offset = memloop["offset"]
                if not __builtin__.type(mem_offset) == __builtin__.int:
                    raise error.ArgError("Member %d description offset should python str." % (item + 1))
                
                if mem_name[0] == "_":
                    mem_pystyle = False
                else:
                    mem_pystyle = True
                #Member name duplicate check
                if type_mem_dict.has_key(mem_name):
                    raise error.ArgError("Duplicate member name %s." % mem_name)
                #Create member information and save
                mem_dict = {"name":mem_name,"type":mem_type, "offset":mem_offset, "pystyle":mem_pystyle}
                type_mem_dict[mem_name] = mem_dict
                type_mem_tuple += (mem_dict, )
                if mem_pystyle:
                    dict_for_class[mem_name] = mem_type
                #Process size and align
                mem_type_size = mem_type._GetSize()
                mem_type_align = mem_type._GetAlign()
                if mem_type_size ==  0 or mem_type_align == 0 or mem_type_size <  mem_type_align or mem_type_size % mem_type_align != 0:
                    raise error.Unknown("Member %d size or align error, size %d, align %d."  % (item + 1, mem_type_size, mem_type_align))
                #Update max align
                if mem_type_align > max_align:
                    max_align = mem_type_align
                #Adjust current pos for set up current member
                remain = cur_pos % mem_type_align
                if  remain != 0:
                    cur_pos += mem_type_align - remain
                #Check and increment current pos
                if cur_pos != mem_offset:
                    raise error.Unknown("Member offset error, need %d, but %d." % (mem_offset, cur_pos))
                cur_pos += mem_type_size
            else:
                #End of foreach, last adjust current pos
                remain = cur_pos % max_align
                if remain != 0:
                    cur_pos += max_align - remain
                #Check current pos
                if cur_pos != size:
                    raise error.Unknown("Member offset error, need %d, but %d."  % (size, cur_pos))
            #Class dict key for struct type
            dict_for_class["_type_mem_dict"] = type_mem_dict
            dict_for_class["_type_mem_tuple"] = type_mem_tuple
            dict_for_class["_type_name"] = name
            dict_for_class["_type_size"] = size
            dict_for_class["_type_align"] = max_align
            if ret_obj == None:
                #No specified  ret_obj, create a new struct type class
                return internal.datatype.TypedefBase._CreateType("class for uter struct type", internal.datatype.StructTemplate, dict_for_class)
            else:
                #Attath dict to zhe given ret_obj
                return internal.datatype.TypedefBase._AttachDict(ret_obj, dict_for_class)
        
        class UnionTemplate(DataTypeBase):
            @classmethod
            def _GetSize(cls):
                return cls._type_size    
            @classmethod
            def _GetAlign(cls):
                return cls._type_align
            @classmethod
            def _DefDesc(cls, stars = "", name = ""):
                template = "union " + cls._type_name + ' {stars}{name}'
                return template.format(stars = stars, name = name)
            @classmethod
            def _MatchType(cls, check_type, cmp_list = None):
                if cls is internal.datatype.UnionTemplate or check_type is internal.datatype.UnionTemplate:
                    return cls is check_type
                if not (isinstance(check_type, _UterMetaClass) and issubclass(check_type, internal.datatype.UnionTemplate)):
                    return False
                if cmp_list == None:
                    cmp_list = []
                else:
                    #Foreach cmp_list
                    for item in __builtin__.range(__builtin__.len(cmp_list)):
                        cmp_dict = cmp_list[item]
                        if cls is cmp_dict["self"] and check_type is cmp_dict["check"]:
                            #If type checked, return pass
                            return True
                    cmp_list += [{"self":cls, "check":check_type}]
                #Detail
                if __builtin__.len(cls._type_mem_tuple) != __builtin__.len(check_type._type_mem_tuple):
                    return False
                for item in __builtin__.range(__builtin__.len(cls._type_mem_tuple)):
                    my_mem_type = cls._type_mem_tuple[item]["type"]
                    check_mem_type = check_type._type_mem_tuple[item]["type"]
                    if not my_mem_type._MatchType(check_mem_type, cmp_list):
                        return False
                else:
                    return True
            @classmethod
            def _Member(cls, name):
                if __builtin__.type(name) != __builtin__.str:
                    raise error.ArgError("Argument should python str.")
                if cls._type_mem_dict.has_key(name):
                    return cls._type_mem_dict[name]["type"]
                else:
                    raise error.NotExist("No such member named %s." % name)
                return None
            @classmethod
            def _TypeDesc(cls, what = "type"):
                return_str = ""
                return_str += "Uter union %s: %s\n" % (what, cls._type_name)
                return_str += "size:%d\n" % cls._type_size
                return_str += "C/C++ description: \n"
                return_str += "union %s {\n" % cls._type_name
                for type_mem_item in __builtin__.range(__builtin__.len(cls._type_mem_tuple)):
                    type_mem_dict = cls._type_mem_tuple[type_mem_item]
                    type_mem_name = type_mem_dict["name"]
                    type_mem_type = type_mem_dict["type"]
                    return_str += "/*%s-%s*/"  % (internal.utils._NumStr(0, 2), internal.utils._NumStr(type_mem_type._GetSize() - 1, 2))
                    return_str += "  %s; \n" % ( type_mem_type._DefDesc("", type_mem_name))
                return_str += "};"
                return return_str
            def _ObjDesc(self):
                return_str = ""
                return_str += "address:%s\n"  % internal.utils._AddrStr(self._data_address)
                return_str += "value:\n"
                return_str += "union %s{ \n" % self._type_name
                for type_mem_item in __builtin__.range(__builtin__.len(self._data_mem_tuple)):
                    data_mem_dict = self._data_mem_tuple[type_mem_item]
                    data_mem_name = data_mem_dict["name"]
                    data_mem_type = data_mem_dict["type"]
                    data_mem_obj = data_mem_dict["obj"]
                    return_str += "/*%s-%s*/"  % (internal.utils._NumStr(0, 2), internal.utils._NumStr(data_mem_type._GetSize() - 1, 2))
                    return_str += "  %s;" % data_mem_type._DefDesc("", data_mem_name)
                    if isinstance(data_mem_obj, internal.datatype.BaseTemplate):
                        return_str += "  -->%d  (hex:%s)  \n" % (data_mem_obj._GetValue(), internal.utils._HexStr(data_mem_obj._GetValue()))
                    else:
                        return_str += "\n"
                return_str += "};"
                return return_str
            def _Obj_Member(self, name):
                if __builtin__.type(name) != __builtin__.str:
                    raise error.ArgError("Argument should python str.")
                if self._data_mem_dict.has_key(name):
                    return self._data_mem_dict[name]["obj"]
                else:
                    raise error.NotExist("No such member named %s." % name)
                return None    
            def __init__(self, mem_name = None, mem_arg = None):
                super(internal.datatype.UnionTemplate, self).__init__()
                #Turn _Member to _Obj_Member
                self._Member = self._Obj_Member
                #Init object data
                self._data_mem_dict = {}
                self._data_mem_tuple = ()

                if not self._data_inited:
                    if mem_name == None:
                        if mem_arg != None:
                            raise error.ArgError("Argument mem_name should python str.")
                        #
                    elif __builtin__.type(mem_name) == __builtin__.str:
                        if not self._type_mem_dict.has_key(mem_name):
                            raise error.ArgError("No member named %s for initialization." % mem_name)
                        #
                        if mem_arg == None:
                            pass
                        elif __builtin__.type(mem_arg) == __builtin__.tuple:
                            pass
                        elif __builtin__.type(mem_arg) == __builtin__.list:
                            mem_arg = __builtin__.tuple(mem_arg)
                        else:
                            type_mem_dict = self._type_mem_dict[mem_name]
                            type_mem_type = type_mem_dict["type"]
                            if not isinstance(type_mem_type, internal.datatype.ByteTypeBase):
                                print "Argument for union member should be a python list/tuple."
                            mem_arg == (mem_arg,)
                    else:
                        raise error.ArgError("Argument mem_name should python str.")
                        #
                    #
                #Foreach union member
                for item in __builtin__.range(__builtin__.len(self._type_mem_tuple)):
                    type_mem_dict = self._type_mem_tuple[item]
                    
                    type_mem_name = type_mem_dict["name"]
                    type_mem_type = type_mem_dict["type"]
                    type_mem_aspy = type_mem_dict["pystyle"]
                    
                    name = type_mem_name
                    addr = self._data_address
                    allocfrom = self._data_allocfrom
                    
                    if self._data_inited or mem_name == None or mem_name != type_mem_name:
                        #1,Initialized already
                        #2,Need initialize, but no member name specified.
                        #3,Need initialize, but no current member
                        init_arg = internal.datatype.DataTypeObjectInitArg(name, addr, allocfrom, True)
                        data_mem_obj = internal.datatype.DataTypeNew(type_mem_type, init_arg)
                    elif mem_arg == None:
                        #Default initialize
                        init_arg = internal.datatype.DataTypeObjectInitArg(name, addr, allocfrom, False)
                        data_mem_obj = internal.datatype.DataTypeNew(type_mem_type, init_arg)
                    else:
                        #Initialize with arguments
                        init_arg = internal.datatype.DataTypeObjectInitArg(name, addr, allocfrom, False)
                        data_mem_obj = internal.datatype.DataTypeNew(type_mem_type, init_arg, *mem_arg)
                    #Save member data object
                    self._data_mem_dict[type_mem_name] = {"obj":data_mem_obj}
                    self._data_mem_tuple += ({"name":type_mem_name, "type":type_mem_type,  "obj":data_mem_obj},)
                    if type_mem_aspy:
                        setattr(self, type_mem_name, data_mem_obj) 
                    #
                return None
        
        @staticmethod 
        def UnionClassMake(name, size, members, ret_obj = None):
             #If no any valid arguments, create a empty union type class
             if name == None and size == None and members == None and ret_obj == None:
                 return internal.datatype.TypedefBase._CreateClass("class for uter union type", internal.datatype.UnionTemplate)
             
             #Check
             if __builtin__.type(name) != __builtin__.str:
                 raise error.ArgError("Argument name should a python str.")
             if __builtin__.type(size) != __builtin__.int:
                 raise error.ArgError("Argument size should a python int.")
             if __builtin__.type(members) == __builtin__.tuple:
                 pass
             elif __builtin__.type(members) == __builtin__.list:
                 members = __builtin__.tuple(members)
             else:
                 raise error.ArgError("Argument members should a python tuple.")
             #Data sets
             dict_for_class = {}
             type_mem_dict = {}
             type_mem_tuple = ()
        
             max_size = 0
             max_align = 0
             for item in __builtin__.range(__builtin__.len(members)):
                 memloop = members[item]
                 
                 if __builtin__.type(memloop) != __builtin__.dict:
                     raise error.ArgError("Member %d should a python dict." % (item + 1))
                 #
                 if not memloop.has_key("type"):
                     raise error.ArgError("Member %d no type specified." % (item + 1))
                 mem_type = memloop["type"]
                 if not internal.datatype.TypedefBase._IsCreateType(mem_type):
                     raise error.ArgError("Member %d type should a uter data type." % (item + 1))
                 #
                 if not memloop.has_key("name"):
                     raise error.ArgError("Member %d no name specified." % (item + 1))
                 mem_name = memloop["name"]
                 if __builtin__.type(mem_name) != __builtin__.str:
                     raise error.ArgError("Member %d name should a python str." % (item + 1))
                 
                 if mem_name[0] == "_":
                     mem_pystyle = False
                 else:
                     mem_pystyle = True
                 #Check member name duplicate
                 if type_mem_dict.has_key(mem_name):
                     raise error.ArgError("Duplicate member name %s." % mem_name)
                 #Make and save member information
                 tmp_dict = {"name":mem_name,"type":mem_type, "pystyle":mem_pystyle}
                 type_mem_dict[mem_name] = tmp_dict
                 type_mem_tuple += (tmp_dict, )
                 if mem_pystyle:
                     dict_for_class[mem_name] = mem_type
                 #Union member size and align is simple then struct
                 mem_type_size = mem_type._GetSize()
                 mem_type_align = mem_type._GetAlign()
                 #Update max size and max align
                 if mem_type_size > max_size:
                     max_size = mem_type_size
                 if mem_type_align > max_align:
                     max_align = mem_type_align
             else:
                 if max_size != size:
                     raise error.Unknown("Member size align error.")
                 #
            #Class dict key for union type
             dict_for_class["_type_mem_dict"] = type_mem_dict
             dict_for_class["_type_mem_tuple"] = type_mem_tuple
             dict_for_class["_type_name"] = name
             dict_for_class["_type_size"] = size
             dict_for_class["_type_align"] = max_align
             if ret_obj == None:
                 return internal.datatype.TypedefBase._CreateType("class for uter union type", internal.datatype.UnionTemplate, dict_for_class)
             else:
                 return internal.datatype.TypedefBase._AttachDict(ret_obj, dict_for_class)
                 
        class ArrayTemplate(DataTypeBase):
            @classmethod
            def _GetSize(cls):
                return cls._type_arrytype._GetSize() * cls._type_arraynum
            @classmethod
            def _GetAlign(cls):
                return cls._type_arrytype._GetAlign()
            @classmethod
            def _DefDesc(cls, stars = "", name = ""):
                template = cls._type_arrytype._DefDesc("", "")
                template += "{stars}{name}"
                for item in __builtin__.range(__builtin__.len(cls._type_rangetuple)):
                    idxsize = cls._type_rangetuple[item]
                    template += "[" + __builtin__.str(idxsize) + "]"
                return template.format(stars = stars, name = name)
            @classmethod
            def _MatchType(cls, check_type, cmp_list = None):
                if cls is internal.datatype.ArrayTemplate or check_type is internal.datatype.ArrayTemplate:
                    return cls is check_type
                if not (isinstance(check_type, _UterMetaClass) and issubclass(check_type, internal.datatype.ArrayTemplate)):
                    return False
                #Detail
                if not cls._type_arrytype._MatchType(check_type._type_arrytype):
                    return False
                if cls._type_rangetuple != check_type._type_rangetuple:
                    return False
                return True
            @classmethod
            def _GetFinalType(cls):
                return cls._type_arrytype
            @classmethod
            def _GetBound(cls):
                return cls._type_rangetuple
            @classmethod
            def _TypeDesc(cls, what = "type"):
                return_str = ""
                return_str += "Uter array %s: %s\n" % (what, cls._type_arrytype._DefDesc("", ""))
                return_str += "size:%d\n" % cls._GetSize()
                return_str += "C/C++ description:%s" % cls._DefDesc("", "")
                return return_str
            def _ObjDesc(self):
                return_str = ""
                return_str += "address:%s\n"  % internal.utils._AddrStr(self._data_address)
                return_str += "value:......."
                return return_str
            def __init__(self):
                super(internal.datatype.ArrayTemplate, self).__init__()
                return None
            #Get array child, eg: xxxx[4], xxxx[2][1]
            def __getitem__(self, key):
                if __builtin__.type(key) != __builtin__.int:
                    raise error.ArgError("Argument should python int.")
                if key < 0:
                    raise error.ArgError("Index should equal or greater than 0.")
                #Range and check
                rangetuple = self._type_rangetuple
                first_range = rangetuple[0]
                if key > first_range:
                    raise error.ArgError("Index should  less than array size %d." % first_range)
                #Self address and allocer
                addr = self._data_address
                allocfrom = self._data_allocfrom
                if __builtin__.len(rangetuple) == 1:
                    #Terminal of array
                    datatype = self._type_arrytype
                    addr +=  datatype._GetSize() * key
                    init_arg = internal.datatype.DataTypeObjectInitArg("(no name)", addr, allocfrom, True)
                    ret_instance = internal.datatype.DataTypeNew(datatype, init_arg)
                else:
                    #Sub array
                    datatype = self._type_arrytype
                    newtuple = rangetuple[1:]
                    newtype = internal.datatype.ArrayClassMake(datatype, newtuple)
                    addr +=  newtype._GetSize() * key
                    init_arg = internal.datatype.DataTypeObjectInitArg("(no name)", addr, allocfrom, True)
                    ret_instance = internal.datatype.DataTypeNew(newtype, init_arg)
                return ret_instance
        
        @staticmethod 
        def ArrayClassMake(datatype, rangetuple):
            if not internal.datatype.TypedefBase._IsCreateType(datatype):
                raise error.ArgError("Argument datatype should a uter data type.")
            if __builtin__.type(rangetuple) == __builtin__.tuple:
                pass
            elif __builtin__.type(rangetuple) == __builtin__.list:
                rangetuple = __builtin__.tuple(rangetuple)
            else:
                rangetuple = (rangetuple, )
            #Number of element in array
            num_of_array = 1
            for item in __builtin__.range(__builtin__.len(rangetuple)):
                cursize = rangetuple[item]
                if __builtin__.type(cursize) != __builtin__.int:
                    raise error.ArgError("Index range should python int.")
                if cursize < 0:
                    raise error.ArgError("Index range should greater than.")
                num_of_array *= cursize
            #Class dict key for array type
            dict_for_class = {}
            dict_for_class["_type_arrytype"] = datatype
            dict_for_class["_type_rangetuple"] = rangetuple
            dict_for_class["_type_arraynum"] = num_of_array
            #Create array type class
            return internal.datatype.TypedefBase._CreateType("class for uter array type", internal.datatype.ArrayTemplate, dict_for_class) 

        class VoidTemplate(DataTypeBase):
            @classmethod
            def _GetSize(cls):
                return 0
            @classmethod
            def _GetAlign(cls):
                return 0
            @classmethod
            def _DefDesc(cls, stars = "", name = ""):
                template = "void" + ' {stars}{name}'
                return template.format(stars = stars, name = name)
            @classmethod
            def _MatchType(cls, check_type, cmp_list = None):
                if cls is internal.datatype.VoidTemplate or check_type is internal.datatype.VoidTemplate:
                    return cls is check_type
                if not (isinstance(check_type, _UterMetaClass) and issubclass(check_type, internal.datatype.VoidTemplate)):
                    return False
                return True
            @classmethod
            def _TypeDesc(cls, what = "type"):
                return_str = ""
                return_str += "Uter void %s: \n"  % what 
                return_str += "size:NA\n"
                return_str += "C/C++ description: void "
                return return_str
            @classmethod
            def _MatchObj(cls, check_obj):
                return cls._MatchType(check_obj)
            @classmethod
            def _GetAddress(cls):
                return 0
            def __init__(self):
                raise error.OperateError("Not support.")
    
    class function(object):
        class FunctionObjectInitArg(object):
            def __init__(self, init_arg):
                self.init_arg = init_arg
                return None
        
        class FunctionBase(_UterObject):
            @classmethod
            def _MatchObj(cls, check_obj):
                if not isinstance(check_obj, internal.function.FunctionBase):
                    return False
                return cls == check_obj._GetType()
            @classmethod
            def _TypeDefDesc(cls):
                return ""
            def _GetType(self):
                return __builtin__.type(self)
            def _SetAddress(self, address):
                if __builtin__.type(address) == __builtin__.int:
                    self.__fun_address__ = __builtin__.long(address)
                elif __builtin__.type(address) == __builtin__.long:
                    self.__fun_address__ = address
                else:
                    raise error.ArgError("Argument address should a python int or python long.") 
                return None
            def _GetAddress(self):
                return self.__fun_address__
            @classmethod
            def __new__(cls, *args, **kwargs):
                instance = super(internal.function.FunctionBase, cls).__new__(cls, *args, **kwargs)
                instance._init_arg = None
                return instance
            def __init__(self, address, length):
                if self._init_arg == None:
                    if __builtin__.type(address) == __builtin__.int or __builtin__.type(address) == __builtin__.long:
                        self.__fun_address__ = __builtin__.long(address)
                    else:
                        raise error.ArgError("Argument address should a python int or python long.") 
                    #
                    if __builtin__.type(length) == __builtin__.int or __builtin__.type(length) == __builtin__.long:
                        self.__fun_high__ = __builtin__.long(length)
                    else:
                        raise error.ArgError("Argument length should a python int or python long.") 
                    #
                    self.__fun_name__ = "(no name)"
                    self.__fun_inited = False
                elif isinstance(self._init_arg, internal.function.FunctionObjectInitArg):
                    self.__fun_name__ = self._init_arg.init_arg["name"]
                    self.__fun_address__ = self._init_arg.init_arg["low"]
                    self.__fun_high__ = self._init_arg.init_arg["high"]
                    self.__fun_inited = True
                else:
                    raise error.Unknown("" )
                self._Coverage = internal.coverage(self)
                return None
        @staticmethod 
        def FunctionNew(typecls, init_arg = None, *args, **kwargs):
            instance = typecls.__new__(*args, **kwargs)
            instance._init_arg = init_arg
            instance.__init__(*args, **kwargs)
            return instance
        
        class FunctionTemplate(FunctionBase):
            @classmethod
            def _GetSize(cls):
                raise error.OperateError("Function type not support _GetSize().")
            @classmethod
            def _GetAlign(cls):
                raise error.OperateError("Function type not support _GetAlign().")
            @classmethod
            def _DefDesc(cls, stars = "", name = ""):
                template = ""
                template += cls.__fun_ret_type__._DefDesc("", "")
                template += "({stars}{name})("
                for item in __builtin__.range(__builtin__.len(cls.__fun_args_type__)):
                    arg_type = cls.__fun_args_type__[item]
                    template += arg_type._DefDesc("", "")
                    if item != __builtin__.len(cls.__fun_args_type__) - 1:
                        template += ", "
                else:
                    template += ")"
                return template.format(stars = stars, name = name)
            @classmethod
            def _MatchType(cls, check_type, cmp_list = None):
                if cls is internal.function.FunctionTemplate or check_type is internal.function.FunctionTemplate:
                    return cls is check_type
                if not (isinstance(check_type, _UterMetaClass) and issubclass(check_type, internal.function.FunctionTemplate)):
                    return False
                #Detail
                if not cls.__fun_ret_type__._MatchType(check_type.__fun_ret_type__):
                    return False
                if __builtin__.len(cls.__fun_args_type__) != __builtin__.len(check_type.__fun_args_type__):
                    return False
                for item in __builtin__.range(__builtin__.len(cls.__fun_args_type__)):
                    my_arg_type = cls.__fun_args_type__[item]
                    check_arg_type = check_type.__fun_args_type__[item]
                    if not my_arg_type._MatchType(check_arg_type):
                        return False
                else:
                    return True
            @classmethod
            def _RetType(cls):
                return cls.__fun_ret_type__
            @classmethod
            def _ArgType(cls):
                return cls.__fun_args_type__
            @classmethod
            def _TypeDesc(cls, what = "type"):
                return_str = ""
                return_str += "Uter fuction %s: (no name)\n" % what
                return_str += "Type:\n"
                return_str += "C/C++ description:"
                return_str += "%s ("  % cls.__fun_ret_type__._DefDesc("", "(no name)")
                for item in __builtin__.range(__builtin__.len(cls.__fun_args_type__)):
                    arg_type = cls.__fun_args_type__[item]
                    return_str +=  arg_type._DefDesc("", "")
                    if item != __builtin__.len(cls.__fun_args_type__) - 1:
                        return_str += ", "
                return_str +=   ")"
                return return_str
            def _ObjDesc(self):
                return_str = ""
                return_str += "name:%s\n" % self.__fun_name__
                return_str +=  "address(function at):%s\n" % internal.utils._AddrStr(self.__fun_address__)
                return_str +=  "value:NA \n"
                return_str +=  "length:%d" % self.__fun_high__
                return return_str
            def __init__(self, address = 0, length = 0):
                super(internal.function.FunctionTemplate, self).__init__(address, length)
                return None
            
            '''
            Call function
            '''
            def _Call(self, *args):
                #Check
                if __builtin__.len(args)  != __builtin__.len(self.__fun_args_type__):
                    raise error.ArgError("Argument number should same as function, need  %d but %d."  % (__builtin__.len(self.__fun_args_type__), __builtin__.len(args)))
                for item in __builtin__.range(__builtin__.len(args)):
                    #Actual argument object
                    argloop = args[item]
                    if not isinstance(argloop, internal.datatype.DataTypeBase):
                        raise error.ArgError("Argument %d should uter object." % (item + 1) )
                    #Formal argument type
                    argtype = self.__fun_args_type__[item]
                    #Convert array to point
                    if isinstance(argloop, internal.datatype.ArrayTemplate):
                        argloop = dopoint(argloop[0])
                    if not argtype._MatchObj(argloop):
                        raise error.ArgError("Argument %d should object of  %s." % (item + 1, argtype._DefDesc()))
                    #
                #Create return object
                ret_len = self.__fun_ret_type__._GetSize()
                ret_align = self.__fun_ret_type__._GetAlign()
                if self.__fun_ret_type__ == void:
                    ret_address = 0
                    ret_alloc = None
                else:
                    ret_address = UterExtern.Malloc(len = __builtin__.int(ret_len))
                    ret_alloc = internal.memory.UterAlloc(ret_address, ret_len)
                #
                #Prepare to call, return object information and actual arguments information
                call_ret = {"address":__builtin__.long(ret_address), "len":ret_len, "align":ret_align, "desc":internal.type2desc(self.__fun_ret_type__)}
                call_args = ()
                for item in __builtin__.range(__builtin__.len(args)):
                    #Actual argument object
                    argloop = args[item]
                    #Formal argument type
                    argtype = self.__fun_args_type__[item]
                    #Convert array to point
                    if isinstance(argloop, internal.datatype.ArrayTemplate):
                        argloop = dopoint(argloop[0])
                    call_args += ({"address":__builtin__.long(argloop._GetAddress()), "len":argloop._GetSize(), "align":argloop._GetAlign(), "desc":internal.type2desc(argtype)},)
                #
                #Call
                call_result = UterExtern.Call(addr = __builtin__.long(self.__fun_address__), ret = call_ret, args = call_args)
                #Do return 
                if self.__fun_ret_type__ == void:
                    return void
                else:
                    init_arg = internal.datatype.DataTypeObjectInitArg("(no name)", ret_address, ret_alloc, True)
                    ret_instance = internal.datatype.DataTypeNew(self.__fun_ret_type__, init_arg)
                    return ret_instance
            '''
            Stub the function with script function
            '''
            def _Stub(self, stub = None):
                if stub == None:
                    UterExtern.Stub(set = False, addr = self.__fun_address__)
                    return None
                #Check stub
                if not inspect.isfunction(stub):
                    raise error.ArgError("Argument stub should be python function.")
                argspec = inspect.getargspec(stub)
                if argspec.keywords != None:
                    raise error.ArgError("Stub function can not be a keyword  base argument.")
                elif argspec.varargs != None:
                    #Tuple varargs is ok
                    pass
                else:
                    num_of_arg_give = __builtin__.len(argspec.args)
                    num_of_arg_need = __builtin__.len(self.__fun_args_type__)
                    if num_of_arg_give != num_of_arg_need:
                        raise error.ArgError("Stub function should %d arguments, but %d." % (num_of_arg_need, num_of_arg_give))
                #Prepare to stub
                stub_ret = {"type":self.__fun_ret_type__, "len":self.__fun_ret_type__._GetSize(), "align":self.__fun_ret_type__._GetAlign(), "desc":internal.type2desc(self.__fun_ret_type__)}
                stub_args = ()
                for item in __builtin__.range(__builtin__.len(self.__fun_args_type__)):
                    argloop = self.__fun_args_type__[item]
                    stub_args += ({"type":argloop, "len":argloop._GetSize(), "align":argloop._GetAlign(), "desc":internal.type2desc(argloop)},)
                #About mod and cu
                if self._exp_info == None:
                    my_mod = None
                    my_cu = None
                else:
                    my_mod = self._exp_info["mod"]
                    my_cu = self._exp_info["cu"]
                #Do stub
                UterExtern.Stub(set = True, addr = self.__fun_address__, \
                driver_arg = {"driver":internal.function.FunctionStubDriver, "name":self.__fun_name__, "mod":my_mod, "cu":my_cu, "stub":stub, "ret":stub_ret, "args":stub_args })
                return None
        
        @staticmethod 
        def FunctionClassMake(ret_type, args_dict_tuple):
            if not internal.datatype.TypedefBase._IsCreateType(ret_type):
                raise error.ArgError("Argument ret_type should a uter data type.")
            #
            if __builtin__.type(args_dict_tuple) == __builtin__.tuple:
                pass
            elif __builtin__.type(args_dict_tuple) == __builtin__.list:
                args_dict_tuple = __builtin__.tuple(args_dict_tuple)
            else:
                raise error.ArgError("Argument args_dict_tuple should a python tuple.")
            #Foreach all function arguments
            arg_types = ()
            for item in __builtin__.range(__builtin__.len(args_dict_tuple)):
                argloop =  args_dict_tuple[item]
                #
                if __builtin__.type(argloop) != __builtin__.dict:
                    raise error.ArgError("Function argument %d should python dict." % (item + 1))
                #
                if not argloop.has_key("type"):
                     raise error.ArgError("Function argument %d no type specified." % (item + 1))
                argtype = argloop["type"]
                if not internal.datatype.TypedefBase._IsCreateType(argtype):
                    raise error.ArgError("Function argument %d should a uter data type." % (item + 1))
                #
                arg_types += (argtype, )
            #Class dict key for function type
            dict_for_class = {}
            dict_for_class["__fun_ret_type__"] = ret_type
            dict_for_class["__fun_args_type__"] = arg_types   
            new_class = internal.datatype.TypedefBase._CreateType("class for uter function type", internal.function.FunctionTemplate, dict_for_class) 
            return new_class

        '''
        Uter script stub function driver
        '''
        @staticmethod 
        def FunctionStubDriver(driver_args, run_info):
            try:
                #No need check everything
                stub = driver_args["stub"]
                name = driver_args["stub"]
                rettype = driver_args["ret"]["type"]
                argstype = driver_args["args"]
                #Foreach make arguments object
                stub_args = ()
                for item in __builtin__.range(__builtin__.len(argstype)):
                    argloop = argstype[item]["type"]
                    infoloop = run_info[item]
                    #Argument data object in target stack now, so no need trace the data object.
                    name = "(no name)"
                    addr = infoloop["address"]
                    init_arg = internal.datatype.DataTypeObjectInitArg(name, addr, None, True)
                    obj = internal.datatype.DataTypeNew(argloop, init_arg)
                    stub_args += (obj, )
                #Block for call
                try:
                    #Call the stub script
                    ret = stub(*stub_args)
                except Exception, e:
                    #Call stub script have a execption 
                    print "Call stub for function %s exception" % name
                    print e
                    return {"head":"execption", "body":e}
                else:
                    #Call stub script no any execption 
                    if rettype._MatchObj(ret):
                        #Call stub script ok
                        return {"head":"success", "body": {"address":__builtin__.long(ret._GetAddress()), "len":ret._GetSize()}}
                    else:
                        #Call stub script return an error object
                        print "Stub return object not match function %s, should object of %s." % (name, rettype._DefDesc())
                        return {"head":"nomatch", "body":ret}
            except Exception,e:
                print "Stub driver exception"
                print e
                return {"head":"error", "body":e}
    '''Module Symbol'''
    class symbol(object):
        def __init__(self, mod, name, source, section, bind, offset, address):
            self._mod = mod
            self._name = name
            self._source = source
            self._section = section
            self._bind = bind
            self._offset = offset
            self._address = address
            return
        def mod(self):
            return self._mod
        def name(self):
            return self._name
        def source(self):
            return self._source
        def section(self):
            return self._section
        def offset(self):
            return self._offset
        def address(self):
            return self._address
        def __str__(self):
            return_str = __builtin__.type.__str__(self) + "\n"
            return_str += "Uter export symbol  object\n"
            return_str += "Module:    %s\n" % self._mod
            return_str += "Name:    %s\n" % self._name
            return_str += "Source:    %s\n" % self._source
            return_str += "Section:    %s\n" % self._section
            return_str += "Offset:    %s\n" % internal.utils._AddrStr(self._offset)
            return_str += "Address:    %s" % internal.utils._AddrStr(self._address)
            return return_str
    '''Function coverage'''
    class coverage(object):
        def __init__(self, function):
            self.__function = function
        '''Start coverage'''
        def _On(self):
            UterExtern.FunCover(addr = self.__function._GetAddress(), opt = "on")
            return None
        '''Stop coverage'''
        def _Off(self):
            UterExtern.FunCover(addr = self.__function._GetAddress(), opt = "off")
            return None
        '''Clear coverage'''
        def _Reset(self):
            UterExtern.FunCover(addr = self.__function._GetAddress(), opt = "reset")
            return None
        '''Get coverage information'''
        def _Count(self):
            call_result = UterExtern.FunCover(addr = self.__function._GetAddress(), opt = "count")
            return {"covs":call_result["covs"], "sum": call_result["sum"]}
        '''Show coverage'''
        def _Detail(self):
            return UterExtern.FunCover(addr = self.__function._GetAddress(), opt = "show")

    class desc2type(object):
        @staticmethod 
        def base(type_list, type_desc_dict):
            if type_desc_dict["body"] == "char":
                return char
            elif type_desc_dict["body"] == "uchar":
                return uchar
            elif type_desc_dict["body"] == "short":
                return short
            elif type_desc_dict["body"] == "ushort":
                return ushort
            elif type_desc_dict["body"] == "int":
                return int
            elif type_desc_dict["body"] == "uint":
                return uint
            elif type_desc_dict["body"] == "long":
                return long
            elif type_desc_dict["body"] == "ulong":
                return ulong
            elif type_desc_dict["body"] == "longlong":
                return longlong
            elif type_desc_dict["body"] == "ulonglong":
                return ulonglong
            else:
                raise "Unknown  target base data type"
            return None
        
        @staticmethod 
        def struct(type_list, type_desc_dict):
            struct_desc = type_desc_dict["body"]
            struct_name = struct_desc["name"]
            struct_members = struct_desc["member"]
            struct_size = struct_desc["size"]    
            #Create a empty struct type and set to  type_list with type description
            new_class = internal.datatype.StructClassMake(None, None, None)
            type_list += [{"desc":type_desc_dict, "type":new_class}]
            #Foreach all member type description
            members = ()
            for item in __builtin__.range(__builtin__.len(struct_members)):
                struct_member_item = struct_members[item]
                if struct_member_item["head"] == "struct_mem":
                    struct_member_desc = struct_member_item["body"]
                    st_mem_type_desc = struct_member_desc["type"]
                    st_mem_name = struct_member_desc["name"]
                    st_mem_offset = struct_member_desc["offset"]
                    st_mem_type = internal.desc2type.all(type_list, st_mem_type_desc)
                    members += ({"name":st_mem_name,  "type": st_mem_type ,"offset":st_mem_offset}, )
                else:
                    raise error.Unknown("Unknown data type description for struct member." )
            #Configure the empty struct type
            return internal.datatype.StructClassMake(struct_name, struct_size, members, new_class)
        
        @staticmethod 
        def union(type_list, type_desc_dict):
            union_desc = type_desc_dict["body"]
            union_name = union_desc["name"]
            union_members = union_desc["member"]
            union_size = union_desc["size"]
            #Create a empty union type and set to  type_list with type description
            new_class = internal.datatype.UnionClassMake(None, None, None)
            type_list += [{"desc":type_desc_dict, "type":new_class}]
            #Foreach all member type description
            members = ()
            for item in __builtin__.range(__builtin__.len(union_members)):
                union_member_item = union_members[item]
                if union_member_item["head"] == "union_mem":
                    union_member_desc = union_member_item["body"]
                    un_mem_type_desc = union_member_desc["type"]
                    un_mem_name = union_member_desc["name"]
                    un_mem_type = internal.desc2type.all(type_list, un_mem_type_desc)
                    members += ({"name":un_mem_name,  "type": un_mem_type }, )
                else:
                    raise error.Unknown("Unknown data type description for union member." )
            #Configure the empty union type
            return internal.datatype.UnionClassMake(union_name, union_size, members, new_class)
        
        @staticmethod 
        def enum(type_list, type_desc_dict):
            enum_desc = type_desc_dict["body"]
            enum_name = enum_desc["name"]
            enum_members = enum_desc["member"]
            enum_size = enum_desc["size"]
            #Foreach all member description
            members = ()
            for item in __builtin__.range(__builtin__.len(enum_members)):
                enum_member_item = enum_members[item]
                if enum_member_item["head"] == "enum_mem":
                    enum_member_desc = enum_member_item["body"]
                    enum_mem_name = enum_member_desc["name"]
                    enum_mem_value = enum_member_desc["value"]
                    members += ({"name":enum_mem_name,  "value":enum_mem_value }, )
                else:
                    raise error.Unknown("Unknown data type description for enum member." )
            #Create and configure the enum type
            return internal.datatype.EnumClassMake(enum_name, members)
        
        @staticmethod 
        def point(type_list, type_desc_dict):
            point_desc = type_desc_dict["body"]
            point_totype = point_desc["pointto"]
            data_type = internal.desc2type.all(type_list, point_totype)
            #Make a point type with the point to type
            return internal.datatype.PointClassMake(data_type)
        
        @staticmethod 
        def array(type_list, type_desc_dict):
            array_desc = type_desc_dict["body"]
            array_totype = array_desc["arrayto"]
            array_index = array_desc["index"]
            data_type = internal.desc2type.all(type_list, array_totype)
            #Make a array type with the array to type
            return internal.datatype.ArrayClassMake(data_type, __builtin__.tuple(array_index))
        
        @staticmethod 
        def typedef(type_list, type_desc_dict):
            typedef_desc = type_desc_dict["body"]
            typedef_totype = typedef_desc["typeto"]
            typedef_name = typedef_desc["name"]
            data_type = internal.desc2type.all(type_list, typedef_totype)
            #Make a typedef type with the typeto to type
            return internal.datatype.TypedefClassMake(data_type, typedef_name)
        
        @staticmethod 
        def void(type_list, type_desc_dict):
            return void
        
        @staticmethod 
        def subrt(type_list, type_desc_dict):
            func_desc = type_desc_dict["body"]
            func_ret = func_desc["ret"]
            func_args = func_desc["args"]
            #Return type
            func_ret_type = internal.desc2type.all(type_list, func_ret)
            #Arguments type
            fun_arg_dict_types = ()
            for item in __builtin__.range(__builtin__.len(func_args)):
                argloop =  func_args[item]
                arg_type = internal.desc2type.all(type_list, argloop)
                fun_arg_dict_types += ({"type":arg_type},)
            #Make subrt data type with return and arguments type
            return internal.function.FunctionClassMake(func_ret_type, fun_arg_dict_types)
        
        @staticmethod 
        def func(type_list, type_desc_dict):
            func_desc = type_desc_dict["body"]
            func_ret = func_desc["ret"]
            func_args = func_desc["args"]
            #Return type
            func_ret_type = internal.desc2type.all(type_list, func_ret)
            #Arguments type
            fun_arg_dict_types = ()
            for item in __builtin__.range(__builtin__.len(func_args)):
                argloop =  func_args[item]
                arg_type = internal.desc2type.all(type_list, argloop)
                fun_arg_dict_types += ({"type":arg_type},)
            #Make function data type with return and arguments type
            return internal.function.FunctionClassMake(func_ret_type, fun_arg_dict_types)
        
        @staticmethod 
        def all(type_list, type_desc_dict):
            if type_list == None:
                type_list = []
            else:
                #Search type description in type in list
                for item in __builtin__.range(__builtin__.len(type_list)):
                    type_dict = type_list[item]
                    if type_dict["desc"] is type_desc_dict:
                        return type_dict["type"]
            #Which data type?
            typehead = type_desc_dict["head"]
            if typehead == "basetype":
                return internal.desc2type.base(type_list, type_desc_dict)
            elif typehead == "struct":
                return internal.desc2type.struct(type_list, type_desc_dict)
            elif typehead == "union":
                return internal.desc2type.union(type_list, type_desc_dict)
            elif typehead == "enum":
                return internal.desc2type.enum(type_list, type_desc_dict)
            elif typehead == "point":
                return internal.desc2type.point(type_list, type_desc_dict)
            elif typehead == "array":
                return internal.desc2type.array(type_list, type_desc_dict)
            elif typehead == "typedef":
                return internal.desc2type.typedef(type_list, type_desc_dict)
            elif typehead == "void":
                return internal.desc2type.void(type_list, type_desc_dict)
            elif typehead == "subroutine":
                return internal.desc2type.subrt(type_list, type_desc_dict)
            elif typehead == "function":
                return internal.desc2type.func(type_list, type_desc_dict)
            else:
                raise error.Unknown("Unknown  target data type." + __builtin__.str(typehead))
            return None

    
    @staticmethod 
    def type2desc(type_class):
        if not internal.datatype.TypedefBase._IsCreateType(type_class):
            raise error.ArgError("Argument type should a uter data type.")
        if issubclass(type_class, internal.datatype.BaseTemplate):
            return type_class.__base_type_name__
        elif issubclass(type_class, internal.datatype.EnumTemplate):
            return "enum"
        elif issubclass(type_class, internal.datatype.StructTemplate):
            return "struct"
        elif issubclass(type_class, internal.datatype.UnionTemplate):
            return "union"
        elif issubclass(type_class, internal.datatype.PointTemplate):
            return "point"
        elif issubclass(type_class, internal.datatype.ArrayTemplate):
            return "array"
        elif issubclass(type_class, internal.datatype.VoidTemplate):
            return "void"
        else:
            raise error.Unknown("Unknown data type.")
        
    class export(object):
        @staticmethod 
        def type(typeid, name, range):
            #Range check
            if range == None:
                range = target.mirror.range()
            elif not isinstance(range, target.mirror.range):
                raise error.ArgError("Argument mod should be uter.range object")
            cu_list = range.cus()
            #Name check
            if __builtin__.type(name) != __builtin__.str:
                raise error.ArgError("Argument name should be python str object.")
            if __builtin__.len(name) == 0:
                raise error.ArgError("Argument name should not be empty.")
            #Get all matched types from cu list
            type_merge_list = []
            for item in __builtin__.range(__builtin__.len(cu_list)):
                key_mod = cu_list[item]["mod"]
                key_cu = cu_list[item]["cu"]
                try:
                    typeinfo = UterExtern.Type(mod = key_mod, cu = key_cu, type = typeid, name = name)
                except Exception,e:
                    pass    #Ignore exception
                else:
                    #Find type in type_merge_list
                    for item_type in __builtin__.range(__builtin__.len(type_merge_list)):
                        if typeinfo == type_merge_list[item_type]["type"]:
                            merge_type = type_merge_list[item_type]
                            break
                    else:
                        #If no this type, create a merger type with empty on list
                        merge_type = {"type":typeinfo, "on":[]}
                        type_merge_list += [merge_type]
                    #Save type an location information to merger list
                    merge_type["on"] += [{"mod":key_mod, "cu":key_cu, "type":typeinfo}]    
            #Check find result
            if __builtin__.len(type_merge_list) == 0:
                raise error.NotExist("No such type.")
            elif __builtin__.len(type_merge_list) > 1:
                str = "Type duplicate:\n"
                for merge_item in __builtin__.range(__builtin__.len(type_merge_list)):
                    mergesetloop = type_merge_list[merge_item]["on"]
                    for type_item in __builtin__.range(__builtin__.len(mergesetloop)):
                        typesetloop = mergesetloop[type_item]
                        str += "Module:" + typesetloop["mod"] \
                                + "      Compile unit:" +  typesetloop["cu"]  + "\n"
                raise error.Ambiguous(str)
            #Only one type matched
            target_type = type_merge_list[0]["type"]
            type_cls = internal.desc2type.all(None, target_type)
            return type_cls
        
        @staticmethod 
        def macro(name, range):
            #Range check
            if range == None:
                range = target.mirror.range()
            elif not isinstance(range, target.mirror.range):
                raise error.ArgError("Argument mod should be uter.range object")
            cu_list = range.cus()
            #Name check
            if __builtin__.type(name) != __builtin__.str:
                raise error.ArgError("Argument name should be python str object.")
            if __builtin__.len(name) == 0:
                raise error.ArgError("Argument name should not be empty.")
            #Get all matched types from cu list
            type_merge_list = [] 
            for item in __builtin__.range(__builtin__.len(cu_list)):
                key_mod = cu_list[item]["mod"]
                key_cu = cu_list[item]["cu"]
                try:
                    macro = UterExtern.Macro(mod = key_mod, cu = key_cu, name = name)
                except Exception,e:
                    pass    #Ignore exception
                else:
                    for item_type in __builtin__.range(__builtin__.len(type_merge_list)):
                        if macro == type_merge_list[item_type]["macro"]:
                            merge_type = type_merge_list[item_type]
                            break
                        else:
                            merge_type = {"macro":macro, "on":[]}
                        merge_type["on"] += [{"mod":key_mod, "cu":key_cu, "macro":data}]
            #Go on
            if __builtin__.len(type_merge_list) == 0:
                raise error.NotExist("No such macro named %s." % name)
            elif __builtin__.len(type_merge_list) > 1:
                str = "Macro %s duplicate:\n" % name
                for merge_item in __builtin__.range(__builtin__.len(type_merge_list)):
                    mergesetloop = type_merge_list[merge_item]["on"]
                    for type_item in __builtin__.range(__builtin__.len(mergesetloop)):
                        typesetloop = mergesetloop[type_item]
                        str += "Module:" + typesetloop["mod"] \
                                + "      Compile unit:" +  typesetloop["cu"]  + "\n"
                raise error.Ambiguous(str)
            #Only one matched macro
            macroinfo = type_merge_list[0]["macro"]
            if macroinfo["head"] == "macro_define":
                return UterMacro({"name":macro_name, "type":macroinfo["head"], "data":macroinfo["body"]})
            elif macroinfo["head"] == "macro_undef":
                return UterMacro({"name":macro_name, "type":macroinfo["head"], "data":macroinfo["body"]})
            else:
                raise error.Unknown("Unknown macro result.")
        
        @staticmethod 
        def variable(name, static, range):
            #Range check
            if range == None:
                range = target.mirror.range()
            elif not isinstance(range, target.mirror.range):
                raise error.ArgError("Argument mod should be uter.range object")
            cu_list = range.cus()
            #Name check
            if __builtin__.type(name) != __builtin__.str:
                raise error.ArgError("Argument name should be python str object.")
            if __builtin__.len(name) == 0:
                raise error.ArgError("Argument name should not be empty.")
            #Get all matched variable from cu list
            var_set_list = []
            for item in __builtin__.range(__builtin__.len(cu_list)):
                key_mod = cu_list[item]["mod"]
                key_cu = cu_list[item]["cu"]
                try:
                    varinfo = UterExtern.Variable(mod = key_mod, cu = key_cu, name = name)
                except Exception,e:
                    pass    #Ignore exception
                else:
                    if static == False and varinfo["ext"] == False:
                        continue
                    else:
                        var_set_list += [{"mod":key_mod, "cu":key_cu, "var":varinfo}]
            #Check find result
            if __builtin__.len(var_set_list) == 0:
                raise error.NotExist("No such variable named %s." % name)
            elif __builtin__.len(var_set_list) > 1:
                str = "Variable duplicate:\n"
                for varset_item in __builtin__.range(__builtin__.len(var_set_list)):
                    varsetloop = var_set_list[varset_item]
                    str += "Module:" + varsetloop["mod"] \
                             + "      Compile unit:" +  varsetloop["cu"] \
                             + "    External:" + __builtin__.str(varsetloop["var"]["ext"])  + "\n"
                raise error.Ambiguous(str)
            #Only one variable matched
            final_result = var_set_list[0]
            target_mod = final_result["mod"]
            target_cu = final_result["cu"]
            target_var = final_result["var"]
            #Get data type
            type_cls = internal.desc2type.all(None, target_var["type"])
            #Create data object
            name = target_var["var"]["name"]
            addr = target_var["var"]["address"]
            init_arg = internal.datatype.DataTypeObjectInitArg(name, addr, None, True)
            ret_instance = internal.datatype.DataTypeNew(type_cls, init_arg)
            ret_instance._exp_info = {"mod":target_mod, "cu":target_cu}
            return ret_instance
        
        @staticmethod 
        def function(name, static, range):
            #Range check
            if range == None:
                range = target.mirror.range()
            elif not isinstance(range, target.mirror.range):
                raise error.ArgError("Argument mod should be uter.range object")
            cu_list = range.cus()
            #Name check
            if __builtin__.type(name) != __builtin__.str:
                raise error.ArgError("Argument name should be python str object.")
            if __builtin__.len(name) == 0:
                raise error.ArgError("Argument name should not be empty.")
            #Get all matched function from cu list
            fun_set_list = []
            for item in __builtin__.range(__builtin__.len(cu_list)):
                key_mod = cu_list[item]["mod"]
                key_cu = cu_list[item]["cu"]
                try:
                    funinfo = UterExtern.Function(mod = key_mod, cu = key_cu, name = name)
                except Exception,e:
                    pass
                else:
                    if static == False and funinfo["ext"] == False:
                        continue
                    else:
                        fun_set_list += [{"mod":key_mod, "cu":key_cu, "fun":funinfo}]
            #Check find result
            if __builtin__.len(fun_set_list) == 0:
                raise error.NotExist("No such function named %s." % name)
            elif __builtin__.len(fun_set_list) > 1:
                str = "Function duplicate:"
                for funset_item in __builtin__.range(__builtin__.len(fun_set_list)):
                    funsetloop = fun_set_list[funset_item]
                    str += "Module:" + funsetloop["mod"] \
                             + "      Compile unit:" +  funsetloop["cu"] \
                             + "    External:" + __builtin__.str(funsetloop["fun"]["ext"])  + "\n"
                raise error.Ambiguous(str)
            #Only one function matched
            final_result = fun_set_list[0]
            target_mod = final_result["mod"]
            target_cu = final_result["cu"]
            target_fun = final_result["fun"]
            #Get data type
            type_cls = internal.desc2type.all(None, target_fun["type"])
            #Create data object
            init_arg = internal.function.FunctionObjectInitArg(target_fun["fun"])
            ret_instance = internal.function.FunctionNew(type_cls, init_arg)
            ret_instance._exp_info = {"mod":target_mod, "cu":target_cu}
            return ret_instance
        
        @staticmethod 
        def functions(*args):
            return None
        
        @staticmethod
        def symbol(name, source, section, bind, range):
            #Range check
            if range == None:
                range = target.mirror.range()
            elif not isinstance(range, target.mirror.range):
                raise error.ArgError("Argument mod should be uter.range object")
            mod_list = range.mods()
            #Name check
            if __builtin__.type(name) == __builtin__.str:
                if __builtin__.len(name) == 0:
                    raise error.ArgError("Argument name should not be empty.")
            else:
                raise error.ArgError("Argument name should be python str object.")
            #Source check
            if __builtin__.type(source) == __builtin__.str:
                if __builtin__.len(name) == 0:
                    raise error.ArgError("Argument source should not be empty.")
            else:
                raise error.ArgError("Argument source should be python str object.")
            #Section check
            if __builtin__.type(section) != __builtin__.str:
                raise error.ArgError("Argument section should be python str object.")
            #Bind check
            if __builtin__.type(bind) != __builtin__.str:
                raise error.ArgError("Argument bind should be python str object.")
            
            #Get all matched symbol from mod list
            sym_set_list = []
            for item in __builtin__.range(__builtin__.len(mod_list)):
                key_mod = mod_list[item]
                
                syminfo = UterExtern.Symbol(mod = key_mod, name = name, source = source, section = section, bind = bind)
                sym_set_list = sym_set_list + syminfo
            #
            #Check find result
            if __builtin__.len(sym_set_list) == 0:
                raise error.NotExist("No such symbol named %s." % name)
            elif __builtin__.len(sym_set_list) > 1:
                str = "Symbol duplicate:\n"
                for symset_item in __builtin__.range(__builtin__.len(sym_set_list)):
                    symset_item = sym_set_list[symset_item]
                    str += "Module:" + symset_item["mod"] + "    Name:" +  symset_item["name"] + "    Section:" + symset_item["section"] \
                          + "    Offset:" + internal.utils._AddrStr(symset_item["offset"]) + "    Address:" + internal.utils._AddrStr(symset_item["address"]) + "\n"
                raise error.Ambiguous(str)
            #Only one symbol matched
            final_result = sym_set_list[0]
            return internal.symbol(mod = final_result["mod"], name = final_result["name"], \
                                                          source = source, section = final_result["section"], bind = None, \
                                                          offset = final_result["offset"], address = final_result["address"])

class memory(object):
    @staticmethod 
    def malloc(len):
        if not (__builtin__.type(len) == __builtin__.int or __builtin__.type(len) == __builtin__.long):
            raise error.ArgError("Argument should python int/long.")
        if len <= 0:
            raise error.ArgError("Malloc size should greater then 0.")
        address = UterExtern.Malloc(len = __builtin__.int(len))
        arraytype = array(char, len)
        init_arg = internal.datatype.DataTypeObjectInitArg("(no name)", address, None, True)
        return internal.datatype.DataTypeNew(arraytype, init_arg)

    @staticmethod 
    def free(addr_or_obj):
        if __builtin__.type(addr_or_obj) == __builtin__.int or __builtin__.type(addr_or_obj) == __builtin__.long:
            #Free memory by address
            UterExtern.Free(addr = __builtin__.long(addr_or_obj))
        elif isinstance(addr_or_obj, internal.datatype.ArrayTemplate):
            #Free memory by array object
            if addr_or_obj._data_allocfrom == None:
                UterExtern.Free(addr = __builtin__.long(addr_or_obj._GetAddress()))
            else:
                raise error.ArgError("Free array object should create from uter.malloc.")
            #
        else:
            raise error.ArgError("Argument should uter array object or  python int/long.")
        return None

    @staticmethod 
    def auto(len):
        if not (__builtin__.type(len) == __builtin__.int or __builtin__.type(len) == __builtin__.long):
            raise error.ArgError("Argument should python int/long.")
        if len <= 0:
            raise error.ArgError("Memory size should greater then 0.")
        address = UterExtern.Malloc(len = __builtin__.int(len))
        #use a internal.memory.UterAlloc object trace the memory
        alloc = internal.memory.UterAlloc(address, len)
        arraytype = array(char, len)
        init_arg = internal.datatype.DataTypeObjectInitArg("(no name)", address, alloc, True)
        return internal.datatype.DataTypeNew(arraytype, init_arg)

def dopoint(from_obj):
    if isinstance(from_obj, _UterObject):
        point_type = point(from_obj._GetType())
        return point_type(from_obj._GetAddress())
    else:
        raise error.ArgError("Argument should a uter data object.")

def depoint(with_point):
    if isinstance(with_point, internal.datatype.PointTemplate):
        init_arg = internal.datatype.DataTypeObjectInitArg("(no name)", with_point._GetValue(), None, True)
        return internal.datatype.DataTypeNew(with_point._GetFinalType(), init_arg)
    else:
        raise error.ArgError("Argument should a uter point object.")

'''
Create point data type
BNF syntax:

For example:
new_point_type_class = uter.point(uter.char)
new_point_type_class = uter.point(uter.int)

new_struct_type_class = uter.struct((uter.int, "a"), (uter.int, "b"))
new_point_type_class = uter.point(new_struct_type_class)
'''
def point(datatype):
    if not internal.datatype.TypedefBase._IsCreateType(datatype):
        raise error.ArgError("Argument should uter data type."  )
    return internal.datatype.PointClassMake(datatype)

'''
Create enum data type
BNF syntax:

For example:
new_enum_type_class = uter.enum("ENUM_MEM_1")
new_enum_type_class = uter.enum("ENUM_MEM_1", "ENUM_MEM_2")
new_enum_type_class = uter.enum("ENUM_MEM_1", "ENUM_MEM_2", "ENUM_MEM_3")

new_enum_type_class = uter.enum(("ENUM_MEM_1", 5), "ENUM_MEM_2", "ENUM_MEM_3")
new_enum_type_class = uter.enum(("ENUM_MEM_1", -1), "ENUM_MEM_2", "ENUM_MEM_3")
new_enum_type_class = uter.enum(("ENUM_MEM_1", -1), ("ENUM_MEM_2", 1), ("ENUM_MEM_3", 2))
'''
def enum(*args):
    #Just use for member name check 
    mem_dict = {}
    #Save member information
    mem_tuple = ()
    #Foreach arguments
    cur_enum_val = 0
    for item in __builtin__.range(__builtin__.len(args)):
        argloop = args[item]
        #Get member name and value
        mem_name = None
        mem_value = None
        if __builtin__.type(argloop) == __builtin__.tuple or __builtin__.type(argloop) == __builtin__.list:
            if __builtin__.type(argloop) == __builtin__.list:
                argloop = __builtin__.tuple(argloop)
            #Parses member tuple
            for item_mem in __builtin__.range(__builtin__.len(argloop)):
                mem_arg = argloop[item_mem]
                if __builtin__.type(mem_arg) == __builtin__.str:
                    #The first python str
                    if mem_name == None:
                        mem_name = mem_arg
                    else:
                        raise error.ArgError("Too much name on member %d." % (item + 1))
                elif __builtin__.type(mem_arg) == __builtin__.int:
                    #The first python int
                    if mem_value == None:
                        mem_value = mem_arg
                    else:
                        raise error.ArgError("Too much value on member %d."  % (item + 1))
                else:
                    raise error.ArgError("No support argument on member %d."  % (item + 1))
        elif __builtin__.type(argloop) == __builtin__.str:
            #Juse python str for member name
            mem_name = argloop
        else:
            raise error.ArgError("Argument %d should python str or python tuple/list." % (item + 1))
        #Check
        if mem_name == None:
            raise error.ArgError("No name on member %d." % (item + 1))
        if mem_value == None:
            mem_value = cur_enum_val
        #Update next enum
        cur_enum_val = mem_value + 1
        #Check member name duplicate
        if mem_dict.has_key(mem_name):
            raise error.ArgError("Duplicate member name %s." % mem_name)
        mem_dict[mem_name] = None
        mem_tuple += ({"name": mem_name, "value":mem_value},)
    #Make enum type with arguments information
    return internal.datatype.EnumClassMake("(no name)", mem_tuple)

'''
Create struct data type
BNF syntax:

For example:
new_struct_type_class = uter.struct((uter.int, "a"), (uter.int, "b"))
'''
def struct(*args):   
    mem_dict = {}                               
    mem_tuple = ()
    #Foreach arguments
    for item in __builtin__.range(__builtin__.len(args)):
        argloop = args[item]
        if __builtin__.type(argloop) == __builtin__.tuple or __builtin__.type(argloop) == __builtin__.list:
            if __builtin__.type(argloop) == __builtin__.list:
                argloop = __builtin__.tuple(argloop)
            #Parse member tuple
            mem_type = None
            mem_name = None
            for item_mem in __builtin__.range(__builtin__.len(argloop)):
                mem_arg = argloop[item_mem]
                if __builtin__.type(mem_arg) == str:
                    #First python string for member name
                    if mem_name == None:
                        mem_name = mem_arg
                        if __builtin__.len(mem_name) == 0:
                            raise error.ArgError("Zero length for member name on member %d." % (item + 1))
                    else:
                        raise error.ArgError("Too much member name on member %d."  % (item + 1))
                elif internal.datatype.TypedefBase._IsCreateType(mem_arg):
                    #First uter data type for member type
                    if mem_type == None:
                        mem_type = mem_arg
                    else:
                        raise error.ArgError("Too much member type on member %d." % (item + 1))
                else:
                    raise error.ArgError("Unsupport argument for member %d." % (item + 1))
            #Check
            if mem_name == None:
                raise error.ArgError("No name for member %d." % (item + 1))
            if mem_type == None:
                raise error.ArgError("No type for memeber %d." % (item + 1))
            #Check member name duplicate
            if mem_dict.has_key(mem_name):
                raise error.ArgError("Duplicate member named %s." % mem_name)
            #Make and save member information
            mem_dict[mem_name] = None
            mem_tuple += ({"name": mem_name, "type":mem_type, "offset":None},)
        else:
            raise error.ArgError("Unsupport argument %d." % (item + 1))
    #Reloop struct member, check size
    current_pos = max_align = 0
    for item in __builtin__.range(__builtin__.len(mem_tuple)):
        mem_dict = mem_tuple[item]
        
        this_type = mem_dict["type"]
        this_size = this_type._GetSize()
        this_align = this_type._GetAlign()
        if this_size ==  0 or this_align == 0 or  this_size < this_align or this_size % this_align != 0:
            raise error.Unknown("Member %d size or align error, size %d, align %d." % (item + 1, this_size, this_align))
        #Update max align
        if this_align > max_align:
            max_align = this_align
        #Adjust current pos for set up current member
        remain = current_pos % this_align
        if remain != 0:
            current_pos += this_align - remain
        #Set offset
        mem_dict["offset"] = current_pos
        current_pos += this_size
    else:
        #End of foreach, last adjust current pos
        remain = current_pos % max_align
        if remain != 0:
            current_pos += max_align - remain
    #Make struct type class
    return internal.datatype.StructClassMake("(no name)", current_pos, mem_tuple)

'''
Create union data type
BNF syntax:

For example:

'''
def union(*args):
    mem_dict = {}
    mem_tuple = ()
    #Foreach arguments
    for item in __builtin__.range(__builtin__.len(args)):
        argloop = args[item]
        if __builtin__.type(argloop) == __builtin__.tuple or __builtin__.type(argloop) == __builtin__.list:
            if  __builtin__.type(argloop) == __builtin__.list:
                argloop = __builtin__.tuple(argloop)
            #Parse member tuple
            mem_type = None
            mem_name = None
            for item_mem in __builtin__.range(__builtin__.len(argloop)):
                mem_arg = argloop[item_mem]
                if __builtin__.type(mem_arg) == __builtin__.str:
                    #First python str for member name
                    if mem_name == None:
                        mem_name = mem_arg
                        if __builtin__.len(mem_name) == 0:
                            raise error.ArgError("Zero length member name on member %d." % (item + 1))
                    else:
                        raise error.ArgError("Too much member name on member %d."  % (item + 1))
                elif internal.datatype.TypedefBase._IsCreateType(mem_arg):
                    #First data type for member type
                    if mem_type == None:
                        mem_type = mem_arg
                    else:
                        raise error.ArgError("Too much member type on member %d." % (item + 1))
                else:
                    raise error.ArgError("Unsupport argument on member %d." % (item + 1))
            #Check
            if mem_name == None:
                raise error.ArgError("No name for member %d." % (item + 1))
            if mem_type == None:
                raise error.ArgError("No type for memeber %d." % (item + 1))
            #Check member name duplicate
            if mem_dict.has_key(mem_name):
                raise error.ArgError("Duplicate member name %s." % mem_name)
            #Make and save member information
            mem_dict[mem_name] = None
            mem_tuple += ({"name": mem_name, "type":mem_type},)
        else:
            raise error.ArgError("Argument should a python tuple on member %d." % (item + 1))
    #Reloop struct member, check size
    max_size = 0
    max_align = 0
    for kloop in __builtin__.range(__builtin__.len(mem_tuple)):
        mem_dict = mem_tuple[kloop]
        
        this_type = mem_dict["type"]
        this_size = this_type._GetSize()
        this_align = this_type._GetAlign()
        if this_size == 0 or this_align == 0 or this_size < this_align or this_size % this_align != 0:
            raise error.Unknown("Member %d size or align error, size %d, align %d." % (item + 1, this_size, this_align))
        #Update max size and max align  
        if this_size > max_size:
            max_size = this_size
        if this_align > max_align:
            max_align = this_align
    #Make struct type class
    return internal.datatype.UnionClassMake("(no name)", max_size, mem_tuple)

'''
Create array data type
BNF syntax:

For example:

'''
def array(datatype, rangetuple):
    if not internal.datatype.TypedefBase._IsCreateType(datatype):
        raise error.ArgError("Argument datatype should a uter data type.")
    if __builtin__.type(rangetuple) == __builtin__.tuple:
        pass
    elif __builtin__.type(rangetuple) == __builtin__.list:
        rangetuple = __builtin__.tuple(rangetuple)
    else:
        rangetuple = (rangetuple, )
    #Parse array index
    indextuple = ()
    for item in __builtin__.range(__builtin__.len(rangetuple)):
        cursize = rangetuple[item]
        if __builtin__.type(cursize) != __builtin__.int:
            raise error.ArgError("Index range should python int.")
        if cursize <= 0:
            raise error.ArgError("Index range should greater than.")
        indextuple += (cursize, )
    #Create array data type class
    return internal.datatype.ArrayClassMake(datatype, indextuple)

'''
Create typedef data type
BNF syntax:

For example:

'''
def typedef(totype, name):
    if not internal.datatype.TypedefBase._IsCreateType(totype):
        raise error.ArgError("Argument totype should a uter data type.")
    if not __builtin__.type(name) == __builtin__.str:
        raise error.ArgError("Argument name should a python str.")
    #Make a new class with new typedef
    return internal.datatype.TypedefClassMake(totype, name)

'''
Built-in data type
BNF syntax:

For example:

'''
char = internal.datatype.TypedefBase._CreateType("class for uter base type", internal.datatype.BaseTemplate, {"__base_type_name__":"char"}) 
uchar = internal.datatype.TypedefBase._CreateType("class for uter base type", internal.datatype.BaseTemplate, {"__base_type_name__":"uchar"}) 
short = internal.datatype.TypedefBase._CreateType("class for uter base type", internal.datatype.BaseTemplate, {"__base_type_name__":"short"}) 
ushort = internal.datatype.TypedefBase._CreateType("class for uter base type", internal.datatype.BaseTemplate, {"__base_type_name__":"ushort"}) 
int = internal.datatype.TypedefBase._CreateType("class for uter base type", internal.datatype.BaseTemplate, {"__base_type_name__":"int"}) 
uint = internal.datatype.TypedefBase._CreateType("class for uter base type", internal.datatype.BaseTemplate, {"__base_type_name__":"uint"}) 
long = internal.datatype.TypedefBase._CreateType("class for uter base type", internal.datatype.BaseTemplate, {"__base_type_name__":"long"}) 
ulong = internal.datatype.TypedefBase._CreateType("class for uter base type", internal.datatype.BaseTemplate, {"__base_type_name__":"ulong"}) 
longlong = internal.datatype.TypedefBase._CreateType("class for uter base type", internal.datatype.BaseTemplate, {"__base_type_name__":"longlong"}) 
ulonglong = internal.datatype.TypedefBase._CreateType("class for uter base type", internal.datatype.BaseTemplate, {"__base_type_name__":"ulonglong"}) 
float = internal.datatype.TypedefBase._CreateType("class for uter base type", internal.datatype.BaseTemplate, {"__base_type_name__":"float"}) 
double = internal.datatype.TypedefBase._CreateType("class for uter base type", internal.datatype.BaseTemplate, {"__base_type_name__":"double"}) 
void = internal.datatype.TypedefBase._CreateType("class for uter void type", internal.datatype.VoidTemplate, {}) 
voidpoint = point(void)

'''
Create function type
BNF syntax:

For example:

'''
def function(ret_type, args_type_tuple):
    if not internal.datatype.TypedefBase._IsCreateType(ret_type):
        raise error.ArgError("Argument ret_type should a uter data type.")
    if __builtin__.type(args_type_tuple) == __builtin__.tuple:
        pass
    elif __builtin__.type(args_type_tuple) == __builtin__.list:
        args_type_tuple = __builtin__.tuple(args_type_tuple)
    else:
        args_type_tuple = (args_type_tuple,)
    args_dict_tuple = ()
    #Foreach all function arguments
    for item in __builtin__.range(__builtin__.len(args_type_tuple)):
        args_type = args_type_tuple[item]
        
        if not internal.datatype.TypedefBase._IsCreateType(args_type):
            raise error.ArgError("Function argument %d should uter data type." % (item + 1))
        
        args_dict_tuple += ({"type":args_type},)
    #Create a function type class
    return internal.function.FunctionClassMake(ret_type, args_dict_tuple)

'''
TODO:
'''
class  UterMacro(object):
    def __init__(self, info_dict):
        self.info = info_dict
    def __str__(self):
        return_str = __builtin__.type.__str__(self) + "\n"
        #return_str += "Mod: " + self.info["mod"]
        #return_str += "Cu: " + self.info["cu"]
        if self.info["type"] == "macro_define":
            return_str +=  "#define " + self.info["data"] 
        elif self.info["type"] == "macro_undef":
            return_str +=  "#undef " + self.info["data"] 
        else:
            raise error.Unknown("Unknown macro type.")
        return return_str
    def numbers(self):
        #检查
        if self.info["type"] == "macro_define":
            pass
        elif self.info["type"] == "macro_undef":
            raise error.OperateError("Macro is undefine.")
        else:
            raise error.Unknown("Unknown macro type.")
        #do it
        define_data = self.info["data"]
        define_name = self.info["name"]
        define_value = define_data[__builtin__.len(define_name) + 1:]
        match_result = re.findall(r'0[Xx][0-9A-Fa-f]+|[+-]?[0-9]+', define_value)
        #类型转换
        result = []
        for item in __builtin__.range(__builtin__.len(match_result)):
            match_loop = match_result[item]
            if match_loop[:2] == "0x" or match_loop[:2] == "0X":
                result += [__builtin__.int(match_loop, 16)]
            else:
                result += [__builtin__.int(match_loop)]
        return result
    def number(self):
        return self.numbers()[0]
    def strings(self):
        #检查
        if self.info["type"] == "macro_define":
            pass
        elif self.info["type"] == "macro_undef":
            raise error.OperateError("Macro is undefine.")
        else:
            raise error.Unknown("Unknown macro type.")
        #do it
        define_data = self.info["data"]
        define_name = self.info["name"]
        define_value = define_data[__builtin__.len(define_name) + 1:]
        #先进行C转义处理
        define_value = define_value.replace(r'\r', '\r')
        define_value = define_value.replace(r'\n', '\n')
        define_value = define_value.replace(r'\t', '\t')
        define_value = define_value.replace(r'\\', '\\')
        define_value = define_value.replace(r'\"', '\x200')
        match_result = re.findall(r'"([^"]*)"', define_value)
        result = []
        for item in __builtin__.range(__builtin__.len(match_result)):
            match_loop = match_result[item]
            result += [match_loop.replace('\x200', r'"')]
        return result
    def string(self):
        return self.strings()[0]
    def name(self):
        if self.info["type"] == "macro_define":
            return self.info["name"] 
        elif self.info["type"] == "macro_undef":
            return self.info["name"] 
        else:
            raise error.Unknown("Unknown macro type.")
    def macro(self):
        if self.info["type"] == "macro_define":
            return "#define " + self.info["data"] 
        elif self.info["type"] == "macro_undef":
            return "#undef" + self.info["data"] 
        else:
            raise error.Unknown("Unknown macro type.")
    def isdefine(self):
        if self.info["type"] == "macro_define":
            return True
        elif self.info["type"] == "macro_undef":
            return False
        else:
            raise error.Unknown("Unknown macro type.")

'''Export interface, export data type or export data object'''
class export(object):
    '''Export any type, include struct union enum typedef:'''
    @staticmethod 
    def anytype(name, range = None):
        return internal.export.type("type", name, range)
    '''Export struct data type:'''
    @staticmethod
    def struct(name, range = None):
        return internal.export.type("struct", name, range)
    '''Export union data type:'''
    @staticmethod
    def union(name, range = None):
        return internal.export.type("union", name, range)
    '''Export enum data type:'''
    @staticmethod 
    def enum(name, range = None):
        return internal.export.type("enum", name, range)
    '''Export typedef data type:'''
    @staticmethod 
    def typedef(name, range = None):
        return internal.export.type("typedef", name, range)
    '''Export function data type:'''
    @staticmethod 
    def function(name, range = None):
        return internal.export.type("function", name, range)
    '''Export macro data:'''
    @staticmethod 
    def macro(name, range = None):
        return internal.export.macro(name, range)
    '''Export variable data object:'''
    @staticmethod 
    def variable(name, static = False, range = None):
        return internal.export.variable(name, static, range)
    '''Export function data object:'''
    @staticmethod 
    def function(name, static = False, range = None):
        return internal.export.function(name, static, range)
    '''Export overloaded function data object:'''
    @staticmethod 
    def functions(name, static = False, range = None):
        return internal.export.functions(name, static, range)
    '''Export symbol object:'''
    @staticmethod
    def symbol(name, source = ".dynsym", section = "", bind = "", range = None):
        return internal.export.symbol(name, source, section, bind, range)
    '''Export data object'''
    @staticmethod
    def dataobject(datatype, address):
        init_arg = internal.datatype.DataTypeObjectInitArg("(no name)" , address, None, True)
        return internal.datatype.DataTypeNew(datatype, init_arg)
class string(object):
    class ascii(object):
        @staticmethod
        def get(getaddr, getlen = None):
            return_str = ""
            #Argument check
            if __builtin__.type(getaddr) == __builtin__.int or __builtin__.type(getaddr) == __builtin__.long:
                pass
            else:
                raise error.ArgError("Argument getaddr should a python int/long.")
            #
            if getlen == None:
                pass
            elif __builtin__.type(getlen) == __builtin__.int or __builtin__.type(getlen) == __builtin__.long:
                if getlen < 0:
                    raise error.ArgError("Argument getlen should equal or greater than 0.")
                #
            else:
                raise error.ArgError("Argument getlen should a python int/long.")
            #Read from memory
            index = 0
            while True:
                init_arg = internal.datatype.DataTypeObjectInitArg("(no name)", getaddr + index, None, True)
                ucharobj = internal.datatype.DataTypeNew(uchar, init_arg)
                uchardata = ucharobj._GetValue()
                if uchardata == 0:
                    #End on 0
                    break
                else:
                    #Save to return string
                    return_str += __builtin__.chr(uchardata)
                    index += 1
                    if getlen != None and index >= getlen:
                        #End with length
                        break
                    #
                #
            return return_str
        @staticmethod
        def set(setaddr, setstr, setlen = None):
            #Argument check
            if __builtin__.type(setaddr) == __builtin__.int or __builtin__.type(setaddr) == __builtin__.long:
                pass
            else:
                raise error.ArgError("Argument setaddr should a python int/long.")
            #
            if not __builtin__.type(setstr) == __builtin__.str:
                raise error.ArgError("Argument setstr should a python str.")
            #
            if setlen == None:
                pass
            elif __builtin__.type(setlen) == __builtin__.int or __builtin__.type(setlen) == __builtin__.long:
                if setlen < 0:
                    raise error.ArgError("Argument setlen should equal or greater than 0.")
                #
            else:
                raise error.ArgError("Argument setlen should a python int/long.")
            #Write to memory
            if __builtin__.len(setstr) == 0:
                return None
            for item in __builtin__.range(__builtin__.len(setstr) + 1):
                if setlen != None and item >= setlen:
                    #End with length
                    break
                if item == __builtin__.len(setstr):
                    uchardata = 0
                else:
                    strloop = setstr[item]
                    uchardata = __builtin__.ord(strloop)
                init_arg = internal.datatype.DataTypeObjectInitArg("(no name)", setaddr + item, None, True)
                ucharobj = internal.datatype.DataTypeNew(uchar, init_arg)
                ucharobj._SetValue(uchardata)
            return None

'''
Target information
'''
class target(object):
    class endian(object):
        @staticmethod
        def isbig():
            info = UterExtern.TargetInfo(nouse = None)
            return info["bigend"]
        @staticmethod
        def islittle():
            info = UterExtern.TargetInfo(nouse = None)
            return not info["bigend"]
        @staticmethod
        def cmd_show():
            print "======================================"
            if target.endian.isbig():
                print "Target is big-endian"
            else:
                print "Target is little-endian"
            print "======================================"
            return None
        @staticmethod
        def cmd_help():
            print "======================================"
            print "uter.target.endian.isbig()                  #Return target is big-endian with python bool"
            print "uter.target.endian.islittle()               #Return target is little-endian with python bool"
            print "uter.target.endian.cmd_show()      #Command: Show target endian"
            print "uter.target.endian.cmd_help()        #Command: Target endian help information"
            print "======================================"
            return None
    '''
    Target function stub management
    '''
    class stub(object):
        @staticmethod
        def list():
            return UterExtern.StubAdmin(opt = "list")
        @staticmethod
        def clear(addr = 0):
            if not (__builtin__.type(addr) == __builtin__.int or __builtin__.type(addr) == __builtin__.long):
                raise error.ArgError("Argument addr should python int/long.")
            #Delete stub
            UterExtern.StubAdmin(opt = "clean", addr = __builtin__.long(addr))
            return None
        #Show list to console
        @staticmethod
        def cmd_show():
            stub_list = UterExtern.StubAdmin(opt = "list")
            print "======================================"
            if __builtin__.len(stub_list) == 0:
                print "No any stub in target."
            else:
                for item in __builtin__.range(__builtin__.len(stub_list)):
                    stubloop = stub_list[item]
                    addr = stubloop["address"]
                    arg = stubloop["arg"]
                    print "Function(C/C++) %s address:%s:" % (arg["name"], internal.utils._AddrStr(addr))
                    print "    Mod:%s, Cu:%s" % (arg["mod"], arg["cu"])
                    print "    Script stub(Python):%s" % arg["stub"].__str__()
                #
            print "======================================"
            return None
        @staticmethod
        def cmd_help():
            print "======================================"
            print "uter.target.stub.list()                     #Return stub information in target"
            print "uter.target.stub.clear()                 #Clear stub in target"
            print "uter.target.stub.cmd_show()     #Command: Show stub in target"
            print "uter.target.stub.cmd_help()       #Command: Target stub help information"
            print "======================================"
            return None
    
    class mirror(object):        
        class range(object):
            def __init__(self, mod_path = None, mod_regex = None, cu_path = None, cu_regex = None):
                #Get module list
                mod_list = UterExtern.Modules(nouse = None)
                self.mod_dict = {}
                
                #Module 
                if mod_path != None and mod_regex != None:
                    raise error.ArgError("Can not support both mod_path and mod_regex.")
                    return None
                elif mod_path != None:
                    #Check input
                    if __builtin__.type(mod_path) != __builtin__.str:
                        raise error.ArgError("Argument mod_path should python str.")
                    if __builtin__.len(mod_path) == 0:
                        raise error.ArgError("Argument mod_path should not be empty.")
                    #Filte modules by module path
                    for item in __builtin__.range(__builtin__.len(mod_list)):
                        modloop = mod_list[item]
                        if mod_path[0] == '/':     #Absolute path
                            if mod_path == modloop:
                                self.mod_dict[modloop] = {}
                            #
                        else:                                   #Relative path
                            if __builtin__.len(mod_path) <= __builtin__.len(modloop) and mod_path == modloop[- __builtin__.len(mod_path):]:
                                self.mod_dict[modloop] = {}
                            #
                        #
                    #
                elif mod_regex != None:
                    if __builtin__.type(mod_regex) != __builtin__.str:
                        raise error.ArgError("Argument mod_regex should python str.")
                    if __builtin__.len(mod_regex) == 0:
                        raise error.ArgError("Argument mod_regex should not be empty.")
                    #Regex compile
                    try:
                        mod_pattern = re.compile(mod_regex)
                    except:
                        raise error.ArgError("Invalid module regex pattern.")
                    #Filte modules by module regex
                    for item in __builtin__.range(__builtin__.len(mod_list)):
                        modloop = mod_list[item]
                        match = mod_pattern.match(modloop)
                        if match != None:
                            self.mod_dict[modloop] = {}
                        #
                    #
                else:
                    for item in __builtin__.range(__builtin__.len(mod_list)):
                        modloop = mod_list[item]
                        self.mod_dict[modloop] = {}
                    #
                #
                
                #Cu
                if cu_path != None and cu_regex != None:
                    raise error.ArgError("Can not support both cu_path and cu_regex.")
                    return None
                elif cu_path != None:
                    if __builtin__.type(cu_path) != __builtin__.str:
                         raise error.ArgError("Argument cu_path should python str.")
                    if __builtin__.len(cu_path) == 0:
                        raise error.ArgError("Argument cu_path should not be empty.")
                    for mod_item in self.mod_dict.keys():
                        cu_list = UterExtern.CompileUnits(mod = mod_item)
                        for cu_item in __builtin__.range(__builtin__.len(cu_list)):
                            culoop = cu_list[cu_item]
                            if cu_path[0] == '/':           #Absolute path
                                if cu_path == culoop:
                                    self.mod_dict[mod_item][culoop] = None
                                #
                            else:                                       #Relative path
                                if __builtin__.len(cu_path) <= __builtin__.len(culoop) and cu_path == culoop[- __builtin__.len(cu_path):]:
                                    self.mod_dict[mod_item][culoop] = None
                                #
                            #
                        #
                        if __builtin__.len(self.mod_dict[mod_item]) == 0:
                            del self.mod_dict[mod_item]
                    #
                elif cu_regex != None:
                    if __builtin__.type(cu_regex) != __builtin__.str:
                        raise error.ArgError("Argument cu_regex should python str.")
                    if __builtin__.len(cu_regex) == 0:
                        raise error.ArgError("Argument cu_regex should not be empty.")
                    #Regex compile
                    try:
                        cu_pattern = re.compile(cu_regex)
                    except:
                        raise error.ArgError("Invalid cu regex pattern.")
                    for mod_item in self.mod_dict.keys():
                        cu_list = UterExtern.CompileUnits(mod = mod_item)
                        for cu_item in __builtin__.range(__builtin__.len(cu_list)):
                            culoop = cu_list[cu_item]
                            match = cu_pattern.match(culoop)
                            if match != None:
                                self.mod_dict[mod_item][culoop] = None
                            #
                        #
                        if __builtin__.len(self.mod_dict[mod_item]) == 0:
                            del self.mod_dict[mod_item]
                        #
                    #   
                else:
                    for mod_item in self.mod_dict.keys():
                        cu_list = UterExtern.CompileUnits(mod = mod_item)
                        for cu_item in __builtin__.range(__builtin__.len(cu_list)):
                            culoop = cu_list[cu_item]
                            self.mod_dict[mod_item][culoop] = None
                        #
                    #
                #
                return None
            def mods(self):
                return self.mod_dict.keys()
            def cus(self):
                return_list = []
                for mod_item in self.mod_dict.keys():
                    for cu_item in self.mod_dict[mod_item].keys():
                        return_list += [{"mod": mod_item,"cu": cu_item}]
                    #
                #
                return return_list;
            def __str__(self):
                return_str = __builtin__.type.__str__(self) + "\n"
                for mod_item in self.mod_dict.keys():
                    return_str += "Mod: %s\n" % mod_item
                    for cu_item in self.mod_dict[mod_item].keys():
                        return_str += "    Cu:%s\n" % cu_item
                    #
                #
                return return_str
        @staticmethod
        def cmd_mod_show():
            range = target.mirror.range()
            print "======================================"
            for mod_item in range.mod_dict.keys():
                print mod_item
            print "======================================"
            return None
        @staticmethod
        def cmd_cu_show():
            print "======================================"
            range = target.mirror.range()
            for mod_item in range.mod_dict.keys():
                print "Mod:" + mod_item
                for cu_item in range.mod_dict[mod_item].keys():
                    print "        Cu:" + cu_item
                #
            #
            print "======================================"
            return None
        @staticmethod
        def cmd_file_show():
            print "======================================"
            range = target.mirror.range()
            for mod_item in range.mod_dict.keys():
                print "Mod:" + mod_item
                for cu_item in range.mod_dict[mod_item].keys():
                    print "        Cu:" + cu_item
                    file_dict = UterExtern.Files(mod = mod_item, cu = cu_item)
                    for file_item in file_dict.keys():
                        print "                File %d:%s" % (file_item, file_dict[file_item])
                    #
                #
            # 
            print "======================================"
            return None
        @staticmethod 
        def cmd_help():
            print "======================================"
            print "uter.range()                                                           #Same as uter.target.mirror.range()"
            print "uter.target.mirror.range()                                #Return a uter module(s) and compilation unit(s) range object"
            print "uter.target.mirror.cmd_mod_show()           #Show all module"
            print "uter.target.mirror.cmd_cu_show()                #Show all compilation unit"
            print "uter.target.mirror.cmd_file_show()               #Show all compilation file"
            print "uter.target.mirror.cmd_help()                        #Target mirror help information"
            print "======================================"
            return None

range = target.mirror.range

'''
Project information and configuration
'''
class project(object):
    class case_dir(object):
        @staticmethod 
        def list():
            prjinfo = UterExtern.ProjectInfo(nouse = None)
            return prjinfo["casedir"]
        @staticmethod
        def cmd_show():
            dirs = project.case_dir.list()
            print "======================================"
            if __builtin__.len(dirs) == 0:
                print "No any case directory."
            else:
                print "Case directories in project:"
                for dir in dirs:
                    print dir
            print "======================================"
            return None
        @staticmethod
        def cmd_help():
            print "======================================"
            print "uter.project.case_dir.list()                    #Return project test case directories with python list"
            print "uter.project.case_dir.cmd_show()     #Command: Show project test case directories"
            print "uter.project.case_dir.cmd_help()       #Command: Project test case help information"
            print "======================================"
            return None

class run(object):
    class internal(object):
        #For load python module
        load_desc = ('.py', 'U', 1)
    
        #Convert path to module name
        @staticmethod 
        def path2modname(path):
            case_mod_name = "TC:/" + os.path.abspath(path)
            case_mod_name = case_mod_name.replace(os.path.extsep, "@")
            return case_mod_name
        #
    @staticmethod 
    def file(file_path):
        print "####################################################################"
        print "Test case file " + file_path
        print "####################################################################"
        #Check
        if not os.path.isfile(file_path):
            print file_path + "is not a file."
            return
        if not file_path[-3:] == ".py":
            print path + "is not a python file."
            return
        #Load
        case_mod_name = run.internal.path2modname(file_path)
        fp = open(file_path, "U")
        mod = imp.load_module(case_mod_name, fp, file_path, run.internal.load_desc)
        suite = unittest.TestLoader().loadTestsFromModule(mod)
        #Execute
        alltests = unittest.TestSuite((suite,))
        unittest.TextTestRunner(verbosity=2).run(alltests)
        #Release
        if case_mod_name in sys.modules:
            del(sys.modules[case_mod_name])
        #
        print ""
        print "####################################################################"
        print "End"
        print "####################################################################"
        return
    
    @staticmethod 
    def dir(dir_path):
        print "####################################################################"
        print "Test case directory " + dir_path
        print "####################################################################"
        
        casefiles = []
        if dir_path == "":
            #Scan all case directory
            for case_dir in project.case_dir.list():
                #Scan all python file in this directory
                for onefile in os.listdir(case_dir):
                    file_path = os.path.join(case_dir, onefile)
                    if not os.path.isfile(file_path):
                        continue
                    if not file_path[-3:] == ".py":
                        continue
                    casefiles += [file_path]
                #
            #
        else:
            #Check
            if not os.path.isdir(dir_path):
                print dir_path + " is not a directory."
                return
            #Scan all python file in the directory
            for onefile in os.listdir(dir_path):
                file_path = os.path.join(dir_path, onefile)
                if not os.path.isfile(file_path):
                    continue
                if not file_path[-3:] == ".py":
                    continue
                casefiles += [file_path]
            #
        #Result
        if __builtin__.len(casefiles) == 0:
            print "No any case file in directory " + dir_path
            return
        else:
            print "Test case file in the list:"
            for file_path in casefiles:
                print file_path + ","
            #
        #Load
        mod_name_list = []
        test_suite_tuple = ()
        for file_path in casefiles:
            case_mod_name = run.internal.path2modname(file_path)        
            fp = open(file_path, "U")
            mod = imp.load_module(case_mod_name, fp, file_path, run.internal.load_desc)
            suite = unittest.TestLoader().loadTestsFromModule(mod)
            mod_name_list += [case_mod_name]
            test_suite_tuple += (suite,)
        #Run
        alltests = unittest.TestSuite(test_suite_tuple)
        unittest.TextTestRunner(verbosity=2).run(alltests)
        #Release
        for case_mod_name in mod_name_list:
            if case_mod_name in sys.modules:
                del(sys.modules[case_mod_name])
            #
        #
        print ""
        print "####################################################################"
        print "End"
        print "####################################################################"
        return
    
    '''
    Use for Console
    '''
    @staticmethod 
    def cmd_run_case(input_name = ""):
        #Check
        if __builtin__.type(input_name) != __builtin__.str:
            raise "Arguement should be python str."
        #Scan all case directory
        casefiles = []
        for case_dir in project.case_dir.list():
            if input_name == "":
                #Scan this case directory
                for onefile in os.listdir(case_dir):
                    file_path = os.path.join(case_dir, onefile)
                    if not os.path.isfile(file_path):
                        continue
                    if not file_path[-3:] == ".py":
                        continue
                    casefiles += [file_path]
                #
            else:
                #Check file in this directory
                if input_name[-3:] == ".py":
                    file_path = os.path.join(case_dir, input_name)
                else:
                    file_path = os.path.join(case_dir, input_name + ".py")
                #
                if not os.path.exists(file_path):
                    continue;
                if not os.path.isfile(file_path):
                    continue;
                casefiles += [file_path]
            #
        #Check scan result
        if __builtin__.len(casefiles) == 0:
            print "No any case file matched."
            return
        else:
            print "Match test case file in the list:"
            for file_path in casefiles:
                print file_path + ","
            #
        #Load
        mod_name_list = []
        test_suite_tuple = ()
        for file_path in casefiles:
            case_mod_name = run.internal.path2modname(file_path)
            fp = open(file_path, "U")
            mod = imp.load_module(case_mod_name, fp, file_path, run.internal.load_desc)
            suite = unittest.TestLoader().loadTestsFromModule(mod)
            test_suite_tuple += (suite,)
            mod_name_list += [case_mod_name]
        #Run
        alltests = unittest.TestSuite(test_suite_tuple)
        unittest.TextTestRunner(verbosity=2).run(alltests)
        #Release
        for case_mod_name in mod_name_list:
            if case_mod_name in sys.modules:
                del(sys.modules[case_mod_name])
            #
        #
        print  __builtin__.str(len(test_suite_tuple)) + "  case file matched."
        
    @staticmethod 
    def cmd_help():
        print "======================================"
        print "uter.run.cmd_run_case()        #Run test case"
        print "uter.run.cmd_help()                 #Uter run help"
        print "======================================" 