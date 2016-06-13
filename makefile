rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *, %,$2),$d))
directories=$(sort $(dir $(wildcard $/*)))
ifeq ($(OS),Windows_NT)
FIX_PATH = $(subst /,\,$1)
	REPORT = @echo $1
	CHK_DIR_EXISTS = if not exist "$(strip $1)" mkdir "$(strip $1)"
	NUKE = rmdir /s /q
	COPY_DIR = xcopy $(call FIX_PATH,$1 $2) /E /H /Y
	COPY_CONTENT = xcopy /s /Y $(call FIX_PATH,$1 $2)
	COPY = xcopy $(call FIX_PATH,$1 $2) /Y
	INSTALL_LIB_DIR := Z:/lib/
	INSTALL_BIN_DIR := Z:/bin/
	INSTALL_INCLUDE_DIR := Z:/include/
	EXE_SUFFIX = .exe
	LIB_SUFFIX =.dll
else
	REPORT = @echo -e "\e[4;1;37m$1\033[0m"
	CHK_DIR_EXISTS = test -d $1 || mkdir -p $1
	NUKE = rm -r $1
	COPY_DIR = cp -rv $1 $2
	FIX_PATH = $1
	INSTALL_LIB_DIR := ~/lib/
	INSTALL_BIN_DIR := ~/bin/
	INSTALL_INCLUDE_DIR := ~/include/
	EXE_SUFFIX =
	LIB_SUFFIX :=.so
endif

PROJECT_DIR :=$(dir $(realpath $(lastword $(MAKEFILE_LIST))))
OBJ_DIR := $(PROJECT_DIR)obj
BIN_DIR := $(PROJECT_DIR)bin
SRC_DIR := $(PROJECT_DIR)/src

C_FILES := $(wildcard $(SRC_DIR)/*.c)
CPP_FILES := $(wildcard $(SRC_DIR)/*.cpp)
SCRIPTS := $(wildcard $(SCRIPT_DIR)/*.sh)

BINARIES := $(patsubst $(SRC_DIR)/binaries/%.c,$(BIN_DIR)/%$(EXE_SUFFIX),$(wildcard $(SRC_DIR)/binaries*.c))
BINARIES += $(patsubst $(SRC_DIR)/binaries/%.cpp,$(BIN_DIR)/%$(EXE_SUFFIX),$(wildcard $(SRC_DIR)/binaries/*.cpp))

OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_FILES))
OBJ_FILES += $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CPP_FILES))
ifeq ($(OS),Windows_NT)
else
	LD_FLAGS += -ldl
endif
LD_FLAGS += -lutilities -L$(INSTALL_LIB_DIR)
COMMON_COMPILER_FLAGS := -march=native -Ofast -flto -pipe -g -I$(PROJECT_DIR)headers -I$(INSTALL_INCLUDE_DIR) -DVKOS_DYNAMIC
C_FLAGS += --std=gnu99 $(COMMON_COMPILER_FLAGS)
CPP_FLAGS += --std=c++14 $(COMMON_COMPILER_FLAGS)

.PRECIOUS: $(OBJ_FILES)

all: binaries

binaries: $(BINARIES)

$(BIN_DIR)%$(EXE_SUFFIX) : $(SRC_DIR)/binaries/%.c $(OBJ_FILES)
	$(call REPORT,Building $@)
	@$(call CHK_DIR_EXISTS, $(dir $@))
	gcc $(C_FLAGS) -o "$@" "$<"  $(patsubst %, "%", $(OBJ_FILES)) $(LD_FLAGS)

$(BIN_DIR)%$(EXE_SUFFIX) : $(SRC_DIR)/binaries/%.cpp $(OBJ_FILES)
	$(call REPORT,Building $@)
	@$(call CHK_DIR_EXISTS, $(dir $@))
	g++ $(CPP_FLAGS) -o "$@" "$<"  $(patsubst %, "%", $(OBJ_FILES)) $(LD_FLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(call REPORT,Compiling $@)
	@$(call CHK_DIR_EXISTS, $(dir $@))
	@gcc $(C_FLAGS) -o "$@" -c "$<"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(PROJECT_DIR)headers/%.hpp
	$(call REPORT,Compiling $@)
	@$(call CHK_DIR_EXISTS, $(dir $@))
	@g++ $(CPP_FLAGS) -o "$@" -c "$<"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(PROJECT_DIR)headers/vulkanOs/%.hpp
	$(call REPORT,Compiling $@)
	@$(call CHK_DIR_EXISTS, $(dir $@))
	@g++ $(CPP_FLAGS) -o "$@" -c "$<"

clean:
	$(call REPORT,Cleaning...)
	@-$(NUKE) "$(OBJ_DIR)" "$(BIN_DIR)"
