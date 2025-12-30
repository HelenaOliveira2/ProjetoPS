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
    double total_turnaround = 0.0;

    // 1. Alocação Dinâmica (Para evitar Stack Overflow)
    int *fds_leitura = malloc(n * sizeof(int));
    pid_t *pids = malloc(n * sizeof(pid_t));
    
    // Alocar matriz de nomes para FIFOs
    char **fifo_names = malloc(n * sizeof(char*));
    for(int i=0; i<n; i++) fifo_names[i] = malloc(64 * sizeof(char));

    if (!fds_leitura || !pids || !fifo_names) {
        perror("Erro de memória");
        exit(1);
    }

    for (int i = 0; i < n; i++) {
        // 2. Definir o nome do FIFO (CORREÇÃO CRÍTICA)
        // Usamos o PID do pai e o índice para garantir unicidade
        snprintf(fifo_names[i], 64, "/tmp/fifo_%d_%d", getpid(), i);

        // 3. Criar o FIFO
        if (mkfifo(fifo_names[i], 0666) < 0) {
            if (errno != EEXIST) {
                perror("Erro ao criar FIFO");
                exit(1);
            }
        }

        // 4. Abrir FIFO no Pai (Non-blocking logic with O_RDWR)
        fds_leitura[i] = open(fifo_names[i], O_RDWR);
        if (fds_leitura[i] < 0) {
            perror("Erro ao abrir FIFO no pai");
            exit(1);
        }

        // 5. Limita paralelismo
        if (processos_ativos >= max_proc) {
            // Espera por QUALQUER filho terminar para libertar uma "vaga"
            wait(NULL); 
            processos_ativos--;
        }

        pid_t pid = fork();

        if (pid < 0) {
            perror("Erro no fork");
            exit(1);
        }

        if (pid == 0) {
            // ===== PROCESSO FILHO =====
            close(fds_leitura[i]); // Fecha leitura herdada

            // Libertar memória do pai no filho (boa prática, opcional aqui mas bom para valgrind)
            free(fds_leitura);
            free(pids);
            // (Nota: o filho tem a sua cópia, não afeta o pai)

            printf("[PID %d] A executar tarefa %d (Duração: %ds)... \n",
                   getpid(), tasks[i].id, tasks[i].duration);

            sleep(tasks[i].duration);

            time_t fim;
            time(&fim);

            double turnaround = difftime(fim, inicio_global);

            // Abre FIFO para escrita
            int fd_escrita = open(fifo_names[i], O_WRONLY);
            if (fd_escrita < 0) {
                perror("Erro filho ao abrir FIFO");
                exit(1);
            }

            if (write(fd_escrita, &turnaround, sizeof(double)) != sizeof(double)) {
                perror("Erro ao escrever no FIFO");
            }

            close(fd_escrita);
            
            // Libertar string do nome no filho
            for(int k=0; k<n; k++) free(fifo_names[k]);
            free(fifo_names);

            printf("[PID %d] Tarefa %d concluída.\n", getpid(), tasks[i].id);
            exit(0);
        }

        // ===== PROCESSO PAI =====
        pids[i] = pid;
        processos_ativos++;
    }

    // 6. Espera final e recolha de resultados
    for (int i = 0; i < n; i++) {
        // Tentamos esperar pelo PID específico.
        // Se o wait(NULL) lá em cima já o tiver apanhado, waitpid retorna -1.
        // Isso não é problema para ler o FIFO, pois os dados estão buffered no kernel.
        waitpid(pids[i], NULL, 0); 

        double t = 0;
        ssize_t bytes = read(fds_leitura[i], &t, sizeof(double));
        
        if (bytes == sizeof(double)) {
            total_turnaround += t;
        } else {
            // Se falhar a leitura, pode ser problemático, mas num lab simples aceita-se
            fprintf(stderr, "Aviso: Não foi possível ler turnaround da tarefa %d\n", tasks[i].id);
        }

        close(fds_leitura[i]);
        unlink(fifo_names[i]); // Apaga o ficheiro
    }

    *turnaround_medio = total_turnaround / n;

    // 7. Limpeza de memória
    free(fds_leitura);
    free(pids);
    for(int i=0; i<n; i++) free(fifo_names[i]);
    free(fifo_names);
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