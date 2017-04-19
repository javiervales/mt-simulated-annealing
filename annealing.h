/* MULTI-THREAD SIMULATED ANNEALING

 DEVELOPED FOR (PLEASE, CITE AS SUCH):
 Vales et al., "A parallel optimization approach for controlling allele diversity in conservation schemes,"
 Mathematical Biosciences, Volume 183, Issue 2, June 2003, Pages 161-173.

 This implementation uses a loose temperature control, where each of the k threadsrun a number of iterations
 with a fixed temperature and then notifies to other threads. Upon notification temperature reduces by 
 a k-th root factor.*/

#ifndef ANNEALING_HH
#define ANNEALING_HH

#include <stdlib.h>
#include <stdio.h>	
#include <stdarg.h>	
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>


#define DEBUG_ANNEALING      // Activate for general debugging
#undef DEBUG_ANNEALING    

#define DEBUG_TIME           // Print execution time when activated
//#undef DEBUG_TIME         

#define MIN 0 
#define MAX 1
#define PROBLEMTYPE MIN         // SET PROBLEM TYPE


struct tfunctions {
	double (*cost)(void *);
	void (*allocate)(void **);
	void (*deallocate)(void **);
	void (*initial)(void *);
        void (*show)(void *, FILE *);
	void (*create)(void *, void *);
	void (*copy)(void *, void*);
};

typedef struct tfunctions tfunctions;

struct tresult {
	void *solution; // A pointer to the solution
	double elapsedtime; 
	double value; 
};

typedef struct tresult tresult;

// NTHREADS: Number of simultaneous thread. Set greater or equal to your number of cores.
// REPETITIONS: Repetitions at each temperature
// ITERATIONS: Number of problem runs from the beggining (could be useful to skip local optima). Set to 1 in case only one run is necessary.
tresult annealing(int NTHREADS, int REPETITIONS, int ITERATIONS, void *F);
void *thread_annealing(void *F);

#endif
