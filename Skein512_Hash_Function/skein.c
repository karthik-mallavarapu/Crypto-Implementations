#include <string.h>
#include <stdio.h>
#include "skein.h"

// initialize a hash context.
void init(hctx *ctx) {
  unsigned char config_string[64];

  uint64_t key[9];
//Setting up the default configuration string.
  memset(config_string,0,64);
  config_string[0] = 0x53;
  config_string[1] = 0x48;
  config_string[2] = 0x41;
  config_string[3] = 0x33;
  config_string[4] = 0x01;
  memcpy(ctx->C,config_string,64);
//Setting number of output bits.
  ctx->C[1] = 512;
 for(int i=0;i<9;i++)
   key[i] = 0; 
  ctx->tweak_msg[0] = 0;
  ctx->tweak_msg[1] = 0;
  ctx->tweak_config[0] = 0;
  ctx->tweak_config[1] = 0;
  ctx->tweak_output[0] = 0;
  ctx->tweak_output[1] = 0;
  //Setting position values for config,msg and output modes.
  ctx->tweak_config[0] = 32;
  ctx->tweak_msg[0] = 64;
  ctx->tweak_output[0] = 8;

//Setting tweak final and first bits. 
  *((char *)(ctx->tweak_config) + 15) = 196;
  *((char *)(ctx->tweak_output) + 15) = 255;

//Calculating initial G value.
  keygen(key,ctx->tweak_config,ctx);
  threefish(ctx->G,ctx->C,ctx);
//Initialiazing context parameters for future use. 
  ctx->final_len = 0;
  for(int i=0;i<8;i++)
    ctx->msg[i] = 0;

}
//threefish rotation constants.
unsigned char rot_const[8][4] = {{46,36,19,37},{33,27,14,42},{17,49,36,39},{44,9,54,56},{39,30,34,24},{13,50,10,17},{25,29,39,43},{8,35,56,22}};

//threefish subkey generation 
void keygen(uint64_t *key,uint64_t *tweak,hctx *ctx) {

tweak[2] = tweak[0]^tweak[1];
key[8] =0x5555555555555555LL;
  for(int i=0;i<8;i++)
  {
     key[8] ^= key[i];
  }
  for(unsigned int sub=0;sub<19;sub++)
  {
    ctx->sub_key[sub][0] = key[(sub)%9]; 
    ctx->sub_key[sub][1] = key[(sub+1)%9]; 
    ctx->sub_key[sub][2] = key[(sub+2)%9]; 
    ctx->sub_key[sub][3] = key[(sub+3)%9]; 
    ctx->sub_key[sub][4] = key[(sub+4)%9]; 
    ctx->sub_key[sub][5] = key[(sub+5)%9] + tweak[sub%3];
    ctx->sub_key[sub][6] = key[(sub+6)%9] + tweak[(sub+1)%3];
    ctx->sub_key[sub][7] = key[(sub+7)%9] + sub;
  }

}

// update a hash context with len bytes at address a.
void update(unsigned char *a, int len, hctx *ctx) {
   
   uint64_t test_msg[8];
   memset(test_msg,0,64);
        
  //If input length is less than 64 bytes, bytes are stored in context until buffer size is reached 
  if(len < 64)
    memcpy((char *)ctx->msg+(ctx->final_len % 64),a,len);
 //If input length is greater than 64 bytes, the first block is  handled immediately.
  else if(len >=64)
    {
       memcpy(test_msg,a,64);
       memcpy((char *)ctx->msg+(ctx->final_len % 64),a + 64,len - 64);
       ctx->tweak_msg[0] = 64;
       *((char *)ctx->tweak_msg + 15) = 112;
       keygen(ctx->G,ctx->tweak_msg,ctx);
       threefish(ctx->G,test_msg,ctx);
  
    }
//Storing and updating the processed bytes in context
 ctx->final_len += len;
 
}

// finalize a hash context and output the hash value in a.
void finalize(unsigned char *a, hctx *ctx) {

//If the block number is 1, the first and final bits are enabled in tweak message mode.
  ctx->tweak_msg[1] = 0;
  ctx->tweak_msg[0] = ctx->final_len;
  *((char *)ctx->tweak_msg + 15) = 240;
//If the block number is not 1,final bit in tweak message mode is enabled only for the last block.
  if(ctx->final_len > 64)
    *((char *)ctx->tweak_msg + 15) = 176;
//UBI Computation.
  keygen(ctx->G,ctx->tweak_msg,ctx);
  threefish(ctx->G,ctx->msg,ctx);
//Output result calculation.
  for(int i=0;i<8;i++)
    ctx->C[i] = 0;
  keygen(ctx->G,ctx->tweak_output,ctx);
  threefish(ctx->hash_context,ctx->C,ctx);
  memcpy(a,ctx->hash_context,64);	

}


void threefish(uint64_t *c,uint64_t *m,hctx *ctx) {

uint64_t v[8],e[8],y[8],c_word[8];  
  memcpy(v,m,64);
        for(int nr=0;nr<72;nr++)
      {
          //Do a subkey addition once for every 4 rounds.
          if(nr%4 == 0)
          {
            e[0] = v[0] + ctx->sub_key[nr/4][0]; 
            e[1] = v[1] + ctx->sub_key[nr/4][1]; 
            e[2] = v[2] + ctx->sub_key[nr/4][2]; 
            e[3] = v[3] + ctx->sub_key[nr/4][3]; 
            e[4] = v[4] + ctx->sub_key[nr/4][4]; 
            e[5] = v[5] + ctx->sub_key[nr/4][5]; 
            e[6] = v[6] + ctx->sub_key[nr/4][6]; 
            e[7] = v[7] + ctx->sub_key[nr/4][7]; 
          }
          else
          {
            e[0] = v[0]; 
            e[1] = v[1];
            e[2] = v[2];
            e[3] = v[3];
            e[4] = v[4];
            e[5] = v[5];
            e[6] = v[6];
            e[7] = v[7];
          }
    
        
       //Mix Operation using rotation constants.
        y[0] = e[0] + e[1]; 
        y[1] = ((e[1] <<  rot_const[nr%8][0]) | (e[1] >> (64 - rot_const[nr%8][0]))) ^ y[0];
        y[2] = e[2] + e[3];
        y[3] = ((e[3] <<  rot_const[nr%8][1]) | (e[3] >> (64 - rot_const[nr%8][1]))) ^ y[2];
        y[4] = e[4] + e[5];
        y[5] = ((e[5] <<  rot_const[nr%8][2]) | (e[5] >> (64 - rot_const[nr%8][2]))) ^ y[4];
        y[6] = e[6] + e[7];
        y[7] = ((e[7] <<  rot_const[nr%8][3]) | (e[7] >> (64 - rot_const[nr%8][3]))) ^ y[6];
        //Permute operation.
	v[0] = y[2];
        v[1] = y[1];
	v[2] = y[4];
        v[3] = y[7];
        v[4] = y[6];
        v[5] = y[5];
        v[6] = y[0];
        v[7] = y[3]; 
     }  //Final subkey addition.
        c_word[0] = v[0] + ctx->sub_key[18][0];
        c_word[1] = v[1] + ctx->sub_key[18][1];
        c_word[2] = v[2] + ctx->sub_key[18][2];
        c_word[3] = v[3] + ctx->sub_key[18][3];
        c_word[4] = v[4] + ctx->sub_key[18][4];
        c_word[5] = v[5] + ctx->sub_key[18][5];
        c_word[6] = v[6] + ctx->sub_key[18][6];
        c_word[7] = v[7] + ctx->sub_key[18][7]; //   memcpy(c_byte,c_word,64);

//xoring in ubi chaining at the end..
     for(int i=0;i<8;i++)
     {
       c[i] = m[i] ^ c_word[i];
     }
    
}
