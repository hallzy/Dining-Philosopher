//Pick up the left forks first, then the right. If the right is not available
//then wait for it to become available.

// note that this is just an exercise and does not work. This program goes into
// Deadlock because all philosophers are holding a fork, leaving no forks left
// But a fork is never unlocked until the philosopher has finished eating...
// Requiring two forks.

#include <unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>

void Simulate(int n);

#define NAPTIME 25000
#define NPHILOSOPHERS 5
#define NFORKS 5

#define NMEALS 5
#define VERBOSE 1

//Defines NPHILOSOPHERS number of philosopher threads in an array.
pthread_t philosopher[NPHILOSOPHERS];
// Defines a mutex for each of the NFORKS forks, so that we can lock them
pthread_mutex_t forks[NFORKS];

// A nap function that makes the philosophers nap for a random length of time
// That is less than NAPTIME
void Nap() {
	usleep(random() % NAPTIME);
}

// Pick up the left fork, by locking it. This means that no other philosopher
// Can take this fork now.
void LockLeftFork(int n) {
	pthread_mutex_lock(&forks[n]);
}

// Same as the previous, but for the right fork this time
void LockRightFork(int n) {
	pthread_mutex_lock(&forks[(n+1)%5]);
}

// After finished eating put down both forks for others to use.
void Eat(int n) {
	pthread_mutex_unlock(&forks[n]);
	pthread_mutex_unlock(&forks[(n+1)%5]);
}


int main() {
	int i;

	// Initializes NFORKS number of Philosophers
	for(i = 0; i < NFORKS; i++) {
		pthread_mutex_init(&forks[i],NULL);
	}
	// Creates a thread for each philosopher and executes the simulate function
	// the i argument passed to Simulate is the number of the fork to that
	// philosophers left. (i+1)%5 is the number of the fork to there right.
	for(i = 0; i < NPHILOSOPHERS; i++) {
		pthread_create(&philosopher[i], NULL, (void *) Simulate, (int *) i);
	}
	Nap();
	Nap();
	// Do not exit proram until all threads are finished.
	for( i = 0; i < NPHILOSOPHERS; i++) {
		pthread_join(philosopher[i], NULL);
	}
	// The program will not pass this point until all philosophers have eaten
	// NMEALS
	for(i = 0; i < NFORKS; i++) {
		pthread_mutex_destroy(&forks[i]);
	}
	return 0;
}

void Simulate(int n) {
	// We need to operate on the number of meals, so we give a variable the value
	// of the defined constant.
	int x = NMEALS;
	// Do all of this until the philosopher has eaten all their meals
	do {
		// The philosopher will stay in this function until they are able to grab
		// their left fork
		LockLeftFork(n);
		// Once they grab the fork print a message saying so
		if(VERBOSE == 1) {
			printf("Philosopher %d got left fork\nPhilosopher %d is napping\n",n ,n);
		}
		// Nap after picking up the fork.
		Nap();
		// After napping attempt to pick up the right fork. If they cant, they keep
		// trying
		LockRightFork(n);
		// if/ when the get the right fork, say so
		if(VERBOSE == 1) {
			printf("Philosopher %d got right fork\nPhilosopher %d is napping\n",n ,n);
		}
		// After getting the fork, nap
		Nap();
		// The philosopher now has both forks and can eat
		if(VERBOSE == 1) {
			printf("Philosopher %d is eating\nPhilosopher %d is napping\n",n,n);
		}
		// After eating nap again.
		Nap();
		if(VERBOSE == 1) {
			printf("Philosopher %d put both forks down, finished eating\nPhilosopher %d is napping\n",n ,n);
		}
		// When they are done eating put down both forks
		Eat(n);
		// and nap
		Nap();
		// The philosopher now has 1 less meal to eat.
		x--;
	}while(x > 0);

}
