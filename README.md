# mt-simulated-annealing
A multi-threaded Simulated Annealing core in C

DEVELOPED FOR (PLEASE, CITE AS SUCH):
Vales et al., "A parallel optimization approach for controlling allele diversity in conservation schemes,"
Mathematical Biosciences, Volume 183, Issue 2, June 2003, Pages 161-173.

This implementation uses a loose temperature control, where each of the k threads run a number of iterations
with a fixed temperature and then notifies to other threads. Upon notification temperature reduces by 
a k-th root factor.

THIS ALGORITHM IS REGARDED AS ONE OF THE ORIGINAL MULTI-THREADING WAYS TO IMPLEMENT SIMULATED ANNEALING

<h2>Library contents</h2>

The library provides function <i>tresult</i> <b>annealing(void *<i>F</i>)</b>



<h2>Files provided</h2>

<ul>
<li> annealing.h: header files for the annealing function
<li> annealing.c: implementation of the multithreading annealing
<li> sampleLP.c: example for solving a generic Linear Program
<li> sampleILP.c: example for solving a generic Integer Linear Program
<li> sample-runtime.c: example for introducing problem type and parameters during run-time
</ul>

<h2>Usage Notes</h2>

The core 


