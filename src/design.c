#include "hardware.h"
#include <iam.h>
#include <vcl.h>
//#include <adc.h>
#include <kbd.h>
#include <rtc.h>
#include <disk_sd.h>
#include <ts_r.h>
#include <utils.h>
//#include <journal.h>

#include <memo1.c>
#include <splash1.c>

#include "ds3234.h"

#include <string.h>

#define WAIT_TIME_TECHNO 60

extern U16 Batery;

const char r_type_str[MAX_TYPE_RESEARCH][16]={
  "pH         ",
  "Импеданс+pH"
//  "Отладка    "
};

const char position_pH_str[MAX_POSITION_PH][16]={
  "Дуод-Антр-Тело",
  "Антр-Тело-Кард",
  "Тело-Кард-Пище",
  "Кард-Пище-Пище",
  "Пище-Пище-Пище"
};

const float CalibrPHSolution[4]={
  1.65,
  1.2,
  6.86,
  9.18
};

const U16 CalibrPHSolutionU16[4]={
  165,
  120,
  686,
  918
};

const float CalibrPHSolutionMin[4]={
   76.,
   33.,   // 1.20
  330.,
  419. // 9,18
};

const float CalibrPHSolutionMax[4]={
  150.,
  133.,
  417.,
  549.
};

TIamEnvironment iam;
U8 NumberCalibrPHSolution=DEFAULT_NUMBER_SOLUTION;

////////////////////////////////////////////////////////// WIZARD

int GetDiskFreeSpaceStr(char *str)
{
  U32 _free,_total;
  if(DISK_Mount()){
    strcpy(str,"Нет SD карты!!!");
    return 1;
  }else{
    _free=DISK_GetFreeSpaceMb();
    _total=DISK_GetTotalSpaceMb();
    sprintf(str,"%dМб",_free);
    IntProgressBarToStr(str+strlen(str),10,_total-_free,_total);
//    DISK_Unmount();
    return 0;
  }
}

int GetBatValueStr(char *str)
{
  float b;
  b=Batery;
  sprintf(str,"%4.2fB",b*2.5/0x1000);
  IntProgressBarToStr(str+strlen(str),10,Batery-POWER_ALARM_CODE,0x0FFF-POWER_ALARM_CODE);
  return 0;
}

extern TCalibrRRecord _gcalibr;
////extern TCalibrPHRecord _gphcalibr;

int GetDateRCalibrStr(char *str)
{
  RTC_DateTimeDMYToStr(_gcalibr.RDateTime,str);
  return 0;
}

int GetDatePHCalibrStr(char *str)
{
  PCalibrPHRecord prec;
  prec=FindPHProbeByN(iam.NumberZond);
  if(prec){
    if(prec->DateTime==0){
      strcpy(str,"нет!");
      return 1;
    }
    RTC_DateTimeDMYToStr(prec->DateTime,str);
  }else{
    strcpy(str,"нет!");
    return 1;
  }
  return 0;
}

int MenuEditTypeResearch(void)
{
  U8 index;
  index=iam.TypeResearch;
  if(EnumBox("Исследования",&index,MAX_TYPE_RESEARCH,r_type_str[0],r_type_str[1],r_type_str[2])==ENUM_BOX_OK){
    iam.TypeResearch=index;
    return 1;
  }else
    return 0;
}

int MenuEditPositionPH(void)
{
  U8 index;
  index=iam.PositionPHProbe;
  if(EnumBox("положение зонда",&index,MAX_POSITION_PH,position_pH_str[0],position_pH_str[1],position_pH_str[2],position_pH_str[3],position_pH_str[4])==ENUM_BOX_OK){
    iam.PositionPHProbe=index;
    return 1;
  }else
    return 0;
}

int MenuEditDuration(void)
{
  char ss[19];
  U8 tt;
  sprintf(ss,"%d",iam.HoursResearch);
  if(InputBox("Длительность час",ss,2)==VCL_RET_OK){
    if(strlen(ss)==0){
      return 0;
    }
    tt=StrToU8(ss);
    if(tt>48){
      MsgBoxEx("Ошибка","максимум 48 ч",MBF_OK);
      return 0;
    }
    if(tt==0){
      MsgBoxEx("Ошибка","минимум 1 ч",MBF_OK);
      return 0;
    }
    iam.HoursResearch=tt;
    return 1;
  }else{
    return 0;
  }
}

int EditDuration(void *prt)
{
  return MenuEditDuration();
}

int GetDurationStr(char *str)
{
  if(iam.HoursResearch==1){
    sprintf(str,"%d час",iam.HoursResearch);
  }else if((iam.HoursResearch>1) && (iam.HoursResearch<5)){
    sprintf(str,"%d часа",iam.HoursResearch);
  }else if((iam.HoursResearch>20) && (iam.HoursResearch<25)){
    sprintf(str,"%d час",iam.HoursResearch);
  }else if((iam.HoursResearch>30) && (iam.HoursResearch<35)){
    sprintf(str,"%d час",iam.HoursResearch);
  }else if((iam.HoursResearch>40) && (iam.HoursResearch<45)){
    sprintf(str,"%d час",iam.HoursResearch);
  }else{
    sprintf(str,"%d часов",iam.HoursResearch);
  }
  return 0;
}

int EditTypeResearch(void *prt)
{
  return MenuEditTypeResearch();
}

int EditPositionPH(void *prt)
{
  return MenuEditPositionPH();
}

int GetTypeResearchStr(char *str)
{
  strcpy(str,r_type_str[iam.TypeResearch]);
  return 0;
}

int GetPositionPHStr(char *str)
{
  strcpy(str,position_pH_str[iam.PositionPHProbe]);
  return 0;
}

int WzGetCalibrPHStr0(char *str)
{
  sprintf(str,"  %4.2f pH",CalibrPHSolution[0]);
  return 0;
}

int WzGetCalibrPHStr1(char *str)
{
  sprintf(str,"  %4.2f pH",CalibrPHSolution[1]);
  return 0;
}

int WzGetCalibrPHStr2(char *str)
{
  sprintf(str,"  %4.2f pH",CalibrPHSolution[2]);
  return 0;
}

int WzGetCalibrPHStr3(char *str)
{
  sprintf(str,"  %4.2f pH",CalibrPHSolution[3]);
  return 0;
}

int GetNumberProbeStr(char *str)
{
  strcpy(str,iam.NumberZond);
  if(strlen(str)){
   if(FindPHProbeByN(str)){
     return 0;
   }else{
     strcpy(str,"-нет-");
     iam.NumberZond[0]=0;
     return 1;
   }
  }else{
    strcpy(str,"-нет-");
    return 1;
  }
}

int GetNumberSolutionStr(char *str)
{
  sprintf(str,"%d раствора",NumberCalibrPHSolution);
  return 0;
}

int GetMedCardStr(char *str)
{
  strcpy(str,iam.MedicalCard);
  return 0;
}

int MenuEditMedCard(void)
{
  char ss[19];
  char ss1[19]="000000/00";
  int len;
  strcpy(ss,iam.MedicalCard);
  if(InputBox("Ввод № карты",ss,0)==VCL_RET_OK){
    len=strlen(ss);
    if(len){
      while(len--){
        ss1[len]=ss[len];
      }
      strcpy(iam.MedicalCard,ss1);
      return 1;
    }else{
      return 0;
    }
  }else{
    return 0;
  }
}

int EditMedCard(void *prt)
{
  return MenuEditMedCard();
}

//int MenuWriteProbe(void) {return 0;}
int MenuImportProbe(void) {return 0;}

char * ProbeListBox(char **text,char *buff,unsigned char flag)
{
  int i,ii;
  char *pp;
  char ss[16];
//  char ss2[16];
  PCalibrPHRecord pcr;

  ii=0;
  for(i=0;i<MAX_RECORD_PH_PROBE;i++){
    text[ii]=GetPHProbeStr(i);
    if(text[ii]){
      pcr = GetPHProbePtr(i);
      if(pcr->DateTime==0){
        strcpy(ss,"-нет-");
      }else{
        RTC_DateTimeDMYToStr(pcr->DateTime,ss);
      }
      strcpy(&buff[ii*16],text[ii]);
      strcat(&buff[ii*16]," ");
      strcat(&buff[ii*16],ss);
      text[ii]=&buff[ii*16];
      ii++;
    }
  }
  if(ii){
    pp = ListBox(text,ii);
    if(pp) pp[6] = 0;
    return pp;
  }else{
    MsgBoxEx("Зонды","не найдены",MBF_OK);
    return 0;
  }
}

int MenuExportProbe(void)
{
  int i,ii;
  char *pp;
  char *mstr[MAX_RECORD_PH_PROBE];
  char ss[18];
  ii=0;
  for(i=0;i<MAX_RECORD_PH_PROBE;i++){
    mstr[ii]=GetPHProbeStr(i);
    if(mstr[ii]) ii++;
  }
  if(ii){
    pp=ListBox(mstr,ii);
    if(pp==0) return 1;
    sprintf(ss,"%s записать на SD?",pp);
    if(MsgBoxEx("Вы уверены?",ss,MBF_OK)!=VCL_RET_OK) return 1;
    ProbeToFile(pp);
//      strcpy(iam.NumberZond,pp);
  }else{
    MsgBox("Зонды не найдены",MBF_OK);
  }
  return 1;
}

int MenuEraseNumberProbe(void)
{
//  int i,ii;
  char *pp;
  char *mstr[MAX_RECORD_PH_PROBE];
  char buff[MAX_RECORD_PH_PROBE][16];
  char ss[18];

  pp=ProbeListBox(mstr,(char*)buff,0);
  if(pp==0){
//      MessageBoxEx("Зонды","не найдены",MBF_OK);
      return 1;
  }
  sprintf(ss,"%s удалить?",pp);
  if(MsgBoxEx("Вы уверены?",ss,MBF_OK)!=VCL_RET_OK) return 1;
  ErasePHProbe(pp);
//      strcpy(iam.NumberZond,pp);
  return 0;
}

int MenuAddNumberProbe(void)
{
  int i,len;
  U8 index;
  char ss1[19]="\x30\x30\x30\x30\x30\x30";
  char ss[19]="\x30\x30\x30\x30\x30\x30";
  TCalibrPHRecord rec;

  memset((void *)&rec,0,sizeof(TCalibrPHRecord));

  if(InputBox("Ввод № зонда",ss1,strlen(ss1))!=VCL_RET_OK) return 0;

  len=strlen(ss1);
  for(i=len;i!=0;i--){
    ss[5-len+i]=ss1[i-1];
  }
  if(FindPHProbeByN(ss)){
    MsgBoxEx("Уже есть такой!",ss,MBF_OK);
    return 0;
  }else{
    strcpy(rec.No,ss);
    len=strlen(ss);
    rec.Type=0xff;  // undef type
    switch(ss[len-1]){
        case 0x32:  // XXXXX2 pH
          rec.Type=0;
        break;
        case 0x36:  // XXXXX6 R+pH
          rec.Type=1;
        break;
    default:
      MsgBoxEx("Недопустимый №",ss,MBF_OK);
      return 0;
    }

    index=2;
    if(EnumBox("электродов pH",&index,3,"один","два ","три ")==VCL_RET_OK){
      rec.MaxAnodpH=index+1;
      if(AddPHProbe(&rec)){
        return 1;
      }else{
        MsgBox("Зондов >10!",MBF_OK);
        return 0;
      }
    }else
      return 0;
  }
}

int MenuEditNumberProbe(void)
{
  int i,ii;
  char *pp;
  char *mstr[MAX_RECORD_PH_PROBE];
  char buff[MAX_RECORD_PH_PROBE][16];
//  ii=0;
//  for(i=0;i<MAX_RECORD_PH_PROBE;i++){
//    mstr[ii]=GetPHProbeStr(i);
//    if(mstr[ii]) ii++;
//  }
//  if(ii){
    pp=ProbeListBox(mstr,(char*)buff,0);
    if(pp){
      strcpy(iam.NumberZond,pp);
      iam.pProbe=FindPHProbeByN(pp);
      iam.TypeResearch=iam.pProbe->Type;
      return 1;
//    }
  }else{
    MsgBoxEx("Зонды","не найдены",MBF_OK);
  }
  return 0;
}

int EditNumberProbe(void *ptr)
{
  return MenuEditNumberProbe();
}

int EditNumberSolutions(void *ptr)
{
  U8 index=0;
  if(NumberCalibrPHSolution==4){
      index=1;
  }
  if(EnumBox("Кол-во растворов",&index,2,"два   ","четыре")==VCL_RET_OK){
    if(index==0){
      NumberCalibrPHSolution=2;
    }else if(index==1){
      NumberCalibrPHSolution=4;
    }
    return 1;
  }else
    return 0;
}


int MenuShowInfo(void)
{
  char ss[18];
  char ss1[18];
  char ss2[18];
  char ss3[18];
  strcpy(ss,"№:");
  GetSerialNumberStr(ss1);
  strcat(ss,ss1);
  strcpy(ss2,"Гастроскан:");
  GetDeviceNameStr(ss1);
  strcat(ss2,ss1);
  strcpy(ss3,"Версия ПО:");
  GetVersionFwStr(ss1);
  strcat(ss3,ss1);
  Info(ss2,ss,ss3,"Исток-Система");
  return 0;
}

int MenuShowBat(void)
{
  IntProgressBar("Напряжение батареи",12);
  return 0;
}

int MenuShowHelp(void)
{
  Memo(pMemo1,0);
  return 0;
}

int Dummy(void *prt)
{
  return 0;
}

int WaitDiskFreeSpace(void *prt)
{
  MsgBox("!Нереализовано!",MBF_OK);
  return 0;
}

int ToDo(void)
{
//  if(DISK_Mount()) return 1;
/*
  strcpy(ss1,"22-06-09/iam_r00.dat");
  if(FS_FileOpen(&file_w,ss1,'r')==0){

//      FS_FileClose2(&file_w,51609600);
//      FS_FileClose2(&file_w,51592600);
      FS_FileClose2(&file_w,51400000);
  }
  DISK_Unmount();

return 1;
*/
  MsgBox("!Нереализовано!",MBF_OK);
  return 0;
}

int DebugViewPH(void)
{
  DebugViewResearch(0,3);
  return 0;
}

int DebugViewR(void)
{
  DebugViewResearch(1,3);
  return 0;
}

int RCalibr(void);
int pHCalibr(void);

int EditCalibrate(void *ptr)
{
//  RCalibr();
//  pHCalibr();
  return 0;
}

int ErrorProbeHandler(void)
{
  MsgBoxEx("Необходимо","добавить зонд!",MBF_OK);
  return 0;
}

int ErrorSDHandler(void)
{
  MsgBoxEx("Вставьте","SD карту!",MBF_OK);
  return 0;
}

int ErrorCalibrateHandler(void)
{
  MsgBoxEx("Необходима","калибровка!",MBF_OK);
  return 0;
}

//////////////////////////////////////////////////////////// WIZARD pH CALIBR
const TWizard ph_calibr_wizard={
  7, // items
  0,
  {
    {"Заводской номер","зонда:",0,GetNumberProbeStr,EditNumberProbe,ErrorProbeHandler},
    {"Дата последней","калибровки:",1,GetDatePHCalibrStr,NULL,NULL},
//    {"Кол-во","растворов:",0,GetNumberSolutionStr,EditNumberSolutions,NULL},
    {"Тех. прогон","раствор:",1,WzGetCalibrPHStr1,NULL,NULL},
    {"Тех. прогон","раствор:",1,WzGetCalibrPHStr3,NULL,NULL},
    {"Тех. прогон","раствор:",1,WzGetCalibrPHStr1,NULL,NULL},
    {"Калибровка","раствор:",1,WzGetCalibrPHStr3,NULL,NULL},
    {"Калибровка","раствор:",1,WzGetCalibrPHStr1,NULL,NULL}
  }
};
//////////////////////////////////////////////////////////// WIZARD R CALIBR
const TWizard r_calibr_wizard={
  1, // items
  0,
  {
//    {"Калибровка","Дата последней",1,GetDateCalibrStr,Dummy},
    {"Присоедините","заглушку",1,GetDateRCalibrStr,NULL,NULL},
  }
};

//////////////////////////////////////////////////////////// WIZARD pH CALIBR
const TWizard ph_calibr_check_wizard={
  3, // items
  0,
  {
    {"Заводской номер","зонда:",0,GetNumberProbeStr,EditNumberProbe,ErrorProbeHandler},
    {"Дата последней","калибровки:",1,GetDatePHCalibrStr,NULL,NULL},
//    {"Тех. прогон","раствор:",1,WzGetCalibrPHStr1,NULL,NULL},
//    {"Тех. прогон","раствор:",1,WzGetCalibrPHStr3,NULL,NULL},
//    {"Тех. прогон","раствор:",1,WzGetCalibrPHStr1,NULL,NULL},
    {"Проверка","в растворе:",1,WzGetCalibrPHStr3,NULL,NULL}
  }
};

//////////////////////////////////////////////////////////// WIZARD R METER
const TWizard rmetr_wizard={
  7, // items
  0,
  {
    {"Мед. карта","123456/2009",0,GetMedCardStr,EditMedCard,NULL},
    {"Заводской номер","зонда:",0,GetNumberProbeStr,EditNumberProbe,ErrorProbeHandler},
    {"Длительность","исследования",0,GetDurationStr,EditDuration,NULL},
//    {"Калибровка","Дата последней",0,GetDatePHCalibrStr,EditCalibrate,ErrorCalibrateHandler},
    {"Калибровка","Дата последней",0,GetDatePHCalibrStr,NULL,ErrorCalibrateHandler},
//    {"Калибровка","Дата последней",0,GetDatePHCalibrStr,NULL,NULL},
    {"Батарейка","Напряжение",1,GetBatValueStr,NULL,NULL},
//    {"Диск","Свободно",0,GetDiskFreeSpaceStr,WaitDiskFreeSpace,ErrorSDHandler},
    {"Диск","Свободно",0,GetDiskFreeSpaceStr,NULL,ErrorSDHandler},
    {"Расположение","зонда",0,GetPositionPHStr,EditPositionPH,NULL}
  }
};

int MenuWork(void)
{
  TCalibrPHRecord *p;
  int item=0;
  int ret;
  U8 nProbes;
//  unsigned int write_bytes;

  while(item>=0){
    item=Wizard(&rmetr_wizard,item);
    if(iam.TypeResearch==TYPE_RPH_RESEARCH){
      if(item==6) item++;  // пропуск "Расположение зонда"
    }
    if(item>=rmetr_wizard.Items){
      if(MsgBox("Начать?",MBF_OKCANCEL)==VCL_RET_OK){
        p=FindPHProbeByN(iam.NumberZond);
        if(p==NULL) return 0;              //???
        nProbes=p->MaxAnodpH;
//        if(iam.pProbe->Type){
          iam.TypeResearch=iam.pProbe->Type;
//        }else{
//        }
        ret=PreViewResearch(iam.TypeResearch,nProbes);
        if(ret!=VCL_RET_OK2){
          return 0;
        }

        ret=Research("Исследование...",iam.TypeResearch,(U32)iam.HoursResearch*3600L);
        if(ret==VCL_RET_CANCEL){
          MsgBoxEx("Прервано","пользователем",MBF_OK);
//          MessageBox("Прервать?",MBF_OK);
//          if(InputBox("Код подтверждения",ss,0)==VCL_RET_OK){
//          }
        }else if(ret==VCL_RET_OK){
          MsgBoxEx("Исследование","закончено",MBF_OK);
        }else if(ret==VCL_RET_POWER_ERROR){
          MsgBoxEx("Прервано","Низкое питание!",MBF_OK);
        }else if(ret==VCL_RET_WRITE_ERROR){
          MsgBoxEx("Прервано","Ошибка записи!",MBF_OK);
        }
      }
      return 0;
    }
  }
  return 0;
}

int MenuEditTime(void)
{
  char ss[19],head[19],tail[19];
  U8 h,m,f=1;
  while(f){
    h=RTC_getHour();
    m=RTC_getMinute();
    RTC_getTimeStrHM(ss);
    if(InputBox("Ввод времени",ss,0)==VCL_RET_OK){
      if(GetStrToDelimiter(ss,head,tail)){
        h=StrToU8(head);
        if(h>=24){
          MsgBoxEx("Ошибка","максимум 23 ч",MBF_OK);
          continue;
        }
        if(GetStrToDelimiter(tail,head,tail)){
          m=StrToU8(head);
          if(m>=60){
            MsgBoxEx("Ошибка","максимум 59 мин",MBF_OK);
            continue;
          }
        }
        RTC_setHMS(h,m,0);
      }
      f=0;
    }else{
      f=0;
    }
  }
  return 0;
}

int MenuEditDate(void)
{
  char ss[19],head[19],tail[19];
  U8 d,m,f=1;
  U16 y;

  while(f){
    d=RTC_getDay();
    m=RTC_getMonth();
    y=RTC_getYear()-2000;
    RTC_getDateStr(ss);
    if(InputBox("Ввод даты",ss,0)==VCL_RET_OK){
      if(GetStrToDelimiter(ss,head,tail)){
        d=StrToU8(head);
        if(GetStrToDelimiter(tail,head,tail)){
          m=StrToU8(head);
          if(GetStrToDelimiter(tail,head,tail)){
            y=StrToU8(head);
          }
        }
      }
      if(m>12){
        MsgBoxEx("Ошибка месяц","максимум 12",MBF_OK);
        continue;
      }
      if(d>RTC_GetDaysPerMonth(m,y)){
        sprintf(ss,"максимум %d",RTC_GetDaysPerMonth(m,y));
        MsgBoxEx("Ошибка день",ss,MBF_OK);
        continue;
      }
      if(d==0) d=1;
      if(m==0) m=1;
      RTC_setDMY(d,m,y);
      f=0;
    }else{
      f=0;
    }
  }
  return 0;
}

int RCalibr(void)
{
  int item=0;

  while(item!=-1){
    item=Wizard(&r_calibr_wizard,item);
    if(item>=r_calibr_wizard.Items){
      if(MsgBox("Начать?",MBF_OKCANCEL)==VCL_RET_OK){
        if(Calibrate("Калибровка...",10,10L)){
          MsgBoxEx("Калибровка","не прошла!!",MBF_OK);
        }else{
          MsgBoxEx("Калибровка","завершена",MBF_OK);
//          if(MessageBox("Записать?",MBF_OKCANCEL)==VCL_RET_OK){
//            WriteCalibrR();
//          }
        }
      }
      return 0;
    }
  }
  return 0;
}

int pHCalibrCheck(void)
{
  int item=0;
  int nextitem=0;
  TCalibrPHRecord cr,*p;
  U16 cal_buff[3];
  int i,j;
  U8 nSulution;
  U8 nProbes;
  char ss[18];

  while(item>=0){

    nextitem=Wizard(&ph_calibr_check_wizard,item);
    if(nextitem<=item){
      item=nextitem;
      continue;
    }

    item=nextitem;

    switch(item){
      case 1:
        p=FindPHProbeByN(iam.NumberZond);
        if(p==NULL) return 0;
        memcpy(&cr,p,sizeof(TCalibrPHRecord));
//        strcpy(cr.No,iam.NumberZond);
//        for(i=0;i<MAX_PH_SOLUTION;i++) cr.pHProbe[i].pH=0xFFFF;
        nProbes=cr.MaxAnodpH;
      break;
/*
      case 3:
        sprintf(ss,"Раствор %4.2f pH",CalibrPHSolution[1]);
        if(WaitTimerBox(ss,WAIT_TIME_TECHNO)!=VCL_RET_OK) item--;
      break;

      case 4:
        sprintf(ss,"Раствор %4.2f pH",CalibrPHSolution[3]);
        if(WaitTimerBox(ss,WAIT_TIME_TECHNO)!=VCL_RET_OK) item--;
      break;

      case 5:
        sprintf(ss,"Раствор %4.2f pH",CalibrPHSolution[1]);
        if(WaitTimerBox(ss,WAIT_TIME_TECHNO)!=VCL_RET_OK) item--;
      break;
*/
      case 3:
        sprintf(ss,"Раствор %4.2f pH",CalibrPHSolution[3]);
        if(WaitTimerBox(ss,WAIT_TIME_TECHNO)!=VCL_RET_OK){
          item--;
        }else{

          if(ViewPH(ss,cr.Type,nProbes,cal_buff)){
            MsgBoxEx("Зонд","не готов!!",MBF_OK);
          }else{
            MsgBoxEx("Зонд","готов",MBF_OK);
          }
        }
      break;
    }

    if(item>=ph_calibr_check_wizard.Items){
       return 0;
    }

  }

  return 0;
}
//////////////////////////////////////////////////// pHCalibr
int pHCalibr(void)
{
  int item=0;
  int nextitem=0;
  TCalibrPHRecord cr,*p;
  U16 cal_buff[3];
  int i,j;
  U8 nSulution;
  U8 nProbes;
  char ss[18];

  while(item>=0){

//    if(NumberCalibrPHSolution==2){
//      if((item==3) || (item==5)) item++;
//    }

    nextitem=Wizard(&ph_calibr_wizard,item);
    if(nextitem<=item){
      item=nextitem;
//      if(NumberCalibrPHSolution==2){
//        if((item==3) || (item==5)) item--;
//      }
      continue;
    }
    item=nextitem;
    switch(item){
      case 1:
        p=FindPHProbeByN(iam.NumberZond);
        if(p==NULL) return 0;
        memcpy(&cr,p,sizeof(TCalibrPHRecord));
//        strcpy(cr.No,iam.NumberZond);
        for(i=0;i<MAX_PH_SOLUTION;i++) cr.pHProbe[i].pH=0xFFFF;
        nProbes=cr.MaxAnodpH;
      break;

      case 3:
        sprintf(ss,"Раствор %4.2f pH",CalibrPHSolution[1]);
        if(WaitTimerBox(ss,WAIT_TIME_TECHNO)!=VCL_RET_OK){
          MsgBoxEx("Прерванно","пользователем",MBF_OK);
          item--;
        }
      break;
      case 4:
        sprintf(ss,"Раствор %4.2f pH",CalibrPHSolution[3]);
        if(WaitTimerBox(ss,WAIT_TIME_TECHNO)!=VCL_RET_OK){
          MsgBoxEx("Прерванно","пользователем",MBF_OK);
          item--;
        }
      break;
      case 5:
        sprintf(ss,"Раствор %4.2f pH",CalibrPHSolution[1]);
        if(WaitTimerBox(ss,WAIT_TIME_TECHNO)!=VCL_RET_OK){
          MsgBoxEx("Прерванно","пользователем",MBF_OK);
          item--;
        }
      break;

      case 6:
      case 7:
        if(item==6){
          nSulution=3;
        }else{
          nSulution=1;
        }
        sprintf(ss,"Раствор %4.2f pH",CalibrPHSolution[nSulution]);
        if(CalibratePH(ss,cr.Type,nProbes,cal_buff,nSulution)){
          MsgBoxEx("Калибровка","не прошла!!",MBF_OK);
//          if(MessageBox("Повторить?",MBF_OKCANCEL)==VCL_RET_OK){
            item--;
//          }else{
//            return 0;
//          }
        }else{
          for(i=0;i<nProbes;i++){
            cr.pHProbe[nSulution].Code[i]=cal_buff[i];
          }
          cr.pHProbe[nSulution].pH=CalibrPHSolutionU16[nSulution];
        }
      break;
    }
    if(item>=ph_calibr_wizard.Items){
          MsgBoxEx("Калибровка","завершена",MBF_OK);
//       if(MessageBox("Записать?",MBF_OKCANCEL)==VCL_RET_OK){
         cr.DateTime=RTC_getDateTime();
         UpdatePHProbe(&cr);
//       }
       return 0;
    }
  }
  return 0;
}

int MenuCalibrR(void)  { RCalibr();  return 0;}
int MenuCalibrPH(void) { pHCalibr(); return 0;}
int MenuCalibrCheckPH(void) { pHCalibrCheck(); return 0;}

//////////////////////////////////////////////////////////// MENU
const TMENU CalibrPHMenu={
  NULL, 3,{
    {MenuCalibrPH, 1,          "Калибровка pH"},
    {MenuCalibrR, 1,           "Калибровка Z"},
    {MenuCalibrCheckPH, 1,     "Проверка pH"}
  }
};
const TMENU ResearchMenu={
  NULL, 2,{
    {MenuEditDuration, 1,      "Длительность"},
    {MenuEditMedCard, 1,       "Мед. карта"}
  }
};

const TMENU ProbeMenu={
  NULL, 2,{
    {MenuAddNumberProbe, 1,    "Добавить зонд"},
    {MenuEraseNumberProbe, 1,  "Удалить зонд"},
//    {MenuImportProbe, 1,       "Импорт"},
//    {MenuExportProbe, 1,       "Экспорт"},
  }
};

int MenuResearch(void) {Menu(&ResearchMenu);  return 0;}
int MenuProbe(void) {Menu(&ProbeMenu);  return 0;}

const TMENU SetupMenu={
  NULL, 4,{
    {MenuEditTime, 1,    "Время"},
    {MenuEditDate, 1,    "Дата"},
    {MenuResearch, 1,    "Исследование"},
    {MenuProbe,    1,    "Зонды"}
  }
};
const TMENU DebugMenu={
  NULL, 2,{
    {DebugViewPH, 1,    "pH-зонд"},
    {DebugViewR, 1,    "Z/pH-зонд"},
  }
};

int MenuDebug(void)  {Menu(&DebugMenu);  return 0;}

const TMENU InfoMenu={
  NULL, 4,{
    {MenuShowBat, 1,           "Батарейка"},
    {MenuShowInfo, 1,          "О приборе"},
    {MenuShowHelp, 1,          "Помощь"},
    {MenuDebug, 1,             "Отладка"}
  }
};

int MenuCalibr(void) {Menu(&CalibrPHMenu);  return 0;}
int MenuSetup(void)  {Menu(&SetupMenu);  return 0;}
int MenuInfo(void)   {Menu(&InfoMenu);  return 0;}

const TMENU MainMenu={
  NULL, 4,{
    {MenuWork, 1,       "Работа"},
    {MenuSetup, 1,      "Установка"},
    {MenuCalibr, 1,     "Калибровка"},
    {MenuInfo, 1,       "Информация"}
  }
};

int ii78;
/*
int TestSD(void)
{
  EmbeddedFile file_w;
  File file_r;
  U8 err;
  unsigned int write_bytes;
  unsigned int size;
  unsigned char buff1[600];
  unsigned char buff2[600];
  int i,j,e,rep;
  char f_name[] = "tt4.bin";
  char ss[64];
  int enum_file=0;

  size = 512;
  ii78++;
  rep = 64;

  for(i=0;i<size;i++){
    buff1[i] = i+ii78;
    buff2[i] = 0xff;
  }

  if(DISK_Mount()){
    printf("DISK_Mount ERROR!\n");
    return 1;
  }

  sprintf(ss,"tes2%02d.dat",enum_file);
  while(FS_FileOpen(&file_w,ss,'w')){
      enum_file++;
      sprintf(ss,"test%02d.dat",enum_file);
      if(enum_file>250){
    printf("FileOpenWrite ERROR %x\n",err);
       return 2;
      }
  }


  for(j=0;j<size;j++) buff1[j] = enum_file;

  err = FS_FileWrite(&file_w,size,&write_bytes,buff1);
  if(err){
    printf("FS_FileWrite ERROR1 %x\n",err);
    FS_FileClose(&file_w);
    return 1;
  }

  for(j=0;j<size;j++) buff1[j] = j;

  for(i=0;i<rep;i++){
    err = FS_FileWrite(&file_w,size,&write_bytes,buff1);
    if(err){
      printf("FS_FileWrite ERROR2 %x j=%x\n",err,j);
      FS_FileClose(&file_w);
      return 1;
    }
  }

  FS_FileClose(&file_w);

  err=FS_FileOpen(&file_r,ss,'r');
  if(err){
    printf("FileOpenRead ERROR %x\n",err);
    return 1;
  }

  err = FS_FileRead(&file_r,size,&write_bytes,buff2);
  if(err){
    printf("FS_FileRead ERROR %x\n",err);
    return 1;
  }

  for(i=0;i<size;i++){
    if(buff2[i]!=(enum_file & 0xFF)){
      printf("[%x] %x<>%x\n",i,buff2[i],enum_file);
      FS_FileClose(&file_r);
      return 1;
    }
  }

//  FS_FileClose(&file_r);
//  DISK_Unmount();
//return;

  for(j=0;j<rep;j++){
    err=FS_FileRead(&file_r,size,&write_bytes,buff2);
    if(err){
      printf("FS_FileRead ERROR2 %x j=%d\n",err,j);
      FS_FileClose(&file_r);
      return 1;
    }
    for(i=0;i<size;i++){
      if(buff1[i]!=buff2[i]){
        printf("j=%x,[%x] %x<>%x\n",j,i,buff1[i],buff2[i]);
        break;
      }
    }
  }

  FS_FileClose(&file_r);

  DISK_Unmount();

  return 0;
}
*/

void EraseAllProbe(void);

void StartUp(void)
{
//  U32 dw,dw2;
  int i;
  int ii;
  int t;
  char ss[18];
  unsigned char buff[256];
  U8 data;

  PH_ENABLE();
  POWER_3V_On();
  LCD_Open();
  /*
  dw=0xFFFFFF00;
  for(i=0;i<256;i++){
    RTC_WriteDword(dw);
    dw2=RTC_ReadDword();
    if(dw2!=dw) printf("dw=%lx,dw2=%lx\n",dw,dw2);
    dw-=256;
  }
  */
  LCD_Update(NULL);

  LCD_MemToScreen((U8 *)pSplash,4);
//  LCD_MemToScreen((U8 *)pSplash3,4);
  i=32;
  ii=0;
  while(i--){
    LCD_StartLine(ii++);
    delay_ms(16);
  }

  LCD_StartLine(0);
  LCD_StartLine(0);

  R_DISABLE();

//  JOURNAL_Open();
  LCD_MemToScreen((U8 *)pSplash,0);

  ii=9;
//  ii=30000;
  t=0;
  while(ii--){
    i=7;
    while(i--){
     LCD_MemToScreen((U8 *)pSplash,0);
//     LCD_MemToScreen((U8 *)pSplash3,0);
//     LCD_MemToScreen((U8 *)pSplash,0);
//     LCD_MemToScreen((U8 *)pSplash3,0);
//     LCD_MemToScreen((U8 *)pSplash,0);
    }
  }

// отладка SD
//  for(;;){
//    TestSD();
//  }

//  strcpy(iam.MedicalCard,"000000/0000");
//  strcpy(iam.NumberZond,"");
//  for(i=0;i<MAX_RECORD_PH_PROBE;i++){
//    iam.pProbe=GetPHProbePtr(i);
//    if(iam.pProbe){
//      strcpy(iam.NumberZond,iam.pProbe->No);
//    };
//  }
//  iam.TypeResearch=DEFAULT_TYPE_RESEARCH;
//  iam.HoursResearch=DEFAULT_HOURS_RESEARCH;
//  iam.PositionPHProbe=DEFAULT_POSITION_PH_PROBE;

//  EraseAllProbe();
/*
  printf("CTRL=0x%x\n",_ds1390_getCtrl());
  data = _ds1390_getCtrl() | (1<<5);
  _ds1390_setCtrl(data);
  printf("CTRL=0x%x\n",_ds1390_getCtrl());
  printf("T=0x%x\n",_ds3234_Temperature());
  printf("T=0x%x\n",_ds3234_Temperature());
  printf("T=0x%x\n",_ds3234_Temperature());
  printf("CTRL=0x%x\n",_ds1390_getCtrl());
*/
  StartMainMenu(&MainMenu);

//  MessageBoxEx("Отладка","выключаемся",MBF_OK);
//  DISK_Unmount();
  LCD_Close();
  POWER_3V_Off();
}
