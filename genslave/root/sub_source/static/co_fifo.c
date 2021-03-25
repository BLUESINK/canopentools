#include "co_fifo.h"

CAN_FRAME CO_FIFO[CO_FIFO_NUM];
static uint8_t co_fifo_pointerIn; // Empty pointer
static uint8_t co_fifo_pointerOut; // Empty pointer

void Canopen_FIFO_Loop(){

}

bool Canopen_PutFIFO(CAN_FRAME* elem){

  uint8_t next_pointerIn = (co_fifo_pointerIn + 1) % CO_FIFO_NUM;
  
  if(next_pointerIn == co_fifo_pointerOut) return false;
  
  CO_FIFO[co_fifo_pointerIn].cob_id = elem->cob_id;
  CO_FIFO[co_fifo_pointerIn].len = elem->len;
  memcpy(CO_FIFO[co_fifo_pointerIn].data, elem->data, 8);
  
  co_fifo_pointerIn = next_pointerIn;
  
  return true;
}

bool Canopen_GetFIFO(CAN_FRAME* elem){

  if(co_fifo_pointerOut == co_fifo_pointerIn) return false;

  elem->cob_id = CO_FIFO[co_fifo_pointerOut].cob_id;
  elem->len = CO_FIFO[co_fifo_pointerOut].len;
  memcpy(elem->data, CO_FIFO[co_fifo_pointerOut].data, 8);

  co_fifo_pointerOut = (co_fifo_pointerOut + 1) % CO_FIFO_NUM;
  
  return true;
}