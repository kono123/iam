//******************************************************************************
//******************************************************************************
#include <hardware.h>
//#include <adc.h>
//#include <rtc.h>
//#include <spi.h>
//#include <vcl.h>
//#include <kbd.h>
//#include <task.h>
#include <timeslot.h>

#include <stdio.h>

PTimeSlotRec curts;
volatile U8 _tsCount;

void StartTimeSlot(PTimeSlotRec tsr)
{
  if(curts){
    StopSysTimer();
    delay_ms(10);
  }
  curts=tsr;
  _tsCount=0;
  R_DISABLE();
  StartSysTimer(curts->SecondCount*curts->Stages);
}

void StopTimeSlot(void)
{
  StopSysTimer();
}

U16 GetSecCounterTimeSlot(void)
{
  return curts->SecondCount;
}

#pragma vector=TIMERA0_VECTOR
__interrupt void isr_TACCR0()
{
  curts->pStageRec[_tsCount].OnTimeEvent((PStageRec)&curts->pStageRec[_tsCount]);
  _tsCount++;
  if(_tsCount>=curts->Stages) _tsCount=0;
}
