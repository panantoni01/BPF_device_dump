#include <stdio.h>
#include <unistd.h>
#include "dump_device_state.skel.h"

int main()
{
        struct dump_device_state_bpf *skel;
        int err = 0;

	skel = dump_device_state_bpf__open_and_load();
	if (!skel) {
		fprintf(stderr, "failed to open and load BPF object\n");
		return 1;
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
