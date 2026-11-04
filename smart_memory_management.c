
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN_POOL_SIZE 1
#define MAX_POOL_SIZE 1000000

// Structure representing a memory block
typedef struct
{
    int start;
    int size;
    int id;
    int allocated;
} Block;

// Global memory pool and metadata
unsigned char *memory = NULL;
Block *blocks = NULL;

int poolSize;
int blockCount = 1;
int capacity = 2;
int nextID = 1;
int strategy = 1;


// Function declarations
int findFirstFit(int size);
int findBestFit(int size);
int findWorstFit(int size);

int allocateMemory(int size);
void releaseMemory(int id);
void resizeMemory(int id, int newSize);

void mergeFreeBlocks(void);
int findBlockByID(int id);

void displayMemoryMap(void);
void displayStatistics(void);
void displayFragmentation(int size);

void changeStrategy(void);
int ensureCapacity(int required);


// Ensure metadata array has sufficient capacity
int ensureCapacity(int required)
{
    if (required <= capacity)
        return 1;

    int newCapacity = capacity;

    while (newCapacity < required)
        newCapacity *= 2;

    Block *temp = realloc(blocks,
                          newCapacity * sizeof(Block));

    if (temp == NULL)
        return 0;

    blocks = temp;
    capacity = newCapacity;

    return 1;
}


// FIRST FIT
int findFirstFit(int size)
{
    for (int i = 0; i < blockCount; i++)
    {
        if (!blocks[i].allocated &&
            blocks[i].size >= size)
        {
            return i;
        }
    }

    return -1;
}


// BEST FIT
int findBestFit(int size)
{
    int best = -1;

    for (int i = 0; i < blockCount; i++)
    {
        if (!blocks[i].allocated &&
            blocks[i].size >= size)
        {
            if (best == -1 ||
                blocks[i].size < blocks[best].size)
            {
                best = i;
            }
        }
    }

    return best;
}


// WORST FIT
int findWorstFit(int size)
{
    int worst = -1;

    for (int i = 0; i < blockCount; i++)
    {
        if (!blocks[i].allocated &&
            blocks[i].size >= size)
        {
            if (worst == -1 ||
                blocks[i].size > blocks[worst].size)
            {
                worst = i;
            }
        }
    }

    return worst;
}


// Find an allocated block using its ID
int findBlockByID(int id)
{
    for (int i = 0; i < blockCount; i++)
    {
        if (blocks[i].allocated &&
            blocks[i].id == id)
        {
            return i;
        }
    }

    return -1;
}


// ALLOCATE MEMORY
int allocateMemory(int size)
{
    if (size <= 0 || size > poolSize)
    {
        printf("Invalid allocation size.\n");
        return 0;
    }

    int index = -1;

    if (strategy == 1)
        index = findFirstFit(size);

    else if (strategy == 2)
        index = findBestFit(size);

    else if (strategy == 3)
        index = findWorstFit(size);

    if (index == -1)
    {
        printf("Allocation failed: no suitable block.\n");

        displayFragmentation(size);

        return 0;
    }

    int remaining = blocks[index].size - size;

    // If splitting is required, grow metadata first
    if (remaining > 0)
    {
        if (!ensureCapacity(blockCount + 1))
        {
            printf("Metadata allocation failed.\n");
            return 0;
        }

        // Shift blocks to create a new entry
        for (int i = blockCount; i > index + 1; i--)
        {
            blocks[i] = blocks[i - 1];
        }

        // Create remaining free block
        blocks[index + 1].start =
            blocks[index].start + size;

        blocks[index + 1].size = remaining;
        blocks[index + 1].id = 0;
        blocks[index + 1].allocated = 0;

        blockCount++;
    }

    blocks[index].size = size;
    blocks[index].id = nextID;
    blocks[index].allocated = 1;

    printf("Allocation successful!\n");
    printf("ID: %d, Start: %d, Size: %d\n",
           nextID, blocks[index].start, size);

    nextID++;

    return 1;
}


// MERGE ADJACENT FREE BLOCKS
void mergeFreeBlocks(void)
{
    int i = 0;

    while (i < blockCount - 1)
    {
        if (!blocks[i].allocated &&
            !blocks[i + 1].allocated)
        {
            blocks[i].size += blocks[i + 1].size;

            // Remove the second block
            for (int j = i + 1; j < blockCount - 1; j++)
            {
                blocks[j] = blocks[j + 1];
            }

            blockCount--;
        }
        else
        {
            i++;
        }
    }
}


// RELEASE MEMORY
void releaseMemory(int id)
{
    int index = findBlockByID(id);

    if (index == -1)
    {
        printf("Invalid ID or allocation already released.\n");
        return;
    }

    blocks[index].allocated = 0;
    blocks[index].id = 0;

    printf("Allocation ID %d released successfully.\n", id);

    mergeFreeBlocks();
}


// RESIZE MEMORY
void resizeMemory(int id, int newSize)
{
    int index = findBlockByID(id);

    if (index == -1)
    {
        printf("Invalid allocation ID.\n");
        return;
    }

    if (newSize <= 0 || newSize > poolSize)
    {
        printf("Invalid new size.\n");
        return;
    }

    int oldSize = blocks[index].size;

    // No change required
    if (newSize == oldSize)
    {
        printf("Allocation already has this size.\n");
        return;
    }

    // SHRINKING
    if (newSize < oldSize)
    {
        if (!ensureCapacity(blockCount + 1))
        {
            printf("Metadata allocation failed.\n");
            return;
        }

        // Shift blocks to create free block
        for (int i = blockCount; i > index + 1; i--)
        {
            blocks[i] = blocks[i - 1];
        }

        blocks[index + 1].start =
            blocks[index].start + newSize;

        blocks[index + 1].size = oldSize - newSize;
        blocks[index + 1].id = 0;
        blocks[index + 1].allocated = 0;

        blocks[index].size = newSize;

        blockCount++;

        mergeFreeBlocks();

        printf("Allocation shrunk successfully.\n");
        return;
    }

    // EXPANDING
    int extra = newSize - oldSize;

    // Check adjacent block
    if (index + 1 < blockCount &&
        !blocks[index + 1].allocated &&
        blocks[index + 1].size >= extra)
    {
        blocks[index].size = newSize;

        blocks[index + 1].start += extra;
        blocks[index + 1].size -= extra;

        // Entire adjacent free block consumed
        if (blocks[index + 1].size == 0)
        {
            for (int i = index + 1; i < blockCount - 1; i++)
            {
                blocks[i] = blocks[i + 1];
            }

            blockCount--;
        }

        printf("Allocation expanded in place.\n");
        return;
    }

    // MOVE TO ANOTHER FREE BLOCK
    int newIndex = -1;

    if (strategy == 1)
        newIndex = findFirstFit(newSize);

    else if (strategy == 2)
        newIndex = findBestFit(newSize);

    else if (strategy == 3)
        newIndex = findWorstFit(newSize);

    // Do not select the current allocation
    if (newIndex == -1 || newIndex == index)
    {
        printf("Resize failed: no suitable free block.\n");
        return;
    }

    // Allocate new metadata capacity before modifying
    int splitNeeded = blocks[newIndex].size > newSize;

    if (splitNeeded && !ensureCapacity(blockCount + 1))
    {
        printf("Metadata allocation failed.\n");
        return;
    }

    int oldStart = blocks[index].start;
    int newStart = blocks[newIndex].start;

    // Save the original data temporarily
    unsigned char *temp = malloc(oldSize);

    if (temp == NULL)
    {
        printf("Temporary memory allocation failed.\n");
        return;
    }

    memcpy(temp, memory + oldStart, oldSize);

    int remaining = blocks[newIndex].size - newSize;

    // Split destination block if necessary
    if (remaining > 0)
    {
        for (int i = blockCount; i > newIndex + 1; i--)
        {
            blocks[i] = blocks[i - 1];
        }

        blocks[newIndex + 1].start = newStart + newSize;
        blocks[newIndex + 1].size = remaining;
        blocks[newIndex + 1].id = 0;
        blocks[newIndex + 1].allocated = 0;

        blockCount++;
    }

    // Mark destination as allocated
    blocks[newIndex].size = newSize;
    blocks[newIndex].id = id;
    blocks[newIndex].allocated = 1;

    // Copy old data to new location
    memcpy(memory + newStart, temp, oldSize);

    free(temp);

    // Release old allocation
    index = findBlockByID(id);

    // The search may return the new allocation.
    // Find the old location using its original offset.
    for (int i = 0; i < blockCount; i++)
    {
        if (blocks[i].start == oldStart &&
            blocks[i].allocated &&
            blocks[i].id == id)
        {
            blocks[i].allocated = 0;
            blocks[i].id = 0;
            break;
        }
    }

    mergeFreeBlocks();

    printf("Allocation moved and resized successfully.\n");
}


// DISPLAY MEMORY MAP
void displayMemoryMap(void)
{
    printf("\nMemory Map (%d bytes):\n", poolSize);

    for (int i = 0; i < blockCount; i++)
    {
        if (blocks[i].allocated)
        {
            printf("[Allocated #%d: %d bytes, Start: %d]\n",
                   blocks[i].id,
                   blocks[i].size,
                   blocks[i].start);
        }
        else
        {
            printf("[Free: %d bytes, Start: %d]\n",
                   blocks[i].size,
                   blocks[i].start);
        }
    }
}


// DISPLAY MEMORY STATISTICS
void displayStatistics(void)
{
    int allocatedBytes = 0;
    int freeBytes = 0;

    int allocatedBlocks = 0;
    int freeBlocks = 0;

    int largestFree = 0;

    for (int i = 0; i < blockCount; i++)
    {
        if (blocks[i].allocated)
        {
            allocatedBytes += blocks[i].size;
            allocatedBlocks++;
        }
        else
        {
            freeBytes += blocks[i].size;
            freeBlocks++;

            if (blocks[i].size > largestFree)
                largestFree = blocks[i].size;
        }
    }

    printf("\n----- Memory Statistics -----\n");

    printf("Total memory: %d bytes\n", poolSize);
    printf("Allocated memory: %d bytes\n", allocatedBytes);
    printf("Free memory: %d bytes\n", freeBytes);

    printf("Allocated blocks: %d\n", allocatedBlocks);
    printf("Free blocks: %d\n", freeBlocks);

    printf("Largest free block: %d bytes\n", largestFree);
}


// DETECT EXTERNAL FRAGMENTATION
void displayFragmentation(int size)
{
    int totalFree = 0;

    printf("\nAvailable free blocks:\n");

    for (int i = 0; i < blockCount; i++)
    {
        if (!blocks[i].allocated)
        {
            printf("%d bytes ", blocks[i].size);
            totalFree += blocks[i].size;
        }
    }

    printf("\nTotal free memory: %d bytes\n", totalFree);

    if (totalFree >= size)
    {
        printf("External fragmentation detected!\n");
        printf("Total free memory is sufficient, but no "
               "individual block is large enough.\n");
    }
    else
    {
        printf("Insufficient total free memory.\n");
    }
}


// CHANGE ALLOCATION STRATEGY
void changeStrategy(void)
{
    int choice;

    printf("\nSelect strategy:\n");
    printf("1. First Fit\n");
    printf("2. Best Fit\n");
    printf("3. Worst Fit\n");

    printf("Choice: ");
    scanf("%d", &choice);

    if (choice >= 1 && choice <= 3)
    {
        strategy = choice;

        printf("Strategy changed successfully.\n");
    }
    else
    {
        printf("Invalid strategy.\n");
    }
}


// MAIN FUNCTION
int main(void)
{
    printf("Enter memory pool size: ");

    if (scanf("%d", &poolSize) != 1 ||
        poolSize < MIN_POOL_SIZE ||
        poolSize > MAX_POOL_SIZE)
    {
        printf("Invalid pool size.\n");
        return 1;
    }

    // Allocate actual memory pool
    memory = malloc(poolSize);

    if (memory == NULL)
    {
        printf("Memory pool allocation failed.\n");
        return 1;
    }

    // Allocate metadata array
    blocks = malloc(capacity * sizeof(Block));

    if (blocks == NULL)
    {
        printf("Metadata allocation failed.\n");
        free(memory);
        return 1;
    }

    // Initially, the entire pool is one free block
    blocks[0].start = 0;
    blocks[0].size = poolSize;
    blocks[0].id = 0;
    blocks[0].allocated = 0;

    printf("\nSelect allocation strategy:\n");
    printf("1. First Fit\n");
    printf("2. Best Fit\n");
    printf("3. Worst Fit\n");

    printf("Choice: ");

    if (scanf("%d", &strategy) != 1 ||
        strategy < 1 || strategy > 3)
    {
        printf("Invalid strategy.\n");
        free(memory);
        free(blocks);
        return 1;
    }

    int choice;
    int size;
    int id;

    do
    {
        printf("\n========== SMART MEMORY MANAGER ==========\n");

        printf("1. Allocate memory\n");
        printf("2. Release memory\n");
        printf("3. Resize allocation\n");
        printf("4. Display memory map\n");
        printf("5. Display memory statistics\n");
        printf("6. Change allocation strategy\n");
        printf("7. Exit\n");

        printf("Choice: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input.\n");
            break;
        }

        switch (choice)
        {
            case 1:
                printf("Requested size: ");

                if (scanf("%d", &size) != 1)
                {
                    printf("Invalid input.\n");
                    break;
                }

                allocateMemory(size);
                break;

            case 2:
                printf("Allocation ID: ");

                if (scanf("%d", &id) != 1)
                {
                    printf("Invalid input.\n");
                    break;
                }

                releaseMemory(id);
                break;

            case 3:
                printf("Allocation ID: ");

                if (scanf("%d", &id) != 1)
                {
                    printf("Invalid input.\n");
                    break;
                }

                printf("New size: ");

                if (scanf("%d", &size) != 1)
                {
                    printf("Invalid input.\n");
                    break;
                }

                resizeMemory(id, size);
                break;

            case 4:
                displayMemoryMap();
                break;

            case 5:
                displayStatistics();
                break;

            case 6:
                changeStrategy();
                break;

            case 7:
                printf("Exiting Memory Manager.\n");
                break;

            default:
                printf("Invalid choice.\n");
        }

    } while (choice != 7);

    // Release all dynamically allocated memory
    free(memory);
    free(blocks);

    return 0;
}
