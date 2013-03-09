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
#define _USE_MATH_DEFINES

/**
 * Individual struct 
 */
 typedef struct individual {
 	double *v;
 } individual;
/**
 * Population struct 
 */
 typedef struct population {
 	individual *i;
 } population;

/** Define initial parameters **/
#define MAX_POP_SIZE 400
#define IND_SIZE 5
#define MAX_GENERATIONS 1000
#define TOURNAMENT 4
#define MUTATION_PROB 1

 individual* changeIndividuals(individual* i, int origin, int destiny);

/** Empty population memory allocation **/
 population* popalloc(population *pop, int popsize) {
 	int i;
 	if((pop->i = malloc(popsize * sizeof(individual)))) {
 		for(i = 0; i < popsize; i++) {
 			if(!(pop->i[i].v = (double*) malloc(IND_SIZE * sizeof(double)))) {
 				break;
 			}
 		}
 	}
 	return pop;
 }

 double range_rand(double max, double min) {

 	double  out = ((double)random()/ (RAND_MAX)) * (max - min) + min;

 	return out;
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
 			for(j = 0; j < IND_SIZE; j++){
 				pop->i[i].v[j] = range_rand(5.12, -5.12);
 			}
 		}

 	}
 	return pop;
 }

/** Return a random individual **/
 individual getIndividual(population p) {
 	int a = random();
 	int index = a % MAX_POP_SIZE;
 	return p.i[index];
 }

void printIndividual(individual p) {
 	int i;
 	for(i = 0; i < IND_SIZE; i++) {
 		printf("%.7lf ", p.v[i]);
 	}
 	return;
 }

void applyCrossover(individual *c1, individual *c2, int popsize, individual p1, individual p2) {
 	int cindex = random() % IND_SIZE;
 	individual *x, *y;
 	int i;
 	x = &p1;
 	y = &p2;

 	c1->v = (double *) malloc(IND_SIZE * sizeof(double));
 	c2->v = (double *) malloc(IND_SIZE * sizeof(double));
 	if(!c1->v || !c2->v) {
 		puts("Memory Error");
 		exit(1);
 	}

 	for(i = 0; i < IND_SIZE; i++) {
 		if(i == cindex) {
 			x = &p2;
 			y = &p1;
 		}
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

void mutate(individual *ind) {
 	int i;
 	for(i = 0; i < IND_SIZE; i++) {
 		double prob = (random() % 100) / 100;
 		if (prob < MUTATION_PROB) {
 			double mut = range_rand(0.05 , -0.05);
 			ind->v[i] = ind->v[i] + mut;
 		}
 	}
 	return;
 }

/** Apply the fitness function **/
double fitness(individual ind) {
 	int i;
 	double sum = 0;
 	for(i = 0; i < IND_SIZE; i++) {
		sum += (ind.v[i] * ind.v[i] - 10 * cos(10 * M_PI * ind.v[i])) ; //f(x) = sum(x^2)
		/*sum +=  (ind.v[i] * ind.v[i] );*/
	}
	/*return sum;*/
	return 10 * (IND_SIZE - 1)  + sum;
}

int is_best(individual i, individual candidate) {
	double ind = fitness(i);
	double cand = fitness(candidate);
	
	ind = (ind < 0)?-1 * ind:ind;
	cand = (cand < 0)?-1 * cand:cand;
	
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


	if(is_best(*(ret),*(ret + 1))) {
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
	}


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
	int breed = (popsize / 2);
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
	double fbest = fitness(best);

	int best_generation = generation;

	while(generation++ < MAX_GENERATIONS) {

		int i, j = 0;		
		population q;
		if(!popalloc(&q, popsize)) {
			puts("Memory Error");
			exit(1);
		}
		/*cloneIndividual(&q.i[j++], best);*/
		/*printIndividual(best);*/
		for(i = 0; i < breed; i++) {
			individual c1, c2;
			individual* tournament = applyTournament(pop);
			
      //Crossover of the two best of tournament
			applyCrossover(&c1, &c2, popsize, *tournament, *(tournament + 1));

      //Mutate with the probability above
			mutate(&c1);
			mutate(&c2);


      //if the fitness of generated individual is worse than individual of tournament do not put this in the population
			//if (is_best(c1, *tournament)) {
		//		cloneIndividual(&c1, *tournament);
        /*puts("IF 1 Oi");*/
		//	}

      //if the fitness of generated individual is worse than individual of tournament do not put this in the population
/*			if (is_best(c2, *(tournament + 1))) {
				cloneIndividual(&c2, *(tournament + 1));
        /*puts("IF 2 Oi");
			}
        */

			cloneIndividual(&q.i[j++], c1);
			cloneIndividual(&q.i[j++], c2);
      //free(c1.v); free(c2.v);  
			free(tournament);
		}

		popfree(&pop, popsize);
		pop = q;

    //verify if was generated a better individual	than in the previous population
		if(is_best(pop.i[0], best))  {   
			cloneIndividual(&pop.i[i], best);
		} else {
			cloneIndividual(&best, pop.i[i]);
		}
		for(i = 1; i < popsize; i++) {
			if(is_best(best, pop.i[i]))  {   
				cloneIndividual(&best, pop.i[i]);
				fbest = fitness(best);
				best_generation = generation;
			}
		}
	} 
	
	printf("best single individual: ");
	printIndividual(best);
	printf("\nfitness: %.6lf\n", fitness(best));
	printf("generation: %d\n", generation-1);
	printf("generation of best ind: %d\n", best_generation);

	return 0;
}

