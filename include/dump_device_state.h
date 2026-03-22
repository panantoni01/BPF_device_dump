#ifndef __DUMP_DEVICE_STATE_H__
#define __DUMP_DEVICE_STATE_H__

#define SPI_DEVICE_CS_CNT_MAX 4

struct user_config {
        short bus_num;
        unsigned char cs;
        unsigned char reg_size;
};

#endif /* __DUMP_DEVICE_STATE_H__ */
