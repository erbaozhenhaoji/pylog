TARGET=libpylog.a
PREFIX=/usr/lib
CC= gcc

INCLUDES = -I./ 
LIBS =
DEFINES = -DLINUX -D_REENTERANT -Wall -D_FILE_OFFSET_BITS=64 

ifeq ($(MAKECMDGOALS), release)
	CFLAGS= $(DEFINES) -DNDEBUG -O3
else
	ifeq ($(MAKECMDGOALS), withpg)
		CFLAGS= $(DEFINES) -g -pg 
	else
		CFLAGS= $(DEFINES) -g
	endif
endif
C_SOURCES = $(wildcard *.c code/*.c)
C_OBJS = $(patsubst %.c, %.o, $(C_SOURCES))

.c.o:
	$(CC) -c -o $*.o $(CFLAGS) $(INCLUDES) $*.c

$(TARGET): $(C_OBJS)
	rm -f $@
	ar cr $@ $(C_OBJS)
	ranlib $@
clean:
	rm -f $(C_OBJS)
	rm -f $(TARGET)
	rm -f *~

install: $(TARGET)
	cp $(TARGET) $(PREFIX)

uninstall:
	rm -f $(PREFIX)/$(TARGET)

rebuild : clean $(TARGET) 
release : $(TARGET)
withpg  : $(TARGET)
