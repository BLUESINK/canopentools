#include "canopen_config.h"

#include "sync.h"
#include "pdo.h"

#if NrOfRXPDO > 0
extern CANOPEN_PDO RPDO[NrOfRXPDO];
#endif
#if NrOfTXPDO > 0
extern CANOPEN_PDO TPDO[NrOfTXPDO];
#endif

void Canopen_SYNC(){

  uint8_t i;
#if NrOfRXPDO > 0  
  // RPDO Process
  for(i = 0; i < NrOfRXPDO; i++){
    if(!RPDO[i].valid && RPDO[i].new_data == 1){
      RPDO[i].PDO_SYNC_Cnt ++;
      if(RPDO[i].PDO_TransType < 0xF1){ // Synchronous
        if(RPDO[i].PDO_SYNC_Cnt >= RPDO[i].PDO_TransType){
          RPDO[i].new_data = 0;
          Canopne_rxPDO_Proc(RPDO[i].PDO_Buffer, i+1);
          RPDO[i].PDO_SYNC_Cnt = 0;
        }
      }      
    }
  }
#endif

#if NrOfTXPDO > 0
  // TPDO Process
  for(i = 0; i < NrOfTXPDO; i++){
    if(!TPDO[i].valid){
      TPDO[i].PDO_SYNC_Cnt ++;
      if(TPDO[i].PDO_TransType < 0xF1){ // Synchronous
        if(TPDO[i].PDO_SYNC_Cnt >= TPDO[i].PDO_TransType){
          Canopen_txPDO_Proc(i+1);
          TPDO[i].PDO_SYNC_Cnt = 0;
        }
      }else if(TPDO[i].PDO_TransType == 0xFC){ // RTR-only (Synchronous)
        // TODO...
      }
    }
  }
#endif
  
}