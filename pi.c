/*
 *      pi.c
 *      Approximates PI integrating the area of a quarter of circle with
 *      riemann integration.
 *
 *      by Matheus Tavares and Giuliano Belinassi
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

unsigned num_threads; // number of worker threads
unsigned N; // number of divisions at integration
double pi_by_4 = 0; // approximation of pi/4
pthread_mutex_t lock; // protects pi_by_4 writes

// Defines a task of integration over the quarter of circle, with an x-axis
// start and end indexes. The work must be done in the interval [start, end[
struct task {
        int start, end;
};

#define DIE(...) { \
        fprintf(stderr, __VA_ARGS__); \
        exit(EXIT_FAILURE); \
}

// This is the function to be executed by all worker_threads. It receives a task
// and sums the thread's work at global pi_by_4 variable.
void *thread_work(void *arg) {
        struct task *t = (struct task *)arg;
        double acc = 0; // Thread's local integration variable
        double interval_size = 1.0 / N; // The circle radius is 1.0

        // Integrates f(x) = sqrt(1 - x^2) in [t->start, t->end[
	for(int i = t->start; i < t->end; ++i) {
                double x = (i * interval_size) + interval_size / 2;
                acc += sqrt(1 - (x * x)) * interval_size;
	}

        // This is a critical section. As we are going to write to a global
        // value, the operation must me protected against race conditions.
        pthread_mutex_lock(&lock);
        pi_by_4 += acc;
        pthread_mutex_unlock(&lock);

        return NULL;
}


int main(int argc, char **argv)
{
        pthread_t *threads;
        struct task *tasks;

        // Argument parsing
        if (argc != 3 || sscanf(argv[1], "%u", &num_threads) != 1 ||
            sscanf(argv[2], "%u", &N) != 1) {
                printf("usage: %s <num_threads> <integration_divisions>\n",
                       argv[0]);
                return 1;
        }

        // Initialize mutex with default attributes
        if(pthread_mutex_init(&lock, NULL))
                DIE("Failed to init mutex\n");

        // Malloc threads' and tasks' arrays
        if((threads = malloc(num_threads * sizeof(pthread_t))) == NULL)
                DIE("Threads malloc failed\n");
        if((tasks = malloc(num_threads * sizeof(struct task))) == NULL)
                DIE("Tasks malloc failed\n");

        // Initialize threads with default attributes.
        // The work is being splitted as evenly as possible between threads.
        int threads_with_one_more_work = N % num_threads;
        for (int i = 0; i < num_threads; ++i) {
                int work_size = N / num_threads;
                if (i < threads_with_one_more_work)
                        work_size += 1;
                tasks[i].start = i * work_size;
                tasks[i].end = (i + 1) * work_size;
                if(pthread_create(&threads[i], NULL, thread_work, (void *)&tasks[i]))
                        DIE("Failed to create thread %d\n", i)
        }

        // Finish threads and ignore their return values
        for (int i = 0; i < num_threads; ++i) {
                if(pthread_join(threads[i], NULL))
                        DIE("failed to join thread %d\n", i);
        }

        printf("pi ~= %.12f\n", pi_by_4 * 4);

        if(pthread_mutex_destroy(&lock)) // Destroy mutex
                DIE("Failed to destroy mutex\n");
        free(threads);
        free(tasks);
        return 0;
}
