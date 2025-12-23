#ifndef FUNCTIONS_H
#define FUNCTIONS_H

// Estrutura Task
typedef struct {
    int id;
    int duration;
} Task;

// Protótipos das funções
int ler_tarefa_binario(int id, Task *t);
void executar_paralelo(Task tasks[], int total_tarefas, int N, double *turnaround_medio);
void escrever_estatisticas(int total_tarefas, double turnaround_medio);

#endif
