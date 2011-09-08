#ifndef	_DISPATCH_H
#define	_DISPATCH_H

//#include <hw_r.h>

typedef void (*FKBDHANDLER)(U8);

typedef struct _process_tag {
  void (*KbdHandler) (U8 key);
  void *CtrlHandler;
} TDISP_PROC, *PDISP_PROC;

#define MAX_MESS_QUEUE 16

typedef struct _D_Message {
  unsigned int Type;
  unsigned int Param1;
  unsigned int Param2;
  unsigned int Param3;
} T_Message, *P_Message;

typedef struct _message_queue_tag {
  unsigned int index_put;
  unsigned int index_get;
  T_Message Message[MAX_MESS_QUEUE];
} T_QUEUE, *P_QUEUE;

typedef void (*ONEVENT)(P_Message);

//#define STATUS_POWER_ALARM_MASK   0x8000
//#define STATUS_WRITE_ERROR_MASK   0x4000

#define VIEWR_DB_MASK 1

#define KBD_DOWN_EVENT         2
#define KBD_REPEAT_EVENT       3
#define VCL_CANCEL_EVENT       4
#define VCL_OK_EVENT           5
#define FILE_WRITE_DATA_EVENT  6
#define ALARM_POWER_EVENT      7
#define WRITE_DATA_EVENT       8
#define FILE_WRITE_ERROR_EVENT 9
#define VCL_TIMER_EVENT        10
#define KBD_SILENCE_EVENT      11
#define TIMER_EVENT            12



int SendMsg(unsigned int mes,unsigned int Param1,unsigned int Param2);
void SetDoorBell(U16 mask);

void DISPATCH_Open(void);
int DISPATCH_Task(void);
int DISPATCH_Message(void);
unsigned int DISPATCH_GetStatus(void);
void DISPATCH_SetStatus(unsigned int);

FKBDHANDLER DISPATCH_SetKbdHandler(FKBDHANDLER KbdHandler);
void SetOnWriteData(ONEVENT EventHandler);
void SetOnTimer(ONEVENT EventHandler);

#endif
