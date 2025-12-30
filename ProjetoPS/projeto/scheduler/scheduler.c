#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h> 
#include "functions.h"
#define FIFO_PATH "/tmp/my_fifo"

int main(int argc, char *argv[]) {
    int fifo_fd = -1;

    // 1. Verificar se o número de argumentos base é suficiente
    // Uso: ./scheduler <num_tasks> <modo> [max_proc]
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <num_tasks> <modo> [max_proc]\n", argv[0]);
        fprintf(stderr, "Modos:\n");
        fprintf(stderr, " 0 - FCFS sequencial\n");
        fprintf(stderr, " 1 - FCFS paralelo\n");
        fprintf(stderr, " 2 - SJF sequencial\n");
        fprintf(stderr, " 3 - SJF paralelo\n");
        return -1;
    }

    int num_tasks = atoi(argv[1]);
    int modo = atoi(argv[2]);
    int max_proc = 2; 

    double turnaround_medio = 0;

    printf("MODO: %d | Tarefas: %d\n", modo, num_tasks);

    // 2. Verificar se o modo é válido
    if (modo < 0 || modo > 3) {
        printf("Modo inválido. Use 0: FCFS sequencial, 1: FCFS paralelo, 2: SJF sequencial, 3: SJF paralelo.\n");
        return -1;
    }

    // 3. Validação de Argumentos Específica por Modo
    if (modo == 0 || modo == 2) {
    // Não podem ter o 4º argumento (max_proc)
        if (argc > 3) {
            printf("Erro: O Modo %d não aceita o argumento 'max_proc'.\n", modo);
            printf("Uso correto: %s %d %d\n", argv[0], num_tasks, modo);
            return -1;
        }
    } 
    else if (modo == 1 || modo == 3) {
        // Obrigatório ter o 4º argumento
        if (argc < 4) {
            printf("Erro: O Modo %d precisa do argumento <max_proc>.\n", modo);
            printf("Uso correto: %s %d %d <max_proc>\n", argv[0], num_tasks, modo);
            return -1;
        }
        max_proc = atoi(argv[3]);

        if (max_proc <= 0) {
            printf("Erro: max_proc deve ser maior que 0.\n");
            return -1;
        }
    }

    // 4. Carregar as tarefas (Comum a todos os modos)
    Task tasks[num_tasks];
    for (int i = 0; i < num_tasks; i++) {
        if (ler_tarefa_binario(i + 1, &tasks[i]) < 0) {
            printf("Erro ao ler tarefa %d\n", i + 1);
            return -1;
        }
    }

    // --- MODO 0: SEM OTIMIZAÇÕES (FCFS SEQUENCIAL) ---
    if (modo == 0) {
        printf("A executar Modo Base (FCFS Sequencial)...\n");
        executar_fcfs(tasks, num_tasks, &turnaround_medio);
    }

    // --- MODO 1: OTIMIZAÇÃO DOS FORKS (FCFS PARALELO) ---
    if (modo == 1) {
        if (max_proc <= 0) {
            printf("O número de processos paralelos deve ser maior que zero.\n");
            return -1;
        }
        printf("A executar com otimização de Forks (N=%d)...\n", max_proc);
        executar_paralelo(tasks, num_tasks, max_proc, &turnaround_medio);
    }

    // --- MODO 2: OTIMIZAÇÃO SJF (SEQUENCIAL) ---
    if (modo == 2) {
        printf("A executar com otimização SJF (Sequencial)...\n");
        ordenar_sjf(tasks, num_tasks); // Primeiro ordena
        executar_fcfs(tasks, num_tasks, &turnaround_medio); // Depois executa sequencial
    }

    // --- MODO 3: TODAS AS OTIMIZAÇÕES (SJF + FORKS) ---
    if (modo == 3) {
        if (max_proc <= 0) {
            printf("O número de processos paralelos deve ser maior que zero.\n");
            return -1;
    }
    printf("A executar com todas as otimizações (SJF + Forks N=%d)...\n", max_proc);
    ordenar_sjf(tasks, num_tasks); // Primeiro ordena
    executar_paralelo(tasks, num_tasks, max_proc, &turnaround_medio); // Depois executa paralelo
    }

    // Guardar estatísticas finais
    escrever_estatisticas(num_tasks, turnaround_medio, modo);

    if (fifo_fd != -1) {
        close(fifo_fd);
    }

    return 0;
}