# === Compiler Settings ===
CXX := g++
CC := gcc
CXXFLAGS := -std=c++20 -Wall -Wextra
CFLAGS := -std=c11 -Wall -Wextra
SHELL := /usr/bin/sh

# === Box2D Library ===
BOX2D_DIR := C:/libs/box2d-main
BOX2D_INCLUDE_DIR := $(BOX2D_DIR)/include
BOX2D_LIB_DIR := $(BOX2D_DIR)/build/src

# === SDL2 / OpenGL ===
LDFLAGS := -L src/lib -L"$(BOX2D_LIB_DIR)"
LDLIBS := -lSDL2main -lSDL2 -lopengl32 -lbox2d

# === Project Paths ===
CONFIG ?= Debug
TARGET_NAME := ecs.exe
BINDIR := bin
BUILDDIR := $(BINDIR)/$(CONFIG)
OBJDIR := $(BINDIR)/obj/$(CONFIG)
PCH := include/pch.h
PCH_GCH := $(PCH).gch

# === Includes ===
CPPFLAGS := -MMD -MP -Isrc -Iinclude -I"$(BOX2D_INCLUDE_DIR)"
PCHFLAGS := -include include/pch.h

# === Source discovery ===
MAIN_SRC := src/main.cpp
SRCS := $(shell python -c "import pathlib; print(' '.join(str(p).replace('\\\\','/') for p in pathlib.Path('src').rglob('*.cpp'))) ")
CSRCS := $(shell python -c "import pathlib; print(' '.join(str(p).replace('\\\\','/') for p in pathlib.Path('src').rglob('*.c'))) ")

OBJS := $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(SRCS)) \
        $(patsubst src/%.c,$(OBJDIR)/%.o,$(CSRCS))

DEPS := $(OBJS:.o=.d)

# === Build Mode ===
ifeq ($(CONFIG),Release)
    CXXFLAGS += -O2
else
    CXXFLAGS += -O0 -g
endif

BIN := $(BUILDDIR)/$(TARGET_NAME)
UNITY_SRC := $(OBJDIR)/UnityBuild.cpp
UNITY_OBJ := $(OBJDIR)/UnityBuild.o
UNITY_BIN := $(BUILDDIR)/ecs_unity.exe
UNITY_SRCS := $(filter-out $(MAIN_SRC),$(SRCS))
UNITY_MAIN_OBJ := $(OBJDIR)/main.o

# === Targets ===
.PHONY: all normal unity clean dirs

all: normal

normal: $(BIN)

unity: $(UNITY_BIN)

# === Linking ===
$(BIN): $(PCH_GCH) $(OBJS) | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(OBJS) -o $@ $(LDFLAGS) $(LDLIBS)

$(UNITY_BIN): $(PCH_GCH) $(UNITY_OBJ) $(UNITY_MAIN_OBJ) | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(UNITY_MAIN_OBJ) $(UNITY_OBJ) -o $@ $(LDFLAGS) $(LDLIBS)

# === Unity build ===
$(UNITY_OBJ): $(UNITY_SRC) | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(PCHFLAGS) -c $< -o $@

$(UNITY_SRC): $(UNITY_SRCS) | $(OBJDIR)
	@/usr/bin/mkdir -p $(dir $@)
	@echo "// Generated unity build file" > $@
	@echo "// Do not edit manually" >> $@
	@for f in $(UNITY_SRCS); do \
		echo "// === $$f ===" >> $@; \
		/usr/bin/cat $$f >> $@; \
		/usr/bin/printf '\n' >> $@; \
	done

# === Compilation Rules ===
$(OBJDIR)/%.o: src/%.cpp $(PCH_GCH) | $(OBJDIR)
	@/usr/bin/mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(PCHFLAGS) -c $< -o $@

$(OBJDIR)/%.o: src/%.c | $(OBJDIR)
	@/usr/bin/mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(PCH_GCH): $(PCH) | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -x c++-header $< -o $@

$(OBJDIR) $(BUILDDIR):
	@/usr/bin/mkdir -p $@

clean:
	@/usr/bin/rm -rf $(BINDIR)/obj $(BINDIR)/Debug $(BINDIR)/Release $(UNITY_SRC) $(PCH_GCH)

-include $(DEPS)
