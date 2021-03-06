{%- set INFO = data['child']['INFO']['child'] -%}
#include "canopen.h"
#include "canopen_config.h"

#include "OD_access.h"
#include "co_fifo.h"
#include "nmt.h"
#include "pdo.h"
#include "sdo.h"
#include "sync.h"
#include "user_appl.h"

CANOPEN _canopen;
#if NrOfRXPDO > 0
extern CANOPEN_PDO RPDO[NrOfRXPDO];
#endif
#if NrOfTXPDO > 0
extern CANOPEN_PDO TPDO[NrOfTXPDO];
#endif

static uint8_t nodeguard_toggle = 0;
static uint32_t last_nodeguard_request = 0;
static uint8_t nodeguard_event_occcur = 0;

void Canopen_Init(){

  Canopen_FIFO_Init();
  OD_INIT(_canopen.node_id);

  // Set RPDO Default Transtype
  {%- set NrOfRXPDO = INFO.DeviceInfo['data'].NrOfRXPDO | int %}
  {%- for n in range(NrOfRXPDO) %}
  {%- set index = 5120 + n %}
  RPDO[{{ n }}].PDO_TransType = OD_0x{{ '%04X' % index }}_02;
  RPDO[{{ n }}].valid = OD_0x{{ '%04X' % index }}_01 & 0x80000000 ? 0 : 1;
  RPDO[{{ n }}].cob_id = OD_0x{{ '%04X' % index }}_01 & 0x000007FF; 
  {%- endfor %}

  // Set TPDO Default Transtype
  {%- set NrOfTXPDO = INFO.DeviceInfo['data'].NrOfTXPDO | int %}
  {%- for n in range(NrOfTXPDO) %}
  {%- set index = 6144 + n %}
  TPDO[{{ n }}].PDO_TransType = OD_0x{{ '%04X' % index }}_02;
  TPDO[{{ n }}].valid = OD_0x{{ '%04X' % index }}_01 & 0x80000000 ? 0 : 1;
  TPDO[{{ n }}].cob_id = OD_0x{{ '%04X' % index }}_01 & 0x000007FF; 
  {%- endfor %}
}

void Canopen_ChangeState(CANOPEN_STATE state){

  switch(state){
    case RESET_APPLICATION_STATE:
      Canopen_Application_Event(APPLICATION_RESET_EVENT);
      _canopen.state = state;
      Canopen_ChangeState(RESET_COMMUNICATION_STATE);
      break;

    case RESET_COMMUNICATION_STATE:
      Canopen_Init();
      nodeguard_event_occcur = 0;
      nodeguard_toggle = 0; // Only NMT reset communication set toggle bit to 0
      Canopen_Application_Event(COMMUNICATION_RESET_EVENT);
      _canopen.state = state;
      Canopen_ChangeState(PRE_OPERATIONAL_STATE);
      break;

    case PRE_OPERATIONAL_STATE:
      // NMT slave produce boot-up protocol after entered pre-op.
      _canopen.state = state;
      CAN_FRAME frame;
      frame.cob_id = 0x700 | _canopen.node_id;
      frame.len = 1;
      frame.data[0] = 0x00;
      Canopen_PutTxFIFO(&frame);
      break;

    case OPERATIONAL_STATE:
      if(_canopen.state == PRE_OPERATIONAL_STATE
      || _canopen.state == STOPPPED_STATE) _canopen.state = state;
      break;

    case STOPPPED_STATE:
      // TODO
      break;

    default:
      break;
  }

}

void Canopen_Start(uint8_t canID){
  _canopen.state = INITIALISING_STATE;
  _canopen.node_id = canID;

  Canopen_ChangeState(RESET_APPLICATION_STATE);
}

void Canopen_Loop(uint32_t ctime){

  CAN_FRAME frame;
  CAN_FRAME nodeguard_frame;

  static uint32_t canopen_looptime = 0;

  canopen_looptime += ctime;

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

#if OD_LIFE_TIME_FACTOR
  if(OD_0x100C_00 != 0 &&
     OD_0x100D_00 != 0 &&
     nodeguard_event_occcur == 0){
    if((canopen_looptime - last_nodeguard_request) > OD_0x100C_00 * OD_0x100D_00 * 1000){
        Canopen_Application_Event(LIFE_GUARDING_EVENT);
        nodeguard_event_occcur = 1;
    }
  }else{
    last_nodeguard_request = canopen_looptime;
  }
#endif

  while(Canopen_GetRxFIFO(&frame)){
    switch(frame.cob_id >> 7){
    case 0x0C: // RX-SDO
      Canopen_rxSDO(frame.data);
      break;
      
    case 0x0E: // Heartbeat Consumer
      if(frame.rtr == 1){
        if(_canopen.state != STOPPPED_STATE &&
           _canopen.state != OPERATIONAL_STATE &&
           _canopen.state != PRE_OPERATIONAL_STATE) break;

        last_nodeguard_request = canopen_looptime;
        nodeguard_event_occcur = 0;

        nodeguard_frame.cob_id = 0x700 | _canopen.node_id;
        nodeguard_frame.len = 1;
        nodeguard_frame.data[0] = _canopen.state;
        if(nodeguard_toggle == 1) nodeguard_frame.data[0] |= 0x80;
        nodeguard_toggle = nodeguard_toggle == 1 ? 0 : 1;

        Canopen_PutTxFIFO(&nodeguard_frame);
      }
      break;

    case 0x00: // NMT
      Canopen_NMT(frame.data);
      break;
      
    case 0x01: // SYNC
      Canopen_SYNC();
      break;
      
    case 0x02: // TIME
      break;

    default: // PDOs
      if(_canopen.state != OPERATIONAL_STATE) break;

#if NrOfRXPDO > 0
    {
      uint8_t i;
      for(i = 0; i < NrOfRXPDO; i++){
        if((frame.cob_id == RPDO[i].cob_id) && RPDO[i].valid){
          Canopen_rxPDO(frame.data, i+1);
          break;
        }
      }
    }
#endif

#if NrOfTXPDO > 0
    {
      uint8_t i;
      if(frame.rtr != 1) break;

      for(i = 0; i < NrOfTXPDO; i++){
        if((frame.cob_id == TPDO[i].cob_id) && TPDO[i].valid && TPDO[i].RTR){

          if(TPDO[i].PDO_TransType == 0xFC){ // RTR-only (synchronous)
            // Send last synchronize PDO frame
            if(TPDO[i].buffer.cob_id == TPDO[i].cob_id){
              Canopen_PutTxFIFO(&TPDO[i].buffer);
              TPDO[i].buffer.cob_id = 0;
            }
          }else if(TPDO[i].PDO_TransType == 0xFD){ // RTR-only (event-driven)
            // Sampling now and send immediately
            Canopen_txPDO_Proc(i+1, 0);
          }else break;

        }
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