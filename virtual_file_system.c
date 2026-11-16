
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BLOCK_SIZE 512
#define NUM_BLOCKS 1024
#define MAX_NAME 50
#define INPUT_SIZE 8192  //2^13 


// STRUCTURE 1: Free Block
// Doubly Linked List

typedef struct FreeBlock
{
    int index;
    struct FreeBlock *next;
    struct FreeBlock *prev;
} FreeBlock;


// STRUCTURE 2: File Node
// Circular Doubly Linked List

typedef struct FileNode
{
    char name[MAX_NAME + 1];

    int isDirectory;

    struct FileNode *parent;
    struct FileNode *children;

    int *blockPointers;
    int blockCount;
    int fileSize;

    struct FileNode *next;
    struct FileNode *prev;

} FileNode;


// GLOBAL VARIABLES

char virtualDisk[NUM_BLOCKS][BLOCK_SIZE];

FreeBlock *freeHead = NULL;
FreeBlock *freeTail = NULL;

FileNode *root = NULL;
FileNode *cwd = NULL;

int freeCount = 0;


// FUNCTION DECLARATIONS

int initializeSystem();
int addFreeBlock(int index);
int allocateBlock();

FileNode *createNode(const char *name, int isDirectory);
FileNode *findChild(FileNode *directory, const char *name);
FileNode *resolveDirectory(const char *path);

void insertChild(FileNode *parent, FileNode *node);
void removeChild(FileNode *parent, FileNode *node);

void mkdirCommand(const char *name);
void createCommand(const char *name);
void writeCommand(char *arguments);
void readCommand(const char *name);
void deleteCommand(const char *name);
void rmdirCommand(const char *name);

void lsCommand(const char *path);
void cdCommand(const char *path);
void pwdCommand();
void dfCommand();

void freeFileBlocks(FileNode *file);
void freeNodeTree(FileNode *node);
void cleanupSystem();

int getName(const char *arguments, char *name);
void processCommand(char *input);


// INITIALIZE FREE BLOCK LIST

int addFreeBlock(int index)
{
    FreeBlock *newBlock = malloc(sizeof(FreeBlock));

    if (newBlock == NULL)
        return 0;

    newBlock->index = index;
    newBlock->next = NULL;
    newBlock->prev = freeTail;

    if (freeHead == NULL)
    {
        freeHead = newBlock;
        freeTail = newBlock;
    }
    else
    {
        freeTail->next = newBlock;
        freeTail = newBlock;
    }

    freeCount++;

    return 1;
}


// ALLOCATE BLOCK FROM FREE LIST

int allocateBlock()
{
    if (freeHead == NULL)
        return -1;

    FreeBlock *temp = freeHead;
    int index = temp->index;

    freeHead = freeHead->next;

    if (freeHead == NULL)
        freeTail = NULL;
    else
        freeHead->prev = NULL;

    free(temp);

    freeCount--;

    return index;
}


// CREATE A FILE OR DIRECTORY NODE

FileNode *createNode(const char *name, int isDirectory)
{
    FileNode *newNode = malloc(sizeof(FileNode));

    if (newNode == NULL)
        return NULL;

    strcpy(newNode->name, name);

    newNode->isDirectory = isDirectory;

    newNode->parent = NULL;
    newNode->children = NULL;

    newNode->blockPointers = NULL;
    newNode->blockCount = 0;
    newNode->fileSize = 0;

    newNode->next = NULL;
    newNode->prev = NULL;

    return newNode;
}


// INITIALIZE VIRTUAL FILE SYSTEM

int initializeSystem()
{
    for (int i = 0; i < NUM_BLOCKS; i++)
    {
        if (!addFreeBlock(i))
            return 0;
    }

    root = createNode("/", 1);

    if (root == NULL)
        return 0;

    cwd = root;

    return 1;
}


// INSERT NODE INTO CIRCULAR LIST

void insertChild(FileNode *parent, FileNode *node)
{
    node->parent = parent;

    if (parent->children == NULL)
    {
        node->next = node;
        node->prev = node;

        parent->children = node;
    }
    else
    {
        FileNode *head = parent->children;
        FileNode *tail = head->prev;

        tail->next = node;
        node->prev = tail;

        node->next = head;
        head->prev = node;
    }
}


// REMOVE NODE FROM CIRCULAR LIST

void removeChild(FileNode *parent, FileNode *node)
{
    if (node->next == node)
    {
        parent->children = NULL;
    }
    else
    {
        node->prev->next = node->next;
        node->next->prev = node->prev;

        if (parent->children == node)
            parent->children = node->next;
    }

    node->next = NULL;
    node->prev = NULL;
    node->parent = NULL;
}


// FIND CHILD BY NAME

FileNode *findChild(FileNode *directory, const char *name)
{
    if (directory == NULL || directory->children == NULL)
        return NULL;

    FileNode *temp = directory->children;

    do
    {
        if (strcmp(temp->name, name) == 0)
            return temp;

        temp = temp->next;

    } while (temp != directory->children);

    return NULL;
}


// CHECK AND EXTRACT A NAME

int getName(const char *arguments, char *name)
{
    while (isspace((unsigned char)*arguments))
        arguments++;

    if (*arguments == '\0')
        return 0;

    int i = 0;

    while (arguments[i] != '\0' &&
           !isspace((unsigned char)arguments[i]))
    {
        if (i >= MAX_NAME)
            return 0;

        name[i] = arguments[i];
        i++;
    }

    name[i] = '\0';

    return 1;
}


// MKDIR COMMAND

void mkdirCommand(const char *name)
{
    if (strlen(name) == 0)
        return;

    if (findChild(cwd, name) != NULL)
    {
        printf("An entry with this name already exists.\n");
        return;
    }

    FileNode *newDir = createNode(name, 1);

    if (newDir == NULL)
    {
        printf("Memory allocation failed.\n");
        return;
    }

    insertChild(cwd, newDir);

    printf("Directory '%s' created successfully.\n", name);
}


// CREATE FILE COMMAND

void createCommand(const char *name)
{
    if (findChild(cwd, name) != NULL)
    {
        printf("An entry with this name already exists.\n");
        return;
    }

    FileNode *newFile = createNode(name, 0);

    if (newFile == NULL)
    {
        printf("Memory allocation failed.\n");
        return;
    }

    insertChild(cwd, newFile);

    printf("File '%s' created successfully.\n", name);
}


// WRITE CONTENT INTO FILE

void writeCommand(char *arguments)
{
    char name[MAX_NAME + 1];
    int consumed = 0;

    while (isspace((unsigned char)*arguments))
        arguments++;

    int i = 0;

    while (arguments[i] != '\0' &&
           !isspace((unsigned char)arguments[i]))
    {
        if (i >= MAX_NAME)
        {
            printf("Invalid filename.\n");
            return;
        }

        name[i] = arguments[i];
        i++;
    }

    if (i == 0)
    {
        printf("Usage: write filename \"content\"\n");
        return;
    }

    name[i] = '\0';
    consumed = i;

    arguments += consumed;

    while (isspace((unsigned char)*arguments))
        arguments++;

    if (*arguments == '"')
    {
        arguments++;

        char *end = strrchr(arguments, '"');

        if (end == NULL)
        {
            printf("Missing closing quotation mark.\n");
            return;
        }

        *end = '\0';
    }

    FileNode *file = findChild(cwd, name);

    if (file == NULL || file->isDirectory)
    {
        printf("File not found.\n");
        return;
    }

    int size = strlen(arguments);

    int required = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;

    // Check available capacity, including old blocks.
    if (required > freeCount + file->blockCount)
    {
        printf("Disk full. Cannot write file.\n");
        return;
    }

    int *newPointers = NULL;

    if (required > 0)
    {
        newPointers = malloc(required * sizeof(int));

        if (newPointers == NULL)
        {
            printf("Memory allocation failed.\n");
            return;
        }
    }

    // Release previous blocks.
    freeFileBlocks(file);

    // Allocate new blocks.
    for (int j = 0; j < required; j++)
    {
        newPointers[j] = allocateBlock();

        if (newPointers[j] == -1)
        {
            free(newPointers);
            printf("Block allocation failed.\n");
            return;
        }
    }

    file->blockPointers = newPointers;
    file->blockCount = required;
    file->fileSize = size;

    // Copy content into virtual disk.
    for (int j = 0; j < required; j++)
    {
        int start = j * BLOCK_SIZE;
        int remaining = size - start;

        int bytes = remaining < BLOCK_SIZE
                        ? remaining
                        : BLOCK_SIZE;

        memcpy(virtualDisk[newPointers[j]],
               arguments + start,
               bytes);
    }

    printf("Data written successfully (size=%d bytes).\n", size);
}


// READ FILE COMMAND

void readCommand(const char *name)
{
    FileNode *file = findChild(cwd, name);

    if (file == NULL || file->isDirectory)
    {
        printf("File not found.\n");
        return;
    }

    if (file->fileSize == 0)
    {
        printf("File is empty.\n");
        return;
    }

    int remaining = file->fileSize;

    for (int i = 0; i < file->blockCount; i++)
    {
        int bytes = remaining < BLOCK_SIZE
                        ? remaining
                        : BLOCK_SIZE;

        fwrite(virtualDisk[file->blockPointers[i]],
               1,
               bytes,
               stdout);

        remaining -= bytes;
    }

    printf("\n");
}


// RETURN FILE BLOCKS TO FREE LIST

void freeFileBlocks(FileNode *file)
{
    for (int i = 0; i < file->blockCount; i++)
    {
        int index = file->blockPointers[i];

        if (!addFreeBlock(index))
        {
            printf("Warning: Unable to release block %d.\n", index);
        }
    }

    free(file->blockPointers);

    file->blockPointers = NULL;
    file->blockCount = 0;
    file->fileSize = 0;
}


// DELETE FILE COMMAND

void deleteCommand(const char *name)
{
    FileNode *file = findChild(cwd, name);

    if (file == NULL || file->isDirectory)
    {
        printf("File not found.\n");
        return;
    }

    freeFileBlocks(file);

    removeChild(cwd, file);

    free(file);

    printf("File deleted successfully.\n");
}


// RMDIR COMMAND

void rmdirCommand(const char *name)
{
    FileNode *directory = findChild(cwd, name);

    if (directory == NULL || !directory->isDirectory)
    {
        printf("Directory not found.\n");
        return;
    }

    if (directory->children != NULL)
    {
        printf("Directory is not empty. Cannot remove.\n");
        return;
    }

    removeChild(cwd, directory);

    free(directory);

    printf("Directory removed successfully.\n");
}


// RESOLVE DIRECTORY PATH

FileNode *resolveDirectory(const char *path)
{
    if (path == NULL || *path == '\0')
        return cwd;

    char copy[INPUT_SIZE];

    if (strlen(path) >= sizeof(copy))
        return NULL;

    strcpy(copy, path);

    FileNode *current;

    if (copy[0] == '/')
        current = root;
    else
        current = cwd;

    char *token = strtok(copy, "/");

    while (token != NULL)
    {
        if (strcmp(token, ".") == 0)
        {
            // Stay in current directory.
        }
        else if (strcmp(token, "..") == 0)
        {
            if (current->parent != NULL)
                current = current->parent;
        }
        else
        {
            FileNode *next = findChild(current, token);

            if (next == NULL || !next->isDirectory)
                return NULL;

            current = next;
        }

        token = strtok(NULL, "/");
    }

    return current;
}


// LS COMMAND

void lsCommand(const char *path)
{
    FileNode *directory;

    if (path == NULL || *path == '\0')
        directory = cwd;
    else
        directory = resolveDirectory(path);

    if (directory == NULL)
    {
        printf("Directory not found.\n");
        return;
    }

    if (directory->children == NULL)
    {
        printf("(empty)\n");
        return;
    }

    FileNode *temp = directory->children;

    do
    {
        if (temp->isDirectory)
            printf("%s/\n", temp->name);
        else
            printf("%s\n", temp->name);

        temp = temp->next;

    } while (temp != directory->children);
}


// CD COMMAND

void cdCommand(const char *path)
{
    FileNode *directory = resolveDirectory(path);

    if (directory == NULL)
    {
        printf("Directory not found.\n");
        return;
    }

    cwd = directory;

    printf("Moved to ");

    // Display new path.
    FileNode *stack[INPUT_SIZE];
    int count = 0;

    FileNode *temp = cwd;

    while (temp != NULL && count < INPUT_SIZE)
    {
        stack[count++] = temp;
        temp = temp->parent;
    }

    for (int i = count - 1; i >= 0; i--)
    {
        if (stack[i] == root)
            printf("/");
        else
            printf("%s%s",
                   stack[i]->name,
                   i == 0 ? "" : "/");
    }

    printf("\n");
}


// PWD COMMAND

void pwdCommand()
{
    FileNode *stack[INPUT_SIZE];
    int count = 0;

    FileNode *temp = cwd;

    while (temp != NULL && count < INPUT_SIZE)
    {
        stack[count++] = temp;
        temp = temp->parent;
    }

    for (int i = count - 1; i >= 0; i--)
    {
        if (stack[i] == root)
            printf("/");
        else
            printf("%s%s",
                   stack[i]->name,
                   i == 0 ? "" : "/");
    }

    printf("\n");
}

// DF COMMAND

void dfCommand()
{
    int used = NUM_BLOCKS - freeCount;

    double percentage =
        (double)used / NUM_BLOCKS * 100.0;

    printf("Total Blocks: %d\n", NUM_BLOCKS);
    printf("Used Blocks: %d\n", used);
    printf("Free Blocks: %d\n", freeCount);
    printf("Disk Usage: %.2f%%\n", percentage);
}


// FREE DIRECTORY TREE

void freeNodeTree(FileNode *node)
{
    if (node == NULL)
        return;

    if (node->children != NULL)
    {
        FileNode *head = node->children;
        FileNode *current = head;

        do
        {
            FileNode *next = current->next;

            freeNodeTree(current);

            current = next;

        } while (current != head);
    }

    free(node->blockPointers);
    free(node);
}


// CLEANUP ALL MEMORY

void cleanupSystem()
{
    // Release all directory and file nodes.
    freeNodeTree(root);

    root = NULL;
    cwd = NULL;

    // Release remaining free block nodes.
    while (freeHead != NULL)
    {
        FreeBlock *temp = freeHead;

        freeHead = freeHead->next;

        free(temp);
    }

    freeTail = NULL;
    freeCount = 0;
}


// PROCESS USER COMMANDS

void processCommand(char *input)
{
    char command[20];
    char name[MAX_NAME + 1];

    int i = 0;

    while (isspace((unsigned char)*input))
        input++;

    while (input[i] != '\0' &&
           !isspace((unsigned char)input[i]) &&
           i < 19)
    {
        command[i] = input[i];
        i++;
    }

    command[i] = '\0';

    char *arguments = input + i;

    while (isspace((unsigned char)*arguments))
        arguments++;

    if (strcmp(command, "mkdir") == 0)
    {
        if (getName(arguments, name))
            mkdirCommand(name);
        else
            printf("Usage: mkdir directory_name\n");
    }
    else if (strcmp(command, "create") == 0)
    {
        if (getName(arguments, name))
            createCommand(name);
        else
            printf("Usage: create filename\n");
    }
    else if (strcmp(command, "write") == 0)
    {
        writeCommand(arguments);
    }
    else if (strcmp(command, "read") == 0)
    {
        if (getName(arguments, name))
            readCommand(name);
        else
            printf("Usage: read filename\n");
    }
    else if (strcmp(command, "delete") == 0)
    {
        if (getName(arguments, name))
            deleteCommand(name);
        else
            printf("Usage: delete filename\n");
    }
    else if (strcmp(command, "rmdir") == 0)
    {
        if (getName(arguments, name))
            rmdirCommand(name);
        else
            printf("Usage: rmdir directory_name\n");
    }
    else if (strcmp(command, "ls") == 0)
    {
        lsCommand(arguments);
    }
    else if (strcmp(command, "cd") == 0)
    {
        if (*arguments != '\0')
            cdCommand(arguments);
        else
            printf("Usage: cd directory_name\n");
    }
    else if (strcmp(command, "pwd") == 0)
    {
        pwdCommand();
    }
    else if (strcmp(command, "df") == 0)
    {
        dfCommand();
    }
    else if (strcmp(command, "exit") == 0)
    {
        return;
    }
    else if (*command == '\0')
    {
        return;
    }
    else
    {
        printf("Invalid command. Try again.\n");
    }
}


// MAIN FUNCTION

int main()
{
    if (!initializeSystem())
    {
        printf("Virtual File System initialization failed.\n");
        cleanupSystem();
        return 1;
    }

    printf("Compact VFS - ready. Type 'exit' to quit.\n");

    char input[INPUT_SIZE];

    while (1)
    {
        // Display current directory.
        pwdCommand();
        printf("> ");

        if (fgets(input, sizeof(input), stdin) == NULL)
            break;

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0)
            break;

        processCommand(input);
    }

    cleanupSystem();

    printf("Memory released. Exiting program...\n");

    return 0;
}
