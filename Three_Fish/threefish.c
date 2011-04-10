#include <string.h>
#include <stdio.h>
#include "threefish.h"

// initialize a cipher context. k points to a 64-byte key, t to a 16-byte tweak.
// you initialize counter mode here and possibly precompute the round keys.
void init(const unsigned char *k, const unsigned char *t, tctx *ctx) {
  uint64_t key[9],tweak[3];
  //Round key computation.
  memcpy(key,k,64);
  memcpy(tweak,t,16);
  tweak[2] = tweak[0]^tweak[1];
  key[8] =0x5555555555555555LL ^ key[0]^key[1]^key[2]^key[3]^key[4]^key[5]^key[6]^key[7];
  for(unsigned char sub=0;sub<19;sub++)
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
  ctx->rem_track = 0;
  ctx->counter = 0;
  ctx->offset_pt = 0;
}

//Rotation Constants. 
  unsigned char rot_const[8][4] = {{46,36,19,37},{33,27,14,42},{17,49,36,39},{44,9,54,56},{39,30,34,24},{13,50,10,17},{25,29,39,43},{8,35,56,22}};

// using context ctx, encrypt len bytes of plaintext p and store the result in b.
void crypt(unsigned char *b, unsigned char *p, int len, tctx *ctx) {

  uint64_t v[8],e[8],y[8],c_word[8];
  unsigned char c_byte[64];
  unsigned int word,len_size;
  //Check for left over bytes from previous encryption operations.
  if(len == ctx->rem_track)
  {
     for(int i=0;i<len;i++)
     b[i] = p[i] ^ ctx->rem_bytes[i];
     ctx->rem_track = 0;
     len_size = 0; 
     return;
  }
 //If the given length of plain text is less than the number of bytes left over, there is no need to generate extra key bytes.
 if(len < ctx->rem_track)
 {
    for(int i=0;i<len;i++)
    b[i] = p[i] ^ ctx->rem_bytes[i];
    memmove(ctx->rem_bytes,ctx->rem_bytes + len,ctx->rem_track - len);
    ctx->rem_track = ctx->rem_track - len;
    len_size = 0;
     return;
 }
 //If the given length of plain text is more than the number of bytes left over, cipher text is generated for the bytes == length of left over bytes. Length is recalculated and number of encryption rounds are calculated based on that.
 else if(len > ctx->rem_track)
 {
    if(ctx->rem_track == 0)
    {
       word = len/64;
       if(len%64 == 0)
       len_size = word;
       else
       len_size = word + 1;
    }
    else
    {
       for(int i=0;i<ctx->rem_track;i++)
       b[i] = p[i] ^ ctx->rem_bytes[i];
       ctx->offset_pt = ctx->rem_track;
       len = len - ctx->rem_track;
       ctx->rem_track = 0;
       word = len/64;
       if(len%64 == 0)
       len_size = word;
       else
       len_size = word + 1;
    }

 } 
  //Number of iterations depending on the key stream to be generated. 
  for(int iter=0;iter < len_size;iter++)
  {
      for(int i=1;i<8;i++)
      v[i] = 0;  
      v[0] = ctx->counter;
      //72 rounds of threefish algorithm.
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
        c_word[7] = v[7] + ctx->sub_key[18][7];
    //Word to byte conversion.  
    memcpy(c_byte,c_word,64);
    
    if(iter == len_size - 1)
    {
       if(len == 64)
       {
          for(int i=0;i<64;i++)
          b[i+ ctx->offset_pt] = c_byte[i] ^ p[i + ctx->offset_pt];
          ctx->rem_track = 0;
          ctx->offset_pt = 0; 
       }
       else 
       {
          int k;
          for(k=0;k<len%64;k++)
          b[k + ctx->offset_pt] = c_byte[k] ^ p[k + ctx->offset_pt];
          for(uint8_t i=0;i<(64 - k);i++)
          ctx->rem_bytes[i] = c_byte[i+k];
          ctx->rem_track = 64 - k;
          ctx->offset_pt = 0; 
       }     
    }
    else
    {
       for(int i=0;i<64;i++)
       b[i+ ctx->offset_pt] = c_byte[i] ^ p[i + ctx->offset_pt];
       ctx->offset_pt = ctx->offset_pt + 64;
    }
    
    (ctx->counter)++;
    
 }
    
}



