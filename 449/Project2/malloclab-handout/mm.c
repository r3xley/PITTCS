// This adds coalescing of free blocks.
// Improves performance to 54/100 ... takes less time.

/*-------------------------------------------------------------------
 *  Malloc Lab Starter code:
 *        single doubly-linked free block list with LIFO policy
 *        with support for coalescing adjacent free blocks
 *
 * Terminology:
 * o We will implement an explicit free list allocator.
 * o We use "next" and "previous" to refer to blocks as ordered in
 *   the free list.
 * o We use "following" and "preceding" to refer to adjacent blocks
 *   in memory.
 *-------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

#include "memlib.h"
#include "mm.h"

/* Macros for unscaled pointer arithmetic to keep other code cleaner.
   Casting to a char* has the effect that pointer arithmetic happens at
   the byte granularity (i.e. POINTER_ADD(0x1, 1) would be 0x2).  (By
   default, incrementing a pointer in C has the effect of incrementing
   it by the size of the type to which it points (e.g. Block).)
   We cast the result to void* to force you to cast back to the
   appropriate type and ensure you don't accidentally use the resulting
   pointer as a char* implicitly.
*/
#define UNSCALED_POINTER_ADD(p, x) ((void *)((char *)(p) + (x)))
#define UNSCALED_POINTER_SUB(p, x) ((void *)((char *)(p) - (x)))

/******** FREE LIST IMPLEMENTATION ***********************************/

/* An BlockInfo contains information about a block, including the size
   as well as pointers to the next and previous blocks in the free list.
   This is similar to the "explicit free list" structure illustrated in
   the lecture slides.

   Note that the next pointer are only needed when the block is free. To
   achieve better utilization, mm_malloc should use the space for next as
   part of the space it returns.

   +--------------+
   |     size     |  <-  Block pointers in free list point here
   |              |
   |   (header)   |
   |              |
   |     prev     |
   +--------------+
   |   nextFree   |  <-  Pointers returned by mm_malloc point here
   |   prevFree   |
   +--------------+      (allocated blocks do not have a 'nextFree' field)
   |  space and   |      (this is a space optimization...)
   |   padding    |
   |     ...      |      Free blocks write their nextFree/prevFree pointers in
   |     ...      |      this space.
   +--------------+

*/
typedef struct _BlockInfo
{
  // Size of the block and whether or not the block is in use or free.
  // When the size is negative, the block is currently free.
  long int size;
  // Pointer to the previous block in the list.
  struct _Block *prev;
} BlockInfo;

/* A FreeBlockInfo structure contains metadata just for free blocks.
 * When you are ready, you can improve your naive implementation by
 * using these to maintain a separate list of free blocks.
 *
 * These are "kept" in the region of memory that is normally used by
 * the program when the block is allocated. That is, since that space
 * is free anyway, we can make good use of it to improve our malloc.
 */
typedef struct _FreeBlockInfo
{
  // Pointer to the next free block in the list.
  struct _Block *nextFree;
  // Pointer to the previous free block in the list.
  struct _Block *prevFree;
} FreeBlockInfo;

/* This is a structure that can serve as all kinds of nodes.
 */
typedef struct _Block
{
  BlockInfo info;
  FreeBlockInfo freeNode;
} Block;

/* Pointer to the first FreeBlockInfo in the free list, the list's head. */
static Block *free_list_head = NULL;
static Block *malloc_list_tail = NULL;

static size_t heap_size = 0;

/* Size of a word on this architecture. */
#define WORD_SIZE sizeof(void *)

/* Alignment of blocks returned by mm_malloc.
 * (We need each allocation to at least be big enough for the free space
 * metadata... so let's just align by that.)  */
#define ALIGNMENT (sizeof(FreeBlockInfo))

/* This function will have the OS allocate more space for our heap.
 *
 * It returns a pointer to that new space. That pointer will always be
 * larger than the last request and be continuous in memory.
 */
void *requestMoreSpace(size_t reqSize);

/* This function will get the first block or returns NULL if there is not
 * one.
 *
 * You can use this to start your through search for a block.
 */
Block *first_block();

/* This function will get the adjacent block or returns NULL if there is not
 * one.
 *
 * You can use this to move along your malloc list one block at a time.
 */
Block *next_block(Block *block);

/* Use this function to print a thorough listing of your heap data structures.
 */
void examine_heap();

/* Checks the heap for any issues and prints out errors as it finds them.
 *
 * Use this when you are debugging to check for consistency issues. */
int check_heap();

Block *searchList(size_t reqSize)
{
  Block *ptrFreeBlock = first_block();
  long int checkSize = -reqSize;

  // ptrFreeBlock will point to the beginning of the memory heap!
  // end will point to the end of the memory heap.
  //
  // You want to go through every block until you hit the end.
  // Make sure you read the explanation for the next_block function above.
  // It should come in handy!

  //
  // YOUR CODE HERE!
  //
  while (ptrFreeBlock != NULL)
  {
    size_t sizeBlock;
    if (ptrFreeBlock->info.size > 0)
    {
      sizeBlock = ptrFreeBlock->info.size;
    }
    else
    {
      sizeBlock = -ptrFreeBlock->info.size;
    }

    if (sizeBlock >= reqSize && ptrFreeBlock->info.size < 0)
    {
      return ptrFreeBlock;
    }

    ptrFreeBlock = next_block(ptrFreeBlock);
  }

  // To begin, you can ignore the free list and just go through every single
  // block in your memory looking for a free space big enough.
  //
  // Return NULL when you cannot find any available node big enough.

  return NULL;
}
//******************************************************************************************************************************************************************************
/* Find a free block of at least the requested size in the free list.  Returns
   NULL if no free block is large enough. */
Block *searchFreeList(size_t reqSize)
{
  Block *ptrFreeBlock = free_list_head;
  long int checkSize = -reqSize;

  //
  // YOUR CODE HERE!
  //

  // When you are ready, you can implement the free list.

  while (ptrFreeBlock)
  {
    // Check if the size of the free block is greater than or equal to reqSize
    if (-(ptrFreeBlock->info.size) >= reqSize)
    {
      return ptrFreeBlock; // Return the free block
    }

    ptrFreeBlock = ptrFreeBlock->freeNode.nextFree; // Move to the next free block in the list
  }

  // else return null
  return NULL;
}

void insertFreeBlock(Block *block)
{
  // Make sure the block is marked as free (size is negative)
  if (block == NULL)
  {
    return;
  }

  if (block->info.size > 0)
  {
    return; // Not a free block
  }

  // Update the nextFree and prevFree pointers of the block
  block->freeNode.nextFree = free_list_head;
  block->freeNode.prevFree = NULL;

  // If the free list is not empty, update the previous block's prevFree pointer
  if (free_list_head != NULL)
  {
    free_list_head->freeNode.prevFree = block;
  }

  // Update the head of the free list to point to the new block
  free_list_head = block;
}

void removeFreeBlock(Block *block)
{
  // Make sure the block is marked as free
  if (block == NULL)
  {
    return;
  }
  if (block->info.size > 0)
  {
    return; // Not a free block
  }

  // Update the previous block's nextFree pointer
  if (block->freeNode.prevFree != NULL)
  {
    block->freeNode.prevFree->freeNode.nextFree = block->freeNode.nextFree;
  }
  else
  {
    // If there is no previous block, it means this block is the head of the free list
    free_list_head = block->freeNode.nextFree;
  }

  // Update the next block's prevFree pointer
  if (block->freeNode.nextFree != NULL)
  {
    block->freeNode.nextFree->freeNode.prevFree = block->freeNode.prevFree;
  }

  // Clear the free list pointers of the removed block
  block->freeNode.nextFree = NULL;
  block->freeNode.prevFree = NULL;
}
//******************************************************************************************************************************************************************************
// TOP-LEVEL ALLOCATOR INTERFACE ------------------------------------

//
/* Allocate a block of size size and return a pointer to it. If size is zero,
 * returns null.
 */
void *mm_malloc(size_t size)
{
  Block *ptrFreeBlock = NULL;
  Block *splitBlock = NULL;
  long int reqSize;

  // Zero-size requests get NULL.
  if (size == 0)
  {
    return NULL;
  }

  // Determine the amount of memory we want to allocate
  reqSize = size;

  // Round up for correct alignment
  reqSize = ALIGNMENT * ((reqSize + ALIGNMENT - 1) / ALIGNMENT);

  //
  // YOUR CODE HERE! (ignore size and use reqSize for the amount to allocate!)
  //
  // ptrFreeBlock = searchList(reqSize);
  ptrFreeBlock = searchFreeList(reqSize);
  // You can change or remove the declarations
  // above.  They are included as minor hints.
  if (ptrFreeBlock)
  {
    //********************************************************************************************************************************************************************************
    // SPLIT
    //********************************************************************************************************************************************************************************
    // check if we can split
    if (-ptrFreeBlock->info.size > (reqSize + sizeof(BlockInfo)))
    {

      // new pointer
      size_t newBlockSize = reqSize + (sizeof(BlockInfo));
      Block *newBlock = (Block *)UNSCALED_POINTER_ADD(ptrFreeBlock, newBlockSize);

      // check if at tail
      if (malloc_list_tail == ptrFreeBlock)
      {
        malloc_list_tail = newBlock;
      }
      // Update the size and status of the current block
      newBlock->info.size = ptrFreeBlock->info.size + newBlockSize;
      newBlock->info.prev = ptrFreeBlock;
      insertFreeBlock(newBlock);

      removeFreeBlock(ptrFreeBlock);
      ptrFreeBlock->info.size = reqSize;

      // Update the prev pointer of the block following the current block
      Block *followingBlock = next_block(newBlock);
      // if it exists
      /// MOVING TAIL
      if (followingBlock == NULL)
      {
        malloc_list_tail = newBlock;
      }
      else
      {
        followingBlock->info.prev = newBlock;
      }

      // malloc_list_tail = followingBlock;
      /// MOVING TAIL

      // followingBlock->info.prev = newBlock;

      return (Block *)UNSCALED_POINTER_ADD(ptrFreeBlock, sizeof(BlockInfo));
      // return ptrFreeBlock;
    }
    else
    {
      removeFreeBlock(ptrFreeBlock);
      ptrFreeBlock->info.size = -ptrFreeBlock->info.size;
      return (Block *)UNSCALED_POINTER_ADD(ptrFreeBlock, sizeof(BlockInfo));
    }

    //********************************************************************************************************************************************************************************
    // ptrFreeBlock->info.size = -ptrFreeBlock->info.size;

    // return (BlockInfo *)(ptrFreeBlock + 1);
    // return (Block *)UNSCALED_POINTER_ADD(ptrFreeBlock, sizeof(BlockInfo));
  }

  else
  {
    ptrFreeBlock = (Block *)requestMoreSpace(reqSize + sizeof(BlockInfo));
    // ptrFreeBlock->info.prev = malloc_list_tail;
    // malloc_list_tail = ptrFreeBlock;
    if (ptrFreeBlock)
    {
      ptrFreeBlock->info.prev = malloc_list_tail;
      malloc_list_tail = ptrFreeBlock;
      ptrFreeBlock->info.size = reqSize; //
      // return (BlockInfo *)(ptrFreeBlock + 1);
      // reqSize = reqSize;
      return (Block *)UNSCALED_POINTER_ADD(ptrFreeBlock, sizeof(BlockInfo));
    }
    else
    {
      return NULL;
    }
  }

  // Implement mm_malloc.  You can change or remove any of the above
  // code.  It is included as a suggestion of where to start.
  //
  // Remember to maintain your free_list_head
  //
  // You will want to replace this return statement...

  // When you are ready to implement a free list, remove the searchList call
  // and uncomment the searchFreeList call below it.
  // ptrFreeBlock = searchList(reqSize);
  // ptrFreeBlock = searchFreeList(reqSize);

  // ptrFreeBlock is either NULL (no room left) or a currently free block
}

void coalesce(Block *blockInfo)
{
  Block *nextBlock = next_block(blockInfo);
  Block *previousBlock = blockInfo->info.prev;
  Block *tmpBlock = NULL;
  //**************************************For free
  // Block *nextBlock = blockInfo->freeNode.nextFree;
  // Block *previousBlock = blockInfo->freeNode.prevFree;
  // Block *tmpBlock = NULL;

  //
  // YOUR CODE HERE!
  //
  if (nextBlock && nextBlock->info.size < 0)
  {
    removeFreeBlock(nextBlock);
    blockInfo->info.size += nextBlock->info.size + -(sizeof(BlockInfo));
    // insertFreeBlock(blockInfo);
    //  negate size
    //  nextBlock->info.prev = blockInfo->info.prev;
    Block *followingBlock = next_block(nextBlock);
    if (followingBlock != NULL)
    {
      followingBlock->info.prev = blockInfo;
    }

    if (nextBlock == malloc_list_tail)
    {
      malloc_list_tail = blockInfo;
    }
  }

  if (previousBlock && previousBlock->info.size < 0)
  {
    // Merge the current block into the previous block
    removeFreeBlock(blockInfo);
    previousBlock->info.size += blockInfo->info.size + -(sizeof(BlockInfo));
    // insertFreeBlock(previousBlock);
    //  blockInfo->info.prev = previousBlock->info.prev;
    Block *followingBlock = next_block(blockInfo);
    if (followingBlock != NULL)
    {
      followingBlock->info.prev = previousBlock;
    }

    // Update the tail pointer if the merged block was the tail
    if (blockInfo == malloc_list_tail)
    {
      malloc_list_tail = previousBlock;
    }
    blockInfo = previousBlock;
  }
  // You can change or remove the declarations
  // above.  They are included as minor hints.
}

/* Free the block referenced by ptr. */
void mm_free(void *ptr)
{
  Block *blockInfo = (Block *)UNSCALED_POINTER_SUB(ptr, sizeof(BlockInfo));

  //
  // YOUR CODE HERE!
  //
  blockInfo->info.size = -blockInfo->info.size;
  insertFreeBlock(blockInfo);
  // You can change or remove the declarations
  // above.  They are included as minor hints.
  //
  // Remember to maintain your free_list_head

  // When you are ready... you will want to implement coalescing:
  coalesce(blockInfo);
}

// PROVIDED FUNCTIONS -----------------------------------------------
//
// You do not need to modify these, but they might be helpful to read
// over.

/* Get more heap space of exact size reqSize. */
void *requestMoreSpace(size_t reqSize)
{
  void *ret = UNSCALED_POINTER_ADD(mem_heap_lo(), heap_size);
  heap_size += reqSize;

  void *mem_sbrk_result = mem_sbrk(reqSize);
  if ((size_t)mem_sbrk_result == -1)
  {
    printf("ERROR: mem_sbrk failed in requestMoreSpace\n");
    exit(0);
  }

  return ret;
}

/* Initialize the allocator. */
int mm_init()
{
  free_list_head = NULL;
  malloc_list_tail = NULL;
  heap_size = 0;

  return 0;
}

/* Gets the first block in the heap or returns NULL if there is not one. */
Block *first_block()
{
  Block *first = (Block *)mem_heap_lo();
  if (heap_size == 0)
  {
    return NULL;
  }

  return first;
}

/* Gets the adjacent block or returns NULL if there is not one. */
Block *next_block(Block *block)
{
  size_t distance = (block->info.size > 0) ? block->info.size : -block->info.size;

  Block *end = (Block *)UNSCALED_POINTER_ADD(mem_heap_lo(), heap_size);
  Block *next = (Block *)UNSCALED_POINTER_ADD(block, sizeof(BlockInfo) + distance);
  if (next >= end)
  {
    return NULL;
  }

  return next;
}

/* Print the heap by iterating through it as an implicit free list. */
void examine_heap()
{
  /* print to stderr so output isn't buffered and not output if we crash */
  Block *curr = (Block *)mem_heap_lo();
  Block *end = (Block *)UNSCALED_POINTER_ADD(mem_heap_lo(), heap_size);
  fprintf(stderr, "heap size:\t0x%lx\n", heap_size);
  fprintf(stderr, "heap start:\t%p\n", curr);
  fprintf(stderr, "heap end:\t%p\n", end);

  fprintf(stderr, "free_list_head: %p\n", (void *)free_list_head);

  fprintf(stderr, "malloc_list_tail: %p\n", (void *)malloc_list_tail);

  while (curr && curr < end)
  {
    /* print out common block attributes */
    fprintf(stderr, "%p: %ld\t", (void *)curr, curr->info.size);

    /* and allocated/free specific data */
    if (curr->info.size > 0)
    {
      fprintf(stderr, "ALLOCATED\tprev: %p\n", (void *)curr->info.prev);
    }
    else
    {
      fprintf(stderr, "FREE\tnextFree: %p, prevFree: %p, prev: %p\n", (void *)curr->freeNode.nextFree, (void *)curr->freeNode.prevFree, (void *)curr->info.prev);
    }

    curr = next_block(curr);
  }
  fprintf(stderr, "END OF HEAP\n\n");

  curr = free_list_head;
  fprintf(stderr, "Head ");
  while (curr)
  {
    fprintf(stderr, "-> %p ", curr);
    curr = curr->freeNode.nextFree;
  }
  fprintf(stderr, "\n");
}

/* Checks the heap data structure for consistency. */
int check_heap()
{
  Block *curr = (Block *)mem_heap_lo();
  Block *end = (Block *)UNSCALED_POINTER_ADD(mem_heap_lo(), heap_size);
  Block *last = NULL;
  long int free_count = 0;

  while (curr && curr < end)
  {
    if (curr->info.prev != last)
    {
      fprintf(stderr, "check_heap: Error: previous link not correct.\n");
      examine_heap();
    }

    if (curr->info.size <= 0)
    {
      // Free
      free_count++;
    }

    last = curr;
    curr = next_block(curr);
  }

  curr = free_list_head;
  last = NULL;
  while (curr)
  {
    if (curr == last)
    {
      fprintf(stderr, "check_heap: Error: free list is circular.\n");
      examine_heap();
    }
    last = curr;
    curr = curr->freeNode.nextFree;
    if (free_count == 0)
    {
      fprintf(stderr, "check_heap: Error: free list has more items than expected.\n");
      examine_heap();
    }
    free_count--;
  }

  return 0;
}
