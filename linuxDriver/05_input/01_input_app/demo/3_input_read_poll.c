#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/input.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <poll.h>

#include "debug.h"

#define INPUT_DEV "/dev/input/event1"


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

void get_dev_info(int _fd)
{
    struct input_id id;
	int err;
	
    /* get input id */
    err = ioctl(_fd, EVIOCGID, &id);
    if (err == 0)
	{
		APPLOG_DDD("bustype = 0x%x", id.bustype );
		APPLOG_DDD("vendor  = 0x%x", id.vendor  );
		APPLOG_DDD("product = 0x%x", id.product );
		APPLOG_DDD("version = 0x%x", id.version );
	}
}

void get_dev_ability(int _fd)
{
	int len;
    unsigned int evbit[2];

    //get input device support events
    len = ioctl(_fd, EVIOCGBIT(0, sizeof(evbit)), &evbit);
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
}

int main(int argc ,char *argv[])
{
    char device[32];
    int input_fd, ret;
	struct input_event event;
	struct pollfd fds[1];
	int timeout;//ms
    
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
    
    get_dev_info(input_fd);
    get_dev_ability(input_fd);
	
    /* setup poll/select fds */
    fds[0].fd = input_fd;
	fds[0].events = POLLIN;
    
	while(1)
	{
		timeout = 3000;/* 3sec*/
		ret = poll(&fds, 1/*nfds*/, timeout);

		if(ret<0)
		{
			APPLOG_E("error");
		}
		else if(ret == 0)
		{
			APPLOG_I("timeout");
		}
		else
		{
			//ret means the num of revent be returned
			if (fds[0].revents == POLLIN)
			{
				while (read(input_fd, &event, sizeof(event)) == sizeof(event))
				{
					printf("get event: type = 0x%x, code = 0x%x, value = 0x%x\n", event.type, event.code, event.value);
				}
			}
		}
	}
	
    return 0;
}


