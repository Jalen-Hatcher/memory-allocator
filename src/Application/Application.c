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

void Application_Init(I_Allocator_t *allocator)
{
   void *ptr1, *ptr2, *ptr3;
   ptr1 = Allocator_Alloc(allocator, sizeof(Context_t));
   ptr2 = Allocator_Alloc(allocator, sizeof(Context_t));
   ptr3 = Allocator_Alloc(allocator, sizeof(Context_t));

   Allocator_Free(allocator, ptr1);
   Allocator_Free(allocator, ptr3);
   Allocator_Free(allocator, ptr2);
   Allocator_Alloc(allocator, sizeof(Context_t));
}
