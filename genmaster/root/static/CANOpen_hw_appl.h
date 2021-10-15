#ifndef __CANOPEN_HW_APPL_H_
#define __CANOPEN_HW_APPL_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define CO_BUFLEN            40

extern void CANOpen_sendFrame(uint16_t cobID, uint8_t* data, uint8_t len);

#ifdef __cplusplus
}
#endif

#endif