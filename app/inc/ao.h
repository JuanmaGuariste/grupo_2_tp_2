/*
* ao.h
* author: José Luis Krüger
*/

#ifndef ACTIVE_OBJECT_H
#define ACTIVE_OBJECT_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

typedef void* event_data_t;
typedef void (*event_callback_t)(event_data_t event);

typedef struct
{
    int id;
   // ao_led_cb_t free_evt_callback;
    int value;
} ao_led_message_t;

typedef struct {
    QueueHandle_t event_queue;
    event_callback_t process_event;
    uint8_t event_size;
    uint8_t obj_id;
} active_object_t;

typedef struct
{
    active_object_t* hao;
    event_data_t payload;
} ao_event_t;

void active_object_init(active_object_t *obj,
                        event_callback_t process_event,
                        size_t queue_size,
                        uint8_t task_priority);

BaseType_t active_object_send_event(event_data_t event);

void active_object_task(void *pv_parameters);

#endif // ACTIVE_OBJECT_H
