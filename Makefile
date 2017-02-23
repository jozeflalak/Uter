#About toolchain 
ARCH =  
CPP = $(ARCH)g++ 
CC = $(ARCH)gcc 
LD = $(ARCH)ld
AS = $(ARCH)as
AR = $(ARCH)ar

#TODO
#make DEBUG=1
DEBUG = 1

#Just support make on project directory
APPDIR= $(PWD)/

#About Qt toolchain
QT = $(APPDIR)third_party_build/Qt5.5.0/
MOC = $(QT)/bin/moc
RCC = $(QT)/bin/rcc

#Directory (source)
SUPER_SRC_DIR = $(APPDIR)superviser/
PTRACER_SRC_DIR = $(APPDIR)ptracer/
EXAMPLE_SRC_DIR = $(APPDIR)examples/

#Directory (destination) 
BUILD_DIR = $(APPDIR)build/
ifeq (${DEBUG}, 1)
	OUTPUT_DIR = $(BUILD_DIR)debug/
else
	OUTPUT_DIR = $(BUILD_DIR)release/
endif
PACK_DIR = $(OUTPUT_DIR)uter/
BIN_DIR = $(PACK_DIR)bin/
LIB_DIR = $(PACK_DIR)lib/
SCRIPT_DIR = $(PACK_DIR)python/
DOC_DIR = $(PACK_DIR)document/

#Superviser include (elf dwarf binutils python)
SUPER_INCLUDE_FLAG +=  \
                                    -I$(APPDIR)third_party_build/libelf-0.8.13/include/libelf/  \
                                    -I$(APPDIR)third_party_build/libdwarf_20150604/include/libdwarf/  \
                                    -I$(APPDIR)third_party_build/binutils-2.25_x86/include/ \
                                    -I$(APPDIR)third_party_build/Python-2.7.9/include/python2.7/ 
#Superviser include (Qt)
SUPER_INCLUDE_FLAG += \
                                    -I$(APPDIR)third_party_build/Qt5.5.0/include/ \
                                    -I$(APPDIR)third_party_build/Qt5.5.0/include/QtCore/ \
                                    -I$(APPDIR)third_party_build/Qt5.5.0/include/QtWidgets/ \
                                    -I$(APPDIR)third_party_build/Qt5.5.0/include/Qsci/ \
                                    -I$(APPDIR)third_party_build/Qt5.5.0/include/QtGui/ 
#Superviser include (Self)
SUPER_H_INCH=$(shell find $(SUPER_SRC_DIR) -name "*.h")
SUPER_HXX_INCH=$(shell find $(SUPER_SRC_DIR) -name "*.hxx")
SUPER_INCH = $(SUPER_H_INCH) $(SUPER_HXX_INCH)
SUPER_ALL_INCDIR=$(sort $(dir $(SUPER_INCH)))
SUPER_INCLUDE_FLAG += $(foreach temp, $(SUPER_ALL_INCDIR), -I$(temp))
#Superviser include (Ptracer)
SUPER_INCLUDE_FLAG += -I$(PTRACER_SRC_DIR)

#Cpp flags
ifeq (${DEBUG}, 1)
	CPP_FLAGS = -g3
else
	CPP_FLAGS = 
endif
CPP_FLAGS += -DQT_BOOTSTRAPPED

#Superviser static library (work)
SUPER_S_LIB +=  -ldwarf -lelf -lopcodes -lbfd -liberty -lutil
#Superviser dynamic library (ui)
SUPER_D_LIB +=  -lQt5Widgets -lQt5PrintSupport -lQt5Gui -lQt5Core -lGL -lQt5Network -lQt5Xml -lqscintilla2
UI_DEP_LIB += libicui18n.so.54 libicuuc.so.54 libicudata.so.54
SUPER_D_LIB += $(addprefix $(APPDIR)third_party_build/Qt5.5.0/lib/, $(UI_DEP_LIB))
#Superviser dynamic library (others)
SUPER_D_LIB += -lpython2.7 -ldl

#Superviser link flags (runtime)
SUPER_LINK_FLAG += -Wl,-rpath,'$$ORIGIN/../lib/' \
                                                  -Wl,-rpath,'$$ORIGIN/../lib/Python-2.7.9/lib/' \
                                                  -Wl,-rpath,'$$ORIGIN/../lib/Qt5.5.0/lib/' 
#Superviser link flags (linktime)
SUPER_LINK_FLAG += -L$(APPDIR)third_party_build/libelf-0.8.13/lib/  \
                                                -L$(APPDIR)third_party_build/libdwarf_20150604/lib/ \
                                               -L$(APPDIR)third_party_build/binutils-2.25_x86/lib \
                                               -L$(APPDIR)third_party_build/Python-2.7.9/lib \
                                               -L$(APPDIR)third_party_build/Python-2.7.9/lib/python2.7/config/ \
                                               -L$(APPDIR)third_party_build/Qt5.5.0/lib/ 

#Resource for superviser
QRC_DIR = $(APPDIR)resource/
QRC_FILE = $(QRC_DIR)/uter.qrc
RESOURCE = $(BIN_DIR)Uter.rcc
$(RESOURCE):$(QRC_FILE)
	@mkdir -p $(dir $@)
	$(RCC) -binary $(QRC_FILE) -o $@

#Superviser source/object/dependence (cpp)
SUPER_CPP_OBJ_DIR = $(OUTPUT_DIR)cpp_obj/
SUPER_CPP_SRC=$(sort $(shell find $(SUPER_SRC_DIR) -name '*.cpp'))
SUPER_CPP_OBJ=$(patsubst $(SUPER_SRC_DIR)%,$(SUPER_CPP_OBJ_DIR)%,$(sort $(SUPER_CPP_SRC:%.cpp=%.o)))
SUPER_CPP_DEP = $(sort $(SUPER_CPP_OBJ:%.o=%.d) )
#Superviser source/object/dependence (moc)
SUPER_MOC_OBJ_DIR = $(OUTPUT_DIR)moc_obj/
SUPER_MOC_OBJ=$(patsubst $(SUPER_SRC_DIR)%,$(SUPER_MOC_OBJ_DIR)%,$(sort $(SUPER_HXX_INCH:%.hxx=%.o)))
SUPER_MOC_DEP = $(sort $(SUPER_MOC_OBJ:%.o=%.d) )
SUPER_MOC_SRC=$(sort $(SUPER_MOC_OBJ:%.o=%.cpp))
	
#Superviser moc
$(SUPER_MOC_SRC):$(SUPER_MOC_OBJ_DIR)%.cpp:$(SUPER_SRC_DIR)%.hxx
	@mkdir -p $(dir $@)
	$(MOC) $< -o $@
	
#Dependence for superviser (cpp)
$(SUPER_CPP_DEP):$(SUPER_CPP_OBJ_DIR)%.d:$(SUPER_SRC_DIR)%.cpp
	@mkdir -p $(dir $@)
	set -e; \
	$(CPP) -M $(CPP_FLAGS) $(SUPER_INCLUDE_FLAG) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,$(SUPER_CPP_OBJ_DIR)\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
-include $(SUPER_CPP_DEP)
#Dependence for superviser (moc)
$(SUPER_MOC_DEP):$(SUPER_MOC_OBJ_DIR)%.d:$(SUPER_MOC_OBJ_DIR)%.cpp
	@mkdir -p $(dir $@)
	set -e; \
	$(CPP) -M $(CPP_FLAGS) $(SUPER_INCLUDE_FLAG) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,$(SUPER_MOC_OBJ_DIR)\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
-include $(SUPER_MOC_DEP)
	
#Object for superviser (cpp)
$(SUPER_CPP_OBJ):$(SUPER_CPP_OBJ_DIR)%.o:$(SUPER_SRC_DIR)%.cpp
	@mkdir -p $(dir $@)
	$(CPP) $(CPP_FLAGS) $(SUPER_INCLUDE_FLAG) \
	    -c $< -o $@  
#Object for superviser (moc)
$(SUPER_MOC_OBJ):$(SUPER_MOC_OBJ_DIR)%.o:$(SUPER_MOC_OBJ_DIR)%.cpp
	@mkdir -p $(dir $@)
	$(CPP) $(CPP_FLAGS) $(SUPER_INCLUDE_FLAG) \
	    -c $< -o $@  

#Superviser
SUPER = $(BIN_DIR)Uter.exe
$(SUPER) :$(SUPER_CPP_OBJ) $(SUPER_MOC_OBJ)
	@mkdir -p $(dir $@)
	$(CPP) $(CPP_FLAGS) -Wl,--start-group $(SUPER_CPP_OBJ) $(SUPER_MOC_OBJ) -Wl,--end-group \
	             -Wl,-Bstatic $(SUPER_S_LIB)  -Wl,-Bdynamic $(SUPER_D_LIB) $(SUPER_LINK_FLAG) -o $@ 
	readelf -d $@

#Install library
.PHONY:Lib_install
Lib_install:
	@mkdir -p $(LIB_DIR)/Qt5.5.0/lib/
	@rsync -a --delete $(APPDIR)third_party_build/Qt5.5.0/lib/  $(LIB_DIR)/Qt5.5.0/lib/
	@mkdir -p $(LIB_DIR)/Qt5.5.0/plugins/
	@rsync -a --delete $(APPDIR)third_party_build/Qt5.5.0/plugins/  $(LIB_DIR)/Qt5.5.0/plugins/
	@mkdir -p $(LIB_DIR)/Python-2.7.9/bin/
	@rsync -a --delete $(APPDIR)third_party_build/Python-2.7.9/bin/  $(LIB_DIR)/Python-2.7.9/bin/
	@mkdir -p $(LIB_DIR)/Python-2.7.9/lib/
	@rsync -a --delete $(APPDIR)third_party_build/Python-2.7.9/lib/  $(LIB_DIR)/Python-2.7.9/lib/
	@mkdir -p $(SCRIPT_DIR)
	@rsync -a --delete $(APPDIR)python/  $(SCRIPT_DIR)
	
	@mkdir -p $(DOC_DIR)
	@cp -f $(APPDIR)/document/userguide.pdf $(DOC_DIR)
	
	cp -f $(APPDIR)packer/setup.sh $(PACK_DIR)
	cp -f $(APPDIR)packer/packer.sh  $(OUTPUT_DIR)packer.sh
	chmod 755 $(OUTPUT_DIR)packer.sh
	
#Ptracer
.PHONY:Ptracer
Ptracer:
	@echo "Make ptracer....................................."
	make -C $(PTRACER_SRC_DIR) all DEBUG=$(DEBUG)

#Examples
.PHONY:Examples
Examples:
	@echo "Make examples................................"
	make -C $(EXAMPLE_SRC_DIR) 
	
#Prepair
.PHONY:Prepair
Prepair:
	@echo "Make superviser  prepair................................"

#Make all
.PHONY:all
all:Prepair Examples Ptracer $(SUPER) $(RESOURCE) Lib_install

#Do clear, do not use $(xxxx) or $(xxxx)/ for safe.
.PHONY:clean
clean:
	rm -rf  $(BUILD_DIR)/release/
	rm -rf  $(BUILD_DIR)/debug/
	