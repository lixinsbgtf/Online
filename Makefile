CC = g++
#CFLAGS = -g
#CFLAGS = -pg
CFLAGS = -O3

all: online 

online: util.o Online.o main.o
	$(CC) $(CFLAGS) -o $@ $^


# suffix rules

.SUFFIXES: .cpp

.cpp.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o online
