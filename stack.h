#include <stdlib.h>

struct Stack {
	int head;
	int capacity;
	int* data;
};

struct Stack* StackInit(int);
void Push(struct Stack*, int);
int Pop(struct Stack*);
int IsFull(struct Stack*);
int IsEmpty(struct Stack*);
