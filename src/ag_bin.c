/*
* ag.c
* 
* Copyright 2012 derik <dersilva@inf.ufpr.br>
* 
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
* MA 02110-1301, USA.
* 
* 
*/
/**
* 
* Genetic Algorithm Shell
* 
* The default implementation evolve to find a solution to the given 
* problem:
* 		y = max(sum(v))
* 		Where v is an integer vector and v[i] = {0, 1}
* 
*/
/**
* @TODO Apply tournament in Crossover Selection
* @TODO Apply Elitism on next generation
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <omp.h>
#include <math.h> 

/**
* Individual struct 
*/
typedef struct individual {
	int *v;
} individual;

/**
* Population struct 
*/
typedef struct population {
	individual *i;
} population;

/** Define initial parameters **/
#define MAX_POP_SIZE 400
#define IND_SIZE 20
#define MAX_GENERATIONS 3000
#define TOURNAMENT 4
#define MUTATION_PROB 0.1

individual* changeIndividuals(individual* i, int origin, int destiny);

/** Empty population memory allocation **/
population* popalloc(population *pop, int popsize) {
	int i;
	if((pop->i = malloc(popsize * sizeof(individual)))) {
		for(i = 0; i < popsize; i++) {
			if(!(pop->i[i].v = malloc(IND_SIZE * sizeof(int)))) {
				break;
			}
		}
	}
	return pop;
}

/** Free a population **/
int popfree(population *pop, int popsize) {
	int i;
	for(i = 0; i < popsize; i++) {
		free(pop->i[i].v);
	}
	free(pop->i);
	return 1;
}

/** Allocation of memory to the population and set initial random values **/
population* initialize(population *pop, int popsize) {
	int i;
	if(popalloc(pop, popsize)) {
		for(i = 0; i< popsize; i++) {
			int j;
			for(j = 0; j < IND_SIZE; j++)
				pop->i[i].v[j] = rand() % 2;
		}
	}
	return pop;
}

/** Return a random individual **/
individual getIndividual(population p) {
	int a = rand();
	int index = a % MAX_POP_SIZE;
	return p.i[index];
}

int binToDec(int i, individual ind) {
	int k = i;
	int j = k;
	int pot = 0;
	int dec;
	dec = (ind.v[k] == 0) ? 0 : -512;
	for (i = i + 9; i > k; i--) {
		dec += ind.v[i] * pow(2, pot++);
	}
	return dec;
}

double fitness(individual ind) {
	int i;
	double sum = 0;
	for(i = 0; i < IND_SIZE; i+=10) {
		double v = binToDec(i, ind)/100.0;
		sum += v * v;
		/*sum += ((v * v) - 10 * cos(M_PI * 2 * v));*/
	}
	/*return 10 * 20 + sum;*/
	return sum;
}


void printIndividual(individual p) {
	int i;
	if (fitness(p) == 0) {
		printf("-------->");
	}
	for(i = 0; i < IND_SIZE; i+= 10) {
		printf("%.2lf ", binToDec(i, p)/100.0);
			/*printf("%d ",p.v[i]);*/
	}
	return;
}
void printPop(population pop) {
	int i;
	for (i = 0; i < MAX_POP_SIZE; i++) {
		printf("%d - ", i);
		printIndividual(pop.i[i]);
		puts("");
	}
}

void applyCrossover(individual *c1, individual *c2, int popsize, individual p1, individual p2) {
	int cindex = rand() % IND_SIZE;
	int cindex2 = rand() % IND_SIZE;
	individual *x, *y;
	int i;
/*	if(cindex > cindex2) {
*/		x = &p1;
		y = &p2;
/*	}
	else {
		x = &p2;
		y = &p1;
	}*/

	c1->v = (int *) malloc(IND_SIZE * sizeof(int));
	c2->v = (int *) malloc(IND_SIZE * sizeof(int));
	if(!c1->v || !c2->v) {
		puts("Memory Error");
		exit(1);
	}

	for(i = 0; i < IND_SIZE; i++) {
		if(i == cindex) {
			x = &p2;
			y = &p1;
		}
/*		if(i == cindex2) {
			x = &p1;
			y = &p2;
		}*/
		c1->v[i] = x->v[i];
		c2->v[i] = y->v[i];
	}

	return;
}

void cloneIndividual(individual *dest, individual src) {
	int i;
	for(i = 0; i< IND_SIZE; i++) {
		dest->v[i] = src.v[i];
	}

	return;
}
 double range_rand(double max, double min) {

 	double  out = ((double)random()/ (RAND_MAX)) * (max - min) + min;

 	return out;
 }

void mutate(individual *ind) {
	int i;
	for(i = 0; i < IND_SIZE; i++) {
		double prob = range_rand(0, 1);
		if (prob < MUTATION_PROB) {
			ind->v[i] = (ind->v[i] == 0) ? 1 : 0;
		}
	} 

	return;
}



int is_best(individual i, individual candidate) {
	double ind = fitness(i);
	double cand = fitness(candidate);
	
	ind = (ind < 0) ? -1 * ind : ind;
	cand = (cand < 0) ? -1 * cand : cand;
	
	if (cand < ind) {
    /*printf("Individuo melhor. Cand: %.7lf Ind: %.7lf\n", cand, ind);*/
		return 1;
	} else {
		return 0;
	}
} 

individual* applyTournament(population pop) {
	individual i1 = getIndividual(pop);
	individual i2 = getIndividual(pop);
	individual i3 = getIndividual(pop);
	individual i4 = getIndividual(pop);

	individual* ret = (individual*) malloc(sizeof(individual) * 4);

	*(ret) = i1;
	*(ret + 1) = i2;
	*(ret + 2) = i3;
	*(ret + 3) = i4;

	double *fc = (double *) malloc(sizeof(double)*4);
	*(fc) = fitness(i1);
	*(fc + 1) = fitness(i2);
	*(fc + 2) = fitness(i3);
	*(fc + 3) = fitness(i4);

	int i;
	for (i = 0; i < 4; i++) {
		int min = i;
		int j;
		for (j = i + 1; j < 4; j++) {		
			if (*(fc + j) < *(fc + min)) {
				min = j;
			}
		}
		if (min != i) {
			changeIndividuals(ret, i, min);
		}

	}
	/*if(is_best(*(ret),*(ret + 1))) {
		ret = changeIndividuals(ret, 0, 1);
	}

	if(is_best(*(ret + 2),*(ret + 3))) {
		ret = changeIndividuals(ret, 2, 3);
	} 

	if(is_best(*(ret),*(ret + 2))) {
		ret = changeIndividuals(ret, 0, 2);
	}

	if(is_best(*(ret + 1),*(ret + 3))) {
		ret = changeIndividuals(ret, 1, 3);
	}           

	if(is_best(*(ret + 1), *(ret + 2))) {
		ret = changeIndividuals(ret, 1, 2);
	}*/



	return ret;

}

individual* changeIndividuals(individual* i, int origin, int destiny) {
	individual temp;

	temp = *(i + destiny);

	*(i + destiny) = *(i + origin);
	*(i + origin) = temp;

	return i;

}


int main(int argc, char **argv) {
	srand(time(NULL));

	int popsize = MAX_POP_SIZE;
	int breed = popsize / 2;
	int generation = 0;
	individual best;
	population pop;

	if(!initialize(&pop, popsize)) {
		puts("Memory Error");
		exit(1);
	}
	best.v = malloc(IND_SIZE * sizeof(individual));
	if(!best.v) {
		puts("Memory Error");
		exit(1);
	}

	cloneIndividual(&best, pop.i[0]);
	int fbest = fitness(best);

	int best_generation = generation;

	for (generation = 0; generation < MAX_GENERATIONS; generation++) {
		int i, j = 0;		
		// population q;
		// if(!popalloc(&q, popsize)) {
		// 	puts("Memory Error");
		// 	exit(1);
		// }

		for(i = 0; i < breed; i++) {
			individual c1, c2;
			individual* tournament = applyTournament(pop);

			applyCrossover(&c1, &c2, popsize, *tournament, *(tournament + 1));

			/*applyCrossover(&c1, &c2, popsize, getIndividual(pop), getIndividual(pop));*/

			mutate(&c1);
			mutate(&c2);


			cloneIndividual(&pop.i[j++], c1);
			cloneIndividual(&pop.i[j++], c2);
			free(c1.v); free(c2.v);  
			free(tournament);
		}

		// popfree(&pop, popsize);
		// pop = q;

		if(is_best(pop.i[0], best))  {   
			cloneIndividual(&pop.i[0], best);
		} else {
			cloneIndividual(&best, pop.i[0]);
		}
		for(i = 1; i < popsize; i++) {
			if(is_best(best, pop.i[i]))  {   		
				//puts("opa");		
				cloneIndividual(&best, pop.i[i]);
				fbest = fitness(best);
				best_generation = generation;
			}
		}

	} 
	//printPop(pop);
	printf("best single individual: ");
	printIndividual(best);
	printf("\nfitness: %lf\n", fitness(best));
	printf("generation: %d\n", generation);
	printf("generation of best ind: %d\n", best_generation);

	return 0;
}
