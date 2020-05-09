remap_annon
===========

Remaps the pages, which refer to the file to which this code is associated, to anonymous pages.

**x86_64 only**

How to use
----------

As an example, I'll use a shared library that will be loaded using LD_PRELOAD.

example.c:
```c
#include <stdio.h>

static __attribute__((constructor)) void init_method(void)
{
	printf("example.so: *malicious code loaded*\n");
}
```

Building example.so **without** remap_annon.o:
```
$ gcc -fPIC -shared -o example.so example.c
```

Testing:
```bash
$ LD_PRELOAD='./example.so' cat /proc/self/maps
example.so: *malicious code loaded*
55cc8fe89000-55cc8fe8b000 r--p 00000000 fe:02 3935617                    /usr/bin/cat
55cc8fe8b000-55cc8fe8f000 r-xp 00002000 fe:02 3935617                    /usr/bin/cat
55cc8fe8f000-55cc8fe92000 r--p 00006000 fe:02 3935617                    /usr/bin/cat
55cc8fe92000-55cc8fe93000 r--p 00008000 fe:02 3935617                    /usr/bin/cat
55cc8fe93000-55cc8fe94000 rw-p 00009000 fe:02 3935617                    /usr/bin/cat
55cc91508000-55cc91529000 rw-p 00000000 00:00 0                          [heap]
7fb579e10000-7fb57a0f7000 r--p 00000000 fe:02 3976050                    /usr/lib/locale/locale-archive
7fb57a0f7000-7fb57a0fa000 rw-p 00000000 00:00 0
7fb57a0fa000-7fb57a11f000 r--p 00000000 fe:02 3935649                    /usr/lib/libc-2.31.so
7fb57a11f000-7fb57a26b000 r-xp 00025000 fe:02 3935649                    /usr/lib/libc-2.31.so
7fb57a26b000-7fb57a2b6000 r--p 00171000 fe:02 3935649                    /usr/lib/libc-2.31.so
7fb57a2b6000-7fb57a2b9000 r--p 001bb000 fe:02 3935649                    /usr/lib/libc-2.31.so
7fb57a2b9000-7fb57a2bc000 rw-p 001be000 fe:02 3935649                    /usr/lib/libc-2.31.so
7fb57a2bc000-7fb57a2c0000 rw-p 00000000 00:00 0
7fb57a2df000-7fb57a301000 rw-p 00000000 00:00 0
7fb57a301000-7fb57a302000 r--p 00000000 fe:03 49546956                   /home/test/example.so
7fb57a302000-7fb57a303000 r-xp 00001000 fe:03 49546956                   /home/test/example.so
7fb57a303000-7fb57a304000 r--p 00002000 fe:03 49546956                   /home/test/example.so
7fb57a304000-7fb57a305000 r--p 00002000 fe:03 49546956                   /home/test/example.so
7fb57a305000-7fb57a306000 rw-p 00003000 fe:03 49546956                   /home/test/example.so
7fb57a306000-7fb57a308000 rw-p 00000000 00:00 0
7fb57a308000-7fb57a30a000 r--p 00000000 fe:02 3935608                    /usr/lib/ld-2.31.so
7fb57a30a000-7fb57a32a000 r-xp 00002000 fe:02 3935608                    /usr/lib/ld-2.31.so
7fb57a32a000-7fb57a332000 r--p 00022000 fe:02 3935608                    /usr/lib/ld-2.31.so
7fb57a333000-7fb57a334000 r--p 0002a000 fe:02 3935608                    /usr/lib/ld-2.31.so
7fb57a334000-7fb57a335000 rw-p 0002b000 fe:02 3935608                    /usr/lib/ld-2.31.so
7fb57a335000-7fb57a336000 rw-p 00000000 00:00 0
7ffc394c7000-7ffc394e8000 rw-p 00000000 00:00 0                          [stack]
7ffc39569000-7ffc3956d000 r--p 00000000 00:00 0                          [vvar]
7ffc3956d000-7ffc3956f000 r-xp 00000000 00:00 0                          [vdso]
ffffffffff600000-ffffffffff601000 --xp 00000000 00:00 0                  [vsyscall]
```

As you can see, the example.so file (/home/test/example.so) are visible, now let's try use remap_annon.o and see what happens...

Building example.so **with** remap_annon.o:
```
$ gcc -fPIC -shared -o example.so remap_annon.o example.c
```

Testing:
```bash
$ LD_PRELOAD='./example.so' cat /proc/self/maps
example.so: *malicious code loaded*
55c52e0e1000-55c52e0e3000 r--p 00000000 fe:02 3935617                    /usr/bin/cat
55c52e0e3000-55c52e0e7000 r-xp 00002000 fe:02 3935617                    /usr/bin/cat
55c52e0e7000-55c52e0ea000 r--p 00006000 fe:02 3935617                    /usr/bin/cat
55c52e0ea000-55c52e0eb000 r--p 00008000 fe:02 3935617                    /usr/bin/cat
55c52e0eb000-55c52e0ec000 rw-p 00009000 fe:02 3935617                    /usr/bin/cat
55c52e3d2000-55c52e3f3000 rw-p 00000000 00:00 0                          [heap]
7fdfe4a9e000-7fdfe4d85000 r--p 00000000 fe:02 3976050                    /usr/lib/locale/locale-archive
7fdfe4d85000-7fdfe4d88000 rw-p 00000000 00:00 0
7fdfe4d88000-7fdfe4dad000 r--p 00000000 fe:02 3935649                    /usr/lib/libc-2.31.so
7fdfe4dad000-7fdfe4ef9000 r-xp 00025000 fe:02 3935649                    /usr/lib/libc-2.31.so
7fdfe4ef9000-7fdfe4f44000 r--p 00171000 fe:02 3935649                    /usr/lib/libc-2.31.so
7fdfe4f44000-7fdfe4f47000 r--p 001bb000 fe:02 3935649                    /usr/lib/libc-2.31.so
7fdfe4f47000-7fdfe4f4a000 rw-p 001be000 fe:02 3935649                    /usr/lib/libc-2.31.so
7fdfe4f4a000-7fdfe4f4e000 rw-p 00000000 00:00 0
7fdfe4f6d000-7fdfe4f8f000 rw-p 00000000 00:00 0
7fdfe4f8f000-7fdfe4f90000 r--p 00000000 00:00 0
7fdfe4f90000-7fdfe4f91000 r-xp 00000000 00:00 0
7fdfe4f91000-7fdfe4f93000 r--p 00000000 00:00 0
7fdfe4f93000-7fdfe4f96000 rw-p 00000000 00:00 0
7fdfe4f96000-7fdfe4f98000 r--p 00000000 fe:02 3935608                    /usr/lib/ld-2.31.so
7fdfe4f98000-7fdfe4fb8000 r-xp 00002000 fe:02 3935608                    /usr/lib/ld-2.31.so
7fdfe4fb8000-7fdfe4fc0000 r--p 00022000 fe:02 3935608                    /usr/lib/ld-2.31.so
7fdfe4fc1000-7fdfe4fc2000 r--p 0002a000 fe:02 3935608                    /usr/lib/ld-2.31.so
7fdfe4fc2000-7fdfe4fc3000 rw-p 0002b000 fe:02 3935608                    /usr/lib/ld-2.31.so
7fdfe4fc3000-7fdfe4fc4000 rw-p 00000000 00:00 0
7ffd43cbe000-7ffd43cdf000 rw-p 00000000 00:00 0                          [stack]
7ffd43d3b000-7ffd43d3f000 r--p 00000000 00:00 0                          [vvar]
7ffd43d3f000-7ffd43d41000 r-xp 00000000 00:00 0                          [vdso]
ffffffffff600000-ffffffffff601000 --xp 00000000 00:00 0                  [vsyscall]
```

The maps that refer to /home/test/example.so are now anonymous!

![](https://gifimage.net/wp-content/uploads/2017/10/ninja-smoke-bomb-gif-9.gif)

Contributing
------------
You can help with code, or donating money.
If you wanna help with code, use the kernel code style as a reference.

Paypal: [![](https://www.paypalobjects.com/en_US/i/btn/btn_donate_SM.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=RAG26EKAYHQSY&currency_code=BRL&source=url)

BTC: 1PpbrY6j1HNPF7fS2LhG9SF2wtyK98GSwq
