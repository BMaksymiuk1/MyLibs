#include "Button.h"
#include "Button_interface.h"

ButtonStatus_t ButtonInit(Button_t *Button, void *GPIO_Port, uint32_t GPIO_Pin, uint32_t DebounceTime, uint32_t LongPressTime,
uint32_t RepeatTime, uint32_t TimeBetweenRepeats)
{
    if(Button == NULL || GPIO_Port == NULL || GPIO_Pin == 0)
    {
        return BUTTON_STATUS_INVALID_PARAM; // Return error if any parameter is invalid
    }
    Button->State = BUTTON_STATE_IDLE;
    Button->GPIO_Port = GPIO_Port;
    Button->GPIO_Pin = GPIO_Pin;
    Button->DebounceTime = DebounceTime;
    Button->LongPressTime = LongPressTime;
    Button->RepeatTime = RepeatTime;
    Button->TimeBetweenRepeats = TimeBetweenRepeats;
    Button->ShortPressCallback = NULL;
    Button->LongPressCallback = NULL;
    Button->RepeatCallback = NULL;
    Button->ReleaseCallback = NULL;
    return BUTTON_STATUS_OK;
}

void ButtonSetDebounceTime(Button_t *Button, uint32_t DebounceTime)
{
    Button->DebounceTime = DebounceTime;
}

void ButtonSetLongPressTime(Button_t *Button, uint32_t LongPressTime)
{
    Button->LongPressTime = LongPressTime;
}

void ButtonSetRepeatTime(Button_t *Button, uint32_t RepeatTime)
{
    Button->RepeatTime = RepeatTime;
}

void ButtonSetTimeBetweenRepeats(Button_t *Button, uint32_t TimeBetweenRepeats)
{
    Button->TimeBetweenRepeats = TimeBetweenRepeats;
}

ButtonStatus_t ButtonRegisterShortPressCallback(Button_t *Button, void (*ShortPressCallback)())
{
    if(Button == NULL || ShortPressCallback == NULL)
    {
        return BUTTON_STATUS_INVALID_PARAM;
    }
    Button->ShortPressCallback = ShortPressCallback;
    return BUTTON_STATUS_OK;
}

ButtonStatus_t ButtonRegisterLongPressCallback(Button_t *Button, void (*LongPressCallback)())
{
    if(Button == NULL || LongPressCallback == NULL)
    {
        return BUTTON_STATUS_INVALID_PARAM;
    }
    Button->LongPressCallback = LongPressCallback;
    return BUTTON_STATUS_OK;
}

ButtonStatus_t ButtonRegisterRepeatCallback(Button_t *Button, void (*RepeatCallback)())
{
    if(Button == NULL || RepeatCallback == NULL)
    {
        return BUTTON_STATUS_INVALID_PARAM;
    }
    Button->RepeatCallback = RepeatCallback;
    return BUTTON_STATUS_OK;
}

ButtonStatus_t ButtonRegisterReleaseCallback(Button_t *Button, void (*ReleaseCallback)())
{
    if(Button == NULL || ReleaseCallback == NULL)
    {
        return BUTTON_STATUS_INVALID_PARAM;
    }
    Button->ReleaseCallback = ReleaseCallback;
    return BUTTON_STATUS_OK;
}

void ButtonUnregisterShortPressCallback(Button_t *Button)
{
    Button->ShortPressCallback = NULL;
}

void ButtonUnregisterLongPressCallback(Button_t *Button)
{
    Button->LongPressCallback = NULL;
}

void ButtonUnregisterRepeatCallback(Button_t *Button)
{
    Button->RepeatCallback = NULL;
}

void ButtonUnregisterReleaseCallback(Button_t *Button)
{
    Button->ReleaseCallback = NULL;
}

static uint8_t ButtonLongPressed(Button_t *Button)
{
    if(ButtonGetTick() - Button->LastTick >= Button->LongPressTime)
    {
        return 1; // Button is long pressed
    }
    else
    {
        return 0; // Button is not long pressed
    }
}

static void ButtonIdleRoutine(Button_t *Button)
{
    if(ButtonIsPressed(Button)) //Button is possibly pressed, start debouncing
    {
        Button->State = BUTTON_STATE_DEBOUNCE;
        Button->LastTick = ButtonGetTick();
    }
}

static void ButtonDebounceRoutine(Button_t *Button)
{
    uint32_t CurrentTick = ButtonGetTick();
   if(CurrentTick - Button->LastTick >= Button->DebounceTime) //wait for the debounce time to pass
    {
        if(ButtonIsPressed(Button)) //check if the button is still pressed after debounce time
        {
            Button->State = BUTTON_STATE_PRESSED; // Button is debounced and pressed
            Button->LastTick = CurrentTick; // Update the last tick to the current tick for further processing
        }
        else
        {
            Button->State = BUTTON_STATE_IDLE; // Button is not pressed after debounce time
        }
    }
    
}

static void ButtonPressedRoutine(Button_t *Button)
{
    if(ButtonIsPressed(Button)){
        if(ButtonLongPressed(Button)) //check if the button is long pressed
        {
            Button->State = BUTTON_STATE_LONG_PRESS;
            Button->LastTick = ButtonGetTick();
        } 
    }
    else //button pressed too short, consider it a short press
        {
            Button->State = BUTTON_STATE_SHORT_PRESS;
        }
}

static void ButtonShortPressRoutine(Button_t *Button)
{
    if(Button->ShortPressCallback != NULL)
    {
        Button->ShortPressCallback(Button); //User defined callback for ShortPress 
    }
    Button->State = BUTTON_STATE_WAIT_FOR_RELEASE;
    Button->LastTick = ButtonGetTick();
}

static void ButtonLongPressRoutine(Button_t *Button)
{
    if(!ButtonIsPressed(Button))
    {
        if(Button->LongPressCallback == NULL)
        {
            Button->State = BUTTON_STATE_WAIT_FOR_RELEASE;
            Button->LastTick = ButtonGetTick();
        }
        else
        {
            Button->LongPressCallback(Button); //User defined callback for LongPress
            Button->State = BUTTON_STATE_WAIT_FOR_RELEASE;
            Button->LastTick = ButtonGetTick();
        }
        
    }
    else if((ButtonGetTick() - Button->LastTick >= Button->RepeatTime) && (Button->TimeBetweenRepeats > 0))
    {
        Button->State = BUTTON_STATE_REPEAT;
    }
}

static void ButtonWaitForReleaseRoutine(Button_t *Button)
{
    if(ButtonGetTick() - Button->LastTick >= Button->DebounceTime) //wait for the debounce time to pass
    {
        if(!ButtonIsPressed(Button)) //Wait for the button to be released
        {
            Button->State = BUTTON_STATE_RELEASE;
        }
    }
}

static void ButtonRepeatRoutine(Button_t *Button)
{
    uint32_t CurrentTick = ButtonGetTick();
    if(ButtonIsPressed(Button) && (CurrentTick - Button->LastTick >= Button->TimeBetweenRepeats)) //doing something while the button is pressed
    {
        if(Button->RepeatCallback != NULL)
        {
            Button->RepeatCallback(Button); //User defined callback for Repeat
            Button->LastTick = CurrentTick;
        }
    }
    else if(!ButtonIsPressed(Button)) //Button released
    {
        Button->State = BUTTON_STATE_WAIT_FOR_RELEASE;
        Button->LastTick = CurrentTick;
    }
}

static void ButtonReleaseRoutine(Button_t *Button)
{
    if(Button->ReleaseCallback != NULL)
    {
        Button->ReleaseCallback(Button); //User defined callback for Release
    }
    Button->State = BUTTON_STATE_IDLE;
}

ButtonStatus_t ButtonTask(Button_t *Button) //This function should be called in the main loop to handle button state transitions
{
    if(Button == NULL)
    {
        return BUTTON_STATUS_INVALID_PARAM; // Return error if the button pointer is NULL
    }
    switch(Button->State)
    {
        case BUTTON_STATE_IDLE:
            ButtonIdleRoutine(Button);
            break;

        case BUTTON_STATE_DEBOUNCE:
            ButtonDebounceRoutine(Button);
            break;

        case BUTTON_STATE_PRESSED:
            ButtonPressedRoutine(Button);
            break;

        case BUTTON_STATE_SHORT_PRESS:
            ButtonShortPressRoutine(Button);
            break;
        
        case BUTTON_STATE_LONG_PRESS:
            ButtonLongPressRoutine(Button);
            break;
        
        case BUTTON_STATE_WAIT_FOR_RELEASE:
            ButtonWaitForReleaseRoutine(Button);
            break;

        case BUTTON_STATE_REPEAT:
            ButtonRepeatRoutine(Button);
            break;

        case BUTTON_STATE_RELEASE:
            ButtonReleaseRoutine(Button);
            break;
        default:
            Button->State = BUTTON_STATE_IDLE; // Reset to idle state in case of an unexpected
            break;
    }
    return BUTTON_STATUS_OK; // Return OK status after processing the button task
}


