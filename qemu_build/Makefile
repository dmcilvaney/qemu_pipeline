.ONESHELL:
SHELL = bash

QEMU_MOUNT_DIR ?= $(abspath $(shell realpath ~/qemu_mnt))


.PHONY: all test update run_32 run_64
all: update
update: tar_32 tar_64
test: test_32 test_64

TAR_32_NAME=../qemu_32.tar.gz
TAR_64_NAME=../qemu_64.tar.gz
TAR_32_RULE=$(if $(wildcard $(TAR_32_NAME)),,$(TAR_32_NAME))
TAR_64_RULE=$(if $(wildcard $(TAR_64_NAME)),,$(TAR_64_NAME))

.PHONY: run_32 run_64 test_32 test_64 instructions prep_32 prep_64
instructions:
	@echo ''
	@echo ==============
	@echo ''
	@echo 'To access the VM run'
	@echo '      sudo screen /dev/pts/<1st#> for normal world'
	@echo '      sudo screen /dev/pts/<2nd#> for secure world'
	@echo 'Login using:'
	@echo '      root'
	@echo 'To mount files at $(QEMU_MNT_DIR) run:'
	@echo '      mkdir -p /mnt/ci && mount -t 9p -o trans=virtio host /mnt/ci'
	@echo 'To copy TAs:'
	@echo '      cp /mnt/ci/*.ta /lib/optee_armtz'
	@echo ''
	@echo ==============
	@echo ''

prep_32: $(TAR_32_RULE)
	mkdir -p ./qemu_32_run
	tar xf $(TAR_32_NAME) -C ./qemu_32_run --strip-components=1

prep_64: $(TAR_64_RULE)
	mkdir -p ./qemu_64_run
	tar xf $(TAR_64_NAME) -C ./qemu_64_run --strip-components=1

run_32: prep_32 instructions
	cd qemu_32_run
	./qemu-system-arm \
	-nographic \
	-serial pty -serial pty \
	-smp 1 \
	-machine virt -machine secure=on -cpu cortex-a15 \
	-d unimp -semihosting-config enable,target=native \
	-m 1057 \
	-bios bl1.bin \
	-fsdev local,id=fsdev0,path=$(QEMU_MOUNT_DIR),security_model=none -device virtio-9p-device,fsdev=fsdev0,mount_tag=host

run_64: prep_64 instructions
	cd qemu_64_run
	./qemu-system-aarch64 \
	-nographic \
	-serial pty -serial pty \
	-smp 1 \
	-machine virt -machine secure=on -cpu cortex-a57 \
	-d unimp -semihosting-config enable,target=native \
	-m 1057 \
	-bios bl1.bin \
	-initrd rootfs.cpio.gz \
	-kernel Image -no-acpi \
	-append 'console=ttyAMA0,38400 keep_bootcon root=/dev/vda2' \
	-virtfs local,id=sh0,path=$(QEMU_MOUNT_DIR),security_model=passthrough,readonly,mount_tag=sh0

TESTS_32 = authvars_test_32 ftpm_test_32
TESTS_64 = authvars_test_64 ftpm_test_64
test_32: prep_32 $(TESTS_32)
	@echo Done all 32 bit tests.
test_64: prep_64 $(TEST_64)
	@echo Done all 64 bit tests.

.PHONY: $(TESTS_32) $(TESTS_64)

TEST_SH = run_$@.sh
$(TESTS_32): prep_32
	cd qemu_32_run
	chmod a+x ../../$(TEST_SH)
	export QEMU_MOUNT_DIR=$(QEMU_MOUNT_DIR)
	../../$(TEST_SH)

$(TESTS_64): prep_64
	cd qemu_64_run
	chmod a+x ../../$(TEST_SH)
	export QEMU_MOUNT_DIR=$(QEMU_MOUNT_DIR)
	../../$(TEST_SH)

.PHONY: tar_32 tar_64
tar_32: $(TAR_32_NAME)
tar_64: $(TAR_64_NAME)

$(TAR_32_NAME): build_32 
	mkdir -p qemu_32_tar
	cp qemu_32/out/bin/bl1.bin qemu_32_tar/ 
	cp qemu_32/out/bin/bl2.bin qemu_32_tar/ 
	cp qemu_32/out/bin/bl32.bin qemu_32_tar/ 
	cp qemu_32/out/bin/bl32_extra1.bin qemu_32_tar/ 
	cp qemu_32/out/bin/bl32_extra2.bin qemu_32_tar/
	cp qemu_32/out/bin/bl33.bin qemu_32_tar/ 
	cp qemu_32/linux/arch/arm/boot/zImage qemu_32_tar/ 
	cp qemu_32/qemu/pc-bios/efi-virtio.rom qemu_32_tar/
	cp qemu_32/arm-trusted-firmware/build/qemu/debug/fip.bin qemu_32_tar/ 
	cp qemu_32/qemu/arm-softmmu/qemu-system-arm qemu_32_tar/ 
	cp qemu_32/out-br/images/rootfs.cpio.gz qemu_32_tar/
	tar -zcvf $(TAR_32_NAME) qemu_32_tar

$(TAR_64_NAME): build_64
	mkdir -p qemu_64_tar
	cp qemu_64/out/bin/bl1.bin qemu_64_tar/ 
	cp qemu_64/out/bin/bl2.bin qemu_64_tar/ 
	cp qemu_64/out/bin/bl31.bin qemu_64_tar/ 
	cp qemu_64/out/bin/bl32.bin qemu_64_tar/ 
	cp qemu_64/out/bin/bl32_extra1.bin qemu_64_tar/ 
	cp qemu_64/out/bin/bl32_extra2.bin qemu_64_tar/
	cp qemu_64/out/bin/bl33.bin qemu_64_tar/ 
	cp qemu_64/linux/arch/arm64/boot/Image qemu_64_tar/
	cp qemu_64/qemu/pc-bios/efi-virtio.rom qemu_64_tar/
	cp qemu_64/arm-trusted-firmware/build/qemu/release/fip.bin qemu_64_tar/ 
	cp qemu_64/qemu/aarch64-softmmu/qemu-system-aarch64 qemu_64_tar/ 
	cp qemu_64/out-br/images/rootfs.cpio.gz qemu_64_tar/
	tar -zcvf $(TAR_64_NAME) qemu_64_tar

.PHONY: build_32 build_64 download_32 download_64 optee_32 optee_64

build_32: download_32
	cd qemu_32/build
	$(MAKE)

build_64: download_64
	cd qemu_64/build
	$(MAKE)

optee_32: download_32
	cd qemu_32/build
	$(MAKE) optee-client-common

optee_64: download_64
	cd qemu_64/build
	$(MAKE) optee-client-common

download_32: google_repo/repo
	mkdir -p qemu_32
	cd qemu_32
	../google_repo/repo init -u https://github.com/OP-TEE/manifest.git -m default.xml
	../google_repo/repo sync -j50
	cd build
	$(MAKE) toolchains

download_64: google_repo/repo
	mkdir -p qemu_64
	cd qemu_64
	../google_repo/repo init -u https://github.com/OP-TEE/manifest.git -m qemu_v8.xml
	../google_repo/repo sync -j50 --no-clone-bundle
	cd build
	$(MAKE) toolchains

google_repo/repo:
	mkdir -p google_repo
	curl https://storage.googleapis.com/git-repo-downloads/repo > google_repo/repo
	chmod a+x google_repo/repo

.PHONY: clean clean_32 clean_64

clean_32:
	rm -rf qemu_32
	rm -rf qemu_32_tar
	rm -rf qemu_32_run
	rm -rf google_repo
	rm -f ../qemu_32.tar.gz

clean_64:
	rm -rf qemu_64
	rm -rf qemu_64_tar
	rm -rf qemu_64_run
	rm -rf google_repo
	rm -f ../qemu_64.tar.gz
