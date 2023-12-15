#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <string.h>

const int MAX_THREADS = 1024;

// a - number of tasks
// b - number of threads
long long a;
long b;

pthread_rwlock_t rwlock;
pthread_mutex_t mutex;
struct list_node_s
{
    int data;
    struct list_node_s *next;
};

struct list_node_s *head = NULL;

void *
Thread_work(void *rank);
int Member(int value, struct list_node_s *head_p);
int Insert(int value, struct list_node_s **head_pp);
int StartInsert(int value, struct list_node_s **head_pp);
int Delete(int value, struct list_node_s **head_pp);
void PrintList(struct list_node_s *head_p);

int main(int argc, char *argv[])
{
    long thread;
    pthread_t *thread_handles;
    srand(time(NULL));

    // generate 10 random numbers for the list
    for (int i = 0; i < 8; i++)
    {
        // Insert a random number
        StartInsert(rand() % 1000, &head);
    }

    printf("Enter the amount of tasks: ");
    scanf("%lld", &a);

    printf("Enter the amount of threads: ");
    scanf("%ld", &b);

    PrintList(head);

    thread_handles = malloc(b * sizeof(pthread_t));

    pthread_rwlock_init(&rwlock, NULL);
    pthread_mutex_init(&mutex, NULL);

    for (thread = 0; thread < b; thread++)
    {
        pthread_create(&thread_handles[thread], NULL, Thread_work,
                       (void *)thread);
    }

    for (thread = 0; thread < b; thread++)
    {
        pthread_join(thread_handles[thread], NULL);
    }
    PrintList(head);
    pthread_rwlock_destroy(&rwlock);
    pthread_mutex_destroy(&mutex);
    free(thread_handles);
    return 0;
}

int Member(int value, struct list_node_s *head_p)
{
    struct list_node_s *curr_p = head_p;

    while (curr_p != NULL && curr_p->data < value)
    {
        curr_p = curr_p->next;
    }

    if (curr_p == NULL || curr_p->data > value)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int Insert(int value, struct list_node_s **head_pp)
{
    struct list_node_s *curr_p = *head_pp;
    struct list_node_s *pred_p = NULL;
    struct list_node_s *temp_p;

    while (curr_p != NULL && curr_p->data < value)
    {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    if (curr_p == NULL || curr_p->data > value)
    {
        temp_p = malloc(sizeof(struct list_node_s));
        temp_p->data = value;
        temp_p->next = curr_p;
        if (pred_p == NULL)
        {
            *head_pp = temp_p;
        }
        else
        {
            pred_p->next = temp_p;
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

// insert method for when the python is created
int StartInsert(int value, struct list_node_s **head_pp)
{
    struct list_node_s *curr_p = *head_pp;
    struct list_node_s *pred_p = NULL;
    struct list_node_s *temp_p;

    while (curr_p != NULL && curr_p->data < value)
    {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    if (curr_p == NULL || curr_p->data > value)
    {
        temp_p = malloc(sizeof(struct list_node_s));
        temp_p->data = value;
        temp_p->next = curr_p;
        if (pred_p == NULL)
        {
            *head_pp = temp_p;
        }
        else
        {
            pred_p->next = temp_p;
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

int Delete(int value, struct list_node_s **head_pp)
{
    struct list_node_s *curr_p = *head_pp;
    struct list_node_s *pred_p = NULL;

    while (curr_p != NULL && curr_p->data < value)
    {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    if (curr_p != NULL && curr_p->data == value)
    {
        if (pred_p == NULL)
        {
            *head_pp = curr_p->next;
            free(curr_p);
        }
        else
        {
            pred_p->next = curr_p->next;
            free(curr_p);
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

void PrintList(struct list_node_s *head_p)
{
    pthread_rwlock_rdlock(&rwlock);
    struct list_node_s *curr_p = head_p;

    printf("\nPrinting list: ");
    while (curr_p != NULL)
    {
        printf("%d ", curr_p->data);
        curr_p = curr_p->next;
    }
    printf("\n");

    pthread_rwlock_unlock(&rwlock);
}

void *Thread_work(void *rank)
{
    long my_rank = (long)rank;
    long long i;
    long long my_tasks = a / b;
    unsigned int seed = (unsigned int)time(NULL) + (unsigned int)my_rank;
    // srand(time(NULL) + my_rank);

    for (i = 0; i < my_tasks; i++)
    {
        int operation = rand_r(&seed) % 3;
        int number = rand_r(&seed) % 1000;

        pthread_rwlock_rdlock(&rwlock);
        switch (operation)
        {
        case 0:
            int member = Member(number, head);
            pthread_mutex_lock(&mutex);

            printf("\nThread %ld: Checking if value %d is in the list. Result: %d", my_rank, number, member);

            pthread_mutex_unlock(&mutex);
            break;
        case 1:
            Insert(number, &head);
            pthread_rwlock_unlock(&rwlock);
            pthread_mutex_lock(&mutex);
            printf("\nThread %ld: Inserting value %d in the list", my_rank, number);
            PrintList(head);
            pthread_mutex_unlock(&mutex);
            pthread_rwlock_rdlock(&rwlock);
            break;
        case 2:
            int delete = Delete(number, &head);
            pthread_rwlock_unlock(&rwlock);
            pthread_mutex_lock(&mutex);
            printf("\nThread %ld: Deleting value %d if it's in the list. Result: %d", my_rank, number, delete);
            PrintList(head);
            pthread_mutex_unlock(&mutex);
            pthread_rwlock_rdlock(&rwlock);
            break;
        default:
            break;
        }
        pthread_rwlock_unlock(&rwlock);
    }
    return NULL;
}
