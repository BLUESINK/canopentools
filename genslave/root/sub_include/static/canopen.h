#ifndef __CANOPEN_H__
#define __CANOPEN_H__

#include "canopen_global.h"

/**********************************************************************/
// User Install Functions
/**********************************************************************/
extern void Canopen_Init(uint8_t canID);
extern void Canopen_Loop(uint32_t ctime);

extern CANOPEN_STATE Canopen_GetState();
extern void Canopen_ChangeState(CANOPEN_STATE state);

extern bool Canopen_GetFrame(CAN_FRAME* elem);
extern bool Canopen_PutFrame(CAN_FRAME* elem);

#endif