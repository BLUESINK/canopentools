#include "canopen_global.h"

#include "nmt.h"
#include "user_appl.h"

void Canopen_NMT(uint8_t* data){

  if(data[1] != 0x00 && data[1] != _canopen.node_id) return;
  
  switch(data[0]){
  case 0x01: // Switch to "Operational"
    if(_canopen.state == PRE_OPERATIONAL_STATE) _canopen.state = OPERATIONAL_STATE;
    break;
    
  case 0x02: // Switch to "Stop"
    // TODO
    break;
    
  case 0x80: // Switch to "Pre-Operational"
    if(_canopen.state == OPERATIONAL_STATE) _canopen.state = PRE_OPERATIONAL_STATE;
    break;
    
  case 0x81: // Reset Node
    Canopen_Application_ChangeState(_canopen.state, RESET_APPLICATION_STATE);
    break;
    
  case 0x82: // Reset Communication
    Canopen_Application_ChangeState(_canopen.state, RESET_COMMUNICATION_STATE);
    break;
  }
}