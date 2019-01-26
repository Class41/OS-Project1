CC = gcc
CFLAGS = -g
TARGET = Parser
OBJS = main.o stack.o

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)
main.o: main.c
	$(CC) $(CFLAGS) -c main.c
stack.o: stack.c stack.h
	$(CC) $(CFLAGS) -c stack.c
clean:
	/bin/rm -f *.o $(TARGET)
