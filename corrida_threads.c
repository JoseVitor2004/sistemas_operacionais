#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_barrier_t barreira;
pthread_mutex_t mutex_vencedor;
int id_vencedor = -1; 

void* iniciar_corrida(void* arg) {
    int meu_id = *(int*)arg;

    pthread_barrier_wait(&barreira);

    for (int i = 0; i < 100; i++) {
        printf("Thread %d: %d\n", meu_id, i);
    }

    pthread_mutex_lock(&mutex_vencedor);
    if (id_vencedor == -1) {
        id_vencedor = meu_id;
    }
    pthread_mutex_unlock(&mutex_vencedor);

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int X = 5; 

    if (argc > 1) {
        X = atoi(argv[1]);
        if (X <= 0) {
            printf("Por favor, insira um número de threads maior que 0.\n");
            return 1;
        }
    }

    pthread_t *threads = malloc(X * sizeof(pthread_t));
    int *thread_ids = malloc(X * sizeof(int));
    pthread_barrier_init(&barreira, NULL, X);
    pthread_mutex_init(&mutex_vencedor, NULL);

    for (int i = 0; i < X; i++) {
        thread_ids[i] = i;
        if (pthread_create(&threads[i], NULL, iniciar_corrida, &thread_ids[i]) != 0) {
            perror("Erro ao criar thread");
            return 1;
        }
    }
    for (int i = 0; i < X; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\n--------------------------------------\n");
    printf("A thread vencedora foi a  %d!\n", id_vencedor);

    pthread_barrier_destroy(&barreira);
    pthread_mutex_destroy(&mutex_vencedor);
    free(threads);
    free(thread_ids);
    return 0;
}