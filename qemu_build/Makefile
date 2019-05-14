.ONESHELL:
SHELL = bash

.PHONY: all
all: qemu_32.tar.gz qemu_64.tar.gz

.PHONY: qemu_32.tar.gz qemu_64.targz
qemu_32.tar.gz: build_32
	mkdir -p qemu_32_tar
	cp qemu_32/out/bin/zImage qemu_32_tar/
	cp qemu_32/out/bin/bl1.bin qemu_32_tar/ 
	cp qemu_32/out/bin/bl2.bin qemu_32_tar/ 
	cp qemu_32/out/bin/bl32.bin qemu_32_tar/ 
	cp qemu_32/out/bin/bl32_extra1.bin qemu_32_tar/ 
	cp qemu_32/out/bin/bl32_extra2.bin qemu_32_tar/
	cp qemu_32/out/bin/bl33.bin qemu_32_tar/ 
	cp qemu_32/out/bin/zImage qemu_32_tar/ 
	cp qemu_32/qemu/pc-bios/efi-virtio.rom qemu_32_tar/
	cp qemu_32/arm-trusted-firmware/build/qemu/debug/fip.bin qemu_32_tar/ 
	cp qemu_32/qemu/arm-softmmu/qemu-system-arm qemu_32_tar/ 
	cp qemu_32/out/bin/rootfs.cpio.gz qemu_32_tar/
	tar -zcvf qemu_32.tar.gz ./qemu_32_tar

qemu_64.tar.gz: build_64
	mkdir -p qemu_64_tar
	cp qemu_64/out/bin/zImage qemu_64_tar/
	cp qemu_64/out/bin/bl1.bin qemu_64_tar/ 
	cp qemu_64/out/bin/bl2.bin qemu_64_tar/ 
	cp qemu_64/out/bin/bl32.bin qemu_64_tar/ 
	cp qemu_64/out/bin/bl32_extra1.bin qemu_64_tar/ 
	cp qemu_64/out/bin/bl32_extra2.bin qemu_64_tar/
	cp qemu_64/out/bin/bl33.bin qemu_64_tar/ 
	cp qemu_64/linux/arch/arm/boot/Imag qemu_64_tar/ 
	cp qemu_64/qemu/pc-bios/efi-virtio.rom qemu_64_tar/
	cp qemu_64/arm-trusted-firmware/build/qemu/debug/fip.bin qemu_64_tar/ 
	cp qemu_64/qemu/arm-softmmu/qemu-system-arm qemu_64_tar/ 
	cp qemu_64/out/bin/rootfs.cpio.gz qemu_64_tar/
	tar -zcvf qemu_64.tar.gz ./qemu_64_tar

.PHONY: build_32 build_64
build_32 build_64: google_repo/repo

build_32:
	mkdir -p qemu_32
	cd qemu_32
	../google_repo/repo init -u https://github.com/OP-TEE/manifest.git -m default.xml
	../google_repo/repo sync -j10 --no-clone-bundle
	cd build
	make -j2 toolchains
	make -j10

build_64:
	mkdir -p qemu_64
	cd qemu_64
	../google_repo/repo init -u https://github.com/OP-TEE/manifest.git -m qemu_v8.xml
	../google_repo/repo sync -j10 --no-clone-bundle
	cd build
	make -j2 toolchains
	make -j10

google_repo/repo:
	mkdir -p google_repo
	curl https://storage.googleapis.com/git-repo-downloads/repo > google_repo/
	chmod a+x google_repo/repo

.PHONY: clean
clean:
	rm -rf qemu_32
	rm -rf qemu_32_tar
	rm -rf qemu_64
	rm -rf qemu_64_tar
	rm -rf google_repo
