build: config
	cd out && ../buildtools/ninja

config:
	buildtools/gn gen --export-compile-commands=x64 out

run:
	./scripts/run-zircon-x64 -z ./out/legacy-image-x64.zbi -t ./out/multiboot.bin

runarm:
	./scripts/run-zircon-arm64 -z ./out/legacy-image-arm64.zbi -t ./out/qemu-boot-shim.bin

runrpi:
	qemu-system-aarch64 -kernel ./out/rpi3-boot-shim.bin -initrd ./out/legacy-image-arm64.zbi \
	-display none \
	-serial null -serial mon:stdio \
	-machine raspi3 \
	-dtb ./bcm2710-rpi-3-b-plus.dtb

clean:
	buildtools/gn clean out
