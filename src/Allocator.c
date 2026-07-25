#include <stdbool.h>
#include <stddef.h>
#include "Allocator.h"

static uint8_t heap[HEAP_CAP] = { 0 };
static bool heapIsInitialized;

static void Allocator_Init()
{
   PUT(heap, 0);
   PUT(heap + (1 * WSIZE), PACK(8, 1)); // prologue header
   PUT(heap + (2 * WSIZE), PACK(8, 1)); // prologue footer
   PUT(heap + (3 * WSIZE), PACK(0, 1)); // epilogue header

   heapIsInitialized = true;
}

static inline bool IsEpilogueHeader(void *blockPtr)
{
   // void* because a header pointer could be passed in as
   // uint8_t* or uint32_t*, doesn't really matter functionally
   return (GET(HDRP(blockPtr)) == 0x1);
}

// first fit, no coalesce for now
void *Allocator_Alloc(uint32_t size)
{
   if(!heapIsInitialized)
   {
      Allocator_Init();
   }

   // point to potential first allocated block
   uint8_t *currentBlock = heap + (3 * WSIZE);
   uint32_t blockSize = WSIZE + ROUNDUP(size) + WSIZE;

   // check if we're at the last possible location for an epilogue header
   while((currentBlock + blockSize) <= &heap[HEAP_CAP] - WSIZE)
   {
      if(!GET_ALLOC(currentBlock) || IsEpilogueHeader(currentBlock))
      {
         PUT(HDRP(currentBlock), PACK(blockSize, 1)); // header
         PUT(FTRP(currentBlock), PACK(blockSize, 1)); // footer
         PUT(NEXT_BLKP(currentBlock), PACK(0, 1)); // new epilogue header

         return PYLDP(currentBlock);
      }
      currentBlock = NEXT_BLKP(currentBlock);
   }
   return NULL;
}
