CC      ?= $(CROSS_COMPILE)gcc
LD      ?= $(CROSS_COMPILE)ld
AR      ?= $(CROSS_COMPILE)ar
NM      ?= $(CROSS_COMPILE)nm
OBJCOPY ?= $(CROSS_COMPILE)objcopy
OBJDUMP ?= $(CROSS_COMPILE)objdump
READELF ?= $(CROSS_COMPILE)readelf

SUB_DIRS = edk2 edk2_driver edk2_test_harness edk2_variable_test
C_FILES = $(foreach dir,$(SUB_DIRS),$(wildcard $(dir)/*.c)) $(wildcard *.c)
OBJS = $(C_FILES:.c=.o)

CFLAGS += -Wall -I../ta/include -I$(TEEC_EXPORT)/include -I./include -I./ -fmax-errors=2 -Werror
#Add/link other required libraries here
LDADD += -l:libteec.a -L$(TEEC_EXPORT)/lib

BINARY = authvars_test

DB = edk2_variable_test/Test_db.blob
KEK = edk2_variable_test/Test_KEK.blob
BLOBS = $(DB) $(KEK) ./test.blob

.PHONY: all
all: $(BINARY)

$(BINARY): $(BLOBS) $(OBJS) 
	$(CC) -o $@ $^ $(LDADD)

.PHONY: clean
clean:
	rm -f $(OBJS) $(BINARY) $(BLOBS)


%.o: %.c
	@echo $$GNUTARGET
	$(CC) $(CFLAGS) -c $< -o $@

BIN_START=_binary_$(subst /,_,$*)_bin_start
BIN_SIZE=_binary_$(subst /,_,$*)_bin_size
SHORT_NAME=_binary_$(*F)_bin
$(BLOBS): %.blob:%.bin
	$(LD) -b binary -r $< -o $@.tmp
	$(OBJCOPY) --redefine-sym $(BIN_START)=$(SHORT_NAME)_start --redefine-sym $(BIN_SIZE)=$(SHORT_NAME)_size $@.tmp $@
	rm -f $@.tmp
