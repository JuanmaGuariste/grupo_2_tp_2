/*
* ao.c
* author: José Luis Krüger
*/

#include "ao.h"
#include "task_button.h"
#include "logger.h"
#define ONLY_THREAD

// QueueHandle_t _ao_queue; // NOTE: commenting for now

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

    obj->event_size = sizeof(event_data_t);
    obj->event_queue = xQueueCreate(queue_size, obj->event_size);
    configASSERT(NULL != obj->event_queue);
    vQueueAddToRegistry(obj->event_queue, get_queue_name(obj->obj_id));
    obj->process_event = process_event;

    // ao_queue = obj->event_queue;

    while (NULL == obj->event_queue) {
        // error
    }

	LOGGER_INFO("Created Active Object.\n");

#ifdef ONLY_THREAD
    static bool init = false;
    if (init) {
    	return;
    }
#endif

    BaseType_t status;
    status = xTaskCreate(active_object_task, "Task", configMINIMAL_STACK_SIZE, obj, task_priority, NULL);
    configASSERT(pdPASS == status);

#ifdef ONLY_THREAD
    init = true;
#endif
}

void active_object_send_event(event_data_t event) {
    ao_event_t *evt = (ao_event_t*)event;

    if (evt == NULL || evt->hao == NULL) {
        LOGGER_INFO("active_object_send_event: event is NULL.\n");
        return;
    }

    if (evt->hao == NULL)
    {
        LOGGER_INFO("active_object_send_event: event is NULL.\n");
    }
    
    LOGGER_INFO("active_object_send_event: Sending event to object ID: %d\n", evt->hao->obj_id);

    BaseType_t status = xQueueSend(evt->hao->event_queue, &(evt->payload), 0);
    if (status == pdPASS) {
        LOGGER_INFO("active_object_send_event: Event successfully sent to queue.\n");
    } else {
        LOGGER_INFO("active_object_send_event: Failed to send event to queue.\n");
    }
}

void active_object_task(void *pv_parameters) {
    active_object_t *obj = (active_object_t *) pv_parameters;
    event_data_t payload = NULL;

    LOGGER_INFO("Active Object Task started for Object ID: %d\n", obj->obj_id);
    LOGGER_INFO("Active Object Task queue handle: %p\n", obj->event_queue);

    for (;;) {
        if (xQueueReceive(obj->event_queue, &payload, portMAX_DELAY) == pdTRUE) {
            LOGGER_INFO("Active Object Task: Received event for processing.\n");
            if (payload == NULL) {
                LOGGER_INFO("Active Object Task: Received NULL payload.\n");
                continue;
            }
            obj->process_event(payload);
        } else {
            LOGGER_INFO("Active Object Task: Queue receive failed.\n");
        }
    }
}
