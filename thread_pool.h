#ifndef THREAD_POOL
#define THREAD_POOL
#include <stddef.h>
#include <stdbool.h>
#include "cthreads.h"

//typedef func_ptr_t (void*)(void*) // <==> void* (*func)(void*)

struct Thread_Pool_Task{
  void (*func)(void*);
  void* arg;
  struct cthreads_semaphore* done;
};

struct Thread_Pool_Queue{
  struct Thread_Pool_Task* task;
  struct Thread_Pool_Queue* next;
};

#define THREAD_POOL_BLOCK_SIZE (sizeof(struct Thread_Pool_Task))

size_t thread_pool_calculate_mem_requirements(const size_t max_tasks, const size_t max_threads);

bool thread_pool_init(size_t max_tasks, size_t max_threads, void* storage);

bool thread_pool_add(struct Thread_Pool_Task* task);

int thread_pool_start(size_t nr_threads);

void thread_pool_destroy(void);

#endif
