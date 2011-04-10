#include <stdint.h>

// Cipher context. Store any state stuff here; e.g. you might put
// round keys, counters, etc.
typedef struct {
//Subkey array.
uint64_t sub_key[19][8];
//Remaining bytes from previous encryption operation.
unsigned char rem_bytes[63];
unsigned int offset_pt;
unsigned int rem_track;
//Counter value.
uint64_t counter;
} tctx;

void crypt(unsigned char *, unsigned char *, int, tctx *);
void init(const unsigned char *, const unsigned char *, tctx *);
