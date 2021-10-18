#include "co_fifo.h"
#include "canopen_config.h"

/***********************************************************************/
// Canopen frame linked list functions
/***********************************************************************/
typedef struct CO_LIST{
  CAN_FRAME frame;
  struct CO_LIST *next, *prev;
}CO_LIST;

static inline void CANOpen_list_init(CO_LIST *list){
  list->next = list;
  list->prev = list;
}

static inline void CANOpen_list_add(CO_LIST *new,
  CO_LIST *prev,
  CO_LIST *next)
{
  next->prev = new;
  new->next = next;
  new->prev = prev;
  prev->next = new;
}

static inline void CANOpen_list_add_next(CO_LIST *new, CO_LIST *head){
  CANOpen_list_add(new, head, head->next);
}

static inline void CANOpen_list_add_prev(CO_LIST *new, CO_LIST *head){
  CANOpen_list_add(new, head->prev, head);
}

static inline void CANOpen_list_del(CO_LIST *entry){
  entry->next->prev = entry->prev;
  entry->prev->next = entry->next;
}

/***********************************************************************/
// Canopen frame FIFO functions
/***********************************************************************/
static CO_LIST rxFrame_list[CO_FIFO_NUM];
static CO_LIST rxFrame_empty_head;
static CO_LIST rxFrame_filled_head;

static CO_LIST txFrame_list[CO_FIFO_NUM];
static CO_LIST txFrame_empty_head;
static CO_LIST txFrame_filled_head;

void Canopen_FIFO_Init(){
  CANOpen_list_init(&rxFrame_empty_head);
  CANOpen_list_init(&rxFrame_filled_head);

  CANOpen_list_init(&txFrame_empty_head);
  CANOpen_list_init(&txFrame_filled_head);

  int i;
  for(i = 0; i < CO_FIFO_NUM; i++){
    CANOpen_list_add_prev(&rxFrame_list[i], &rxFrame_empty_head);
    CANOpen_list_add_prev(&txFrame_list[i], &txFrame_empty_head);
  }
}

bool Canopen_PutTxFIFO(CAN_FRAME* elem){
  CO_LIST* element = txFrame_empty_head.next;
  
  if(element == &txFrame_empty_head) return false;
  
  memcpy(&element->frame, elem, sizeof(CAN_FRAME));
  CANOpen_list_del(element);
  CANOpen_list_add_prev(element, &txFrame_filled_head);
  return true;
}

bool Canopen_GetTxFIFO(CAN_FRAME* elem){
  CO_LIST* element = txFrame_filled_head.next;

  if(element == &txFrame_filled_head) return false;

  memcpy(elem, &element->frame, sizeof(CAN_FRAME));
  CANOpen_list_del(element);
  CANOpen_list_add_prev(element, &txFrame_empty_head);
  return true;
}

bool Canopen_PutRxFIFO(CAN_FRAME* elem){
  CO_LIST* element = rxFrame_empty_head.next;
  
  if(element == &rxFrame_empty_head) return false;
  
  memcpy(&element->frame, elem, sizeof(CAN_FRAME));
  CANOpen_list_del(element);
  CANOpen_list_add_prev(element, &rxFrame_filled_head);
  return true;
}

bool Canopen_GetRxFIFO(CAN_FRAME* elem){
  CO_LIST* element = rxFrame_filled_head.next;

  if(element == &rxFrame_filled_head) return false;

  memcpy(elem, &element->frame, sizeof(CAN_FRAME));
  CANOpen_list_del(element);
  CANOpen_list_add_prev(element, &rxFrame_empty_head);
  return true;
}