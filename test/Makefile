#
#
#
#
PLIB=..
INCLUDE = -I./ -I../src
LDFLAGS     = -L./ -L../src -lpylog -lpthread -lm  -g
COMMON_DEFINES = -DLINUX -D_REENTERANT -Wall -D_FILE_OFFSET_BITS=64 $(INCLUDE)  -g

ifeq "$(MAKECMDGOALS)" "release"
	DEFINES=$(COMMON_DEFINES) -DNDEBUG -O3
	CFLAGS= $(DEFINES) 
else
	ifeq "$(MAKECMDGOALS)" "withpg"
		DEFINES=$(COMMON_DEFINES) 
		CFLAGS= -g -pg $(DEFINES) 
	else
		DEFINES=$(COMMON_DEFINES)
		CFLAGS= -g $(DEFINES) 
	endif
endif
CC  = gcc
AR  = ar
#=========================================================================

EXECUTABLE =  test_log \
	      thread_log

TEST_EXEC = 

all	:  $(EXECUTABLE) $(LIBS) $(TEST_EXEC)

deps :
	$(CC) -MM -MG *.c >depends


test_log : test_log.o
	$(CC) -o $@ $^ $(LDFLAGS)

thread_log : thread_log.o
	$(CC) -o $@ $^ $(LDFLAGS)

rebuild : clean all
clean   :
	/bin/rm -f *.o core.* *~ $(EXECUTABLE) $(TEST_EXEC) 


release : all
withpg  : all

-include depends

