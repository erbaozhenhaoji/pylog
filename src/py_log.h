/***********************************************************************************
 * Descri   : log management lib, this is a simple log lib. it only provide normal
 *          : 'open' and 'write' functions, log names will be grouped by date sufix
 *          : thread is supported in this lib.
 *
 * Author   : Paul Yang, zhenahoji@gmail.com
 *
 * Create   : 2008-10-15
 *
 * Update   : 2008-10-15
 **********************************************************************************/
#ifndef PY_LOG_H
#define PY_LOG_H
#include <stdarg.h>

enum LOGLEVEL{
	PY_LOG_NOTSET=1,
	PY_LOG_DEBUG,
	PY_LOG_INFO,
	PY_LOG_NOTICE,
	PY_LOG_WARN,
	PY_LOG_ERROR,
	PY_LOG_CRIT,
	PY_LOG_ALERT,
	PY_LOG_FATAL,
	PY_LOG_EMERG
};

/*
 * func : open log, set log properties
 *
 * args : path, log path
 *      : name, log name
 *      : level, log level
 * 
 * ret  : 0, succeed; else error
 */
int py_log_open_withlevel(const char* path, const char* name, enum LOGLEVEL level);

/*
 * func : open log, set log properties
 *
 * args : path, log path
 *      : name, log name
 *      : level, log level string
 * 
 * ret  : 0, succeed; else error
 */
int py_log_open(const char* path, const char* name, const char* level);

/*
 * func : write one log to log file
 *
 * args : level, log leve, if log level is weaker than init level,
 *      : nothing would be logged.
 *      : fmt, just like printf()
 * 
 * ret  : 0, succeed; else error
 */
int py_log_write(enum LOGLEVEL level, const char* fmt, ...);

/*
 * func : close log
 */
int py_log_close(void);


#endif

