# Zircon  
这是从Fuchsia官方的20190715版本代码仓库中分离出来的Zircon代码，用于进行Zircon内核学习。可以通过简陋的Makefile脚本进行编译并在qemu上运行。  
目前支持在ubuntu 18.04上编译运行，Mac用户可能需要补全prebuilt目录下的mac版本的clang/gcc。（注意，官方clang大概率还不能编译，可考虑拉取google官方修改的prebuilt工具）  
一些我们关于zircon的分析放在[这里](https://github.com/PanQL/zircon-notes)，欢迎补充！  
### 关于代码分析工具  
* [这里](https://fuchsia.googlesource.com/fuchsia/+/refs/heads/master/docs/development/languages/c-cpp/editors.md)是适用于fuchsia仓库的文档(需要科学上网)。我们使用vscode，相关的vscode设置在仓库中的.vscode目录，希望有所帮助。  
* 解析代码依赖的compile_commands.json将会随build过程生成到out文件夹，建议先成功build再用vscode打开zircon目录。  

----  

Zircon is the core platform that powers the Fuchsia OS.  Zircon is
composed of a microkernel (source in kernel/...) as well as a small
set of userspace services, drivers, and libraries (source in system/...)
necessary for the system to boot, talk to hardware, load userspace
processes and run them, etc.  Fuchsia builds a much larger OS on top
of this foundation.

The canonical Zircon Git repository is located
at: https://fuchsia.googlesource.com/zircon

The Zircon Kernel provides syscalls to manage processes, threads,
virtual memory, inter-process communication, waiting on object state
changes, and locking (via futexes).

Currently there are some temporary syscalls that have been used for early
bringup work, which will be going away in the future as the long term
syscall API/ABI surface is finalized.  The expectation is that there will
be about 100 syscalls.

Zircon syscalls are generally non-blocking.  The wait_one, wait_many
port_wait and thread sleep being the notable exceptions.

This page is a non-comprehensive index of the zircon documentation.

+ [Getting Started](docs/getting_started.md)
+ [Contributing Patches](docs/contributing.md)

+ [Concepts Overview](docs/concepts.md)
+ [Kernel Objects](docs/objects.md)
+ [Kernel Scheduling](docs/kernel_scheduling.md)
+ [Process Objects](docs/objects/process.md)
+ [Thread Objects](docs/objects/thread.md)
+ [Handles](docs/handles.md)
+ [System Calls](docs/syscalls.md)

+ [Driver Development Kit](docs/ddk/overview.md)

+ [Testing](docs/testing.md)
+ [Hacking notes](docs/hacking.md)
+ [Memory usage analysis tools](docs/memory.md)
+ [Relationship with LK](docs/zx_and_lk.md)
+ [Micro-benchmarks](docs/benchmarks/microbenchmarks.md)
