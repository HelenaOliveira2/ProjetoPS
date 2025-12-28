#include <stdio.h>
#include <stdlib.h>
#include "functions.h"

int main(int argc, char *argv[]) {

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
    
    // Lê max_proc se existir, senão assume 1
    int max_proc = (argc == 4) ? atoi(argv[3]) : 1;  //Se o utilizador passou 4 argumentos, então:lê o valor de max_proc do terminal. Caso contrário: usa 1 como valor padrão

    double turnaround_medio = 0;

    printf("MODO: %d | Tarefas: %d\n", modo, num_tasks);
    
    // 2. Verificar se o modo é válido
    if (modo < 0 || modo > 3) {
        printf("Modo inválido. Use 0: FCFS sequencial, 1: FCFS paralelo, 2: SJF sequencial, 3: SJF paralelo.\n");
        return -1;
    }

    // 3. Caso o modo precise de forks (1 ou 3), verifica se o argumento foi passado
    if ((modo == 1 || modo == 3) && argc < 4) {
        printf("Para os modos com paralelismo (1 ou 3), é necessário fornecer o max_proc.\n");
        return -1;
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
        if (max_proc) {
            printf("Registe apenas 2 argumentos.\n");
            return -1;
        }
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
        if (max_proc) {
            printf("Registe apenas 2 argumentos.\n");
            return -1;
        }
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

    // 5. Guardar estatísticas finais
    escrever_estatisticas(num_tasks, turnaround_medio);

    return 0;
}