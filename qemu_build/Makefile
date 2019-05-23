.ONESHELL:
SHELL = bash

QEMU_MOUNT_DIR?=~/qemu_mnt

.PHONY: all test update run_32 run_64
all: update
update: tar_32 tar_64
test: test_32 test_64

TAR_32_NAME=../qemu_32.tar.gz
TAR_64_NAME=../qemu_64.tar.gz
TAR_32_RULE=$(if $(wildcard $(TAR_32_NAME)),,$(TAR_32_NAME))
TAR_64_RULE=$(if $(wildcard $(TAR_64_NAME)),,$(TAR_64_NAME))

.PHONY: run_32 run_64 test_32 test_64
run_32: $(TAR_32_RULE)
	mkdir -p ./qemu_32_run
	tar xf $(TAR_32_NAME) -C ./qemu_32_run --strip-components=1
	cd qemu_32_run
	./qemu-system-arm \
	-nographic \
	-serial pty -serial pty \
	-smp 1 \
	-machine virt -machine secure=on -cpu cortex-a15 \
	-d unimp -semihosting-config enable,target=native \
	-m 1057 \
	-bios bl1.bin \
	-virtfs local,id=sh0,path=/home/,security_model=passthrough,readonly,mount_tag=sh0

test_32: $(TAR_32_RULE)
	mkdir -p ./qemu_32_run
	tar xf $(TAR_32_NAME) -C ./qemu_32_run --strip-components=1
	cd qemu_32_run
	chmod a+x ../../run_ftpm_test_32.sh
	export QEMU_MOUNT_DIR=$(QEMU_MOUNT_DIR)
	../../run_ftpm_test_32.sh

run_64: $(TAR_64_RULE)
	mkdir -p ./qemu_64_run
	tar xf $(TAR_64_NAME) -C ./qemu_64_run --strip-components=1
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
	-virtfs local,id=sh0,path=/home/,security_model=passthrough,readonly,mount_tag=sh0

test_64: $(TAR_64_RULE)
	mkdir -p ./qemu_64_run
	tar xf $(TAR_64_NAME) -C ./qemu_64_run --strip-components=1
	cd qemu_64_run
	chmod a+x ../../run_ftpm_test_64.sh
	export QEMU_MOUNT_DIR=$(QEMU_MOUNT_DIR)
	../../run_ftpm_test_64.sh

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

.PHONY: build_32 build_64
build_32: google_repo/repo
	mkdir -p qemu_32
	cd qemu_32
	../google_repo/repo init -u https://github.com/OP-TEE/manifest.git -m default.xml
	../google_repo/repo sync -j10
	cd build
	$(MAKE) toolchains
	$(MAKE)

build_64: google_repo/repo
	mkdir -p qemu_64
	cd qemu_64
	../google_repo/repo init -u https://github.com/OP-TEE/manifest.git -m qemu_v8.xml
	../google_repo/repo sync -j10 --no-clone-bundle
	cd build
	$(MAKE) toolchains
	$(MAKE)

google_repo/repo:
	mkdir -p google_repo
	curl https://storage.googleapis.com/git-repo-downloads/repo > google_repo/repo
	chmod a+x google_repo/repo

.PHONY: clean
clean:
	rm -rf qemu_32
	rm -rf qemu_32_tar
	rm -rf qemu_32_run
	rm -rf qemu_64
	rm -rf qemu_64_tar
	rm -rf qemu_64_run
	rm -rf google_repo
