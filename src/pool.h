#ifndef	_POOL_H
#define	_POOL_H

//#include <hw_r.h>

void Pool_Init(void);
void WriteToPool(unsigned int *ptr, unsigned int words);
void FlushPool(void);
unsigned int* GetStatusPool(unsigned int *pbytes);

#endif
