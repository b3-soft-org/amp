// Copyright 2018 Matthias Schwarz
// License: MIT



/*
    Functions to parse the given options.
*/



#include "amp.h"



struct argp_option options[] =
{
    { "no-stdout", 'o', 0,       0, "Do not write to stdout" },
    { "buffer",    'b', "SIZE",  0, "The buffer size" },
    { "count",     'c', "COUNT", 0, "The number of bytes to read from stdin" },
    { "last-fd",   'f', "LAST",  0, "The number of the last file descriptor to open" },
    { "lines",     'l', "LINES", 0, "The number of lines to read from stdin" },
    { "status",    's', "RATE",  0, "Print status with the specified rate (e.g. time or space)" },
    { 0 }
};

const char *argp_program_version = "1.0";
const char  doc[]                = "amp - Read from stdin and write to various output streams.";

struct argp argp = { options, parse_opt, NULL, doc };
struct args args;



error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    struct args *arguments = state->input;
    
    switch (key)
    {
        case 'o':
            arguments->ign_stdout = 1;
            break;
        case 'b':
            arguments->raw_buffer_size = arg;
            break;
        case 'c':
            arguments->raw_count = arg;
            break;
        case 'f':
            arguments->raw_last_fd = arg;
            break;
        case 'l':
            arguments->raw_lines = arg;
            break;
        case 's':
            arguments->raw_status_rate = arg;
            break;
        case ARGP_KEY_ARG:
            argp_usage(state);
        case ARGP_KEY_END:
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    
    return 0;
}

/*
    Parses the given buffer size.
*/
int get_buffer_size(char *arg, unsigned long long *buffer_size)
{
    unsigned long long value = 0;
    unsigned long long factor = 0;
    int   endlen = 0;
    char *endptr;
    
    if (arg == NULL)
    {
        *buffer_size = DEF_BUFFER_SIZE;
        
        return 0;
    }
    
    errno = 0;
    value = strtoull(arg, &endptr, 10);
    
    if (errno != 0)
    {
        if (errno == ERANGE && (value == 0 || value == ULLONG_MAX))
        {
            log_error("Argument 'buffer' is out of range.\n");
            
            return 1;
        }
        
        if (value == 0)
        {
            log_error("Argument 'buffer' is not a valid number.\n");
            
            return 1;
        }
    }
    
    if (arg == endptr)
    {
        log_error("Argument 'buffer' does not contain any digits.\n");
        
        return 1;
    }
    
    if (*endptr == '\0')
    {
        *buffer_size = value;
        
        return 0;
    }
    
    endlen = strlen(endptr);
    
    if (endlen != 1)
    {
        log_error("Unit of argument 'buffer' is too long.\n");
        
        return 1;
    }
    
    if (*endptr == 'B')
    {
        factor = 1;
    }
    else if (*endptr == 'K')
    {
        factor = 1024;
    }
    else if (*endptr == 'M')
    {
        factor = 1024 * 1024;
    }
    else if (*endptr == 'G')
    {
        factor = 1024 * 1024 * 1024;
    }
    else
    {
        log_error("Unknown unit for argument 'buffer'.\n");
        
        return 1;
    }
    
    *buffer_size = value * factor;
    
    return 0;
}

/*
    Parses the given count.
*/
int get_count(char *arg, unsigned long long *count)
{
    unsigned long long value = 0;
    unsigned long long factor = 0;
    int   endlen = 0;
    char *endptr;
    
    if (arg == NULL)
    {
        *count = DEF_COUNT;
        
        return 0;
    }
    
    errno = 0;
    value = strtoull(arg, &endptr, 10);
    
    if (errno != 0)
    {
        if (errno == ERANGE && (value == 0 || value == ULLONG_MAX))
        {
            log_error("Argument 'count' is out of range.\n");
            
            return 1;
        }
        
        if (value == 0)
        {
            log_error("Argument 'count' is not a valid number.\n");
            
            return 1;
        }
    }
    
    if (arg == endptr)
    {
        log_error("Argument 'count' does not contain any digits.\n");
        
        return 1;
    }
    
    if (*endptr == '\0')
    {
        *count = value;
        
        return 0;
    }
    
    endlen = strlen(endptr);
    
    if (endlen != 1)
    {
        log_error("Unit of argument 'count' is too long.\n");
        
        return 1;
    }
    
    if (*endptr == 'B')
    {
        factor = 1;
    }
    else if (*endptr == 'K')
    {
        factor = 1024;
    }
    else if (*endptr == 'M')
    {
        factor = 1024 * 1024;
    }
    else if (*endptr == 'G')
    {
        factor = 1024 * 1024 * 1024;
    }
    else
    {
        log_error("Unknown unit for argument 'count'.\n");
        
        return 1;
    }
    
    *count = value * factor;
    
    return 0;
}

/*
    Parses the given last file descriptor.
*/
int get_last_fd(char *arg, unsigned int *last_fd)
{
    unsigned long long value = 0;
    char *endptr;
    
    if (arg == NULL)
    {
        *last_fd = DEF_LAST_FD;
        
        return 0;
    }
    
    errno = 0;
    value = strtoul(arg, &endptr, 10);
    
    if (errno != 0)
    {
        if (errno == ERANGE && (value == 0 || value == ULONG_MAX))
        {
            log_error("Argument 'last_fd' is out of range.\n");
            
            return 1;
        }
        
        if (value == 0)
        {
            log_error("Argument 'last_fd' is not a valid number.\n");
            
            return 1;
        }
    }
    
    if (arg == endptr)
    {
        log_error("Argument 'last_fd' does not contain any digits.\n");
        
        return 1;
    }
    
    if (value < MIN_FD)
    {
        log_error("Argument 'last_fd' is less than the minimum of %d.\n", MIN_FD);
        
        return 1;
    }
    
    if (value > MAX_FD)
    {
        log_error("Argument 'last_fd' is greater than the maximum of %d.\n", MAX_FD);
        
        return 1;
    }
    
    if (*endptr != '\0')
    {
        log_error("Argument 'last_fd' may not have a suffix.\n");
        
        return 1;
    }
    
    *last_fd = (unsigned int)(value);
    
    return 0;
}

/*
    Parses the given number of lines.
*/
int get_lines(char *arg, unsigned long long *lines)
{
    unsigned long long value = 0;
    unsigned long long factor = 0;
    int   endlen = 0;
    char *endptr;
    
    if (arg == NULL)
    {
        *lines = DEF_LINES;
        
        return 0;
    }
    
    errno = 0;
    value = strtoull(arg, &endptr, 10);
    
    if (errno != 0)
    {
        if (errno == ERANGE && (value == 0 || value == ULLONG_MAX))
        {
            log_error("Argument 'lines' is out of range.\n");
            
            return 1;
        }
        
        if (value == 0)
        {
            log_error("Argument 'lines' is not a valid number.\n");
            
            return 1;
        }
    }
    
    if (arg == endptr)
    {
        log_error("Argument 'lines' does not contain any digits.\n");
        
        return 1;
    }
    
    if (*endptr == '\0')
    {
        *lines = value;
        
        return 0;
    }
    
    endlen = strlen(endptr);
    
    if (endlen != 1)
    {
        log_error("Argument 'lines' is too long.\n");
        
        return 1;
    }
    
    if (*endptr == 'B')
    {
        factor = 1;
    }
    else if (*endptr == 'K')
    {
        factor = 1024;
    }
    else if (*endptr == 'M')
    {
        factor = 1024 * 1024;
    }
    else if (*endptr == 'G')
    {
        factor = 1024 * 1024 * 1024;
    }
    else
    {
        log_error("Unknown unit for argument 'lines'.\n");
        
        return 1;
    }
    
    *lines = value * factor;
    
    return 0;
}

/*
    Parses the given status rate.
*/
int get_status_rate(char *arg, unsigned long long *status_rate, int *status_type)
{
    unsigned long long value = 0;
    unsigned long long factor = 0;
    int   endlen = 0;
    char *endptr;
    
    if (arg == NULL)
    {
        *status_rate = DEF_STATUS_RATE;
        *status_type = DEF_STATUS_TYPE;
        
        return 0;
    }
    
    errno = 0;
    value = strtoull(arg, &endptr, 10);
    
    if (errno != 0)
    {
        if (errno == ERANGE && (value == 0 || value == ULLONG_MAX))
        {
            log_error("Argument 'status' is out of range.\n");
            
            return 1;
        }
        
        if (value == 0)
        {
            log_error("Argument 'status' is not a valid number.\n");
            
            return 1;
        }
    }
    
    if (arg == endptr)
    {
        log_error("Argument 'status' does not contain any digits.\n");
        
        return 1;
    }
    
    if (*endptr == '\0')
    {
        *status_rate = value;
        
        return 0;
    }
    
    endlen = strlen(endptr);
    
    if (endlen != 1)
    {
        log_error("Argument 'status' is too long.\n");
        
        return 1;
    }
    
    if (*endptr == 'B')
    {
        factor = 1;
        *status_type = STATUS_TYPE_SIZE;
    }
    else if (*endptr == 'K')
    {
        factor = 1024;
        *status_type = STATUS_TYPE_SIZE;
    }
    else if (*endptr == 'M')
    {
        factor = 1024 * 1024;
        *status_type = STATUS_TYPE_SIZE;
    }
    else if (*endptr == 'G')
    {
        factor = 1024 * 1024 * 1024;
        *status_type = STATUS_TYPE_SIZE;
    }
    else if (*endptr == 's')
    {
        factor = 1;
        *status_type = STATUS_TYPE_TIME;
    }
    else if (*endptr == 'm')
    {
        factor = 60;
        *status_type = STATUS_TYPE_TIME;
    }
    else if (*endptr == 'h')
    {
        factor = 3600;
        *status_type = STATUS_TYPE_TIME;
    }
    else
    {
        log_error("Unknown unit for argument 'status'.\n");
        
        return 1;
    }
    
    *status_rate = value * factor;
    
    return 0;
}
