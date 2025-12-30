CC = gcc
CFLAGS = -Wall -g

# The targets
# Linking the final executable
scheduler: scheduler.o functions.o

# Compiling the header file
functions.o: functions.c functions.h

# Compiling the main file
scheduler.o: scheduler.c functions.h

# Clean object files and executables
clean:
	rm -f *.o scheduler
