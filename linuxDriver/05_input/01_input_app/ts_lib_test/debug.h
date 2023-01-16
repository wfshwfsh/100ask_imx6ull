#ifndef DEBUG_COMMON_H
#define DEBUG_COMMON_H

#include <stdio.h>


//#ifdef DEBUG
#if 1

#define DBG_0 printf("[%s] 00000000000000000000000 \n", __func__);
#define DBG_1 printf("[%s] 11111111111111111111111 \n", __func__);
#define DBG_2 printf("[%s] 22222222222222222222222 \n", __func__);
#define DBG_3 printf("[%s] 33333333333333333333333 \n", __func__);
#define DBG_4 printf("[%s] 44444444444444444444444 \n", __func__);
#define DBG_5 printf("[%s] 55555555555555555555555 \n", __func__);
#define DBG_A printf("[%s] AAAAAAAAAAAAAAAAAAAAAAA \n", __func__);
#define DBG_B printf("[%s] BBBBBBBBBBBBBBBBBBBBBBB \n", __func__);
#define DBG_C printf("[%s] CCCCCCCCCCCCCCCCCCCCCCC \n", __func__);
#define GAR_WHILE while(1){;}

#define APPLOG_DBG_1 printf("[%s %d] 11111111111111111111111 \n", __func__, __LINE__);
#define APPLOG_DBG_2 printf("[%s %d] 22222222222222222222222 \n", __func__, __LINE__);
#define APPLOG_DBG_3 printf("[%s %d] 33333333333333333333333 \n", __func__, __LINE__);
#define APPLOG_DBG_4 printf("[%s %d] 44444444444444444444444 \n", __func__, __LINE__);
#define APPLOG_DBG_5 printf("[%s %d] 55555555555555555555555 \n", __func__, __LINE__);

#define APPLOG_STD_I(...)	printf(__VA_ARGS__); printf("\n")
#define APPLOG_STD_W(...)	printf(__VA_ARGS__); printf("\n")
#define APPLOG_STD_E(...)	printf(__VA_ARGS__); printf("\n")

//#define ALOGV(fmt, args...) printf("V:<%s,%d> "fmt"\n",__FUNCTION__,__LINE__, ##args)
#define APPLOG_D(fmt, args...)		printf("[%s %d] "fmt"\n", __FUNCTION__, __LINE__, ##args)
#define APPLOG_DD(fmt, args...)		printf("[%s %d] "fmt"\n", __FUNCTION__, __LINE__, ##args)
#define APPLOG_DDD(fmt, args...)	printf("[%s %d] "fmt"\n", __FUNCTION__, __LINE__, ##args)

#else
   
#define DBG_0
#define DBG_1
#define DBG_2
#define DBG_3
#define DBG_4
#define DBG_5
#define DBG_A
#define DBG_B
#define DBG_C
#define GAR_WHILE 

#define APPLOG_DBG_1 
#define APPLOG_DBG_2 
#define APPLOG_DBG_3 
#define APPLOG_DBG_4 
#define APPLOG_DBG_5 

#define APPLOG_STD_I(...)	
#define APPLOG_STD_W(...)	
#define APPLOG_STD_E(...)	

//#define ALOGV(fmt, args...) 
#define APPLOG_D(fmt, args...)		
#define APPLOG_DD(fmt, args...)		
#define APPLOG_DDD(fmt, args...)	

#endif



#define APPLOG_I(fmt, args...)		printf("[%s %d][I:] "fmt"\n", __FUNCTION__, __LINE__, ##args)
#define APPLOG_W(fmt, args...)		printf("[%s %d][W:] "fmt"\n", __FUNCTION__, __LINE__, ##args)
#define APPLOG_E(fmt, args...)		printf("[%s %d][E:] "fmt"\n", __FUNCTION__, __LINE__, ##args)
#define APPLOG_TBD(fmt, args...)	printf("[%s %d][TBD] "fmt"\n", __FUNCTION__, __LINE__, ##args)

#endif