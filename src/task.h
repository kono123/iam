#ifndef	_TASK_H
#define	_TASK_H

#include <hw_r.h>

#define VIEW_R_EVENT 1
#define KBD_EVENT    2

#define VIEWR_DB_MASK 1

void MainTaskInit(void);
void MainTask(void);
int SendMessage(unsigned int mes,unsigned int Param1,unsigned int Param2);
void SetDoorBell(U16 mask);

void StartWorkTimeSlot(U16 cr0);
void StopWorkTimeSlot(void);

#endif
