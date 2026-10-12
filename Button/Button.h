#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>
#include <stddef.h>

typedef enum
{
    BUTTON_STATUS_OK = 0,
    BUTTON_STATUS_INVALID_PARAM = -1
}ButtonStatus_t;

typedef enum  //This enumeration defines the possible states of a button
{
    BUTTON_STATE_IDLE = 0,
    BUTTON_STATE_DEBOUNCE,
    BUTTON_STATE_PRESSED,
    BUTTON_STATE_SHORT_PRESS,
    BUTTON_STATE_LONG_PRESS,
    BUTTON_STATE_WAIT_FOR_RELEASE,
    BUTTON_STATE_REPEAT,
    BUTTON_STATE_RELEASE
}ButtonState_t;

typedef struct ButtonStruct Button_t; // Forward declaration of the Button structure

typedef struct ButtonStruct//This structure holds the configuration and state of a button
{
    ButtonState_t State;

    void *GPIO_Port;
    uint32_t GPIO_Pin;

    uint32_t DebounceTime;
    uint32_t LongPressTime;
    uint32_t RepeatTime;
    uint32_t TimeBetweenRepeats;
    uint32_t LastTick;

    void (*ShortPressCallback)(Button_t *Button);
    void (*LongPressCallback)(Button_t *Button);
    void (*RepeatCallback)(Button_t *Button);
    void (*ReleaseCallback)(Button_t *Button);
}Button_t;

ButtonStatus_t ButtonInit(Button_t *Button, void *GPIO_Port, uint32_t GPIO_Pin, uint32_t DebounceTime, uint32_t LongPressTime,
            uint32_t RepeatTime, uint32_t TimeBetweenRepeats); //This function initializes the button with user's configuration
void ButtonSetDebounceTime(Button_t *Button, uint32_t DebounceTime); //This function sets the debounce time for the button
void ButtonSetLongPressTime(Button_t *Button, uint32_t LongPressTime); //This function sets the long press time for the button
void ButtonSetRepeatTime(Button_t *Button, uint32_t RepeatTime); //This function sets the repeat mode time for the button
//This function sets the time between repeats for the button, set to 0 to disable repeat mode
void ButtonSetTimeBetweenRepeats(Button_t *Button, uint32_t TimeBetweenRepeats); 

//Callback registration functions
ButtonStatus_t ButtonRegisterShortPressCallback(Button_t *Button, void (*ShortPressCallback)());
ButtonStatus_t ButtonRegisterLongPressCallback(Button_t *Button, void (*LongPressCallback)());
ButtonStatus_t ButtonRegisterRepeatCallback(Button_t *Button, void (*RepeatCallback)());
ButtonStatus_t ButtonRegisterReleaseCallback(Button_t *Button, void (*ReleaseCallback)());

//Callback unregistration functions
void ButtonUnregisterShortPressCallback(Button_t *Button);
void ButtonUnregisterLongPressCallback(Button_t *Button);
void ButtonUnregisterRepeatCallback(Button_t *Button);
void ButtonUnregisterReleaseCallback(Button_t *Button);

ButtonStatus_t ButtonTask(Button_t *Button); //This function should be called in the main loop to handle button state transitions

#endif // BUTTON_H