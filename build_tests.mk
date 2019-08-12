TESTS_32 = $(addsuffix _32, authvars_test ftpm_test)
TESTS_64 = $(addsuffix _64,authvars_test ftpm_test)
CLEAN_32 = $(addsuffix _clean, $(TESTS_32))
CLEAN_64 = $(addsuffix _clean, $(TESTS_64))
GCC_32_PATH ?= ./toolchains/gcc-linaro-6.4.1-2017.11-x86_64_arm-linux-gnueabihf/bin/
GCC_64_PATH ?= ./toolchains/gcc-linaro-7.2.1-2017.11-x86_64_aarch64-linux-gnu/bin/
GCC_32 = $(GCC_32_PATH)/arm-linux-gnueabihf-
GCC_64 = $(GCC_64_PATH)/aarch64-linux-gnu-

ifneq ($(TEEC_EXPORT_PATH),)
TEEC_EXPORT_PATH_32 ?= $(TEEC_EXPORT_PATH)
TEEC_EXPORT_PATH_64 ?= $(TEEC_EXPORT_PATH)
else
TEEC_EXPORT_PATH_32 ?= ./qemu_build/qemu_32/optee_client/out/export/usr
TEEC_EXPORT_PATH_64 ?= ./qemu_build/qemu_64/optee_client/out/export/usr
endif

.PHONY: all test_32 test_64 $(TESTS_32) $(TESTS_64) test_toolchains clean

test_32: $(TESTS_32)
test_64: $(TESTS_64)

all: $(GCC_32) $(GCC_64)
$(info OUT_DIR = $(OUT_DIR))
OUT_DIR ?= ./tests/
$(info OUT_DIR = $(OUT_DIR))

OUT_PATH_32=$(abspath $(OUT_DIR)/$@/)
$(info OUT_PATH_32 = $(OUT_PATH_32))
$(TESTS_32): $(GCC_32_PATH) optee_32
	$(MAKE) -C $(subst _32,,$@) \
		CROSS_COMPILE=$(abspath $(GCC_32)) \
		PLATFORM=vexpress-qemu_virt \
		TEEC_EXPORT=$(abspath $(TEEC_EXPORT_PATH_32)) \
		OUT_DIR=$(abspath $(OUT_PATH_32))

OUT_PATH_64=$(abspath $(OUT_DIR)/$@/)
$(TESTS_64): $(GCC_64_PATH) optee_64
	$(MAKE) -C $(subst _64,,$@) \
		CROSS_COMPILE=$(abspath $(GCC_64)) \
		PLATFORM=vexpress-qemu_armv8a \
		TEEC_EXPORT=$(abspath $(TEEC_EXPORT_PATH_64)) \
		OUT_DIR=$(abspath $(OUT_PATH_64))

.PHONY: optee_32 optee_64

optee_32:
	if [ ! -d $(TEEC_EXPORT_PATH_32) ]; \
	then \
	  $(MAKE) -C qemu_build $@; \
	fi;

optee_64:
	if [ ! -d $(TEEC_EXPORT_PATH_64) ]; \
	then \
	  $(MAKE) -C qemu_build $@; \
	fi;

test_toolchains: $(GCC_32_PATH) $(GCC_64_PATH)

$(GCC_32_PATH):
	if [ ! -f $(GCC_32)gcc ]; \
	then \
	  mkdir -p toolchains; \
	  wget https://releases.linaro.org/components/toolchain/binaries/6.4-2017.11/arm-linux-gnueabihf/gcc-linaro-6.4.1-2017.11-x86_64_arm-linux-gnueabihf.tar.xz; \
	  tar xf gcc-linaro-6.4.1-2017.11-x86_64_arm-linux-gnueabihf.tar.xz -C toolchains; \
	fi;
	rm -f gcc-linaro-6.4.1-2017.11-x86_64_arm-linux-gnueabihf.tar.xz

$(GCC_64_PATH):
	if [ ! -f $(GCC_64)gcc ]; \
	then \
	  mkdir -p toolchains; \
	  wget https://releases.linaro.org/components/toolchain/binaries/7.2-2017.11/aarch64-linux-gnu/gcc-linaro-7.2.1-2017.11-x86_64_aarch64-linux-gnu.tar.xz; \
	  tar xf gcc-linaro-7.2.1-2017.11-x86_64_aarch64-linux-gnu.tar.xz -C toolchains; \
	fi;
	rm -f gcc-linaro-7.2.1-2017.11-x86_64_aarch64-linux-gnu.tar.xz

clean: $(CLEAN_32) $(CLEAN_64) clean_tools

.PHONY: $(CLEAN_32) $(CLEAN_64) clean_tools

clean_tools:
	rm -rf toolchains

CLEAN_PATH_32=$(subst _clean,,$(abspath $(OUT_DIR)/$@/))
$(CLEAN_32):
	$(MAKE) -C $(subst _32_clean,,$@) clean OUT_DIR=$(abspath $(CLEAN_PATH_32))

CLEAN_PATH_64=$(subst _clean,,$(abspath $(OUT_DIR)/$@/))
$(CLEAN_64):
	$(MAKE) -C $(subst _64_clean,,$@) clean OUT_DIR=$(abspath $(CLEAN_PATH_64))