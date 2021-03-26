#include "canopen_config.h"

#include "cia301.h"
#include "OD_access.h"
#include "pdo.h"

#if NrOfRXPDO > 0
extern CANOPEN_PDO RPDO[NrOfRXPDO];
#endif
#if NrOfTXPDO > 0
extern CANOPEN_PDO TPDO[NrOfTXPDO];
#endif

uint32_t cia301_subIndex_mapping(OD_SUBINDEX* subInstance, uint8_t is_RPDO){
  uint32_t* mapped_data32;
  uint16_t mappingIndex;
  uint8_t mappingSubIndex;

  mapped_data32 = (uint32_t*)subInstance->data;
  mappingIndex = (uint16_t)(mapped_data32[1] >> 16);
  mappingSubIndex = (uint8_t)(mapped_data32[1] >> 8 & 0xFF);
  subInstance = OD_GET_SUBINDEX(mappingIndex, mappingSubIndex);

  if(subInstance == NULL) return 0x06020000; // Not exist
  if((subInstance->ObjAccess & OD_ACCESS_MAPPABLE) != OD_ACCESS_MAPPABLE) return 0x06040041; // Not mappable
  if(is_RPDO){
    if((subInstance->ObjAccess & OD_ACCESS_WRITE) != OD_ACCESS_WRITE) return 0x06040041; // Not mappable
  }else{
    if((subInstance->ObjAccess & OD_ACCESS_READ) != OD_ACCESS_READ) return 0x06040041; // Not mappable
  }
  if((uint8_t)(mapped_data32[1] & 0xFF) != subInstance->BitLength) return 0x06070010; // Bit not match

  mapped_data32[0] = mapped_data32[1];
  return 0;
}

uint32_t cia301_check_mappinglength(uint16_t Index){
  OD_SUBINDEX* subInstance;
  uint16_t mappingIndex;
  uint8_t mappingSubIndex;
  uint8_t mappingBitCount = 0;
  uint8_t i;
  
  uint32_t* mapped_data32;
  uint8_t* mapped_data8;

  subInstance = OD_GET_SUBINDEX(Index, 0);
  mapped_data8 = (uint8_t*)subInstance->data;
  
  for(i = 0; i < mapped_data8[1]; i++){
    subInstance = OD_GET_SUBINDEX(Index, i + 1);
    mapped_data32 = (uint32_t*)subInstance->data;
    mappingIndex = (uint16_t)(mapped_data32[0] >> 16);
    mappingSubIndex = (uint8_t)(mapped_data32[0] >> 8 & 0xFF);
    
    subInstance = OD_GET_SUBINDEX(mappingIndex, mappingSubIndex);
    if(subInstance == NULL) return 0x06020000; // Not exist
    if((uint8_t)(mapped_data32[0] & 0xFF) != subInstance->BitLength) return 0x06070010; // Bit not match
    
    mappingBitCount += (uint8_t)(mapped_data32[0] & 0xFF);
  }

  if(mappingBitCount > 64) return 0x06040042; // Length of mapped would be exceed

  mapped_data8[0] = mapped_data8[1];
  return 0;
}

uint32_t cia301_proc(uint16_t Index, uint8_t subIndex, uint8_t* go_next){

  *go_next = 1;

  OD_INDEX* Instance = OD_GET_INDEX(Index); if(Instance == NULL) return 0x06020000;
  OD_SUBINDEX* subInstance = OD_GET_SUBINDEX(Index, subIndex); if(subInstance == NULL) return 0x06020000;
  uint32_t* mapped_data32;
  uint8_t* mapped_data8;

#if NrOfRXPDO > 0
  if(Index >= 0x1400 && Index <= 0x15FF){ // [[ 0x1400 - 0x15FF : Check RPDO communication parameter ]]

    switch(subIndex){
    case 0x01: // COB-ID used by RPDO
      mapped_data32 = (uint32_t*)subInstance->data;
      if((mapped_data32[0] ^ mapped_data32[1]) == 0x80000000){  // Only Valid bit change
        mapped_data32[0] = mapped_data32[1];
        RPDO[Index - 0x1400].valid = mapped_data32[0] & 0x80000000 ? 1 : 0;
        *go_next = 0;
        return 0;
      }else return 0x06090030;

    case 0x02: // Transmission Type
      mapped_data32 = (uint32_t*)subInstance->data;
      if(mapped_data32[1] <= 0xF0){ // Synchronous
        mapped_data32[0] = mapped_data32[1];
        RPDO[Index - 0x1400].PDO_TransType = mapped_data32[0];
        *go_next = 0;
        return 0;
      }else{
        // TODO : Asynchronous
        return 0;
      }

    }

  }else if(Index >= 0x1600 && Index <= 0x17FF){ // [[ 0x1600 - 0x17FF : Check RPDO mapping parameter ]]
    if(!RPDO[Index - 0x1600].valid) return 0x06010000; 

    switch(subIndex){

      case 0x00: // Number of mapped application objects in PDO
        mapped_data8 = (uint8_t*)subInstance->data;
        if(mapped_data8[1] > Instance->subIndexLength- 1) return 0x06090031; // Mapping number too high

        *go_next = 0;
        return cia301_check_mappinglength(Index);

      default: // Sub-index mapping
        *go_next = 0;
        return cia301_subIndex_mapping(subInstance, 1);

    }
  }
#endif

#if NrOfTXPDO > 0  
  if(Index >= 0x1800 && Index <= 0x19FF){ // [[ 0x1800 - 0x19FF : Check TPDO communication parameter ]]
    uint32_t check_bit;
    switch(subIndex){

      case 0x01: // COB-ID used by TPDO
        mapped_data32 = (uint32_t*)subInstance->data;
        check_bit = mapped_data32[0] ^ mapped_data32[1];
        if(check_bit == 0x80000000){  // Only Valid bit change
          mapped_data32[0] = mapped_data32[1];
          TPDO[Index - 0x1800].valid = mapped_data32[0] & 0x80000000 ? 1 : 0;
          *go_next = 0;
          return 0;
        }else if(check_bit == 0){ // Not change
          *go_next = 0;
          return 0;
        }else return 0x06090030;  

      case 0x02: // Transmission Type  
        mapped_data8 = (uint8_t*)subInstance->data;
        if(mapped_data8[1] <= 0xF0){ // Synchronous
          mapped_data8[0] = mapped_data8[1];
          TPDO[Index - 0x1800].PDO_TransType = mapped_data8[0];
          *go_next = 0;
          return 0;
        }else{
          // TODO : Asynchronous
          return 0;
        }

    }
  }else if(Index >= 0x1A00 && Index <= 0x1BFF){ // [[ 0x1A00 - 0x1BFF : Check TPDO mapping parameter ]]
    if(!TPDO[Index - 0x1A00].valid) return 0x06010000; 
    switch(subIndex){

      case 0x00: // Number of mapped application objects in PDO
        mapped_data8 = (uint8_t*)subInstance->data;
        if(mapped_data8[1] > Instance->subIndexLength- 1) return 0x06090031; // Mapping number too high

        *go_next = 0;
        return cia301_check_mappinglength(Index);

      default: // Sub-index mapping
        *go_next = 0;
        return cia301_subIndex_mapping(subInstance, 0);
    }
  }
#endif

#if HEARTBEAT
  if(Index == 0x1017 && subIndex == 0x00){
    OD_0x1017_00 = OD_0x1017_00_buf;
    *go_next = 0;
  }
#endif

  return 0;
}