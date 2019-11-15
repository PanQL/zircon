build: config
	cd out && ../buildtools/ninja

config:
	buildtools/gn gen --export-compile-commands=x64 out

run:
	./scripts/run-zircon-x64 -z ./out/legacy-image-x64.zbi -t ./out/multiboot.bin

runarm:
	./scripts/run-zircon-arm64 -z ./out/legacy-image-arm64.zbi -t ./out/qemu-boot-shim.bin

clean:
	buildtools/gn clean out
