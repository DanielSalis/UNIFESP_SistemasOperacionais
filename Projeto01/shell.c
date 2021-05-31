#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

#define EXIT_WITH_SUCESS 0
#define EXIT_WITH_FAILURE 1
#define OPERATOR_PIPE 1
#define OPERATOR_SEMICOLON 2

int run_in_background = 0;

char get_operator_type(int type){
    if(type == OPERATOR_PIPE){
        return '|';
    }else if(type == OPERATOR_SEMICOLON){
        return ';';
    }
}

int get_operators(int type, char **argv){
    char character = get_operator_type(type);
    int count = 0;
    int i = 0;

    for(i; argv[i]; i++){
        if (*argv[i] == character){
            count++;
        }
    }
    return count;
}

int find_operator_by_position(int type, int i, char **argv){
    char character = get_operator_type(type);
    for (i;argv[i];i++){
        if (*argv[i] == character){
            return i;
        }
    }
    return -1;
}

int exec_default(char **args){
    int pid;
    pid = fork();
    if (pid == 0){
        if (execvp(args[0], args) < 0){
            printf("dentro do if, erno = %d\n",errno);
            perror("execvp");
            return -1;
        }
    }
    else if (pid > 0){
        if(!run_in_background){
            waitpid(pid, NULL, 0);
        }
    }
    return 1;
}

int exec_pipes(char **argv, int number_of_pipes){
    int fd[2]; 
    int i = 0; 
    int caracter_position;
    int auxiliar_fd = STDIN_FILENO;

    for (int j = 0; j <= number_of_pipes; j++){
        caracter_position = find_operator_by_position(OPERATOR_PIPE, i, argv);
        char **current_command = &argv[i];
        
        if (caracter_position != -1){
            current_command[caracter_position - i] = NULL;
        }

        if (pipe(fd) < 0){
            perror("pipe");
            return -1;
        }

        int pid_processo_filho = fork();
        if (pid_processo_filho == 0){
            close(fd[0]);
            dup2(auxiliar_fd, STDIN_FILENO);

            if (j < number_of_pipes)
                dup2(fd[1], STDOUT_FILENO); 

            if (execvp(current_command[0], current_command) < 0){
                perror("execvp pipe filho");
                return -1;
            }
        }else if (pid_processo_filho > 0){
            auxiliar_fd = fd[0];
            close(fd[1]);
            waitpid(pid_processo_filho, NULL, 0);
        }else{
            return -1;
        }
        i = caracter_position + 1;
    }
    return 1;
}

int exec_semmicolons(char **args, int number_of_semicolons){
    int counter = 0;
    int position;
    for (int i = 0; i <= number_of_semicolons; i++) {
        printf("\n");
        position = find_operator_by_position(OPERATOR_SEMICOLON, counter, args);
        char **current_command = &args[counter];
        
        current_command[position-counter] = NULL;
        
        exec_default(current_command);
        
        counter = position+1;
    }
}

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
    if(line[strlen(line)-1]=='&'){
        run_in_background = 1;
        line[strlen(line)-1]='\0';
        if(line[strlen(line)-1]==' '){
            line[strlen(line)-1]='\0';
        }
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

    if(count_args > 0){
        if(strcmp(args[0], "")==0){
            for(i=0; i < count_args; i++){
                args[i]=args[i+1];
            }
        }
    }

}

char **process_argumets(char *commands){
    char** args = malloc(strlen(commands)*sizeof(char**));
    parse_spaces(commands,args);
    
    return args;
}

void process_commands(char** args){
    if(run_in_background==1){
        exec_default(args);
    }else{
        int number_of_pipes = get_operators(OPERATOR_PIPE, args);
        if (number_of_pipes > 0){
            exec_pipes(args, number_of_pipes);
        }else{
            int number_of_semicolons = get_operators(OPERATOR_SEMICOLON, args);
            if(number_of_semicolons > 0){
                exec_semmicolons(args, number_of_semicolons);
            }else{
                exec_default(args);
            }
        }
    }
}

void run_shell(){
    int status = 1;

    while (status) {
        char *input;
        printf("\033[0;31m");
        printf("\n%s@shell>", getenv("USER"));
        printf("\033[0m");

        input = read_commands();
        
        char **args = process_argumets(input);

        process_commands(args);

        free(input);
    }
}

int main(int argc, char **argv){
    system("clear");
    run_shell();
    return EXIT_WITH_SUCESS;
}