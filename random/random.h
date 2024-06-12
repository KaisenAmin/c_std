#ifndef RANDOM_H_
#define RANDOM_H_ 

#include <stdio.h>
#include <stddef.h>

void random_seed(unsigned int seed);
void random_shuffle(void *array, size_t n, size_t size);

int random_randint(int a, int b);
int random_randrange(int a, int b, int step);
int random_getrandbits(int a);

double random_random();
double random_uniform(double a, double b);

#endif 