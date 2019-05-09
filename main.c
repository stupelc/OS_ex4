
/**
 * HW3 Tester
 *
 * WHAT YOU SHOULD SEE:
 * A table of threads doing tasks. For example:
 * 			================================
 * 			Task execution table by threads:
 * 			================================
 *			   TIME:   | T 1 (TID= 9975) | T 2 (TID= 9976) | T 3 (TID= 9977) | T 4 (TID= 9978) | T 5 (TID= 9979)
 *			-----------------------------------------------------------------------------------------------------
 *			2162318835 |      START      |                 |                 |                 |                 |
 *			2162318842 |                 |      START      |                 |                 |                 |
 *			2162318849 |                 |                 |      START      |                 |                 |
 *			2162318850 |                 |                 |                 |      START      |                 |
 *			2162318852 |                 |                 |                 |                 |      START      |
 *			2162318862 |                 |                 |                 |                 |       END       |
 *			2162318862 |                 |                 |                 |                 |      START      |
 *			2162318874 |                 |                 |                 |                 |       END       |
 *			2162318875 |                 |                 |                 |                 |      START      |
 *			2162318882 |                 |                 |                 |                 |       END       |
 *			2162318882 |                 |                 |                 |                 |      START      |
 *			2162318892 |                 |                 |                 |                 |       END       |
 *			2162318892 |                 |                 |                 |                 |      START      |
 *			2162318901 |                 |                 |                 |                 |       END       |
 *			2162318901 |                 |                 |                 |                 |      START      |
 *			2162318913 |                 |                 |                 |                 |       END       |
 *			2162318913 |                 |                 |                 |                 |      START      |
 *			2162318923 |                 |                 |                 |                 |       END       |
 *			2162318923 |                 |                 |                 |                 |      START      |
 *			2162318931 |       END       |                 |                 |                 |                 |
 *			2162318931 |      START      |                 |                 |                 |                 |
 *			2162318939 |       END       |                 |                 |                 |                 |
 *			2162318939 |      START      |                 |                 |                 |                 |
 *			2162318949 |       END       |                 |                 |                 |                 |
 *			2162318949 |      START      |                 |                 |                 |                 |
 *			2162318961 |       END       |                 |                 |                 |                 |
 *			2162318961 |      START      |                 |                 |                 |                 |
 *			2162318969 |       END       |                 |                 |                 |                 |
 *			2162318969 |      START      |                 |                 |                 |                 |
 *			2162318979 |       END       |                 |                 |                 |                 |
 *			2162318979 |      START      |                 |                 |                 |                 |
 *			2162318988 |       END       |                 |                 |                 |                 |
 *			2162318988 |      START      |                 |                 |                 |                 |
 *			2162319003 |       END       |                 |                 |                 |                 |
 *			2162319003 |      START      |                 |                 |                 |                 |
 *			2162319011 |       END       |                 |                 |                 |                 |
 *			2162319017 |                 |       END       |                 |                 |                 |
 *			2162319022 |                 |                 |       END       |                 |                 |
 *			2162319032 |                 |                 |                 |       END       |                 |
 *			2162319033 |                 |                 |                 |                 |       END       |
 * The output may not help very much, but it may give you some clues.
 * It may be difficult to read in a narrow screen, so try calling ./our_tester>log instead of calling
 * ./our_tester, so you can view the log like a human being.
 * We suggest you add the entire #ifndef HW3_DEBUG block to threadPool.h and use the PRINT() macro
 * in threadPool.c to help you figure out what's going on.
 *
 * IF YOU DO: Remember to set USE_GETTID and HW3_DEBUG to 0 before submitting!
 *
 * USAGE NOTES:
 * @ If you're running this on VMWare (Linux 2.4XXX), set USE_GETTID to 0 (defined as a macro bellow).
 *   Otherwise, you'll want to set USE_GETTID to 1 to use gettid() instead of getpid() - later versions
 *   of Linux return the same value of getpid() for threads in the same process, so calling getpid()
 *   would be useless.
 *   To get a thread ID, use TID(). It will choose between gettid() and getpid() for you.
 * @ If you want to print extra stuff in the tests, set HW3_DEBUG to 1 (also defined bellow)
 *   and use PRINT() instead of printf() (so you can easily get rid of extra garbage printing).
 * @ To write your own tests, you may find these useful:
 *   - INIT(n)
 *   - DESTROY(should)
 *   - SETUP_PTRS(n)
 *   - CREATE_TASKS(n,tp)
 *   - CREATE_TASKS_DELAY(n,tp,d)
 *   - ASSERT_TASKS_DONE(n)
 *   Read about them next to their definition. For some examples, see the existing test functions called
 *   via main().
 * @ We've set up a default task for threads to do. By default (if the global force_milliseconds is set to 0) it
 *   counts to some high random number (so different threads take different lengths of time to complete
 *   their task), and you can set force_milliseconds=X to force all threads to take X milliseconds to complete.
 *   Also, whenever you use the macros to creates the tasks, an array called completion[] will be created.
 *   completion[i]=0 <==> Task #i has been completed. Thus, you can make sure tasks are completed by testing
 *   the value of elements of completion[] (this is what ASSERT_TASKS_DONE does)
 */


#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "threadPool.h"

/*********************************************************************
 * Debug printing method.
 *
 * Uses the HW3_DEBUG flag to decide whether or not to print.
 *
 * We use an ifndef directive here because these macros are defined
 * in (our) threadPool.h, so for portability we redefine them here.
 *********************************************************************/
#ifndef HW3_DEBUG

#define HW3_DEBUG 0	// Set to 1 to print, set to 0 to prevent printing
#define USE_GETTID 1

#if HW3_DEBUG
#define PRINT(...) printf(__VA_ARGS__)
		#define PRINT_IF(cond,...) if(cond) PRINT(__VA_ARGS__)
#else
#define PRINT(...)		// If we're not debugging, just erase these lines
#define PRINT_IF(...)
#endif

#if USE_GETTID
#include <sys/syscall.h>		// For gettid()
#include <sys/time.h>
#include <unistd.h>

#define TID() syscall(SYS_gettid)
#else
#define TID() getpid()			// If syscalls.h isn't included, syscall(SYS_gettid) shouldn't appear anywhere in the code
#endif

#endif

/**********************************************************************
 * Some simple testing macros
 *********************************************************************/
#define FAIL(msg) do { \
		printf("FAIL! In %s, Line %d: " #msg "\n",__FILE__,__LINE__); \
		return 0; \
	} while(0)

#define ASSERT(x) do { \
		if (!(x)) FAIL(#x " is false"); \
	} while(0)

#define RUN_TEST(b,...) do { \
		printf("Running " #b ", " __VA_ARGS__ "...\n"); \
		if (b()) printf("OK\n"); \
	} while(0)


/**********************************************************************
 * Thread tasks
 *
 * These functions / global variables are used to give the threads something to do.
 * They're used for giving them tasks of varying lengths and printing them.
 *********************************************************************/
// Use these global variables to print the output of a test in a nice way
int* thread_ids;
int total_threads;
int* thread_progress;
OSQueue* task_log;

// Use this global variable to flag the random_task to be less random...
int force_milliseconds;
double MS_RATIO;

// Helper method to get the current time in milliseconds
unsigned long now_mil() {
    struct timeval t;
    gettimeofday(&t, 0);
    return (unsigned long)t.tv_sec * 1000 + (unsigned long)t.tv_usec / 1000;
}

// Call this function ONCE to try to find out how many for() iterations
// in one second
void init_ms_ratio() {
    long start,end;
    long i;
    start = now_mil();
    for(i=0; i<1000000000; ++i);
    end = now_mil();

    // How many milliseconds did it take to count to 1 billion?
    // Why, end-start of course!

    // Save the ratio for later use
    MS_RATIO = (double)1000000000 / (double)(end-start);

}

// Use this to update the progress of each thread in "real-time" (thanks to Roy for this idea)
void update_progress() {

    // First, find the index of the thread
    int index=-1, tid = TID();
    while(thread_ids[++index] != tid && index<total_threads);

    // Next, update the progress:
    if (thread_progress[index] < 0)
        thread_progress[index]=0;
    else {
        thread_progress[index]+=5;
        if (thread_progress[index]%1000 == 105) {
            thread_progress[index]+=1000;						// One more task done
            thread_progress[index]-=thread_progress[index]%1000;// Wipe the current progress
        }
    }

    // Print it
    printf("\r THREAD PROGRESS: [ ");
    int i;
    for (i=0; i<total_threads; ++i)
        printf("%3d%%/%d ",thread_progress[i]%1000,thread_progress[i]/1000+1);
    printf("]");
    fflush(NULL);

}

// Use this function to busy-wait for ms milliseconds
void waitfor(double ms) {
    if (ms<0) return;
    long total = MS_RATIO*ms;
    int i;
    for(i=0; i<total; ++i);
}

// A generic task to give to threads.
// Expects a valid pointer to an integer (so we can
// check to see if it's done)
// Assumes the integer sent is 0. Counts to some number, and when
// it's done inserts "1" to the int pointer sent.
void task_log_start();	// Declare these in advance, random_task uses them
void task_log_end();
void random_task(void* x) {

    // Get milliseconds, then split them into 20 chunks
    int i,r;
    r = force_milliseconds ?
        force_milliseconds :	// The number of desired milliseconds
        (rand()%10)*100;		// 10~1000 milliseconds, averages at about 500 milliseconds
    double chunk = (double)r/20;

    task_log_start();
    for (i=0; i<20; ++i) {
        waitfor(chunk);
        update_progress();
    }
    task_log_end();
    *((int*)x)=1;				// Done
    return;
}


// Use these functions in random_task to print the task state nicely.
// First, makes sure the thread exists in thread_ids (in log_start only).
// If not, adds it.
// Next, adds a new string to print in task_log* (both start() and end()).
typedef struct task_log_t {
    int index;			// Thread number (not thread ID, it's index in thread_ids[])
    int started;		// Started or ended with this event?
    long event_time;	// Time of event
} TaskLog;
void task_log_start() {

    // Find the thread index.
    // If it doesn't exists, create it
    int index, tid = TID();
    for (index=0; index<total_threads; ++index) {
        if (!thread_ids[index] || thread_ids[index] == tid) {
            thread_ids[index] = tid;
            break;
        }
    }

    // Update the progress bar
    update_progress();

    // Log the beginning of the task
    TaskLog* t = (TaskLog*)malloc(sizeof(TaskLog));
    t->index = index;
    t->started = 1;
    t->event_time = now_mil();
    osEnqueue(task_log, (void*)t);

}
void task_log_end() {

    // Find the thread index.
    int index=-1, tid = TID();
    while(thread_ids[++index] != tid && index<total_threads);

    // Log the beginning of the task
    TaskLog* t = (TaskLog*)malloc(sizeof(TaskLog));
    t->index = index;
    t->started = 0;
    t->event_time = now_mil();
    osEnqueue(task_log, (void*)t);

}
void destroy_log() {
    while(!osIsQueueEmpty(task_log)) {
        free((TaskLog*)osDequeue(task_log));
    }
    osDestroyQueue(task_log);
}
void print_task_table() {
    int i,j;
    printf("\n================================\n");
    printf("Task execution table by threads:\n");
    printf("================================\n");
    printf("   TIME:   ");
    for (i=0; i<total_threads; ++i)
        printf("| T%2d (TID=%5d) ",i+1,thread_ids[i]);
    printf("\n-----------");
    for (i=0; i<total_threads; ++i)
        printf("------------------");
    printf("\n");
    while(!osIsQueueEmpty(task_log)) {
        TaskLog* t = (TaskLog*)osDequeue(task_log);
        printf("%10lu |",t->event_time);
        for (i=0; i<t->index; ++i)
            printf("                 |");
        printf("      %s      |",t->started ? "START" : " END ");
        for (i=t->index+1; i<total_threads; ++i)
            printf("                 |");
        printf("\n");
        free(t);
    }
    printf("================================\n");
    printf(" Done printing execution table! \n");
    printf("================================\n");
}

/**********************************************************************
 * Macros to be used in test functions.
 *
 * For some examples, see the tests bellow.
 *********************************************************************/
// Creates a thread pool with n threads, and updates the globals.
#define INIT(n) \
	total_threads = n; \
	task_log = osCreateQueue(); \
	thread_ids = (int*)malloc(sizeof(int)*n); \
	thread_progress = (int*)malloc(sizeof(int)*n); \
	do { \
		int i; \
		for(i=0; i<n; ++i) { \
			thread_ids[i]=0; \
			thread_progress[i]=-1; \
		} \
	} while(0); \
	ThreadPool* tp = tpCreate(n)
// Destroys the thread pool (with the should_wait parameter)
// and takes care of other cleanup
#define DESTROY(should) do { \
		tpDestroy(tp,should); \
		print_task_table(); \
		free(thread_ids); \
		free(thread_progress); \
		total_threads = 0; \
		destroy_log(); \
	} while(0)
// Sets up an array of integers, so its pointers
// can be sent to the random task.
#define SETUP_PTRS(n) \
	int completion[n]; \
	do { \
		int i; \
		for (i=0; i<n; ++i) \
			completion[i]=0; \
	} while(0)
// Create n random tasks and insert into the pool
#define CREATE_TASKS(n,tp) CREATE_TASKS_DELAY(n,tp,0)
// Create n random tasks, insert them into the pool with some
// delay (in milliseconds) between insertions
#define CREATE_TASKS_DELAY(n,tp,ms) \
		SETUP_PTRS(n); \
		do { \
			int i,j; \
			for (i=0; i<n; ++i) \
				ASSERT(!tpInsertTask(tp,random_task,(void*)(completion+i))); \
			waitfor(ms); \
		} while(0)
// After all tasks should be done, use this to make sure it's true
#define ASSERT_TASKS_DONE(n) do { \
		int i; \
		for (i=0; i<n; ++i) ASSERT(completion[i]); \
	} while(0)


/**********************************************************************************************
 **********************************************************************************************
                                   TEST FUNCTIONS AND MAIN()
 **********************************************************************************************
 *********************************************************************************************/

int one_thread_one_long_task() {
    force_milliseconds = 5000;
    INIT(1);
    CREATE_TASKS(1,tp);
    DESTROY(1);
    ASSERT_TASKS_DONE(1);
    return 1;
}

int one_thread_three_tasks() {
    force_milliseconds = 500;
    INIT(1);
    CREATE_TASKS(3,tp);
    DESTROY(1);
    ASSERT_TASKS_DONE(3);
    return 1;
}

int three_threads_one_task() {
    force_milliseconds = 1000;
    INIT(3);
    CREATE_TASKS(1,tp);
    DESTROY(1);
    ASSERT_TASKS_DONE(1);
    return 1;
}

int three_threads_three_tasks() {
    force_milliseconds = 500;
    INIT(3);
    CREATE_TASKS(3,tp);
    DESTROY(1);
    ASSERT_TASKS_DONE(3);
    return 1;
}

/**
 * Just to see things basically work...
 */
int short_test() {
    force_milliseconds = 10;
    INIT(5);
    CREATE_TASKS(20, tp);
    DESTROY(1);
    ASSERT_TASKS_DONE(20);
    return 1;
}

/**
 * Medium duration test
 */
int medium_test() {
    force_milliseconds = 100;
    INIT(10);
    CREATE_TASKS(50, tp);
    DESTROY(1);
    ASSERT_TASKS_DONE(50);
    return 1;
}

/**
 * Give the threads long jobs
 */
int long_test() {
    force_milliseconds=1000;
    INIT(10);
    CREATE_TASKS(100,tp);
    DESTROY(1);
    ASSERT_TASKS_DONE(100);
    return 1;
}

/**
 * Just give them lots of tasks at once
 */
int stress_test() {
    force_milliseconds = 0;
    INIT(10);
    CREATE_TASKS(100,tp);
    DESTROY(1);
    ASSERT_TASKS_DONE(100);
    return 1;
}

/**
 * Give a task every X time, to give the threads some time to wait
 */
int delay_test() {
    force_milliseconds = 0;
    INIT(10);
    CREATE_TASKS_DELAY(100,tp,10);
    DESTROY(1);
    ASSERT_TASKS_DONE(100);
    return 1;
}

int main() {

    // Initialize the random number generator and the millisecond-for() loop ratio
    printf("Calculating MS_RATIO. Should take less than 30 seconds.\n");
    srand(time(NULL));
    init_ms_ratio();
    printf("MS_RATIO set to %f\n",MS_RATIO);	// May take a while. Less than a minute.

    // Run tests
    RUN_TEST(one_thread_one_long_task, "should take 5 seconds");
    RUN_TEST(one_thread_three_tasks, "should take 1.5 seconds");
    RUN_TEST(three_threads_one_task, "should take 1 second");
    RUN_TEST(three_threads_three_tasks, "should take 1.5 seconds");
    RUN_TEST(short_test, "should take 0.2 seconds");
    RUN_TEST(medium_test, "should take 5 seconds");
    RUN_TEST(long_test, "should take 100 seconds");
    RUN_TEST(stress_test, "should take about 50 seconds - average 500 milliseconds per task, 100 tasks");
    RUN_TEST(delay_test, "should take 51 seconds - average 500 milliseconds per task, 100 tasks, 10 ms delay per task");

    return 0;
}
