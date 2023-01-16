#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/input.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <poll.h>

#include <tslib.h>

#include "debug.h"

#define INPUT_DEV "/dev/input/event1"

int distence(struct ts_sample_mt *a, struct ts_sample_mt *b)
{
    int x = a->x - b->x;
    int y = a->y - b->y;
    
    return x*x+y*y;
}

int main(int argc ,char *argv[])
{
    struct ts_sample_mt **samp_mt = NULL;
    struct ts_sample_mt **pre_samp_mt = NULL;
    struct tsdev *ts = NULL;
    const char *tsdevice = NULL;
    int point_pressed[20];
    int32_t max_slots = 1;
    int ret;
	int flags = 0;
    
    if (argc != 2)
	{
		printf("Usage: %s <dev>\n", argv[0]);
        //strcpy(tsdevice, INPUT_DEV);
        tsdevice = INPUT_DEV;
		//return -1;
	}else{
        tsdevice = argv[1];
    }
    
    ts = ts_setup(tsdevice, 0);
    if (!ts) {
		perror("ts_setup");
		return errno;
	}
    
    if (ioctl(ts_fd(ts), EVIOCGABS(ABS_MT_SLOT), &slot) < 0) {
		perror("ioctl EVIOGABS");
		ts_close(ts);
		return errno;
	}

	max_slots = slot.maximum + 1 - slot.minimum;
    
    samp_mt = malloc(sizeof(struct ts_sample_mt *));
	if (!samp_mt) {
		ts_close(ts);
		return -ENOMEM;
	}
	samp_mt[0] = calloc(max_slots, sizeof(struct ts_sample_mt));
	if (!samp_mt[0]) {
		free(samp_mt);
		ts_close(ts);
		return -ENOMEM;
	}
    
    pre_samp_mt = malloc(sizeof(struct ts_sample_mt *));
	if (!samp_mt) {
		ts_close(ts);
		return -ENOMEM;
	}
	pre_samp_mt[0] = calloc(max_slots, sizeof(struct ts_sample_mt));
	if (!pre_samp_mt[0]) {
		free(pre_samp_mt);
		ts_close(ts);
		return -ENOMEM;
	}
    
    
    int touch_cnt = 0;
	while(1)
	{
		ret = ts_read_mt(ts, samp_mt, max_slots, 1);
        
		if (ret < 0) {
			printf("ts_read_mt err\n");
			ts_close(ts);
			return -1;
		}
        
        for (int i = 0; i < max_slots; i++) {
			if (samp_mt[0][i].valid & TSLIB_MT_VALID){
				memcpy(pre_samp_mt, samp_mt, sizeof(struct ts_sample_mt));
            }
        }
        
        touch_cnt = 0;
        
        //.valid means has new data
        //check have new data && "not release" touch point 
        for (int i = 0; i < max_slots; i++) {
            if (pre_samp_mt[0][i].valid && pre_samp_mt[0][i].tracking_id != -1){
                point_pressed[touch_cnt++] = i;
            }
        }
        
        if(2 == touch_cnt){
            
            printf("distence = %08d \n"
                , distence(&pre_samp_mt[0][point_pressed[0]], &pre_samp_mt[0][point_pressed[1]]));
        }
	}
	
OUT:
    free(samp_mt[0]);
    free(samp_mt);
    free(pre_samp_mt[0]);
    free(pre_samp_mt);
    ts_close(ts);
    
    return 0;
}


