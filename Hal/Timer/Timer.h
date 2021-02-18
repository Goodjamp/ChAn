#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>
#include <stdbool.h>

void timerInit(uint32_t eventPeriodMs, void (*eventCb)(void));
void timerStart(bool isStarted);


#endif
