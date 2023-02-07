
#define _GNU_SOURCE 1

#include <sys/types.h>
#include <sys/socket.h>
#include <dlfcn.h>
#include <unistd.h>

#include "reuse.skel.c"


static struct reuse_kern *reuse_kern;
static int (*old_setsockopt)(int fd, int level, int optname, const void *optval, socklen_t optlen);

int
setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
{
  if (level == SOL_SOCKET && optname == SO_REUSEPORT)
    {
      int rc = old_setsockopt(fd, level, optname, optval, optlen);
      
      if (rc >= 0)
        {
          write(2, "foo\n", 4);
          int bpf_fd = bpf_program__fd(reuse_kern->progs.random_choice);
          write(2, "bar\n", 4);
          if (bpf_fd < 0)
            return -1;
          write(2, "baz\n", 4);
          if (old_setsockopt(fd, SOL_SOCKET, SO_ATTACH_REUSEPORT_EBPF, &bpf_fd, sizeof(bpf_fd)) < 0)
            return -1;
          write(2, "bak\n", 4);
//          close(bpf_fd);
        }
    }
  return old_setsockopt(fd, level, optname, optval, optlen);
}

static void
_init_wrapped_functions(void)
{
  old_setsockopt = dlsym(RTLD_NEXT, "setsockopt");
  if (!old_setsockopt)
    abort();
}

__attribute__((constructor))
static void
_initialize()
{
  reuse_kern = reuse_kern__open_and_load();
  if (!reuse_kern)
    {
      char *msg = "unable to load eBPF program to the kernel\n";
      write(2, msg, strlen(msg));
      abort();
    }
  _init_wrapped_functions();
}

__attribute__((destructor))
static void
_deinitialize()
{
  reuse_kern__destroy(reuse_kern);
}
