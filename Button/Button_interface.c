#include <stdint.h>

#include "Button_interface.h"


#define STM32 //choose your platform here

#ifdef STM32

#include "stm32f4xx_hal.h" // Include the STM32 HAL library for GPIO and timing functions

uint8_t ButtonIsPressed(Button_t *Button)
{
    return HAL_GPIO_ReadPin(Button->GPIO_Port, Button->GPIO_Pin); // Returns 1 if the button is pressed, 0 otherwise
}


uint32_t ButtonGetTick(void)
{
    return HAL_GetTick(); // Returns the current tick count in milliseconds
}


#endif // STM32

#ifdef ESP32
// ESP32 if
#endif // ESP32