
#ifndef _STRING_H
#define _STRING_H
int puts(const char *s);
void puthex(unsigned int val);
void put_s_hex(const char *s, unsigned int val);
void memset(void *dst, unsigned char val, unsigned int len);
void memcpy(void *dst, void *src, unsigned int len);
#endif

