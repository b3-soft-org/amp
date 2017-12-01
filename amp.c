#include "amp.h"

struct Stream *head;
struct Stream *tail;

void print_status(unsigned long long total, double elapsed_time)
{
    double value = 0;
    char suffix[] = "BKMGT";
    int i = 0;
    
    for (i = 0; i < 5 && total >= 1024; i++, total /= 1024)
    {
        value = total / 1024.0;
    }
    
    log_info("%.2f %c, %.2f s (%.2f %c/s)\n", value, suffix[i], elapsed_time, value / elapsed_time, suffix[i]);
}

/*
    Reads from stdin and writes the buffer to the output streams
*/
void amplify(int streams)
{
    struct Stream *current = head;
    unsigned char buffer[args.buffer_size];
    size_t read = 0;
    size_t written = 0;
    int flushed = 0;
    int any_stream = 1;
    
    unsigned long long s1 = 0;
    unsigned long long s2 = 0;
    clock_t t0 = 0;
    clock_t t1 = 0;
    clock_t t2 = 0;
    
    unsigned long long total = 0;
    
    t0 = clock();
    t1 = clock();
    s1 = 0;
    
    while (any_stream == 1 && (read = fread(buffer, sizeof(unsigned char), args.buffer_size, stdin)) > 0)
    {
        current = head;
        
        total += read;
        
        if (args.status_type == STATUS_TYPE_SIZE)
        {
            s2 = total;
            
            if (s2 - s1 >= args.status_rate)
            {
                s1 = total;
                
                print_status(total, (double)(t2 - t0) / CLOCKS_PER_SEC);
            }
            t2 = clock();
        }
        
        if (args.status_type == STATUS_TYPE_TIME)
        {
            t2 = clock();
            
            if ((double)(t2 - t1) / CLOCKS_PER_SEC >= args.status_rate)
            {
                print_status(total, (double)(t2 - t0) / CLOCKS_PER_SEC);
                
                t1 = clock();
            }
        }
        
        while (current != NULL)
        {
            if (current->status != 0)
            {
                goto next;
            }
            
            written = fwrite(buffer, sizeof(unsigned char), read, current->file);
            
            if (read != written)
            {
                current->status = ERR_IMBALANCE;
                log_error("Read/write imbalance detected for stream %d (Read: %d, Written: %d). Ignoring...\n", current->index, read, written);
            }
            
            if (feof(current->file) != 0)
            {
                current->status = ERR_EOF;
                log_error("Reached the end of stream %d. Ignoring...\n", current->index);
                clearerr(current->file);
            }
            
            if (ferror(current->file) != 0)
            {
                current->status = ERR_WRITE;
                log_error("Failed to write to stream %d: %m (%d). Ignoring...\n", current->index, errno);
                clearerr(current->file);
            }
            
            flushed = fflush(current->file);
            
            if (flushed != 0)
            {
                current->status = ERR_FLUSH;
                log_error("Failed to flush stream %d: %m (%d). Ignoring...\n", current->index, errno);
                clearerr(current->file);
            }
            
next:
            current = current->next;
        }
        
        // TODO: function
        any_stream = 0;
        current = head;
        
        while (current != NULL) {
            if (current->status == 0) {
                any_stream = 1;
            }
            current = current->next;
        }
        
        if (any_stream == 0) {
            log_info("No more output streams to write to. Exiting...\n");
            break;
        }
    }
    
    t2 = clock();
    
    log_info("Runtime: %f\n", (double)(t2 - t1) / CLOCKS_PER_SEC);
}

int main(int argc, char **argv) {
    int err = 0;
    int streams = 0;
    
    int buffer_size_result = 0;
    int status_result = 0;
    int last_fd_result = 0;
    
    args.use_stderr = DEF_USE_STDERR;
    args.use_syslog = DEF_USE_SYSLOG;
    args.ign_stdout = DEF_IGN_STDOUT;
    args.buffer_size = DEF_BUFFER_SIZE;
    args.status_rate = DEF_STATUS_RATE;
    args.status_type = DEF_STATUS_TYPE;
    args.last_fd = DEF_LAST_FD;
    args.raw_buffer_size = NULL;
    args.raw_status_rate = NULL;
    args.raw_last_fd = NULL;
    
    argp_parse(&argp, argc, argv, 0, 0, &args);
    
    buffer_size_result = get_buffer_size(args.raw_buffer_size, &args.buffer_size);
    
    if (buffer_size_result < 0)
    {
        return -1;
    }
    
    status_result = get_status_rate(args.raw_status_rate, &args.status_rate, &args.status_type);
    
    if (status_result < 0)
    {
        return -2;
    }
    
    last_fd_result = get_last_fd(args.raw_last_fd, &args.last_fd);
    
    if (last_fd_result < 0)
    {
        return -3;
    }
    
    streams = open_streams();
    
    if (streams == 0)
    {
        log_error("No streams to write to. Exiting...\n");
        
        return -4;
    }
    
    amplify(streams);
    
    list_clear();
    
    return err;
}
