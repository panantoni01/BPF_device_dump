#include "vmlinux.h"
#include <bpf/bpf_core_read.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <dump_device_state.h>

char LICENSE[] SEC("license") = "Dual BSD/GPL";

struct spi_dev_config spi_cfg;

struct reg_op {
        u64 timestamp;
        u8 reg;
        u32 value;
};

struct {
        __uint(type, BPF_MAP_TYPE_HASH);
	__uint(max_entries, 4194304); /* from /proc/sys/kernel/pid_max */
	__type(key, u64);
	__type(value, struct reg_op);
} __pid_to_reg_op SEC(".maps");

struct reg_record {
        u64 timestamp_sec;
        u64 timestamp_nsec;
        u32 value;
};

struct {
        __uint(type, BPF_MAP_TYPE_ARRAY);
	__uint(max_entries, 255);
	__type(key, u32);
	__type(value, struct reg_record);
} registers SEC(".maps");

SEC("kprobe/_regmap_write")
int BPF_KPROBE(kprobe_regmap_write, struct regmap *map, unsigned int reg, unsigned int val)
{
        int result = 0;
        u64 pid_tgid = bpf_get_current_pid_tgid();
        struct reg_op r_op;
        struct device *curr_device = BPF_CORE_READ(map, dev);
        struct spi_device *curr_spi_dev = container_of(curr_device, struct spi_device, dev);

        s16 bus_num = BPF_CORE_READ(curr_spi_dev, controller, bus_num);
        u8 chip_select = BPF_CORE_READ(curr_spi_dev, chip_select[0]);

        if (bus_num == spi_cfg.bus_num && chip_select == spi_cfg.cs) {
                r_op.timestamp = bpf_ktime_get_ns();
                r_op.reg = (u8)reg;
                r_op.value = val;

                result = bpf_map_update_elem(&__pid_to_reg_op, &pid_tgid, &r_op, BPF_ANY);
        }

        return result;
}

SEC("kretprobe/_regmap_write")
int BPF_KRETPROBE(kretprobe_regmap_write, int ret)
{
        int result = 0;
        u64 pid_tgid;
        struct reg_op* r_op;
        struct reg_record r_record;

        if (ret == 0) {
                pid_tgid = bpf_get_current_pid_tgid();

                r_op = bpf_map_lookup_elem(&__pid_to_reg_op, &pid_tgid);
                if (!r_op)
                        return 0;

                r_record.timestamp_sec = r_op->timestamp / 1000000000;
                r_record.timestamp_nsec = r_op->timestamp % 1000000000;
                r_record.value = r_op->value;

                result = bpf_map_update_elem(&registers, &r_op->reg, &r_record, BPF_ANY);
        }

        return result;
}
