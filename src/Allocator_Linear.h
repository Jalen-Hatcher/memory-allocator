#ifndef ALLOCATOR_LINEAR_H
#define ALLOCATOR_LINEAR_H

#include <stdbool.h>
#include "I_Allocator.h"

typedef struct
{
   I_Allocator_t interface;

   struct
   {
      bool heapIsInitialized;
   } _private;
} Allocator_Linear_t;

/*!
 * initialize the linear allocator
 * @param instance
 */
void Allocator_Linear_Init(Allocator_Linear_t *instance);

#endif
