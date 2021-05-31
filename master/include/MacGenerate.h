

#ifndef _MACGENERATE_H_
#define _MACGENERATE_H_
#include "Platform_Types.h"

void Preprocess(uint8* datavalue, uint8 datalen);

void gethash(uint8 *hash);

void Mac_Generate(uint8* datavalue, uint8 datalen, uint8* hash);

#endif




