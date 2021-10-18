{%- set OD = data['child']['OD']['child'] -%}
#include "OD.h"
#include "OD_def.h"
#include "OD_access.h"

OD_STATE OD_READPROC(OD_INTERFACE* response, bool Init, OD_INDEX* od_index);
OD_STATE OD_WRITEPROC(OD_INTERFACE* response, OD_INTERFACE* request, bool Init, OD_INDEX* od_index);

OD_INDEX* OD_GET_INDEX(uint16_t index){
  uint8_t i;
  for(i = 0; i < ODIndexNum; i++){
    if(ObjDic.IndexList[i] == index){
      return ObjDic.IndexObj[i];
    }
  }
  
  return NULL;  
}

OD_SUBINDEX* OD_GET_SUBINDEX(uint16_t index, uint8_t subindex){
  uint8_t i;
  for(i = 0; i < ODIndexNum; i++){
    if(ObjDic.IndexList[i] == index){
      if(subindex < ObjDic.IndexObj[i]->subIndexLength){
        return &ObjDic.IndexObj[i]->subIndexObj[subindex];
      }
    }
  }
  
  return NULL;  
}

OD_STATE OD_INIT(uint8_t NODEID){

  {%- for _,index in OD.items() %}
  {%- for _,subindex in index['child'].items() %}
  {%- if subindex.data['DefaultValue'] %}
  {%- if subindex.data['DataTypeStr'][2] == 'Basic' %}
  OD_0x{{ index.name }}_{{ subindex.name }} = {{ subindex.data['DefaultValue'] }};
  {%- elif subindex.data['DataTypeStr'][2] == 'Extended' %}
  memcpy(OD_0x{{ index.name }}_{{ subindex.name }}, {{ subindex.data['DefaultValue'] }}, {{ (subindex.data['DataTypeStr'][1]/8)|int }});
  {%- endif %}
  {%- else %}
  OD_0x{{ index.name }}_{{ subindex.name }} = 0;
  {%- endif %}
  {%- endfor %}
  {% endfor %}

  return OD_OK;
}

OD_STATE OD_WRITE(OD_INTERFACE* response, OD_INTERFACE* request, bool Init){
  uint8_t i;
  for(i = 0; i < ODIndexNum; i++){
    if(ObjDic.IndexList[i] == request->Index){
      return OD_WRITEPROC(response, request, Init, ObjDic.IndexObj[i]);
    }
  }
  
  return OD_NOT_EXIST;
}

OD_STATE OD_READ(OD_INTERFACE* response, bool Init){
  
  uint8_t i;
  for(i = 0; i < ODIndexNum; i++){
    if(ObjDic.IndexList[i] == response->Index){
      return OD_READPROC(response, Init, ObjDic.IndexObj[i]);
    }
  }
  
  return OD_NOT_EXIST;
  
}

////////////////////////////////////////////////////////////////////////////////
// Read Process Function
////////////////////////////////////////////////////////////////////////////////
OD_STATE OD_READPROC(OD_INTERFACE* response, bool Init, OD_INDEX* od_index){
  
  uint8_t       subIndex;
  uint16_t       copyByteLength;
  uint32_t      copyBitLength;
  
  subIndex = response->SubIndex;
  
  if(subIndex > od_index->subIndexLength - 1) return OD_SUBINDEX_NOT_EXIST;
  
  response->BitLength = od_index->subIndexObj[subIndex].BitLength;
  
  if(response->BitLength == 0) return OD_SUBINDEX_NOT_EXIST;

  if(!(od_index->subIndexObj[subIndex].ObjAccess & OD_ACCESS_READ)) return OD_WRITE_ONLY;
  
  if(response->BitLength < 33){ // Less than 5 bytes
    copyByteLength = (response->BitLength - 1) / 8 + 1;
    memcpy(response->data, od_index->subIndexObj[subIndex].data, copyByteLength);
    od_index->subIndexObj[subIndex].BitPointer = 0;
    response->n = 7 - copyByteLength;
    response->c = 1;
    return OD_OK;
  }
  
  if(Init){
    od_index->subIndexObj[subIndex].BitPointer = 0;
    return OD_OK;
  }
  
  copyBitLength = od_index->subIndexObj[subIndex].BitLength - od_index->subIndexObj[subIndex].BitPointer;
  if(copyBitLength > 56){ // Larger than 7 bytes
    memcpy(response->data, (uint8_t*)od_index->subIndexObj[subIndex].data + od_index->subIndexObj[subIndex].BitPointer / 8, 7);
    od_index->subIndexObj[subIndex].BitPointer += 56;
    response->n = 0;
    response->c = 0;
  }else{ // Reading Complete
    copyByteLength = (copyBitLength - 1) / 8 + 1;
    memcpy(response->data, (uint8_t*)od_index->subIndexObj[subIndex].data + od_index->subIndexObj[subIndex].BitPointer / 8, copyByteLength);
    od_index->subIndexObj[subIndex].BitPointer = 0;
    response->n = 7 - copyByteLength;
    response->c = 1;
  }

  return OD_OK;
  
}

////////////////////////////////////////////////////////////////////////////////
// Write Process Function
////////////////////////////////////////////////////////////////////////////////
OD_STATE OD_WRITEPROC(OD_INTERFACE* response, OD_INTERFACE* request, bool Init, OD_INDEX* od_index){

  uint8_t       i;
  uint8_t       subIndex;
  uint16_t       copyByteLength;
  uint16_t       copyByteStart;
  
  subIndex = request->SubIndex;
  
  // Check parameters
  if(subIndex > od_index->subIndexLength - 1) return OD_SUBINDEX_NOT_EXIST;
  if(od_index->subIndexObj[subIndex].BitLength == 0) return OD_SUBINDEX_NOT_EXIST;
  if(!(od_index->subIndexObj[subIndex].ObjAccess & OD_ACCESS_WRITE)) return OD_READ_ONLY;
  
  // Do write process
  copyByteLength = (od_index->subIndexObj[subIndex].BitLength - 1) / 8 + 1;
  
  if(copyByteLength < 5 && Init){ // Expedited download
    if(copyByteLength > (request->BitLength / 8)) return OD_PARAM_LEN_SHORT;
    if(copyByteLength < (request->BitLength / 8)) return OD_PARAM_LEN_LONG;

    response->Index = request->Index;
    response->SubIndex = request->SubIndex;
    memcpy(response->data, request->data, copyByteLength);
    
    for(i = 0;i < copyByteLength; i++){
      if(i < copyByteLength){

        if(
          (od_index->subIndexObj[subIndex].ObjAccess & (OD_ACCESS_READ | OD_ACCESS_WRITE)) 
            == (OD_ACCESS_READ | OD_ACCESS_WRITE)){ // Copy data to buffer when RW mode
          *((uint8_t*)od_index->subIndexObj[subIndex].data + (od_index->subIndexObj[subIndex].BitLength-1)/8 + 1 + i) = request->data[i];
        }else{
          *((uint8_t*)od_index->subIndexObj[subIndex].data + i) = request->data[i];
        }

      }else *((uint8_t*)od_index->subIndexObj[subIndex].data + i) = 0x00;
    }  
  }else{ // Segment download
    if(Init){
      if(od_index->subIndexObj[subIndex].BitLength > request->BitLength) return OD_PARAM_LEN_SHORT;
      if(od_index->subIndexObj[subIndex].BitLength < request->BitLength) return OD_PARAM_LEN_LONG;

      od_index->subIndexObj[subIndex].BitPointer = 0;
      return OD_OK;
    }

    copyByteStart = od_index->subIndexObj[subIndex].BitPointer / 8;
    od_index->subIndexObj[subIndex].BitPointer += (7 - request->n) * 8;

    if(od_index->subIndexObj[subIndex].BitPointer > od_index->subIndexObj[subIndex].BitLength){
      od_index->subIndexObj[subIndex].BitPointer = 0;
      return OD_PARAM_LEN_LONG;
    }

    if(
      (od_index->subIndexObj[subIndex].ObjAccess & (OD_ACCESS_READ | OD_ACCESS_WRITE)) 
        == (OD_ACCESS_READ | OD_ACCESS_WRITE)){ // Copy data to buffer when RW mode
      memcpy((uint8_t*)od_index->subIndexObj[subIndex].data + (od_index->subIndexObj[subIndex].BitLength-1)/8 + 1 + copyByteStart, request->data, 7 - request->n);
    }else{
      memcpy((uint8_t*)od_index->subIndexObj[subIndex].data + copyByteStart, request->data, 7 - request->n);
    }

  }
  
  return OD_OK;
}