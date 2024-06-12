#ifndef RANDOM_H_
#define RANDOM_H_ 

void random_seed(unsigned int seed);

int random_randint(int a, int b);
int random_randrange(int a, int b, int step);

double random_random();

#endif 