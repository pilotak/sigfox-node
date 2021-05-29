#include "PeripheralPins.h"
#include "mbed_toolchain.h"

const PinMap PinMap_UART_TX[] = {
    {PA_2_ALT0,  LPUART_1, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF6_LPUART1)}, // Connected to STDIO_UART_TX
    {PA_9,       UART_1,   STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF1_USART1)},
    {NC, NC, 0}
};

const PinMap PinMap_UART_RX[] = {
    {PA_3_ALT0,  LPUART_1, STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF6_LPUART1)}, // Connected to STDIO_UART_RX
    {PA_10,      UART_1,   STM_PIN_DATA(STM_MODE_AF_PP, GPIO_PULLUP, GPIO_AF1_USART1)},
    {NC, NC, 0}
};