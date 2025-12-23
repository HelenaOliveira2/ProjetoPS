#ifndef FUNCTIONS_H
#define FUNCTIONS_H

typedef struct task {
    int id;
    int duration; // duração em segundos
} Task;

// Funções para o prepare
void salvar_tarefa_binario(Task t);

// Funções para o scheduler
int ler_tarefa_binario(int id, Task *t);
void executar_fcfs(Task tasks[], int n, double *turnaround_medio);  // Task tasks[] → array de tarefas carregadas da pasta tasks_data, n → número de tarefas no array
void executar_paralelo(Task tasks[], int n, int max_proc, double *turnaround);
void ordenar_sjf(Task tasks[], int n);
void escrever_estatisticas(int total_tarefas, double turnaround_medio);

#endif
