#ifndef __CANOPEN_GLOBAL_H__
#define __CANOPEN_GLOBAL_H__

#include "stdint.h"
#include "stdbool.h"
#include "string.h"

#include "OD.h"

typedef struct{
  uint16_t cob_id;
  uint8_t len;
  uint8_t data[8];
}CAN_FRAME;

typedef enum{
  INITIALISING_STATE,
  RESET_APPLICATION_STATE,
  RESET_COMMUNICATION_STATE,
  PRE_OPERATIONAL_STATE,
  OPERATIONAL_STATE,
  STOPPPED_STATE
}CANOPEN_STATE;

typedef struct{
  uint8_t               node_id;
  CAN_FRAME             res;
  CANOPEN_STATE         state;
  OD_INTERFACE          od_res;
  OD_INTERFACE          od_req;
}CANOPEN;

extern CANOPEN _canopen;

#endif