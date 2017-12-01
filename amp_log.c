#include "amp.h"

void stderr_info(const char *format, va_list vl)
{
    fprintf(stderr, "INFO: ");
    vfprintf(stderr, format, vl);
}

void stderr_warning(const char *format, va_list vl)
{
    fprintf(stderr, "WARNING: ");
    vfprintf(stderr, format, vl);
}

void stderr_error(const char *format, va_list vl)
{
    fprintf(stderr, "ERROR: ");
    vfprintf(stderr, format, vl);
}

void stderr_critical(const char *format, va_list vl)
{
    fprintf(stderr, "CRITICAL: ");
    vfprintf(stderr, format, vl);
}

void syslog_info(const char *format, va_list vl)
{
    vsyslog(LOG_INFO, format, vl);
}

void syslog_warning(const char *format, va_list vl)
{
    vsyslog(LOG_WARNING, format, vl);
}

void syslog_error(const char *format, va_list vl)
{
    vsyslog(LOG_ERR, format, vl);
}

void syslog_critical(const char *format, va_list vl)
{
    vsyslog(LOG_CRIT, format, vl);
}

void log_info(const char *format, ...)
{
    va_list vl;
    va_start(vl, format);
    
    if (args.use_stderr == 0)
    {
        stderr_info(format, vl);
    }
    
    if (args.use_syslog == 1)
    {
        syslog_info(format, vl);
    }
    
    va_end(vl);
}

void log_warning(const char *format, ...)
{
    va_list vl;
    va_start(vl, format);
    
    if (args.use_stderr == 0)
    {
        stderr_warning(format, vl);
    }
    
    if (args.use_syslog == 1)
    {
        syslog_warning(format, vl);
    }
    
    va_end(vl);
}

void log_error(const char *format, ...)
{
    va_list vl;
    va_start(vl, format);
    
    if (args.use_stderr == 0)
    {
        stderr_error(format, vl);
    }
    
    if (args.use_syslog == 1)
    {
        syslog_error(format, vl);
    }
    
    va_end(vl);
}

void log_critical(const char *format, ...)
{
    va_list vl;
    va_start(vl, format);
    
    if (args.use_stderr == 0)
    {
        stderr_critical(format, vl);
    }
    
    if (args.use_syslog == 1)
    {
        syslog_critical(format, vl);
    }
    
    va_end(vl);
}
