#ifndef	_TIME_SLOT_H
#define	_TIME_SLOT_H

#include <hardware.h>

typedef struct _stage_rec_tag {
  void (*OnTimeEvent) (struct _stage_rec_tag *pctrl);
  U16 Param1;
  U16 Param2;
} TStageRec, *PStageRec;

typedef struct _time_slot_rec_tag {
  U16 Stages;
  U16 SecondCount;
  TStageRec const *pStageRec;
} TTimeSlotRec, *PTimeSlotRec;

void StartTimeSlot(PTimeSlotRec tsr);
void StopTimeSlot(void);
U16 GetSecCounterTimeSlot(void);

#endif
