// Copyright 2018 Matthias Schwarz
// License: MIT

#include "amp.h"

int main(int argc, char **argv)
{
    int streams = 0;
    int result = 0;
    
    int buffer_size_result = 0;
    int count_result = 0;
    int last_fd_result = 0;
    int lines_result = 0;
    int status_result = 0;
    int amplify_result = 0;
    
    args.ign_stdout = DEF_IGN_STDOUT;
    args.buffer_size = DEF_BUFFER_SIZE;
    args.count = DEF_COUNT;
    args.last_fd = DEF_LAST_FD;
    args.lines = DEF_LINES;
    args.status_rate = DEF_STATUS_RATE;
    args.status_type = DEF_STATUS_TYPE;
    args.raw_buffer_size = NULL;
    args.raw_count = NULL;
    args.raw_last_fd = NULL;
    args.raw_status_rate = NULL;
    
    argp_parse(&argp, argc, argv, 0, 0, &args);
    
    count_result = get_count(args.raw_count, &args.count);
    if (count_result != 0)
    {
        result = 1;
        goto cleanup;
    }
    
    lines_result = get_lines(args.raw_lines, &args.lines);
    if (lines_result != 0)
    {
        result = 1;
        goto cleanup;
    }
    
    if (args.count > 0 && args.lines > 0)
    {
        log_error("Options -c|--count and -l|--lines may not be used at the same time.\n");
        result = 1;
        goto cleanup;
    }
    
    buffer_size_result = get_buffer_size(args.raw_buffer_size, &args.buffer_size);
    if (buffer_size_result != 0)
    {
        result = 1;
        goto cleanup;
    }
    
    last_fd_result = get_last_fd(args.raw_last_fd, &args.last_fd);
    if (last_fd_result != 0)
    {
        result = 1;
        goto cleanup;
    }
    
    status_result = get_status_rate(args.raw_status_rate, &args.status_rate, &args.status_type);
    if (status_result != 0)
    {
        result = 1;
        goto cleanup;
    }
    
    if (args.count < args.buffer_size)
    {
        log_warning("The buffer size is greater than the requested number of bytes (count).\n");
    }
    
    streams = open_streams();
    
    if (streams == 0)
    {
        log_error("No streams to write to. Exiting...\n");
        
        result = 1;
        goto cleanup;
    }
    
    amplify_result = amplify(streams);
    
    if (amplify_result != 0)
    {
        result = 1;
        goto cleanup;
    }
    
cleanup:
    list_clear();
    
    return result;
}
