{%- set INFO = data['child']['INFO']['child'] -%}
#include "canopen.h"
#include "canopen_config.h"

#include "OD_access.h"
#include "co_fifo.h"
#include "nmt.h"
#include "pdo.h"
#include "sdo.h"
#include "sync.h"

CANOPEN _canopen;
#if NrOfRXPDO > 0
extern CANOPEN_PDO RPDO[NrOfRXPDO];
#endif
#if NrOfTXPDO > 0
extern CANOPEN_PDO TPDO[NrOfTXPDO];
#endif

void Canopen_Init(uint8_t canID){
  _canopen.node_id = canID;
  _canopen.state = INITIALISING_STATE;

  Canopen_FIFO_Init();
  OD_INIT(canID);

  // Set RPDO Default Transtype
  {%- set NrOfRXPDO = INFO.DeviceInfo['data'].NrOfRXPDO | int %}
  {%- for n in range(NrOfRXPDO) %}
  {%- set index = 5120 + n %}
  RPDO[{{ n }}].PDO_TransType = OD_0x{{ '%04X' % index }}_02;
  RPDO[{{ n }}].valid = OD_0x{{ '%04X' % index }}_01 & 0x80000000 ? 1 : 0;
  {%- endfor %}

  // Set TPDO Default Transtype
  {%- set NrOfTXPDO = INFO.DeviceInfo['data'].NrOfTXPDO | int %}
  {%- for n in range(NrOfTXPDO) %}
  {%- set index = 6144 + n %}
  TPDO[{{ n }}].PDO_TransType = OD_0x{{ '%04X' % index }}_02;
  TPDO[{{ n }}].valid = OD_0x{{ '%04X' % index }}_01 & 0x80000000 ? 1 : 0;
  {%- endfor %}

  _canopen.state = PRE_OPERATIONAL_STATE;
}

void Canopen_Loop(){

  CAN_FRAME frame;

  while(Canopen_GetRxFIFO(&frame)){
    switch(frame.cob_id >> 7){
    case rxPDO1:
      Canopen_rxPDO(frame.data, 1);
      break;

    case rxPDO2:
      Canopen_rxPDO(frame.data, 2);
      break;

    case rxPDO3:
      Canopen_rxPDO(frame.data, 3);
      break;

    case rxPDO4:
      Canopen_rxPDO(frame.data, 4);
      break;

    case rxSDO:
      Canopen_rxSDO(frame.data);
      break;
      
    case HTBT:
      break;

    case NMT:
      Canopen_NMT(frame.data);
      break;
      
    case SYNC:
      Canopen_SYNC();
      break;
      
    case TIME:
      break;
    }
  }
  
}

bool Canopen_GetFrame(CAN_FRAME* elem){
  return Canopen_GetTxFIFO(elem);
}

bool Canopen_PutFrame(CAN_FRAME* elem){
  return Canopen_PutRxFIFO(elem);
}