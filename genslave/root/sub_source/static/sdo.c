#include "canopen_config.h"

#include "sdo.h"
#include "co_fifo.h"
#include "OD.h"

#include "user_appl.h"
#include "cia301.h"

static uint32_t abortCode = 0;

uint32_t Canopen_ODstate_to_AbortCode(OD_STATE odState){
  switch(odState){
    case OD_NOT_EXIST:
      return 0x06020000;
    case OD_WRITE_ONLY:
      return 0x06010001;
    case OD_READ_ONLY:
      return 0x06010002;
    case OD_PARAM_LEN_SHORT:
      return 0x06070013;
    case OD_PARAM_LEN_LONG:
      return 0x06070012;
    case OD_SUBINDEX_NOT_EXIST:
      return 0x06090011;
    default:
      return 0;
  }
}

void Canopen_SDOabort(CAN_FRAME* res, uint16_t Index, uint8_t subIndex){
  
  res->data[0] = 0x80;
  res->data[1] = (uint8_t)Index;
  res->data[2] = (uint8_t)(Index >> 8);
  res->data[3] = subIndex;

  memcpy(_canopen.res.data + 4, &abortCode, sizeof(uint32_t));  
}

/******************************************************************************/
// SDO Application Functions
/******************************************************************************/
uint32_t Canopen_SDOapplication_BeforeSendData(uint16_t Index, uint8_t subIndex){
  return Canopen_Application_BeforeSendData(Index, subIndex);
}

uint32_t Canopen_SDOapplication_AfterGetData(uint16_t Index, uint8_t subIndex){

  uint32_t abortCode = 0;
  uint8_t go_next;

  // Pre-check device profiles
  abortCode = cia301_proc(Index, subIndex, &go_next); if(abortCode != 0 || go_next == 0) return abortCode;

  return Canopen_Application_AfterGetData(Index, subIndex);
}

/******************************************************************************/
// SDO Download Functions
/******************************************************************************/
void Canopen_SDOdownload_Expedited(CANOPEN* co, uint8_t* data){ // CiA 301 V4.2.0 (7.2.4.3.3)
  OD_STATE odState;
  
  // Set Object Dictionary parameters
  co->od_req.Index = (uint16_t)data[2] << 8 | (uint16_t)data[1];
  co->od_req.SubIndex = data[3];
  memcpy(co->od_req.data, data + 4, 4);
    
  switch(data[0]){
  case 0x2F: // 1 byte
    co->od_req.BitLength = 8;
    odState = OD_WRITE(&co->od_res, &co->od_req, true);
    break;
    
  case 0x2B: // 2 byte
    co->od_req.BitLength = 16;
    odState = OD_WRITE(&co->od_res, &co->od_req, true);
    break;
    
  case 0x27: // 3 byte
    co->od_req.BitLength = 24;
    odState = OD_WRITE(&co->od_res, &co->od_req, true);
    break;
    
  case 0x23: // 4 byte
    co->od_req.BitLength = 32;
    odState = OD_WRITE(&co->od_res, &co->od_req, true);
    break;
  }

  if(odState == OD_OK){

    // User Application
    abortCode = Canopen_SDOapplication_AfterGetData(co->od_res.Index, co->od_res.SubIndex);
    if(abortCode != 0) return;

    co->res.data[0] = 0x60;
    co->res.data[1] = data[1];
    co->res.data[2] = data[2];
    co->res.data[3] = data[3];
    co->res.data[4] = 0x00;
    co->res.data[5] = 0x00;
    co->res.data[6] = 0x00;
    co->res.data[7] = 0x00;
  }

  
  abortCode = Canopen_ODstate_to_AbortCode(odState);
  
}

void Canopen_SDOdownload_Initiate(CANOPEN* co, uint8_t* data){ // CiA 301 V4.2.0 (7.2.4.3.3)
  OD_STATE odState;
  
  // Set Object Dictionary parameters
  co->od_req.Index = (uint16_t)data[2] << 8 | (uint16_t)data[1];
  co->od_req.SubIndex = data[3];
  memcpy(co->od_req.data, data + 4, 4);
  
  uint32_t byteLength = (uint32_t)co->od_req.data[3] << 8 | (uint32_t)co->od_req.data[2];
  byteLength = byteLength << 8 | (uint32_t)co->od_req.data[1];
  byteLength = byteLength << 8 | (uint32_t)co->od_req.data[0];
  co->od_req.BitLength = byteLength * 8;
  
  odState = OD_WRITE(&co->od_res, &co->od_req, true);
  
  if(odState == OD_OK){
    co->res.data[0] = 0x60;
    co->res.data[1] = data[1];
    co->res.data[2] = data[2];
    co->res.data[3] = data[3];
    co->res.data[4] = 0x00;
    co->res.data[5] = 0x00;
    co->res.data[6] = 0x00;
    co->res.data[7] = 0x00;
  }
  
  abortCode = Canopen_ODstate_to_AbortCode(odState);
}

void Canopen_SDOdownload_Segment(CANOPEN* co, uint8_t* data, uint8_t toggle){ // CiA 301 V4.2.0 (7.2.4.3.4)
  OD_STATE odState;
  
  co->od_req.n = (data[0] & 0x0E) >> 1;
  co->od_req.c = data[0] & 0x01;
  memcpy(co->od_req.data, data + 1, 7);
  
  odState = OD_WRITE(&co->od_res, &co->od_req, false);
  
  if(odState == OD_OK){
    
    if(co->od_req.c){
      abortCode = Canopen_SDOapplication_AfterGetData(co->od_res.Index, co->od_res.SubIndex); // User Application
      if(abortCode != 0) return;
    }
    
    co->res.data[0] = 0x20 | (toggle << 4);
    co->res.data[1] = 0x00;
    co->res.data[2] = 0x00;
    co->res.data[3] = 0x00;
    co->res.data[4] = 0x00;
    co->res.data[5] = 0x00;
    co->res.data[6] = 0x00;
    co->res.data[7] = 0x00;
  }
  
  abortCode = Canopen_ODstate_to_AbortCode(odState);
}
/******************************************************************************/
// SDO Upload Functions
/******************************************************************************/
void Canopen_SDOupload_Expedited(CANOPEN* co){ // CiA 301 V4.2.0 (7.2.4.3.6)

  switch(7 - co->od_res.n){
      case 1: co->res.data[0] = 0x4F; break;
      case 2: co->res.data[0] = 0x4B; break;
      case 3: co->res.data[0] = 0x47; break;
      case 4: co->res.data[0] = 0x43; break;
  }
  
  co->res.data[1] = (uint8_t)co->od_res.Index;
  co->res.data[2] = (uint8_t)(co->od_res.Index >> 8);
  co->res.data[3] = co->od_res.SubIndex;
  
  uint8_t i;
  for(i = 0; i < 4; i ++){
    if(i < 7 - co->od_res.n) co->res.data[4+i] = co->od_res.data[i];
    else co->res.data[4+i] = 0x00;
  }
  
}

void Canopen_SDOupload_Normal_Initiate(CANOPEN* co){ // CiA 301 V4.2.0 (7.2.4.3.6)

  co->res.data[0] = 0x41;
  co->res.data[1] = (uint8_t)co->od_res.Index;
  co->res.data[2] = (uint8_t)(co->od_res.Index >> 8);
  co->res.data[3] = co->od_res.SubIndex;
  
  uint16_t byteLength = (co->od_res.BitLength - 1) / 8 + 1;
  co->res.data[4] = (uint8_t)byteLength;
  co->res.data[5] = (uint8_t)(byteLength >> 8);
  co->res.data[6] = (uint8_t)(byteLength >> 16);
  co->res.data[7] = (uint8_t)(byteLength >> 24);  
  
}

void Canopen_SDOupload_Segment(CANOPEN* co, uint8_t toggle_bit){ // CiA 301 V4.2.0 (7.2.4.3.7)
  co->res.data[0]  = (toggle_bit == 1) ? 0x10 : 0x00;
  co->res.data[0] |= (co->od_res.n << 1) | co->od_res.c;
  
  memcpy(co->res.data+1, co->od_res.data, 7 - co->od_res.n);
  
}

/******************************************************************************/
// SDO Main Function
/******************************************************************************/
void Canopen_rxSDO(uint8_t* data){
  
  _canopen.res.cob_id = (uint16_t)0x580 | (uint16_t)_canopen.node_id;
  _canopen.res.len = 8;
  
  uint8_t ccs = (data[0] & 0xE0) >> 5;
  switch(ccs){
  case 0 : // Download segment request
    
    // [[ Protocol SDO download segment | CiA 301 V4.2.0 (7.2.4.3.4) ]]
    Canopen_SDOdownload_Segment(&_canopen, data, (data[0] & 0x10) >> 4);
    if(abortCode != 0) break;
    
    // Response
    Canopen_PutTxFIFO(&_canopen.res);
    return;
    
  case 1 : // Initiate download request
    
    // [[ Protocol SDO download initiate | CiA 301 V4.2.0 (7.2.4.3.3) ]]
  
    // Check transfer type
    if(data[0] & 0x02) Canopen_SDOdownload_Expedited(&_canopen, data); // Expedited SDO Transfer
    else Canopen_SDOdownload_Initiate(&_canopen, data); // Normal Transfer
    if(abortCode != 0) break;

    // Response
    Canopen_PutTxFIFO(&_canopen.res);
    return;
    
  case 2 : // Initiate upload request
    
    // [[ Protocol SDO upload initiate | CiA 301 V4.2.0 (7.2.4.3.6) ]]
    // 1. Read Object Dictionary
    _canopen.od_res.Index = (uint16_t)data[2] << 8 | (uint16_t)data[1];
    _canopen.od_res.SubIndex = data[3];
    
    abortCode = Canopen_SDOapplication_BeforeSendData(_canopen.od_res.Index, _canopen.od_res.SubIndex);
    if(abortCode != 0) break;
  
    abortCode = Canopen_ODstate_to_AbortCode(OD_READ(&_canopen.od_res, true));
    if(abortCode != 0) break;
      
    // 2. Response
    if(_canopen.od_res.BitLength < 33) Canopen_SDOupload_Expedited(&_canopen); // Expedited SDO Upload
    else Canopen_SDOupload_Normal_Initiate(&_canopen); // Normal SDO Upload Init
    if(abortCode != 0) break;
    Canopen_PutTxFIFO(&_canopen.res);
    return; 
    
  case 3 : // Upload segment request

    // [[ Protocol SDO upload segment (t = 0) | CiA 301 V4.2.0 (7.2.4.3.6) ]]
    // 1. Read Object Dictionary
    abortCode = Canopen_ODstate_to_AbortCode(OD_READ(&_canopen.od_res, false));
    if(abortCode != 0) break;
    
    // 2. Response
    Canopen_SDOupload_Segment(&_canopen, (data[0] & 0x10) >> 4);
    Canopen_PutTxFIFO(&_canopen.res);
    return;
    
  case 5 : // Block Upload
    break;
    
  case 6 : // Block download
    break;
  }
  
  // Error Code
  Canopen_SDOabort(&_canopen.res, _canopen.od_res.Index, _canopen.od_res.SubIndex);
  
  Canopen_PutTxFIFO(&_canopen.res);
}