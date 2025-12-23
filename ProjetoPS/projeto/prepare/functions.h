#ifndef FUNCTIONS_H
#define FUNCTIONS_H

typedef struct task {
    int id;       // identificador da tarefa
    int duration; // duração em segundos
} Task;

// Protótipo da função para guardar a tarefa
void salvar_tarefa_binario(Task t);

#endif