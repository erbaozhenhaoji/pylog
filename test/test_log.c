#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <py_log.h>
#include <unistd.h>


static double calc_time(struct timeval* tv_begin, struct timeval* tv_end);

int main(int argc, char* argv[])
{
	int i = 0;
	const char* logstr1 = "this is a WARN: xixixhaha,11111111111111111111111111111111111111111111111111\n";
	const char* logstr2 = "this is a FATAL: xixihaha,22222222222222222222222222222222222222222222222222\n";
	const char* logstr3 = "this is a DEBUG: xixihaha,33333333333333333333333333333333333333333333333333\n";
	unsigned int num = 10000000;
	unsigned int len = 0;
	double       time_used = 0.0;
	struct timeval tv_begin;
	struct timeval tv_end;

	if(py_log_open_withlevel("./log", "testlog",  PY_LOG_DEBUG)<0){
		fprintf(stdout, "open log failed");
		exit(-1);
	}
	else{
		fprintf(stdout, "open log succeed\n");
	}

	gettimeofday(&tv_begin, NULL);
	for(i=0;i<num;i++){
//		py_log_write(PY_LOG_WARN, logstr1); 
//		py_log_write(PY_LOG_FATAL, logstr2);
		py_log_write(PY_LOG_DEBUG, logstr3);
	}
	gettimeofday(&tv_end, NULL);

	time_used = calc_time(&tv_begin, &tv_end);
	
//	sleep(100);

	py_log_close();

	py_log_write(PY_LOG_WARN, "after close, log will be write to stderr\n");

	len = strlen(logstr1)+strlen(logstr2)+strlen(logstr3);
	fprintf(stdout, "test_log finished succsessfully.\n");
	fprintf(stdout, "%d log has been write to ./log/testlog.XXXX\n", num );
	fprintf(stdout, "%d log has been write to ./log/testlog.wf.XXXX\n", 2*num);
	fprintf(stdout, "time used : %f\n", time_used);
	fprintf(stdout, "logging speed: %f logs/s, or %f M/s\n", 3*num/time_used, len*num/(1024*1024*time_used));



	return 0;
}


double calc_time(struct timeval* tv_begin, struct timeval* tv_end)
{
	double usec = 0.0;

	usec = tv_end->tv_usec - tv_begin->tv_usec;
	usec += 1000000*(tv_end->tv_sec - tv_begin->tv_sec);

	return usec/1000000;
}
