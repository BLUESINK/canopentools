{%- set INFO = data['child']['INFO']['child'] -%}
#ifndef __CANOPEN_GLOBAL_H__
#define __CANOPEN_GLOBAL_H__

#include "stdint.h"
#include "stdbool.h"
#include "string.h"

#include "OD.h"
#include "OD_access.h"
#define NrOfRXPDO {{ INFO.DeviceInfo['data'].NrOfRXPDO }}
#define NrOfTXPDO {{ INFO.DeviceInfo['data'].NrOfTXPDO }}

#define NMT     0x00
#define SYNC    0x01
#define EMCY    0x01
#define TIME    0x02

#define txPDO1  0x03
#define rxPDO1  0x04
#define txPDO2  0x05
#define rxPDO2  0x06
#define txPDO3  0x07
#define rxPDO3  0x08
#define txPDO4  0x09
#define rxPDO4  0x0A

#define txSDO   0x0B
#define rxSDO   0x0C

#define HTBT   0x0E

typedef struct CANOPEN_RES{
  uint16_t cob_id;
  uint16_t loop_count;
  uint16_t expire_count;
  uint8_t len;
  uint8_t data[8];
}CANOPEN_RES;

typedef enum CANOPEN_STATE{
  INITIALISING_STATE,
  PRE_OPERATIONAL_STATE,
  OPERATIONAL_STATE,
  STOPPPED_STATE
}CANOPEN_STATE;

typedef struct CANOPEN{
  uint8_t               node_id;
  CANOPEN_RES           res;
  CANOPEN_STATE         state;
  OD_INTERFACE          od_res;
  OD_INTERFACE          od_req;
}CANOPEN;

extern CANOPEN _canopen;

#endif