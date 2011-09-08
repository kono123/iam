#include <lvtypes.h>
#include <kbd.h>

#include <stdio.h>
#include <string.h>

const char hexdig[] = {"0123456789ABCDEF"};
const char decdig[] = {"0123456789"};
const U32 pow10[] = {
  1,
  10,
  100,
  1000,
  10000,
  100000,
  1000000,
  10000000,
  100000000,
  1000000000,
};

//unsigned int sum_[3];
U16 sum_[3];
U16 sum_n;

void WriteToAddBuff(unsigned int *ptr, unsigned int words)
{
  if(words>7){
    ptr +=11;
  }else{
    ptr +=4;
  }
  if(sum_n<16){
    sum_[0] +=*ptr++;
    sum_[1] +=*ptr++;
    sum_[2] +=*ptr++;
    sum_n++;
  }
}

void ClearAddBuff(void)
{
  sum_[0]=0;
  sum_[1]=0;
  sum_[2]=0;
  sum_n=0;
}

U16 GetAddBuff(int index)
{
  if(index<3){
    return sum_[index];
  }else{
    return 0;
  }
}

U16 GetCounterAddBuff(void)
{
  return sum_n;
}


U8 StrToU8(char *str)
{
  unsigned char dig;
  U8 ret;
  int len,i;

  len=strlen(str);
  ret=0;
  i=0;
  while(len--){
    dig=str[i]-0x30;
    ret +=dig*pow10[len];
    i++;
  }
  return ret;
}

U16 StrToU16(char *str)
{
  unsigned char dig;
  U16 ret;
  int len,i;

  len=strlen(str);
  ret=0;
  i=0;
  while(len--){
    dig=str[i]-0x30;
    ret +=dig*pow10[len];
    i++;
  }
  return ret;
}

void StrToTemplate(char *d,char *s,int param)
{
  int i=0;
  if(param){
    while(param--){
//      if((s[i]>=KBD_0) && (s[i]<=KBD_9)){
        d[i++]=0xBF;
//      }else{
//        d[i]=s[i];
//      }
//      i++;
    }
    d[i]=0;
  }else{
    while(s[i]!=0){
      if((s[i]>=KBD_0) && (s[i]<=KBD_9)){
        d[i]=0xBF;
      }else{
        d[i]=s[i];
      }
      i++;
    }
    d[i]=0;
  }
}

int isDelimiter(char c)
{
  if((c<KBD_0) || (c>KBD_9)){
    return 1;
  }else{
    return 0;
  }
}

int GetStrToDelimiter(char *s,char *head,char *tail)
{
  int i=0;
  int pos;
  int isdelimiter=0;

//  tail[0]=0;
  if(s[0]==0) return 0;

  while(s[i]!=0){
    if(isdelimiter){
      tail[i-pos-1]=s[i];
    }else{
      head[i]=s[i];
      if(isDelimiter(s[i])){
        head[i]=0;
        isdelimiter=1;
        pos=i;
      }
    }
    i++;
  }
  if(isdelimiter){
    tail[i-pos-1]=0;
  }else{
    head[i]=0;
  }
  return 1;
}

void StrLimitLen(char *source, char *dist, U8 len)
{
  int i;

  for(i=0;i<len;i++){
    dist[i]=source[i];
    if(source[i]==0) break;
  }
  dist[i+1]=0;
}

void StrSpacePading(char *source, char *dist, U8 len)
{
  int i,lens;

  lens=strlen(source);
  for(i=0;i<(len-1);i++){
    if(i<=lens){
      dist[i]=source[i];
    }else{
      dist[i]=' ';
    }
  }
  dist[i+1]=0;
}

void StrZeroPading(char *source, char *dist, U8 len)
{
  int i,lens;

  lens=strlen(source);
  for(i=0;i<(len-1);i++){
    if(i<=lens){
      dist[i]=source[i];
    }else{
      dist[i]=' ';
    }
  }
  dist[i+1]=0;
}
