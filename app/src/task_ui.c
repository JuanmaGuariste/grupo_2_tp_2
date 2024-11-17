/*
 * Copyright (c) 2023 Sebastian Bedin <sebabedin@gmail.com>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @author : Sebastian Bedin <sebabedin@gmail.com>
 */

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os.h"
#include "board.h"
#include "logger.h"
#include "dwt.h"
#include "task_button.h"
#include "task_ui.h"
#include "task_led.h"
/*****************************************************************************/
#include "ao.h"
#include "ao_controller.h"
/********************** macros and definitions *******************************/

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

/********************** external functions definition ************************/

/*************************************************************************** */
void init_ui_active_object(active_object_t *ui_obj, void (*callback)(event_data_t), uint8_t priority) {
  active_object_init(ui_obj, callback , MAX_QUEUE_LENGTH, priority);
}

void ui_process_event(event_data_t event) {
    button_event_t *button_event = (button_event_t *) event;

    LOGGER_INFO("UI processor: got button type = %d\n", *(button_event->type));

    if (button_event->current_obj_id != NULL) {
        LOGGER_INFO("UI processor: current object ID: %d\n", *(button_event->current_obj_id));
    } else {
        LOGGER_INFO("UI processor: current object ID is NULL.\n");
    }

    // TODO: I think bug is here, I must replicate the way button task sends payload.
    switch (*(button_event->type)) {
        case BUTTON_TYPE_PULSE:
        	LOGGER_INFO("UI processor: Detected BUTTON_TYPE_PULSE.\n");
            LOGGER_INFO("UI processor: sending event to red led active object.\n");
            LOGGER_INFO("UI processor: RED LED Active Object ID: %d\n", button_event->red_led_obj->obj_id);
            LOGGER_INFO("UI processor: RED LED queue handle: %p\n", button_event->red_led_obj->event_queue);

            active_object_send_event(&button_event->red_led_obj);
            break;
        case BUTTON_TYPE_SHORT:
        	LOGGER_INFO("UI processor: Detected BUTTON_TYPE_SHORT.\n");
            LOGGER_INFO("UI processor: sending event to green led active object.\n");
            LOGGER_INFO("UI processor: GREEN LED Active Object ID: %d\n", button_event->green_led_obj->obj_id);
            LOGGER_INFO("UI processor: GREEN LED queue handle: %p\n", button_event->green_led_obj->event_queue);
            active_object_send_event(&button_event->green_led_obj);
            break;
        case BUTTON_TYPE_LONG:
        	LOGGER_INFO("UI processor: Detected BUTTON_TYPE_LONG.\n");
            LOGGER_INFO("UI processor: sending event to blue led active object.\n");
            LOGGER_INFO("UI processor: BLUE LED Active Object ID: %d\n", button_event->blue_led_obj->obj_id);
            LOGGER_INFO("UI processor: BLUE LED queue handle: %p\n", button_event->blue_led_obj->event_queue);
            active_object_send_event(&button_event->blue_led_obj);
            break;
        default:
            LOGGER_INFO("Unknown button type: %d\n", *(button_event->type));
            break;
    }
}
/********************** end of file ******************************************/
