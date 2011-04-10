#include <stdio.h>
#include <string.h>
#include "gf2m.h"

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

#define BBLOCKS 32

void benchmark() {
	ff_t g, e;
	ff_rnd(g);
	ff_rnd(e);
	uint64_t t0 = rdtsc();
	for(int i=0; i<BBLOCKS; i++) ff_exp(e,g,e);
	uint64_t t1 = rdtsc();
	printf("%ld %.2f\n",t1-t0,(t1-t0)/(BBLOCKS*1.0));
}

int main() {

	ff_t c, d, g, zero, one; ff_zero(zero); ff_zero(one); one[0] = 0x1;

	ff_t a = {	0x0B922A5894CCBA66ULL,0x1860C1FA43DFA1D6ULL,0x0A2C46E6F566C8BBULL,0x5BDC0859A82A229DULL,
			0x0149E61C743AC579ULL,0x84AAF6FCDB7557D0ULL,0xAF4D0A4C5C1EBD9DULL,0x5CDCDB0635E6E149ULL,
			0x7FCEDD2CD9E9F7C1ULL,0x879282896EF5DF79ULL,0x2D34F6AB203332B8ULL,0xB7436E13D727B998ULL,
			0xDB688A68BE8506ABULL,0x75B69A48E978945DULL,0xBCDCB66664C09EDFULL,0x36F5116FE21F6F72ULL,
			0x9F71F0AEDB3DA236ULL,0x1DCFBB1B4BDDD28AULL,0x619D35529B3B7C35ULL,0x175E699D141A4617ULL };
	ff_t b = {	0x8FA3193D212CB2F5ULL,0x2DDEF46FE756F455ULL,0xCB0A47DEE5C190EAULL,0x3DF84E4602C3460BULL,
			0x46AD72AFA180FEE8ULL,0x9122CF0EF4628FE5ULL,0x6CCAE25685532090ULL,0x82DD4792720BD4FCULL,
			0xB83EC7B1384FB7DFULL,0x2A0F4982EC6A2928ULL,0x0DAA7BB8E8C7B704ULL,0x1128BEA051D9931AULL,
			0x531056A2DAFA703AULL,0x1A51C93F00528552ULL,0xA4BE7A0A69594A3DULL,0xD1F187FD245C009EULL,
			0x8A5F505F03FC8B56ULL,0x09D9C08E6201A420ULL,0x80A066A9052585C5ULL,0x41DD3E8F2D5907DBULL };
	ff_t c1 = {	0x2C5F468B3DF70052ULL,0x5BC18CA04B829154ULL,0xD58D45E75D6EDFD2ULL,0x9AADF5A79425EC7DULL,
			0x6E5B6BDAC531E3B4ULL,0x258696116348E455ULL,0x183F6E673E8519EDULL,0x748AC5D82409A33FULL,
			0x96B161B2071A7CCBULL,0xCB9D071C8D6CF9C7ULL,0xEAE8CF43082F472AULL,0x123C89C9EFB10AF9ULL,
			0xE34C8632873A5AB8ULL,0xD650FE4011008EC2ULL,0x99C34CB21A0C0000ULL,0x0F86D600077EB9BFULL,
			0x204456286B7D531AULL,0xDE7BDDB1EAAE9E8EULL,0xC7D397B376D669B0ULL,0x6C93A5EFCCDDBC0AULL };
	ff_t c2 = {	0x7FCB0A61F42EB58DULL,0x583ADB555F1284D9ULL,0x8E951AC27F91A692ULL,0xDD101B5793AB10EFULL,
			0x527D5EC8C1505D09ULL,0x1563EC121497C50BULL,0x12811C2A141AF09FULL,0x7015DF7E23BFD9B0ULL,
			0x7AA0E653020E1B32ULL,0x11665182A3DC879DULL,0xC9186CD9C8740F5EULL,0x79E7A6B3BF2DAC97ULL,
			0x9E50515076DD1CF9ULL,0xF949431E934271FEULL,0x88EA54205E4634EDULL,0x744CA69F46AEAF3DULL,
			0x2F6202C4204B6CD2ULL,0xE8FE4CDB5208091DULL,0xFDC3C1AAD3CD8010ULL,0x7B3B91E7B60D418FULL };

	// test 1: general multiplication
	ff_mul(c, a, b);
	if(!memcmp(c, c1, NUMWORDS * sizeof(uint64_t))) printf("PASS\n");
	else printf("FAIL\n");

	// test 2: multiplication by zero
	ff_mul(c, a, zero);
	if(!memcmp(c, zero, NUMWORDS * sizeof(uint64_t))) printf("PASS\n");
	else printf("FAIL\n");

	// test 3: multiplication by one
	ff_mul(c, a, one);
	if(!memcmp(c, a, NUMWORDS * sizeof(uint64_t))) printf("PASS\n");
	else printf("FAIL\n");

	// test 4: general exponentiation
	ff_exp(c, a, b);
	if(!memcmp(c, c2, NUMWORDS * sizeof(uint64_t))) printf("PASS\n");
	else printf("FAIL\n");

	// test 5: power 0
	ff_exp(c, a, zero);
	if(!memcmp(c, one, NUMWORDS * sizeof(uint64_t))) printf("PASS\n");
	else printf("FAIL\n");

	// test 6: power 1
	ff_exp(c, a, one);
	if(!memcmp(c, a, NUMWORDS * sizeof(uint64_t))) printf("PASS\n");
	else printf("FAIL\n");

	// test 7: power 2
	ff_zero(d); d[0] = 0x2;
	ff_exp(c, a, d); ff_mul(d, a, a);
	if(!memcmp(c, d, NUMWORDS * sizeof(uint64_t))) printf("PASS\n");
	else printf("FAIL\n");

	// test 8: power 2^m
	ff_copy(c, a);
	for(int i=0; i<1279; i++) ff_mul(c, c, c);
	if(!memcmp(c, a, NUMWORDS * sizeof(uint64_t))) printf("PASS\n");
	else printf("FAIL\n");

	// test 9: inverse
	memset(d, 0xFF, NUMWORDS * sizeof(uint64_t)); d[NUMWORDS-1] = 0x7FFFFFFFFFFFFFFFULL; d[0] = 0xFFFFFFFFFFFFFFFEULL;
	ff_exp(c, a, d);
	ff_mul(c, c, a);
	if(!memcmp(c, one, NUMWORDS * sizeof(uint64_t))) printf("PASS\n");
	else printf("FAIL\n");

	// test 10: Diffie-Hellman
	ff_zero(g); g[0] = 0x2;
	ff_exp(c, g, a);
	ff_exp(d, g, b);
	ff_exp(b, c, b);
	ff_exp(a, d, a);
	if(!memcmp(a, b, NUMWORDS * sizeof(uint64_t))) printf("PASS\n");
	else printf("FAIL\n");

	benchmark();

	return 0;
}

/*
kosh ~/assignment4_model 58 % make clean; make
/bin/rm -f gf2m.o gf2m_driver *.o core *~
cc -std=c99 -I.  -O2 -fomit-frame-pointer -funroll-loops -c gf2m.c 
cc -std=c99 -I.  -O2 -fomit-frame-pointer -funroll-loops -o gf2m_driver gf2m_driver.c gf2m.o 
kosh ~/assignment4_model 59 % ./gf2m_driver
PASS
PASS
PASS
PASS
PASS
PASS
PASS
PASS
PASS
PASS
301744413 9429512.91
kosh ~/assignment4_model 60 % 
*/
