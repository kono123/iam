#ifndef	_TS_FUNC_H
#define	_TS_FUNC_H

#include <hardware.h>
#include <timeslot.h>

void tsReserv(PStageRec sr);
void tsKbdScan(PStageRec sr);
void tsViewTime(PStageRec sr);
void tsBatery(PStageRec sr);

#endif
