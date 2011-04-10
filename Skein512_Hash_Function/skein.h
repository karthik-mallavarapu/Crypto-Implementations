#include <stdint.h>

/* Skein hash context. you might store:
1) Up to a block of buffered message.
2) The chaining value.
3) The tweak.
4) Anything else you might need.
*/
typedef struct {
uint64_t sub_key[19][8];
uint64_t tweak_msg[3];
uint64_t tweak_config[3];
uint64_t tweak_output[3];
uint64_t G[9];
uint64_t hash_context[8];
uint64_t C[8];
int final_len;
uint64_t msg[8];
} hctx;

void keygen(uint64_t *, uint64_t *,hctx *);
void threefish(uint64_t *,uint64_t *,hctx *);
void init(hctx *);
void update(unsigned char *, int, hctx *);
void finalize(unsigned char *, hctx *);




