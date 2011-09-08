#ifndef	_ADC_H
#define	_ADC_H

#include <hardware.h>

U16 ADC_R(U8 ch1,U8 ch2);
U16 ADC_Batery(void);
U16 ADC_Temperature(void);
U16 ADC_UBase(void);

void ADC_ConnectR(U8 ch1,U8 ch2);
U16 ADC_R_(void);
U16 ADC_pH(U8 ch);
void ADC_SetHold(U8 hold);
U16 ADC_GetHold(void);

#endif
