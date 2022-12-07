#clang -g   -target bpf -nostdinc -I/usr/include -I/usr/include/x86_64-linux-gnu/ -I./include -c proba.c -o proba.o

%.kern.o: %.kern.c
	clang -O2 -g -target bpf -nostdinc -I/usr/include  -I./include -c $< -o $@

%.skel.c: %.kern.o
	bpftool gen skeleton $< > $@
	

#cp proba_skel.c ~/src/syslog-ng/syslog-ng/modules/afsocket/

all: reuse.skel.c
