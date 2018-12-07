//Donald MacPhail
//first attempt at re-creating malloc with help from online.
//not quite working yet.  WIP.

#include <string.h>
#include <unistd.h>
#define GLOBAL_SIZE sizeof(struct freenode)
struct freenode *globalHead = NULL;
struct freenode *globalTail = NULL;

struct freenode {
    // chunk of memory to store metadata about allocated memory
    // malloc should allocate a block of memory + space for a freenode
    size_t size;
    // pointer to next block in linked list
    struct freenode *next;
    unsigned isFree;
};

struct freenode *checkFreeList(size_t size) {
    // find if there is a free chunk big enough for the request
    struct freenode *current = globalHead;
    while (current) {
        if (current->isFree && current->size >= size) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void my_free(void *block) {
    struct freenode *header, *temp;
    void *brk;

    if (!block) {
        return;
    }

    // get the header of the block we want to free(move -1 for extra space)
    header = (struct freenode *)block - 1;
    brk = sbrk(0);  // current program break

    // find end of current block, check if it's at end of heap
    if ((char *)block + header->size == brk) {
        // reset head and tail
        if (globalHead == globalTail) {
            globalHead = NULL;
            globalTail = NULL;
        } else {
            temp = globalHead;
            while (temp) {
                if (temp->next == globalTail) {
                    temp->next = NULL;
                    globalTail = temp;
                }
                temp = temp->next;
            }
        }
        // size of memory to be released
        // passing negative number to sbrk releases memory
        sbrk(0 - sizeof(struct freenode) - header->size);
        return;
    }
    header->isFree = 1;
}

void *my_malloc(size_t size) {
    size_t totalSize;
    struct freenode *block;
    struct freenode *header;

    if (size <= 0) {
        // if requested size is 0 return null
        return NULL;
    }

    header = checkFreeList(size);
    if (header) {
        // if a free block is found, mark header as not free
        header->isFree = 0;
        // return a pointer to this block to the user
        return (void *)(header + 1);
    }

    // if a free block isnt found, increase the heap size using sbrk()
    // dont forget to add space for header
    totalSize = sizeof(struct freenode) + size;
    // increment the break
    block = (struct freenode *)sbrk(totalSize);

    // fill the header with the requested size, mark as not free
    header = (struct freenode *)block;
    header->size = size;
    header->isFree = 0;
    header->next = NULL;

    // set the head and tail of list storing memory locations
    if (!globalHead) {
        globalHead = header;
    }

    if (globalTail) {
        globalTail->next = header;
    }

    globalTail = header;
    return (void *)(header + 1);
}
