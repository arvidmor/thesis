
# Directories
BUILDDIR = build
SRCDIR = src
OBJDIR = $(BUILDDIR)/obj

MSPGCC_ROOT_DIR = $(HOME)/tools/msp430-gcc
MSPGCC_BIN_DIR = $(MSPGCC_ROOT_DIR)/bin
MSP_GCC_LIB_DIR = $(MSPGCC_ROOT_DIR)/include
CCS_DIR = /opt/ccstudio/ccs
DEBUG_BIN_DIR = $(CCS_DIR)/ccs_base/DebugServer/bin
DEBUG_DRIVERS_DIR = $(CCS_DIR)/ccs_base/DebugServer/drivers

INCLUDE_DIRS = $(MSP_GCC_LIB_DIR)
LIB_DIRS = $(MSP_GCC_LIB_DIR)

# Toolchain
CC = $(MSPGCC_BIN_DIR)/msp430-elf-gcc
GDB = $(MSPGCC_BIN_DIR)/msp430-elf-gdb
DEBUG = LD_LIBRARY_PATH=$(DEBUG_DRIVERS_DIR) $(DEBUG_BIN_DIR)/mspdebug

# Files
SRCS = $(SRCDIR)/main.c
OBJS = $(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
TARGET = $(BUILDDIR)/app

# Flags
MMCU = msp430fr5994
CFLAGS = -I$(INCLUDE_DIRS) -Wall -mmcu=$(MMCU)
LDFLAGS = -L$(LIB_DIRS) -mmcu=$(MMCU)

# Build mode
BUILD_DEBUG = 1
ifeq ($(BUILD_DEBUG), 1)
	CFLAGS += -g
else 
	CFLAGS += -O2
endif

#### TARGETS ####


$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

#### PHONIES ####
.PHONY: all clean flash

all: $(TARGET)

flash: $(TARGET)
	$(DEBUG) tilib "prog $(TARGET)"

clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf $(OBJDIR)
