# Compilador e flags
CC = gcc
CFLAGS = -Wall -g

# Alvo final (executável)
otimizacao1: otimizacao1.o functions.o

# Compilação do ficheiro de funções
functions.o: functions.c functions.h

# Compilação do ficheiro main
main.o: otimizacao1.c functions.h

# Limpeza de objetos e executável
clean:
	rm -f *.o otimizacao1
