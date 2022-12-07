#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>

SEC("sk_reuseport")
int random_choice(struct sk_reuseport_md *md)
{
  return bpf_get_prandom_u32() % 16;
}
