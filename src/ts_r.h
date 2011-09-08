#ifndef	_TS_R_H
#define	_TS_R_H

#include <hardware.h>
#include <timeslot.h>
#include <ts_func.h>

#define MAX_R_CYCLES 50 //40
#define MAX_PH_CYCLES 10
//#define MAX_R_ITEMS 7
//#define MAX_R_BUFFERS 10
#define MAX_ITEMS 16
//#define MAX_BUFFERS 40

#define MAX_TIME_STAMP 60*30

typedef struct _HeaderIAM_tag {
  U16 Signature;
  U16 HeaderBytes;
  U16 FrameBytes;
} THeaderIAM, *PHeaderIAM;

typedef struct _FrameR_tag {
  U16 Signature;
  U16 HeaderBytes;
  U16 rbuff[MAX_ITEMS];
} TFrameR, *PFrameR;

PCalibrPHRecord FindPHProbeByN(char *strN);
PCalibrPHRecord AddPHProbe(PCalibrPHRecord pRec);
int UpdatePHProbe(PCalibrPHRecord pRec);
int ErasePHProbe(char *strN);
char *GetPHProbeStr(U8 index);
PCalibrPHRecord GetPHProbePtr(U8 index);
int ProbeToFile(char *strN);
int FileToProbe(char *strN);


PTimeSlotRec GetPtrOptionTimeSlot(void);
PTimeSlotRec GetPtrRTimeSlot(void);
PTimeSlotRec GetPtrPHTimeSlot(void);

int PreViewResearch(int type,U8 NProbes);
int Research(char *Title,int flag,U32 duration);
int Calibrate(char *Title,int type,U32 duration);

int CalibratePH(char *Title,int type,U8 NumberSolution,U16 *ptr,U8 nSulution);

int ViewPH(char *Title,int type,U8 NProbes,U16 *ptr);

void  GetSerialNumberStr(char *str);
void  GetDeviceNameStr(char *str);
void  GetVersionFwStr(char *str);

int DebugViewResearch(int type,U8 NProbes);

void WriteCalibrR(void);

#endif
