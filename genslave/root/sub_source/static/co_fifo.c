#include "co_fifo.h"

typedef struct{
  uint8_t head;
  uint8_t tail;
  CAN_FRAME fifo[CO_FIFO_NUM];
} queue_t;

bool queue_read(queue_t *queue, CAN_FRAME* elem){

  if(queue->tail == queue->head) return false;

  memcpy(elem, &queue->fifo[queue->tail], sizeof(CAN_FRAME));
  queue->tail = (queue->tail + 1) % CO_FIFO_NUM;
  return true;

}

bool queue_write(queue_t *queue, CAN_FRAME* elem){

  uint8_t next = (queue->head + 1) % CO_FIFO_NUM;
  if(next == queue->tail) return false;

  memcpy(&queue->fifo[queue->head], elem, sizeof(CAN_FRAME));
  queue->head = next;

  return true;
}


queue_t tx_fifo;
queue_t rx_fifo;

void Canopen_FIFO_Init(){
  tx_fifo.head = 0;
  tx_fifo.tail = 0;
  rx_fifo.head = 0;
  rx_fifo.tail = 0;
}

bool Canopen_PutTxFIFO(CAN_FRAME* elem){
  return queue_write(&tx_fifo, elem);
}

bool Canopen_GetTxFIFO(CAN_FRAME* elem){
  return queue_read(&tx_fifo, elem);
}

bool Canopen_PutRxFIFO(CAN_FRAME* elem){
  return queue_write(&rx_fifo, elem);
}

bool Canopen_GetRxFIFO(CAN_FRAME* elem){
  return queue_read(&rx_fifo, elem);
}