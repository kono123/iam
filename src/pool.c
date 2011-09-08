#include <hardware.h>
#include <dispatch.h>

#include <stdio.h>
#include <string.h>

#define POOL_DATA_WSIZE   512
#define POOL_DATA_BSIZE   (POOL_DATA_WSIZE<<1)
#define POOL_IO_PART      (POOL_DATA_WSIZE>>1)

unsigned int _pool_data[POOL_DATA_WSIZE];

unsigned int _pool_put_index,_pool_get_index;

void Pool_Init(void)
{
  _pool_put_index=0;
  _pool_get_index=0;
}

void* GetPtrPoolPut(void)
{
  return &_pool_data[_pool_put_index];
}

unsigned int* GetStatusPool(unsigned int *pbytes)
{
  *pbytes=(_pool_put_index-_pool_get_index)<<1;
  return &_pool_data[_pool_get_index];
}

void IncPoolPut(unsigned int words)
{
  __disable_interrupt();
  _pool_put_index +=words;
  if((_pool_put_index>=(POOL_DATA_WSIZE))){
    _pool_put_index -= POOL_DATA_WSIZE;
  }
  __enable_interrupt();
}

void FlushPool(void)
{
  unsigned int words;
  words=_pool_put_index-_pool_get_index;
  SendMsg(FILE_WRITE_DATA_EVENT,(unsigned int)&_pool_data[_pool_get_index],words<<1);
  _pool_put_index=0;
  _pool_get_index=0;
}

void WriteToPool(unsigned int *ptr, unsigned int words)
{
  unsigned int tail,len;

  if((_pool_put_index+words)>POOL_DATA_WSIZE){
    tail=POOL_DATA_WSIZE-_pool_put_index;
//    memcpy(&_pool_data[_pool_put_index],ptr,tail<<1);
    dmemcpy(&_pool_data[_pool_put_index],ptr,tail);

    ptr += tail;
    words -= tail;
//    memcpy(_pool_data,ptr,words<<1);
    dmemcpy(_pool_data,ptr,words);
    _pool_put_index = words;
  }else{
//    memcpy(&_pool_data[_pool_put_index],ptr,words<<1);
    dmemcpy(&_pool_data[_pool_put_index],ptr,words);
    _pool_put_index += words;
    if(_pool_put_index>=POOL_DATA_WSIZE) _pool_put_index=0;
  }

  if(_pool_put_index>_pool_get_index){
    len=_pool_put_index-_pool_get_index;
  }else{
    len=POOL_DATA_WSIZE-_pool_get_index+_pool_put_index;
  }

  if(len>=POOL_IO_PART){
    SendMsg(FILE_WRITE_DATA_EVENT,(unsigned int)&_pool_data[_pool_get_index],POOL_IO_PART<<1);
    if(_pool_get_index){
      _pool_get_index = 0;
    }else{
      _pool_get_index = POOL_IO_PART;
    }
  }
}

