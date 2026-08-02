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
   ALIGNMENT = DSIZE,
   MIN_BLOCK_SIZE = 2 * DSIZE, // double-word aligned
   HEAP_CAP = ALIGNMENT * 512,
   HEAP_OFFSET_START = 3 * WSIZE,
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

static void *Coalesce(void *blockPtr)
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

   Coalesce(((uint8_t *)payload) - WSIZE);
}

// Place a block once confirmed to fit, split if remainder > MIN_BLOCK_SIZE
static void Place(void *blockPtr, uint32_t requestedBlockSize)
{
   uint32_t splitBlockSize = GET_BLOCK_SIZE(HDRP_FROM_BP(blockPtr)) - requestedBlockSize;

   if(splitBlockSize >= MIN_BLOCK_SIZE)
   {
      // Place requested block
      PUT(HDRP_FROM_BP(blockPtr), PACK(requestedBlockSize, 1));
      PUT(FTRP_FROM_BP(blockPtr), PACK(requestedBlockSize, 1));

      // Place remaining split block
      PUT(HDRP_FROM_BP(NEXT_BLKP(blockPtr)), PACK(splitBlockSize, 0));
      PUT(FTRP_FROM_BP(NEXT_BLKP(blockPtr)), PACK(splitBlockSize, 0));
   }

   else
   {
      TOGGLE_ALLOC(HDRP_FROM_BP(blockPtr));
      TOGGLE_ALLOC(FTRP_FROM_BP(blockPtr));
   }
}

// First-Fit
static void *Alloc(I_Allocator_t *instance, uint32_t size)
{
   (void)instance;

   // Point to potential first allocated block
   uint8_t *currentBlock = heap + HEAP_OFFSET_START;
   uint32_t requestedBlockSize = BLOCK_SIZE_RESOLVED(size);

   // Check if we're at the last possible location for an epilogue header
   while((currentBlock + requestedBlockSize) <= &heap[HEAP_CAP - 1] - WSIZE)
   {
      if((!GET_ALLOC(currentBlock) && size <= GET_BLOCK_SIZE(currentBlock)))
      {
         Place(currentBlock, requestedBlockSize);

         return PYLDP_FROM_BP(currentBlock);
      }

      else if(IsEpilogueHeader(currentBlock))
      {
         PUT(HDRP_FROM_BP(currentBlock), PACK(requestedBlockSize, 1)); // header
         PUT(FTRP_FROM_BP(currentBlock), PACK(requestedBlockSize, 1)); // footer
         PUT(NEXT_BLKP(currentBlock), PACK(0, 1)); // new epilogue header

         return PYLDP_FROM_BP(currentBlock);
      }
      currentBlock = NEXT_BLKP(currentBlock);
   }
   return NULL;
}

static const I_Allocator_Api_t api = { Alloc, Free };

void Allocator_Rev1_Init(Instance_t *instance)
{
   instance->interface.api = &api;

   InitializeHeap();
}
