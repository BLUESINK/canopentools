#ifndef __CANOPEN_FIFO_H__
#define __CANOPEN_FIFO_H__

#define CO_FIFO_NUM   10

#include "canopen_global.h"

extern void Canopen_FIFO_Loop();
extern bool Canopen_PutFIFO(CAN_FRAME* elem);
extern bool Canopen_GetFIFO(CAN_FRAME* elem);

#endif