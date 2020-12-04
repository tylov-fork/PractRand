# This makefile will work for GCC on both Linux and Windows.
# It builds out-of-source, into build/Windows or build/Linux,
# and provides dependecy checks.
#
# Tested on Windows with TDM-GCC64 9.2.0 - built executables requires no external DLLs.
# It includes ming32-make.exe This makefile also need mkdir and rm, which are found in
# C:\Program Files\Git\usr\bin if Git is installed. Use make -j or mingw32-make -j for
# fast parallel build.

ifeq ($(OS),Windows_NT)
	EXE   := .exe
	SYS   := build/Windows
else
	SYS   := build/$(shell uname)
endif

OBJ_DIR   := $(SYS)/obj
BIN_DIR   := $(SYS)/bin

PRAND     := PractRand
LIB_SRCS  := $(wildcard src/*.cpp) $(wildcard src/RNGs/*.cpp) $(wildcard src/RNGs/other/*.cpp)
LIB_OBJS  := $(LIB_SRCS:%.cpp=$(OBJ_DIR)/%.o)
LIB_DEPS  := $(LIB_SRCS:%.cpp=$(OBJ_DIR)/%.d)
PRAND_LIB := $(OBJ_DIR)/lib$(PRAND).a

PNAMES    := RNG_test RNG_benchmark RNG_output
PROGRAMS  := $(PNAMES:%=$(BIN_DIR)/%$(EXE))
PROG_SRCS := $(PNAMES:%=tools/%.cpp)
PROG_OBJS := $(PROG_SRCS:%.cpp=$(OBJ_DIR)/%.o)
PROG_DEPS := $(PROG_SRCS:%.cpp=$(OBJ_DIR)/%.d)

CXX       := g++
CXXFLAGS  ?= -std=c++11 -O2 -MMD -Iinclude -Wno-shift-count-overflow
LDFLAGS   ?= -pthread
AR_RCS    := ar rcs
MKDIR_P   := mkdir -p
RM_F      := rm -f
STRIP     := strip


all: $(PROGRAMS)

clean:
	$(RM_F) $(LIB_OBJS) $(PROG_OBJS) $(LIB_DEPS) $(PROG_DEPS) $(PRAND_LIB)

distclean:
	$(RM_F) $(OBJ_DIR) $(BIN_DIR)

$(PRAND_LIB): $(LIB_OBJS)
	$(AR_RCS) $@ $(LIB_OBJS)

$(OBJ_DIR)/%.o: %.cpp
	@$(MKDIR_P) $(@D)
	$(CXX) -o $@ $< -c $(CXXFLAGS)

$(BIN_DIR)/RNG_%$(EXE): $(OBJ_DIR)/tools/RNG_%.o $(PRAND_LIB)
	@$(MKDIR_P) $(@D)
	$(CXX) -o $@ $< -L$(OBJ_DIR) $(LDFLAGS) -l$(PRAND)
	$(STRIP) $@


.SECONDARY: $(PROG_OBJS) # Prevent deleting objs after building
.PHONY: all clean distclean

-include $(LIB_DEPS) $(PROG_DEPS)