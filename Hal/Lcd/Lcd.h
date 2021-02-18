#ifndef __LCD_H__
#define __LCD_H__

#include <stdint.h>
#include <stdbool.h>

#define SCREEN_WIDTH     128
#define SCREEN_HEIGHT    64

void lcdInit(void);
void lcdDot(uint32_t x, uint32_t y, bool isOn);
void lcdRefresh(void);
uint8_t * lcdgetLcdBuff(void);

#endif
