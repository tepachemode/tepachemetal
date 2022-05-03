DEBUG	= -g -O0
CC	= gcc
INCLUDE	= -I/usr/local/include
#OPT 	= -O3
CFLAGS	= $(DEBUG) -Wall $(INCLUDE) -Winline -pipe -std=c99 $(OPT)
LDFLAGS	= -L/usr/local/lib
LDLIBS	= -lwiringPi -lwiringPiDev
OBJS = snesbot.o
all: snesbot

snesbot: $(OBJS)
	@echo [link]
	@$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

clean:
	rm -rf *.o snesbot