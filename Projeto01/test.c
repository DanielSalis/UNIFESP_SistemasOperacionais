#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_LENGTH 500
#define EXIT_WITH_SUCESS 0
#define EXIT_WITH_FAILURE 1
#define EXECUTION_STANDART 1
#define EXECUTION_PIPE 2
#define PIPE 1


void run_shell(){
    char *input;
    char *args[MAX_LENGTH];
    char *multiple_args[MAX_LENGTH];
    int status = 1;
    int type_of_execution = 0;

    while (status) {
        printf("\nshell>");
        
        input = read_commands();
        
        type_of_execution = process_argumets(input, args, multiple_args);

        execute_commands(type_of_execution, args, multiple_args);

        free(input);
    }
}

int main(int argc, char **argv) {
  run_shell();
  return EXIT_SUCCESS;
}