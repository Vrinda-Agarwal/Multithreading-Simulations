#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <assert.h>
#include <time.h>
#include <semaphore.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
sem_t sem;
int M2;
int time_since_start;
int counter = 0;
int people_who_have_left = 0;
int time_that_has_been_wasted = 0;
struct input
{
    int ID;
    int T_i;
    int W_i;
    int P_i;
};
int comparator(const void *a, const void *b)
{
    struct input *a1 = (struct input *)a;
    struct input *b1 = (struct input *)b;
    // int *x = (int *)(*a1->T_i);
    // int *y = (int *)(*b1->T_i);
    // return *x-*y;
    return (a1->T_i - b1->T_i);
}
// void* func(struct input a, int M2)
void *func(void *a0)
{
    struct input *a = (struct input *)a0;
    sleep(a->T_i);
    int time_arrived = time_since_start;
    printf("%d : Student %d arrives\n\033[0m", time_since_start, a->ID);
    // if (M2 > 0)
    // {
    //     sem_wait(&sem);
    //     printf("Student %d starts washing\n", a->ID);
    //     M2 -= 1;
    //     sleep(a->W_i);
    //     M2 += 1;
    //     sem_post(&sem);
    //     printf("Student %d leaves after washing\n", a->ID);
    //     return NULL;
    // }
    // else if (M2 <= 0)
    // {
    struct timespec pp;
    if (clock_gettime(CLOCK_REALTIME, &pp) == -1)
    {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }
    pp.tv_sec += a->P_i;
    int s;
    while ((s = sem_timedwait(&sem, &pp)) == -1 && errno == EINTR)
        continue;
    if (s == -1)
    {
        counter++;
        people_who_have_left++;
        time_that_has_been_wasted += time_since_start - time_arrived;
        printf("\033[0;31m%d : Student %d leaves without washing\n\033[0m", time_since_start, a->ID);
    }
    else
    {
        // printf("sem_timedwait() succeeded\n");
        // sem_wait(&sem);
        time_that_has_been_wasted += time_since_start - time_arrived;
        printf("\033[0;32m%d : Student %d starts washing\n\033[0m", time_since_start, a->ID);
        int started_washing = time_since_start;
        M2 -= 1;
        // sleep(a->W_i);
        M2 += 1;
        for (;;)
        {
            if (time_since_start == started_washing + a->W_i)
                break;
        }
        printf("\033[0;33m%d : Student %d leaves after washing\n\033[0m", time_since_start, a->ID);
        people_who_have_left++;
        sem_post(&sem);
        return NULL;
    }
    return NULL;
}
struct input inputs[100]; // array of structures
int main()
{
    struct timespec start;
    time_t t = time(NULL);
    int N, M;
    scanf("%d %d", &N, &M);
    M2 = M;
    int St_cloth_nowash = 0;
    int St_wait_time = 0;
    for (int i = 0; i < N; ++i)
    {
        int Ti, Wi, Pi;
        scanf("%d %d %d", &Ti, &Wi, &Pi);
        inputs[i].T_i = Ti;
        inputs[i].W_i = Wi;
        inputs[i].P_i = Pi;
        inputs[i].ID = i + 1;
    }
    // for(int i=0;i<N;++i){
    //     printf("%d %d %d %d\n",inputs[i].ID,inputs[i].T_i,inputs[i].W_i,inputs[i].P_i);
    // }
    qsort(inputs, N, sizeof(struct input), comparator); // sorting on the basis of arrival time
    // if arrival time is same then , it is sorted on the basis of ID used in the structure for each student

    // for(int i=0;i<N;++i){
    //     printf("%d %d %d %d\n",inputs[i].ID,inputs[i].T_i,inputs[i].W_i,inputs[i].P_i);
    // }
    if (clock_gettime(CLOCK_REALTIME, &start) == -1)
    {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }
    struct timespec v = start;

    // sem = *sem_open("/SEMAPHORE", O_CREAT, 0777, N);
    // if (&sem == SEM_FAILED)
    // {
    //     perror("Sem couldn't be accesed:");
    //     exit(EXIT_FAILURE);
    // }
    sem_init(&sem, 0, M);
    pthread_t p[N];
    for (int i = 0; i < N; ++i)
    {
        int cc;
        cc = pthread_create(&p[i], NULL, func, &inputs[i]);
        assert(cc == 0);
    }
    while (1)
    {
        struct timespec time_stamp;
        if (clock_gettime(CLOCK_REALTIME, &time_stamp) == -1)
        {
            perror("clock_gettime");
            exit(EXIT_FAILURE);
        }
        time_since_start = time_stamp.tv_sec - start.tv_sec;
        if (people_who_have_left == N)
            break;
    }
    printf("%d\n%d\n",counter,time_that_has_been_wasted);
    // printf("Done\n");
    //  sleep(10000);
    //  if (sem_unlink("/SEMAPHORE") == -1)
    //  {
    //      perror("sem_unlink");
    //      exit(EXIT_FAILURE);
    //  }
    if((counter/N)>0.25){
        printf("Yes\n");
    }
    else{
        printf("No\n");
    }
}