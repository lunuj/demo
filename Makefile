# 编译工具
CROSS_COMPILE	:=
CC		:= $(CROSS_COMPILE)gcc
LD		:= $(CROSS_COMPILE)ld
OBJCOPY := $(CROSS_COMPILE)objcopy
NM		:= $(CROSS_COMPILE)nm

# 文件目录
SOURCE_DIR 	= src
INCLUDE_DIR = include
LIB_DIR		= lib
OUTPUT_DIR	= output
OUTPUT_DEP_DIR	= $(OUTPUT_DIR)/dep
OUTPUT_OBJ_DIR	= $(OUTPUT_DIR)/obj
TOOL_DIR	= tool

# 源文件收集
SRCS 	:= $(shell find $(SOURCE_DIR) -type f -name "*.c")
OBJS 	:= $(patsubst %.c,$(OUTPUT_OBJ_DIR)/%.o,$(SRCS))
DEPS 	:= $(patsubst %.c,$(OUTPUT_DEP_DIR)/%.d,$(SRCS))

# 库文件收集
LIB_FILES  	:= $(shell find $(LIB_DIR) -type f -name "*")
LIBS 		:= $(patsubst %,-l%,$(LIB_FILES))
LIBS_NODIR  := $(patsubst $(LIB_DIR)/lib%.*,-l%,$(LIB_FILES))

# 编译标志
CFLAGS	:= -Wall -Wextra
INCLUDE	:= -I./$(INCLUDE_DIR)
LIB		:= -L$(LIB_DIR)
TARGET  := demo

# 编译命令
$(OUTPUT_OBJ_DIR)/%.o : %.c
	@$(SHELL) -c "echo $(SRCS)"
	@$(SHELL) -c "mkdir -p $(dir $@)"
	@$(SHELL) -c "mkdir -p $(OUTPUT_DEP_DIR)/$(dir $*)"
	$(CC) $(CFLAGS) $(INCLUDE) -MMD -MP -MF $(OUTPUT_DEP_DIR)/$*.d -MT $@ -c $< -o $@

-include $(DEPS)

all: $(OUTPUT_DIR)/$(TARGET) $(LIB_FILES)

$(OUTPUT_DIR)/$(TARGET): $(OBJS)
	$(CC) $^ $(LIB) $(LIBS_NODIR) -o $@

print-%:
	@echo $($*)

clean:
	rm -rf $(OUTPUT_DIR)

.PHONY: all clean
