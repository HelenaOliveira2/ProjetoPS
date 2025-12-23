#include <stdio.h>
#include <stdlib.h>
#include "functions.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <num_tasks> [max_proc]\n", argv[0]);
        return 1;
    }

    int num_tasks = atoi(argv[1]);
    // Se o utilizador não passar N, o padrão é 1 (sequencial - modo base)
    int max_proc = (argc == 3) ? atoi(argv[2]) : 1;

    if (num_tasks <= 0) {
        fprintf(stderr, "Número de tarefas inválido\n");
        return 1;
    }

    Task tasks[num_tasks];

    // Ler todas as tarefas
    for (int i = 0; i < num_tasks; i++) {
        if (ler_tarefa_binario(i + 1, &tasks[i]) < 0) {
            fprintf(stderr, "Erro ao ler tarefa %d\n", i + 1);
            return 1;
        }
    }

    // Executar FCFS
    double turnaround_medio = 0;
    if (max_proc > 1) {
        printf("--- Otimização 1: Execução em Paralelo (N=%d) ---\n", max_proc);
        executar_paralelo(tasks, num_tasks, max_proc, &turnaround_medio);
    } else {
        printf("--- Modo Base: Execução Sequencial (FCFS) ---\n");
        executar_fcfs(tasks, num_tasks, &turnaround_medio);
    }

    // Guardar estatísticas
    escrever_estatisticas(num_tasks, turnaround_medio);

    return 0;
}
