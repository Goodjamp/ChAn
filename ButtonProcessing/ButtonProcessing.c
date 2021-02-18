#include "stdint.h"
#include "stdbool.h"

#include "nrf.h"

#include "ButtonProcessing.h"

#define READ_PIN(X)       (NRF_GPIO->IN & (1 << X))
#define DEBOUNCE_TICKS    150

static struct {
    uint32_t pin;
    uint32_t debounce;
} buttonsList[] = {
    [BUTTON_UP] = {
        .pin = 22,
        .debounce = 0

    },
    [BUTTON_DOWN] = {
        .pin = 24,
        .debounce = 0
    },
};
static ButtonPressCb buttonPressCb;

void buttonProcessingInit(ButtonPressCb buttonPress)
{
    for (uint32_t k = 0; k < sizeof(buttonsList) / sizeof(buttonsList[0]); k++) {
        NRF_GPIO->PIN_CNF[buttonsList[k].pin] =
                                   (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos)
                                   | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
                                   | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
                                   | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
                                   | (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos);
    }
    buttonPressCb = buttonPress;
}

void buttonProcessing(void)
{
    if (!buttonPressCb) {
        return;
    }
    for (uint32_t k = 0; k < sizeof(buttonsList) / sizeof(buttonsList[0]); k++) {
        if (buttonsList[k].debounce){
            buttonsList[k].debounce--;
            continue;
        }
        if(!READ_PIN(buttonsList[k].pin)) {
            buttonsList[k].debounce = DEBOUNCE_TICKS;
            buttonPressCb((ButtonT)k);

        }
    }
}
