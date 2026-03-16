TARGET = dump_device_state
LINUX_SRC ?= /home/antoni/Documents/linux

ARCH ?= x86
CROSS_COMPILE ?=

BPF_OBJ = ${TARGET:=.bpf.o}
USER_C = ${TARGET:=.c}
USER_SKEL = ${TARGET:=.skel.h}

all: $(TARGET)

$(TARGET): $(USER_C) $(USER_SKEL)
	$(CROSS_COMPILE)$(CC) -o $@ $(USER_C) \
		-Wall \
		-I ./include \
		-I$(LINUX_SRC)/tools/bpf/bpftool/libbpf/include \
		-L$(LINUX_SRC)/tools/bpf/bpftool/libbpf/ \
		-l:libbpf.a -lelf -lz

%.bpf.o: %.bpf.c
	clang -target bpf -D__TARGET_ARCH_$(ARCH) -Wall -O2 -g -I ./include -c $<

$(USER_SKEL): $(BPF_OBJ)
	bpftool gen skeleton $< > $@

clean:
	rm -f *.skel.h *.bpf.o

.PHONY: all
