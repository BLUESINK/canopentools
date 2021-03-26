#ifndef __CANOPEN_USER_APPL_H__
#define __CANOPEN_USER_APPL_H__

#include "stdint.h"

extern uint32_t Canopen_Application_BeforeSendData(uint16_t Index, uint8_t subIndex);
extern uint32_t Canopen_Application_AfterGetData(uint16_t Index, uint8_t subIndex);

#endif