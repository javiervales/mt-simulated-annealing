# mt-simulated-annealing
A multi-threaded Simulated Annealing core in C

DEVELOPED FOR (PLEASE, CITE AS SUCH):
Vales et al., "A parallel optimization approach for controlling allele diversity in conservation schemes,"
Mathematical Biosciences, Volume 183, Issue 2, June 2003, Pages 161-173.

This implementation uses a loose temperature control, where each of the k threads run a number of iterations
with a fixed temperature and then notifies to other threads. Upon notification temperature reduces by 
a k-th root factor.

THIS ALGORITHM IS REGARDED AS ONE OF THE ORIGINAL MULTI-THREADING WAYS TO IMPLEMENT SIMULATED ANNEALING

<h1>USAGE</h1>




