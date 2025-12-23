CC = gcc
CFLAGS = -Wall -g

# The targets
# Linking the final executable
prepare: prepare.o functions.o

# Compiling the header file
functions.o: functions.c functions.h

# Compiling the main file
prepare.o: prepare.c functions.h

# Clean object files and executables
clean:
	rm -f *.o prepare
	rm -rf ../tasks_data/*.bin