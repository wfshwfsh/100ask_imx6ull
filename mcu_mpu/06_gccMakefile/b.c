#include <stdio.h>
#include "b.h"
int b=B;

void func_b(void)
{
	printf("%s b=%d \n", __func__, b);
	
}
