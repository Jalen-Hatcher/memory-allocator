#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/mman.h>
#include "MemoryManager.h"

static char heap[HEAP_CAP]; // heap array

static char *memHeap; // pointer to first byte of heap
static char *memBreak; // pointer to last byte of heap
static char *memMaxAddr; // max legal heap addr plus 1

int mm_Init()
{
   memHeap = heap;
   memBreak = memHeap;
   memMaxAddr = memHeap + HEAP_CAP;

   PUT(memHeap, 0); // double-word aligned
   PUT((memHeap + WSIZE), PACK(8, 1)); // prologue header
   PUT(memHeap + (2 * WSIZE), PACK(8, 1)); // prologue footer
   PUT(memHeap + (3 * WSIZE), PACK(0, 1)); // epilogue footer
   return 0;
}

void mm_Survey()
{
   uint32_t *itr = (uint32_t *)(memHeap + WSIZE);
   while(GET_BLOCK_SIZE(itr) != 0)
   {
      printf("Size: %d\nAllocation: %d\n\n", GET_BLOCK_SIZE(itr), GET_ALLOC(itr));
      itr = (uint32_t *)HDRP(NEXT_BLKP((itr + 1)));
   }
}

static void *mem_sbrk(int incr)
{
   char *oldBreak = memBreak;

   errno = ENOMEM;
   if((incr < 0) || ((memBreak + incr) > memMaxAddr))
   {
      fprintf(stderr, "ERROR: memBrk failed. Ran out of memory...\n");
      return (void *)-1;
   }
   memBreak += incr;
   return (void *)oldBreak;
}

static void *coalesce(void *bp)
{
   size_t prevAlloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
   size_t nextAlloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
   size_t size = GET_BLOCK_SIZE(HDRP(bp));

   if(prevAlloc && nextAlloc) // previous block & next block allocated
   {
      return bp;
   }

   if(prevAlloc && !nextAlloc) // previous block allocated & next block free
   {
      size += GET_BLOCK_SIZE(HDRP(NEXT_BLKP(bp)));
      PUT(HDRP(bp), PACK(size, 0));
      PUT(FTRP(bp), PACK(size, 0));
   }

   if(!prevAlloc && nextAlloc) // previous block free & next block allocated
   {
      size += GET_BLOCK_SIZE(HDRP(PREV_BLKP(bp)));
      PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
      PUT(FTRP(PREV_BLKP(bp)), PACK(size, 0));
      bp = PREV_BLKP(bp);
   }

   if(!prevAlloc && !nextAlloc) // previous block and next block free
   {
      size += (GET_BLOCK_SIZE(HDRP(NEXT_BLKP(bp))) + GET_BLOCK_SIZE(HDRP(PREV_BLKP(bp))));
      PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
      PUT(FTRP(PREV_BLKP(bp)), PACK(size, 0));
      bp = PREV_BLKP(bp);
   }

   return bp;
}

void *mm_ExtendHeap(size_t words)
{
   char *bp;
   size_t size; /* Allocate an even number of words to maintain alignment */
   size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
   if((long)(bp = mem_sbrk(size)) == -1)
   {
      return NULL;
   }

   // initialize free block header/footer and the epilogue header
   PUT(HDRP(bp), PACK(size, 0)); // free block header
   PUT(FTRP(bp), PACK(size, 0)); // free block footer
   PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); // new epilogue header
   // coalesce if the previous block was free
   return coalesce(bp);
}

void *mm_Malloc(size_t size)
{
   // if(size > 0)
   // {
   //    void *bp;
   //    size_t alignedSize = ROUNDUP(size);
   // }
   (void)size;

   return NULL;
}
