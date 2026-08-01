#ifndef ALLOCATORUTILS_H
#define ALLOCATORUTILS_H

// encode the size and allocation type (used/unused)
#define PACK(size, alloc) ((size) | (alloc))

// read/write a word at address p
#define GET(p) (*(uint32_t *)(p))
#define PUT(p, val) ((GET(p)) = val)

// Read the size and allocated fields at address p
#define GET_BLOCK_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) ((GET(p)) & 0x1)

// Toggle allocated field
#define TOGGLE_ALLOC(p) GET(p) ^= 0x1

// get pointers to header, payload and footer given block pointer
#define HDRP_FROM_BP(bp) ((uint32_t *)(bp))
#define FTRP_FROM_BP(bp) ((uint32_t *)(((uint8_t *)bp) + (GET_BLOCK_SIZE(bp) - WSIZE)))
#define PYLDP_FROM_BP(bp) (void *)(((uint8_t *)bp) + WSIZE)

// get pointers to header, footer given payload pointer
#define HDRP_FROM_PYLDP(payload) (uint32_t *)(((uint8_t *)payload) - WSIZE)
#define FTRP_FROM_PYLDP(payload) (uint32_t *)(((uint8_t *)payload) + GET_BLOCK_SIZE(HDRP_FROM_PYLDP(payload)) - DSIZE)

// get next and previous block pointer given current block pointer
#define NEXT_BLKP(bp) ((uint8_t *)((bp) + GET_BLOCK_SIZE((uint8_t *)(bp))))
#define PREV_BLKP(bp) ((uint8_t *)((bp) - GET_BLOCK_SIZE((uint8_t *)((bp) - WSIZE))))

// align memory segment request (8 bytes/double word)
#define ROUNDUP(sz) (((sz) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))
#endif
