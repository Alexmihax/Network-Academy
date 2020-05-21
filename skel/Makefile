CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic -m32 -g
PUBL=publications

.PHONY: build clean

build: $(PUBL).o

heap.o: heap.c heap.h
	$(CC) $(CFLAGS) heap.c -c -o heap.o

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) queue.c -c -o queue.o

hashtable.o: hashtable.c hashtable.h
	$(CC) $(CFLAGS) hashtable.c -c -o hashtable.o

linkedList.o: linkedList.c linkedList.h
	$(CC) $(CFLAGS) linkedList.c -c -o linkedList.o

publ.o: $(PUBL).c $(PUBL).h
	$(CC) $(CFLAGS) $(PUBL).c -c -o publ.o

$(PUBL).o: hashtable.o linkedList.o publ.o queue.o heap.o
	ld -m elf_i386 -r publ.o hashtable.o linkedList.o queue.o heap.o -o $(PUBL).o

clean:
	rm -f *.o *.h.gch
