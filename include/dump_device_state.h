#ifndef __DUMP_DEVICE_STATE_H__
#define __DUMP_DEVICE_STATE_H__

#define SPI_DEVICE_CS_CNT_MAX 4

struct spi_dev_config {
        short bus_num;
        unsigned char cs;
};

#endif /* __DUMP_DEVICE_STATE_H__ */
