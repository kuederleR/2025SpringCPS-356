SHELL=/usr/bin/bash
KOBJS = \
	kkernel/kbio.o\
	kkernel/kexec.o\
	kkernel/kfile.o\
	kkernel/kfs.o\
	kkernel/kide.o\
	kkernel/kioapic.o\
	kkernel/kalloc.o\
	kkernel/kkbd.o\
	kkernel/klapic.o\
	kkernel/klog.o\
	kkernel/kmain.o\
	kkernel/kmp.o\
	kkernel/kpicirq.o\
	kkernel/kpipe.o\
	kkernel/kproc.o\
	kkernel/kshutdown.o\
	kkernel/ksleeplock.o\
	kkernel/kspinlock.o\
	kkernel/kswtch.o\
	kkernel/ktrapasm.o\
	kkernel/ktrap.o\
	kkernel/kuart.o\
	kkernel/kvectors.o\
	kkernel/kvm.o\
	kkernel/syscall.o\
	kkernel/sysfile.o\
	kkernel/sysproc.o\
	kkernel/dconsole.o\
	kkernel/dhello.o\
	kkernel/dqueue.o\
	string.o\

# Cross-compiling (e.g., on Mac OS X)
# TOOLPREFIX = i386-jos-elf

# Using native tools (e.g., on X86 Linux)
#TOOLPREFIX = 

# Try to infer the correct TOOLPREFIX if not set
ifndef TOOLPREFIX
TOOLPREFIX := $(shell if i386-jos-elf-objdump -i 2>&1 | grep '^elf32-i386$$' >/dev/null 2>&1; \
	then echo 'i386-jos-elf-'; \
	elif objdump -i 2>&1 | grep 'elf32-i386' >/dev/null 2>&1; \
	then echo ''; \
	else echo "***" 1>&2; \
	echo "*** Error: Couldn't find an i386-*-elf version of GCC/binutils." 1>&2; \
	echo "*** Is the directory with i386-jos-elf-gcc in your PATH?" 1>&2; \
	echo "*** If your i386-*-elf toolchain is installed with a command" 1>&2; \
	echo "*** prefix other than 'i386-jos-elf-', set your TOOLPREFIX" 1>&2; \
	echo "*** environment variable to that prefix and run 'make' again." 1>&2; \
	echo "*** To turn off this error, run 'gmake TOOLPREFIX= ...'." 1>&2; \
	echo "***" 1>&2; exit 1; fi)
endif

# If the makefile can't find QEMU, specify its path here
# QEMU = qemu-system-i386

# Try to infer the correct QEMU
ifndef QEMU
QEMU = $(shell if which qemu > /dev/null; \
	then echo qemu; exit; \
	elif which qemu-system-i386 > /dev/null; \
	then echo qemu-system-i386; exit; \
	elif which qemu-system-x86_64 > /dev/null; \
	then echo qemu-system-x86_64; exit; \
	else \
	qemu=/Applications/Q.app/Contents/MacOS/i386-softmmu.app/Contents/MacOS/i386-softmmu; \
	if test -x $$qemu; then echo $$qemu; exit; fi; fi; \
	echo "***" 1>&2; \
	echo "*** Error: Couldn't find a working QEMU executable." 1>&2; \
	echo "*** Is the directory containing the qemu binary in your PATH" 1>&2; \
	echo "*** or have you tried setting the QEMU variable in Makefile?" 1>&2; \
	echo "***" 1>&2; exit 1)
endif

CC = $(TOOLPREFIX)gcc
AS = $(TOOLPREFIX)as
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump
CFLAGS = -I. -I./kkernel -fno-pic -static -fno-builtin -fno-strict-aliasing -O2 -Wall -MD -ggdb -m32 -Werror -fno-omit-frame-pointer -Wno-array-bounds -Wno-infinite-recursion
CFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)
ASFLAGS = -I. -I./kkernel -m32 -gdwarf-2 -Wa,-divide
# FreeBSD ld wants ``elf_i386_fbsd''
LDFLAGS += -z noexecstack -m $(shell $(LD) -V | grep elf_i386 2>/dev/null | head -n 1)

# Disable PIE when possible (for Ubuntu 16.10 toolchain)
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]no-pie'),)
CFLAGS += -fno-pie -no-pie
endif
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]nopie'),)
CFLAGS += -fno-pie -nopie
endif

xv6.img: bootblock kernel
	dd if=/dev/zero of=xv6.img count=10000
	dd if=bootblock of=xv6.img conv=notrunc
	dd if=kernel of=xv6.img seek=1 conv=notrunc

xv6memfs.img: bootblock kernelmemfs
	dd if=/dev/zero of=memfs.img count=10000
	dd if=bootblock of=memfs.img conv=notrunc
	dd if=kernelmemfs of=memfs.img seek=1 conv=notrunc

bootblock: kkernel/bootasm.S kkernel/bootmain.c
	$(CC) $(CFLAGS) -fno-pic -O -nostdinc -I. -o kkernel/bootmain.o -c kkernel/bootmain.c
	$(CC) $(CFLAGS) -fno-pic -nostdinc -I. -o kkernel/bootasm.o -c kkernel/bootasm.S
	$(LD) $(LDFLAGS) -N -e start -Ttext 0x7C00 -o kkernel/bootblock.o kkernel/bootasm.o kkernel/bootmain.o
	$(OBJDUMP) -S kkernel/bootblock.o > kkernel/bootblock.asm
	$(OBJCOPY) -S -O binary -j .text kkernel/bootblock.o bootblock
	./sign.pl bootblock

entryother: kkernel/kentryother.S
	$(CC) $(CFLAGS) -fno-pic -nostdinc -I. -c kkernel/kentryother.S -o kkernel/kentryother.o
	$(LD) $(LDFLAGS) -N -e start -Ttext 0x7000 -o kkernel/bootblockother.o kkernel/kentryother.o
	$(OBJCOPY) -S -O binary -j .text kkernel/bootblockother.o entryother
	$(OBJDUMP) -S kkernel/bootblockother.o > kkernel/kentryother.asm

initcode: kkernel/initcode.S
	$(CC) $(CFLAGS) -nostdinc -I. -o kkernel/initcode.o -c kkernel/initcode.S
	$(LD) $(LDFLAGS) -N -e start -Ttext 0 -o kkernel/initcode.out kkernel/initcode.o
	$(OBJCOPY) -S -O binary kkernel/initcode.out initcode
	$(OBJDUMP) -S kkernel/initcode.o > kkernel/initcode.asm

kernel: $(KOBJS) kkernel/kentry.o entryother initcode kernel.ld
	$(LD) $(LDFLAGS) -T kernel.ld -o kernel kkernel/kentry.o $(KOBJS) -b binary initcode entryother
	$(OBJDUMP) -S kernel > kernel.asm
	$(OBJDUMP) -t kernel | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > kernel.sym

# kernelmemfs is a copy of kernel that maintains the
# disk image in memory instead of writing to a disk.
# This is not so useful for testing persistent storage or
# exploring disk buffering implementations, but it is
# great for testing the kernel on real hardware without
# needing a scratch disk.
MEMFSOBJS = $(filter-out kide.o,$(KOBJS)) kkernel/memide.o
kernelmemfs: $(MEMFSOBJS) kkernel/kentry.o kentryother initcode kernel.ld fs.img
	$(LD) $(LDFLAGS) -T kernel.ld -o kernelmemfs kkernel/kentry.o  $(MEMFSOBJS) -b binary initcode kentryother fs.img
	$(OBJDUMP) -S kernelmemfs > kernelmemfs.asm
	$(OBJDUMP) -t kernelmemfs | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > kernelmemfs.sym

tags: $(KOBJS) kentryother.S _init
	etags *.S *.c

kkernel/kvectors.S: vectors.pl
	./vectors.pl > kkernel/kvectors.S

# Prevent deletion of intermediate files, e.g. cat.o, after first build, so
# that disk image changes after first build are persistent until clean.  More
# details:
# http://www.gnu.org/software/make/manual/html_node/Chained-Rules.html
.PRECIOUS: %.o

# mkfs is a Linux program that creates the virtual disk images (*.img) used
# by xv6
mkfs: mkfs.c fs.h
	gcc -Werror -Wall -o mkfs mkfs.c

fs.img: mkfs README
	@ rm -rf userbin
	@ mkdir userbin
	pushd user; make all; make fs; popd 

-include *.d

DIRS=\
	. \
	kkernel \
	user \

clean: 
	@ rm -rf userbin
	@ mkdir userbin
	@ for dir in $(DIRS); do \
		pushd $$dir ; \
		rm -f *.tex *.dvi *.idx *.aux *.log *.ind *.ilg \
			*.o *.d *.asm *.sym kvectors.S bootblock kentryother \
			initcode initcode.out kernel xv6.img fs.img kernelmemfs \
			memfs.img mkfs .gdbinit ; \
		popd ; \
	done
	@ rm -rf userbin/*

# make a printout
FILES = $(shell grep -v '^\#' runoff.list)
PRINT = runoff.list runoff.spec README toc.hdr toc.ftr $(FILES)

xv6.pdf: $(PRINT)
	./runoff
	ls -l xv6.pdf

print: xv6.pdf

# run in emulators

bochs : fs.img xv6.img
	if [ ! -e .bochsrc ]; then ln -s dot-bochsrc .bochsrc; fi
	bochs -q

# try to generate a unique GDB port
GDBPORT = $(shell expr `id -u` % 5000 + 25000)
# QEMU's gdb stub command line changed in 0.11
QEMUGDB = $(shell if $(QEMU) -help | grep -q '^-gdb'; \
	then echo "-gdb tcp::$(GDBPORT)"; \
	else echo "-s -p $(GDBPORT)"; fi)
ifndef CPUS
CPUS := 2
endif
QEMUOPTS = -drive file=fs.img,index=1,media=disk,format=raw -drive file=xv6.img,index=0,media=disk,format=raw -smp $(CPUS) -m 512 $(QEMUEXTRA)

qemu: fs.img xv6.img
	$(QEMU) -device isa-debug-exit -serial mon:stdio $(QEMUOPTS)

qemu-memfs: xv6memfs.img
	$(QEMU) -device isa-debug-exit -drive file=xv6memfs.img,index=0,media=disk,format=raw -smp $(CPUS) -m 256

qemu-nox: fs.img xv6.img
	$(QEMU) -device isa-debug-exit -nographic $(QEMUOPTS)

.gdbinit: .gdbinit.tmpl
	sed "s/localhost:1234/localhost:$(GDBPORT)/" < $^ > $@

qemu-gdb: fs.img xv6.img .gdbinit
	@echo "*** Now run 'gdb'." 1>&2
	$(QEMU) -device isa-debug-exit -serial mon:stdio $(QEMUOPTS) -S $(QEMUGDB)

qemu-nox-gdb: fs.img xv6.img .gdbinit
	@echo "*** Now run 'gdb'." 1>&2
	$(QEMU) -device isa-debug-exit -nographic $(QEMUOPTS) -S $(QEMUGDB)

# CUT HERE
# prepare dist for students
# after running make dist, probably want to
# rename it to rev0 or rev1 or so on and then
# check in that version.

EXTRA=\
	mkfs.c ulib.c user.h cat.c echo.c testfork.c grep.c kill.c\
	ln.c ls.c mkdir.c rm.c testfs.c testuser.c wc.c zombie.c\
	printf.c malloc.c\
	README dot-bochsrc *.pl toc.* runoff runoff1 runoff.list\
	.gdbinit.tmpl gdbutil\

dist:
	rm -rf dist
	mkdir dist
	for i in $(FILES); \
	do \
		grep -v PAGEBREAK $$i >dist/$$i; \
	done
	sed '/CUT HERE/,$$d' Makefile >dist/Makefile
	echo >dist/runoff.spec
	cp $(EXTRA) dist

dist-test:
	rm -rf dist
	make dist
	rm -rf dist-test
	mkdir dist-test
	cp dist/* dist-test
	cd dist-test; $(MAKE) print
	cd dist-test; $(MAKE) bochs || true
	cd dist-test; $(MAKE) qemu

# update this rule (change rev#) when it is time to
# make a new revision.
tar:
	rm -rf /tmp/xv6
	mkdir -p /tmp/xv6
	cp dist/* dist/.gdbinit.tmpl /tmp/xv6
	(cd /tmp; tar cf - xv6) | gzip >xv6-rev10.tar.gz  # the next one will be 10 (9/17)

.PHONY: dist-test dist
