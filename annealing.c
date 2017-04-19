/* MULTI-THREAD SIMULATED ANNEALING

 DEVELOPED FOR (PLEASE, CITE AS SUCH):
 Vales et al., "A parallel optimization approach for controlling allele diversity in conservation schemes,"
 Mathematical Biosciences, Volume 183, Issue 2, June 2003, Pages 161-173.

 This implementation uses a loose temperature control, where each of the k threadsrun a number of iterations
 with a fixed temperature and then notifies to other threads. Upon notification temperature reduces by 
 a k-th root factor.*/

#include <annealing.h>

// Variables shared among threads 
// solution y fcost -> Currently accepted solution
// solopt y fcostopt -> Best solution found

void *solution, *solopt, *solglobalopt;
double fcost, fcostopt, fcostglobalopt, KT, kthread;
int repMAX;
int nivMAX;

// Semaphores control
sem_t lsem_solution, z_solution, x_solution, y_solution, esem_solution, sem_solopt, sem_temperatura;
int contesc_solution = 0, contlect_solution = 0;

tresult annealing(int nthreads, int repetitions, int iterations, void *program) {

	int I, iter; 
	double t=0.1, k=0.9;
	double t1, t2;
	struct timeval hora;
	pthread_t *thread;
	tresult result;

	gettimeofday(&hora, NULL); //time(NULL);
	t1 = ((double)hora.tv_sec)*1000+((double)hora.tv_usec)/1000;

	(* ((tprogram *)program)->allocate)(&solution);
	(* ((tprogram *)program)->allocate)(&solopt);
	(* ((tprogram *)program)->allocate)(&solglobalopt);

	for(iter=0;iter<iterations;iter++) {

#ifdef DEBUG_ANNEALING
		fprintf(stderr, "Iter #%d\n\n", iter);
#endif
		t=0.1, k=0.9;

		if((thread=(pthread_t *)malloc(nthreads*sizeof(pthread_t)))==NULL) {
			fprintf(stderr,"No enough memory\n");
			exit(-1);
		}

#ifdef DEBUG_ANNEALING
		fprintf(stderr,"Setting up annealing\n");
#endif

		KT = t/k;
		kthread = pow(k,1.0/nthreads);
		repMAX = repetitions/nthreads;
		nivMAX = 1000;

		/* Initial solution setup */
		(* ((tprogram *)program)->initial)(solution);
#ifdef DEBUG_ANNEALING
		(* ((tprogram *)program)->show)(solution, stdout);
#endif

		fcostopt = fcost = (* ((tprogram *)program)->cost)(solution);
		if(!iter) fcostglobalopt = fcostopt;
	 	(* ((tprogram *)program)->copy)(solution, solopt);
#ifdef DEBUG_ANNEALING
		fprintf(stderr,"Initial cost: %f, repMAX: %d\n", fcost, repMAX);
#endif

		/* Synchoronization variables */
		if(sem_init(&sem_solopt, 0, 1) == -1) {
			fprintf(stderr,"Error creating semaphore\n");
			exit(-1);
		}
		if(sem_init(&lsem_solution, 0, 1) == -1) {
			fprintf(stderr,"Error creating semaphore\n");
			exit(-1);
		}
		if(sem_init(&esem_solution, 0, 1) == -1) {
			fprintf(stderr,"Error creating semaphore\n");
			exit(-1);
		}
		if(sem_init(&x_solution, 0, 1) == -1) {
			fprintf(stderr,"Error creating semaphore\n");
			exit(-1);
		}
		if(sem_init(&y_solution, 0, 1) == -1) {
			fprintf(stderr,"Error creating semaphore\n");
			exit(-1);
		}
		if(sem_init(&z_solution, 0, 1) == -1) {
			fprintf(stderr,"Error creating semaphore\n");
			exit(-1);
		}
		if(sem_init(&sem_temperatura, 0, 1) == -1) {
			fprintf(stderr,"Error creating semaphore\n");
			exit(-1);
		}


		/* THREADS LAUNCH */
		for(I=0;I<nthreads;I++) {
#ifdef DEBUG_ANNEALING
			fprintf(stderr,"Thread %d created\n", I);
#endif
			pthread_create (thread+I, NULL, (void *) &thread_annealing, program);
		}

		/* WAIT FOR THREADS ENDING */
		for(I=0;I<nthreads;I++) {
			pthread_join (thread[I], NULL);
#ifdef DEBUG_ANNEALING
			fprintf(stderr,"Thread %d finished\n", I);
#endif
		}

		/* FREE VARIABLES */
		free(thread);
		sem_destroy(&lsem_solution);
		sem_destroy(&esem_solution);
		sem_destroy(&x_solution);
		sem_destroy(&y_solution);
		sem_destroy(&z_solution);
		sem_destroy(&sem_solopt);
		sem_destroy(&sem_temperatura);

		if( (PROBLEMTYPE==MAX && fcostglobalopt<fcostopt) || (PROBLEMTYPE==MIN && fcostglobalopt>fcostopt) ) {
			(* ((tprogram *)program)->copy)(solopt, solglobalopt);
			fcostglobalopt = fcostopt;
		}
	}

	gettimeofday(&hora, NULL); //time(NULL);
	t2 = ((double)hora.tv_sec)*1000+((double)hora.tv_usec)/1000;
	result.elapsedtime = (t2-t1)/1000;
	result.value = fcostglobalopt;
	(* ((tprogram *)program)->allocate)(&result.solution);
	(* ((tprogram *)program)->copy)(solglobalopt,result.solution);

#ifdef DEBUG_ANNEALING
	fprintf (stderr, "Tiempo: %f  Fcoste: %f  KT:%f\n", result.elapsedtime, result.value, KT);
	(* ((tprogram *)program)->show)(solglobalopt,stderr);
#endif

	(* ((tprogram *)program)->deallocate)(&solution);
	(* ((tprogram *)program)->deallocate)(&solopt);
	(* ((tprogram *)program)->deallocate)(&solglobalopt);

	return result;
} 

void *thread_annealing(void *program) {

        int niv, rep, numero_cambios=1;
	double delta, omega, KT_thread, uniforme;
	char cambio;

	// Alternative solution
	void *sol_thread;  // Thread solution
	double fcost_thread = 0;
	void *aux;   // Auxiliar solution
	double fcostaux;

	(* ((tprogram *)program)->allocate)(&sol_thread);
	(* ((tprogram *)program)->allocate)(&aux);

	// Random seed, different for each thread
	srand(time(NULL)+(unsigned int)pthread_self());
	
	/* Copy current to alternative solution in mutual exclusion */ 
	sem_wait(&esem_solution);
	(* ((tprogram *)program)->copy)(solution, sol_thread);
	fcost_thread = fcost;
	sem_post(&esem_solution);

	/* LEVELS LOOP */ 
	for(niv=0;niv<nivMAX && numero_cambios;niv++) {
		/* Actualizamos la temperatura global del algoritmo */
		sem_wait(&sem_temperatura);
		KT *= kthread;
		KT_thread = KT; 
		sem_post(&sem_temperatura);
#ifdef DEBUG_ANNEALING
		//fprintf(stderr,"niv: %d, KT: %f ", niv, KT);
#endif 
		/* Si otro thread cambio la solution, cambiamos nuestra solution alternativa a esa */
		/* Lo hacemos con semaforos en exclusion mutua segun el paradigma de los lectores/escritores 
		 * con prioridad de los escritores (William Stallins, Sistemas Operativos, pagina 212) */
		sem_wait(&z_solution);
		sem_wait(&lsem_solution);
		sem_wait(&x_solution);
		contlect_solution++;
		if(contlect_solution==1) {
			sem_wait(&esem_solution);
		}
		sem_post(&x_solution);
		sem_post(&lsem_solution);
		sem_post(&z_solution);
			
		if(fcost!=fcost_thread) {
			fcost_thread = fcost;
			(* ((tprogram *)program)->copy)(solution, sol_thread);
		}
		
		sem_wait(&x_solution);
		contlect_solution--;
		if(contlect_solution==0) {
			sem_post(&esem_solution);
		} 
		sem_post(&x_solution);
		
		numero_cambios = 0;
		
		// MAIN SIMULATED ANNEALING PART
		for(rep=0;rep<repMAX;rep++) {

			// Create new solution
			(* ((tprogram *)program)->create)(sol_thread, aux); 
			fcostaux = (* ((tprogram *)program)->cost)(aux); 

			// Do we use new solution or the former one
			cambio = 0;
			if (PROBLEMTYPE == MAX) { // ADJUST FOR MIN OR MAX
				delta = fcost_thread - fcostaux;   
			} else {
				delta = fcostaux - fcost_thread;
			}

			if(delta<0) {
				delta=0;
			}
			omega = exp(-delta/KT_thread);

			if(omega>=1) {
				cambio = 1;
			} else {
				uniforme = rand() / ((double)RAND_MAX + (double)1.0);
				if (uniforme<omega) {
					cambio = 1;
				}
			}

			if(!cambio) {
				// Go back to the last solution
				continue;
			}

			// Use new solution instead
			sem_wait(&y_solution);
			contesc_solution++;
			if(contesc_solution==1) {
				sem_wait(&lsem_solution);
			}
			sem_post(&y_solution);
			sem_wait(&esem_solution);

			fcost_thread = fcost = fcostaux;
			(* ((tprogram *)program)->copy)(aux, sol_thread);
			(* ((tprogram *)program)->copy)(sol_thread, solution);

			sem_post(&esem_solution);
			sem_wait(&y_solution);
			contesc_solution--;
			if(contesc_solution==0) {
				sem_post(&lsem_solution);
			}
			sem_post(&y_solution);

			// If new is better than the optimal we update optimal
			sem_wait(&sem_solopt);
			if( (PROBLEMTYPE==MAX && fcostaux>fcostopt) || (PROBLEMTYPE==MIN && fcostaux<fcostopt) ) {
#ifdef DEBUG_ANNEALING
			       	fprintf(stderr,"Fcosteopt: %f, ", fcostaux);
#endif
				fcostopt = fcostaux;
				(* ((tprogram *)program)->copy)(sol_thread, solopt);

#ifdef DEBUG_ANNEALING
				// Show solution
				(* ((tprogram *)program)->show)(solopt, stderr);
#endif
			}	
			sem_post(&sem_solopt);

			numero_cambios++;
		}		
	}
	// If there are not changes at this temperature we stop iterating 

	(* ((tprogram *)program)->deallocate)(&sol_thread);
	(* ((tprogram *)program)->deallocate)(&aux);


	return NULL;
}
