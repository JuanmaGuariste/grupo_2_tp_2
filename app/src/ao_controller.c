/*
* ao_controller.c
*
*
*/
#include "ao_controller.h"
#include "memory.h"

void evt_process_callback (event_data_t event)
{
  if (event == NULL) return;
  button_event_t *payload = (button_event_t *)event;
  switch (payload->current_obj_id)
  {
    case RED_LED_AO_ID:
      handle_red_led_event(payload);
		  break;
    case GREEN_LED_AO_ID:
      handle_green_led_event(payload);
      break;
    case BLUE_LED_AO_ID:
      handle_blue_led_event(payload);
    break;
    case UI_INTERFACE_AO_ID:
      ui_process_event(payload);
      break;
    default:
    break;
  }
  if (payload->free_payload != NULL) payload->free_payload(payload);
  // Se agrega solo con el fin de trackear memoria
  if (payload->current_obj_id == BLUE_LED_AO_ID)
  	  FINALIZE_TRACKING
}
