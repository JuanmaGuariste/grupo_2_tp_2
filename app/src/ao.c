/*
* ao.c
* author: José Luis Krüger
*/
#include "ao.h"
#include "task_button.h"
#include "logger.h"
#define ONLY_THREAD

static bool is_thread_created = false;
static int main_task_priority = tskIDLE_PRIORITY;

// Shared global queue handle
static QueueHandle_t shared_event_queue = NULL;
// constantes definidas para facilitar el debugging del programa
static char *QUEUE_ID_SHARED = "Shared_Queue"; // Queue global
static char *QUEUE_ID_1 = "Queue_id_1";
static char *QUEUE_ID_2 = "Queue_id_2";
static char *QUEUE_ID_3 = "Queue_id_3";
static char *QUEUE_ID_4 = "Queue_id_4";
static char *QUEUE_ID_DEF = "Queue_id_default";

char *get_queue_name( uint8_t id)
{
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
                        uint8_t task_priority)
{
    obj->event_size = sizeof(event_data_t);
    if (shared_event_queue == NULL)
    {
        shared_event_queue = xQueueCreate(queue_size, obj->event_size);
        configASSERT(NULL != shared_event_queue);
        vQueueAddToRegistry(shared_event_queue, QUEUE_ID_SHARED);
    }
    obj->event_queue = shared_event_queue;
    obj->process_event = process_event;
    while (NULL == obj->event_queue)
    {
        LOGGER_INFO("Event queue error.\n");
    }
    LOGGER_INFO("Created Active Object.\n");

#ifdef ONLY_THREAD
    static bool init = false;
    if (init)
    {
        return; // only 1 task for evt_process_callback must exist
    }
#endif
    BaseType_t status;
    status = xTaskCreate(active_object_task, "Task", configMINIMAL_STACK_SIZE, obj, task_priority, NULL);
    configASSERT(pdPASS == status);
#ifdef ONLY_THREAD
    init = true;
    main_task_priority = task_priority;
    is_thread_created = true;
#endif
}

BaseType_t active_object_send_event(event_data_t event)
{
    ao_event_t *evt = (ao_event_t*)event;
    if (evt == NULL || evt->hao == NULL)
    {
        LOGGER_INFO("active_object_send_event: event is NULL.\n");
        return pdFAIL;
    }
#ifdef ONLY_THREAD
    if (!is_thread_created)
    {
        BaseType_t task_status;
        task_status = xTaskCreate(active_object_task, "Task", configMINIMAL_STACK_SIZE, evt->hao, main_task_priority, NULL);
        configASSERT(pdPASS == task_status);
        is_thread_created = true;
    }
#endif
    BaseType_t queue_status = xQueueSend(evt->hao->event_queue, &(evt->payload), 0);
    if (queue_status == pdPASS)
    {
        LOGGER_INFO("active_object_send_event: Event successfully sent to queue.\n");
    } else {
        LOGGER_INFO("active_object_send_event: Failed to send event to queue.\n");
    }
    return queue_status;
}

void active_object_task(void *pv_parameters)
{
    active_object_t *obj = (active_object_t *) pv_parameters;
    event_data_t payload = NULL;

    for (;;)
    {
        if (xQueueReceive(obj->event_queue, &payload, portMAX_DELAY) == pdTRUE)
        {
            obj->process_event(payload);
        } else {
            LOGGER_INFO("Active Object Task: Queue receive failed.\n");
        }
    }
}

void current_task_delete (void)
{
    is_thread_created = false;
    vTaskDelete(NULL);
}
