#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
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
            printf("\n%s: Error: Invalid Argument or Arguments missing. Use -h to see usage.", argv[0]);
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
         
         int exitstatus;

         waitpid(forkdata, &exitstatus, 0);

         if( WIFEXITED(exitstatus) )
         {
            if (WEXITSTATUS(exitstatus) == 42)
            {
               printf("\n%s: Parent: Error: Abort on iter# %i\n", argv[0], itercounter);
               return;
            }
         }

         printf("\n%s: Parent: Fork End: %i\n", argv[0], forkdata);
      }
      else
      {
         int count;

         fscanf(input, "%i", &count); 

         /*FILE* trackread = fopen("tracker", "r");

         if(trackread != NULL)
         {
            int posval = 0;
            fscanf("%i", &posval);
            
            if(posval > 0)
               fseek(input, posval, SEEK_SET);
         }*/

         struct Stack* stack = StackInit(count);

         int i;
         int tmp;
         char line[1000];

         fgetc(input);
         fgets(line, 1000, input);

         char* value = strtok(line, " ");
         
         while(value != NULL)
         {
            if(!IsFull(stack))
            {
               Push(stack, atoi(value));
            }
            else
            {
               printf("\n%s: Error: Input Line Data and Count Mismatch.\n", argv[0]);
               exit(42);
            }
            value = strtok(NULL, " ");
         }

         fprintf(output, "%i: ", getpid());
         for(i = 0; i < count; i++)
         {  
            fprintf(output, "%i ", Pop(stack));
         }
         
         FILE* tracker = fopen("tracker", "wr");
         
         /*long test = ftell(tracker);
         printf("VALUE : : %l", test);

         fprintf("%i", ftell(tracker));
         fclose(tracker);
*/
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
