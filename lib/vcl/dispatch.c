#include <hardware.h>
#include <iam.h>
#include <design.h>
#include <dispatch.h>
#include <timeslot.h>
#include <vcl.h>
#include <kbd.h>
//#include <adc.h>
//#include <spi.h>
#include <ts_option.h>
#include <ts_func.h>
#include <disk_sd.h>

#include <stdio.h>
#include <string.h>

static T_QUEUE que;
volatile U16 DoorBell;

extern U16 rrr[7*4];
extern U16 ubase;

const char Version[6]="01.03";

void (*OnKbdHandler) (U8 key);
unsigned int write_bytes;

unsigned int disp_status;

ONEVENT OnWriteData;
ONEVENT OnTimer;

extern TIamEnvironment iam;

void SetOnWriteData(ONEVENT EventHandler)
{
  OnWriteData=EventHandler;
}

void SetOnTimer(ONEVENT EventHandler)
{
  OnTimer=EventHandler;
}

FKBDHANDLER DISPATCH_SetKbdHandler(FKBDHANDLER KbdHandler)
{
  FKBDHANDLER kbdh;
  kbdh = OnKbdHandler;
  __disable_interrupt();
  OnKbdHandler = KbdHandler;
  __enable_interrupt();
  return kbdh;
}

void DISPATCH_KbdHandler(U8 key)
{
  if(key==KBD_ON){
    MUX_Open();
    SPI_Init();
    RTC_CS_OUTPUT();

    StartUp();

    MUX_Close();
//    PinPowerOff();

    SPI_Close();
//    ADC_Close();
    RTC_CS_INPUT();
    SleepDevice();
//    PinPowerOn();
  }
}

void DISPATCH_Open(void)
{
  KBD_Init();
  RTC_Init();
  ADC_Open();
  VCL_Open();

  que.index_put=que.index_get=0;
  disp_status=0;
  DoorBell = 0;
  OnTimer=NULL;

  strcpy(iam.MedicalCard,"000000/0000");
  strcpy(iam.NumberZond,"");
  iam.TypeResearch=DEFAULT_TYPE_RESEARCH;
  iam.HoursResearch=DEFAULT_HOURS_RESEARCH;
  iam.PositionPHProbe=DEFAULT_POSITION_PH_PROBE;

  OnKbdHandler = DISPATCH_KbdHandler;
  StartTimeSlot(GetPtrOptionTimeSlot());
  StartKbdTimer();

}

unsigned int DISPATCH_GetStatus(void)
{
  return disp_status;
}

void DISPATCH_SetStatus(unsigned int status)
{
  disp_status=status;
}

int DISPATCH_Message(void)
{
    T_Message mes;
//  int i;
    char ss[19];

#ifdef IAM_EMU
  IamWinProc();
#endif

    if(que.index_put==que.index_get) return 0;

    memcpy(&mes,&que.Message[que.index_get],sizeof(T_Message));

    if(que.index_get<(MAX_MESS_QUEUE-1)){
      que.index_get++;
    }else{
      que.index_get=0;
    }

    switch(mes.Type){

      case KBD_REPEAT_EVENT:
        mes.Param1 |= 0x80;
      case KBD_DOWN_EVENT:
        if(OnKbdHandler) OnKbdHandler((U8)mes.Param1);
      break;

      case FILE_WRITE_DATA_EVENT:
        TP3_LO();
        if(OnWriteData) OnWriteData(&mes);
        TP3_HI();
      break;

      case WRITE_DATA_EVENT:
        if(OnWriteData) OnWriteData(&mes);
      break;
/*
      case VCL_TIMER_EVENT:
        if(OnTimer) OnTimer(&mes);
      break;
*/
      case ALARM_POWER_EVENT:
//        disp_status |= STATUS_POWER_ALARM_MASK;
        VCL_SetRetVal(VCL_RET_POWER_ERROR);
        VCL_FreeMutex();
      break;

      case FILE_WRITE_ERROR_EVENT:
//        disp_status |= STATUS_WRITE_ERROR_MASK;
        VCL_SetRetVal(VCL_RET_WRITE_ERROR);
        VCL_FreeMutex();
      break;

      case VCL_CANCEL_EVENT:
        VCL_SetRetVal(VCL_RET_CANCEL);
        VCL_FreeMutex();
      break;

      case VCL_OK_EVENT:
        VCL_SetRetVal(VCL_RET_OK);
        VCL_FreeMutex();
      break;

      case KBD_SILENCE_EVENT:
        if(Kbdblock(mes.Param1)==VCL_RET_OK){
//        MessageBoxEx("Клава","разблокирована",MBF_OK);
          SetSilenceCounter(MAX_SILENCE_COUNTER);
          _TextXY(0,3,"\x20\x20",0);
          LCD_Update(NULL);
        }
      break;

      default:
        printf("Pass msg=0x%x,0x%x,0x%x\n",mes.Type,mes.Param1,mes.Param2);
    }
    return 0;
}

void SetDoorBell(U16 mask)
{
  DoorBell |= mask;
}

int SendMsg(unsigned int mes,unsigned int Param1,unsigned int Param2)
{
  P_Message pp;
  pp=&que.Message[que.index_put];
  pp->Type=mes;
  pp->Param1=Param1;
  pp->Param2=Param2;
  if(que.index_put<(MAX_MESS_QUEUE-1)){
    que.index_put++;
  }else{
    que.index_put=0;
  }
  return(0);
}

int DISPATCH_Task(void)
{
  int Terminate=0;

  while(!Terminate){
    DISPATCH_Message();
  }
  return Terminate;
}

