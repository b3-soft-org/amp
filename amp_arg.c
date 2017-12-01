#include "amp.h"

struct argp_option options[] =
{
    { "stderr",    'e', 0,      0, "Also write to stderr." },
    { "syslog",    'l', 0,      0, "Log messages to Syslog." },
    { "no-stdout", 'o', 0,      0, "Do not write to stdout." },
    { "buffer",    'b', "SIZE", 0, "The buffer size." },
    { "status",    's', "RATE", 0, "Print status with the specified rate (e.g. time or space)." },
    { "last-fd",   'f', "LAST",  0, "The number of the last file descriptor to open." },
    { 0 }
};

char doc[] = "amp - Read from stdin and write to various output streams.";

struct argp argp = { options, parse_opt, NULL, doc };
struct Arguments args;

error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    struct Arguments *arguments = state->input;
    
    switch (key)
    {
        case 'e':
            arguments->use_stderr = 1;
            break;
        case 'l':
            arguments->use_syslog = 1;
            break;
        case 'o':
            arguments->ign_stdout = 1;
            break;
        case 'b':
            arguments->raw_buffer_size = arg;
            break;
        case 's':
            arguments->raw_status_rate = arg;
            break;
        case 'f':
            arguments->raw_last_fd = arg;
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

int get_buffer_size(char *arg, unsigned long long *buffer_size)
{
    unsigned long long value = 0;
    unsigned long long factor = 0;
    char *end = NULL;
    int endlen = 0;
    
    if (arg == NULL)
    {
        *buffer_size = DEF_BUFFER_SIZE;
        
        return 0;
    }
    
    errno = 0;
    value = strtoul(arg, &end, 10);
    
    if (errno == ERANGE)
    {
        return -1;
    }
    
    if (value == 0)
    {
        return -2;
    }
    
    if (*end == 0)
    {
        *buffer_size = value;
        
        return 0;
    }
    
    endlen = strlen(end);
    
    if (endlen != 1)
    {
        return -3;
    }
    
    if (*end == 'B')
    {
        factor = 1;
    }
    else if (*end == 'K')
    {
        factor = 1024;
    }
    else if (*end == 'M')
    {
        factor = 1024 * 1024;
    }
    else if (*end == 'G')
    {
        factor = 1024 * 1024 * 1024;
    }
    else
    {
        return -4;
    }
    
    *buffer_size = value * factor;
    
    return 0;
}

int get_status_rate(char *arg, unsigned long long *status_rate, int *status_type)
{
    unsigned long long value = 0;
    unsigned long long factor = 0;
    char *end = NULL;
    int endlen = 0;
    
    if (arg == NULL)
    {
        // FIXME: Sinnvolle Werte finden
        *status_rate = DEF_STATUS_RATE;
        *status_type = STATUS_TYPE_NONE;
        
        return 0;
    }
    
    errno = 0;
    value = strtoul(arg, &end, 10);
    
    if (errno == ERANGE)
    {
        return -1;
    }
    
    if (value == 0)
    {
        return -2;
    }
    
    if (*end == 0)
    {
        return -3;
    }
    
    endlen = strlen(end);
    
    if (endlen != 1)
    {
        return -4;
    }
    
    if (*end == 'B')
    {
        factor = 1;
        *status_type = STATUS_TYPE_SIZE;
    }
    else if (*end == 'K')
    {
        factor = 1024;
        *status_type = STATUS_TYPE_SIZE;
    }
    else if (*end == 'M')
    {
        factor = 1024 * 1024;
        *status_type = STATUS_TYPE_SIZE;
    }
    else if (*end == 'G')
    {
        factor = 1024 * 1024 * 1024;
        *status_type = STATUS_TYPE_SIZE;
    }
    else if (*end == 's')
    {
        factor = 1;
        *status_type = STATUS_TYPE_TIME;
    }
    else if (*end == 'm')
    {
        factor = 60;
        *status_type = STATUS_TYPE_TIME;
    }
    else if (*end == 'h')
    {
        factor = 3600;
        *status_type = STATUS_TYPE_TIME;
    }
    else
    {
        return -4;
    }
    
    *status_rate = value * factor;
    
    return 0;
}

int get_last_fd(char *arg, int *last_fd)
{
    unsigned long long value = 0;
    char *end = NULL;
    
    if (arg == NULL)
    {
        *last_fd = DEF_LAST_FD;
        
        return 0;
    }
    
    errno = 0;
    value = strtoul(arg, &end, 10);
    
    if (errno == ERANGE)
    {
        return -1;
    }
    
    if (value == 0)
    {
        return -2;
    }
    
    if (value > INT_MAX)
    {
        return -3;
    }
    
    if (*end != 0)
    {
        return -4;
    }
    
    *last_fd = (int)(value);
    
    return 0;
}
