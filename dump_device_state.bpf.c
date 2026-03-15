#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

char LICENSE[] SEC("license") = "Dual BSD/GPL";

SEC("kprobe/_regmap_write")
int BPF_KPROBE(kprobe_regmap_write, struct regmap *map, unsigned int reg, unsigned int val)
{
        const char fmt_str[] = "Write to reg 0x%x: 0x%x";
        bpf_trace_printk(fmt_str, sizeof(fmt_str), reg, val);
        return 0;
}
