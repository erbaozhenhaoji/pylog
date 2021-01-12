/********************************************************************************
 * Descri : collection of some common functions
 *
 * Author : Paul Yang, zhenhaoji@gmail.com
 *
 * Create : 2008-10-15
 *
 * Update : 
 ********************************************************************************/
#ifndef _PY_UTILS_H
#define _PY_UTILS_H

#include <string.h>

int split_c(char* str, char** items, int item_size, const char* delim);

/*
 * func : compose a fullpath from specified path and filename
 *
 * args : fullpath, nsize, fullpath buffer and its size
 *      : path, specifed file path
 *      : file, specifed file name
 *
 * ret  : -1, error
 *      : 0,  succeed
 */
int cmps_path(char* buff, int buff_size, const char* path, const char* file);

/*
 * func : get linenumber and file size of a text file
 *
 * args : filename, the file full path
 *      : linenum, file line number
 *      : filesize, file size (in byte)
 *
 * ret  : 0, succeed.
 *      : -1, failed
 */
int py_fstat(const char* filename, int* linenum, int* filesize);

/*
 * func : get the line number of a text file
 *
 * args : fullpath, the file full path name
 *      : linenum, the result line number
 *
 * ret  : 0, succeed.
 *      : -1, error.
 */
int py_fline(const char* filename, int* linenum);

/*
 * func : check whether a word is GBK hanzi
 *
 * args : pstr, the input str
 * 
 * ret  : true, it is a hanzi;
 *      : false, it is NOT a hanzi
 */
int is_gbk_hz(unsigned char* pstr);

/*
 * func : sleep function for thread
 *
 * args : sec, time to sleep, ( in seconds )
 *
 * ret  :
 */
void pthr_sleep(int sec);

/*
 * func : split a string buffer into tokens by delimiters, each delimeter 
 *      : is trated as a token.
 *
 * args : src, original string
 *      : strbuf, strbuf_size, buffer to hold the token string
 *      : tokens, token_size, result token array ant its size
 *      : delim, the delimeters
 *
 * ret  : <0, error
 *      : else, tokern number
 *
 */
int split_all(const char* src, char* strbuf, const int strbuf_size, 
		             char** tokens, int token_size, const char *delim);


#endif
