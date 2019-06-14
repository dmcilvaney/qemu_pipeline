CROSS_COMPILE ?= ~/gcc-linaro-6.4.1-2017.11-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
PLATFORM ?= vexpress-qemu_virt
TEEC_EXPORT ?= ../../qemu_build

CC      ?= $(CROSS_COMPILE)gcc
LD      ?= $(CROSS_COMPILE)ld
AR      ?= $(CROSS_COMPILE)ar
NM      ?= $(CROSS_COMPILE)nm
OBJCOPY ?= $(CROSS_COMPILE)objcopy
OBJDUMP ?= $(CROSS_COMPILE)objdump
READELF ?= $(CROSS_COMPILE)readelf

OUT_DIR ?= ./out

OBJS = main.o

CFLAGS += -Wall -I../ta/include -I$(TEEC_EXPORT)/include -I./include
#Add/link other required libraries here
LDADD += -l:libteec.a -L$(TEEC_EXPORT)/lib

BINARY = $(OUT_DIR)/ftpm_test

.PHONY: all
all: $(BINARY)

$(BINARY): $(addprefix $(OUT_DIR)/,$(OBJS))
	mkdir -p $(dir $@)
	$(CC) -o $@ $< $(LDADD)

.PHONY: clean
clean:
	rm -f $(addprefix $(OUT_DIR)/,$(OBJS) $(BINARY))
	rm -rf $(OUT_DIR)


$(OUT_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@
