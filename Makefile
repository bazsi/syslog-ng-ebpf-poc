#clang -g   -target bpf -nostdinc -I/usr/include -I/usr/include/x86_64-linux-gnu/ -I./include -c proba.c -o proba.o

%.kern.o: %.kern.c
	clang -O2 -g -target bpf -nostdinc -I/usr/include  -I./bpf-include -c $< -o $@

%.skel.c: %.kern.o
	bpftool gen skeleton $< > $@
	

all: reuse-bpf.so

reuse-bpf.so: attach-reuse-bpf.o
	ld -shared attach-reuse-bpf.o -o reuse-bpf.so -lbpf
