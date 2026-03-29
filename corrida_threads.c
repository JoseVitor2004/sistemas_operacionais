#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Variáveis globais para sincronização
pthread_barrier_t barreira;
pthread_mutex_t mutex_vencedor;
int id_vencedor = -1; // -1 indica que ninguém venceu ainda

// Função que cada thread irá executar
void* iniciar_corrida(void* arg) {
    int meu_id = *(int*)arg;

    // 1. Barreira: Nenhuma thread passa daqui até que todas as X threads cheguem
    pthread_barrier_wait(&barreira);

    // 2. A corrida (contagem de 0 a 99)
    for (int i = 0; i < 100; i++) {
        // printf é thread-safe, mas as impressões vão se misturar na tela (isso é normal e esperado)
        printf("Thread %d: %d\n", meu_id, i);
    }

    // 3. Linha de chegada: Tenta se declarar como vencedora
    pthread_mutex_lock(&mutex_vencedor);
    if (id_vencedor == -1) {
        // Se ainda for -1, significa que sou a primeira a chegar aqui!
        id_vencedor = meu_id;
    }
    pthread_mutex_unlock(&mutex_vencedor);

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int X = 5; // Número padrão de threads se o usuário não informar

    if (argc > 1) {
        X = atoi(argv[1]);
        if (X <= 0) {
            printf("Por favor, insira um número de threads maior que 0.\n");
            return 1;
        }
    }

    printf("Preparando corrida com %d threads...\n\n", X);

    // Alocação de memória para os IDs e as threads
    pthread_t *threads = malloc(X * sizeof(pthread_t));
    int *thread_ids = malloc(X * sizeof(int));

    // Inicialização da barreira (espera X threads) e do mutex
    pthread_barrier_init(&barreira, NULL, X);
    pthread_mutex_init(&mutex_vencedor, NULL);

    // Criação das threads
    for (int i = 0; i < X; i++) {
        thread_ids[i] = i;
        if (pthread_create(&threads[i], NULL, iniciar_corrida, &thread_ids[i]) != 0) {
            perror("Erro ao criar thread");
            return 1;
        }
    }

    // Aguarda todas as threads terminarem
    for (int i = 0; i < X; i++) {
        pthread_join(threads[i], NULL);
    }

    // Resultados
    printf("\n====================================================\n");
    printf("🏆 A thread vencedora foi a de ID: %d!\n", id_vencedor);
    printf("====================================================\n");

    // Limpeza de recursos
    pthread_barrier_destroy(&barreira);
    pthread_mutex_destroy(&mutex_vencedor);
    free(threads);
    free(thread_ids);

    return 0;
}