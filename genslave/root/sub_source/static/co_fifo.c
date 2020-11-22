#include "co_fifo.h"

CANOPEN_RES CO_FIFO[CO_FIFO_NUM];
static uint8_t co_fifo_pointerIn; // Empty pointer
static uint8_t co_fifo_pointerOut; // Empty pointer

void Canopen_FIFO_Loop(){
  uint8_t pointer;

  if(co_fifo_pointerOut == co_fifo_pointerIn) return; // FIFO empty

  pointer = co_fifo_pointerOut; // Start record
  do{
    if(CO_FIFO[pointer].expire_count != 0){
      CO_FIFO[pointer].loop_count ++;
      if(CO_FIFO[pointer].loop_count == CO_FIFO[pointer].expire_count) CO_FIFO[pointer].loop_count = CO_FIFO[pointer].expire_count;
    }

    pointer = (pointer + 1) % CO_FIFO_NUM;
  }while(pointer != co_fifo_pointerIn);

}

bool Canopen_PutFIFO(CANOPEN_RES* elem){

  uint8_t next_pointerIn = (co_fifo_pointerIn + 1) % CO_FIFO_NUM;
  
  if(next_pointerIn == co_fifo_pointerOut) return false;
  
  CO_FIFO[co_fifo_pointerIn].cob_id = elem->cob_id;
  CO_FIFO[co_fifo_pointerIn].len = elem->len;
  memcpy(CO_FIFO[co_fifo_pointerIn].data, elem->data, 8);
  
  co_fifo_pointerIn = next_pointerIn;
  
  return true;
}

bool Canopen_GetFIFO(CANOPEN_RES* elem){

  if(co_fifo_pointerOut == co_fifo_pointerIn) return false;

  elem->cob_id = CO_FIFO[co_fifo_pointerOut].cob_id;
  elem->len = CO_FIFO[co_fifo_pointerOut].len;
  memcpy(elem->data, CO_FIFO[co_fifo_pointerOut].data, 8);

  co_fifo_pointerOut = (co_fifo_pointerOut + 1) % CO_FIFO_NUM;
  
  return true;
}