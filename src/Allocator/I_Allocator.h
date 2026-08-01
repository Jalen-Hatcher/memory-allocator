#ifndef I_ALLOCATOR_H
#define I_ALLOCATOR_H

#include "stdint.h"

struct I_Allocator_Api_t;

typedef struct
{
   const struct I_Allocator_Api_t *api;
} I_Allocator_t;

typedef struct I_Allocator_Api_t
{
   /*!
    * @param instance
    * @param size
    * @returns void *
    */
   void *(*Alloc)(I_Allocator_t *instance, uint32_t size);

   /*!
    * @param instance
    * @param payload
    */
   void (*Free)(I_Allocator_t *instance, void *payload);
} I_Allocator_Api_t;

static inline void *Allocator_Alloc(I_Allocator_t *instance, uint32_t size)
{
   return instance->api->Alloc(instance, size);
}

static inline void Allocator_Free(I_Allocator_t *instance, void *payload)
{
   instance->api->Free(instance, payload);
}
#endif
