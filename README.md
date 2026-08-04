# C Thread Pool

A small thread pool implementation in C using [CThreads](https://github.com/PerformanC/CThreads).

The goal of this project is to provide a simple reusable thread pool for C projects without hidden allocations. The pool uses a fixed amount of memory supplied by the user and a custom pool allocator for task queue nodes.

## Features

- Fixed-size thread pool
- User-provided memory storage
- No internal heap allocations
- FIFO task queue
- Custom allocator for task management
- Simple synchronization using mutexes and semaphores
- Designed for small systems projects and personal C libraries

## Design

The thread pool consists of:

- A fixed array of worker threads
- A task queue protected by a mutex
- A semaphore used to wake sleeping workers when tasks are available
- A [pool allocator]( https://github.com/BatMW/Memory-Allocators) used for task queue nodes

Worker threads sleep when no tasks are available and wake up when new work is submitted.

## Example Usage
``` c
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
  
  struct Thread_Pool_Task task = {
    .func = my_function,
    .arg = my_data,
    .done = &completion_sem
};

thread_pool_add(&task);

cthreads_sem_wait(task.done);
thread_pool_destroy();
free(storage);

```
