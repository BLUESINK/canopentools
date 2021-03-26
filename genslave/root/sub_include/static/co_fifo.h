#ifndef __CANOPEN_FIFO_H__
#define __CANOPEN_FIFO_H__

#define CO_FIFO_NUM   10

#include "canopen_global.h"

extern void Canopen_FIFO_Init();
extern bool Canopen_PutTxFIFO(CAN_FRAME* elem);
extern bool Canopen_GetTxFIFO(CAN_FRAME* elem);
extern bool Canopen_PutRxFIFO(CAN_FRAME* elem);
extern bool Canopen_GetRxFIFO(CAN_FRAME* elem);

#endif