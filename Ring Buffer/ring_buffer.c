#include <stddef.h>

#include "ring_buffer.h"

RB_Status_t RB_Write(RB_t *Buffer, uint8_t Data)
{
    if (Buffer == NULL)
    {
        return RB_ERROR;
    }

    if ((Buffer->Head + 1) % BUFFER_SIZE == Buffer->Tail)
    {
        return RB_FULL; // Buffer is full
    }

    Buffer->Buffer[Buffer->Head] = Data;
    Buffer->Head = (Buffer->Head + 1) % BUFFER_SIZE;

    return RB_OK;
}

RB_Status_t RB_Read(RB_t *Buffer, uint8_t *Data)
{
    if (Buffer == NULL || Data == NULL)
    {
        return RB_ERROR;
    }

    if (Buffer->Head == Buffer->Tail)
    {
        return RB_EMPTY; // Buffer is empty
    }

    *Data = Buffer->Buffer[Buffer->Tail];
    Buffer->Tail = (Buffer->Tail + 1) % BUFFER_SIZE;

    return RB_OK;
}

RB_Status_t RB_Flush(RB_t *Buffer)
{
    if (Buffer == NULL)
    {
        return RB_ERROR;
    }
    Buffer->Head = 0;
    Buffer->Tail = 0;
    return RB_OK;
}

RB_Status_t RB_Init(RB_t *Buffer)
{
    if (Buffer == NULL)
    {
        return RB_ERROR;
    }
    Buffer->Head = 0;
    Buffer->Tail = 0;
    return RB_OK;
}