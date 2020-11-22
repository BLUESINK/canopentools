#ifndef __CANOPEN_PDO_APPL_H__
#define __CANOPEN_PDO_APPL_H__

#include "stdint.h"

extern void Canopen_PDOapplication_BeforeSendData(uint16_t Index, uint8_t subIndex);
extern void Canopen_PDOapplication_AfterGetData(uint16_t Index, uint8_t subIndex);

#endif