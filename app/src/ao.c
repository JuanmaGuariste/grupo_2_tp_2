/*
* ao.c
* author: José Luis Krüger
*/

#include "ao.h"
#include "task_button.h"
#include "logger.h"
#define ONLY_THREAD

QueueHandle_t _ao_queue;

// constantes definidas para facilitar el debugging del programa
static char *QUEUE_ID_1 = "Queue_id_1";
static char *QUEUE_ID_2 = "Queue_id_2";
static char *QUEUE_ID_3 = "Queue_id_3";
static char *QUEUE_ID_4 = "Queue_id_4";
static char *QUEUE_ID_DEF = "Queue_id_default";

 char *get_queue_name( uint8_t id) {
     char *buffer[4] = { 0 };
     if (id > 4) return QUEUE_ID_DEF;

     buffer[0] = QUEUE_ID_1;
     buffer[1] = QUEUE_ID_2;
     buffer[2] = QUEUE_ID_3;
     buffer[3] = QUEUE_ID_4;

    return buffer[id - 1];
}

void active_object_init(active_object_t *obj,
                        event_callback_t process_event,
                        size_t queue_size,
                        uint8_t task_priority) {

#ifdef ONLY_THREAD
    static bool init = false;
    if (init) return;
#endif

    obj->event_size = sizeof(event_data_t);
    obj->event_queue = xQueueCreate(queue_size, obj->event_size);

    configASSERT(NULL != obj->event_queue);

    vQueueAddToRegistry(obj->event_queue, get_queue_name(obj->obj_id));
    obj->process_event = process_event;
    _ao_queue = obj->event_queue;

    while (NULL == obj->event_queue) {
        // error
    }

    BaseType_t status;
    status = xTaskCreate(active_object_task, "Task", configMINIMAL_STACK_SIZE, obj, task_priority, NULL);
    configASSERT(pdPASS == status);

#ifdef ONLY_THREAD
    init = true;
#endif
}

void active_object_send_event(event_data_t event) {
    ao_event_t *evt = (ao_event_t*)event;
    xQueueSend(evt->hao->event_queue, &evt->payload, 0);
	LOGGER_INFO("Sent payload event to Active Object.\n");
}

void active_object_task(void *pv_parameters) {
    active_object_t *obj = (active_object_t *) pv_parameters;
    event_data_t payload = NULL;

    for (;;) {
        if (xQueueReceive(obj->event_queue, &payload, portMAX_DELAY) == pdTRUE)
        	LOGGER_INFO("AO task: got payload from event queue.\n");
            obj->process_event(payload);
    }
}
