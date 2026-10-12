#ifndef BUTTON_INTERFACE_H
#define BUTTON_INTERFACE_H

#include <stdint.h>

#include "Button.h"

uint8_t ButtonIsPressed(Button_t *Button); //returns 1 if the button is pressed, 0 otherwise
uint32_t ButtonGetTick(void); //returns the current tick count in milliseconds
#endif // BUTTON_INTERFACE_H