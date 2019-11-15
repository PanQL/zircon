# Zircon  
这是从Fuchsia官方的20190715版本代码仓库中分离出来的Zircon代码，用于进行Zircon内核学习。可以通过简陋的Makefile脚本进行编译并在qemu上运行。  
目前支持在ubuntu 18.04和Mac上编译运行。  
一些我们关于zircon的分析放在[这里](https://github.com/PanQL/zircon-notes)，欢迎补充！  
### 关于代码分析工具  
* [这里](https://fuchsia.googlesource.com/fuchsia/+/refs/heads/master/docs/development/languages/c-cpp/editors.md)是适用于fuchsia仓库的文档(需要科学上网)。我们使用vscode，相关的vscode设置在仓库中的.vscode目录，希望有所帮助。  
* 解析代码依赖的compile_commands.json将会随build过程生成到out文件夹，建议先成功build再用vscode打开zircon目录。  
### 关于编译运行  
为了减小仓库体积，我们将prebuilt目录进行了大幅调整;因此运行之前请下载google预编译好的clang，解压后放到某个权限合适的位置，然后在代码的[这个位置](https://github.com/PanQL/zircon/blob/master/public/gn/toolchain/clang.gni#L16)将**绝对目录**修改为对应位置。  
* [云盘下载链接](https://cloud.tsinghua.edu.cn/d/7ab1d87feecd4b2cb3d8/)  
* 官方CIPD包下载链接如下  
    * [Linux](https://chrome-infra-packages.appspot.com/p/fuchsia/clang/linux-amd64/+/oEsFSe99FkcDKVxZkAY0MKi6C-yYOan1m-QL45N33W8C)  
    * [Mac](https://chrome-infra-packages.appspot.com/p/fuchsia/clang/mac-amd64/+/Lc64-GTi4kihzkCnW8Vaa80TWTnMpZY0Fy6AqChmqvcC)  

当前只支持在Mac OS及Linux x64上进行编译。  

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
