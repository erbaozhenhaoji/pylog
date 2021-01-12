/***********************************************************************************
 * Describe : log implementation file
 * Author   : Paul Yang, zhenahoji@gmail.com
 * Create   : 2008-10-15
 * Modify   : 2008-10-15
 **********************************************************************************/
#include <stdio.h>
#include <py_log.h>
#include <py_utils.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_FILENAME_SIZE 256
#define MAX_LOG_SIZE      1*1024*1024*1024

// datatype defined here
// 
typedef struct _logdata{
	int                  isopen;
	FILE*                fp;
	int                  is_lock_init;
	pthread_mutex_t      lock;
	enum LOGLEVEL        level;
	int                  last_open_year;
	int                  last_open_mon;
	int                  last_open_day;
	char                 path[MAX_FILENAME_SIZE];
	char                 name[MAX_FILENAME_SIZE];
}LOGDATA;


// static functions defined here
//
static int      logdata_open(LOGDATA* logdata, const char* path, const char* name, enum LOGLEVEL level);
static int      logdata_close(LOGDATA* logdata);
static int      logdata_write(LOGDATA* logdata, enum LOGLEVEL level, const char* fmt, va_list args);
static char*    get_timestr(char* buff, const int buff_size, struct tm* t);
static int     is_next_day(LOGDATA* logdata, struct tm* t);

// global variables defined here
// 
LOGDATA g_logdata_norm={0, NULL, 0, PTHREAD_MUTEX_INITIALIZER, PY_LOG_NOTSET, 0,0,0, "", ""};
LOGDATA g_logdata_warn={0, NULL, 0, PTHREAD_MUTEX_INITIALIZER, PY_LOG_WARN, 0,0,0, "", ""};


/*
 * func : open log, set log properties
 *
 * args : path, log path
 *      : name, log name
 *      : level, log level
 * 
 * ret  : 0, succeed; else error
 */
int py_log_open_withlevel(const char* path, const char* name, enum LOGLEVEL level)
{
	char wname[256];
	//char timestr[30];


	strcpy(wname, name);
	strcat(wname,".wf");

	if(logdata_open(&g_logdata_norm, path, name, level)<0){
		return -1;
	}
	if(logdata_open(&g_logdata_warn, path, wname, level)<0){
		return -1;
	}

	return 0;
}


/*
 * func : open log, set log properties
 *
 * args : path, log path
 *      : name, log name
 *      : level, log level
 * 
 * ret  : 0, succeed; else error
 */
int py_log_open(const char* path, const char* name, const char* level_str)
{
	enum LOGLEVEL level = PY_LOG_NOTSET;

	if(strcmp(level_str, "DEBUG")==0){
		level = PY_LOG_DEBUG;
	}
	else if(strcmp(level_str, "INFO")==0){
		level = PY_LOG_INFO;
	}
	else if(strcmp(level_str, "NOTICE")==0){
		level = PY_LOG_INFO;
	}
	else if(strcmp(level_str, "WARN")==0){
		level = PY_LOG_WARN;
	}
	else if(strcmp(level_str, "ERROR")==0){
		level = PY_LOG_ERROR;
	}
	else if(strcmp(level_str, "CRIT")==0){
		level = PY_LOG_CRIT;
	}
	else if(strcmp(level_str, "ALERT")==0){
		level = PY_LOG_ALERT;
	}
	else if(strcmp(level_str, "FATAL")==0){
		level = PY_LOG_FATAL;
	}
	else if(strcmp(level_str, "EMERG")==0){
		level = PY_LOG_EMERG;
	}
	else{
		level = PY_LOG_NOTSET;
	}

	return py_log_open_withlevel(path, name, level);

}


/*
 * func : write one log to log file
 *
 * args : level, log leve, if log level is weaker than init level,
 *      : nothing would be logged.
 *      : fmt, just like printf()
 * 
 * ret  : 0, succeed; else error
 */
int py_log_write(enum LOGLEVEL level, const char* fmt, ...)
{
	int     ret  = 0;
	va_list args;

	va_start(args, fmt);
	if(level<PY_LOG_WARN){
		ret = logdata_write(&g_logdata_norm, level, fmt, args);
	}else{
		ret = logdata_write(&g_logdata_warn, level, fmt, args);
	}
//	va_end(argc
	
	return ret;
	
}

/*
 * func : close log
 */
int py_log_close(void)
{
	
	if(logdata_close(&g_logdata_norm)<0){
		return -1;
	}
	if(logdata_close(&g_logdata_warn)<0){
		return -1;
	}

	return 0;
}

/*
 * func : logdata, pinter to LOGDATA
 *
 * args : path, name, log path and filename
 *      : level, log level
 *
 * ret  : 0, succeed
 *      : -1, error
 */
int logdata_open(LOGDATA* logdata, const char* path, const char* name, enum LOGLEVEL level)
{

	time_t        now;
	struct tm     t;
	FILE*         fp   = NULL;
	char          fullpath[MAX_FILENAME_SIZE];
	char          filename[128];
	char          datestr[30];

	time(&now);
	localtime_r(&now, &t);
	t.tm_year += 1900;
	snprintf(datestr, sizeof(datestr), ".%02d%02d%02d", t.tm_year, t.tm_mon+1, t.tm_mday);

	strncpy(filename, name, sizeof(filename)-1);
	filename[sizeof(filename)-1] = '\0';
	strncat(filename, datestr, sizeof(filename)-strlen(datestr));
	cmps_path(fullpath, sizeof(fullpath), path, filename);
	if((fp=fopen(fullpath, "a+"))==NULL){
		return -1;
	}
	setbuf(fp, NULL);

	// copy data
	strncpy(logdata->path, path, MAX_FILENAME_SIZE);
	logdata->path[MAX_FILENAME_SIZE-1] = '\0';
	strncpy(logdata->name, name, MAX_FILENAME_SIZE);
	logdata->name[MAX_FILENAME_SIZE-1] = '\0';
	logdata->level = level; 
	logdata->fp    = fp;
	logdata->last_open_year = t.tm_year;
	logdata->last_open_mon  = t.tm_mon;
	logdata->last_open_day  = t.tm_mday;
	logdata->isopen         = 1;
	if(logdata->is_lock_init==0){
		pthread_mutex_init(&(logdata->lock), NULL);
		logdata->is_lock_init=1;
	}


	return 0;
}


/*
 * func : close a LOGDATA struct
 *
 * args : 
 *
 * ret  : always 0
 */
int logdata_close(LOGDATA* logdata)
{

	if(logdata==NULL){
		return 0;
	}

	//logdata->path[0] = '\0'; 	// bug
	//logdata->name[0] = '\0';
	//logdata->level   = PY_LOG_NOTSET;
	
	if(logdata->fp!=NULL && logdata->fp!=stderr){
		fclose(logdata->fp);
		logdata->fp = stderr;
	}
	logdata->isopen = 0;

	return 0;
}

const char* levelstr_tab[11]={
	"UNKNOWN",
	"NOSET",
	"DEBUG",
	"INFO",
	"NOTICE",
	"WARN",
	"ERROR",
	"CRIT",
	"ALERT",
	"FATAL",
	"EMERGE"
};

/*
 * func : write log to LOGDATA specified files
 *
 * args : logdata, pointer to LOGDATA
 *      : level, log level
 *      : fmt, args, log format and arguements
 *
 * ret  : 0, succeed
 *      : -1, error
 */
int logdata_write(LOGDATA* logdata, enum LOGLEVEL level, const char* fmt, va_list args)
{
	int  ret        = 0;
	int  len        = 0;
	int  bufflen    = 0;
	char buff[4024];
	char timebuf[20];
	time_t now;
	struct tm t;

	
	if(level < logdata->level){
		return 0;
	}
	
	time(&now);
	localtime_r(&now, &t);

	
	if(is_next_day(logdata, &t)){

		pthread_mutex_lock(&(logdata->lock));
		if(logdata_close(logdata)<0){
			pthread_mutex_unlock(&(logdata->lock));
			return -1;
		}

		// should be locked
		if(logdata_open(logdata, logdata->path, logdata->name,logdata->level)<0){
			pthread_mutex_unlock(&(logdata->lock));
			return -1;
		}
		pthread_mutex_unlock(&(logdata->lock));
	}


	// make buffer 
	len = sprintf(buff, "%s\t%lu\t%s\n", levelstr_tab[(int)level], (unsigned long)pthread_self(),
		       	get_timestr(timebuf, sizeof(timebuf),&t));
	// replace the tail '\n' to  '\t'
	buff[len-1] = '\t';
	
	bufflen = vsnprintf(buff+len, sizeof(buff)-len, fmt, args);
	bufflen+=len;
	if((unsigned int)bufflen>=sizeof(buff)){
		return -1;
	}

	pthread_mutex_lock(&(logdata->lock));
	if(logdata->fp == NULL){
		logdata->fp = stderr;
	}
	ret = fprintf(logdata->fp,"%s", buff);
	while(ret<0){
		logdata_close(logdata);
		logdata_open(logdata, logdata->path, logdata->name, logdata->level);
		ret = fprintf(logdata->fp,"%s",  buff);
		sleep(1);
	}
	pthread_mutex_unlock(&(logdata->lock));

	return 0;
}



/*
 * func : get current time string
 *
 * args : buff, holding the time string
 *
 * ret  : same to buff
 */
char* get_timestr(char* buff, const int buff_size, struct tm* t)
{       

	t->tm_year += 1900;
	snprintf(buff,buff_size, "%02d-%02d %02d:%02d:%02d", t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);

	return  buff;  
}


/*
 * func : 
 *
 * args :
 *
 * ret  :
 */
int is_next_day(LOGDATA* logdata, struct tm* t)
{
	
	if(logdata->isopen == 0){
		return 0;
	}

	if(!(t->tm_mon==logdata->last_open_mon && t->tm_mday==logdata->last_open_day)){
		return 1;
	}
	else{
		return 0;
	}
}


