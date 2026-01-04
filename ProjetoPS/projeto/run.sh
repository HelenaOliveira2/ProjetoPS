#!/bin/bash

# --- 1. CONFIGURAÇÃO ---
SRC_SCHED_DIR="scheduler"
SRC_PREP_DIR="prepare"
STATS_FILE="Estatisticas_Globais.txt"
TASKS_DIR="tasks_data"

# Nomes dos executáveis que os Makefiles geram
EXE_SCHED="$SRC_SCHED_DIR/scheduler"
EXE_PREP="$SRC_PREP_DIR/prepare"

# Limpar estatísticas antigas para começar do zero
if [ -f "$STATS_FILE" ]; then
    rm "$STATS_FILE"
fi

NUM_TASKS=5              # Número de tarefas que o scheduler vai processar
NUM_PARALELO=2           # Número de processos paralelos
NUM_TAREFAS_A_CRIAR=5    # Número de tarefas a gerar

# --- 2. COMPILAÇÃO ---
echo "==================================="
echo "--- A Compilar ---"
echo "==================================="

echo " -> A compilar pasta: $SRC_SCHED_DIR"
make -C "$SRC_SCHED_DIR"
if [ $? -ne 0 ]; then
    echo "ERRO: Falha no make do Scheduler."
    exit 1
fi

echo " -> A compilar pasta: $SRC_PREP_DIR"
make -C "$SRC_PREP_DIR"
if [ $? -ne 0 ]; then
    echo "ERRO: Falha no make do Prepare."
    exit 1
fi

# --- 4. GERAR TAREFAS ---
echo ""
echo "==================================="
echo "--- A Gerar Tarefas ---"
echo "==================================="

for (( i=1; i<=NUM_TAREFAS_A_CRIAR; i++ ))
do
    SIZE=$(( ( RANDOM % 5 ) + 1 ))
    ./"$EXE_PREP" "$i" "$SIZE"
    echo "   -> Criada Tarefa $i com $SIZE s"
done

echo "Tarefas geradas com sucesso!"

# Verificar se o executável do scheduler existe
if [ ! -f "$EXE_SCHED" ]; then
    echo "ERRO: Executável '$EXE_SCHED' não encontrado."
    exit 1
fi

# --- 5. LOOP INTERATIVO ---
echo ""
echo "==================================="
echo "Pronto para executar os modos!"
echo "==================================="

while true
do
    echo ""
    echo "========================================"
    echo "         MENU DE SELEÇÃO"
    echo "========================================"
    echo "   0 - FCFS Sequencial"
    echo "   1 - FCFS Paralelo (N=$NUM_PARALELO)"
    echo "   2 - SJF Sequencial"
    echo "   3 - SJF Paralelo (N=$NUM_PARALELO)"
    echo "   4 - Ver Estatísticas e SAIR"
    echo "========================================"
    echo ""
    
    read -p "Escolhe o modo (0-4): " MODO
    
    # Validação
    if [[ ! "$MODO" =~ ^[0-4]$ ]]; then
        echo ""
        echo "ERRO: Opção inválida! Escolhe um número entre 0 e 4."
        continue
    fi
    
    # Sair
    if [[ "$MODO" == "4" ]]; then
        echo ""
        echo "A terminar o programa..."
        break
    fi
    
    # Executar o scheduler
    if [[ "$MODO" == "0" || "$MODO" == "2" ]]; then
        ./"$EXE_SCHED" "$NUM_TASKS" "$MODO"
    else
        ./"$EXE_SCHED" "$NUM_TASKS" "$MODO" "$NUM_PARALELO"
    fi
    
    echo ""
    echo "Modo $MODO concluído!"
    echo ""
    read -p "Pressiona ENTER para voltar ao menu..."
    
done

# --- 8. RESULTADOS FINAIS ---
echo ""
echo "========================================"
echo "      ESTATÍSTICAS FINAIS"
echo "========================================"

if [ -f "$STATS_FILE" ]; then
    cat "$STATS_FILE"
else
    echo "Aviso: Ficheiro '$STATS_FILE' não foi criado."
fi
