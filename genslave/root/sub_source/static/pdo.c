#include "pdo.h"

#include "co_fifo.h"
#include "user_appl.h"

#include "canopen_config.h"

#if NrOfRXPDO > 0
CANOPEN_PDO RPDO[NrOfRXPDO];
#endif
#if NrOfTXPDO > 0
CANOPEN_PDO TPDO[NrOfTXPDO];
#endif

void Canopen_txPDO_Proc(uint8_t channel, uint8_t buffered){

  uint8_t i, j;
  
  uint64_t dummy64, dummy64_2;
  
  uint8_t pdoMappingLength;
  uint16_t pdoMappingIndex;
  uint8_t pdoMappingSubIndex;
  uint8_t pdoMappingBitLength;
  uint8_t pdoMappingByteLength;
  uint8_t pdoMappingBitOffset;
  
  OD_STATE od_state;
  
  // Transmit Mapping Number
  _canopen.od_res.Index = 0x1A00 + channel - 1;
  _canopen.od_res.SubIndex = 0x00;
  od_state = OD_READ(&_canopen.od_res, false); 
  
  if(od_state != OD_OK) return;
  
  pdoMappingLength = _canopen.od_res.data[0];
  
  pdoMappingBitOffset = 0;
  dummy64 = 0;
  
  // Init Buffer
  memset(_canopen.res.data, 0, 8);
  
  for(i = 0; i < pdoMappingLength; i++){

    // Read i-th entry
    _canopen.od_res.Index = 0x1A00 + channel - 1;
    _canopen.od_res.SubIndex = i + 1;
    od_state = OD_READ(&_canopen.od_res, false);
    if(od_state != OD_OK) return;

    pdoMappingIndex = (uint16_t)_canopen.od_res.data[3] << 8 | (uint16_t)_canopen.od_res.data[2];
    pdoMappingSubIndex = _canopen.od_res.data[1];
    pdoMappingBitLength = _canopen.od_res.data[0];
    pdoMappingByteLength = (pdoMappingBitLength - 1) / 8 + 1;
    
    // User Application
    Canopen_Application_BeforeSendData(pdoMappingIndex, pdoMappingSubIndex);
    
    _canopen.od_res.Index = pdoMappingIndex;
    _canopen.od_res.SubIndex = pdoMappingSubIndex;
    od_state = OD_READ(&_canopen.od_res, false);
    if(od_state != OD_OK) return;

    // Mapping Data
    j = pdoMappingByteLength;
    do{
      j --;
      if(j == pdoMappingByteLength - 1){
        switch(pdoMappingBitLength % 8){
        case 0 : dummy64_2 = _canopen.od_res.data[j]; break;
        case 1 : dummy64_2 = _canopen.od_res.data[j] & 0x01; break;
        case 2 : dummy64_2 = _canopen.od_res.data[j] & 0x03; break;
        case 3 : dummy64_2 = _canopen.od_res.data[j] & 0x07; break;
        case 4 : dummy64_2 = _canopen.od_res.data[j] & 0x0F; break;
        case 5 : dummy64_2 = _canopen.od_res.data[j] & 0x1F; break;
        case 6 : dummy64_2 = _canopen.od_res.data[j] & 0x3F; break;
        case 7 : dummy64_2 = _canopen.od_res.data[j] & 0x7F; break;
        }
      }else{
        dummy64_2 = dummy64_2 << 8;
        dummy64_2 |= _canopen.od_res.data[j];
      }
    }while(j != 0);
    
    dummy64 |= dummy64_2 << pdoMappingBitOffset;
    
    pdoMappingBitOffset += pdoMappingBitLength;
    
  }
  
  if(buffered == 1){
    TPDO[channel-1].buffer.cob_id = TPDO[channel-1].cob_id;
    TPDO[channel-1].buffer.len = (pdoMappingBitOffset - 1) / 8 + 1;
    memcpy(TPDO[channel-1].buffer.data, &dummy64, 8);
  }else{
    _canopen.res.cob_id = TPDO[channel-1].cob_id;
    _canopen.res.len = (pdoMappingBitOffset - 1) / 8 + 1;
    memcpy(_canopen.res.data, &dummy64, 8);
    
    Canopen_PutTxFIFO(&_canopen.res);    
  }

  return;
}

void Canopne_rxPDO_Proc(uint8_t* data, uint8_t channel){
  uint8_t i, j;

  uint64_t dummy64;
  uint8_t dummyData[8];
  
  uint8_t pdoMappingLength;
  uint16_t pdoMappingIndex;
  uint8_t pdoMappingSubIndex;
  uint8_t pdoMappingBitLength;
  uint8_t pdoMappingByteLength;
  uint8_t pdoMappingBitOffset;
  
  uint8_t startIndex, endIndex;
  
  OD_STATE od_state;
  
  // 1. Read Mapping Number
  _canopen.od_res.Index = 0x1600 + channel - 1;
  _canopen.od_res.SubIndex = 0x00;
  od_state = OD_READ(&_canopen.od_res, false);
  
  if(od_state != OD_OK) return;
  
  pdoMappingLength = _canopen.od_res.data[0];
  
  pdoMappingBitOffset = 0;
  
  for(i = 0; i < pdoMappingLength; i++){
    _canopen.od_res.Index = 0x1600 + channel - 1;
    _canopen.od_res.SubIndex = i + 1;
    OD_READ(&_canopen.od_res, false);
    pdoMappingIndex = (uint16_t)_canopen.od_res.data[3] << 8 | (uint16_t)_canopen.od_res.data[2];
    pdoMappingSubIndex = _canopen.od_res.data[1];
    pdoMappingBitLength = _canopen.od_res.data[0];
    pdoMappingByteLength = (pdoMappingBitLength - 1) / 8 + 1;
    
    // Init Buffer
    memset(dummyData, 0, 8);

    // Mapping Data
    startIndex = pdoMappingBitOffset / 8;
    endIndex = (pdoMappingBitOffset + pdoMappingBitLength - 1) / 8;
    
    if(startIndex == endIndex){
      dummyData[0] = data[startIndex] >> (pdoMappingBitOffset % 8);
      switch(pdoMappingBitLength){
      case 1 : dummyData[0] &= 0x01; break;
      case 2 : dummyData[0] &= 0x03; break;
      case 3 : dummyData[0] &= 0x07; break;
      case 4 : dummyData[0] &= 0x0F; break;
      case 5 : dummyData[0] &= 0x1F; break;
      case 6 : dummyData[0] &= 0x3F; break;
      case 7 : dummyData[0] &= 0x7F; break;
      }
    }else{
      j = endIndex - startIndex + 1;
      do{
        j --;
        if(j == endIndex - startIndex){
          switch((pdoMappingBitOffset + pdoMappingBitLength) % 8){
          case 0 : dummy64 = data[endIndex]; break;
          case 1 : dummy64 = data[endIndex] & 0x01; break;
          case 2 : dummy64 = data[endIndex] & 0x03; break;
          case 3 : dummy64 = data[endIndex] & 0x07; break;
          case 4 : dummy64 = data[endIndex] & 0x0F; break;
          case 5 : dummy64 = data[endIndex] & 0x1F; break;
          case 6 : dummy64 = data[endIndex] & 0x3F; break;
          case 7 : dummy64 = data[endIndex] & 0x7F; break;
          }
        }else if(j == 0){
          dummy64 = dummy64 << (8 - pdoMappingBitOffset % 8);
          dummy64 |= data[startIndex] >> (pdoMappingBitOffset % 8);
        }else{
          dummy64 = dummy64 << 8;
          dummy64 |= data[startIndex + j];
        }
      }while(j != 0);
      
      for(j = 0; j < pdoMappingByteLength; j++){
        dummyData[j] = dummy64 & 0x000000FF;
        dummy64 = dummy64 >> 8;
      }
    }
    
    // Write PDO Data
    _canopen.od_req.Index = pdoMappingIndex;
    _canopen.od_req.SubIndex = pdoMappingSubIndex;
    _canopen.od_req.BitLength = pdoMappingBitLength;
    memcpy(_canopen.od_req.data, dummyData, pdoMappingByteLength);
    OD_WRITE(&_canopen.od_res, &_canopen.od_req, true);
    
    pdoMappingBitOffset += pdoMappingBitLength;
    
    // End of RPDO Process
    Canopen_Application_AfterGetData(pdoMappingIndex, pdoMappingSubIndex);
  }
  
}

void Canopen_rxPDO(uint8_t* data, uint8_t channel){
  
  if(channel == 0 || channel > NrOfRXPDO) return;
  
  channel --;

#if NrOfRXPDO > 0  
  // Asynchronous PDO is not implemented yet... 
  memcpy(RPDO[channel].PDO_Buffer, data, 8);
  RPDO[channel].new_data = 1;
#endif
  
}