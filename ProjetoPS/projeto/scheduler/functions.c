#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include "functions.h"
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h> // Necessário para wait()


int ler_tarefa_binario(int id, Task *t) {
    char nome_ficheiro[100];
    snprintf(nome_ficheiro, sizeof(nome_ficheiro),  "../tasks_data/task_%d.bin", id);

    int fd = open(nome_ficheiro, O_RDONLY);
    if (fd < 0) {
        perror("Erro ao abrir ficheiro da tarefa!");
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
    double total_turnaround = 0;
    time_t inicio_global;
    time(&inicio_global);

    for (int i = 0; i < n; i++) {
        printf("A executar tarefa %d (Duração: %ds)...\n", tasks[i].id, tasks[i].duration);

        sleep(tasks[i].duration);  // simula execução
        time_t fim;
        time(&fim);

        double turnaround = difftime(fim, inicio_global);
        printf("Tarefa %d concluída. Turnaround: %.2f s\n",tasks[i].id, turnaround);
        total_turnaround += turnaround;
    }

    *turnaround_medio = total_turnaround / n;
}

void executar_paralelo(Task tasks[], int n, int max_proc, double *turnaround_medio) {
    time_t inicio_global;
    time(&inicio_global);

    int processos_ativos = 0;
    double total_turnaround = 0;

    int pipes[n][2];  // um pipe por tarefa

    for (int i = 0; i < n; i++) {
        if (pipe(pipes[i]) < 0) {
            perror("Erro ao criar pipe");
            exit(1);
        }

        if (processos_ativos >= max_proc) {
            wait(NULL);
            processos_ativos--;
        }

        pid_t pid = fork();

        if (pid < 0) {
            perror("Erro ao criar fork");
            exit(1);
        }

        if (pid == 0) {
            // === FILHO ===
            close(pipes[i][0]); // fecha leitura

            printf("[Filho %d] A executar tarefa %d (%ds)\n",
                   getpid(), tasks[i].id, tasks[i].duration);

            sleep(tasks[i].duration);

            time_t fim;
            time(&fim);

            double turnaround = difftime(fim, inicio_global);

            // Envia o turnaround ao pai
            write(pipes[i][1], &turnaround, sizeof(double));
            close(pipes[i][1]);

            exit(0);
        } else {
            // === PAI ===
            close(pipes[i][1]); // fecha escrita
            processos_ativos++;
        }
    }

    // Esperar por todos os filhos
    while (processos_ativos > 0) {
        wait(NULL);
        processos_ativos--;
    }

    // Ler todos os turnarounds
    for (int i = 0; i < n; i++) {
        double t;
        read(pipes[i][0], &t, sizeof(double));
        close(pipes[i][0]);
        total_turnaround += t;
    }

    *turnaround_medio = total_turnaround / n;

    // os pipes são para se calcular bem o turnaround time, são tipo os fifos que o stor não chagou a dar, sem eles o turnaround está mal calculado
}  // mudar para fifos

// Função de comparação para o qsort ordenar por duração crescente
int comparar_tarefas(const void *a, const void *b) {
    Task *t1 = (Task *)a;
    Task *t2 = (Task *)b;
    return (t1->duration - t2->duration);
}

void ordenar_sjf(Task tasks[], int n) {
    qsort(tasks, n, sizeof(Task), comparar_tarefas);
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