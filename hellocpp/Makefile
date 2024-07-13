TARGET = hellocpp.x

CROSS = m68k-xelf-
CC = $(CROSS)g++
AS = $(CROSS)as
LD = $(CROSS)g++

CFLAGS = -m68000 -O2 -g
LDFLAGS =

all: $(TARGET)

SRCS = main.cpp ccstring.cpp ccmap.cpp cclambda.cpp ccrtti.cpp ccsmart.cpp
OBJS = $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^
	
%.o: %.cpp
	$(CC) $(CFLAGS) -c $<

clean:
	-rm -f $(TARGET) $(OBJS) *.x*

.PHONY: all clean
