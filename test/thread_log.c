#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <py_log.h>

static void* log_func(void* arg);

int main(int argc, char* argv[])
{
	int i = 0;
	int thread_num = 100;
	pthread_t tids[1000];
	
	if(py_log_open_withlevel("./log", "testlog", PY_LOG_DEBUG)<0){
		fprintf(stdout, "open log failed");
		exit(-1);
	}
	else{
		fprintf(stdout, "open log succeed");
	}

	for(i=0;i<thread_num;i++){
		pthread_create(&tids[i], NULL, log_func, NULL);
	}

	for(i=0;i<thread_num;i++){
		pthread_join(tids[i], NULL);
	}




	py_log_close();

	return 0;
}

void* log_func(void* arg)
{
	int i = 0;

	for(i=0;i<10000;i++){
		py_log_write(PY_LOG_WARN, "this is a warn %s\n", "xixihaha");
		py_log_write(PY_LOG_WARN, "this is a warn %s, %d\n", "xixihaha", 50);
		py_log_write(PY_LOG_DEBUG, "this is a debug %s\n", "xixihaha");
	}

	return NULL;

}

	

