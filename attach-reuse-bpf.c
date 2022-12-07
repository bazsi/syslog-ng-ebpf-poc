
#define _GNU_SOURCE 1

#include <sys/types.h>
#include <sys/socket.h>
#include <dlfcn.h>

#include "reuse.skel.c"


static struct reuse_kern *reuse_kern;
static int (*old_setsockopt)(int fd, int level, int optname, const void *optval, socklen_t optlen);

int
setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
{
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
  _init_wrapped_functions();
}

__attribute__((destructor))
static void
_deinitialize()
{
  reuse_kern__destroy(reuse_kern);
}
