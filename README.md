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
<li> sample-runtime.c: example for introducing problem type and parameters during run-time
</ul>

<h2>Library contents</h2>

The library provides a single function implementing the multi-threading simulated annealing:<br>

```c
tresult annealing(int NTHREADS, int REPETITIONS, int ITERATIONS, void *F);
```

This function <b>minimizes</b> the given problem (SEE NOTES BELOW TO CHANGE TO MAXIMIZATION), whose arguments are:

<ol>
<li> NTHREADS: Number of simultaneous threads to use. Set it greater or equal than the available number of cores.
<li> REPETITIONS: Repetitions at each temperature. Increment for more exhaustive search. 
<li> ITERATIONS: Number of independent problem runs to be performed (could be useful to set greater than 1 to skip local optima). 
<li> F: A struct containing the user-provided functions to allocate, deallocate, initiate, create, evaluate cost, copy, and show problem solutions (SEE BELOW)
</ol>

Besides, it returns a structure:

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
<li> value: the cost associated to the solution.
<li> elapsedtime: the time required to find the solution (in seconds)
</ol>

<h2>Usage Notes</h2>

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


