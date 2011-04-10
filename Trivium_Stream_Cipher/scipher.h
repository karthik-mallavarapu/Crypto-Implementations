#include <stdint.h>

/*
  structure for stream cipher state (the FSRs).
*/
typedef struct {

} sctx;

void init(unsigned char *, unsigned char *, sctx *);
void crypt(unsigned char *, unsigned char *, int, sctx *);

