
#include "memory.h"
#include "FreeRTOS.h"
#include "logger.h"
#include "cmsis_os.h"
#include <inttypes.h>

#define MEMORY_TRACKING

typedef struct {
  uint32_t addr;
  char *file;
  uint32_t line;
  bool reserved;
} memory_t;

memory_t memory_buffer[255] = { 0 };

static void add_memory_to_register (uint32_t pointer, char* file, uint64_t line);
static void remove_memory_to_register (uint32_t pointer);

void *memory_alloc (size_t length, char* file, uint64_t line) {
  if (length == 0) return NULL;

  void *pointer = pvPortMalloc(length);
  if (pointer != NULL) {
    #ifdef MEMORY_TRACKING
    add_memory_to_register ((uint32_t)pointer,file, line);
    #endif
  }
  return pointer;
}

void memory_free (void* pointer) {
  if (pointer == NULL) return;

  vPortFree(pointer);

  #ifdef MEMORY_TRACKING
  remove_memory_to_register ((uint32_t)pointer);
  #endif
}

static void add_memory_to_register (uint32_t pointer, char* file, uint64_t line) {
  uint8_t index = 0;
  bool finalized = false;
  while (index < sizeof(memory_buffer)/sizeof(memory_t) && !finalized) {
    if (!memory_buffer[index].reserved){
      memory_buffer[index].addr = pointer;
      memory_buffer[index].file = file;
      memory_buffer[index].line = (uint32_t)line;
      memory_buffer[index].reserved = true;
      finalized = true;
    }
    index++;
  }
}

static void remove_memory_to_register (uint32_t pointer) {
  uint8_t index = 0;
  bool finalized = false;
  while (index < sizeof(memory_buffer)/sizeof(memory_t) && !finalized) {
    if (memory_buffer[index].addr == pointer) {
      memory_buffer[index].addr = 0x00;
      memory_buffer[index].reserved = false;
      finalized = true;
    }
    index++;
  }
}

void finalize_tracking (void) {
  uint8_t index = 0;

  while (index < sizeof(memory_buffer)/sizeof(memory_t)) {
    if (memory_buffer[index].reserved) {
      LOGGER_INFO("Memoria no liberada : 0x%lx",memory_buffer[index].addr);
      LOGGER_INFO("en el archivo %s", memory_buffer[index].file);
      LOGGER_INFO("linea: %lu", memory_buffer[index].line);
    }
    index++;
  }
}
