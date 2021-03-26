#ifndef __CANOPEN_PDO_H__
#define __CANOPEN_ODO_H__

#include "stdint.h"

typedef struct{
  uint8_t PDO_Buffer[8];
  uint8_t PDO_SYNC_Cnt;
  uint8_t PDO_TransType;
  uint8_t valid;
  uint8_t new_data;
}CANOPEN_PDO;

extern void Canopen_txPDO_Proc(uint8_t channel);
extern void Canopne_rxPDO_Proc(uint8_t* data, uint8_t channel);

extern void Canopen_rxPDO(uint8_t* data, uint8_t channel);

#endif