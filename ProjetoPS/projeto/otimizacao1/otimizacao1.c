#include <stdio.h>
#include <stdlib.h>
#include "functions.h"  // Inclui Task e protótipos

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <num_tarefas> <N_paralelo>\n", argv[0]);
        return 1;
    }

    int num_tarefas = atoi(argv[1]);
    int N = atoi(argv[2]);

    if (num_tarefas < 1 || N < 1) {
        printf("num_tarefas e N devem ser >= 1\n");
        return 1;
    }

    Task *tasks = malloc(num_tarefas * sizeof(Task));
    if (!tasks) {
        perror("Erro ao alocar memória");
        return 1;
    }

    for (int i = 0; i < num_tarefas; i++) {
        if (ler_tarefa_binario(i + 1, &tasks[i]) < 0) {
            fprintf(stderr, "Erro ao ler tarefa %d\n", i + 1);
            free(tasks);
            return 1;
        }
    }

    double turnaround_medio;
    executar_paralelo(tasks, num_tarefas, N, &turnaround_medio);
    escrever_estatisticas(num_tarefas, turnaround_medio);

    free(tasks);
    return 0;
}
