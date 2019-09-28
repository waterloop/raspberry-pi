CC=g++
CFLAGS=-I lib/include
OBJS=lib/canbus/canbus.o

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

main: main.cpp $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(OBJS)
