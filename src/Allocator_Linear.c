#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "Allocator_Linear.h"
#include "I_Allocator.h"
#include "utils.h"

typedef Allocator_Linear_t Instance_t;

static void InitializeHeap(Instance_t *instance)
{
   PUT(heap, 0);
   PUT(heap + (1 * WSIZE), PACK(8, 1)); // prologue header
   PUT(heap + (2 * WSIZE), PACK(8, 1)); // prologue footer
   PUT(heap + (3 * WSIZE), PACK(0, 1)); // epilogue header

   instance->_private.heapIsInitialized = true;
}

static inline bool IsEpilogueHeader(void *blockPtr)
{
   // void* because a header pointer could be passed in as
   // uint8_t* or uint32_t*, doesn't really matter functionally
   return (GET(HDRP(blockPtr)) == 0x1);
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
      PUT(HDRP(blockPtr), PACK(blockSize, 0));
      PUT(FTRP(NEXT_BLKP(blockPtr)), PACK(blockSize, 0));
   }

   else if(!prevAlloc && nextAlloc)
   {
      blockSize += GET_BLOCK_SIZE(PREV_BLKP(blockPtr));
      PUT(FTRP(blockPtr), PACK(blockSize, 0));
      PUT(HDRP(PREV_BLKP(blockPtr)), PACK(blockSize, 0));
   }

   else
   {
      blockSize += GET_BLOCK_SIZE(PREV_BLKP(blockPtr)) + GET_BLOCK_SIZE(NEXT_BLKP(blockPtr));
      PUT(HDRP(PREV_BLKP(blockPtr)), PACK(blockSize, 0));
      PUT(FTRP(NEXT_BLKP(blockPtr)), PACK(blockSize, 0));
      blockPtr = PREV_BLKP(blockPtr);
   }

   return blockPtr;
}

void Allocator_Free(void *blockPtr)
{
   PUT(HDRP(blockPtr), PACK(0, 0));
   PUT(FTRP(blockPtr), PACK(0, 0));

   coalesce(blockPtr);
}

// first fit, no coalesce for now
static void *Alloc(I_Allocator_t *_instance, uint32_t size)
{
   REINTERPRET(instance, _instance, Allocator_Linear_t *);

   if(!instance->_private.heapIsInitialized)
   {
      InitializeHeap(instance);
   }

   // point to potential first allocated block
   uint8_t *currentBlock = heap + (3 * WSIZE);
   uint32_t blockSize = WSIZE + ROUNDUP(size) + WSIZE;

   // check if we're at the last possible location for an epilogue header
   while((currentBlock + blockSize) <= &heap[HEAP_CAP - 1] - WSIZE)
   {
      if((!GET_ALLOC(currentBlock) && size <= GET_BLOCK_SIZE(currentBlock)) || IsEpilogueHeader(currentBlock))
      {
         PUT(HDRP(currentBlock), PACK(blockSize, 1)); // header
         PUT(FTRP(currentBlock), PACK(blockSize, 1)); // footer
         PUT(NEXT_BLKP(currentBlock), PACK(0, 1)); // new epilogue header

         printf("Offset of current allocation: %lu\n", currentBlock - heap);
         printf("Block size: %d\n\n", blockSize);

         return PYLDP(currentBlock);
      }
      currentBlock = NEXT_BLKP(currentBlock);
   }
   return NULL;
}

static const I_Allocator_Api_t api = { Alloc };

void Allocator_Linear_Init(Allocator_Linear_t *instance)
{
   instance->interface.api = &api;
}
