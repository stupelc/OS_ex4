//chagit stupel 209089960

#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <stdio.h>
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

/**
 *
 * @param threadPool a pointer to our threadPool
 * @param num the status of a specific function
 */
void checkSuccess(ThreadPool *threadPool, int num) {
    if (num != SUCCESS) {
        PrintError();
        tpDestroy(threadPool, 0);
        _exit(EXIT_FAILURE);
    }

}

/**
 *
 * @param args
 * runs the tasks
 */
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

/**
 *
 * @param arg pretend to be a threadPool
 * wait for all the threads to finish
 */
void waitForAllThreads(void *arg) {
    int i;
    ThreadPool *threadPool = (ThreadPool *) arg;
    //loop that run all over the threads and wait for them to finish their task.
    for (i = 0; i < threadPool->numOfThreads; i++) {
        if (pthread_join(threadPool->threads[i], NULL)!=0){
            PrintError();
            _exit(EXIT_FAILURE);
        }
    }
}

//check if the mission queue is empty
int missionQueueEmpty(ThreadPool* tp) {
    //lock the queue
    int status;
    status = pthread_mutex_lock(&tp->mutexTasksQueueu);
    checkSuccess(tp,status);
    //check if the queue is empty
    int isEmpty = osIsQueueEmpty(tp->tasksQueue);
    //unlock the queue
    status = pthread_mutex_unlock(&tp->mutexTasksQueueu);
    checkSuccess(tp,status);

    return isEmpty;
}

//get a new mission from the mission queue
Task* getMissionFromQueue(ThreadPool* tp) {
    //lock the queue
    int status;
    status = pthread_mutex_lock(&tp->mutexTasksQueueu);
    checkSuccess(tp,status);

    //add the mission to the queue
    Task* task = (Task*) osDequeue(tp->tasksQueue);

    //unlock the queue
    status = pthread_mutex_unlock(&tp->mutexTasksQueueu);
    checkSuccess(tp,status);
    return task;
}

/**
 *
 * @param threadPool treadPool of the mission
 * @param shouldWaitForTasks 0-wait until all the tasks will stop running without running the task in the queue
 * otherwise wait until all the task, also those in the queue, will finish running
 */
void tpDestroy(ThreadPool *threadPool, int shouldWaitForTasks) {
    //check if this function have been call
    if (threadPool->isDestroyed) {
        return;
    }
    //cant get new tasks from now
    threadPool->blockNewTasks = TRUE;

    //lock this critical section.
    if (pthread_mutex_lock(&threadPool->mutexTasksQueueu) != 0) {
        PrintError();
        _exit(EXIT_FAILURE);
    }
    //wake up the threadPool
    if (pthread_cond_broadcast(&threadPool->cond) != 0) {
        PrintError();
    }
    if (pthread_mutex_unlock(&threadPool->mutexTasksQueueu) != 0) {
        PrintError();
        _exit(EXIT_FAILURE);
    }

    if (shouldWaitForTasks == 1) {
        if (pthread_mutex_lock(&threadPool->mutexEmpty) != 0) {
            PrintError();
            _exit(EXIT_FAILURE);
        }
        threadPool->emptyTheQueue = TRUE; //we don't wanna run tasks from the queue anymore

        if (pthread_mutex_unlock(&threadPool->mutexEmpty) != 0) {
            PrintError();
            _exit(EXIT_FAILURE);
        }
    } else {
        if (pthread_mutex_lock(&threadPool->mutexEmpty) != 0) {
            PrintError();
            _exit(EXIT_FAILURE);
        }
        threadPool->emptyTheQueue = FALSE; //we wanna continue run tasks from the queue

        if (pthread_mutex_unlock(&threadPool->mutexEmpty) != 0) {
            PrintError();
            _exit(EXIT_FAILURE);
        }
    }

    //if we need to empty to queue
    if (threadPool->emptyTheQueue) {
        //the threadpool need the empty the queue and run all the tasks
        //wait for all the thread to finish their job.
        waitForAllThreads(threadPool);
        threadPool->isDestroyed = TRUE;

        //if there are more mission in the queue, delete them
        while(!missionQueueEmpty(threadPool)) {
            free(getMissionFromQueue(threadPool));
        }
        //lock the queue
        if(pthread_mutex_lock(&threadPool->mutexTasksQueueu) != 0) {
            PrintError();
        }
        //destroy the queue
        osDestroyQueue(threadPool->tasksQueue);
        threadPool->tasksQueue = NULL;
        if(pthread_mutex_unlock(&threadPool->mutexTasksQueueu) != 0) {
            PrintError();
        }

    } else {
        waitForAllThreads(threadPool);
        threadPool->isDestroyed = TRUE;
    }

    //destroy and free all the mutexes and allocation memory.
    pthread_cond_destroy(&threadPool->cond);
    pthread_mutex_destroy(&threadPool->mutexQueue);
    pthread_mutex_destroy(&threadPool->mutexTasksQueueu);
    pthread_mutex_destroy(&threadPool->mutexEmpty);
    free(threadPool->tasksQueue);
    free(threadPool->threads);
    free(threadPool);
}

/**
 *
 * @param threadPool threadPool of the mission
 * @param computeFunc the func we wanna the task will run
 * @param param the params of the task
 * @return 0 - if the function successed, -1 otherwise
 */
int tpInsertTask(ThreadPool *threadPool, void (*computeFunc)(void *), void *param) {

    //check if the threadpool is destroyed or can't get new tasks.
    if (threadPool->isDestroyed || threadPool->blockNewTasks) {
        return FAIL;
    }

    //crete the mession
    Task *task = malloc(sizeof(Task));
    //check if the allocation have been failed.
    if (task == NULL) {
        PrintError();
    }
    task->args = param;
    task->func = computeFunc;

    //lock the mutex to the queue.
    int status;
    status = pthread_mutex_lock(&threadPool->mutexQueue);
    checkSuccess(threadPool, status);

    //add the mession to the queue
    osEnqueue(threadPool->tasksQueue, task);

    //unlock the mutex to the queue.
    status = pthread_mutex_unlock(&threadPool->mutexQueue);
    checkSuccess(threadPool, status);

    //wake up thread.
    status = pthread_cond_signal(&threadPool->cond);
    checkSuccess(threadPool, status);

    return SUCCESS;
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