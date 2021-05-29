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

char *read_commands(void){
    char *line = NULL;
    ssize_t bufsize = 0;

    if (getline(&line, &bufsize, stdin) == -1){
        if (feof(stdin)) {
            exit(EXIT_WITH_SUCESS);
        } else  {
            perror("readline");
            exit(EXIT_WITH_FAILURE);
        }
    }

    if(line[strlen(line)-1]=='\n'){
        line[strlen(line)-1]='\0';
    }
    if(line[strlen(line)-1]==' '){
        line[strlen(line)-1]='\0';
    }

    return line;
}

void parse_spaces(char *commands, char *args[]){
    int i = 0;
    int count_args=0;
    long int size_of_commands = strlen(commands);
    char *commands_aux = malloc(size_of_commands*sizeof(char*));
    memcpy(commands_aux, commands, strlen(commands));

    for(i; i < size_of_commands; i++){
        char *aux = strsep(&commands_aux, " ");
        args[i]=aux;
        count_args++;
        if(args[i] == NULL) 
            break;
    }

    if(strcmp(args[0], "")==0){
        for(i=0; i < count_args; i++){
            args[i]=args[i+1];
        }
    }

}

int parse_pipes(char* commands, char *multiple_args[], int number_of_pipes){
    int i=0;
    
    char *commands_aux = malloc(strlen(commands)*sizeof(char*));
    memcpy(commands_aux, commands, strlen(commands)-1);

    for(i=0;i<number_of_pipes+1; i++){
        multiple_args[i] = strsep(&commands_aux, "|");
        if(multiple_args[i] == NULL) 
            break;
    }
}

int verify_number_of_pipes(char* commands){
    long int size_of_commands = strlen(commands)-1;
    int i=0;
    int count=0;
    
    for(i=0;i<size_of_commands; i ++){
        if(commands[i]=='|'){
            count++;
        }
    }

    return count;
}

int process_argumets(char *commands, char *args[], char *multiple_args[]){
    int number_of_pipes= verify_number_of_pipes(commands);
    if(number_of_pipes > 0){
        parse_pipes(commands, multiple_args, number_of_pipes);
        return EXECUTION_PIPE;
    }else{
        parse_spaces(commands,args);
        return EXECUTION_STANDART;
    }
}

void execute_normal(char *args[]){
    int pid = fork();

    if(pid == -1){
        printf("\nFALHOU!!\n");
    }else if(pid == 0){
        int exec_value = execvp(args[0], args);
        if(exec_value < 0){
            printf("\nComando não existe\n");
        }
        exit(EXIT_WITH_SUCESS);
    }else{
        wait(NULL);
        return;
    }
}

void execute_pipe(char** multiple_args){
    printf("AINDA NÃO ESTA PRONTO\n");
}

void execute_commands(int type, char *args[], char *multiple_args[]){
    if(type == EXECUTION_STANDART){
        execute_normal(args);
    }else if(type == EXECUTION_PIPE){
        execute_pipe(multiple_args);
    }
}

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