#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>

SEC("socket")
int random_choice(struct __sk_buff *skb)
{
  return bpf_get_prandom_u32() % 4;
}
