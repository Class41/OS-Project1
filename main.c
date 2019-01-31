#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "stack.h"

int main(int argc, char** argv)
{
	int optionItem;

	char* inputName = malloc(50 * sizeof(char) + 1); //store filenames
	char* outputName = malloc(50 * sizeof(char) + 1);

	strcpy(inputName, "input.dat"); //put initial strings into the malloc
	strcpy(outputName, "output.dat");
	while ((optionItem = getopt(argc, argv, "hi:o:")) != -1) //read option list
	{
		switch (optionItem)
		{
		case 'h': //show help menu
			printf("\t%s Help Menu\n\
            \t-h : show help dialog \n\
            \t-i [filename] : input filename. default: input.dat\n\
            \t-o [filename] : output filename. default: output.dat\n", argv[0]);
			return;
		case 'i': //change input 
			strcpy(inputName, optarg);
			printf("\n%s: Info: set input file to: %s", argv[0], optarg);
			break;
		case 'o': //change output
			strcpy(outputName, optarg);
			printf("\n%s: Info: set output file to: %s", argv[0], optarg);
			break;
		case '?': //an error has occoured reading arguments
			printf("\n%s: Error: Invalid Argument or Arguments missing. Use -h to see usage.", argv[0]);
			return;
		}
	}

	printf("\n%s: Info: input file: %s, output file: %s", argv[0], inputName, outputName);

	FILE* input = fopen(inputName, "r"); //open input/output files specified
	FILE* output = fopen(outputName, "wr");

	if (input == NULL) //check if the input file exists
	{
		printf("\n%s: ", argv[0]);
		fflush(stdout);
		perror("Error: Failed to open input file");
		return;
	}

	int iterations; //how many sets of data there are
	fscanf(input, "%i", &iterations);

	printf("\n%s: Info: Performing %i operations\n", argv[0], iterations);

	int itercounter; //keep track of current iteration out of total
	int* forks = calloc(3, sizeof(int)); //store child PIDs
	for (itercounter = 0; itercounter < iterations; itercounter++)
	{
		int forkdata = fork(); //create child fork

      if(forkdata == -1) //fork creation failed
      {
         printf("\n%s: ", argv[0]);
		   fflush(stdout);
		   perror("Error: Failed to fork: ");
         exit(1);
      }

		if (forkdata > 0) //If parent
		{
			printf("\n%s: Parent: Fork Start: %i", argv[0], forkdata);
			forks[itercounter] = forkdata; //store child PID

			int exitstatus; //store child exit

			waitpid(forkdata, &exitstatus, 0);

			if (WIFEXITED(exitstatus))
			{
				if (WEXITSTATUS(exitstatus) == 42) //child exited with an error
				{
					printf("\n%s: Parent: Error: Abort on iter# %i\n", argv[0], itercounter);
					return;
				}
			}

			printf("\n%s: Parent: Fork End: %i\n", argv[0], forkdata);
		}
		else //if child
		{
			FILE* trackread = fopen(".filetrack", "r"); //temp file pos tracker

			if (trackread != NULL) //if a position exists to start from
			{
				int posval;
				fscanf(trackread, "%i", &posval);

				if (posval > 0) //set our current file pos to the offset from start
					fseek(input, posval, SEEK_SET);
			}

			int count; //store how many characters to scan for on the next line
			fscanf(input, "%i", &count);

			struct Stack* stack = StackInit(count); //stack instance

			int i; //generic iterator
			int tmp; //stores data temporarily (deprecated) 
			char line[1000]; //temp storage for line read

			fgetc(input); //eat newline 
			fgets(line, 1000, input); //get line of characters from file

			char* value = strtok(line, " "); // split numbers by spaces

			while (value != NULL) //check if token exists
			{
				if (!IsFull(stack)) //make sure the amount provided matches actual
				{
					Push(stack, atoi(value));
				}
				else
				{
					printf("\n%s: Error: Input Line Data and Count Mismatch.\n", argv[0]);
					exit(42); //exit with error
				}
				value = strtok(NULL, " "); //get next token
			}

         if(HasExpectedCount(stack, count) != 1)
         {
            printf("\n%s: Error: Input Line Data and Count Mismatch.\n", argv[0]);
            exit(42);
         }

			fprintf(output, "%i: ", getpid()); //write PID to file
			for (i = 0; i < count; i++)
			{
				fprintf(output, "%i ", Pop(stack)); //write values in reverse
			}

			FILE* filetrack = fopen(".filetrack", "wr"); //write current pos to file

			fprintf(filetrack, "%i", ftell(input));
			printf("%l", ftell(input));
			fclose(filetrack);

			fprintf(output, "\n");
			exit(0);
		}
	}

	fprintf(output, "All children were: "); //write children
	for (itercounter = 0; itercounter < iterations; itercounter++)
	{
		fprintf(output, "%i ", forks[itercounter]); //write child PID
	}
   fprintf(output, "\nSelf: %i", getpid()); //write own PID

	remove(".filetrack"); //delete temp file
	fclose(output); //close I/O
   fclose(input);
   free(forks); //prevent memleaks
   free(inputName);
   free(outputName);
	return 0;
}
