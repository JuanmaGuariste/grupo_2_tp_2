#include "app.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define pvMALLOC(s) memory_alloc(s,__FILE__,__LINE__)
#define vFREE(s) memory_free(s)
#define FINALIZE_TRACKING finalize_tracking();

void *memory_alloc (size_t length, char* file, uint64_t line);
void memory_free (void* pointer);
void finalize_tracking (void);
