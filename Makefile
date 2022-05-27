CC=gcc
CFLAGS=-Wall -O
LDFLAGS=
EXEC=exe   

all: $(EXEC)

exe : readcmd.o listejob.o main.o
	$(CC) -o exe main.o readcmd.o listejob.o $(LDFLAGS)

listejob.o : listejob.c
	$(CC) -o listejob.o -c listejob.c $(CFLAGS)

readcmd.o : readcmd.c
	$(CC) -o  readcmd.o -c readcmd.c $(CFLAGS)

main.o : main.c
	$(CC) -o main.o -c main.c $(CFLAGS)

clean:
	rm -f *.o core

mrproper: clean
	rm -f $(EXEC)