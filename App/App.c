#include <stdint.h>
#include <stdbool.h>

#include "Lcd.h"
#include "Rssi.h"
#include "Timer.h"

#include "nrf.h"
#include "frameHall.h"
#include "ButtonProcessing.h"
#include "Images.h"

#define SCREEN_MAX_X           (SCREEN_WIDTH - 1)
#define SCREEN_MAX_Y           (SCREEN_HEIGHT - 1)
#define GRAPH_SHIFT            1
#define RSSI_MAX_VALUE_INIT    26
#define RSSI_MIN_VALUE         89

typedef enum {
    MoveMarkerHold,
    MoveMarkerHUp,
    MoveMarkerHDownd,
    MoveMarkerVRight,
    MoveMarkerVLeft,
} MoveMarker;

static volatile bool isLcdRefresh;
static volatile uint32_t delayCounter = 0;
static FrameDescr frameDescr;
static FrameHandl frameH = &frameDescr;
static uint32_t rssiMaxLevel = RSSI_MAX_VALUE_INIT;

static volatile struct {
    uint8_t currentLevel;
    MoveMarker move;
    uint8_t text[4];
} hMarkerState = {
    .currentLevel = RSSI_MAX_VALUE_INIT
                    + SCREEN_MAX_Y / 2,
    .move = MoveMarkerHold,
    .text = "-00"
};

static void startHfClock(void)
{
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;

    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {

    }
}

void timerEvent(void)
{
    static uint32_t lcdRefreshCounter = 0;

    buttonProcessing();
    if (delayCounter > 0) {
        delayCounter--;
    }
    if (lcdRefreshCounter++ == 33) {
        lcdRefreshCounter = 0;
        isLcdRefresh = true;
    }
}

static void delayMs(uint32_t msToDelay)
{
    delayCounter = msToDelay;
    while (delayCounter != 0) {

    }
}

void buttonPress(ButtonT button)
{
    switch (button) {
    case BUTTON_UP:
        hMarkerState.move = MoveMarkerHUp;
        break;

    case BUTTON_DOWN:
        hMarkerState.move = MoveMarkerHDownd;
        break;

    default:
        break;
    }
}

static inline void updateMarkerStr(void) {
    hMarkerState.text[1] = hMarkerState.currentLevel / 10 + '0';
    hMarkerState.text[2] = hMarkerState.currentLevel % 10 + '0';
}

static inline void updateScreenUserArea(void)
{
    /*Add marker position text*/
    frameSetPosition(frameH, 104, 8);
    frameAddString(frameH, hMarkerState.text, ARIAL_8PTS, true);
    frameSetPosition(frameH, 104, 8 + 12);
    frameAddString(frameH, "dB", ARIAL_8PTS, true);
    /*Add marker line*/
    frameAddHorizontalLine(frameH,
                           (Point){1, hMarkerState.currentLevel - rssiMaxLevel},
                           1, 100);
}

void corsairSeensaver(void)
{
    uint32_t pos = 0;
    while (pos < (SCREEN_WIDTH - corsairLogoWidthPixels)) {
        delayMs(40);
        frameClear(frameH);
        frameSetPosition(frameH, pos, 0);
        frameAddImage(frameH, corsairLogoBitmaps, corsairLogoHeightPixels, corsairLogoWidthPixels, true, false);
        lcdRefresh();
        if (pos == (SCREEN_WIDTH - corsairLogoWidthPixels) / 2) {
            delayMs(3000);
        }
        pos += 4;
    }
}

void app(void)
{
    static uint32_t h[101] = {[0 ... 100] = 63};
    uint32_t rssi;
    bool isNewFrame = true;

    startHfClock();
    timerInit(1, timerEvent);
    timerStart(true);
    delayMs(200);
    rssiInit();
    lcdInit();
    buttonProcessingInit(buttonPress);

    updateMarkerStr();

    frameInit(frameH, lcdgetLcdBuff(), SCREEN_HEIGHT, SCREEN_WIDTH);
    corsairSeensaver();
    frameClear(frameH);

    for(;;) {
        /*Update marker state*/
        if (hMarkerState.move != MoveMarkerHold) {
            __disable_irq();
            switch (hMarkerState.move) {
            case MoveMarkerHDownd:
                if (hMarkerState.currentLevel == RSSI_MIN_VALUE) {
                    break;
                }
                hMarkerState.currentLevel++;
                if (hMarkerState.currentLevel > (rssiMaxLevel + SCREEN_MAX_Y)) {
                    rssiMaxLevel++;
                }
                break;

            case MoveMarkerHUp:
                if (hMarkerState.currentLevel == 0) {
                    break;
                }
                hMarkerState.currentLevel--;
                if (hMarkerState.currentLevel < rssiMaxLevel) {
                    rssiMaxLevel = hMarkerState.currentLevel;
                }
                break;

            default: break;
            }
            updateMarkerStr();
            hMarkerState.move = MoveMarkerHold;
            __enable_irq();
        }
        frameClear(frameH);
        updateScreenUserArea();

        for (int32_t i = 100; i >= 0; i--) {
            rssi = rssiGet(i);

            if (rssi > h[i]) {
                rssi = ((rssi << 7) + (h[i] << 7)) >> 8;
            }


            /**/
            //if (isNewFrame) {
                if (rssi < (rssiMaxLevel + SCREEN_MAX_Y)) {
                    for (uint32_t k = (rssi < rssiMaxLevel) ? 0 : (rssi - rssiMaxLevel); k < SCREEN_HEIGHT; k++) {
                        lcdDot(GRAPH_SHIFT + i, k, 1);
                    }
                }
            /*
            }  else {
                if (rssi < h[i]) {
                    uint32_t targetPos = (rssi < rssiMaxLevel) ? 0 : rssi - rssiMaxLevel;
                    uint32_t currentPos = (rssi < rssiMaxLevel) ? 0 : rssi - rssiMaxLevel;
                    for (uint32_t k = h[i]; k >= targetPos; k--) {
                        lcdDot(GRAPH_SHIFT + i, k - rssiMaxLevel, 1);
                    }
                } else if (rssi > h[i]) {
                    for (uint32_t k = h[i]; k <= rssi; k++) {
                        lcdDot(GRAPH_SHIFT + i, k - rssiMaxLevel, 0);
                    }
                }
            }
            */
            h[i] = rssi;
        }

        if (isLcdRefresh == true) {
            lcdRefresh();
            isLcdRefresh = false;
        }
    }
}

