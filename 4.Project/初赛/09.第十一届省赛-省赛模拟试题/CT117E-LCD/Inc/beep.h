#ifndef __BEEP_H
#define __BEEP_H

#include "stm32f10x.h"

void beep_init(void);
void beep_ctrl(FunctionalState NewState);

#endif /*__BEEP_H*/
