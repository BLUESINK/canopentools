#ifndef __CANOPEN_USER_APPL_H__
#define __CANOPEN_USER_APPL_H__

#include "canopen_global.h"

extern uint32_t Canopen_Application_BeforeSendData(uint16_t Index, uint8_t subIndex);
extern uint32_t Canopen_Application_AfterGetData(uint16_t Index, uint8_t subIndex);
extern void Canopen_Application_ResetAppl();
extern void Canopen_Application_ResetComm();

#endif