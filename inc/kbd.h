#ifndef	_KBD_HW_H
#define	_KBD_HW_H

#define KBD_ARROW_H  0x01
#define KBD_ARROW_V  0x02
#define KBD_0        0x30
#define KBD_1        0x31
#define KBD_2        0x32
#define KBD_3        0x33
#define KBD_4        0x34
#define KBD_5        0x35
#define KBD_6        0x36
#define KBD_7        0x37
#define KBD_8        0x38
#define KBD_9        0x39
#define KBD_STOP     0x03
#define KBD_OK       0x04
#define KBD_CANCEL   0x05
#define KBD_ON       0x06
#define KBD_ON_REP   0x86 // KBD_ON | 0x80
#define KBD_TIMER    0x20

#define MAX_KBD_REPEAT_COUNT 30
#define MAX_SILENCE_COUNTER 500

void KBD_Init(void);
void KBD_Detach(void);
U8 KBD_scan(void);
void SetSilenceCounter(U16 val);
void SetSilenceDone(void);
void SetSilenceFlag(U8 flag);

void KBD_EnableInterrupt(void);
void KBD_DisableInterrupt(void);
void KBD_ClrInterrupt(void);


#endif
