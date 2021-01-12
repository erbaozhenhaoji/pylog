#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <assert.h>
#include <py_utils.h>

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
int cmps_path(char* fullpath, int nsize, const char* path, const char* file)
{
	int len = 0;

	assert(path && file && fullpath);
	
	len = strlen(path);
	if(path[len-1]=='/'){
		if(snprintf(fullpath, nsize, "%s%s", path, file)>=nsize){
			return -1;
		}
	}
	else{
		if(snprintf(fullpath, nsize, "%s/%s", path, file)>=nsize){
			return -1;
		}
	}

	return 0;
}


/*
 * func  : split string to words according to certain delimiters
 *
 * args  : str, the tring to be splitted.
 *       : items, item_size, result word array and its size
 *       : delim, specified delimiters
 *
 * ret   : -1, failed.
 *       : else, number of words
 *
 * note  : str will be CHANGED!
 */
int split_c(char* str, char** items, int item_size, const char* delim)
{
      char* begin = NULL;
      char* end   = NULL;
      int   cnt   = 0;

      assert(str && items);

      begin = str;
      while(*begin){
	    // find word begin
	    while(*begin&&strchr(delim, *begin)!=NULL){
		  begin++;
	    }

	    // find the word end.
	    end = begin;
	    while(*end&&strchr(delim, *end)==NULL){
		  end++;
	    }

	    if(cnt+1 == item_size)
		  return -1;
	    if(end - begin>0){
		  items[cnt++] = begin;
	    }
	    if(*end){
		  *end = 0;
		  begin = end+1;
	    }

	    else{
		  break;
	    }
      }
      return cnt;
}


/*
 * func : get the line number of a text file
 *
 * args : fullpath, the file full path name
 *      : linenum, the result line number
 *
 * ret  : 0, succeed.
 *      : -1, error.
 */
int py_fline(const char* fullpath, int* linenum)
{
	char ch = 0;
	FILE* fp = NULL;

	*linenum = 0;

	if((fp=fopen(fullpath, "rb"))==NULL){
		return -1;
	}

	while((ch=fgetc(fp))!=EOF) {  
		if(ch=='\n'){
			++(*linenum);
		}
	}   		 

	fclose(fp);

	return 0;
}


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
int py_fstat(const char* filename, int* linenum, int* filesize)
{
	char ch = 0;
	FILE* fp = NULL;

	*linenum = 0;

	if((fp=fopen(filename, "rb"))==NULL){
		return -1;
	}
	
	while((ch=fgetc(fp))!=EOF) {  
		if(ch=='\n'){
			++(*linenum);
		}
	} 
	*filesize = ftell(fp);	

	fclose(fp);

	return 0;
}

/*
 * func : check whether a word is GBK hanzi
 *
 * args : pstr, the input str
 * 
 * ret  : true, it is a hanzi;
 *      : false, it is NOT a hanzi
 */
int is_gbk_hz(unsigned char* pstr)
{
	assert(pstr);
	if(pstr[0]==0 || pstr[1]==0){
		return 0;
	}

	if(pstr[1] == 0x7f){
		return 0; 
	}

	// if it is a gbk/2 word
	if ( (pstr[0]>=0xB0) && (pstr[0]<=0xF7) &&
			(pstr[1]>=0xA1) && (pstr[1]<=0xFE))
	{       
		return 1;
	}

	// if it is a gbk/3 word
	else if ((pstr[0]>=0x81) && (pstr[0]<=0xa0) &&
			(pstr[1]>=0x40) && (pstr[1]<=0xfe))
	{                       
		return 1;
	}

	// if it is a gbk/4 word

	else if ((pstr[0]>=0xaa) && (pstr[0]<=0xfe) &&
			(pstr[1]>=0x40) && (pstr[1]<=0xa0))
	{       
		return 1;
	}
	else{
		return 0;
	}
}	


/*
 * func : sleep function for thread
 *
 * args : sec, time to sleep, ( in seconds )
 *
 * ret  :
 */
void pthr_sleep(int sec)
{
	struct timeval t_val;

	t_val.tv_sec = sec;
	t_val.tv_usec = 0;

	select(0, NULL, NULL, NULL, &t_val);
}

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
		             char** tokens, int token_size, const char *delim)
{
	int token_num = 0;
	int strbuf_curpos = 0;
	const char* cur = NULL;
			

	assert(strbuf!=NULL && strbuf_size>1);
	assert(tokens!=NULL && token_size>0);

	strbuf[0] = '\0';

	cur = src;
	while (*cur){
		while(*cur && strchr(delim, *cur)){// for all delimerters
			if(strbuf_curpos + 2 == strbuf_size){
				return -1;
			}
			if(token_num+1==token_size){
				return -1;
			}
			tokens[token_num++]=strbuf+strbuf_curpos;

			strbuf[strbuf_curpos++] = *cur;
			strbuf[strbuf_curpos++] = '\0';


			cur++;
		}

		// get a non-delimeter token
		const char* end   = cur;
		while(*end && !strchr(delim, *end)){
			++end;
		}

		if(end-cur>0){
			int len = end - cur;
			if(token_num+1==token_size){
				return -1;
			}
			tokens[token_num++] = strbuf+strbuf_curpos;

			if(strbuf_curpos+len+1==strbuf_size){
				return -1;
			}
			memcpy(strbuf+strbuf_curpos, cur, len);
			strbuf_curpos += len;
			strbuf[strbuf_curpos++] = '\0';
			cur = end;
		}
	}

	return token_num;

}

