CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic -m32 -g
PUBL=publications

.PHONY: build clean

build: $(PUBL).o

Hashtable.o: Hashtable.c Hashtable.h
	$(CC) $(CFLAGS) Hashtable.c -c -o Hashtable.o

LinkedList.o: LinkedList.c LinkedList.h
	$(CC) $(CFLAGS) LinkedList.c -c -o LinkedList.o

publ.o: $(PUBL).c $(PUBL).h
	$(CC) $(CFLAGS) $(PUBL).c -c -o publ.o

$(PUBL).o: Hashtable.o LinkedList.o publ.o
	ld -m elf_i386 -r publ.o Hashtable.o LinkedList.o -o $(PUBL).o

clean:
	rm -f *.o *.h.gch
