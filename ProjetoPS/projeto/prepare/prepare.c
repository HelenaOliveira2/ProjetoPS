#include <stdio.h>
#include <stdlib.h>
#include "functions.h"

int main(int argc, char *argv[]) {
    // Validação de argumentos: ./prepare <task_id> <duration>
    if (argc != 3) {
        printf("Uso: %s <task_id> <duration_in_seconds>\n", argv[0]);
        return 1;
    }

    Task nova_tarefa;
    nova_tarefa.id = atoi(argv[1]);
    nova_tarefa.duration = atoi(argv[2]);

    salvar_tarefa_binario(nova_tarefa);

    return 0;
}