Newlib porting documentation
============================
# General documentation
The port has been based on the 4.4.0 version of cygqin-newlib.

## Crt0:
The crt0 should:
1. Correctly set the stack pointer
2. Clear the BSS segment by zeroing it out
3. If the .data section is stored in some flash memory, copy the contents to the proper memory location in the RAM
4. Do any initialization that may be necessary
5. After everything is ready, jump to main

## Linker script:
It should provide a correct description of the memory layout and the necessary symbols that will enable the crt0 to properly set up the runtime. Additionally, the linker script shoud provide the proper infrastructure for dynamic memory allocation, as most of the library functions rely in some sort of dynamic memory allocation. If this is not done properly, they will fail.

## Reentrancy:
For a single-threaded, bare metal application, reentrancy is not required.

Documentation is provided in the newlib/libc/include/reent.h file. For our use case, we provide the _* named version of the syscalls (namespace clean versions) and set the `syscall_dir=syscalls`, as described in the 2nd configuration option in the `reent.h` file.

Note that reentrancy will fail if the linker script and the crt0 are not properly set, as it depends on the `struct _reent`, whose initial data are saved on the .data section and are copied to memory when the runtime is set up.

**This port is not reentrant and thus usage of threads strongly discouraged!** In order to be able to use threads, the reentrant verion of the syscalls should be implemented.

# Syscalls:
A minimal set of syscalls should be implemented so that the basic IO functions can operate (printf etc).
The syscalls are provided by libgloss. The newlib supplied syscalls should be disabled.

## _write:
Write a string of size len from ptr to the file specifier specified by file. If file is stdout that write to UART.

## _exit:
Trap execution in infinite loop.

## _fstat:
Check status of file.

## _isatty:
Return true if the file descriptor is a tty (teletype)

## _read:
That one is a bit more complicated: If reading from a file, `_read` should read the number of bytes specified by len, unless there are not enough bytes left. In that case, it should return as many bytes are left.

In case we are reading from `stdin`, there is no clear specification on how it should be done.

Personal understanding: Read from input until `\n` or `\r` is encountered or until we have completely popuplated the buffer. Append `\0` at the end of buffer. Buffer size is usually about 1024 bytes.

## _sbrk:
Tries to increase heap size by moving the top of the heap. If the heap is preallocated using the linker script, the syscall will always fail.

# Configuration
In order to build newlib with libgloss run the following script:

	../src/configure --target=riscv64-unknown-elf\
				--prefix=/opt/headsail-newlib/cygwin-newlib/\
				--disable-newlib-supplied-syscalls\
				--disable-newlib-multithread\
				CFLAGS_FOR_TARGET="${CFLAGS_FOR_TARGET} -march=rv64imac -nostdlib -mabi=lp64 -mcmodel=medany -g -O0 -Fuse-ld=riscv64-unknown-elf-ld" 
	make 
	make install

## Flags
* `--target=riscv64-unknown-elf`: Target sets the architecture that we want to build for
* `--prefix=/opt/headsail-newlib/cygwin-newlib/`: Installation path
* `--disable-newlib-supplied-syscalls`: Tells the build system to get the syscalls from libgloss instead of newlib.
* `--disable-newlib-multithread`: Disables multithreading support for newlib

## Building
In order to rebuild the library after making changes, if the configuration options have not been changed, a simple `make` and `make install` is enough. To completely clean the compilation artifacts there is the `make clean` recipe, and in order to clean the configuration settings as well as the build artifacts one can run `make distclean`.

# Contributions
Andreas Stergiopoulos <andreas.stergiopoulos@tuni.fi> (Porting and documentation)  
Väinö-Waltteri Granat <vaino-waltteri.granat@tuni.fi> (Porting)  
Copyright (c) 2024 SoCHub Finland.