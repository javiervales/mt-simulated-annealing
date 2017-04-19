// SAMPLE PROBLEM FOR ANNEALING
//
// LINEAR PROGRAM
//
// min a'x, s.t. x>=0, b'x<=c, c>=0  where a, b, c and its dimension D are problem parameters

#include <annealing.h>

// PROBLEM PARAMETERS 
// CHANGE AS YOU MAY NEED IT
#define D 10
float a[]={-0.1, 0.1, 1.2, 1.0, 0.3, 0.7, -0.1, -0.1, -0.1, 1.8};
float b[]={1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
float c=10.0;

// AN STRUCTURE WHICH CONTAINS ALL VARIABLES OF A SOLUTION 
struct tsolution {
	double x[D];
};

typedef struct tsolution tsolution;

double cost(void *solution);
void allocate(void **solution);
void deallocate(void **solution);
void initial(void *newsolution);
void show(void *solution, FILE *F);
void create(void *currentsolution, void *newsolution);
void copy(void *currentsolution, void *newsolution);

int main(int argc, char **argv) {

	tfunctions F;
	tresult r;

	F.cost = *cost;
	F.allocate = *allocate;
	F.deallocate = *deallocate;
	F.initial = *initial;
	F.show = *show;
	F.create = *create;
	F.copy = *copy;

	r = annealing(2, 1000, 20, &F);

	printf("Sol: %f, Elapsed time: %f\n", r.value, r.elapsedtime);
	show(r.solution, stdout);
	deallocate(&r.solution);

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
}

void deallocate(void **solution) {
	free(*solution);
	*solution = NULL;
}


void initial(void *newsolution) {

	int j;

	for(j=0;j<D;j++) {
		((tsolution *)(newsolution))->x[j] = 0.0;  // Note it fulfills constraint b'x >= 0
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

void create(void *currentsolution, void *newsolution) {

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
	
#ifdef DEBUG_THREAD
	show(newsolution, stdout);
#endif
}

void copy(void *currentsolution, void *newsolution) {
	memcpy( ((tsolution *)newsolution)->x, ((tsolution *)currentsolution)->x, D*sizeof(double));
}
