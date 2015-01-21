CC = gcc
CFLAGS = -Wall -g

all:	phil phil-ordered phil-random 

phil:	phil.c
	$(CC) $(CFLAGS) -o phil phil.c -lpthread

phil-ordered:	phil-ordered.c
	$(CC) $(CFLAGS) -o phil-ordered phil-ordered.c -lpthread

phil-random:	phil-random.c
	$(CC) $(CFLAGS) -o phil-random phil-random.c -lpthread

clean:
	rm -f phil phil-ordered phil-random *.o *~
	rm -rf *.dSYM
