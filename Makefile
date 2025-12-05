CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra
CPPFLAGS := -MMD -MP -Isrc -Iinclude -include include/pch.h
LDFLAGS := -L src/lib
LDLIBS := -lSDL2main -lSDL2 -lopengl32
SHELL := /usr/bin/sh

CONFIG ?= Debug
TARGET_NAME := ecs.exe
BINDIR := bin
BUILDDIR := $(BINDIR)/$(CONFIG)
OBJDIR := $(BINDIR)/obj/$(CONFIG)

PCH := include/pch.h
PCH_GCH := $(PCH).gch

MAIN_SRC := src/main.cpp
SRCS := $(shell python -c "import pathlib; print(' '.join(str(p).replace('\\\\','/') for p in pathlib.Path('src').rglob('*.cpp'))) ")
CSRCS := $(shell python -c "import pathlib; print(' '.join(str(p).replace('\\\\','/') for p in pathlib.Path('src').rglob('*.c'))) ")
OBJS := $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(SRCS)) \
        $(patsubst src/%.c,$(OBJDIR)/%.o,$(CSRCS))
DEPS := $(OBJS:.o=.d)
GLAD_OBJ := $(OBJDIR)/glad.o

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

.PHONY: all normal unity clean dirs

all: normal
normal: $(BIN)

unity: $(UNITY_BIN)

$(BIN): $(PCH_GCH) $(OBJS) | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(OBJS) -o $@ $(LDFLAGS) $(LDLIBS)

$(UNITY_BIN): $(PCH_GCH) $(UNITY_OBJ) $(UNITY_MAIN_OBJ) $(GLAD_OBJ) | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(UNITY_MAIN_OBJ) $(UNITY_OBJ) $(GLAD_OBJ) -o $@ $(LDFLAGS) $(LDLIBS)

$(UNITY_OBJ): $(UNITY_SRC) | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(UNITY_SRC): $(UNITY_SRCS) | $(OBJDIR)
	@/usr/bin/mkdir -p $(dir $@)
	@echo "// Generated unity build file" > $@
	@echo "// Do not edit manually" >> $@
	@for f in $(UNITY_SRCS); do \
		echo "// === $$f ===" >> $@; \
		/usr/bin/cat $$f >> $@; \
		/usr/bin/printf '\n' >> $@; \
	done

$(OBJDIR)/%.o: src/%.cpp $(PCH_GCH) | $(OBJDIR)
	@/usr/bin/mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJDIR)/%.o: src/%.c $(PCH_GCH) | $(OBJDIR)
	@/usr/bin/mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(PCH_GCH): $(PCH) | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -MMD -MP -Isrc -Iinclude -x c++-header $< -o $@

$(OBJDIR) $(BUILDDIR):
	@/usr/bin/mkdir -p $@

clean:
	@/usr/bin/rm -rf $(BINDIR)/obj $(BINDIR)/Debug $(BINDIR)/Release $(UNITY_SRC) $(PCH_GCH)

-include $(DEPS)
