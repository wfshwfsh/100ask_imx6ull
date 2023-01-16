#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include "smbus.h"
#include "i2cbusses.h"

int main(int argc, char *argv[])
{
    unsigned char dev_addr = 0x1e;
    int file;
	char filename[20];
    int res;//, i2cbus;
    unsigned char block_data[I2C_SMBUS_BLOCK_MAX];
    
    //i2cbus = lookup_i2c_bus(argv[optind]);
	//if (i2cbus < 0){
    //    printf("i2cbus error\n");
    //}
    
    /* open */
    file = open_i2c_dev("/dev/i2c-1", filename, sizeof(filename), 1);
    if(file<0){
        printf("open_i2c_dev error\n");
        return -1;
    }
    
    if (set_slave_addr(file, dev_addr, 1))
	{
		printf("can't set_slave_addr\n");
		return -1;
	}
    
    /* write rst: reg:0, data:0x04 */
    res = i2c_smbus_write_byte_data(file, /*reg*/0, 0x4);
    if (res < 0) {
		fprintf(stderr, "Error: Write failed\n");
		close(file);
		exit(1);
	}
    
    /* write enable: reg:0, data:0x03 */
    res = i2c_smbus_write_byte_data(file, /*reg*/0, 0x3);
    if (res < 0) {
		fprintf(stderr, "Error: Write failed\n");
		close(file);
		exit(1);
	}
    
    /* read data: reg:0xC 0xD, two byte */
    res = i2c_smbus_read_block_data(file, 0xC, block_data);
    if (res < 0) {
		fprintf(stderr, "Error: Read failed\n");
		exit(2);
	}
    
    for (int i = 0; i < res - 1; ++i)
        printf("0x%02hhx ", block_data[i]);
    printf("0x%02hhx\n", block_data[res - 1]);
    
    
    /* read data: reg:0xE 0xF, two byte */
    res = i2c_smbus_read_block_data(file, 0xE, block_data);
    if (res < 0) {
		fprintf(stderr, "Error: Read failed\n");
		exit(2);
	}
    
    close(file);
    
    for (int i = 0; i < res - 1; ++i)
        printf("0x%02hhx ", block_data[i]);
    printf("0x%02hhx\n", block_data[res - 1]);
    
    return 0;
}
