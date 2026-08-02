#include "Allocator_Rev1.h"
#include "Application.h"

static Allocator_Rev1_t allocatorInstance;

int main(void)
{
   Allocator_Rev1_Init(&allocatorInstance);
   Application_Init(&allocatorInstance.interface);
   return 0;
}
