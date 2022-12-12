#include <stdio.h>
#include "a.h"
int a=A;

void func_a(void)
{
	printf("%s a=%d \n", __func__, a);
	
}
