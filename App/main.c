#include "App.h"
#include "system_nrf51.h"

int main(void)
{
    SystemCoreClockUpdate();

    app();

    for (;;) {

    }

    return 0;
}
