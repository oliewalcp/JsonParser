CC = gcc
DEFINES = 
CCFLAGS = -Wall -g $(DEFINES)
CXX = g++
CXXFLAGS = $(CCFLAGS) -std=c++11  # g++ 编译选项
STD_LIB = #-lpthread -lstdc++fs # 需要链接的标准库

# 第三方库
INCLUDE_PATH = 
LIB_PATH = 
DEPENDENT_LIBS =

# 输出位置
OUTPUT_DIR = output
OBJ_DIR = $(OUTPUT_DIR)/obj
BIN_DIR = $(OUTPUT_DIR)/bin

# 输出类型
OUTPUT_TYPE = BINARY # 当OUTPUT_TYPE值为BINARY时，生成.exe/.sh文件，否则，生成.a文件

# 项目名称
PROJECT_NAME = json_parser

# 文件移动命令
MOVE_CMD = mv

# Windows平台
ifeq ($(OS), Windows_NT)
DEPENDENT_LIBS = #-lwsock32

MOVE_CMD = move /y

SOURCES_PATH = $(shell dir /ad /b /s) # 递归查找当前目录下的所有目录
$(shell if not exist "$(OUTPUT_DIR)" mkdir $(OUTPUT_DIR))
$(shell if not exist "$(OUTPUT_DIR)\obj" mkdir $(OUTPUT_DIR)\obj)
$(shell if not exist "$(OUTPUT_DIR)\bin" mkdir $(OUTPUT_DIR)\bin)

# 编译目标
ifeq ($(OUTPUT_TYPE), BINARY)
TARGET = $(BIN_DIR)/$(PROJECT_NAME).exe
else
TARGET = $(BIN_DIR)/$(PROJECT_NAME).a
endif

# 搜寻各目录下下的cpp文件
SOURCES := $(foreach it, $(SOURCES_PATH), $(wildcard $(it)/*.cpp))
SOURCES := $(SOURCES) $(wildcard *.cpp)

else

LIBS = 
SOURCES_PATH = $(shell find . -maxdepth 5 -type d) # 递归查找当前目录下的所有目录
$(shell mkdir -p $(OUTPUT_DIR))
$(shell mkdir -p $(OBJ_DIR))
$(shell mkdir -p $(BIN_DIR))
# 编译目标
ifeq ($(OUTPUT_TYPE), BINARY)
TARGET = $(BIN_DIR)/$(PROJECT_NAME).sh
else
TARGET = $(BIN_DIR)/$(PROJECT_NAME).a
endif

# 搜寻各目录下下的cpp文件
SOURCES := $(foreach it, $(SOURCES_PATH), $(wildcard $(it)/*.cpp))

endif

# 当前目录路径
PROJECT_PATH = $(abspath .)

# 转换为相对路径
SOURCES := $(subst $(PROJECT_PATH)/,,$(subst \,/,$(SOURCES)))

# 获取同名的.o文件
objects = $(patsubst %.cpp, %.o, $(SOURCES))

# 生成目标文件
ifeq ($(OUTPUT_TYPE), BINARY)
all: $(objects)
	$(CXX) $(CXXFLAGS) $(LIB_PATH) -o $(TARGET) $(wildcard $(OBJ_DIR)/*.o) $(DEPENDENT_LIBS) $(STD_LIB)
else
all: $(objects)
	ar cq $(TARGET) $(wildcard $(OBJ_DIR)/*.o)
endif

$(objects):%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c $< $(INCLUDE_PATH) -o $(addprefix $(OUTPUT_DIR)/obj/, $(notdir $@))

.PHONY: clean
clean:
	rm -f $(objects)
