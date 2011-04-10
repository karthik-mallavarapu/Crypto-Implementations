// GF(2^1279) = GF(2)[x]/(x^1279 + x^319 + x^127 + x^63 + 1)
#include <string.h>
#include <stdio.h>
#include "gf2m.h"

void ff_rnd(ff_t a) {
	FILE* urandom = fopen("/dev/urandom", "r");
	size_t bytes_read = fread(a, NUMWORDS, sizeof(uint64_t), urandom);
	fclose(urandom);
	a[NUMWORDS-1] &= 0x7FFFFFFFFFFFFFFFULL;
}

void ff_print(ff_t a) {
	for(int i=NUMWORDS-1; i>=0; i--) printf("%016lX ", a[i]);
	printf("\n");
}

void ff_print_poly(ff_t a) {
	for(int i=0; i<NUMWORDS; i++)
		for(int j=0; j<64; j++)
			if(a[i] >> j & 1)
				printf("+x^%d", i*64+j);
	printf("\n");
}

// c = a*b. Don't clobber a or b.
void ff_mul(ff_t c, const ff_t a, const ff_t b) {

//Irreducible polynomial r = x^1279 + x^319 + x^127 + x^63 + 1.
ff_t r,temp_b,temp_a;
uint64_t temp_c[40];
for(int i=0;i<20;i++)
{
  r[i] = 0;
  temp_b[i] = b[i];
  temp_a[i] = a[i];
  c[i] = 0; 
}
//Irreducible polynomial equation.
r[19] = 0x8000000000000000;
r[4] = 0x8000000000000000;
r[1] = 0x8000000000000000;
r[0] =0x8000000000000001;
/*1st multiplication algorithm starts here */
  
  if(temp_a[0] & 1)
  {
    for(int i=0;i<20;i++)
      c[i] = temp_b[i];

  }
  //for i from 1 to m-1.
  for(int i=1;i<1279;i++)
  {
     uint64_t over_flow=0;
     //b = b*x
     for(int j=19;j>=1;j--)
      {
        
         if((temp_b[j-1] & 0x8000000000000000))
            over_flow = 1;
         else
            over_flow = 0;
         temp_b[j] =temp_b[j] << 1;
         temp_b[j] = temp_b[j] + over_flow; 
      }
      temp_b[0] = temp_b[0] << 1;
    
      //reducing b if degree of b greater than 1278.
      if((temp_b[19] & 0x8000000000000000))
      {
           for(int k=0;k<20;k++)
           temp_b[k] = temp_b[k] ^ r[k];
      }
      //if ith bit in a is 1, c = c + b.
      if(((temp_a[i/64] >> i%64) & 1))
      {
          for(int k=0;k<20;k++)
         c[k] = c[k] ^ temp_b[k];

      } 
  
  }

}

// c = b^e. Don't clobber b or e.
void ff_exp(ff_t c, const ff_t b, const ff_t e) {

ff_t temp_e,temp_b;
int flag=0;
for(int i=0;i<20;i++)
{
   temp_e[i] = e[i];
   temp_b[i] = b[i];
   c[i] = 0;
}
c[0] = 1;

  //checking if e > 0 
  for(int i=0;i<20;i++)
  {
    if(temp_e[i] > 0)
    {
      flag = 1;
      break;
    }
  }
  while(flag)
  {
     flag = 0;
     //if e is odd, A = A*S.
     if(temp_e[0] & 1)
       ff_mul(c,c,temp_b);
     
      //right shift e. e = e/2.
      for(int i=0;i<19;i++)
      {
         if(temp_e[i+1] & 1)
           {
              temp_e[i] = temp_e[i] >> 1;
              temp_e[i] |= 0x8000000000000000;
           }
         else
           temp_e[i] = temp_e[i] >> 1;    

      }
      temp_e[19] = temp_e[19] >> 1;
      //checking if e > 0.
      for(int i=0;i<20;i++)
      {
        if(temp_e[i] > 0)
        {
           flag = 1;
           break;
        }
      }
      // if e > 0 S = S*S.
      if(flag)
        ff_mul(temp_b,temp_b,temp_b);
  }

}


