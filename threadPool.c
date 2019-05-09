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
    threadPool = (ThreadPool *) arg;
    threadPool->executeTasks(arg);
}

void checkSuccess(ThreadPool *threadPool, int num) {
    if (num != SUCCESS) {
        PrintError();
        tpDestroy(threadPool, 0);
        _exit(EXIT_FAILURE);
    }

}

void executeTasks(void *args) {
    ThreadPool *threadPool = (ThreadPool *) args;
    if (threadPool == NULL) {
        PrintError();
        exit(1);
    }
    //keep the return val from different methods for checking
    int status;

    //check if the threadPool have been destroyed
    while (!threadPool->isDestroyed) {
        //lock this critical section
        status = pthread_mutex_lock(&threadPool->mutexTasksQueueu);
        checkSuccess(threadPool, status);

        //check if the queqe is empty and the threadPool is not empty
        if (osIsQueueEmpty(threadPool->tasksQueue) && (!threadPool->isDestroyed)) {
            //lock the access to the mutexEmpty variable
            status = pthread_mutex_lock(&threadPool->mutexEmpty);
            checkSuccess(threadPool, status);

            //check if there is no need to wait to new tasks.
            if (threadPool->emptyTheQueue != INITIALIZE) {
                status = pthread_mutex_unlock(&threadPool->mutexEmpty);
                checkSuccess(threadPool, status);
                status = pthread_mutex_unlock(&threadPool->mutexTasksQueueu);
                checkSuccess(threadPool, status);
                break;
            }
            status = pthread_mutex_unlock(&threadPool->mutexEmpty);
            checkSuccess(threadPool, status);

            status = pthread_cond_wait(&threadPool->cond, &threadPool->mutexTasksQueueu);
            checkSuccess(threadPool, status);

        }

        status = pthread_mutex_unlock(&threadPool->mutexTasksQueueu);
        checkSuccess(threadPool, status);

        status = pthread_mutex_lock(&threadPool->mutexQueue);
        checkSuccess(threadPool, status);

        //check if the queue is empty or not.
        if (!osIsQueueEmpty(threadPool->tasksQueue)) {
            Task *task = (Task *) osDequeue(threadPool->tasksQueue);

            if (task == NULL) {
                continue;
            }
            status = pthread_mutex_unlock(&threadPool->mutexQueue);
            checkSuccess(threadPool, status);

            //run the task.
            task->func(task->args);
            free(task);
            //there is no tasks in the queue.
        } else {
            status = pthread_mutex_unlock(&threadPool->mutexQueue);
            checkSuccess(threadPool, status);
        }

        //lock the access to the mutexEmpty.
        status = pthread_mutex_lock(&threadPool->mutexEmpty);
        checkSuccess(threadPool, status);

        //check if there is need to get out of the while loop.
        if (threadPool->emptyTheQueue == FALSE) {
            status = pthread_mutex_unlock(&threadPool->mutexEmpty);
            checkSuccess(threadPool, status);
            break;
        }
        status = pthread_mutex_unlock(&threadPool->mutexEmpty);
        checkSuccess(threadPool, status);
    }
}

/**
 *
 * @param threadPool tha threadPool we created
 * @param numOfThreads num of threads gonna run in the treadpool
 */
void initializeParamOfStrucr(ThreadPool *threadPool, int numOfThreads) {
    threadPool->tasksQueue = osCreateQueue(); //initialize the queqe
    threadPool->numOfThreads = numOfThreads; //initialize the num of threads
    threadPool->executeTasks = executeTasks; //the function that will be run for all the treades
    threadPool->isDestroyed = FALSE;
    threadPool->blockNewTasks = FALSE;
    threadPool->emptyTheQueue = INITIALIZE;

    //initialize the mutex and the conditions
    if (pthread_mutex_init(&threadPool->mutexTasksQueueu, NULL) || pthread_cond_init(&threadPool->cond, NULL)
        || pthread_mutex_init(&threadPool->mutexQueue, NULL) || pthread_mutex_init(&threadPool->mutexEmpty, NULL)) {
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

void tpDestroy(ThreadPool *threadPool, int shouldWaitForTasks) {

}

int tpInsertTask(ThreadPool *threadPool, void (*computeFunc)(void *), void *param) {

}


/**
 *
 * @param numOfThreads num of thread to create in the threadPool
 * @return the new threadPool object
 */
ThreadPool *tpCreate(int numOfThreads) {
    ThreadPool *threadPool = malloc(sizeof(ThreadPool));
    initializeParamOfStrucr(threadPool, numOfThreads);
    return threadPool;
}