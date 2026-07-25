#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "stdint.h"
#include "stdio.h"

enum
{
   WSIZE = 4,
   DSIZE = 8, // (bytes)
   ALIGNMENT = DSIZE, // double-word aligned
   MAX_HEAP_EXTEND = 1 << 12,
   HEAP_CAP = 4096
};

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

/*!
 * allocate a block of memory
 */
void *Allocator_Alloc(uint32_t size);

#endif
