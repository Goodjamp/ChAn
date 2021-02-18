#include "Lcd.h"
#include <stddef.h>
#include "nrf.h"

#define LCD_SDA 5
#define LCD_SCL 7

#define LCD_IIC NRF_TWI0

#define LCD_ADDR (0x78 >> 1)

#define LCD_MEMORYMODE          0x20
#define LCD_COLUMNADDR          0x21
#define LCD_PAGEADDR            0x22
#define LCD_SETCONTRAST         0x81
#define LCD_CHARGEPUMP          0x8D
#define LCD_SEGREMAP            0xA0
#define LCD_DISPLAYALLON_RESUME 0xA4
#define LCD_DISPLAYALLON        0xA5
#define LCD_NORMALDISPLAY       0xA6
#define LCD_INVERTDISPLAY       0xA7
#define LCD_SETMULTIPLEX        0xA8
#define LCD_DISPLAYOFF          0xAE
#define LCD_DISPLAYON           0xAF
#define LCD_COMSCANINC          0xC0
#define LCD_COMSCANDEC          0xC8
#define LCD_SETDISPLAYOFFSET    0xD3
#define LCD_SETDISPLAYCLOCKDIV  0xD5
#define LCD_SETPRECHARGE        0xD9
#define LCD_SETCOMPINS          0xDA
#define LCD_SETVCOMDETECT       0xDB

#define LCD_SETLOWCOLUMN        0x00
#define LCD_SETHIGHCOLUMN       0x10
#define LCD_SETSTARTLINE        0x40

#define LCD_EXTERNALVCC         0x01
#define LCD_SWITCHCAPVCC        0x02

#define LCD_RIGHT_HORIZONTAL_SCROLL              0x26
#define LCD_LEFT_HORIZONTAL_SCROLL               0x27
#define LCD_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define LCD_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL  0x2A
#define LCD_DEACTIVATE_SCROLL                    0x2E
#define LCD_ACTIVATE_SCROLL                      0x2F
#define LCD_SET_VERTICAL_SCROLL_AREA             0xA3

#define LCD_LCDWIDTH  128
#define LCD_LCDHEIGHT  64

static uint8_t lcdBuff[LCD_LCDWIDTH * LCD_LCDHEIGHT / 8 + 1];

static void lcdWrite(uint8_t data[], uint32_t size)
{
    LCD_IIC->EVENTS_TXDSENT = 0;
    LCD_IIC->TXD = data[0];
    LCD_IIC->TASKS_STARTTX = 1;
    while (LCD_IIC->EVENTS_TXDSENT == 0) {

    }
    for (uint32_t  i = 1; i < size; i++) {
        LCD_IIC->EVENTS_TXDSENT = 0;
        LCD_IIC->TXD = data[i];
        while (LCD_IIC->EVENTS_TXDSENT == 0) {

        }
    }
    LCD_IIC->EVENTS_STOPPED = 0;
    LCD_IIC->TASKS_STOP = 1;
    while (LCD_IIC->EVENTS_STOPPED == 0) {

    }
}

static void lcdWriteCommand(uint8_t cmd)
{
    uint8_t buff[] = {0x80, cmd};
    lcdWrite(buff, sizeof(buff));
}

void lcdInit(void)
{
    lcdBuff[0] = 0x40;
    NRF_GPIO->PIN_CNF[LCD_SDA] = GPIO_PIN_CNF_DRIVE_S0D1 << GPIO_PIN_CNF_DRIVE_Pos;
    NRF_GPIO->PIN_CNF[LCD_SCL] = GPIO_PIN_CNF_DRIVE_S0D1 << GPIO_PIN_CNF_DRIVE_Pos;
    LCD_IIC->PSELSCL = LCD_SCL;
    LCD_IIC->PSELSDA = LCD_SDA;
    LCD_IIC->FREQUENCY = TWI_FREQUENCY_FREQUENCY_K400;
    LCD_IIC->ADDRESS = LCD_ADDR;
    LCD_IIC->ENABLE = TWI_ENABLE_ENABLE_Enabled;

    lcdWriteCommand(LCD_DISPLAYOFF);
    lcdWriteCommand(LCD_SETDISPLAYCLOCKDIV);
    lcdWriteCommand(0x80);
    lcdWriteCommand(LCD_SETMULTIPLEX);
    lcdWriteCommand(LCD_LCDHEIGHT - 1);
    lcdWriteCommand(LCD_SETDISPLAYOFFSET);
    lcdWriteCommand(0x00);
    lcdWriteCommand(LCD_SETSTARTLINE | 0x00);
    lcdWriteCommand(LCD_CHARGEPUMP);
    lcdWriteCommand(0x14);
    lcdWriteCommand(LCD_MEMORYMODE);
    lcdWriteCommand(0x00);
    lcdWriteCommand(LCD_SEGREMAP | 0x1);
    lcdWriteCommand(LCD_COMSCANDEC | 0x1);
    lcdWriteCommand(LCD_SETCOMPINS);
    lcdWriteCommand(0x12);
    lcdWriteCommand(LCD_SETCONTRAST);
    lcdWriteCommand(0xCF);
    lcdWriteCommand(LCD_SETPRECHARGE);
    lcdWriteCommand(0xF1);
    lcdWriteCommand(LCD_SETVCOMDETECT);
    lcdWriteCommand(0x40);
    lcdWriteCommand(LCD_DISPLAYALLON_RESUME);
    lcdWriteCommand(LCD_NORMALDISPLAY);
    lcdWriteCommand(LCD_DEACTIVATE_SCROLL);
    lcdWriteCommand(LCD_DISPLAYON);
    lcdWriteCommand(LCD_COLUMNADDR);
    lcdWriteCommand(0);
    lcdWriteCommand(0x7F);
    lcdWriteCommand(LCD_PAGEADDR);
    lcdWriteCommand(0);
    lcdWriteCommand(0x07);
}

void lcdDot(uint32_t x, uint32_t y, bool isOn)
{
    uint8_t *pBuff = &lcdBuff[1];
    if (isOn == true) {
        pBuff[((y >> 3) << 7) + x] |= (1 << (y & 7));
    } else {
        pBuff[((y >> 3) << 7) + x] &= ~(1 << (y & 7));
    }
}

void lcdRefresh(void)
{
    lcdWrite(lcdBuff, sizeof(lcdBuff));
}

uint8_t * lcdgetLcdBuff(void)
{
    return &lcdBuff[1];
}
