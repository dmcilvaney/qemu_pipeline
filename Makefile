# Basic commands: run_32, run_64: Start a
#

.PHONY: all test_32 run_32 test_64 run_64 clean clean_32 clean_64
all: test_32

ARCH=$(subst test_,,$(subst clean_,,$(subst run_,,$@)))
run_32 run_64:
	$(MAKE) -C qemu_build $@ QEMU_MOUNT_DIR=$(abspath ./run/qemu_$(ARCH)_mnt)

clean: clean_32 clean_64

test_32 test_64:
	$(MAKE) -f build_tests.mk $@ OUT_DIR=$(abspath ./run/qemu_$(ARCH)_mnt)
	$(MAKE) -C qemu_build $@ QEMU_MOUNT_DIR=$(abspath ./run/qemu_$(ARCH)_mnt)

clean_32 clean_64:
	$(MAKE) -f build_tests.mk clean OUT_DIR=$(abspath ./run/qemu_$(ARCH)_mnt)
	$(MAKE) -C qemu_build clean QEMU_MOUNT_DIR=$(abspath ./run/qemu_$(ARCH)_mnt)

