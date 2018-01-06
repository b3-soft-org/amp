// Copyright 2018 Matthias Schwarz
// License: MIT



/*
    Core function to read from stdin and write to the output streams.
*/



#include "amp.h"



struct stream *head;
struct stream *tail;

/*
    Reads from stdin and writes the buffer to the output streams
*/
int amplify(int streams)
{
    struct stream *current = head;
    unsigned char *buffer = NULL;
    unsigned long long bufsiz = 0;
    size_t read = 0;
    size_t written = 0;
    int flushed = 0;
    int any_stream = 1;
    int done = 0;
    int pos = 0;
    int lines = 0;
    
    unsigned long long total = 0;
    
    unsigned long long s1 = 0;
    unsigned long long s2 = 0;
    clock_t t0 = 0;
    clock_t t1 = 0;
    clock_t t2 = 0;
    double elapsed_time = 0.0;
    double total_elapsed_time = 0.0;
    
    bufsiz = args.buffer_size;
    if (args.count != DEF_COUNT && args.count < bufsiz)
    {
        // It makes no sense to read more than 'count' bytes into the buffer,
        // if the given buffer size is larger than 'count'.
        bufsiz = args.count;
    }
    
    buffer = (unsigned char*)(calloc(bufsiz, sizeof(unsigned char)));
    
    if (buffer == NULL)
    {
        log_error("Failed to allocate %lld bytes for buffer.\n", bufsiz);
        
        return 1;
    }
    
    t0 = clock();
    t1 = clock();
    s1 = 0;
    
    while (done == 0 && any_stream == 1 && (read = fread(buffer, sizeof(unsigned char), bufsiz, stdin)) > 0)
    {
        current = head;
        
        if (args.count > 0)
        {
            // Determine whether we read the requested number of bytes
            if (total + read >= args.count)
            {
                read = args.count - total;
                done = 1;
            }
        }
        
        if (args.lines > 0)
        {
            // Determine whether we read the requested number of lines
            for (pos = 0; pos < read; pos++)
            {
                if (buffer[pos] == '\n')
                {
                    lines++;
                }
                
                if (lines >= args.lines)
                {
                    read = pos + 1;
                    done = 1;
                    
                    break;
                }
            }
        }
        
        total += read;
        
        while (current != NULL)
        {
            if (current->state != 0)
            {
                goto next;
            }
            
            written = fwrite(buffer, sizeof(unsigned char), read, current->file);
            
            if (read != written)
            {
                current->state = ERR_IMBALANCE;
                log_error("Read/write imbalance detected for stream %d (Read: %d, Written: %d). Ignoring...\n", current->index, read, written);
            }
            
            if (feof(current->file) != 0)
            {
                current->state = ERR_EOF;
                log_error("Reached the end of stream %d. Ignoring...\n", current->index);
                clearerr(current->file);
            }
            
            if (ferror(current->file) != 0)
            {
                current->state = ERR_WRITE;
                log_error("Failed to write to stream %d: %m (%d). Ignoring...\n", current->index, errno);
                clearerr(current->file);
            }
            
            flushed = fflush(current->file);
            
            if (flushed != 0)
            {
                current->state = ERR_FLUSH;
                log_error("Failed to flush stream %d: %m (%d). Ignoring...\n", current->index, errno);
                clearerr(current->file);
            }
            
next:
            current = current->next;
        }
        
        if (args.status_type == STATUS_TYPE_SIZE)
        {
            s2 = total;
            
            if (s2 - s1 >= args.status_rate)
            {
                s1 = total;
                
                elapsed_time = (double)(t2 - t0) / CLOCKS_PER_SEC;
                
                print_status(total, elapsed_time);
            }
            
            t2 = clock();
        }
        else if (args.status_type == STATUS_TYPE_TIME)
        {
            t2 = clock();
            
            elapsed_time = (double)(t2 - t1) / CLOCKS_PER_SEC;
            total_elapsed_time = (double)(t2 - t0) / CLOCKS_PER_SEC;
            
            if (elapsed_time >= args.status_rate)
            {
                print_status(total, total_elapsed_time);
                
                t1 = clock();
            }
        }
        
        any_stream = has_any_stream();
        
        if (any_stream == 0)
        {
            log_info("No more output streams to write to. Exiting...\n");
            break;
        }
    }
    
    t2 = clock();
    
    log_info("Runtime: %.2f s\n", (double)(t2 - t1) / CLOCKS_PER_SEC);
    
    return 0;
}
