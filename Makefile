# 编译工具
CROSS_COMPILE	:=
CC		:= $(CROSS_COMPILE)gcc
LD		:= $(CROSS_COMPILE)ld
OBJCOPY := $(CROSS_COMPILE)objcopy
NM		:= $(CROSS_COMPILE)nm
BIN_SUFFIX	:= .out
# 文件目录
SOURCE_DIR 	= src
INCLUDE_DIR = include
THIRD_DIR	= third
LIB_DIR		= lib
OUTPUT_DIR	= output
TEST_DIR	= test
OUTPUT_DEP_DIR	= $(OUTPUT_DIR)/dep
OUTPUT_OBJ_DIR	= $(OUTPUT_DIR)/obj
TOOL_DIR	= tool

# 源文件收集
SRCS 	:= $(shell find $(SOURCE_DIR) -type f -name "*.c")
OBJS 	:= $(patsubst %.c,$(OUTPUT_OBJ_DIR)/%.o,$(SRCS))
DEPS 	:= $(patsubst %.c,$(OUTPUT_DEP_DIR)/%.d,$(SRCS))

SRCS_NO_MAIN := $(filter-out $(SOURCE_DIR)/main.c, $(SRCS))
OBJS_NO_MAIN := $(patsubst %.c,$(OUTPUT_OBJ_DIR)/%.o,$(SRCS_NO_MAIN)) 
DEPS_NO_MAIN := $(patsubst %.c,$(OUTPUT_DEP_DIR)/%.d,$(SRCS_NO_MAIN))

# 库文件收集
LIB_FILES  	:= $(shell find $(LIB_DIR) -type f -name "*")
LIBS  := $(patsubst $(LIB_DIR)/lib%.*,-l%,$(LIB_FILES))

THIRD_INC := $(patsubst %,-I%,$(wildcard $(THIRD_DIR)/*/install/include) $(wildcard $(THIRD_DIR)/*/include))
THIRD_LIB_DIR := $(patsubst %,-L%,$(wildcard $(THIRD_DIR)/*/install/lib) $(wildcard $(THIRD_DIR)/*/lib))
THIRD_LIB_DIR_PATH := $(foreach d,$(wildcard $(THIRD_DIR)/*/install/lib) $(wildcard $(THIRD_DIR)/*/lib),-Wl,-rpath,$(d))
THIRD_LIB_FILE := $(filter %.so %.lib %.a, $(wildcard $(THIRD_DIR)/*/install/lib/*) $(wildcard $(THIRD_DIR)/*/lib/*))
THIRD_LIBS := $(patsubst lib%,-l%,$(basename $(notdir $(THIRD_LIB_FILE))))

# 操作系统库
ifeq ($(OS),)
  UNAME_S := $(shell uname -s)
  ifeq ($(UNAME_S),Darwin)
    OS := Mac
  else ifeq ($(UNAME_S),Linux)
    OS := Linux
  else ifeq ($(UNAME_S),Windows_NT)
    OS := Windows
  else
    OS := Unknown
  endif
endif

ifeq ($(OS), Mac)
	OS_LIBS = -framework Cocoa -framework AppKit -framework Foundation \
			  -framework CoreFoundation -framework IOKit -framework CoreVideo \
			  -framework OpenGL -framework QuartzCore
else ifeq ($(OS), Linux)
  	OS_LIBS := -ldl -lpthread -lGL -lX11
else ifeq ($(OS), Windows)
	OS_LIBS = -lgdi32 -luser32 -lkernel32 -lshell32 -lopengl32
else
  $(error Unsupported OS: $(OS))
endif

# 编译标志
CFLAGS	:= -Wall -Wextra -g
INCLUDE	:= -I./$(INCLUDE_DIR) $(THIRD_INC)
LIB		:= -L./$(LIB_DIR) $(THIRD_LIB_DIR) $(THIRD_LIB_DIR_PATH)
TARGET  := demo$(BIN_SUFFIX)

# 编译命令
$(OUTPUT_OBJ_DIR)/%.o : %.c
	@$(SHELL) -c "echo $(SRCS)"
	@$(SHELL) -c "mkdir -p $(dir $@)"
	@$(SHELL) -c "mkdir -p $(OUTPUT_DEP_DIR)/$(dir $*)"
	$(CC) $(CFLAGS) $(INCLUDE) -MMD -MP -MF $(OUTPUT_DEP_DIR)/$*.d -MT $@ -c $< -o $@

-include $(DEPS)
-include $(DEPS_NO_MAIN)

all: $(OUTPUT_DIR)/$(TARGET) $(LIB_FILES) $(THIRD_LIB_FILE)

$(OUTPUT_DIR)/$(TARGET): $(OBJS)
	$(CC) $^ $(LIB) $(LIBS) $(THIRD_LIBS) $(OS_LIBS) -o $@

# TODO: %.o will be deleted, should we keep it?
$(OUTPUT_DIR)/%.out: $(OUTPUT_OBJ_DIR)/$(TEST_DIR)/%.o $(OBJS_NO_MAIN) | $(LIB_FILES)
	echo $(THIRD_LIB_DIR_PATH)
	$(CC) $^ $(LIB) $(LIBS) $(THIRD_LIBS) $(OS_LIBS) -o $@
print-%:
	@echo $($*)

clean:
	rm -rf $(OUTPUT_DIR)

.PHONY: all clean
