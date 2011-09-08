#include <hardware.h>
#include <timeslot.h>
#include <ts_func.h>

#include <stdio.h>

#define MAX_OPTION_CYCLES 4

const TStageRec OptionStageRec[MAX_OPTION_CYCLES]={
/*00*/  {tsBatery,0,0},
/*01*/  {tsReserv,0,0},
/*02*/  {tsReserv,0,0},
/*03*/  {tsReserv,0,0},
};

const TTimeSlotRec OptionTimeSlot ={
  MAX_OPTION_CYCLES,
  10,
  OptionStageRec
};

PTimeSlotRec GetPtrOptionTimeSlot(void)
{
  return (PTimeSlotRec)&OptionTimeSlot;
}
