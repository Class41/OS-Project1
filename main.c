#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"

int main(int argc, char** argv)
{
   int item;

   char* inputname  = malloc(50 * sizeof(char) + 1);
   char* outputname = malloc(50 * sizeof(char) + 1); 

   strcpy(inputname, "input.dat");
   strcpy(outputname, "output.dat");
   while((item = getopt(argc, argv, "hi:o:")) != -1)
   {
      switch(item)
      {
         case 'h':
            printf("\t%s Help Menu\n\
            \t-h : show help dialog \n\
            \t-i [filename] : input filename. default: input.dat\n\
            \t-o [filename] : output filename. default: output.dat\n", argv[0]);
            return;
         case 'i':
            strcpy(inputname, optarg);
            printf("\n%s: Info: set input file to: %s", argv[0], optarg);
            break;
         case 'o':
            strcpy(outputname, optarg);
            printf("\n%s: Info: set output file to: %s", argv[0], optarg);
            break;
         case '?':
            printf("\n%s: Error: Arguments missing. Use -h to see usage.", argv[0]);
            return;
      }
   }

   printf("\n%s: Info: input file: %s, output file: %s", argv[0] ,inputname, outputname);

   FILE* input = fopen(inputname, "r");
   FILE* output = fopen(outputname, "wr");

   if(input == NULL)
   {
       printf("\n%s: ", argv[0]);
       fflush(stdout);
       perror("Error: Failed to open input file");
       return;
   }

   int iterations; //how many sets of data there are
   fscanf(input, "%i", &iterations); 

   printf("\n%s: Info: Performing %i operations\n", argv[0], iterations);

   int itercounter;
   int* forks = calloc(3, sizeof(int));
   for(itercounter = 0; itercounter < iterations; itercounter++)
   {
      int forkdata = fork();

      if(forkdata > 0)
      {
         printf("\n%s: Parent: Fork Start: %i", argv[0], forkdata);
         forks[itercounter] = forkdata;
         wait();
         printf("\n%s: Parent: Fork End: %i\n", argv[0], forkdata);
      }
      else
      {
         int count;
         fscanf(input, "%i", &count); 
      
         struct Stack* stack = StackInit(count);

         int tmp;
         int i;
         for(i = 0; i < count; i++)
         {
            fscanf(input, "%i", &tmp);
            Push(stack, tmp);
         }

         fprintf(output, "%i: ", getpid());
         for(i = 0; i < count; i++)
         {  
            fprintf(output, "%i ", Pop(stack));
         }  

         fprintf(output, "\n");
         exit(0);
      }
   }
   
   fprintf(output, "All children were: ");
   for(itercounter = 0; itercounter < iterations; itercounter++)
   {
      fprintf(output,"%i ", forks[itercounter]);
   }

   fclose(output);
   return 0;
}
