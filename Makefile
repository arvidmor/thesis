
# Directories
BUILDDIR = build
SRCDIR = src
OBJDIR = $(BUILDDIR)/obj
ASMDIR = $(BUILDDIR)/asm

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
ASMS = $(SRCS:$(SRCDIR)/%.c=$(ASMDIR)/%.s)
TARGET = $(BUILDDIR)/app

# Flags
MMCU = msp430fr5994
CFLAGS = -I$(INCLUDE_DIRS) -Wall -mmcu=$(MMCU) -mlarge  -mhwmult=f5series
LDFLAGS = $(CFLAGS) -L$(LIB_DIRS) -mlarge -Xlinker -Map=$(BUILDDIR)/app.map

# Build mode
BUILD_DEBUG = 1
ifeq ($(BUILD_DEBUG), 1)
	CFLAGS += -g
else 
	CFLAGS += -O2
endif

#### TARGETS ####


$(TARGET): $(OBJS) $(ASMS) $(BUILDDIR)/main.diff
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

$(ASMDIR)/%.s: $(SRCDIR)/%.c
	@mkdir -p $(ASMDIR)
	@mv $@ $@.old 2>/dev/null || true
	$(CC) $(CFLAGS) -S $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/main.diff: $(ASMS)
	diff $(ASMS) $(ASMS).old > $(BUILDDIR)/main.diff || true

#### PHONIES ####
.PHONY: all clean flash

all: $(TARGET)

flash: $(TARGET)
	$(DEBUG) tilib "prog $(TARGET)"

clean:
	rm -f $(OBJS) $(TARGET) $(ASMS) $(BUILDDIR)/main.diff
	rm -rf $(OBJDIR)
