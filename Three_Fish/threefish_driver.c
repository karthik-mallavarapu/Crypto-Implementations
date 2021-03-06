// this file gets replaced with our own driver when we grade your submission.
// So do what you want here but realize it won't persist when we grade it.

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "threefish.h"

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
	tctx ctx;
	int i;
	unsigned char b[64],t[16],k[64];
	uint64_t t0 = rdtsc();
	init(k,t,&ctx);
	for(i=0; i<BBLOCKS; i++)crypt(b, b, 64, &ctx);
	uint64_t t1 = rdtsc();
	printf("%llu %.2f\n",(t1-t0),(t1-t0)/(BBLOCKS*64.0));
}

#define CLOCK(x) (x >> 8 | (x ^ (x >> 7) ^ (x >> 6) ^ (x >> 2)) << 24)
void rnd(unsigned char *b, int len) {
	int i;
	static uint32_t s = 0xFEEDFACE;
	for(i=0; i<len; i++, s=CLOCK(s)) b[i] = (unsigned char)s;
}

#define MLEN 65

int main() {
	int i;
	unsigned char check0[] = {
	0x05,0x5d,0x1b,0xe6,0x11,0x26,0x66,0xf6,0xbf,0xfd,0x2c,0x50,0x12,0x75,0x14,0xde,
	0x87,0x41,0x3f,0x01,0xfc,0xde,0x64,0xc4,0xc3,0x59,0x18,0xe3,0x12,0xa3,0x29,0xaa,
	0x8b,0xfb,0x6a,0x13,0x66,0x99,0x54,0x89,0xca,0xec,0x39,0x75,0xba,0x8d,0x1f,0xbc,
	0x90,0x5d,0x13,0x54,0x28,0x49,0xb9,0xd4,0x75,0xd5,0x74,0x2b,0x47,0x24,0xd8,0x6b, 0xe6};
	unsigned char check1[] = {
	0xc5,0xfd,0x9b,0xcf,0xa2,0x68,0x51,0x1a,0x81,0x13,0x61,0xab,0xc4,0xe3,0xd3,0xf5,
	0x56,0xe6,0xf2,0x4d,0xe6,0x33,0x0d,0xc4,0x76,0x30,0xc4,0x14,0xe0,0xe2,0x13,0x30,
	0xd3,0x93,0x57,0xcd,0x4a,0x07,0x43,0x5d,0x4f,0x72,0x1e,0x8a,0xed,0xab,0xec,0xb6,
	0x6b,0xc3,0x1d,0x41,0xff,0x6e,0x3e,0x42,0x78,0xea,0x18,0xa7,0xde,0x39,0xfe,0x9c };
	tctx ctx;
	unsigned char p[MLEN], c[MLEN], k[64], t[16];
	rnd(k,64); rnd(t,16); rnd(p,MLEN); 
	init(k, t, &ctx);
	crypt(c, p, MLEN, &ctx);
	printf("%s\n", !memcmp(c, check0, MLEN) ? "PASS" : "FAIL");
	rnd(k,64); rnd(t,16); rnd(c,64);
	for(i=0; i<1024; i++) crypt(c, c, 64, &ctx);
	printf("%s\n", !memcmp(c, check1, 64) ? "PASS" : "FAIL");
	benchmark();
	return 0;
}

/*
	for(int i=0; i<19;i++)
		for(int j=0; j<8; j++)
			printf("subkey[%02d][%d]:\t%016lx\n",i,j,ctx.K[i][j]);
*/

