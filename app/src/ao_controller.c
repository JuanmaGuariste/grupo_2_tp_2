/*
* ao_controller.c
*
*
*/
#include "ao_controller.h"

void evt_process_callback (event_data_t event) {
  if (event == NULL) {
	  LOGGER_INFO("evt_process_callback: got NULL event.\n");
	  return;
  }
  LOGGER_INFO("evt_process_callback: got event.\n");

  button_event_t *payload = (button_event_t *)event;

  if (payload->current_obj_id != NULL) {
	  LOGGER_INFO("evt_process_callback object ID: %d\n", *(payload->current_obj_id));
  } else {
	  LOGGER_INFO("evt_process_callback object ID is NULL.\n");
  }

  LOGGER_INFO("evt_process_callback: got button type = %d\n", *(payload->type));

  LOGGER_INFO("ao controller: RED LED queue handle: %p\n", payload->red_led_obj->event_queue);
  LOGGER_INFO("ao controller: GREEN LED queue handle: %p\n", payload->green_led_obj->event_queue);
  LOGGER_INFO("ao controller: BLUE LED queue handle: %p\n", payload->blue_led_obj->event_queue);

  switch (*(payload->current_obj_id)) {
  	  case RED_LED_AO_ID:
    	LOGGER_INFO("Entered RED LED Active Object switch case.\n");
    	handle_red_led_event(payload);
		break;
	case GREEN_LED_AO_ID:
  	  LOGGER_INFO("ao controller: entered GREEN LED Active Object switch case.\n");
      handle_green_led_event(payload);
      break;
    case BLUE_LED_AO_ID:
  	  LOGGER_INFO("ao controller: entered BLUE LED Active Object switch case.\n");
      handle_blue_led_event(payload);
      break;
    case UI_INTERFACE_AO_ID:
  	  LOGGER_INFO("ao controller: entered UI Active Object switch case.\n");
      ui_process_event(payload);
      break;

    default:
        // TODO: agregar callback de liberacion de memoria
      break;
  }
}
