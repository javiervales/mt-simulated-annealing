// SAMPLE PROBLEM FOR ANNEALING
//
// LINEAR PROGRAM
//
// min a'x, s.t. x>=0, b'x<=c, c>=0 
// where a, b, c and its dimension D are problem parameters introduced at runtime

#include <annealing.h>

#define DEBUG_FUNCTIONS
#undef DEBUG_FUNCTIONS

// GLOBAL VARIABLES REQUIRED FOR THE PROBLEM
int D; // Problem dimension
float *a, *b, c; // Problem parameters

// AN STRUCTURE WHICH CONTAINS ALL VARIABLES OF A SOLUTION 
struct tsolution {
	double *x;
};

typedef struct tsolution tsolution;

double cost(void *solution);
void allocate(void **solution);
void deallocate(void **solution);
void initial(void *newsolution);
void show(void *solution, FILE *F);
void createLP(void *currentsolution, void *newsolution);
void createILP(void *currentsolution, void *newsolution);
void copy(void *currentsolution, void *newsolution);

int main(int argc, char **argv) {

	// PROBLEM PARAMETERS 
	tfunctions F;
	tresult r;
	int type, j;

	F.cost = *cost;
	F.allocate = *allocate;
	F.deallocate = *deallocate;
	F.initial = *initial;
	F.show = *show;
	F.copy = *copy;

	do {
		printf("Introduce problem type and parameters. Press CTRL+C to stop\n");

		printf("Set problem type (0 Linear program/1 Integer Linear program): ");
		scanf("%d", &type);

		if (type) {
			F.create = *createILP;
		} else {
			F.create = *createLP;
		}

		printf("Set problem dimension (D): ");
		scanf("%d", &D);

		a = malloc(D*sizeof(float));
		b = malloc(D*sizeof(float));
		for(j=0;j<D;j++)  {
			printf("Set problem parameter (a[%d]): ", j);
			scanf("%f", &a[j]);
		}

		for(j=0;j<D;j++)  {
			printf("Set problem parameter (b[%d]): ", j);
			scanf("%f", &b[j]);
		}

		printf("Set problem parameter (c): ");
		scanf("%f", &c);
		printf("\n");

		r = annealing(10, 1000, 10, &F);  // It can take a while depending on the problem... 

		printf("Sol: %f, Elapsed time: %f\n", r.value, r.elapsedtime);
		show(r.solution, stdout);
		deallocate(&r.solution); // Deallocate when no longer needed
		free(a);
		free(b);
	} while(1);

	return (0);
}



//
// FUNCTIONS FOR YOUR PARTICULAR PROBLEM
//
double cost(void *solution) {

	double aux = 0.0;
	int j; 

	for(j=0;j<D;j++) {
		aux += ((tsolution *)(solution))->x[j]*a[j];
	}

	return aux;
}

void allocate(void **solution) {
	*solution = (void *)malloc(sizeof(tsolution));
	((tsolution *)(*solution))->x = (double *)malloc(D*sizeof(double));
}

void deallocate(void **solution) {
	free(((tsolution *)(*solution))->x);
	free(*solution);
	*solution = NULL;
}


void initial(void *newsolution) {

	int j;

	for(j=0;j<D;j++) {
		((tsolution *)(newsolution))->x[j] = 0.0;  // Note it fulfills constraint b'x == 0 < c
	}
}

void show(void *solution, FILE *F) {

	int j;

	fprintf(F,"x=[");
	for(j=0;j<D;j++) {
		fprintf(F,"%f, ", ((tsolution *)(solution))->x[j]);
	}
	fprintf(F,"]\n");
}

void createLP(void *currentsolution, void *newsolution) {

	int position, j; 
	double aux;
	float delta;

	while (1) {
		copy(currentsolution, newsolution);
		position =   (int) ((double)D * (rand() / ((float)RAND_MAX)));
		delta = 0.2 * (rand() / ((float)RAND_MAX )) - (float)0.1;
		((tsolution *)(newsolution))->x[position] += delta;
		
		// Check constraints
		if (((tsolution *)(newsolution))->x[position]<0) continue; 
		aux = 0.0;
		for(j=0;j<D;j++) {
			aux += b[j]*((tsolution *)(newsolution))->x[j];
		}
		if (aux<=c) break; // Constraints fulfilled
	}
	
#ifdef DEBUG_FUNCTIONS
	show(newsolution, stdout);
#endif
}

void createILP(void *currentsolution, void *newsolution) {

	int position, j; 
	double aux;
	float delta;

	while (1) {
		copy(currentsolution, newsolution);
		position =   (int) ((double)D * (rand() / ((float)RAND_MAX)));
		delta = (rand() / ((float)RAND_MAX ));
		if (delta>0.5) {
			((tsolution *)(newsolution))->x[position] ++;
		} else {
			((tsolution *)(newsolution))->x[position] --;
		}
		
		// Check constraints
		if (((tsolution *)(newsolution))->x[position]<0) continue; 
		aux = 0.0;
		for(j=0;j<D;j++) {
			aux += b[j]*((tsolution *)(newsolution))->x[j];
		}
		if (aux<=c) break; // Constraints fulfilled
	}
	
#ifdef DEBUG_THREAD
	show(newsolution, stdout);
#endif
}

void copy(void *currentsolution, void *newsolution) {
	memcpy( ((tsolution *)newsolution)->x, ((tsolution *)currentsolution)->x, D*sizeof(double));
}
