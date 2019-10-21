build: config
	cd out && ../buildtools/ninja -j9

config:
	buildtools/gn gen --export-compile-commands=x64 out

run:
	buildtools/linux-x64/qemu/bin/qemu-system-x86_64 \
		-kernel out/multiboot.bin \
		-initrd out/x64.zbi \
		-m 2048 \
		-nographic \
		-net none \
		-smp 4,threads=2 \
		-machine q35 \
		-device isa-debug-exit,iobase=0xf4,iosize=0x4 \
		-cpu Haswell,+smap,-check,-fsgsbase \
		-append 'TERM=screen kernel.serial=legacy kernel.entropy-mixin=1ff22352dfe4cf3e447214ba03ca684ddcc4f1957fa1e8d2d140f26392c66875 kernel.halt-on-panic=true'

clean:
	buildtools/gn clean out
