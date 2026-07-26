#include "Allocator_Linear.h"
#include "Application.h"

static Allocator_Linear_t allocatorInstance;

int main(void)
{
   Allocator_Linear_Init(&allocatorInstance);
   Application_Init(&allocatorInstance.interface);
   return 0;
}
