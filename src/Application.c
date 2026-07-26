#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "Application.h"
#include "utils.h"

typedef struct
{
   const char *msg;
   uint8_t id;
} Context_t;

typedef struct
{
   Context_t *context;
   uint64_t header;
   uint32_t footer;
} NewContext_t;

void Application_Init(I_Allocator_t *allocator)
{
   Allocator_Alloc(allocator, sizeof(Context_t));
   Allocator_Alloc(allocator, sizeof(NewContext_t));
   Allocator_Alloc(allocator, sizeof(Context_t));
}
