#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include "functions.h"
#include <stdlib.h>
#include <string.h>


int ler_tarefa_binario(int id, Task *t) {
    char nome_ficheiro[100];
    snprintf(nome_ficheiro, sizeof(nome_ficheiro),  "../tasks_data/task_%d.bin", id);

    int fd = open(nome_ficheiro, O_RDONLY);
    if (fd < 0) {
        perror("Erro ao abrir ficheiro da tarefa");
        return -1;
    }

    ssize_t bytes = read(fd, t, sizeof(Task));
    if (bytes != sizeof(Task)) {
        perror("Erro ao ler dados binários");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

void executar_fcfs(Task tasks[], int n, double *turnaround_medio) {
    time_t inicio, fim;
    double total_turnaround = 0;

    for (int i = 0; i < n; i++) {
        printf("A executar tarefa %d (Duração: %ds)...\n", tasks[i].id, tasks[i].duration);

        time(&inicio);
        sleep(tasks[i].duration);  // simula execução
        time(&fim);

        double turnaround = difftime(fim, inicio);
        printf("Tarefa %d concluída. Turnaround: %.2f s\n",tasks[i].id, turnaround);
        total_turnaround += turnaround;
    }

    *turnaround_medio = total_turnaround / n;
}

void escrever_estatisticas(int total_tarefas, double turnaround_medio) {
    int fd = open("Estatisticas_Globais.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("Erro ao criar ficheiro de estatísticas");
        return;
    }

    dprintf(fd, "Total Tarefas Executadas: %d\n", total_tarefas);
    dprintf(fd, "Turnaround Time Médio: %.2f segundos\n", turnaround_medio);

    close(fd);
    printf("Estatísticas gravadas em 'Estatisticas_Globais.txt'\n");
}