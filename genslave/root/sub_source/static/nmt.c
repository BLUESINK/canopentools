#include "nmt.h"

uint8_t canopen_state;

void Canopen_NMT(uint8_t* data){

  if(data[1] != 0x00 && data[1] != _canopen.node_id) return;
  
  canopen_state = data[0];
  
  switch(data[0]){
  case 0x01: // Switch to "Operational"
    break;
    
  case 0x02: // Switch to "Stop"
    break;
    
  case 0x80: // Switch to "Pre-Operational"
    break;
    
  case 0x81: // Reset Node
    break;
    
  case 0x82: // Reset Communication
    break;
  }
}