CC = gcc
CC_OPTS = -Wall -g --std=gnu99
LIBS = -lpthread

INCLUDES = serial_linux.h

OBJS = serial_linux.o

BINS = client.run

.phony:	clean all

all: $(BINS)

%.o:	%.c $(INCLUDES)
	$(CC) $(CC_OPTS) -c  $<

%.run:  %.o $(OBJS) $(INCLUDES)
	$(CC) $(CC_OPTS) -o $@ $< $(OBJS) $(LIBS)

clean:
	rm -rf $(OBJS) $(BINS) *~
