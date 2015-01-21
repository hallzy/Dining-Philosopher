// The philosophers randomly attempt pick up their left, or right fork
// If they fail, they drop the forks that they have and nap.


#include <unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include <stdbool.h>
#include<pthread.h>
#include<semaphore.h>

void Simulate(int n);

#define NAPTIME 25000
#define NPHILOSOPHERS 5
#define NFORKS 5

#define NMEALS 5
#define VERBOSE 1

pthread_t philosopher[NPHILOSOPHERS];
pthread_mutex_t forks[NFORKS];

// Nap for a random length of time between 0 and NAPTIME useconds
void Nap() {
	usleep(random() % NAPTIME);
}

// Release Forks
void ReleaseFork(int n) {
	pthread_mutex_unlock(&forks[n]);
	pthread_mutex_unlock(&forks[(n+1)%5]);
}

// Drop forks when finished eating
void Eat(int n) {
	ReleaseFork(n);
}

// Randomly pick up the left or right fork
int RandomForks(int n, int y) {
	if(y == 0) {	// Pick the left fork
		return pthread_mutex_trylock(&forks[n]);
	}

	else if(y == 1)	{ // Pick Right Fork
		return pthread_mutex_trylock(&forks[(n+1)%5]);
	}
	else	{
		return -1;
	}
}



int main() {
	int i;

	// Initialize philosophers
	for(i = 0; i < NFORKS; i++) {
		pthread_mutex_init(&forks[i],NULL);
	}
	// Create philosophers
	for(i = 0; i < NPHILOSOPHERS; i++) {
		pthread_create(&philosopher[i], NULL,(void *) Simulate, (int *) i); //
	}
	// Do not exit program until all threads are done.
	for( i = 0; i < NPHILOSOPHERS; i++) {
		pthread_join(philosopher[i], NULL);
	}
	for(i = 0; i < NFORKS; i++) {
		pthread_mutex_destroy(&forks[i]);
	}

	printf("\n\nAll Philosophers are Finished Eating %d meals\n\n", NMEALS);

	return 0;
}

void Simulate(int n)
{
	int x = NMEALS;
	int y;
	// Flag to tell if the philosopher picked up the fork or not.
	bool failed = true;
	do {	
		do {
			do {
				// give y a random value 0, or 1 to pick up the left or right fork
				y = random()%2;
				// if they could not pick up the fork, nap and try again
				if(RandomForks(n,y) != 0) {
					if(VERBOSE) {
						printf("Philosopher %d could not pick up first fork\n", n);
					}
					Nap();
				}
				else {
					// otherwise they go the fork... depending on the value of y, either the left or right
					failed = false;
					if(y == 1) {
						if(VERBOSE == 1) {
							printf("Philosopher %d got right Fork, Philosopher %d is napping\n",n, n);
						}
					}
					else if(y == 0){
						if(VERBOSE == 1) {
							printf("Philosopher %d got left Fork, Philosopher %d is napping\n",n, n);
						}
					}
					Nap();
				}
			}while(failed);
			failed = true;
			// if they picked up the left fork, now they try to pick up the right fork, and vice versa
			if(y == 1) {
				y = 0;
			}
			else if (y == 0) {
				y = 1;
			}
			// For the strange case where y got something other than a 0 or 1 somehow
			else {
				if(VERBOSE) {
					printf("error");
				}
			}
			// If they could not pick up the second fork, drop all forks and nap, and start at the beginning
			if(RandomForks(n,y) != 0) {
				if(VERBOSE == 1) {
					printf("Philosopher %d could not pick up second fork. Release Fork\n", n);
				}
				ReleaseFork(n);
				Nap();
			}
			else {
				// Otherwise they got the fork.
				failed = false;
				if(y == 1) {
					if(VERBOSE == 1) {
						printf("Philosopher %d got right fork, Philosopher %d is napping\n",n ,n);
					}
				}
				else if (y == 0) {
					if(VERBOSE == 1) {
						printf("Philosopher %d got left fork, Philosopher %d is napping\n",n ,n);
					}
				}
				Nap();
			}
		}while(failed);
		failed = true;

		if(VERBOSE)
			printf("Philosopher %d is eating, Philosopher %d is napping\n",n,n);
		Nap();
		if(VERBOSE == 1)
			printf("Philosopher %d put both forks down, done eating, Philosopher %d is napping\n",n ,n);
		Eat(n);
		Nap();

		x--;
	}while(x > 0);

}
