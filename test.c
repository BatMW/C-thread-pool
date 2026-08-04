#include "stdio.h"
#include "cthreads.h"
#include "thread_pool.h"
#include <stdlib.h>
#include <string.h>

#define NR_TASKS 1024
#define NR_THREADS 7

struct cthreads_mutex mx_stdout;

struct test_func_arg_list{
  int x;
  int y;
  int z;
};

void test_func(void* args){
  struct test_func_arg_list* type_args = (struct test_func_arg_list*)args;
  cthreads_mutex_lock(&mx_stdout);
  printf("Arguments:\nx: %d y: %d z: %d\n", type_args->x, type_args->y, type_args->z);
  cthreads_mutex_unlock(&mx_stdout);
}

int main(void){

  size_t storage_size = thread_pool_calculate_mem_requirements(NR_TASKS, NR_THREADS);
  if(storage_size == 0){
    printf("mem requirement failed.\n");
    return 1;
  }
  void* storage = malloc(storage_size);
  if(storage == NULL){
    printf("Allocation failed..\n");
    return 1;
  }
  bool success = thread_pool_init(NR_TASKS, NR_THREADS, storage);
  if(!success){
     printf("Thread pool init failed.\n");
    return 1;
  }
  cthreads_mutex_init(&mx_stdout, NULL);
  struct Thread_Pool_Task tasks[NR_TASKS];
  struct test_func_arg_list arg_list[NR_TASKS];
  struct cthreads_semaphore done_list[NR_TASKS];
  for(int i=0; i<NR_TASKS; ++i){
    arg_list[i].x = (i+1)*i;
    arg_list[i].y = (i+1)*i + 1;
    arg_list[i].z = (i+1)*i + 2;
    tasks[i].func = &test_func;
    tasks[i].arg = (void*)&arg_list[i];
    tasks[i].done = &done_list[i];
    cthreads_sem_init(tasks[i].done, 0);
    if(!thread_pool_add(&tasks[i])){
      printf("Failed to add task\n");
      return 1;
    }
  }
  printf("Initialized all tasks and added to queue.\n");

  printf("Starting pool...\n");
  int started_threads = thread_pool_start(NR_THREADS);
  printf("Started %i threads.\n", started_threads);
  printf("Waiting for tasks...\n");
  for(int i =0; i<NR_TASKS; ++i){
    cthreads_sem_wait(tasks[i].done);

    //printf("Task closed, ");
  }

    printf("\n");

    printf("Adding tasks while pool is running...\n");
  for(int a=0; a < 100; ++a){
    for(int i=0; i<NR_TASKS; ++i){
      cthreads_sem_init(tasks[i].done, 0);
      thread_pool_add(&tasks[i]);
    }
    for(int i =0; i<NR_TASKS; ++i){
      cthreads_sem_wait(tasks[i].done);
    }
  }
  printf("Destroying stuff.\n");

  thread_pool_destroy();
  free(storage);
  printf("All done.\n");
  return 0;
}
