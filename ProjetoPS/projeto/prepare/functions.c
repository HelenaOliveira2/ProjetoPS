#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "functions.h"

void salvar_tarefa_binario(Task t) {
    char nome_ficheiro[100];
    
    // Cria a pasta "tasks_data" se não existir (permissões 0700)
    mkdir("../tasks_data", 0700);

    // Nome do ficheiro inclui o ID da tarefa conforme a "Dica" do enunciado
    snprintf(nome_ficheiro, sizeof(nome_ficheiro), "../tasks_data/task_%d.bin", t.id);

    // Abre para escrita, cria se não existir, limpa se já existir
    int fd = open(nome_ficheiro, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    
    if (fd < 0) {
        perror("Erro ao criar ficheiro da tarefa");
        printf("ola oi");
        printf("por favor dá");
        return;
    }

    // Escreve a struct completa em formato binário
    ssize_t bytes = write(fd, &t, sizeof(Task));
    if (bytes != sizeof(Task)) {
        perror("Erro ao escrever dados binários");
    }


    close(fd);
    printf("Tarefa %d (Duração: %ds) guardada com sucesso.\n", t.id, t.duration);
}