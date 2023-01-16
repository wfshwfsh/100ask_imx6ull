#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/input.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "debug.h"

#define INPUT_DEV "/dev/input/event2"


const char *ev_names[] = {
	"EV_SYN",
	"EV_KEY",
	"EV_REL",
	"EV_ABS",
	"EV_MSC",
	"EV_SW",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	"EV_LED",
	"EV_SND",
	NULL,
	"EV_REP",
	"EV_FF",
	"EV_PWR",
	"EV_FF_STATUS"
};

int main(int argc ,char *argv[])
{
    char device[32];
    int input_fd, ret, err, len;
    struct input_id id;
    unsigned int evbit[2];
	struct input_event input_evt;
	
    
    if (argc != 2)
	{
		printf("Usage: %s <dev>\n", argv[0]);
        strcpy(device, INPUT_DEV);
		//return -1;
	}else{
        strcpy(device, argv[1]);
    }
    
    input_fd = open(device, O_RDWR);
    if(input_fd < 0){
        APPLOG_DDD("open device failed");
    }
    
    /* get input id */
    err = ioctl(input_fd, EVIOCGID, &id);
    if (err == 0)
	{
		APPLOG_DDD("bustype = 0x%x", id.bustype );
		APPLOG_DDD("vendor  = 0x%x", id.vendor  );
		APPLOG_DDD("product = 0x%x", id.product );
		APPLOG_DDD("version = 0x%x", id.version );
	}
    
    len = ioctl(input_fd, EVIOCGBIT(0, sizeof(evbit)), &evbit);
    APPLOG_DDD("evbit = 0x%08x", evbit);

	printf("support event types: ");
	if(len>0 && len<sizeof(evbit))
	{
		for(int i=0;i<len;i++)
	    {
	    	int byte = ((unsigned char *)evbit)[i];

			for(int bit=0;bit<8;bit++)
			{
				if((1<<bit) & byte)
				{
					printf("%s ", ev_names[i*8+bit]);
				}
			}			
		}
	}	
    printf("\n");
    return 0;
}


