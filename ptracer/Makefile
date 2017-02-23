
ARCH =  
CPP = $(ARCH)g++ 
CC = $(ARCH)gcc 
LD = $(ARCH)ld
AS = $(ARCH)as
AR = $(ARCH)ar

#Just support make on project directory
APPDIR= $(PWD)/

#Directory (source)
PTRACER_SRC_DIR = $(APPDIR)ptracer/

#Directory (destination)
BUILD_DIR = $(APPDIR)build/
ifeq (${DEBUG}, 1)
	OUTPUT_DIR = $(BUILD_DIR)debug/
else
	OUTPUT_DIR = $(BUILD_DIR)release/
endif
PACK_DIR = $(OUTPUT_DIR)uter/
BIN_DIR = $(PACK_DIR)bin/
PTRACER_CPP_OBJ_DIR = $(OUTPUT_DIR)ptracer_obj/

PTRACER_CPP_SRC = $(sort $(shell find $(PTRACER_SRC_DIR) -name '*.cpp'))
PTRACER_CPP_OBJ = $(patsubst $(PTRACER_SRC_DIR)%,$(PTRACER_CPP_OBJ_DIR)%,$(sort $(PTRACER_CPP_SRC:%.cpp=%.o)))
PTRACER_CPP_DEP = $(sort $(PTRACER_CPP_OBJ:%.o=%.d) )

PTRACER_INCLUDE_FLAG +=  -I$(PTRACER_SRC_DIR)

#Cpp flags
ifeq (${DEBUG}, 1)
	CPP_FLAGS = -g3
else
	CPP_FLAGS =
endif

#Ptracer static library (work)
PTRACER_S_LIB = 
#Ptracer dynamic library
PTRACER_D_LIB +=  -lpthread -luuid -lc -lstdc++ -ldl

#Dependence for ptracer
$(PTRACER_CPP_DEP):$(PTRACER_CPP_OBJ_DIR)%.d:$(PTRACER_SRC_DIR)%.cpp
	@mkdir -p $(dir $@)
	set -e; \
	$(CPP) -M $(CPP_FLAGS) $(PTRACER_INCLUDE_FLAG) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,$(PTRACER_CPP_OBJ_DIR)\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
-include $(PTRACER_CPP_DEP)

#Object for ptracer
$(PTRACER_CPP_OBJ):$(PTRACER_CPP_OBJ_DIR)%.o:$(PTRACER_SRC_DIR)%.cpp
	@mkdir -p $(dir $@)
	$(CPP) $(CPP_FLAGS) $(PTRACER_INCLUDE_FLAG) \
	    -c $< -o $@  
	    
#Ptracer i386
PTRACER_I386 = $(BIN_DIR)ptracer_i386.exe
$(PTRACER_I386): $(PTRACER_CPP_OBJ)
	@mkdir -p $(dir $@)
	$(CPP) -Wl,--start-group $(PTRACER_CPP_OBJ) -Wl,--end-group \
	   -Wl,-Bstatic $(PTRACER_S_LIB) -Wl,-Bdynamic $(PTRACER_D_LIB) -o $@

#Prepair
.PHONY:Prepair
Prepair:
	@echo "Make ptracer  prepair................................"
	@echo $(PTRACER_I386)

PTRACER = $(PTRACER_I386)
.PHONY:all
all: Prepair $(PTRACER)