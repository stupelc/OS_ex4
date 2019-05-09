//chagit stupel 209089960

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
    int isDestroyed;
    int blockNewTasks;
    int emptyTheQueue;


} ThreadPool;

//struct of a mission witch the thread runs on
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

/**
 *
 * @param threadPool treadPool of the mission
 * @param shouldWaitForTasks 0-wait until all the tasks will stop running without running the task in the queue
 * otherwise wait until all the task, also those in the queue, will finish running
 */
void tpDestroy(ThreadPool *threadPool, int shouldWaitForTasks);

/**
 *
 * @param threadPool threadPool of the mission
 * @param computeFunc the func we wanna the task will run
 * @param param the params of the task
 * @return 0 - if the function successed, -1 otherwise
 */
int tpInsertTask(ThreadPool *threadPool, void (*computeFunc)(void *), void *param);


#endif //OS_EX4_2_THREADPOOL_H
