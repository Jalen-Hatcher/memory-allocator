#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "AllocatorUtils.h"
#include "Allocator_Rev1.h"
#include "I_Allocator.h"
#include "utils.h"

enum
{
   WSIZE = 4,
   DSIZE = 2 * WSIZE, // (bytes)
   ALIGNMENT = DSIZE, // double-word aligned
   HEAP_CAP = 4096
};

static uint8_t heap[HEAP_CAP] = { 0 };

typedef Allocator_Rev1_t Instance_t;

static void InitializeHeap(void)
{
   PUT(heap, 0);
   PUT(heap + (1 * WSIZE), PACK(8, 1)); // prologue header
   PUT(heap + (2 * WSIZE), PACK(8, 1)); // prologue footer
   PUT(heap + (3 * WSIZE), PACK(0, 1)); // epilogue header
}

static inline bool IsEpilogueHeader(void *blockPtr)
{
   // void* because a header pointer could be passed in as
   // uint8_t* or uint32_t*, doesn't really matter functionally
   return (GET(HDRP_FROM_BP(blockPtr)) == 0x1);
}

static void *coalesce(void *blockPtr)
{
   uint8_t prevAlloc = GET_ALLOC(PREV_BLKP(blockPtr));
   uint8_t nextAlloc = GET_ALLOC(NEXT_BLKP(blockPtr));
   uint32_t blockSize = GET_BLOCK_SIZE(blockPtr);

   if(prevAlloc && nextAlloc)
   {
      return blockPtr;
   }

   if(prevAlloc & !nextAlloc)
   {
      blockSize += GET_BLOCK_SIZE(NEXT_BLKP(blockPtr));
      PUT(HDRP_FROM_BP(blockPtr), PACK(blockSize, 0));
      PUT(FTRP_FROM_BP(blockPtr), PACK(blockSize, 0));
   }

   else if(!prevAlloc && nextAlloc)
   {
      blockSize += GET_BLOCK_SIZE(PREV_BLKP(blockPtr));
      PUT(FTRP_FROM_BP(blockPtr), PACK(blockSize, 0));
      PUT(HDRP_FROM_BP(PREV_BLKP(blockPtr)), PACK(blockSize, 0));
   }

   else
   {
      blockSize += GET_BLOCK_SIZE(PREV_BLKP(blockPtr)) + GET_BLOCK_SIZE(NEXT_BLKP(blockPtr));
      PUT(HDRP_FROM_BP(PREV_BLKP(blockPtr)), PACK(blockSize, 0));
      PUT(FTRP_FROM_BP(NEXT_BLKP(blockPtr)), PACK(blockSize, 0));
      blockPtr = PREV_BLKP(blockPtr);
   }

   return blockPtr;
}

static void Free(I_Allocator_t *instance, void *payload)
{
   (void)instance;

   TOGGLE_ALLOC(HDRP_FROM_PYLDP(payload));
   TOGGLE_ALLOC(FTRP_FROM_PYLDP(payload));

   coalesce(((uint8_t *)payload) - WSIZE);
}

// first fit, no coalesce for now
static void *Alloc(I_Allocator_t *instance, uint32_t size)
{
   (void)instance;

   // point to potential first allocated block
   uint8_t *currentBlock = heap + (3 * WSIZE);
   uint32_t blockSize = WSIZE + ROUNDUP(size) + WSIZE;

   // check if we're at the last possible location for an epilogue header
   while((currentBlock + blockSize) <= &heap[HEAP_CAP - 1] - WSIZE)
   {
      if((!GET_ALLOC(currentBlock) && size <= GET_BLOCK_SIZE(currentBlock)) || IsEpilogueHeader(currentBlock))
      {
         PUT(HDRP_FROM_BP(currentBlock), PACK(blockSize, 1)); // header
         PUT(FTRP_FROM_BP(currentBlock), PACK(blockSize, 1)); // footer
         PUT(NEXT_BLKP(currentBlock), PACK(0, 1)); // new epilogue header

         printf("Offset of current allocation: %lu\n", currentBlock - heap);
         printf("Block size: %d\n\n", blockSize);

         return PYLDP_FROM_BP(currentBlock);
      }
      currentBlock = NEXT_BLKP(currentBlock);
   }
   return NULL;
}

static const I_Allocator_Api_t api = { Alloc, Free };

void Allocator_Rev1_Init(Allocator_Rev1_t *instance)
{
   instance->interface.api = &api;

   InitializeHeap();
}
