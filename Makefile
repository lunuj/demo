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
else ifeq ($(OS),Windows_NT)
	OS := Windows
endif

# 编译工具
CROSS_COMPILE  := 
CC              := $(CROSS_COMPILE)gcc
LD              := $(CROSS_COMPILE)ld
OBJCOPY         := $(CROSS_COMPILE)objcopy
NM              := $(CROSS_COMPILE)nm
BIN_SUFFIX      := .out

# 文件目录
SOURCE_DIR      := src
LIB_DIR         := lib
INCLUDE_DIR     := include
TEST_DIR        := test
OUTPUT_DIR      := output
OUTPUT_DEP_DIR  := $(OUTPUT_DIR)/.dep
OUTPUT_OBJ_DIR  := $(OUTPUT_DIR)/.obj
TOOL_DIR        := tool
EXAMPLE_DIR		:= example

# 确保必要目录存在
$(shell mkdir -p $(SOURCE_DIR) $(LIB_DIR)/$(OS) $(INCLUDE_DIR) $(TEST_DIR) $(OUTPUT_DIR) $(TOOL_DIR) $(EXAMPLE_DIR))

# 源文件收集
SRCS            := $(shell find $(SOURCE_DIR) -type f -name "*.c")
OBJS            := $(patsubst %.c,$(OUTPUT_OBJ_DIR)/%.o,$(SRCS))
DEPS            := $(patsubst %.c,$(OUTPUT_DEP_DIR)/%.d,$(SRCS))

SRCS_NO_MAIN    := $(filter-out $(SOURCE_DIR)/main.c, $(SRCS))
OBJS_NO_MAIN    := $(patsubst %.c,$(OUTPUT_OBJ_DIR)/%.o,$(SRCS_NO_MAIN)) 
DEPS_NO_MAIN    := $(patsubst %.c,$(OUTPUT_DEP_DIR)/%.d,$(SRCS_NO_MAIN))

TSET_SRCS       := $(shell find $(TEST_DIR) -type f -name "*.c")
TEST_OBJS       := $(patsubst %.c,$(OUTPUT_OBJ_DIR)/%.o,$(TSET_SRCS)) 
TEST_DEPS       := $(patsubst %.c,$(OUTPUT_DEP_DIR)/%.d,$(TSET_SRCS)) 

-include $(DEPS)
-include $(DEPS_NO_MAIN)
-include $(TEST_DEPS)

# 编译标志
CFLAGS          := -Wall -Wextra -g -Wno-unused-variable -Wno-missing-braces
INCLUDE         := -I./$(INCLUDE_DIR)
LIB             := -L./$(LIB_DIR)
TARGET          := demo$(BIN_SUFFIX)

# 库文件收集
LIB_FILES 		:= $(shell find $(LIB_DIR)/$(OS) -type f  -name "*.so" -o -name "*.lib" -o -name "*.dll" -o -name "*.a")
LIBS            := $(patsubst $(LIB_DIR)/$(OS)/lib%.*,-l%,$(LIB_FILES))

ifeq ($(OS), Mac)
	OS_LIBS =
else ifeq ($(OS), Linux)
  	OS_LIBS =
else ifeq ($(OS), Windows)
	OS_LIBS =
else
  	$(error Unsupported OS: $(OS))
endif

# 创建目录的规则
define MAKE_DIR
    @mkdir -p $1
endef

# 编译命令
$(OUTPUT_OBJ_DIR)/%.o : %.c
	$(call MAKE_DIR,$(dir $@))
	$(call MAKE_DIR,$(OUTPUT_DEP_DIR)/$(dir $*))
	$(CC) $(CFLAGS) $(INCLUDE) -MMD -MP -MF $(OUTPUT_DEP_DIR)/$*.d -MT $@ -c $< -o $@

# 默认目标
all: $(OUTPUT_DIR)/$(TARGET) $(LIB_FILES)

$(OUTPUT_DIR)/$(TARGET): $(OBJS)
	$(CC) $^ $(LIB) $(LIBS) $(OS_LIBS) -o $@

# 测试目标
$(OUTPUT_DIR)/%.out: $(OUTPUT_OBJ_DIR)/$(TEST_DIR)/%.o $(OBJS_NO_MAIN) | $(LIB_FILES)
	$(call MAKE_DIR,$(dir $@))
	$(CC) $^ $(LIB) $(LIBS) $(OS_LIBS) -o $@

# 打印目标
print-%:
	@echo $($*)

# 清理目标
clean:
	rm -rf $(OUTPUT_DIR)

.PHONY: all clean

# TODO add static and dynamic lib
# 静态库目标
static: $(OBJS_NO_MAIN) | $(LIB_FILES)
	ar rcs $(OUTPUT_DIR)/lib$(TARGET_NAME).a $<

# 动态库目标
dynamic: $(OBJS_NO_MAIN) | $(LIB_FILES)
	$(CC) -shared -o $(OUTPUT_DIR)/lib$(TARGET_NAME).so $<
