#ifndef HEAPENTRY_H
#define HEAPENTRY_H

#include "stdint.h"
#include "stdlib.h"

enum
{
   WSIZE = 4,
   DSIZE = 8, // (bytes)
   ALIGNMENT = 8, // double-word aligned
   MAX_HEAP_EXTEND = 1 << 12
};

// encode the size and allocation type (used/unused)
#define PACK(size, alloc) ((size) | (alloc))

// read/write a word at address p
#define GET(p) (*(uint32_t *)(p))
#define PUT(p, val) ((GET(p)) = val)

// Read the size and allocated fields at address p
#define GET_BLOCK_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) ((GET(p)) & 0x1)

// get header and footer given block pointer
#define HDRP(bp) ((char *)(bp) - WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_BLOCK_SIZE(HDRP(bp)) - DSIZE)

// get header and footer of next block
#define NEXT_BLKP(bp) ((char *)(bp) + GET_BLOCK_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_BLOCK_SIZE(((char *)(bp) - DSIZE)))

// align memory segment request (8 bytes/double word)
#define ROUNDUP(sz) (((sz) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

// initialize memory
int mm_init(size_t size);

// extend the heap by (size) words
void *mm_extendHeap(size_t size);

#endif
