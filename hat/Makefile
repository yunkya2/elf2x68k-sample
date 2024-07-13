TARGET = hat.x

all: $(TARGET)

%.x: %.bas
	m68k-xelf-bas -o $@ $<

clean:
	-rm -f $(TARGET)*

.PHONY: all clean
