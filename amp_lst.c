#include "amp.h"

/*
    Adds an item to the linked list.
*/
int list_add(FILE *file, int index)
{
    struct Stream *item = (struct Stream*)malloc(sizeof(struct Stream));
    
    if (item == NULL)
    {
        return -1;
    }
    
    item->file = file;
    item->next = NULL;
    item->index = index;
    item->status = 0;
    
    if (head == NULL)
    {
        head = item;
        tail = item;
    }
    else
    {
        tail->next = item;
        tail = item;
    }
    
    return 0;
}

/*
    Clears the linked list and frees all allocated memory.
*/
void list_clear()
{
    struct Stream *item = head;
    
    while (item != NULL)
    {
        if (item->file != NULL)
        {
            fclose(item->file);
            item->file = NULL;
        }
        
        free(item);
        
        item = item->next;
    }
}

/*
    Tries to open all streams up to LAST_FD for writing and adds them to the list.
*/
int open_streams()
{
    int fd = 0;
    int added = 0;
    int result = 0;
    FILE *file = NULL;
    
    for (fd = 1; fd <= args.last_fd; fd++)
    {
        if (fd == 1 && args.ign_stdout == 1)
        {
            continue;
        }
        
        if (fd == 2 && args.use_stderr == 0)
        {
            continue;
        }
        
        file = fdopen(fd, "wb");
        
        if (file == NULL)
        {
            continue;
        }
        
        added = list_add(file, fd);
        
        if (added != 0)
        {
            continue;
        }
        
        result++;
    }
    
    return result;
}
