#include <hardware.h>
#include <kbd.h>
#include <dispatch.h>

#include <stdio.h>

#ifdef KBD_INVERSE
  const U8 KBDLUT[17]={
    0,
    KBD_STOP,KBD_CANCEL,KBD_0,KBD_OK,
    KBD_ON,KBD_1,KBD_2,KBD_3,
    KBD_ARROW_H,KBD_7,KBD_8,KBD_9,
    KBD_ARROW_V,KBD_4,KBD_5,KBD_6
  };
#else
  const U8 KBDLUT[17]={
    0,
    KBD_6,      KBD_9,      KBD_3,     KBD_OK,
    KBD_5,      KBD_8,      KBD_2,     KBD_0,
    KBD_4,      KBD_7,      KBD_1,     KBD_CANCEL,
    KBD_ARROW_V,KBD_ARROW_H,KBD_ON,    KBD_STOP
};
#endif

U8 kbd_scan_code=0;
U8 _kbdCount=0;
U16 _repeatcount=MAX_KBD_REPEAT_COUNT;
U16 _gtimer=0;
U16 silence_count=0;
U16 silence_flag=0;
U16 areyousleeping=0;

void SetSilenceDone(void)
{
  silence_flag=0;
}

void SetSilenceFlag(U8 flag)
{
  silence_flag=flag;
}

void SetSilenceCounter(U16 val)
{
  silence_count=val;
  silence_flag=0;
}

void _KbdScan(void)
{
  U8 temp;
  temp=KBDLUT[KBD_scan()];
  if(temp){
    if(kbd_scan_code!=temp){
      kbd_scan_code=temp;
      _repeatcount=MAX_KBD_REPEAT_COUNT;
      if(silence_flag){
        SendMsg(KBD_SILENCE_EVENT,temp,0);
      }else{
        if(silence_count){
          silence_count=MAX_SILENCE_COUNTER;
        }
        SendMsg(KBD_DOWN_EVENT,temp,0);
      }
    }else{
      if(_repeatcount){
        _repeatcount--;
        if(_repeatcount==0) SendMsg(KBD_REPEAT_EVENT,temp,0);
      }
    }
  }else{
    kbd_scan_code=0;
  }
}

extern U16 IsShowTime;

#ifdef IAM_EMU
void isr_TBCCR0()
#else
#pragma vector=TIMERB0_VECTOR
__interrupt void isr_TBCCR0()
#endif
{
  _KbdScan();
  if(silence_count){
    silence_count--;
    if(silence_count==0){
      silence_flag=1;
      if(IsShowTime){
        _TextXY(0,3,"\x94\x95",0);
        LCD_Update(NULL);
      }
    }
  }
  if(_gtimer>16){
    _gtimer=0;
//    SendMsg(KBD_DOWN_EVENT,KBD_TIMER,0);
  }else{
    _gtimer++;
  }
}

#ifdef IAM_EMU
void isr_port_2(void)
#else
#pragma vector=PORT2_VECTOR
__interrupt void isr_port_2(void)
#endif

{
  U8 temp;
//  P2IFG &= ~0x0F0;
  KBD_EnableInterrupt();
  KBD_Init();
  temp=KBDLUT[KBD_scan()];
  if(temp==KBD_ON){
//  if(areyousleeping){
	  KBD_ClrInterrupt();
//      areyousleeping = 0;
      kbd_scan_code=0;
      SendMsg(KBD_DOWN_EVENT,temp,0);
    __low_power_mode_off_on_exit();
  }else{
    KBD_Detach();
  }
}
