//Pick up the Lowest indexed fork first, then the higher, for each
//Philosopher. The philosophers will attempt to lock the forks.
// If they cannot, they nap and try again later.


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
#define VERBOSE 1 // Displays everything philosophers do when = 1

pthread_t philosopher[NPHILOSOPHERS];
pthread_mutex_t forks[NFORKS];

// Naps for a random length of time between 0 and NAPTIME useconds
void Nap() {
	usleep(random() % NAPTIME);
}

// try to Lock the lowest index fork for philosopher n
int LockLowestFork(int n) {
	if( n < ((n+1)%5) ) {
		return pthread_mutex_trylock(&forks[n]);
	}
	else if( n > ((n+1)%5) ) {
		return pthread_mutex_trylock(&forks[(n+1)%5]);
	}
	else {
		if(VERBOSE) {
			printf("error");
		}
	}
	return -1;
}

// Try to lock the highest index fork for philosopher n
int LockHigherFork(int n) {
	if( n > ((n+1)%5) ) {
		return pthread_mutex_trylock(&forks[n]);
	}
	else if( n < ((n+1)%5) ) {
		return pthread_mutex_trylock(&forks[(n+1)%5]);
	}
	else {
		if(VERBOSE) {
			printf("error");
		}
	}
	return -1;
}

// drop the forks when done eating
void Eat(int n) {
	pthread_mutex_unlock(&forks[n]);
	pthread_mutex_unlock(&forks[(n+1)%5]);
}

int main() {
	int i;

	// Initialize the philosopher threads
	for(i = 0; i < NFORKS; i++) {
		pthread_mutex_init(&forks[i],NULL);
	}
	// Create the philosopher threads
	for(i = 0; i < NPHILOSOPHERS; i++) {
		pthread_create(&philosopher[i], NULL, (void *) Simulate,(int *) i);
	}
	// Do not exit program until all threads are finished.
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
	// Flag that determines if the philosopher needs to try to get a fork again
	bool failed = true;
	do {
		do {
			// if the philosopher failed to get the fork, nap and try again
			// otherwise, continue (failed is now false, so exit loop)
			if(LockLowestFork(n) != 0) {
				Nap();
			}
			else {
				failed = false;
			}
		}while(failed);
		
		//Reinitialize
		failed = true;
		if(VERBOSE) {
			printf("Philosopher %d got first fork\nPhilosopher %d is napping\n",n ,n);
		}
		Nap();
		do{
			// If the philosopher failed to get the second fork, nap and try again.
			// Otherwise continue.
			if(LockHigherFork(n) != 0) {
				Nap();
			}
			else {
				failed = false;
			}
		}while(failed);

		//reinitialize
		failed = true;

		if(VERBOSE) {
			printf("Philosopher %d got second fork\nPhilosopher %d is napping\n",n ,n);
		}
		Nap();
		if(VERBOSE) {
			printf("Philosopher %d is eating\nPhilosopher %d is napping\n",n,n);
		}
		Nap();
		if(VERBOSE) {
			printf("Philosopher %d put both forks down, finished eating\nPhilosopher %d is napping\n",n ,n);
		}
		Eat(n);
		Nap();

		x--;
	}while(x > 0);

}
