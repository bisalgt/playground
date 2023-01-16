#Cross compiler definition
CC = $(CROSS_COMPILE)gcc
RM = rm

#-Wall
CFLAGS  = -g -std=gnu99 -Wall -Werror
CFLAGS += -I/root/redpitaya/api/include/redpitaya -Ipthread.h
CFLAGS += -L/root/redpitaya/api/lib -lm -lpthread -lrp
CFLAGS += kiss_fft.c -lm
CFLAGS += kiss_fftr.c -lm
CFLAGS += -DFIXED_POINT=32
CFLAGS += -DKISS_FFT_USE_ALLOCA
CFLAGS += -DDEBUG


SRCS=$(wildcard *iic.c)
OBJS=$(SRCS:.c=)

all: $(OBJS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	$(RM) *.o
	$(RM) $(OBJ)
