
LLC=./tools/llc
LLD=./tools/lld
LLVM_MC=./tools/llvm-mc
OBJCOPY=./tools/llvm-objcopy
CLANG=./tools/clang

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# Source Files Configuration. Place all compile targets here. #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

# Basic
_CSRCS = $(wildcard src/*.c)
_LIBSRCS = $(wildcard asmlib/*.s)
_LIBC_SRCS = $(wildcard lib/src/*.c)
_OBJS = rt0.o
_OBJS += $(patsubst lib/src/%.c, lib/%.o, $(_LIBC_SRCS))
_OBJS += $(patsubst src/%.c, %.o, $(_CSRCS))
_OBJS += $(patsubst asmlib/%.s, %.o, $(_LIBSRCS))

ODIR=obj
CFLAGS=-Iinclude -MMD -MP -Wall -Wno-main-return-type -Iinclude -Ilib/include -ffunction-sections -Wno-incompatible-library-redeclaration -Wno-unused-function -fverbose-asm

OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

.PHONY: default deploy build upload sim

default: build

build: $(ODIR)/a.bin

$(ODIR)/rt0.o: rt0.s
	@mkdir -p $(ODIR)
	@mkdir -p $(ODIR)/lib
	$(LLVM_MC) --filetype obj -triple=z80-unknown-unknown-code16 $< -o $@

$(ODIR)/%.o: asmlib/%.s
	@mkdir -p $(@D)
	$(LLVM_MC) --filetype obj -triple=z80-unknown-none-code16 $< -o $@

$(ODIR)/%.o: src/%.c
	mkdir -p $(@D)
	@$(CLANG) --target=z80-unknown-none-code16 -fintegrated-as -O3 -c $(CFLAGS) -Wno-everything $< -S -emit-llvm -o $@.ll
	$(CLANG) --target=z80-unknown-none-code16 -fintegrated-as -O3 -c $(CFLAGS) $< -S -o $@.s
	@$(CLANG) --target=z80-unknown-none-code16 -fintegrated-as -O3 -c $(CFLAGS) -Wno-everything $< -o $@

$(ODIR)/lib/%.o: lib/src/%.c
	mkdir -p $(@D)
	@$(CLANG) --target=z80-unknown-none-code16 -fintegrated-as -O3 -c $(CFLAGS) -Ilib/src/templates -Wno-everything $< -S -emit-llvm -o $@.ll
	$(CLANG) --target=z80-unknown-none-code16 -fintegrated-as -O3 -c $(CFLAGS) -Ilib/src/templates $< -S -o $@.s
	@$(CLANG) --target=z80-unknown-none-code16 -fintegrated-as -O3 -c $(CFLAGS) -Ilib/src/templates -Wno-everything $< -o $@

$(ODIR)/a.elf: linker.ld $(OBJS)
	$(LLD) -flavor gnu --gc-sections -T $^ -o $@
	$(LLD) -flavor gnu -T $^ -o $@.nogc

$(ODIR)/a.bin: $(ODIR)/a.elf
	llvm-objcopy -O binary $^ $@
	llvm-objcopy -O binary $^.nogc $@.nogc

deploy: build
	tl-flash -p /dev/ttyUSB0 $(ODIR)/a.bin

sim: $(ODIR)/a.bin
	./sim $<

clean:
	rm -rf obj/*


-include $(OBJS:.o=.d)
