#include <math.h>
#include <pthread.h>
#include <fcntl.h>
#include <inttypes.h>
#include <limits.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#define gettid() syscall(SYS_gettid)
// struct sigaction
#define _XOPEN_SOURCE
#define BUFFER_SIZE 200

//Element that producer thread pass it to consumer thread
typedef struct struct_Element
{
    int currentIndex;
    //sum which producer thread find
    int sum;
    //check visited or not
    int visited;
    //array that producer thread visited
    int *path;
} Element;

pthread_t producer, consumers[8];
int numOfConsumers;
int consumersCount;
Element buffer[BUFFER_SIZE];
Element currentElem[8];
int tid[8];
int searchCountProducerSum;
int searchCountConsumersSum;
//store each consumer thread search count
int searchCount[8];
int **cache;
int map[50][50];
int curr_prod = 0;
int curr_cons = 0;

pthread_mutex_t consMutex;
pthread_mutex_t prodMutex;
pthread_cond_t consCond;
pthread_cond_t prodCond;

int bestResult = 99999;
int fastestWay[50];
int fileLength;

int min(int, int);
int getConsumerNumber(void);
int TSP_consumer(int *, int, int, int, int, int);
int TSP_producer(int *, int, int, int, int);
void getUserInput(void);
int getNum(const char *);
int findFileLength(FILE *);
void *producer_thread(void *);
void *consumer_thread(void *);
void handleSigaction(struct sigaction *);
void showThread(void);
void showStat(void);
void onDisconnect(int);

int min(int a, int b)
{
    return a < b ? a : b;
}
int TSP_consumer(int *pathRecord, int sum, int threadNumber, int count, int currentIndex, int visited)
{
    pathRecord[count] = currentIndex;

    pthread_mutex_lock(&consMutex);
    //critical section
    searchCountConsumersSum++;
    searchCount[threadNumber]++;

    pthread_mutex_unlock(&consMutex);

    //if visited all
    if (visited >= (1 << fileLength) - 1)
    {
        //if it is smallest
        if (sum < bestResult)
        {
            //critical section: write to global varable
            pthread_mutex_lock(&consMutex);
            bestResult = sum + map[currentIndex][pathRecord[0]];
            for (int i = 0; i < fileLength; i++)
            {
                fastestWay[i] = pathRecord[i];
            }
            //unlcok
            pthread_mutex_unlock(&consMutex);
        }
        return sum + map[currentIndex][pathRecord[0]];
    }
    int *ptr = &cache[currentIndex][visited];
    //check it is checked or not
    if (*ptr && *ptr != 99999)
    {
        return sum + (*ptr);
    }
    *ptr = 99999;

    for (int next = 0; next < fileLength; next++)
    {
        //if the route is visited
        if (currentIndex == next)
            continue;
        //if it is visited
        if (visited & (1 << next))
            continue;
        //find the smallest
        *ptr = min(*ptr, TSP_consumer(pathRecord, sum + map[currentIndex][next], threadNumber, count + 1, next, visited | (1 << next)) +
                             map[currentIndex][next]);
    }
    return *ptr;
}
int TSP_producer(int *pathRecord, int sum, int currentIndex, int visited, int count)
{
    pathRecord[count] = currentIndex;
    searchCountProducerSum++;
    if (count == fileLength - 12)
    {
        //critical section: use global variable
        pthread_mutex_lock(&prodMutex);
        //if buffer is not empty
        while (buffer[curr_prod].visited)
        {
            pthread_cond_wait(&prodCond, &prodMutex);
        }

        //Write values
        buffer[curr_prod].sum = sum;
        buffer[curr_prod].currentIndex = currentIndex;
        buffer[curr_prod].visited = visited;
        for (int i = 0; i < count + 1; i++)
        {
            buffer[curr_prod].path[i] = pathRecord[i];
        }
        curr_prod = (curr_prod + 1) % BUFFER_SIZE;

        //unlock after work
        pthread_mutex_unlock(&prodMutex);

        pthread_mutex_lock(&consMutex);
        pthread_cond_signal(&consCond);
        pthread_mutex_unlock(&consMutex);
        return sum;
    }
    int *ptr = &cache[currentIndex][visited];
    //check it is checked or not
    if (*ptr && *ptr != 99999)
    {
        return sum + (*ptr);
    }
    *ptr = 99999;

    for (int next = 0; next < fileLength; next++)
    {
        //if the route is visited
        if (currentIndex == next)
            continue;
        //if it is visited
        if (visited & (1 << next))
            continue;
        //find the smallest
        *ptr = min(*ptr, TSP_producer(pathRecord, sum + map[currentIndex][next], next, visited | (1 << next), count + 1) +
                             map[currentIndex][next]);
    }
    return *ptr;
}

//return producer threads number
int getConsumerNumber(void)
{
    //cosumer thread id
    return consumersCount++;
}

void getUserInput(void)
{
    char buffer[100];
    while (1)
    {
        fgets(buffer, 100, stdin);
        //if every threads are terminated, finish it
        if (numOfConsumers == 0)
        {
            return;
        }
        if (strncmp(buffer, "stat", strlen("stat")) == 0)
        {
            showStat();
        }
        else if (strncmp(buffer, "threads", strlen("threads")) == 0)
        {
            showThread();
            showStat();
        }
        else if (strncmp(buffer, "num", strlen("num")) == 0)
        {
            int num = getNum(buffer);
            if (1 > num || num > 8)
            {
                printf("You need to enter right number for threads(1 to 8)\n");
                exit(1);
            }

            int currentCount = numOfConsumers;
            numOfConsumers = num;
            int err;
            for (int i = 0; i < currentCount; i++)
            {
                pthread_cancel(consumers[i]);
            }

            for (int i = 0; i < num; i++)
            {
                err = pthread_create(&consumers[i], NULL, consumer_thread, NULL);
                if (err)
                {
                    printf("Fail to create thread\n");
                    exit(1);
                }
            }
        }
        else
        {
            printf("Wrong keyword\n");
        }
    }
}

int getNum(const char *buffer)
{
    int result;
    char *tempPtr = (char *)calloc(strlen(buffer) + 1, sizeof(char));
    char *original = tempPtr;
    strcpy(tempPtr, buffer);
    tempPtr = strtok(tempPtr, " ");
    tempPtr = strtok(NULL, " ");
    result = atoi(tempPtr);
    free(original);
    return result;
}
int findFileLength(FILE *fp)
{
    int count = 0, temp;
    while (fscanf(fp, "%d", &temp) == 1)
    {
        count++;
    }
    int n = (int)sqrt(count);
    rewind(fp);
    return n;
}

//producer thread
void *producer_thread(void *ptr)
{
    int path[50] = {
        0,
    };
    for (int i = 0; i < fileLength; i++)
    {
        TSP_producer(path, 0, i, 1 << i, 0);
    }
    buffer[curr_prod].currentIndex = -5555;
    return NULL;
}
void *consumer_thread(void *ptr)
{
    Element elem;
    int consumerNumber = getConsumerNumber();
    tid[consumerNumber] = gettid();
    int currentRecord[50];
    //when it resume, store current element
    if (currentElem[consumerNumber].visited)
    {
        elem = currentElem[consumerNumber];
        for (int next = 0; next < fileLength; next++)
        {
            //if go to same place, don't
            if (elem.currentIndex == next)
                continue;
            //if already visited, don't
            if (elem.visited & (1 << next))
                continue;
            TSP_consumer(currentRecord, elem.sum + map[elem.currentIndex][next], consumerNumber, fileLength - 11, next, elem.visited | (1 << next));
        }
        currentElem[consumerNumber].sum = 0;
        currentElem[consumerNumber].currentIndex = 0;
        currentElem[consumerNumber].visited = 0;
    }
    while (1)
    {
        pthread_mutex_lock(&consMutex);
        //Producer thread terminate consumer thread
        if (buffer[curr_cons].currentIndex == -5555)
        {
            numOfConsumers--;
            pthread_mutex_unlock(&consMutex);
            pthread_cond_signal(&consCond);
            if (numOfConsumers == 0)
            {
                printf("\nTask Done!\n");
                onDisconnect(0);
            }
            break;
        }

        while (buffer[curr_cons].visited == 0)
        {
            pthread_cond_wait(&consCond, &consMutex);
        }

        elem = buffer[curr_cons];
        for (int i = 0; i < fileLength - 11; i++)
        {
            currentRecord[i] = elem.path[i];
        }
        buffer[curr_cons].sum = 0;
        buffer[curr_cons].currentIndex = 0;
        buffer[curr_cons].visited = 0;
        currentElem[consumerNumber].sum = elem.sum;
        currentElem[consumerNumber].currentIndex = elem.currentIndex;
        currentElem[consumerNumber].visited = elem.visited;

        curr_cons = (curr_cons + 1) % BUFFER_SIZE;
        pthread_mutex_unlock(&consMutex);
        pthread_mutex_lock(&prodMutex);
        pthread_cond_signal(&prodCond);
        pthread_mutex_unlock(&prodMutex);

        for (int next = 0; next < fileLength; next++)
        {
            if (elem.currentIndex == next)
            {
                continue;
            }
            if (elem.visited & (1 << next))
                continue;
            TSP_consumer(currentRecord, elem.sum + map[elem.currentIndex][next],
                         consumerNumber, fileLength - 11, next,
                         elem.visited | (1 << next));
        }
        currentElem[consumerNumber].sum = 0;
        currentElem[consumerNumber].currentIndex = 0;
        currentElem[consumerNumber].visited = 0;
    }

    pthread_exit(NULL);

    return NULL;
}

// handler when user press ctrl + c
void handleSigaction(struct sigaction *actionPtr)
{
    memset(actionPtr, 0, sizeof(*actionPtr));
    actionPtr->sa_handler = onDisconnect;
    sigaction(SIGINT, actionPtr, NULL);
    sigaction(SIGQUIT, actionPtr, NULL);
    sigaction(SIGTERM, actionPtr, NULL);
    sigaction(SIGQUIT, actionPtr, NULL);
}
void showThread(void)
{
    for (int i = 0; i < numOfConsumers; i++)
    {
        printf("Thread Number : %d\n  TID: %d\n   searched routes of subtask: %d\n", i + 1, tid[i], searchCount[i]);
    }
}
void showStat(void)
{
    printf("Number of searched routes (except cached routes): %d\n", searchCountProducerSum + searchCountConsumersSum);
    printf("Number of searched routes by Producer Thread: %d\n", searchCountProducerSum);
    printf("Number of searched routes by Consumer Threads: %d\n", searchCountConsumersSum);
    //if it is same as init num, don't print it
    if (bestResult == 99999)
    {
        return;
    }
    printf("Current Lowest Sum of Weights: %d\n", bestResult);
    printf("Way: ");
    for (int i = 0; i < fileLength; i++)
    {
        printf("%d->", fastestWay[i]);
    }
    printf("%d\n", fastestWay[0]);
}

// When you terminate the program, free the memory and terminate
void onDisconnect(int sig)
{
    showStat();
    showThread();

    //close threads
    for (int i = 0; i < numOfConsumers; i++)
    {
        pthread_cancel(consumers[i]);
    }
    pthread_cancel(producer);

    //close mutex
    pthread_mutex_destroy(&prodMutex);
    pthread_mutex_destroy(&consMutex);

    pthread_cond_destroy(&prodCond);
    pthread_cond_destroy(&consCond);

    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        free(buffer[i].path);
    }

    for (int i = 0; i < fileLength; i++)
    {
        free(cache[i]);
    }
    free(cache);

    pthread_exit(NULL);
    exit(0);
}

int main(int argc, char **argv)
{

    //If user press ctrl+c, handle it
    struct sigaction action;
    handleSigaction(&action);

    int err;

    //Check user enter right arguments
    if (argc == 1)
    {
        printf("Enter the file name and a number of threads\n");
        exit(1);
    }
    else if (argc == 2)
    {
        printf("Enter a number of threads\n");
        exit(1);
    }
    else if (argc > 3)
    {
        printf("Enter only the file name and a number of threads\n");
        exit(1);
    }

    char *filename = argv[1];
    numOfConsumers = atoi(argv[2]);
    //If user enter under or over number of threads
    if (1 > numOfConsumers || numOfConsumers > 8)
    {
        printf("You need to enter right number for threads(1 to 8)\n");
        exit(1);
    }

    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("%s is not opend\n", filename);
    }
    fileLength = findFileLength(fp);

    printf("number of file rows: %d\n", fileLength);
    printf("Calculating...\n");
    for (int i = 0; i < fileLength; i++)
    {
        for (int j = 0; j < fileLength; j++)
        {
            fscanf(fp, "%d", &map[i][j]);
        }
    }
    fclose(fp);

    //Init buffer
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        buffer[i].path = (int *)calloc(50, sizeof(int));
    }

    cache = (int **)calloc(fileLength, sizeof(int *));
    if (cache == NULL)
    {
        printf("Fail to memory allocation\n");
        exit(1);
    }
    for (int i = 0; i < fileLength; i++)
    {
        cache[i] = (int *)calloc(1 << fileLength, sizeof(int));
        if (cache[i] == NULL)
        {
            printf("Fail to memory allocation\n");
            exit(1);
        }
    }

    //Init mutexes
    pthread_mutex_init(&consMutex, NULL);
    pthread_mutex_init(&prodMutex, NULL);
    pthread_cond_init(&consCond, NULL);
    pthread_cond_init(&prodCond, NULL);

    //create threads
    err = pthread_create(&producer, NULL, producer_thread, NULL);
    if (err)
    {
        printf("Fail to create thread\n");
        exit(1);
    }

    for (int i = 0; i < numOfConsumers; i++)
    {
        err = pthread_create(&consumers[i], NULL, consumer_thread, NULL);
        if (err)
        {
            printf("Fail to create thread\n");
            exit(1);
        }
    }

    getUserInput();

    return 0;
}