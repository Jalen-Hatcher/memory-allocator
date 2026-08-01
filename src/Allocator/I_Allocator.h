#ifndef I_ALLOCATOR_H
#define I_ALLOCATOR_H

#include "stdint.h"

enum
{
   WSIZE = 4,
   DSIZE = 8, // (bytes)
   ALIGNMENT = DSIZE, // double-word aligned
   HEAP_CAP = 4096
};

static uint8_t heap[HEAP_CAP] = { 0 };

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
    * @param copyTo
    * @returns void *
    */
   void *(*Alloc)(I_Allocator_t *instance, uint32_t size);
} I_Allocator_Api_t;

// encode the size and allocation type (used/unused)
#define PACK(size, alloc) ((size) | (alloc))

// read/write a word at address p
#define GET(p) (*(uint32_t *)(p))
#define PUT(p, val) ((GET(p)) = val)

// Read the size and allocated fields at address p
#define GET_BLOCK_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) ((GET(p)) & 0x1)

// get pointers to header, payload and footer given block pointer
#define HDRP(bp) ((uint32_t *)(bp))
#define FTRP(bp) ((uint32_t *)(((uint8_t *)bp) + (GET_BLOCK_SIZE(bp) - WSIZE)))
#define PYLDP(bp) (void *)(((uint8_t *)bp) + WSIZE)

// get next and previous block pointer, given current block pointer
#define NEXT_BLKP(bp) ((uint8_t *)((bp) + GET_BLOCK_SIZE((uint8_t *)(bp))))
#define PREV_BLKP(bp) ((uint8_t *)((bp) - GET_BLOCK_SIZE((uint8_t *)((bp) - WSIZE))))

// align memory segment request (8 bytes/double word)
#define ROUNDUP(sz) (((sz) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

static inline void *Allocator_Alloc(I_Allocator_t *instance, uint32_t size)
{
   return instance->api->Alloc(instance, size);
}
#endif
