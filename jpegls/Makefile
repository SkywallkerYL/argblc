BUILD_DIR = ./build

export PATH := $(PATH):$(abspath ./utils)

milltest:
	mill -i __.test

verilog:
	mkdir -p $(BUILD_DIR)
	mill -i __.test.runMain decoder.Elaborate -td $(BUILD_DIR)

help:
	mill -i __.test.runMain Elaborate --help

compile:
	mill -i __.compile

bsp:
	mill -i mill.bsp.BSP/install

reformat:
	mill -i __.reformat

checkformat:
	mill -i __.checkFormat


.PHONY: test verilog help compile bsp reformat checkformat clean

sim:
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	@echo "Write this Makefile by yourself."

#include ../Makefile
# Generate Verilog code sbt
doit:
	sbt run
#Makefile for Verilator

#rsdecodertop Decoder2Col rsdecodertopGauss rsdecoder2colGauss
#jpeglsencodesimtop
TOP?=jpeglsencodesimtop
TOPNAME?=$(TOP)
TOPNAMETEST?=$(TOP)
#FSM_m #keyboard_top #top
NXDC_FILES=constr/top.nxdc
INC_PATH?=

#include ../../include

#add ccache speed up
VERILATOR = verilator 
# Generate C++ in executable form
#for nvboard
VERILATOR_FLAGS += -MMD --build -cc
VERILATOR_FLAGS += -Os --x-assign fast
VERILATOR_FLAGS += --x-initial fast --noassert
VERILATOR_FLAGS += -Wno-fatal
#for testbench
VERILATOR_CFLAGS += -cc --exe --noassert
#-Os -x-assign 0 --trace --coverage
VERILATOR_CFLAGS +=	--x-assign fast --x-initial fast 
#VERILATOR_CFLAGS +=  --trace 
#--coverage 
VERILATOR_CFLAGS +=  --build 
VERILATOR_CFLAGS += -Wno-fatal -O3 
VERILATOR_CFLAGS += -DVL_DEBUG
VERILATOR_CFLAGS += -j 8
VERILATOR_CFLAGS += --no-timing
VERILATOR_CFLAGS += --top-module $(TOPNAMETEST)
#VERILATOR_CFLAGS += --x-initial fast --noassert
#VERILATOR_CFLAGS += --timing



CSRCT = $(shell find $(abspath ./csrc) -name "*.c" -or -name "main.cpp" -or -name "*.cc" ) 
#addertop.cpp alu_tb.cpp main.cpp

#BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj_dir
BIN = $(BUILD_DIR)/$(TOPNAMETEST)

#HSRC = $($(NVBOARD_HOME)/include/    -name "*.h")
 
default: $(BIN)

$(shell mkdir -p $(BUILD_DIR))

#XDC
SRC_AUTO_BIND = $(abspath $(BUILD_DIR)/auto_bind.cpp)
$(SRC_AUTO_BIND): $(NXDC_FILES)
	python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $^ $@
	
VSRC = $(shell find $(abspath ./build) -name "*.v")
VSRC += $(shell find $(abspath ./vsrc ) -name "*.v")
CSRC = $(shell find $(abspath ./csrc) -name "*.c" -or -name "main.cpp" -or -name "*.cc" )
CSRC += $(SRC_AUTO_BIND)
#VSRC += $(shell find $(abspath ./generated) -name "*.v")
#VSRC += $(shell find $(abspath ../templete/Mux) -name "*.v")
#VSRC += $(shell find $(abspath ../templete/bcd7seg) -name "*.v")

#rules for nvboard
#include $(NVBOARD_HOME)/scripts/nvboard.mk
#rules for verilator
INCFLAGS = $(addprefix -I, $(INC_PATH))
CFLAGS += -w
CFLAGS += $(INCFLAGS) -DTOP_NAME="\"V$(TOPNAME)\""
VCFLAGS += $(INCFLAGS) -DTOP_NAME="\"V$(TOPNAME)\""
#CFLAGS += -I/usr/lib/llvm-14/include -std=c++14   
#CFLAGS += -fno-exceptions -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS 
#CFLAGS += -D__STDC_LIMIT_MACROS
#CFLAGS += -fPIE -g

CFLAGS += $(shell llvm-config --cxxflags) -fPIE -g 
LDFLAGS += -lSDL2 -lSDL2_image 
LDFLAGS += $(shell llvm-config --libs)
LDFLAGS += -lreadline
CFLAGS_BUILD ?=
#CFLAGS_BUILD += $(if $(CONFIG_CC_DEBUG),-Og -ggdb3		,)
#CFLAGS_BUILD += $(if $(CONFIG_CC_ASAN) ,-fsanitize=address,)
CFLAGS_BUILD += -Og -ggdb3			
CFLAGS_BUILD += -fsanitize=address	
#CFLAGS += $(CFLAGS_BUILD)
#LDFLAGS+= $(CFLAGS_BUILD)
IMG?=

	
$(BIN) : $(VSRC) $(CSRCT) 
	@rm -rf $(OBJ_DIR)
	$(VERILATOR) $(VERILATOR_CFLAGS) \
		--top-module $(TOPNAME) $^ \
		$(addprefix -CFLAGS , $(CFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS))\
		--Mdir $(OBJ_DIR) --exe -o $(abspath $(BIN))

all: default




runnvboard: $(BIN)
	#for nvboard
	@$^ $(BATCHMODE) $(NPCMODE) $(IMG)
run: 
	#for testbench        
	@echo "---------------VERILATE------------------"
#发现了一个问题hhh -cflags只有在编译cpp的时候才需要
#不要向原来一样把vsrc和csrc放在一起编译，不然会有redefine的问题，而且编译极其缓慢
#还是不幸
#$(VERILATOR) $(VERILATOR_CFLAGS) $(VSRC) 
#$(VERILATOR) $(VERILATOR_CFLAGS) $(CSRCT) $(addprefix -CFLAGS , $(CFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS))
	$(VERILATOR) $(VERILATOR_CFLAGS) $(CSRCT) $(VSRC) $(addprefix -CFLAGS , $(CFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS))
#$(VERILATOR) $(VERILATOR_CFLAGS) $(VSRC) $(addprefix -CFLAGS , $(VCFLAGS))
#$(VERILATOR) $(VERILATOR_CFLAGS) $(CSRCT) $(addprefix -CFLAGS , $(CFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS))
	

	@echo "-----------------BUILD-------------------"

	$(MAKE) -j -C obj_dir OPT_FAST="-Os -march=native" -f V$(TOPNAMETEST).mk V$(TOPNAMETEST) 
# $(MAKE) -j -C obj_dir -f ../Makefile_obj

	@echo "-------------------RUN-------------------"
	./obj_dir/V$(TOPNAMETEST)

#gtkwave wave.vcd
wave:
	gtkwave wave.vcd
#perf script -i perf.data &> perf.unfold
#sudo ~/FlameGraph/stackcollapse-perf.pl perf.unfold &> perf.folded
#sudo ~/FlameGraph/flamegraph.pl perf.folded > perf.svg
#
show-config:
	$(VERILATOR) -V
clean:
	-rm -rf obj_dir logs *.log *.dmp *.vpd *.vcd $(BUILD_DIR)  perf.data perf.data.old perf.unfold perf.folded perf.svg
#git clean -fd
.PHONY: default all clean run
