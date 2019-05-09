//chagit stupel 209089960

#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include "threadPool.h"


/**
 * print an error message
 */
void PrintError() {
    write(2, "Error in system call\n", strlen("Error in system call\n"));
}


/**
 *
 * @param arg pretend to be a threadPool
 */
void *execute(void *arg) {
    ThreadPool *threadPool;
    threadPool = (ThreadPool*) arg;
    threadPool->executeTasks(arg);
}

void executeTasks(void *args){
    ThreadPool *threadPool = (ThreadPool *) args;
    if (threadPool == NULL) {
        PrintError();
        exit(1);
    }
}

//initialize the struct of the threadPool
void initializeParamOfStrucr (ThreadPool* threadPool, int numOfThreads){
    threadPool->tasksQueue = osCreateQueue(); //initialize the queqe
    threadPool->numOfThreads=numOfThreads; //initialize the num of threads
    threadPool->executeTasks=executeTasks; //the function that will be run for all the treades
    threadPool->toDestroyed = FALSE;
    threadPool->blockNewTasks=FALSE;
    threadPool->emptyTheQueue = INITIALIZE;

    //initialize the mutex and the conditions
    if(pthread_mutex_init(&threadPool->mutexTasksQueueu, NULL) || pthread_cond_init(&threadPool->cond, NULL)
       || pthread_mutex_init(&threadPool->mutexQueue,NULL) || pthread_mutex_init(&threadPool->mutexEmpty, NULL)) {
        PrintError();
        free(threadPool);
        _exit(EXIT_FAILURE);
    }

    //initialize the threads
    threadPool->threads = malloc(sizeof(pthread_t) * numOfThreads);
    //check if the allocation have been failed
    if (threadPool->threads == NULL) {
        PrintError();
        free(threadPool);
        _exit(EXIT_FAILURE);
    }

    //create the threads
    int i;
    for (i = 0; i < threadPool->numOfThreads; i++) {
        if (pthread_create(&(threadPool->threads[i]), NULL, execute, threadPool) != 0) {
            tpDestroy(threadPool, 0);
            PrintError();
            _exit(EXIT_FAILURE);
        }
    }
}

void tpDestroy(ThreadPool *threadPool, int shouldWaitForTasks){

}

int tpInsertTask(ThreadPool *threadPool, void (*computeFunc)(void *), void *param){

}


/**
 *
 * @param numOfThreads num of thread to create in the threadPool
 * @return the new threadPool object
 */
ThreadPool *tpCreate(int numOfThreads){
    ThreadPool *threadPool = malloc (sizeof(ThreadPool));
    initializeParamOfStrucr (threadPool,numOfThreads);
    return threadPool;
}