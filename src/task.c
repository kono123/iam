#include <hardware.h>
#include <lcd.h>
#include <vcl.h>
#include <task.h>

#include <stdio.h>
#include <string.h>

extern U16 rr[7*4];
extern U16 rrr[7*4];
extern U16 ubase;

void MainTaskKbdHandler(U8 key)
{
  char ss[20];
  sprintf(ss,"0x%02X ",key);
  LCD_Text(4,ss);

  switch(key){

    case 0x05:
      LCD_StartLine(0);
      StartWorkTimeSlot(4000);
      break;

    case 0x01:
      LCD_StartLine(32);
      StopWorkTimeSlot();
      break;
  }
}

void MainTaskInit(void)
{
  que.index_put=0;
  que.index_get=0;
  DoorBell = 0;
  KbdHandler = MainTaskKbdHandler;
//  StartWorkTimeSlot(4000);
  StartWorkTimeSlot(4800);
}

float kr=0.000725;

void SetDoorBell(U16 mask)
{
  DoorBell |= mask;
}

void StartWorkTimeSlot(U16 cr0)
{
// Timer init
  TACTL = TASSEL_2 + MC_1;    // Run to CCR0 with SMCLK
  TACCR0 = cr0;
  TACCTL0 = CCIE;             // Enable CCR0 int.
}

void StopWorkTimeSlot(void)
{
  StopSysTimer(); //TACCTL0 &= ~CCIE;             // Disable CCR0 int.
}

void MainTask(void)
{
  P_Message pp;
  char ss[32];
  int i,i1,i2;

  if(DoorBell & VIEWR_DB_MASK){
//      memset(rrr,14,14);

    for(i=0;i<7;i++){
    if(rrr[i]<ubase){
      rrr[i]=ubase-rrr[i];
    }else{
      rrr[i]=rrr[i]-ubase;
    }
    }

      sprintf(ss,"0x%04X,0x%04X ",rrr[0],rrr[1]);
//      sprintf(ss,"%6.3f;%6.3f",rrr[0]*kr,rrr[1]*kr);
      LCD_Text(0,ss);
//      sprintf(ss,"%6.3f;%6.3f",rrr[2]*kr,rrr[3]*kr);
      sprintf(ss,"0x%04X,0x%04X ",rrr[2],rrr[3]);
      LCD_Text(1,ss);
//      sprintf(ss,"%6.3f;%6.3f",rrr[4]*kr,rrr[5]*kr);
      sprintf(ss,"0x%04X,0x%04X ",rrr[4],rrr[5]);
      LCD_Text(2,ss);
//      sprintf(ss,"%6.3f",rrr[6]*kr);
      sprintf(ss,"0x%04X,0x%04X ",rrr[6],ubase);
      LCD_Text(3,ss);
      DoorBell &= ~VIEWR_DB_MASK;
  }

  if(que.index_put==que.index_get) return;

  pp=&que.Message[que.index_get];

  switch(pp->Type){

    case VIEW_R_EVENT:
      /*
      memcpy(rrr,rr,14);
//      sprintf(ss,"0x%04X,0x%04X ",rr[0],rr[1]);
      sprintf(ss,"%6.3f;%6.3f",rrr[0]*kr,rrr[1]*kr);
      LCD_Text(0,ss);
      sprintf(ss,"%6.3f;%6.3f",rrr[2]*kr,rrr[3]*kr);
//      sprintf(ss,"0x%04X,0x%04X ",rr[2],rr[3]);
      LCD_Text(1,ss);
      sprintf(ss,"%6.3f;%6.3f",rrr[4]*kr,rrr[5]*kr);
//      sprintf(ss,"0x%04X,0x%04X ",rr[4],rr[5]);
      LCD_Text(2,ss);
      sprintf(ss,"%6.3f",rrr[6]*kr);
//      sprintf(ss,"0x%04X ",rr[6]);
      LCD_Text(3,ss);
      */
    break;

    case KBD_EVENT:
      KbdHandler((U8)pp->Param1);
    break;

    default:
      printf("Pass msg=0x%x,0x%x,0x%x\n\r",pp->Type,pp->Param1,pp->Param2);
  }
  if(que.index_get<(MAX_MESS_QUEUE-1)){
    que.index_get++;
  }else{
    que.index_get=0;
  }
}
/*
int SendMessage(unsigned int mes,unsigned int Param1,unsigned int Param2)
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
*/