#ifndef ALLOCATOR_REV1_H
#define ALLOCATOR_REV1_H

#include <stdbool.h>
#include "I_Allocator.h"

typedef struct
{
   I_Allocator_t interface;
} Allocator_Rev1_t;

/*!
 * initialize the linear allocator
 * @param instance
 */
void Allocator_Rev1_Init(Allocator_Rev1_t *instance);

#endif
