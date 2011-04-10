#include <stdio.h>
#include <string.h>
#include "scipher.h"

inline uint64_t rdtsc() {
    uint32_t lo, hi;
    __asm__ __volatile__ (
      "xorl %%eax, %%eax\n"
      "cpuid\n"
      "rdtsc\n"
      : "=a" (lo), "=d" (hi)
      :
      : "%ebx", "%ecx" );
    return (uint64_t)hi << 32 | lo;
}

#define BBLOCKS 8192

void benchmark() {
	sctx ctx;
	unsigned char b[BBLOCKS];
	init(b, b, &ctx);
	uint64_t t0 = rdtsc();
	crypt(b, b, BBLOCKS, &ctx);
	uint64_t t1 = rdtsc();
	printf("%ld %.2f\n",t1-t0,(t1-t0)/(BBLOCKS*1.0));
}

#define MESSAGE1 "Hello"

int main() {

	sctx ctx;
	unsigned char k[] = {0x0F,0x62,0xB5,0x08,0x5B,0xAE,0x01,0x54,0xA7,0xFA};
	unsigned char iv[] = {0x28,0x8F,0xF6,0x5D,0xC4,0x2B,0x92,0xF9,0x60,0xC7};
	init(k, iv, &ctx);

	unsigned char ks[16];
	memset(ks, 0, 16);
	crypt(ks, ks, 16, &ctx);
	unsigned char check1[] = {0xA4,0x38,0x6C,0x6D,0x76,0x24,0x98,0x3F,0xEA,0x8D,0xBE,0x73,0x14,0xE5,0xFE,0x1F};
	if(!memcmp(ks, check1, 16)) printf("PASS\n");
	else printf("FAIL\n");

	init(k, iv, &ctx);
	unsigned char m[] = MESSAGE1;
	unsigned char b[] = MESSAGE1;
	unsigned char check2[] = {0xEC, 0x5D, 0x00, 0x01, 0x19};
	crypt(b, m, strlen(MESSAGE1), &ctx);
	if(!memcmp(b, check2, strlen(MESSAGE1))) printf("PASS\n");
	else printf("FAIL\n");
	benchmark(); // <- this benchmarks your code in CPU cycles per byte.
	return 0;
}

/*
kosh ~/assignment2_model 62 % make clean ; make
/bin/rm -f scipher.o scipher_driver *.o core *~
cc -std=c99 -I.  -O2 -fomit-frame-pointer -funroll-loops -c scipher.c 
cc -std=c99 -I.  -O2 -fomit-frame-pointer -funroll-loops -o scipher_driver scipher_driver.c scipher.o 
kosh ~/assignment2_model 63 % ./scipher_driver
PASS
PASS
118035 14.41
kosh ~/assignment2_model 64 % 
*/
