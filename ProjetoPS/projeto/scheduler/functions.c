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

// Temos de procurar qual era a tarefa associada a esse PID.
int encontrar_id_tarefa(pid_t pid, pid_t pids[], Task tasks[], int n) {
    for (int i = 0; i < n; i++) {
        if (pids[i] == pid) {
            return tasks[i].id;
        }
    }
    return -1; // Não encontrado
}

int ler_tarefa_binario(int id, Task *t) {
    char nome_ficheiro[100];
    snprintf(nome_ficheiro, sizeof(nome_ficheiro),  "../tasks_data/task_%d.bin", id);

    int fd = open(nome_ficheiro, O_RDONLY);
    if (fd < 0) {
        perror("Erro ao abrir ficheiro da tarefa! ");
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
    double total_turnaround = 0.0;
    time_t inicio_global = time(NULL);

    for (int i = 0; i < n; i++) {
        printf("A executar tarefa %d (Duração: %ds)...\n", tasks[i].id, tasks[i].duration);

        sleep(tasks[i].duration);  // simula execução
        time_t fim = time(NULL);

        double turnaround = difftime(fim, inicio_global);
        printf("Tarefa %d concluída. Turnaround: %.2f s\n",tasks[i].id, turnaround);
        total_turnaround += turnaround;
    }

    *turnaround_medio = total_turnaround / n;
}

void executar_paralelo(Task tasks[], int n, int max_proc, double *turnaround_medio) {

    time_t inicio_global = time(NULL);
    int processos_ativos = 0;
    double total_turnaround = 0.0;

    pid_t *pids = malloc(n * sizeof(pid_t));
    if (!pids) {
        perror("Erro de memória");
        exit(1);
    }

    int i = 0;

    while (i < n || processos_ativos > 0) {

        // Criar novo processo se possível
        if (i < n && processos_ativos < max_proc) {

            pid_t pid = fork();
            if (pid < 0) {
                perror("Erro no fork");
                exit(1);
            }

            if (pid == 0) {
                free(pids);
                printf("A executar tarefa %d (Duração: %ds)...\n", tasks[i].id, tasks[i].duration);
                sleep(tasks[i].duration);
                exit(0);
            }

            // Processo pai
            pids[i] = pid;
            processos_ativos++;
            i++;
        }
        // Caso contrário, esperar por um filho
        else {

            pid_t pid_concluido = wait(NULL);
            time_t fim = time(NULL);

            double ta = difftime(fim, inicio_global);
            total_turnaround += ta;
            processos_ativos--;

            int id_task = encontrar_id_tarefa(pid_concluido, pids, tasks, n);
            printf("[Pai] Tarefa %d (PID %d) terminou. Turnaround: %.2f s\n",
                   id_task, pid_concluido, ta);
        }
    }

    *turnaround_medio = total_turnaround / n;
    free(pids);
}


// Função de comparação para o qsort ordenar por duração crescente
int comparar_tarefas(const void *a, const void *b) {
    Task *t1 = (Task *)a;
    Task *t2 = (Task *)b;
    return (t1->duration - t2->duration);
}

void ordenar_sjf(Task tasks[], int n) {
    qsort(tasks, n, sizeof(Task), comparar_tarefas);
}


void escrever_estatisticas(int total_tarefas, double turnaround_medio, int modo) {
    int fd = open("Estatisticas_Globais.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) {
        perror("Erro ao criar ficheiro de estatísticas");
        return;
    }
    if (modo == 0) {
        dprintf(fd, "--- Em execução FCFS sequencial ---\n");
    } else if (modo == 1) {
        dprintf(fd, "--- Em execução FCFS paralelo ---\n");
    } else if (modo == 2) {
        dprintf(fd, "--- Em execução SJF sequencial ---\n");
    }else if (modo == 3) {
        dprintf(fd, "--- Em execução SJF paralelo ---\n");
    }
    dprintf(fd, "Total Tarefas Executadas: %d\n", total_tarefas);
    dprintf(fd, "Turnaround Time Médio: %.2f segundos\n", turnaround_medio);

    close(fd);
    printf("Estatísticas gravadas em 'Estatisticas_Globais.txt'\n");
}