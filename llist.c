// Copyright 2017 Matthias Schwarz
// License: MIT

#include "amp.h"

/*
    Add an item to the linked list, or create the list, if it doesn't exist.
*/
int list_add(FILE *file, int index)
{
    struct stream *item = (struct stream*)malloc(sizeof(struct stream));
    
    if (item == NULL)
    {
        log_error("Failed to allocate memory for stream.\n");
        
        return 1;
    }
    
    item->file = file;
    item->next = NULL;
    item->index = index;
    item->state = 0;
    
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
    struct stream *item = head;
    
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
    Try to open all streams up to last_fd for writing and add them to the list.
*/
int open_streams()
{
    FILE *file = NULL;
    int fd = 0;
    int added = 0;
    int streams = 0;
    
    // First, add stdout, if desired
    if (args.ign_stdout == 0)
    {
        added = list_add(stdout, 1);
        
        if (added != 0)
        {
            log_error("Failed to add stdout to list.\n");
        }
        
        streams++;
    }
    
    // Cycle through file descriptors upto last_fd
    for (fd = MIN_FD; fd <= args.last_fd; fd++)
    {
        // Try to open the file descriptor
        file = fdopen(fd, "wb");
        
        if (file == NULL)
        {
            // Failed to open stream. Try next
            continue;
        }
        
        added = list_add(file, fd);
        
        if (added != 0)
        {
            log_error("Failed to add stream %d to list.\n", fd);
            
            continue;
        }
        
        streams++;
    }
    
    return streams;
}
