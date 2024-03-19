
# Directories
BUILDDIR = build
SRCDIR = src
OBJDIR = $(BUILDDIR)/obj
LIBDIR = support/include
TOOLDIR = tools/bin
# Tools
CC = $(TOOLDIR)/msp430-elf-gcc
GDB = $(TOOLDIR)/msp430-elf-gdb
# Files
SRCS = $(SRCDIR)/main.c
OBJS = $(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
TARGET = $(BUILDDIR)/app
# Flags
MMCU = msp430fr5994
CFLAGS = -I$(LIBDIR) -Wall -mmcu=$(MMCU)
LDFLAGS = -L$(LIBDIR) -mmcu=$(MMCU)
# Debug
DEBUG = 1
ifeq ($(DEBUG), 1)
	CFLAGS += -g
else 
	CFLAGS += -O2
endif

#### TARGETS ####
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf $(OBJDIR)

.PHONY: all clean