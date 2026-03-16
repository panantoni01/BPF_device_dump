#include "vmlinux.h"
#include <bpf/bpf_core_read.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <dump_device_state.h>

char LICENSE[] SEC("license") = "Dual BSD/GPL";

struct spi_dev_config spi_cfg;

SEC("kprobe/_regmap_write")
int BPF_KPROBE(kprobe_regmap_write, struct regmap *map, unsigned int reg, unsigned int val)
{
        const char fmt_str1[] = "Write to spi%u.%u:";
        const char fmt_str2[] = "  reg[%u] := 0x%x";
        struct device *curr_device = BPF_CORE_READ(map, dev);
        struct spi_device *curr_spi_dev = container_of(curr_device, struct spi_device, dev);

        s16 bus_num = BPF_CORE_READ(curr_spi_dev, controller, bus_num);
        u8 chip_select = BPF_CORE_READ(curr_spi_dev, chip_select[0]);

        if (bus_num == spi_cfg.bus_num && chip_select == spi_cfg.cs) {
                bpf_trace_printk(fmt_str1, sizeof(fmt_str1), spi_cfg.bus_num, spi_cfg.cs);
                bpf_trace_printk(fmt_str2, sizeof(fmt_str2), reg, val);
        }

        return 0;
}
