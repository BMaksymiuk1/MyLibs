
#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>

#define BUFFER_SIZE 8 // User defined size of the ring buffer

typedef enum
{
    RB_OK = 0,
    RB_FULL,
    RB_EMPTY,
    RB_ERROR
} RB_Status_t;

typedef struct
{
    volatile uint16_t Head;
    volatile uint16_t Tail;
    uint8_t Buffer[BUFFER_SIZE]; 
}RB_t;

RB_Status_t RB_Write(RB_t *Buffer, uint8_t Data);
RB_Status_t RB_Read(RB_t *Buffer, uint8_t *Data);
RB_Status_t RB_Flush(RB_t *Buffer);
RB_Status_t RB_Init(RB_t *Buffer);

#endif // RING_BUFFER_H