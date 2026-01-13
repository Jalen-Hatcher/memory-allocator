#include <stdint.h>
#include <stdio.h>
#include "MemoryManager.h"

int main(void)
{
   // static unsigned char array[32];

   // uint32_t *ptr = (uint32_t *)array;
   // *ptr = UINT32_MAX;

   // for(uint8_t i = 0; i < 4; i++)
   // {
   //    printf("%d \n", array[i]);
   // }

   mm_Init();
   // mm_ExtendHeap(2);
   //    mm_Survey();
   return 0;
}
