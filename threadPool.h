//
// Created by chagit on 5/9/19.
//

#ifndef OS_EX4_2_THREADPOOL_H
#define OS_EX4_2_THREADPOOL_H

#define TRUE 1
#define FALSE 0
#define FAIL -1
#define SUCCESS 0
#define INITIALIZE 2


#include <pthread.h>
#include "osqueue.h"

//structure of threadPool
typedef struct thread_pool {

    OSQueue *tasksQueue; //the queqqe of the tasks
    pthread_t* threads; //the thread we are gonna to create
    void (*executeTasks)(void *);

    //mutex
    pthread_mutex_t mutexTasksQueueu;
    pthread_mutex_t mutexQueue;
    pthread_mutex_t mutexEmpty;
    pthread_cond_t cond;

    //num of threads we are gonna to create
    int numOfThreads;

    //flags
    int toDestroyed;
    int blockNewTasks;
    int emptyTheQueue;


} ThreadPool;

//struct of a mission witch will the tread run
typedef struct {
    void (*func)(void *);
    void *args;
} Task;

/**
 *
 * @param numOfThreads num of thread to create in the threadPool
 * @return the new threadPool object
 */
ThreadPool *tpCreate(int numOfThreads);

void tpDestroy(ThreadPool *threadPool, int shouldWaitForTasks);

int tpInsertTask(ThreadPool *threadPool, void (*computeFunc)(void *), void *param);


#endif //OS_EX4_2_THREADPOOL_H
