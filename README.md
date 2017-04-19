# mt-simulated-annealing
A multi-threaded Simulated Annealing core in C

DEVELOPED FOR (PLEASE, CITE AS SUCH):
Vales et al., "A parallel optimization approach for controlling allele diversity in conservation schemes,"
Mathematical Biosciences, Volume 183, Issue 2, June 2003, Pages 161-173.

This implementation uses a loose temperature control, where each of the k threads run a number of iterations
with a fixed temperature and then notifies to other threads. Upon notification temperature reduces by 
a k-th root factor.

THIS ALGORITHM IS REGARDED AS ONE OF THE ORIGINAL MULTI-THREADING WAYS TO IMPLEMENT SIMULATED ANNEALING


<h2>Files provided</h2>

<ul>
<li> annealing.h: header files for the annealing function
<li> annealing.c: implementation of the multithreading annealing
<li> sampleLP.c: example for solving a generic Linear Program
<li> sampleILP.c: example for solving a generic Integer Linear Program
<li> sample-runtime.c: example for introducing problem type (LP or ILP) and parameters at run-time
</ul>

<h2>Library contents</h2>

The library provides a single function implementing the multi-threading simulated annealing:<br>

```c
tresult annealing(int nthreads, int repetitions, int iterations, void *program);
```

This function <b>minimizes</b> the given problem/program (SEE NOTES BELOW TO CHANGE TO MAXIMIZATION), whose arguments are:

<ol>
<li> nthreads: Number of simultaneous threads to use. Set it greater or equal than the available number of cores.
<li> repetitions: Repetitions at each temperature. Increment for more exhaustive search. 
<li> iterations: Number of independent problem runs to be performed (could be useful to set greater than 1 to skip local optima). 
<li> program: A struct containing the user-provided functions to allocate, deallocate, initiate, create, evaluate cost, copy, and show program solutions.
</ol>

Program structure is declared as 

```c
struct tprogram {
        double (*cost)(void *);
        void (*allocate)(void **);
        void (*deallocate)(void **);
        void (*initial)(void *);
        void (*show)(void *, FILE *);
        void (*create)(void *, void *);
        void (*copy)(void *, void*);
};
```

Each of these functions must be user-provided, according to the optimization problem to solve. Please see below for function definitions.

Besides, the annealing function returns a structure:

```c
struct tresult {
        void *solution; // A pointer to the solution
        double elapsedtime;
        double value;
};
```

which contains: 

<ol>
<li> *solution: a user-defined structure with solution's variables   
<li> value: the cost associated to the solution
<li> elapsedtime: the time required to find the solution (in seconds)
</ol>

<h2>Optimization problem definition</h2>

<b>Please see provided files (sampleLP, sampleILP, sample-runtime) for examples of program definitions.</b>

Optimization program is defined by providing the following structure and functions:

```c
struct tsolution {
        // All the variables required to define the problem come here
};
```
<hr>

```c
double cost(void *solution); 
```

It computes the given solution's cost.

<hr>

```c
void create(void *currentsolution, void *newsolution) {

        char constraintsfulfilled = 0;
        
        // Common template for this function
        while (!constraintsfulfilled) {
                copy(currentsolution, newsolution); // Possibly it starts by creating a copy of the current solution
                ...
                
        }
}
```
It creates a new solution to be tested for the optimization problem. It receives also the current considered solution of the annealing algorithm, which the function may use to create a modified one. This function is in charge to check solution's conformity to problem's constraints, i.e. it can only submit valid solutions.
<hr>

```c
void initial(void *newsolution);
```

It creates the first solution for the problem. As the previous function it can only returns a solution fulfilling all problem's constraints. 
<hr>

```c
void allocate(void **solution);
```
It must allocate a solution and its components (by issuing all necessary malloc calls). It is internally called by the annealing function. 
<hr>

```c
void deallocate(void **solution);
```
It must free all the resources allocated by a solution. It is internally called by the annealing function. 
<hr>


```c
void show(void *solution, FILE *F);
```
It displays solution at the given FILE (e.g. <i> stderr, stdout, ... </i>. 

```c
void copy(void *currentsolution, void *newsolution);
```
It copies all the internal components of a solution strcuture (currensolution) to another solution strcture (newsolution). It is internally called by the annealing function, and can be used by other problem's functions like create as shown above.

<h2>Notes</h2>

<h3> Minimization/Maximization </h3>

By default annealing seeks problem's minimum. To find maximum, you can either:

<ul>
<li> Invert cost's function result, e.g. instead of 

```c
return value;
```

use

```c
return -value;
```

and take into account that problem's final value will be also inverted. 

<li> Change:

```c
#DEFINE PROBLEMTYPE MIN
```

to:

```c
#DEFINE PROBLEMTYPE MAX
```
</ul>


