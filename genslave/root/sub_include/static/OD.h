#ifndef __CANOPEN_OD_H__
#define __CANOPEN_OD_H__

#include "stdint.h"
#include "stdbool.h"
#include "string.h"

#define OD_ACCESS_READ          0x01
#define OD_ACCESS_WRITE         0x02
#define OD_ACCESS_MAPPABLE      0x04

typedef enum OD_STATE{
  OD_OK,
  OD_NOT_EXIST,
  OD_WRITE_ONLY,        // 0x06010001
  OD_READ_ONLY,         // 0x06010002
  OD_PARAM_LEN_SHORT,   // 0x06070013
  OD_PARAM_LEN_LONG,    // 0x06070012
  OD_SUBINDEX_NOT_EXIST,// 0x06090011
}OD_STATE;

typedef struct OD_SUBINDEX{
  uint16_t       BitLength;
  uint8_t       ObjAccess;
  uint16_t       BitPointer;
  void*         data;
}OD_SUBINDEX;

typedef struct OD_INDEX{
  uint8_t      subIndexLength;
  OD_SUBINDEX* subIndexObj;
}OD_INDEX;

typedef struct OD{
  uint32_t*     IndexList;
  OD_INDEX**    IndexObj;
}OD;

typedef struct OD_INTERFACE{
  uint16_t Index;
  uint8_t SubIndex;
  uint64_t BitLength;
  uint8_t data[7];
  uint8_t n, c;
}OD_INTERFACE;

extern OD_STATE OD_INIT(uint8_t NODEID);
extern OD_STATE OD_WRITE(OD_INTERFACE* response, OD_INTERFACE* request, bool Init);
extern OD_STATE OD_READ(OD_INTERFACE* response, bool Init);

extern OD_INDEX* OD_GET_INDEX(uint16_t index);
extern OD_SUBINDEX* OD_GET_SUBINDEX(uint16_t index, uint8_t subindex);
#endif