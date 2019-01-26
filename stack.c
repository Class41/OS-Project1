#include "stack.h"

struct Stack* StackInit(int size)
{
   struct Stack* StackObj = (struct Stack*)calloc(1, sizeof(struct Stack));
   StackObj->capacity = size; 
   StackObj->head = -1; //intialize the head of the stack to be out of bounds of array
   StackObj->data = (int*)calloc(size, sizeof(int));
   return StackObj;
}

void Push(struct Stack* stack, int value)
{
   if(IsFull(stack) == 1)
   {
      return;
   }

  stack->data[++stack->head] = value;
}

int Pop(struct Stack* stack)
{
   if(IsEmpty(stack) == 1)
   {
      return NULL;
   }

   return stack->data[stack->head--];
}

int IsFull(struct Stack* stack)
{
   if(stack->head >= stack->capacity - 1)
      return 1;
   else
      return 0;
}

int IsEmpty(struct Stack* stack)
{
   if(stack->head < 0)
      return 1;
   else
      return 0;

}
