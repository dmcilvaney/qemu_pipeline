CC      ?= $(CROSS_COMPILE)gcc
LD      ?= $(CROSS_COMPILE)ld
AR      ?= $(CROSS_COMPILE)ar
NM      ?= $(CROSS_COMPILE)nm
OBJCOPY ?= $(CROSS_COMPILE)objcopy
OBJDUMP ?= $(CROSS_COMPILE)objdump
READELF ?= $(CROSS_COMPILE)readelf

OBJS = main.o edk2_driver/AuthVarsDxe.o edk2_driver/uefi_c.o

CPPFLAGS += -fshort-wchar
CFLAGS += -Wall -I../ta/include -I$(TEEC_EXPORT)/include -I./include -fmax-errors=3 -Werror 
#Add/link other required libraries here
LDADD += -l:libteec.a -L$(TEEC_EXPORT)/lib

BINARY = authvars_test

.PHONY: all
all: $(BINARY)

$(BINARY): $(OBJS)
	$(CC) -o $@ $^ $(LDADD)

.PHONY: clean
clean:
	rm -f $(OBJS) $(BINARY)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
