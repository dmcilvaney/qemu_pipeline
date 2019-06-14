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

SUB_DIRS = edk2 edk2_driver edk2_test_harness edk2_variable_test
C_FILES = $(foreach dir,$(SUB_DIRS),$(wildcard $(dir)/*.c)) $(wildcard *.c)
OBJS = $(C_FILES:.c=.o)

CFLAGS += -Wall -I../ta/include -I$(TEEC_EXPORT)/include -I./include -I./ -fmax-errors=2 -Werror -fbounds-check -g
#Add/link other required libraries here
LDADD += -l:libteec.a -L$(TEEC_EXPORT)/lib -fbounds-check

BINARY = authvars_test

DB = edk2_variable_test/Test_db.blob
KEK = edk2_variable_test/Test_KEK.blob
BLOBS = $(DB) $(KEK) hello_world.blob

.PHONY: all
all: $(BINARY)

$(BINARY): $(addprefix $(OUT_DIR)/,$(OBJS) $(BLOBS))
	$(CC) -o $(OUT_DIR)/$@ $^ $(LDADD)

.PHONY: clean
clean:
	rm -f $(addprefix $(OUT_DIR)/,$(OBJS) $(BINARY) $(BLOBS))
	rm -rf $(OUT_DIR)


$(OUT_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

BIN_START=_binary_$(subst /,_,$*)_bin_start
BIN_SIZE=_binary_$(subst /,_,$*)_bin_size
SHORT_NAME=_binary_$(*F)_bin
$(OUT_DIR)/%.blob: %.bin
	$(LD) -b binary -r $< -o $@.tmp
	$(OBJCOPY) --redefine-sym $(BIN_START)=$(SHORT_NAME)_start --redefine-sym $(BIN_SIZE)=$(SHORT_NAME)_size $@.tmp $@
	rm -f $@.tmp
