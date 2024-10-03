/* shopping.c */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

sem_t sem1;
sem_t sem2;

void *print_produce( void * );
void *print_dairy( void * );
void *print_dairy(void *items)
{
  int i = 0;
  char** array = (void*)items;
  {
    sem_wait(&sem2);
    printf("got %s\n", (array[i++]) );
    printf("got %s\n", (array[i++]) );
    sem_post(&sem1);
  }
  return( NULL );
}

void *print_produce(void *items)
{
  int i = 0;
  char** array = (void*)items;
  {
    printf("got %s\n", (array[i++]) );
    sem_post(&sem2);
    sem_wait(&sem1);
    printf("got %s\n", (array[i++]) );
  }
  return( NULL );
}

int main()
{
  char *produce[] = { "Salad", "Apples", NULL };
  char *dairy[] = { "Milk", "Butter", NULL };
  if (sem_init(&sem1, 0, 0) == -1) 
  {
  	exit(1);
  }
  if (sem_init(&sem2, 0, 0) == -1)
  {
        exit(1);
  }

  pthread_t th1, th2;
  pthread_create( &th1, NULL, print_produce, (void*)produce);
  pthread_create( &th2, NULL, print_dairy, (void*)dairy);
  pthread_join(th1, NULL);
  pthread_join(th2, NULL);
  sem_destroy(&sem1);
  sem_destroy(&sem2);
}

