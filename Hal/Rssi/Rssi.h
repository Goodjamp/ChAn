#ifndef __RSSI_H__
#define __RSSI_H__

#include <stdint.h>
#include <stdbool.h>

void rssiInit(void);
uint8_t rssiGet(uint8_t channel);
void rssiDeinit(void);

#endif
