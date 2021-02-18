#ifndef __BUTTON_PROCESSING_H__
#define __BUTTON_PROCESSING_H__

#include "stdint.h"
#include "stdbool.h"

typedef enum {
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_CNT,
} ButtonT;

typedef void (*ButtonPressCb)(ButtonT);

void buttonProcessingInit(ButtonPressCb buttonPress);
void buttonProcessing(void);

#endif
