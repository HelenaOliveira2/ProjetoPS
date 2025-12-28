#!/bin/bash
# Limpar estatísticas antigas para começar do zero
if [ -f "Estatisticas_Globais.txt" ]; then
    rm "Estatisticas_Globais.txt"
fi

# --- 1. CONFIGURAÇÃO ---

# O Scheduler está dentro da pasta "scheduler"
SRC_SCHED_DIR="scheduler"
EXE_SCHED="scheduler_exec"

# O Prepare está dentro da pasta "prepare"
SRC_PREP_DIR="prepare"
EXE_PREP="prepare_exec"

INPUT_FILE="function.txt"
MAX_PROC=4
NUM_TAREFAS_A_CRIAR=5
NUM_PARALELO = 2

# --- 2. COMPILAÇÃO ---
echo "--- A Compilar ---"

# 2.1 Compilar o Scheduler (Incluindo o functions.c da pasta scheduler)
if [ -d "$SRC_SCHED_DIR" ]; then
    # Compila todos os .c dentro da pasta scheduler (*.c)
    gcc -Wall "$SRC_SCHED_DIR"/*.c -o "$EXE_SCHED"
    
    if [ $? -ne 0 ]; then
        echo "Erro na compilação do Scheduler!"
        exit 1
    fi
    echo "Scheduler compilado."
else
    echo "Erro: Pasta $SRC_SCHED_DIR não encontrada!"
    exit 1
fi

# 2.2 Compilar o Prepare (Incluindo o functions.c da pasta prepare)
if [ -d "$SRC_PREP_DIR" ]; then
    # Compila todos os .c dentro da pasta prepare
    gcc -Wall "$SRC_PREP_DIR"/*.c -o "$EXE_PREP"
    
    if [ $? -ne 0 ]; then
        echo "Erro na compilação do Prepare!"
        exit 1
    fi
    echo "Prepare compilado."
else
    echo "Aviso: Pasta $SRC_PREP_DIR não encontrada."
    exit 1
fi

# --- 3. GERAR TAREFAS ---
echo "A gerar $NUM_TAREFAS_A_CRIAR tarefas com o '$EXE_PREP'..."

# Limpa/Cria o ficheiro de input vazio
> "$INPUT_FILE"

# Loop de 1 até NUM_TAREFAS_A_CRIAR
for (( i=1; i<=NUM_TAREFAS_A_CRIAR; i++ ))
do
    # Gera um número aleatório entre 1 e 5 para o "num_task" (duração/tamanho)
    SIZE=$(( ( RANDOM % 5 ) + 1 ))

    # 1. Executa o prepare para criar o ficheiro físico da tarefa
    # Argumentos: ID (i) e NUM_TASK (SIZE)
    ./"$EXE_PREP" "$i" "$SIZE"

    # 2. Adiciona a linha ao ficheiro function.txt para o scheduler ler
    # Formato: ID DURAÇÃO
    echo "$i $SIZE" >> "$INPUT_FILE"
    
    echo "   -> Criada Tarefa $i com  $SIZE s"
done

echo "----------------------------------------"
echo "A INICIAR TESTES (Modos 0 a 3)"
echo "----------------------------------------"

# Verifica se o executável existe
if [ ! -f "./$EXE_SCHED" ]; then
    echo "Erro: Executável não encontrado."
    exit 1
fi

# Ciclo que vai de 0 a 3
for MODO in {0..3}
do
    echo ""
    echo "A executar MODO $MODO..."
    
    # Lógica para decidir os argumentos
    if [[ "$MODO" == "0" || "$MODO" == "2" ]]; then
        # Modos Sequenciais (FCFS Seq / SJF Seq)
        ./"$EXE_SCHED" "$MAX_PROC" "$MODO"
        
    else
        # Modos Paralelos (FCFS Par / SJF Par)
        # Passamos o MAX_PROC também como o "Num Paralelo"
        ./"$EXE_SCHED" "$MAX_PROC" "$MODO" "$MAX_PROC"
    fi
    
done

echo "----------------------------------------"
echo "Execução terminada."

# --- 6. RESULTADOS ---
if [ -f "Estatisticas_Globais.txt" ]; then
    echo "Últimos registos em Estatisticas_Globais.txt:"
    cat "Estatisticas_Globais.txt"
fi