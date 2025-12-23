#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    int duration;
    // outros campos que possas ter
} Task;

int ler_tarefa_binario(int id, Task *t) {
    char nome_ficheiro[100];
    snprintf(nome_ficheiro, sizeof(nome_ficheiro), "../tasks_data/task_%d.bin", id);

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

// Executa uma única tarefa (chamado pelo processo filho)
void executar_tarefa(Task *t, time_t inicio_global) {
    printf("[PID %d] A executar tarefa %d (Duração: %ds)...\n", 
           getpid(), t->id, t->duration);
    
    sleep(t->duration);
    
    time_t fim;
    time(&fim);
    
    double turnaround = difftime(fim, inicio_global);
    
    printf("[PID %d] Tarefa %d concluída. Turnaround: %.2f s\n", 
           getpid(), t->id, turnaround);
}

void executar_paralelo(Task tasks[], int total_tarefas, int N, double *turnaround_medio) {
    time_t inicio_global;
    time(&inicio_global);
    
    int tarefas_executadas = 0;
    int tarefas_em_execucao = 0;
    pid_t pids[N];  // Array para guardar PIDs dos processos ativos
    
    printf("=== Executando até %d tarefas em paralelo ===\n", N);
    
    while (tarefas_executadas < total_tarefas || tarefas_em_execucao > 0) {
        
        // Lança novas tarefas até atingir o limite N
        while (tarefas_em_execucao < N && tarefas_executadas < total_tarefas) {
            pid_t pid = fork();
            
            if (pid < 0) {
                perror("Erro ao criar processo filho");
                exit(1);
            }
            
            if (pid == 0) {
                // Processo filho - executa a tarefa
                executar_tarefa(&tasks[tarefas_executadas], inicio_global);
                exit(0);  // Filho termina após executar
            }
            
            // Processo pai - guarda o PID
            pids[tarefas_em_execucao] = pid;
            tarefas_em_execucao++;
            tarefas_executadas++;
        }
        
        // Espera que pelo menos um processo filho termine
        int status;
        pid_t pid_terminado = wait(&status);
        
        if (pid_terminado > 0) {
            // Remove o PID terminado do array
            for (int i = 0; i < tarefas_em_execucao; i++) {
                if (pids[i] == pid_terminado) {
                    // Move os PIDs restantes
                    for (int j = i; j < tarefas_em_execucao - 1; j++) {
                        pids[j] = pids[j + 1];
                    }
                    tarefas_em_execucao--;
                    break;
                }
            }
        }
    }
    
    // Calcula turnaround médio (tempo total / número de tarefas)
    time_t fim_global;
    time(&fim_global);
    
    double tempo_total = difftime(fim_global, inicio_global);
    
    *turnaround_medio = tempo_total;  // ou outra métrica que precises
}

void escrever_estatisticas(int total_tarefas, double turnaround_medio) {
    FILE *f = fopen("Estatisticas_Globais.txt", "w");
    if (f == NULL) {
        perror("Erro ao criar ficheiro de estatísticas");
        return;
    }

    fprintf(f, "Total Tarefas Executadas: %d\n", total_tarefas);
    fprintf(f, "Turnaround Time Médio: %.2f segundos\n", turnaround_medio);

    fclose(f);
    printf("\nEstatísticas gravadas em 'Estatisticas_Globais.txt'\n");
}

