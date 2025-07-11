TARGET = pthread_test.x

CROSS = m68k-xelf-
CC = gcc
LD = gcc

CFLAGS = -D_GNU_SOURCE -O2 -g
LDFLAGS =
LIBS = -lpthread

SRCS = pthread_test.c \
    test_mutex.c \
    test_cond.c \
    test_barrier.c \
    test_rwlock.c \
    test_spinlock.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CROSS)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(LD) $(LDFLAGS) -o $(@:.x=) $(^:.o=.lo)  $(LIBS)
	
%.o: %.c
	$(CROSS)$(CC) $(CFLAGS) -c $<
	$(CC) $(CFLAGS) -c $< -o $(<:.c=.lo)

clean:
	-rm -f $(TARGET) $(OBJS) $(TARGET:.x=) $(OBJS:.o=.lo) *.elf

.PHONY: all clean
