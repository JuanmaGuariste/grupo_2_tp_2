/*
* ao_controller.c
*
*
*/
#include "ao_controller.h"

evt_process_callback (event_data_t event) {
  if (event == NULL) return;
  
  button_event_t *payload = (button_event_t *)event;
  switch ((int)payload->current_obj_id) {
    case RED_LED_AO_ID:
      handle_red_led_event(event);
      break;

    case GREEN_LED_AO_ID:
      handle_green_led_event(event);
      break;

    case BLUE_LED_AO_ID:
      handle_blue_led_event(event);
      break;
    
    case UI_INTERFACE_AO_ID:
      ui_process_event(event);
      break;

    default:
        //callback de liberacion
      break;
  }

}