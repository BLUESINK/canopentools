#include "canopen_global.h"

#include "nmt.h"
#include "user_appl.h"

extern void Canopen_ChangeState(CANOPEN_STATE state);

void Canopen_NMT(uint8_t* data){

  if(data[1] != 0x00 && data[1] != _canopen.node_id) return;
  
  switch(data[0]){
  case 0x01: // Switch to "Operational"
    Canopen_ChangeState(OPERATIONAL_STATE);
    break;
    
  case 0x02: // Switch to "Stop"
    Canopen_ChangeState(STOPPPED_STATE);
    break;
    
  case 0x80: // Switch to "Pre-Operational"
    Canopen_ChangeState(PRE_OPERATIONAL_STATE);
    break;
    
  case 0x81: // Reset Node
    Canopen_ChangeState(RESET_APPLICATION_STATE);
    break;
    
  case 0x82: // Reset Communication
    Canopen_ChangeState(RESET_COMMUNICATION_STATE);
    break;
  }
}