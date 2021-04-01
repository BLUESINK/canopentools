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

CANOPEN_STATE Canopen_GetState(){
  return _canopen.state;
}

void Canopen_ChangeState(CANOPEN_STATE state){
  _canopen.state = state;

  // NMT slave produce boot-up protocol after entered pre-op.
  if(state == PRE_OPERATIONAL_STATE){
    CAN_FRAME frame;
    frame.cob_id = 0x700 | _canopen.node_id;
    frame.len = 1;
    frame.data[0] = 0x00;
    Canopen_PutTxFIFO(&frame);
  }
}

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
  RPDO[{{ n }}].cob_id = OD_0x{{ '%04X' % index }}_01 & 0x000007FF; 
  {%- endfor %}

  // Set TPDO Default Transtype
  {%- set NrOfTXPDO = INFO.DeviceInfo['data'].NrOfTXPDO | int %}
  {%- for n in range(NrOfTXPDO) %}
  {%- set index = 6144 + n %}
  TPDO[{{ n }}].PDO_TransType = OD_0x{{ '%04X' % index }}_02;
  TPDO[{{ n }}].valid = OD_0x{{ '%04X' % index }}_01 & 0x80000000 ? 1 : 0;
  TPDO[{{ n }}].cob_id = OD_0x{{ '%04X' % index }}_01 & 0x000007FF; 
  {%- endfor %}
}

void Canopen_Loop(uint32_t ctime){

  CAN_FRAME frame;
  uint8_t i;

#if HEARTBEAT
  // Heartbeat Producer
  static uint32_t heartbeat_counter = 0;
  static CAN_FRAME heartbeat_frame;

  if(OD_0x1017_00 != 0){
    heartbeat_counter += ctime;
    if(heartbeat_counter >= OD_0x1017_00 * 1000){
      heartbeat_counter = 0;
      heartbeat_frame.cob_id = 0x700 | _canopen.node_id;
      heartbeat_frame.len = 1;
      heartbeat_frame.data[0] = _canopen.state < 4 ? 0 : _canopen.state;
      Canopen_PutTxFIFO(&heartbeat_frame);
    }
  }
#endif

  while(Canopen_GetRxFIFO(&frame)){
    switch(frame.cob_id >> 7){
    case 0x0C: // RX-SDO
      Canopen_rxSDO(frame.data);
      break;
      
    case 0x0E: // Heartbeat Consumer
      break;

    case 0x00: // NMT
      Canopen_NMT(frame.data);
      break;
      
    case 0x01: // SYNC
      Canopen_SYNC();
      break;
      
    case 0x02: // TIME
      break;

    default:
#if NrOfRXPDO > 0
      if(_canopen.state != OPERATIONAL_STATE) break;

      for(i = 0; i < NrOfRXPDO; i++){
        if((frame.cob_id == RPDO[i].cob_id) && RPDO[i].valid){
          Canopen_rxPDO(frame.data, i+1);
          break;
        }
      }
#endif
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