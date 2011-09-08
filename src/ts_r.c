#include "hardware.h"
#include "iam.h"
#include <vcl.h>
#include <dispatch.h>
#include <timeslot.h>
#include <ts_r.h>
#include <ts_func.h>
#include <ts_option.h>
#include <disk_sd.h>
#include <pool.h>
#include <utils.h>

#include <stdio.h>
#include <string.h>

#define R1 1
#define R2 2
#define R3 4
#define R4 8
#define R5 16
#define R6 32
#define R7 64
#define R8 128

typedef void (*ONCOPYDATAW)(unsigned int *ptr, unsigned int words);

ONCOPYDATAW OnCopyDate;

U16 rrr[7*4];
U16 *data_buff;
U32 *data_buff_calibr;

U16 ubase;
U16 r_index;
//U16 r_buff_index;
//U16 r_cancel_index=0;
U16 r_stop_lo_index  = 0;
U32 r_second_counter = 0;
U32 r_second_counter2 = 0;
U32 r_minute_counter = 0;
U32 r_time_stamp;
U16 r_timer_index;
U16 r_counter=0;
U16 r_type_research=0;
//U16 r_transfer=32;
U16 r_transfer=22;
U32 research_duration;
//U16 *prtpooldata=0;
U16 _h,_m;
char _strTime[6];
U16 IsShowTime = 0;

//EmbeddedFile file_w;
FILE file_w;
U16 r_mask_event;
U16 r_prev_mask_event;
TTimeSlotRec *pCurTimeSlot;

extern U16 Batery;
extern TIamEnvironment iam;

//__no_init TGscanConfig _gconf @ 0x1040;      //Segment C
//__no_init TCalibrRRecord _gcalibr @ 0x1000;  //Segment D
//__no_init TCalibrPHRecord _gphcalibrbd[MAX_RECORD_PH_PROBE] @ 0x7C00;

extern TGscanConfig _gconf;      //Segment C
extern TCalibrRRecord _gcalibr;  //Segment D
extern TCalibrPHRecord _gphcalibrbd[MAX_RECORD_PH_PROBE];


const TIamResearchDataConfig _gdconf={
  {
    TAG_IAM_DATA_CONFIG_R,sizeof(TResearchDataConfig)-4,
    50,14,0,0,0,0,
  },
  {
    TAG_IAM_DATA_CONFIG_PH,sizeof(TResearchDataConfig)-4,
    50,12,0,0,0,0,
  }
};

const U16 CalibrR[7]={
  9900,
  5020,
  1890,
  1005,
   540,
   200,
   100
};

extern const float CalibrPHSolutionMin[4];
extern const float CalibrPHSolutionMax[4];

extern char MedicalCard[12];

void LastTimeSlot(PStageRec sr);
void Rmetr(PStageRec sr);
void Rmetr0(PStageRec sr);
void Rmetr1(PStageRec sr);
void ViewR(PStageRec sr);
void ViewTime(PStageRec sr);
void tsKbdScan(PStageRec sr);
void UBase(PStageRec sr);
void RWrite(PStageRec sr);
void pHmetr0(PStageRec sr);
void pHmetr(PStageRec sr);
void pHWrite(PStageRec sr);
void EventWrite(PStageRec sr);
void ts_pH_enable(PStageRec sr);

const TStageRec RStageRec[MAX_R_CYCLES]={
/*00*/  {Rmetr0,R1,R2}, // отключаем pH
/*01*/  {Rmetr,R2,R1},
/*02*/  {Rmetr,R1,R2},
/*03*/  {Rmetr,R2,R1},

/*04*/  {Rmetr,R2,R3},
/*05*/  {Rmetr,R3,R2},
/*06*/  {Rmetr,R2,R3},
/*07*/  {Rmetr,R3,R2},

/*08*/  {Rmetr,R3,R4},
/*09*/  {Rmetr,R4,R3},
/*10*/  {Rmetr,R3,R4},
/*11*/  {Rmetr,R4,R3},

/*12*/  {Rmetr,R4,R5},
/*13*/  {Rmetr,R5,R4},
/*14*/  {Rmetr,R4,R5},
/*15*/  {Rmetr,R5,R4},

/*16*/  {Rmetr,R5,R6},
/*17*/  {Rmetr,R6,R5},
/*18*/  {Rmetr,R5,R6},
/*19*/  {Rmetr,R6,R5},

/*20*/  {Rmetr,R6,R7},
/*21*/  {Rmetr,R7,R6},
/*22*/  {Rmetr,R6,R7},
/*23*/  {Rmetr,R7,R6},

/*24*/  {Rmetr,R7,R8},
/*25*/  {Rmetr,R8,R7},
/*26*/  {Rmetr,R7,R8},
/*27*/  {Rmetr,R8,R7},

/*28*/  {Rmetr,0,0},
/*29*/  {ts_pH_enable,0,0},
/*30*/  {tsReserv,0,0},
/*31*/  {pHmetr,1,0},
/*32*/  {pHmetr,2,0},
/*33*/  {pHmetr,3,0},
/*34*/  {RWrite,0,0},
/*35*/  {tsReserv,0,0},
/*36*/  {tsReserv,0,0},
/*37*/  {EventWrite,0,0},
/*38*/  {tsReserv,0,0},

/*36*/  {tsBatery,0,0},
/*36*/  {tsReserv,0,0},
/*36*/  {tsReserv,0,0},
/*36*/  {tsReserv,0,0},
/*36*/  {tsReserv,0,0},
/*36*/  {tsReserv,0,0},
/*36*/  {tsReserv,0,0},
/*36*/  {tsReserv,0,0},
/*36*/  {tsReserv,0,0},
/*36*/  {tsReserv,0,0},

/*39*/  {LastTimeSlot,0,0}
};

const TTimeSlotRec RTimeSlot ={
  MAX_R_CYCLES,
  40,
  RStageRec
};

void InitTimeSlot(void)
{
  r_mask_event=0;
  r_counter=0;
  r_stop_lo_index=0;
  r_second_counter=0;
}

PTimeSlotRec GetPtrRTimeSlot(void)
{
  return (PTimeSlotRec)&RTimeSlot;
}

const TStageRec pHStageRec[MAX_PH_CYCLES]={
/*00*/  {pHmetr0,0,0},
/*01*/  {tsReserv,0,0},
/*02*/  {pHmetr,1,0},
/*03*/  {pHmetr,2,0},
/*04*/  {pHmetr,3,0},
/*05*/  {pHWrite,0,0},
/*06*/  {tsReserv,0,0},
/*07*/  {EventWrite,0,0},
/*08*/  {tsBatery,0,0},
/*09*/  {LastTimeSlot,0,0}
};

const TTimeSlotRec pHTimeSlot ={
  MAX_PH_CYCLES,
  8,
  pHStageRec
};

void  GetSerialNumberStr(char *str)
{
  strcpy(str,_gconf.SerNumDevice);
}

void  GetDeviceNameStr(char *str)
{
  strcpy(str,_gconf.NameDevice);
}

void  GetVersionFwStr(char *str)
{
  strcpy(str,_gconf.FwVersion);
}

///////////////////////////////////////////////////////////////////////////
//                                                                Probe  //
///////////////////////////////////////////////////////////////////////////

PCalibrPHRecord FindPHProbeByN(char *strN)
{
  int i,len;
  len=strlen(strN);
  for(i=0;i<MAX_RECORD_PH_PROBE;i++){
    if(strncmp(strN,_gphcalibrbd[i].No,len)==0){
      return &_gphcalibrbd[i];
    }
  }
  return 0;
}

char *GetPHProbeStr(U8 index)
{
  if(_gphcalibrbd[index].No[0]==0xFF){
    return 0;
  }else{
    return _gphcalibrbd[index].No;
  }
}

PCalibrPHRecord GetPHProbePtr(U8 index)
{
  if(_gphcalibrbd[index].No[0]==0xFF){
    return 0;
  }else{
    return &_gphcalibrbd[index];
  }
}

PCalibrPHRecord AddPHProbe(PCalibrPHRecord pRec)
{
  int i;
  TCalibrPHRecord tt[MAX_RECORD_PH_PROBE];

  for(i=0;i<MAX_RECORD_PH_PROBE;i++){
    if(_gphcalibrbd[i].No[0]==0xFF){
      memcpy(tt,&_gphcalibrbd,sizeof(tt));
      memcpy(&tt[i],pRec,sizeof(TCalibrPHRecord));
      write_flash((U8*)&tt[0],(U32)&_gphcalibrbd,512);
      return &_gphcalibrbd[i];
    }
  }
  return 0;
}

int UpdatePHProbe(PCalibrPHRecord pRec)
{
  unsigned int p;
  PCalibrPHRecord ptr;
  TCalibrPHRecord tt[MAX_RECORD_PH_PROBE];

  ptr=FindPHProbeByN(pRec->No);
  if(ptr==NULL) return 0;
  p=(unsigned int)tt +(unsigned int)ptr-(unsigned int)&_gphcalibrbd;
  memcpy(tt,&_gphcalibrbd,sizeof(tt));
  memcpy((void *)p,pRec,sizeof(TCalibrPHRecord));
  write_flash((U8*)&tt[0],(U32)&_gphcalibrbd,sizeof(tt));
  return 1;
}

int ErasePHProbe(char *strN)
{
  unsigned int p;
  PCalibrPHRecord ptr;
  TCalibrPHRecord tt[MAX_RECORD_PH_PROBE];

  ptr=FindPHProbeByN(strN);
  if(ptr==NULL) return 0;
  p=(unsigned int)tt +(unsigned int)ptr-(unsigned int)&_gphcalibrbd;
  memcpy(tt,&_gphcalibrbd,sizeof(tt));
  memset((void *)p,-1,sizeof(TCalibrPHRecord));
  write_flash((U8*)&tt[0],(U32)&_gphcalibrbd,sizeof(tt));
  return 1;
}

void EraseAllProbe(void)
{
  TCalibrPHRecord tt[MAX_RECORD_PH_PROBE];
  memset(tt,-1,sizeof(tt));
  write_flash((U8*)&tt[0],(U32)&_gphcalibrbd,sizeof(tt));
}

int ProbeToFile(char *strN)
{
  PCalibrPHRecord ptr;
//  EmbeddedFile file_probe;
  FILE file_probe;
  U16 tt[2];
  char ss[16];
  unsigned int write_bytes;

  if(DISK_Mount()) return 1;

  ptr=FindPHProbeByN(strN);
  if(ptr==NULL) return 0;
  strcpy(ss,strN);
  strcat(ss,".prb");
  if(FS_FileOpen(&file_probe,ss,'w')){
     MsgBoxEx("Ошибка","записи файла!",MBF_OK);
     return 2;
  }
  tt[0]=TAG_IAM_PROBE_PH;
  tt[1]=sizeof(TCalibrPHRecord);

  if(FS_FileWrite(&file_probe,4,&write_bytes,(U8*)tt)){
     MsgBoxEx("Ошибка","записи файла!",MBF_OK);
     return 2;
  }
  if(FS_FileWrite(&file_probe,sizeof(TCalibrPHRecord),&write_bytes,(U8*)ptr)){
     MsgBoxEx("Ошибка","записи файла!",MBF_OK);
     return 2;
  }
  FS_FileClose(&file_probe);
  DISK_Unmount();

  return 0;
}

int FileToProbe(char *strN)
{
  unsigned int p;
  PCalibrPHRecord ptr;
  TCalibrPHRecord tt[MAX_RECORD_PH_PROBE];

  ptr=FindPHProbeByN(strN);
  if(ptr==NULL) return 0;
  p=(unsigned int)tt +(unsigned int)ptr-(unsigned int)&_gphcalibrbd;
  memcpy(tt,&_gphcalibrbd,sizeof(tt));
  memset((void *)p,-1,sizeof(TCalibrPHRecord));
//  write_flash((U8*)&tt[0],(U32)&_gphcalibrbd,sizeof(tt));
  return 1;
}

PTimeSlotRec GetPtrPHTimeSlot(void)
{
  return (PTimeSlotRec)&pHTimeSlot;
}

void Rmetr0(PStageRec sr)
{
  int i;
  TP1_LO();
  PH_DISABLE();
  ADC_ConnectR(sr->Param1,sr->Param2);
  data_buff[0]=TAG_IAM_DATA_7R_3PH; //0xABBA;
  data_buff[1]=r_transfer-4;
  data_buff[2]=r_counter;
  data_buff[3]=Batery;//r_mask_event;
  for(i=4;i<MAX_ITEMS;i++) data_buff[i]=0;
  r_index=4*4;
}

void ts_pH_enable(PStageRec sr)
{
  PH_ENABLE();
}

void pHmetr0(PStageRec sr)
{
  int i;
  TP1_LO();
  data_buff[0]=TAG_IAM_DATA_3PH;
  data_buff[1]=r_transfer-4;
  data_buff[2]=r_counter;
  data_buff[3]=Batery;
  for(i=4;i<MAX_ITEMS;i++) data_buff[i]=0;
  r_index=4*4;
}

void pHmetr(PStageRec sr)
{
  PH_ENABLE();
  R_DISABLE();
//  if(r_type_research==10) return;
  data_buff[r_index>>2] = ADC_pH(sr->Param1);
  r_index+=4;
}

void Rmetr(PStageRec sr)
{
//  rr[rindex>>2] += ADC_R(sr->Param1,sr->Param2);
  data_buff[r_index>>2] += ADC_R_();
  ADC_ConnectR(sr->Param1,sr->Param2);
  r_index++;
}

void ViewR(PStageRec sr)
{
//  TP3_HI();
  if(r_type_research) return;
  memcpy(rrr,data_buff,14);
  SetDoorBell(VIEWR_DB_MASK);
}

void pHWrite(PStageRec sr)
{
//  U32 d;
  TP4_LO();

//  WriteToPool(data_buff,r_transfer>>1);
  if(OnCopyDate) OnCopyDate((unsigned int*)data_buff,r_transfer>>1);

  TP4_HI();
  TP1_HI(); // отключение
}

void RWrite(PStageRec sr)
{
  int i;
  U32 d;
  TP4_LO();

  if(r_type_research>=10){
    if(r_type_research==10){
      if(r_counter<256){
        for(i=0;i<MAX_R_SENSORS;i++){
          d=data_buff[i+4];
          data_buff_calibr[i] +=d;
        }
      }
    }
  }else{
    if(OnCopyDate) OnCopyDate((unsigned int*)data_buff,r_transfer>>1);
  }
  TP4_HI();
  TP1_HI();
}

void EventWrite(PStageRec sr)
{
  U32 dt;
  if(r_mask_event!=r_prev_mask_event){
    r_prev_mask_event=r_mask_event;
    data_buff[0]=TAG_IAM_EVENT;
    data_buff[1]=6;
    data_buff[2]=r_mask_event;
    dt=RTC_getDateTime();
    data_buff[3]=dt;
    data_buff[4]=dt>>16;
    WriteToPool(data_buff,5);
  }
}

void LastTimeSlot(PStageRec sr)
{
  r_counter++;
  r_stop_lo_index++;
  if(r_stop_lo_index<pCurTimeSlot->SecondCount) return;
  r_stop_lo_index=0;
  r_second_counter++;
  if(r_second_counter2<60){
    r_second_counter2++;
  }else{
    r_minute_counter++;
    r_second_counter2=0;
    if(IsShowTime){
      _h = r_minute_counter / 60;
      _m = r_minute_counter - (_h*60);
      sprintf(_strTime,"%02d:%02d",_h,_m);
      _TextXY(12,3,_strTime,0);
      LCD_Update(NULL);
    }
  }
  if(r_second_counter>=research_duration){
    StopTimeSlot();
    r_second_counter=0;
    r_stop_lo_index=0;
    r_counter=0;
    SendMsg(VCL_OK_EVENT,0,0);
  }
  if(r_second_counter>=r_time_stamp){
    r_time_stamp+=MAX_TIME_STAMP;
    if(r_mask_event & 0x8000){
      r_mask_event &= 0x7FFF;
    }else{
      r_mask_event |= 0x8000;
    }
  }
}

///////////////////////////////////////////////////////////////////////////
//                                                             Research  //
///////////////////////////////////////////////////////////////////////////

//static int _rretval;
U16 mask_event;

void ResearchKbdHandler(U8 key)
{
  char ss[2];

  switch(key){

    case KBD_0:
    case KBD_1:
    case KBD_2:
    case KBD_3:
    case KBD_4:
    case KBD_5:
    case KBD_6:
    case KBD_7:
    case KBD_8:
    case KBD_9:
      if(r_mask_event & (1<<(key-0x30))){
        r_mask_event &= ~(1<<(key-0x30));
        _TextXY(8+key-0x30,2,"\x82",0);
      }else{
        r_mask_event |= 1<<(key-0x30);
        ss[0]=(char)key;
        ss[1]=0;
        _TextXY(8+key-0x30,2,ss,0);
      }
      r_time_stamp+=MAX_TIME_STAMP;
      LCD_Update(NULL);
    break;

    case KBD_STOP:
      VCL_SetTimer(80);
      IsShowTime = 0;
      if(MsgBox("Прервать?",MBF_OKCANCEL)==VCL_RET_OK){
        StopTimeSlot();
        VCL_SetRetVal(VCL_RET_CANCEL);
        VCL_FreeMutex();
      }else{
        IsShowTime = 1;
      }
    break;
  }
}

void DataToSD(P_Message pmes)
{
  unsigned int write_bytes;
  if(FS_FileWrite(&file_w,pmes->Param2,&write_bytes,(U8*)pmes->Param1)){
    SendMsg(FILE_WRITE_ERROR_EVENT,0,0);
  }
}

int PrepareFile(int rtype)
{
  char ss[32];
  char ss1[32];
  U16 sig;
  U8 enum_file=0;
  TResearchConfig rconf;
  TIamResearchDataConfig dconf;
  PCalibrPHRecord prec;
  unsigned int write_bytes;

  if(DISK_Mount()) return 1;

  memcpy(&dconf,&_gdconf,sizeof(_gdconf));
  dconf.R.SamplePerSec=RTimeSlot.SecondCount;
  dconf.pH.SamplePerSec=pHTimeSlot.SecondCount;
  RTC_getDateStr(ss);
  if(FS_MakeDir(ss)){
//    MessageBox("dir err",MBF_OK);
//    return 3;
  }
  sprintf(ss1,"%s/iam_r%02d.dat",ss,enum_file);

  while(FS_FileOpen(&file_w,ss1,'w')){
      enum_file++;
      sprintf(ss1,"%s/iam_r%02d.dat",ss,enum_file);
      if(enum_file>250){
       MsgBoxEx("ошибка записи","файла!!!",MBF_OK);
       return 2;
      }
  }

  Pool_Init();

  WriteToPool((unsigned int*)&_gconf,sizeof(_gconf)>>1);
//  FS_FileWrite(&file_w,sizeof(_gconf),&write_bytes,(U8*)&_gconf);

  switch(rtype){
    case 0:
      rconf.MeasuringTag=TAG_IAM_DATA_3PH;
    break;

    case 1:
      rconf.MeasuringTag=TAG_IAM_DATA_7R_3PH;
    break;
    default:
      rconf.MeasuringTag=0;
    break;
  }
  rconf.Tag=TAG_IAM_RESEARCH_EX; //TAG_IAM_RESEARCH;
  rconf.Bytes=sizeof(TResearchConfig)-4;
  strcpy(rconf.MedicalCard,iam.MedicalCard);
  rconf.StartTime=RTC_getDateTime();
  rconf.PositionPHProbe=iam.PositionPHProbe;

  WriteToPool((unsigned int*)&rconf,sizeof(rconf)>>1);
//  FS_FileWrite(&file_w,sizeof(rconf),&write_bytes,(U8*)&rconf);

  if(rtype){
    WriteToPool((unsigned int*)&dconf.R,sizeof(dconf.R));
//    FS_FileWrite(&file_w,sizeof(dconf.R)<<1,&write_bytes,(U8*)&dconf.R);
  }else{
    WriteToPool((unsigned int*)&dconf.pH,sizeof(dconf.pH)>>1);
//    FS_FileWrite(&file_w,sizeof(dconf.pH),&write_bytes,(U8*)&dconf.pH);
  }

  prec=FindPHProbeByN(iam.NumberZond);
  sig=TAG_IAM_CALIBR_PH;
  WriteToPool(&sig,1);
//  FS_FileWrite(&file_w,2,&write_bytes,(U8*)&sig);

  sig=sizeof(TCalibrPHRecord);
  WriteToPool(&sig,1);
//  FS_FileWrite(&file_w,2,&write_bytes,(U8*)&sig);

  WriteToPool((unsigned int*)prec,sizeof(TCalibrPHRecord)>>1);
//  FS_FileWrite(&file_w,sizeof(TCalibrPHRecord),&write_bytes,(U8*)prec);
  if(rtype){
    WriteToPool((unsigned int*)&_gcalibr,sizeof(_gcalibr)>>1);
//    FS_FileWrite(&file_w,sizeof(_gcalibr),&write_bytes,(U8*)&_gcalibr);
  }
/*
  FS_FileClose(&file_w);

  if(FS_FileOpen(&file_w,ss1,'a')){
    MessageBox("ошибка файл a!",MBF_OK);
    return 1;
  }
  */
//  Pool_Init();
  return 0;
}

int FinaleFile(int type,U8 flag)
{
  TIamFinalResearchRecord fr;
  unsigned int write_bytes;
  unsigned int *ptr,bytes;

  ptr=GetStatusPool(&bytes);
  FS_FileWrite(&file_w,bytes,&write_bytes,(U8*)ptr);

  fr.Tag=TAG_IAM_FINAL_RESEARCH;
  fr.Bytes=sizeof(fr)-4;
  fr.FinishTime=RTC_getDateTime();
  fr.Flags=flag;
  FS_FileWrite(&file_w,sizeof(fr),&write_bytes,(U8*)&fr);
  FS_FileClose(&file_w);
  DISK_Unmount();
  return 0;
}

extern const char r_type_str[MAX_TYPE_RESEARCH][16];

void PreViewKbdHandler(U8 key)
{
  switch(key){

    case KBD_OK:
//      VCL_SetRetVal(VCL_RET_OK);
      VCL_SetRetVal(VCL_RET_OK2);
      VCL_FreeMutex();
    break;
    case KBD_CANCEL:
      VCL_SetRetVal(VCL_RET_CANCEL);
      VCL_FreeMutex();
    break;
  }
}

void DummyH(P_Message pmes)
{
  return;
}

float CodeTomV(U16 code)
{
  return 612.65-0.309*code;
}

float CodeTopH(U16 code,U8 nProbe)
{
  float a,b,d;
  PCalibrPHRecord p;
  p=FindPHProbeByN(iam.NumberZond);
  if(p==NULL) return 0;              //???
  d=p->pHProbe[1].Code[nProbe] ;
  b=code;
  if(b<d){
    d-=p->pHProbe[3].Code[nProbe];
    if(d==0){
      return 555;  //?????
    }
    a=p->pHProbe[1].pH;
    a-=p->pHProbe[3].pH;
    a=a/d;
    b = p->pHProbe[3].pH;
    b *=p->pHProbe[1].Code[nProbe];
    b-= 1.0*p->pHProbe[1].pH * p->pHProbe[3].Code[nProbe];
  }else{
    d-=1983;
    if(d==0){
      return 555; // ???
    }
    a=p->pHProbe[1].pH;
    a=a/d;
    b = -1.0*p->pHProbe[1].pH * 1983;
  }
  b=b/d;
  return (a*code+b)/100.0;
}

void DebugViewKbdHandler(U8 key)
{
  switch(key){

    case KBD_OK:
//      VCL_SetRetVal(VCL_RET_OK);
      VCL_SetRetVal(VCL_RET_OK2);
      VCL_FreeMutex();
    break;
    case KBD_CANCEL:
      VCL_SetRetVal(VCL_RET_CANCEL);
      VCL_FreeMutex();
    break;
  }
}

int DebugViewResearch(int type,U8 NProbes)
{
  char ss[MAX_COLS+1];
  int _vrmb[MAX_ROWS*MAX_COLS];
  FKBDHANDLER prvkbdh;
  int i,ret;
  int _mutex;
  U16 data_buff_[MAX_ITEMS];
  U16 dd;
  U32 tt;

  LCD_CopyVram(_vrmb);

  data_buff=data_buff_;
  research_duration=1;
  r_index=4*4;
  r_type_research=type;
  InitTimeSlot();

  _ClearScreen();

  if(type){
    _TextXY(0,0,"Импеданс",0);
    r_transfer=8+14+6;
  }else{
    _TextXY(0,0,"pH",0);
    r_transfer=8+6;
  }

  LCD_Update(NULL);

  OnCopyDate=WriteToAddBuff;
  ClearAddBuff();
  SetOnWriteData(DummyH);

  ret=VCL_RET_OK;
  prvkbdh=DISPATCH_SetKbdHandler(DebugViewKbdHandler);
  if(type){
    pCurTimeSlot=(PTimeSlotRec)&RTimeSlot;
  }else{
    pCurTimeSlot=(PTimeSlotRec)&pHTimeSlot;
  }
//  ClearAddBuff();
//  StartTimeSlot(pCurTimeSlot);

  while(ret==VCL_RET_OK){
    _mutex=VCL_SetMutex();
    research_duration=1;
    ClearAddBuff();
    StartTimeSlot(pCurTimeSlot);
    VCL_WaitMutex(_mutex);
    ret=VCL_GetRetVal();
    for(i=0;i<NProbes;i++){
//      tt=GetAddBuff(i);
      tt=GetAddBuff(i)/GetCounterAddBuff();
//      dd=GetCounterAddBuff();
      if(type){
//        sprintf(ss,"%d: %4.0f mV   ",i,CodeTomV(data_buff[11]));
      sprintf(ss,"%d: %4.0f mV   ",i+1,CodeTomV(tt));
//        sprintf(ss,"%d: %lx %x %x   ",i,tt,data_buff[11+i],dd);
      }else{
//        sprintf(ss,"%d: %4.0f mV   ",i,CodeTomV(data_buff[4+i]));
      sprintf(ss,"%d: %4.0f mV   ",i+1,CodeTomV(tt));
//        sprintf(ss,"%d: %x %x %x   ",i,tt,data_buff[4+i],dd);
      }
      _TextXY(0,i+1,ss,0);
    }
    LCD_Update(NULL);
  }

  LCD_Update(_vrmb);
  ret=VCL_GetRetVal();
  DISPATCH_SetKbdHandler(prvkbdh);
  StartTimeSlot(GetPtrOptionTimeSlot());
  ADC_ConnectR(0,0);

  return ret;
}

int PreViewResearch(int type,U8 NProbes)
{
  char ss[MAX_COLS+1];
  int _vrmb[MAX_ROWS*MAX_COLS];
  FKBDHANDLER prvkbdh;
  int i,ret;
  int _mutex;
  U16 data_buff_[MAX_ITEMS];

//  _mutex=VCL_SetMutex();
  LCD_CopyVram(_vrmb);

  data_buff=data_buff_;
  research_duration=1;
  r_index=4*4;
  r_type_research=type;
  InitTimeSlot();

  OnCopyDate=0;
  SetOnWriteData(DummyH);

  _ClearScreen();
  _TextXY(0,0,"Начать запись?",0);
  LCD_Update(NULL);

  ret=VCL_RET_OK;

  if(type){
    pCurTimeSlot=(PTimeSlotRec)&RTimeSlot;
  }else{
    pCurTimeSlot=(PTimeSlotRec)&pHTimeSlot;
  }
  prvkbdh=DISPATCH_SetKbdHandler(PreViewKbdHandler);
  StartTimeSlot(pCurTimeSlot);

  while(ret==VCL_RET_OK){
    _mutex=VCL_SetMutex();
    research_duration=0;
    StartTimeSlot(pCurTimeSlot);
    VCL_WaitMutex(_mutex);
    ret=VCL_GetRetVal();
    for(i=0;i<NProbes;i++){
      if(type){
        sprintf(ss,"%d:%4.2f pH   ",i+1,CodeTopH(data_buff[11+i],i));
      }else{
        sprintf(ss,"%d:%4.2f pH   ",i+1,CodeTopH(data_buff[4+i],i));
      }
      _TextXY(0,i+1,ss,0);
    }
    LCD_Update(NULL);
  }

  LCD_Update(_vrmb);
//  ret=VCL_GetRetVal();
  DISPATCH_SetKbdHandler(prvkbdh);
  StartTimeSlot(GetPtrOptionTimeSlot());
  return ret;
}
///////////////////////////////////////////////////////////////////////////
//                                                          Research     //
///////////////////////////////////////////////////////////////////////////
int Research(char *Title,int type,U32 duration)
{
  char ss[MAX_COLS+1];
  int _vrmb[MAX_ROWS*MAX_COLS];
  FKBDHANDLER prvkbdh;
  int i,ret;
  int _mutex;
  U8 flag;
  U16 data_buff_[MAX_ITEMS];

  _mutex=VCL_SetMutex();
  LCD_CopyVram(_vrmb);
  data_buff=data_buff_;
  research_duration=duration;
  r_index=4*4;
  r_type_research=type;
  InitTimeSlot();
  r_prev_mask_event=0;

  r_time_stamp=MAX_TIME_STAMP;
  r_stop_lo_index=r_second_counter=0;
  r_second_counter2 = 0;
  r_minute_counter  = 0;
  IsShowTime = 1;
  if(type){
    r_transfer=8+14+6;
  }else{
    r_transfer=8+6;
  }
  VCL_SetRetVal(VCL_RET_OK);

  OnCopyDate=WriteToPool;

  Pool_Init();
  SetOnWriteData(DataToSD);

  for(i=0;i<(MAX_COLS-2);i++){
    ss[i]=Title[i];
    if(Title[i]==0) break;
  }
  ss[i+1]=0;
  _ClearScreen();
  _TextXY(0,0,ss,0);
  _TextXY(0,1,(char*)r_type_str[type],0);
  _TextXY(0,2,"Метки:  \x82\x82\x82\x82\x82\x82\x82\x82\x82\x82",0);
  LCD_Update(NULL);

  PrepareFile(r_type_research);

  prvkbdh=DISPATCH_SetKbdHandler(ResearchKbdHandler);
  if(type){
    pCurTimeSlot=GetPtrRTimeSlot();
  }else{
    pCurTimeSlot=GetPtrPHTimeSlot();
  }
  StartTimeSlot(pCurTimeSlot);

  SetSilenceCounter(MAX_SILENCE_COUNTER);

  VCL_WaitMutex(_mutex);
  SetSilenceCounter(0);

  flag=0;
  if(VCL_GetRetVal()==VCL_RET_POWER_ERROR){
    flag=2;
  }else if(VCL_GetRetVal()==VCL_RET_WRITE_ERROR){
    flag=3;
  }else if(VCL_GetRetVal()==VCL_RET_CANCEL){
    flag=1;
  }
  IsShowTime = 0;
  FinaleFile(r_type_research,flag);
  LCD_Update(_vrmb);
   DISPATCH_SetKbdHandler(prvkbdh);
  ret=VCL_GetRetVal();
  StartTimeSlot(GetPtrOptionTimeSlot());
  return ret;
}
///////////////////////////////////////////////////////////////////////////
//                                                         Calibrate     //
///////////////////////////////////////////////////////////////////////////
void DataToCalibrRec(P_Message pmes)
{
//  unsigned int write_bytes;
//  FS_FileWrite(&file_w,pmes->Param2,&write_bytes,(U8*)pmes->Param1);
//  prtpooldata=(U16*)pmes->Param1;
  return;
}

void WriteCalibrR(void)
{
  TCalibrRRecord _cr;
  int i;

  memcpy(&_cr,&_gcalibr,sizeof(_gcalibr));
  _cr.RDateTime=RTC_getDateTime();

  for(i=0;i<MAX_R_SENSORS;i++){
    _cr.Code[i]=(data_buff_calibr[i]+128)>>8; //округление
    _cr.R[i]=CalibrR[i];
  }
  write_SegD((U8*)&_cr,0,sizeof(_gcalibr));
}

void WriteCalibrPH(U8 nSulution,U8 npHSensors)
{
  TCalibrPHRecord cr,*p;
  int i,j;

  p=FindPHProbeByN(iam.NumberZond);
  if(p==NULL) return;
  memcpy(&cr,p,sizeof(TCalibrPHRecord));
//  strcpy(cr.No,iam.NumberZond);
  cr.DateTime=RTC_getDateTime();
  //TODO

  for(i=0;i<npHSensors;i++){
    cr.pHProbe[nSulution].Code[i]=(data_buff_calibr[i]+128)>>8;
  }
  cr.pHProbe[nSulution].pH=CalibrR[0];

  UpdatePHProbe(&cr);
}

//esint8 file_fclose2(FILE *file,euint32 size);


int Calibrate(char *Title,int type,U32 duration)
{
  char ss[MAX_COLS+1];
//  char ss1[128];
  int _vrmb[MAX_ROWS*MAX_COLS];
  FKBDHANDLER prvkbdh;
  int i,ret;
  int _mutex;
  U16 data_buff_[MAX_ITEMS+4];
  U32 data_buff_calibr_[MAX_ITEMS];

  for(i=0;i<MAX_R_SENSORS;i++) data_buff_calibr_[i]=0L;
  data_buff=data_buff_;
  data_buff_calibr=data_buff_calibr_;
  _mutex=VCL_SetMutex();
  LCD_CopyVram(_vrmb);
  research_duration=duration;
  r_index=4*4;
//  r_buff_index=0;
  r_type_research=type;
  InitTimeSlot();
  VCL_SetRetVal(VCL_RET_OK);

  SetOnWriteData(DataToCalibrRec);

  for(i=0;i<(MAX_COLS-2);i++){
    ss[i]=Title[i];
    if(Title[i]==0) break;
  }
  ss[i+1]=0;
  LCD_ScreenRect();
  _TextXY(1,1,ss,0);
  LCD_Update(NULL);
  prvkbdh=DISPATCH_SetKbdHandler(CancelKbdHandler);

  pCurTimeSlot=GetPtrRTimeSlot();//(PTimeSlotRec)&RTimeSlot;
  StartTimeSlot(pCurTimeSlot);

  VCL_WaitMutex(_mutex);

//  ret=VCL_GetRetVal();
  ret = 1;
  if(VCL_GetRetVal()!=VCL_RET_CANCEL){
    i=(data_buff_calibr[0]-data_buff_calibr[MAX_R_SENSORS-1])>>8;
//    printf("i=%x\n",i);
    if(i>5000){
      ret = 0;
      WriteCalibrR();
    }
  }
/*
  if(type==10){
    WriteCalibrR();
  }else{
//    WriteCalibrPH();
  }
*/
  LCD_Update(_vrmb);
  DISPATCH_SetKbdHandler(prvkbdh);
//  ret=VCL_GetRetVal();
  StartTimeSlot(GetPtrOptionTimeSlot());
  return ret;
}

#define MAX_CAL_BUFF 16
#define CALIBR_DELTA 30 //17

int CalibratePH(char *Title,int type,U8 NProbes,U16 *ptr,U8 nSulution)
{
  char ss[MAX_COLS+1];
  int _vrmb[MAX_ROWS*MAX_COLS];
  FKBDHANDLER prvkbdh;
  int i,j,k,ret;
  int ok_flag=0;
  int _mutex;
  U16 data_buff_[MAX_ITEMS+4];
  U32 data_buff_calibr_[MAX_ITEMS];
  U16 cal_buff[MAX_PH_SENSORS][MAX_CAL_BUFF];
  U16 cal_buff_max[MAX_PH_SENSORS];
  U16 cal_buff_min[MAX_PH_SENSORS];
  U8  index_cal_buff=0;
  U16 dd;
  U16 tt;

//  _mutex=VCL_SetMutex();
  LCD_CopyVram(_vrmb);

  for(i=0;i<MAX_PH_SENSORS;i++){
      data_buff_calibr_[i]=0L;
  }
  data_buff=data_buff_;
  data_buff_calibr=data_buff_calibr_;
//  research_duration=duration;
  r_index=4*4;
  r_type_research=type;
  InitTimeSlot();
  VCL_SetRetVal(VCL_RET_OK);

//  strcpy(ss,"Раствор ");
//  strcat(ss,Title);
  _ClearScreen();
  _TextXY(0,1,Title,0);
  LCD_Update(NULL);

  OnCopyDate=WriteToAddBuff;
  ClearAddBuff();
  SetOnWriteData(DataToCalibrRec);

  if(type){
    pCurTimeSlot=(PTimeSlotRec)&RTimeSlot;
    r_transfer=8+14+6;
  }else{
    pCurTimeSlot=(PTimeSlotRec)&pHTimeSlot;
    r_transfer=8+6;
  }
//  StartTimeSlot(pCurTimeSlot);
  prvkbdh=DISPATCH_SetKbdHandler(CancelKbdHandler);

  for(i=0;i<(30-MAX_CAL_BUFF);i++){
    sprintf(ss,"Ждем %03d сек ",30-i);
    _TextXY(0,2,ss,0);
    LCD_Update(NULL);
    _mutex=VCL_SetMutex();
    research_duration=0;
    ClearAddBuff();
    StartTimeSlot(pCurTimeSlot);
    VCL_WaitMutex(_mutex);
    if(VCL_GetRetVal()==VCL_RET_CANCEL){
      LCD_Update(_vrmb);
      DISPATCH_SetKbdHandler(prvkbdh);
      StartTimeSlot(GetPtrOptionTimeSlot());
      return 1;
    }
  }

  for(i=0;i<MAX_CAL_BUFF;i++){
    sprintf(ss,"Ждем %03d сек ",MAX_CAL_BUFF-i);
    _TextXY(0,2,ss,0);
    LCD_Update(NULL);
    _mutex=VCL_SetMutex();
    research_duration=1;
    ClearAddBuff();
    StartTimeSlot(pCurTimeSlot);
    VCL_WaitMutex(_mutex);
    if(VCL_GetRetVal()==VCL_RET_CANCEL){
      LCD_Update(_vrmb);
      DISPATCH_SetKbdHandler(prvkbdh);
      StartTimeSlot(GetPtrOptionTimeSlot());
      return 1;
    }
    for(j=0;j<NProbes;j++){
      tt=GetAddBuff(j)/GetCounterAddBuff();
      if(type){
//        cal_buff[j][i]=data_buff[11+j];
        cal_buff[j][i]=tt;
      }else{
//        cal_buff[j][i]=data_buff[4+j];
        cal_buff[j][i]=tt;
      }
    }
  }

  _ClearScreen();
  strcpy(ss,"Калибровка");
  strcat(ss,&Title[7]);
  _TextXY(0,0,ss,0);

  for(j=0;j<NProbes;j++){
    cal_buff_min[j]=0xFFFF;
    cal_buff_max[j]=0x0000;
    for(i=0;i<MAX_CAL_BUFF;i++){
      if(cal_buff_min[j]>cal_buff[j][i]){
        cal_buff_min[j]=cal_buff[j][i];
      }
      if(cal_buff_max[j]<cal_buff[j][i]){
        cal_buff_max[j]=cal_buff[j][i];
      }
    }
  }

  index_cal_buff=0;
  for(k=0;k<255;k++){
    for(j=0;j<NProbes;j++){
      tt=GetAddBuff(j)/GetCounterAddBuff();
      if(type){
//        sprintf(ss,"%d:%4.0f mV (%3.0f)",j,CodeTomV(data_buff[11+j]),CodeTomV(cal_buff_max[j]-cal_buff_min[j]));
//        sprintf(ss,"%d:%4.0f mV",j,CodeTomV(data_buff[11+j]));
//        cal_buff[j][index_cal_buff]=data_buff[11+j];
        sprintf(ss,"%d:%4.0f mV",j+1,CodeTomV(tt));
        cal_buff[j][index_cal_buff]=tt;
      }else{
//        sprintf(ss,"%d:%4.0f mV (%3.0f)",j,CodeTomV(data_buff[4+j]),CodeTomV(cal_buff_max[j]-cal_buff_min[j]));
//        sprintf(ss,"%d:%4.0f mV",j,CodeTomV(data_buff[4+j]));
//        cal_buff[j][index_cal_buff]=data_buff[4+j];
        sprintf(ss,"%d:%4.0f mV",j+1,CodeTomV(tt));
        cal_buff[j][index_cal_buff]=tt;
      }
//      sprintf(ss,"%d:%x(%x)  ",j,data_buff[11+j],cal_buff_max[j]-cal_buff_min[j]);
      _TextXY(0,j+1,ss,0);
    }
    index_cal_buff++;
    if(index_cal_buff>=MAX_CAL_BUFF) index_cal_buff=0;
    LCD_Update(NULL);
    _mutex=VCL_SetMutex();
    research_duration=1;
    ClearAddBuff();
    StartTimeSlot(pCurTimeSlot);
    VCL_WaitMutex(_mutex);
    if(VCL_GetRetVal()==VCL_RET_CANCEL){
      LCD_Update(_vrmb);
      DISPATCH_SetKbdHandler(prvkbdh);
      StartTimeSlot(GetPtrOptionTimeSlot());
      return 1;
    }
    ok_flag=0;

    for(j=0;j<NProbes;j++){
      cal_buff_min[j]=0xFFFF;
      cal_buff_max[j]=0;
      for(i=0;i<MAX_CAL_BUFF;i++){
        if(cal_buff_min[j]>cal_buff[j][i]){
          cal_buff_min[j]=cal_buff[j][i];
        }
        if(cal_buff_max[j]<cal_buff[j][i]){
          cal_buff_max[j]=cal_buff[j][i];
        }
      }
      if((cal_buff_max[j]-cal_buff_min[j])<CALIBR_DELTA){
        ok_flag++;
      }
    }
    if(ok_flag==NProbes) break;
  }

  ret=0;
  if(ok_flag==NProbes){
      for(j=0;j<NProbes;j++){
        dd=0;
        for(i=0;i<MAX_CAL_BUFF;i++) dd+=cal_buff[j][i];
        ptr[j] = dd>>4;
        if((CodeTomV(ptr[j])<CalibrPHSolutionMin[nSulution]) || (CodeTomV(ptr[j])>CalibrPHSolutionMax[nSulution])){
          ret|=2;
          break;
        }
      }
  }else{
    ret=1;
  }

  delay_ms(3000);

  LCD_Update(_vrmb);
  DISPATCH_SetKbdHandler(prvkbdh);
//  ret=VCL_GetRetVal();
  StartTimeSlot(GetPtrOptionTimeSlot());
  return ret;
}

int ViewPH(char *Title,int type,U8 NProbes,U16 *ptr)
{
  char ss[MAX_COLS+1];
  int _vrmb[MAX_ROWS*MAX_COLS];
  FKBDHANDLER prvkbdh;
  int i,j,k,ret=0;
//  int ok_flag=0;
  int _mutex;
  U16 data_buff_[MAX_ITEMS+4];
  U32 data_buff_calibr_[MAX_ITEMS];
  U16 cal_buff[MAX_PH_SENSORS][MAX_CAL_BUFF];
  U8  index_cal_buff=0;
  U16 dd;
  U16 tt;

//  _mutex=VCL_SetMutex();
  LCD_CopyVram(_vrmb);

  for(i=0;i<MAX_PH_SENSORS;i++){
      data_buff_calibr_[i]=0L;
  }
  data_buff=data_buff_;
  data_buff_calibr=data_buff_calibr_;
//  research_duration=duration;
  r_index=4*4;
  r_type_research=type;
  InitTimeSlot();
//  r_mask_event=0;
//  r_counter=0;
//  r_stop_lo_index=0;
//  r_second_counter=0;
  VCL_SetRetVal(VCL_RET_OK);

//  strcpy(ss,"Раствор ");
//  strcat(ss,Title);
  _ClearScreen();
  _TextXY(0,0,Title,0);
  LCD_Update(NULL);

  OnCopyDate=WriteToAddBuff;
  ClearAddBuff();
  SetOnWriteData(DataToCalibrRec);

  if(type){
    pCurTimeSlot=(PTimeSlotRec)&RTimeSlot;
    r_transfer=8+14+6;
  }else{
    pCurTimeSlot=(PTimeSlotRec)&pHTimeSlot;
    r_transfer=8+6;
  }

  prvkbdh=DISPATCH_SetKbdHandler(CancelKbdHandler);

  for(i=0;i<MAX_CAL_BUFF;i++){
//    sprintf(ss,"Ждем %03d сек ",MAX_CAL_BUFF-i);
//    _TextXY(0,2,ss,0);
//    LCD_Update(NULL);
    _mutex=VCL_SetMutex();
    research_duration=1;
    ClearAddBuff();
    StartTimeSlot(pCurTimeSlot);
    VCL_WaitMutex(_mutex);
    if(VCL_GetRetVal()==VCL_RET_CANCEL){
      LCD_Update(_vrmb);
      DISPATCH_SetKbdHandler(prvkbdh);
      StartTimeSlot(GetPtrOptionTimeSlot());
      return 1;
    }

    for(j=0;j<NProbes;j++){
      tt=GetAddBuff(j)/GetCounterAddBuff();
      cal_buff[j][i]=tt;
    }
  }

  index_cal_buff=0;
  while(1){
    for(j=0;j<NProbes;j++){
      tt=GetAddBuff(j)/GetCounterAddBuff();
      if(type){
        sprintf(ss,"%d:%4.0f mV,%4.2f pH",j+1,CodeTomV(tt),CodeTopH(tt,j));
        cal_buff[j][index_cal_buff]=tt;
      }else{
        sprintf(ss,"%d:%4.0f mV,%4.2f pH",j+1,CodeTomV(tt),CodeTopH(tt,j));
        cal_buff[j][index_cal_buff]=tt;
      }
      _TextXY(0,j+1,ss,0);
    }
    LCD_Update(NULL);

    index_cal_buff++;
    if(index_cal_buff>=MAX_CAL_BUFF) index_cal_buff=0;
    LCD_Update(NULL);
    _mutex=VCL_SetMutex();
    research_duration=1;
    ClearAddBuff();
    StartTimeSlot(pCurTimeSlot);
    VCL_WaitMutex(_mutex);
/*    if(VCL_GetRetVal()==VCL_RET_CANCEL){
      LCD_Update(_vrmb);
      DISPATCH_SetKbdHandler(prvkbdh);
      StartTimeSlot(GetPtrOptionTimeSlot());
      return 1;
    }*/
    if(VCL_GetRetVal()==VCL_RET_CANCEL){
      break;
    }
  }

  ret=0;
  for(j=0;j<NProbes;j++){
    dd=0;
    for(i=0;i<MAX_CAL_BUFF;i++) dd+=cal_buff[j][i];
    dd >>= 4;
//    printf("V=%4.0f mV\n",CodeTomV(dd));
    if((CodeTomV(dd)<CalibrPHSolutionMin[3]) || (CodeTomV(dd)>CalibrPHSolutionMax[3])){
      ret |= 2;
      break;
    }
  }

  LCD_Update(_vrmb);
  DISPATCH_SetKbdHandler(prvkbdh);
//  ret=VCL_GetRetVal();
  StartTimeSlot(GetPtrOptionTimeSlot());
  return ret;
}

