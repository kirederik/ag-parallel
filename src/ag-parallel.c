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
#define MAX_POP_SIZE 50
#define IND_SIZE 10
#define MAX_GENERATIONS 400
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

void printIndividual(individual p) {
    int i;
    for(i = 0; i < IND_SIZE; i++) {
		printf("%d ", p.v[i]);
	}
    return;
}

void applyCrossover(individual *c1, individual *c2, int popsize, individual p1, individual p2) {
	int cindex = rand() % IND_SIZE;
	individual *x, *y;
	int i;
	x = &p1;
	y = &p2;
    
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

individual* applyTournament(population pop) {
  individual i1 = getIndividual(pop);
  individual i2 = getIndividual(pop);
  individual i3 = getIndividual(pop);
  individual i4 = getIndividual(pop);

  // printIndividual(i1);
  // printf("\n");
  // printIndividual(i2);
  // printf("\n");
  // printIndividual(i3);
  // printf("\n");
  // printIndividual(i4);
  // printf("\n");


  individual* ret = (individual*) malloc(sizeof(individual) * 4);

  *(ret) = i1;
  *(ret + 1) = i2;
  *(ret + 2) = i3;
  *(ret + 3) = i4;
  
  if(fitness(*(ret)) < fitness(*(ret + 1))) {
    ret = changeIndividuals(ret, 0, 1);
  }

  if(fitness(*(ret + 2)) < fitness(*(ret + 3))) {
    ret = changeIndividuals(ret, 2, 3);
  } 

  if(fitness(*(ret)) < fitness(*(ret + 2))) {
    ret = changeIndividuals(ret, 0, 2);
  }

  if(fitness(*(ret + 1)) < fitness(*(ret + 3))) {
    ret = changeIndividuals(ret, 1, 3);
  }           
    
  if(fitness(*(ret + 1)) < fitness(*(ret + 2))) {
    ret = changeIndividuals(ret, 1, 2);
  }


  // printf("%d %d %d %d\n", fitness(*ret), fitness(*(ret + 1)), fitness(*(ret + 2)), fitness(*(ret + 3))); 

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
	
	population pop1;
  population pop2;
  population pop3;
  population pop4;

  individual best1;
  individual best2;
  individual best3;
  individual best4;
	
	if(!initialize(&pop1, popsize)) {
		puts("Memory Error");
		exit(1);
	}

  if(!initialize(&pop2, popsize)) {
    puts("Memory Error");
    exit(1);
  }

  if(!initialize(&pop3, popsize)) {
    puts("Memory Error");
    exit(1);
  }

  if(!initialize(&pop4, popsize)) {
    puts("Memory Error");
    exit(1);
  }

  best1.v = (int *) malloc(IND_SIZE * sizeof(individual));
  if(!best1.v) {
    puts("Memory Error");
    exit(1);
  }
  best2.v = (int *) malloc(IND_SIZE * sizeof(individual));
  if(!best2.v) {
    puts("Memory Error");
    exit(1);
  }
  best3.v = (int *) malloc(IND_SIZE * sizeof(individual));
  if(!best3.v) {
    puts("Memory Error");
    exit(1);
  }
  best4.v = (int *) malloc(IND_SIZE * sizeof(individual));
  if(!best4.v) {
    puts("Memory Error");
    exit(1);
  }


  #pragma omp parallel shared(pop1, pop2, pop3, pop4, best1, best2, best3, best4)
  {

    int tid = omp_get_thread_num();
    int generation = 0;

    individual best;

    population pop;

    if (tid == 0) {
      pop = pop1;
    }
    else if (tid == 1){
      pop = pop2;
    }
    else if (tid == 2){
      pop = pop3;
    }
    else if (tid == 3){
      pop = pop4;
    }

    best.v = malloc(IND_SIZE * sizeof(individual));
    if(!best.v) {
      puts("Memory Error");
      exit(1);
    }

  	cloneIndividual(&best, pop.i[0]);
    int fbest = fitness(best);
    
    if (tid == 0) {
      cloneIndividual(&best1, best);
    }
    else if (tid == 1){
      cloneIndividual(&best2, best);
    }
    else if (tid == 2){
      cloneIndividual(&best3, best);
    }
    else if (tid == 3){
      cloneIndividual(&best4, best);          
    }

    printf("%d\n", tid);

    int best_generation = generation;
  	while(fitness(best) != IND_SIZE && generation++ < MAX_GENERATIONS) {
  		int i, j = 0;		
  		population q;
  		if(!popalloc(&q, popsize)) {
  			puts("Memory Error");
  			exit(1);
  		}
  		
  		for(i = 0; i < breed; i++) {
  			individual c1, c2;
        individual* tournament = applyTournament(pop);

  //      printf("%d\n", fitness(*(tournament)));
  //      printf("%d\n", fitness(*(tournament + 1)));

        if (tid == 0) {
          applyCrossover(&c1, &c2, popsize, best, best2);
        }
        else if (tid == 1){
          applyCrossover(&c1, &c2, popsize, best, best3);
        }
        else if (tid == 2){
          applyCrossover(&c1, &c2, popsize, best, best4);
        }
        else if (tid == 3){
          applyCrossover(&c1, &c2, popsize, best, best1);
        }
        
        // applyCrossover(&c1, &c2, popsize, best, *tournament);

        //printIndividual(*(tournament));
        //printIndividual(*(tournament + 1));
        //printIndividual(*(tournament + 2));
        //printIndividual(*(tournament + 3));
        
        mutate(&c1);
        mutate(&c2);

  			cloneIndividual(&q.i[j++], c1);
  			cloneIndividual(&q.i[j++], c2);
        free(c1.v); free(c2.v);  
        free(tournament);
  		}

      popfree(&pop, popsize);
  		pop = q;
  		
      for(i = 0; i < popsize; i++) {
  		  if(fitness(pop.i[i]) > fbest)  {                
  			  cloneIndividual(&best, pop.i[0]);
          fbest = fitness(best);
          best_generation = generation;
          if (tid == 0) {
            cloneIndividual(&best1, best);
          }
          else if (tid == 1){
            cloneIndividual(&best2, best);
          }
          else if (tid == 2){
            cloneIndividual(&best3, best);
          }
          else if (tid == 3){
            cloneIndividual(&best4, best);          
          }
    		}
  	  }


  	} 
    printf("best single individuo: ");
    printIndividual(best);
    printf("\nfitness: %d\n", fitness(best));
    printf("generation: %d\n", generation-1);
    printf("generation of best ind: %d\n", best_generation);

	}

  
	return 0;
}

