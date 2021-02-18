#include "Rssi.h"
#include <stddef.h>
#include "nrf.h"

#define RADIO_BASE_0                        0xAC5EAB5C
#define RADIO_BASE_1                        0x5DACF5AE
#define RADIO_PREFIX_0                      0x5EBA37A7
#define RADIO_PREFIX_1                      0x7F5DB7CA
#define RADIO_TX_POWER                      (RADIO_TXPOWER_TXPOWER_Pos4dBm << RADIO_TXPOWER_TXPOWER_Pos)
#define RADIO_MODE                          (RADIO_MODE_MODE_Nrf_2Mbit << RADIO_MODE_MODE_Pos)
#define RADIO_CRCCNF                        (RADIO_CRCCNF_LEN_Two << RADIO_CRCCNF_LEN_Pos)
#define RADIO_CRCINIT                       0xFFFFUL
#define RADIO_CRCPOLY                       0x11021UL
#define RADIO_PCNF0                         (8 << RADIO_PCNF0_LFLEN_Pos) | (8 << RADIO_PCNF0_S1LEN_Pos)
#define RADIO_PCNF1                         (4 << RADIO_PCNF1_BALEN_Pos)

void rssiInit(void)
{
    NRF_RADIO->BASE0 = RADIO_BASE_0;
    NRF_RADIO->BASE1 = RADIO_BASE_1;
    NRF_RADIO->PREFIX0 = RADIO_PREFIX_0;
    NRF_RADIO->PREFIX1 = RADIO_PREFIX_1;
    NRF_RADIO->TXPOWER = RADIO_TX_POWER;
    NRF_RADIO->MODE = RADIO_MODE;

    NRF_RADIO->FREQUENCY = 0;

    NRF_RADIO->CRCCNF = RADIO_CRCCNF;
    NRF_RADIO->CRCINIT = RADIO_CRCINIT;
    NRF_RADIO->CRCPOLY = RADIO_CRCPOLY;
    NRF_RADIO->PCNF0 = RADIO_PCNF0;

    NRF_RADIO->PCNF1 = RADIO_PCNF1 | (1 << RADIO_PCNF1_MAXLEN_Pos);
    NRF_RADIO->TXADDRESS = 0;
    NRF_RADIO->RXADDRESSES = 0b11111111;

    static uint32_t rxDataBuffer;
    NRF_RADIO->PACKETPTR = (uint32_t) &rxDataBuffer;
}

uint8_t rssiGet(uint8_t channel)
{
    NRF_RADIO->FREQUENCY = channel;
    NRF_RADIO->EVENTS_READY = 0x00;
    NRF_RADIO->EVENTS_RSSIEND = 0x00;
    NRF_RADIO->TASKS_RXEN = 0x01;
    while (NRF_RADIO->EVENTS_READY == 0x00) {

    }
    NRF_RADIO->TASKS_START = 0x01;
    NRF_RADIO->TASKS_RSSISTART = 0x01;
    while (NRF_RADIO->EVENTS_RSSIEND == 0x00) {

    }
    NRF_RADIO->TASKS_DISABLE = 0x01;
    return NRF_RADIO->RSSISAMPLE;
}

void rssiDeinit(void)
{
    NRF_RADIO->TASKS_STOP = 0x01;
    NRF_RADIO->TASKS_DISABLE = 0x01;

    NRF_RADIO->EVENTS_ADDRESS = 0x00;
    NRF_RADIO->EVENTS_PAYLOAD = 0x00;
    NRF_RADIO->EVENTS_DISABLED = 0x00;
}
