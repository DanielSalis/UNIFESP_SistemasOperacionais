#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
    return line;
}

void parse_spaces(char *commands, char **args){
    int i = 0;

    long int size_of_commands = strlen(commands)-1;
    
    commands[strlen(commands)-1]='\0';

    for(i; i < size_of_commands; i++){
        args[i] = strsep(&commands, " ");
        if(args[i] == NULL) 
            break;
    }
}

int process_argumets(char *commands, char **args){
    parse_spaces(commands,args);
    return EXECUTION_STANDART;
}

void execute_normal(char **args){
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

void execute_commands(int type, char **args){
    if(type == EXECUTION_STANDART){
        execute_normal(args);
    }else if(type == EXECUTION_PIPE){
        printf("AINDA NÃO ESTA PRONTO\n");
    }
}

void run_shell(){
    char *commands;
    char **args;
    int status;
    int type_of_execution = 0;

    while (status) {
        printf("\nshell>");
        
        //Função que lê a informação de cada linha
        commands = read_commands();
        
        //Função que faz o split dos argumentos caso tenha espaço ou pipe
        char** args = malloc(strlen(commands) * sizeof(char**));
        type_of_execution = process_argumets(commands, args);

        //Função que executa os comandos
        execute_commands(type_of_execution, args);

        free(commands);
        free(args);
    }
}

int main(int argc, char **argv) {
  run_shell();
  return EXIT_SUCCESS;
}