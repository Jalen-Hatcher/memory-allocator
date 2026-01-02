#ifndef HEAPENTRY_H
#define HEAPENTRY_H

#include "stdint.h"
#include "stdlib.h"

enum
{
   WSIZE = 4, // (bytes)
   ALIGNMENT = 8, // double-word aligned
};

// encode the size and allocation type (used/unused)
#define PACK(size, alloc) ((size) | (alloc))

// read/write a word at address p
#define GET(p) (*(uint32_t *)(p))
#define PUT(p, val) ((GET(p)) = val)

// Read the size and allocated fields at address p
#define GET_BLOCK_SIZE(h) (h & ~0x7)
#define GET_ALLOC(h) (h & 0x1)

#define SET_BLOCK_SIZE(h, val) (h = (size_t)val)
#define SET_USED(h) (h |= 0x1)
#define SET_UNUSED(h) (h &= 0x1)

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
