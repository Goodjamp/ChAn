#include "Timer.h"

#include <stddef.h>
#include <nrf.h>

#define TIMER_BASE                  NRF_TIMER2
#define TIMER_IRQ_HANDLER           TIMER2_IRQHandler
#define TIMER_IRQ_N                 TIMER2_IRQn

#define TIMER_CLOCK_PRESCALER       4
#define KILO                        1000

static void (*eventCallback)(void);

void TIMER_IRQ_HANDLER(void)
{
    TIMER_BASE->EVENTS_COMPARE[0] = 0x00;
    if (eventCallback != NULL) {
        eventCallback();
    }
    __DSB();

}

void timerInit(uint32_t eventPeriodMs, void (*eventCb)(void))
{
    eventCallback = eventCb;
    TIMER_BASE->MODE = TIMER_MODE_MODE_Timer << TIMER_MODE_MODE_Pos;
    TIMER_BASE->BITMODE = TIMER_BITMODE_BITMODE_32Bit << TIMER_BITMODE_BITMODE_Pos;
    TIMER_BASE->PRESCALER = TIMER_CLOCK_PRESCALER;
    TIMER_BASE->CC[0] = eventPeriodMs * KILO;
    TIMER_BASE->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos;
    TIMER_BASE->INTENSET = TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos;
    NVIC_EnableIRQ(TIMER_IRQ_N);
}


void timerStart(bool isStarted)
{
    if (isStarted) {
        TIMER_BASE->TASKS_START = 0x01;
    } else {
        TIMER_BASE->TASKS_STOP = 0x01;
    }
}

