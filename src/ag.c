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
#define MAX_POP_SIZE 100
#define IND_SIZE 10
#define MAX_GENERATIONS 500
#define TOURNAMENT 4
#define MUTATION_PROB 0.1

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
	int index = rand() % MAX_POP_SIZE;
	return p.i[index];
}

void printIndividual(individual p) {
    int i;
    for(i = 0; i < IND_SIZE; i++) {
		printf("%d ", p.v[i]);
	}
    return;
}

void applyCrossover(
		individual *c1, individual *c2, int popsize,
		individual p1, individual p2
	) {
	int cindex = rand() % IND_SIZE;
	individual *x, *y;
	int i;
	x = &p1;
	y = &p2;
    
    c1->v = malloc(IND_SIZE * sizeof(int));
    c2->v = malloc(IND_SIZE * sizeof(int));
	if(!c1->v || !c2->v) {
        puts("Memory Error");
        exit(1);
    }
	for(i = 0; i < popsize; i++) {
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
        double prob = (rand() % 100) / 100;
        if (prob < MUTATION_PROB) {
            ind->v[i] = (ind->v[i] == 0) ? 1 : 0;
        }
    }

    return;
}

/** Apply the fitness function **/
int fitness(individual ind) {
	int i;
	int sum = 0;
	for(i = 0; i < IND_SIZE; i++) {
		sum += ind.v[i];
	}
	return sum;
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
	while(fitness(best) != IND_SIZE) {
		int i, j = 0;
        fbest = fitness(best);
		for(i = 0; i < popsize; i++) {
			if(fitness(pop.i[i]) > fbest)  {                
				cloneIndividual(&best, pop.i[0]);
			}
		}
		
		population q;
		if(!popalloc(&q, popsize)) {
			puts("Memory Error");
			exit(1);
		}
		
		for(i = 0; i < breed; i++) {
			individual c1, c2;
			applyCrossover(
				&c1, &c2, popsize,
				getIndividual(pop), 
				getIndividual(pop)
			);
            mutate(&c1);
            mutate(&c2);

			cloneIndividual(&q.i[j++], c1);
			cloneIndividual(&q.i[j++], c2);
            free(c1.v); free(c2.v);

		}
        popfree(&pop, popsize);
		pop = q;
	} 
	
    printf("best single individuo: ");
    printIndividual(best);
	printf("\nfitness: %d\n", fitness(best));
    printf("generation: %d\n", generation-1);
	return 0;
}

