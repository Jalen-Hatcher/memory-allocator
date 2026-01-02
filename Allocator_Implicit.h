#ifndef HEAPENTRY_H
#define HEAPENTRY_H

#include "stdlib.h"

enum
{
   HEADER_SIZE = 8, // bytes (long),
   ALIGNMENT = 8,
};

#define GET_HEADER(blk) (blk.header)
#define GET_BLOCK_SIZE(h) (h >> 3UL) // using right shift as this may be more cross-compatible
#define GET_USED(h) (h & 1UL)
#define SET_BLOCK_SIZE(h, val) (h = (size_t)val)
#define SET_USED(h) (h |= 1UL)
#define SET_UNUSED(h) (h &= ~(1UL))

typedef struct
{
   size_t header;
   void *payload;
} HeapBlock_t;

static void *segmentStart;
static size_t segmentSize;
static HeapBlock_t *base;

void Heap_Init(void *heapStart, const size_t heapSize);

void HeapAlloc(size_t size);
#endif
