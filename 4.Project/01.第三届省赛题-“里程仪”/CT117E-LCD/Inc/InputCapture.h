#ifndef __INPUTCAPTURE_H
#define __INPUTCAPTURE_H

#include "stm32f10x.h"

extern uint16_t Time2_IC2ReadValue1;
extern uint16_t Time2_IC2ReadValue2;
extern uint8_t Time2_CaptureNumber;
extern uint32_t Time2_Capture;
extern uint32_t TIM2Freq;
extern uint32_t total_meter_now;
void Time2_InputCapture_Init(void);

#endif /*__INPUTCAPTURE_H*/
