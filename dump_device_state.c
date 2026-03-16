#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dump_device_state.h>
#include "dump_device_state.skel.h"

int main(int argc, char *argv[])
{
        struct spi_dev_config spi_cfg;
        struct dump_device_state_bpf *skel;
        int opt, err = 0;

        while ((opt = getopt(argc, argv, "a:b:")) != -1) {
                switch (opt) {
                case 'a':
                        spi_cfg.cs = atoi(optarg);
                        break;
                case 'b':
                        spi_cfg.bus_num = atoi(optarg);
                        break;
                default:
                        fprintf(stderr, "usage: %s -a <cs> -b <bus_num>\n", argv[0]);
                        return -EINVAL;
                }
        }

	skel = dump_device_state_bpf__open();
	if (!skel) {
		fprintf(stderr, "failed to open BPF object\n");
		return 1;
	}

        memcpy(&skel->bss->spi_cfg, &spi_cfg, sizeof(struct spi_dev_config));

	err = dump_device_state_bpf__load(skel);
	if (err) {
		fprintf(stderr, "failed to load and verify BPF skeleton\n");
		goto cleanup;
	}

	err = dump_device_state_bpf__attach(skel);
	if (err) {
		fprintf(stderr, "failed to attach BPF skeleton: %d\n", err);
		goto cleanup;
	}

	while (true) {
		sleep(1);
	}

cleanup:
	dump_device_state_bpf__destroy(skel);
	return -err;
}
