#include <assert.h>
#include "Allocator.h"

int main(void)
{
   void *ptr = Allocator_Alloc(502);
   assert(ptr != NULL);

   ptr = Allocator_Alloc(50);
   assert(ptr != NULL);
   return 0;
}
