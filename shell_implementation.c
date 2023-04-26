#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define TRUE 1
#define FALSE 0

#define OP_PIPE 0
#define OP_BACKGROUND 1
#define OP_OR 2
#define OP_AND 3
#define OP_INPUT 4
#define OP_OUTPUT 5
#define OP_APPEND_OUTPUT 6

#define MAX_STRING_LENGTH 1024

typedef struct {
    int fd[2], full;
} pipe_t;

int contador_pipes(char **argv);
int encontra_operador(char **argv, int *op);
int encontra_pos_operador(char **argv, char *token);
int trata_operador_or(int pos, char **argv);
void imprime_argv(char **argv);


int main(int argc, char **argv) {
    if (argc <= 1) {
        printf("Uso: ./trabalho-shell <comando> <parametros> <operador> <comando>...\n");
        return 0;
    }
    
    char **cmd;
    cmd = &argv[1];

    FILE *input_file, *output_file;
    char filename[MAX_STRING_LENGTH]; // buffer para armazenar o nome do arquivo
    char output[MAX_STRING_LENGTH]; //****

    pid_t p_id;

    int i, qtd_pipes = contador_pipes(argv);

    pipe_t* pipeline = (pipe_t *) malloc(qtd_pipes * sizeof(pipe_t));
    for (i = 0; i < qtd_pipes; i++) {
        pipe(pipeline[i].fd);
        pipeline[i].full = FALSE;
    }

    int pp = 0; // posição do pipe

    int op, posicao_op; // operador e posição do operador
    int status;
    int verifica_or;
    // verifica_or será exclusivamente usado para verificar se o próximo operador após um or é um pipe "|"

    while (cmd) {
        posicao_op = encontra_operador(cmd, &op);
        // primeiro vamos verificar se trata de um operador OR, se sim, vamos
        // verificar se sua saída deve ser redirecionada a um pipe.
        if (op == OP_OR) // é "||" ?
            verifica_or = trata_operador_or(posicao_op, cmd); // verifica se o próximo operador é um pipe

        
        if (posicao_op != -1) cmd[posicao_op] = NULL;

        p_id = fork();
        if (p_id == 0) { // filho
            if (pipeline[pp - 1].full == TRUE && pp > 0)
                dup2(pipeline[pp - 1].fd[0], STDIN_FILENO);

            // \/ se o operador for um pipe "|" ou um OR "||" que tem como operador seguinte um pipe "|"
            if (op == OP_PIPE || verifica_or) {
                dup2(pipeline[pp].fd[1], STDOUT_FILENO);
            }
            if (op == OP_INPUT) { // operador "<" redirecionamento de entrada padrão
                // o nome do arquivo está após o operador
                strcpy(filename, cmd[posicao_op + 1]);

                input_file = fopen(filename, "r");
                if (input_file == NULL) {
                    perror("Erro ao abrir arquivo de entrada.");
                    exit(EXIT_FAILURE);
                }
                // redireciona a entrada padrão
                dup2(fileno(input_file), STDIN_FILENO);
                // remover operador e nome do arquivo da lista de argumentos
                cmd[posicao_op] = NULL;
                cmd[posicao_op + 1] = NULL;
            }

            if (op == OP_OUTPUT || op == OP_APPEND_OUTPUT) { // operador ">" redirecionamento de saída padrão e ">>" append + redirecionamento de saída padrão
                 // o nome do arquivo está após o operador
                strcpy(filename, cmd[posicao_op + 1]);

                // se o comando for ">" então sobrescreve, se for ">>" ele concatena
                output_file = op == OP_OUTPUT ? fopen(filename, "w") : fopen(filename, "a"); 
                if (output_file == NULL) {
                    perror("Erro ao abrir o arquivo de saída.");
                    exit(EXIT_FAILURE);
                }
                // Redireciona a saída padrão para o arquivo
                dup2(fileno(output_file), STDOUT_FILENO);
                
                // remover operador e nome do arquivo da lista de argumentos
                cmd[posicao_op] = NULL;
                cmd[posicao_op + 1] = NULL;
            }
            execvp(cmd[0], cmd);
            fclose(output_file);
            //return -1;

        } else if (p_id > 0) { // pai
            if (op == OP_BACKGROUND) // logo o operador é '&'. Executa comando em background.
                waitpid(-1, &status, WNOHANG);
            else
                waitpid(p_id, &status, 0);

            if (WIFEXITED(status)) // verifica se o processo filho terminou corretamente, retorna 0 se não
                status = WEXITSTATUS(status); // a varíável status recebe o status do processo filho
                                              // se o processo finalizou com sucesso, então recebe 0

            cmd = &cmd[posicao_op + 1];
            if (status == 0) { // caso o processo filho tenha finalizado corretamente, prossiga
                if(pipeline[pp - 1].full)
                    close(pipeline[pp - 1].fd[0]);

                if (op == OP_PIPE) { // caso for um pipe "|"
                    close(pipeline[pp].fd[1]);
                    pipeline[pp].full = TRUE;
                    pp++; // altera o índice do pipe
                    
                } else if (op == OP_OR) { // caso for um operador OR "||"
                    if (verifica_or) { // entra nesse if se o próximo operador for um pipe "|"
                        posicao_op = encontra_pos_operador(cmd, "|");
                        // atualizamos a posição do operador na matriz cmd, pois já iremos executar
                        // as devidas operações, logo atualizamos para que não haja repetição

                        close(pipeline[pp].fd[1]);
                        pipeline[pp].full = TRUE;
                        pp++;
                        verifica_or = FALSE;
                    } else // é um operador OR e não possui um pipe como próximo operador.
                        // como o operador filho foi executado corretamente e não possuímos
                        // pipe como próximo operador, então basta irmos ao próximo operador.
                        posicao_op = encontra_operador(cmd, &op);

                    cmd = &cmd[posicao_op + 1];
                }
            } else { // se o processo filho não foi executado com sucesso
                if (op == OP_AND) { // verifica se o operador é um and "&&"
                    // se o operador for um AND, então não poderemos executar seu comando,
                    // portanto iremos para excluir ele da matriz de argumentos
                    cmd[posicao_op] = NULL;

                    // agora procuramos o próximo operador
                    posicao_op = encontra_operador(cmd, &op);
                    cmd = &cmd[posicao_op + 1];
                }
            }

        } else { // negativo = erro
            perror("fork()");
            return -1;
        }
        
        // visto que durante o processo pai alteramos o valor de posicao_op, 
        // vamos verificar se a posicao_op foi encontrada.
        if (posicao_op == -1) break;
    }
    free(pipeline);
    return 0;
}

int contador_pipes(char **argv) {
    int i = 0, qtd_pipes = 0;
    while (argv[i]) {
        if (strcmp(argv[i], "|") == 0) qtd_pipes++;

        i++;
    }
    return qtd_pipes;
}

int encontra_operador(char **argv, int *op) {
    int i = 0; // posição do operador
    while (argv[i]) {
        if (strcmp(argv[i], "|") == 0) { // operador pipe
            *op = OP_PIPE;
            return i;
        } else if (strcmp(argv[i], "&") == 0) { // executar em background
            *op = OP_BACKGROUND;
            return i;
        } else if (strcmp(argv[i], "||") == 0) { // operador or
            *op = OP_OR;
            return i;
        } else if (strcmp(argv[i], "&&") == 0) { // operador and
            *op = OP_AND;
            return i;
        } else if (strcmp(argv[i], "<") == 0) { // operador entrada padrão
            *op = OP_INPUT;
            return i;
        } else if (strcmp(argv[i], ">") == 0) { // operador saída padrão
            *op = OP_OUTPUT;
            return i;
        } else if (strcmp(argv[i], ">>") == 0) { // operador saída padrão adiciona ao fim do arquivo
            *op = OP_APPEND_OUTPUT;
            return i;
        }
        i++;
    }
    // caso o operador não exista vamos atribuir um valor negativo à ele:
    *op = -1;
    // logo nenhuma posição encontrada, retarnamos -1;
    return -1;
}


// dada uma matriz de argumentos e a quantidade de argumentos, procura por 
// um token e retorna seu índice na matriz de argumentos.
int encontra_pos_operador(char **argv, char *token) {
    int i = 0;
    while (argv[i]) {
        if (strcmp(argv[i], token) == 0) { return i; }
        i++;
    }
    return -1;
}

// Se você tiver um pipe (|) depois de um operador OR (||) em um comando do Linux, isso criará um 
// comportamento inesperado e possivelmente indesejado. Isso ocorre porque o operador OR é executado 
// primeiro, avaliando o resultado do comando anterior. Se o comando anterior falhar, o próximo comando não será executado. 
// Se o comando anterior for bem-sucedido, o próximo comando será executado.
int trata_operador_or(int pos, char **argv) {
    int i = pos + 1; // dado a posição seguinte do operador OR
    while (argv[i]) { // iremos verificar se o próximo operador é um pipe
        if (strcmp(argv[i], "|") == 0) { return TRUE; }
        else if (strcmp(argv[i], "&") == 0) { return FALSE; }
        else if (strcmp(argv[i], "||") == 0) { return FALSE; }
        else if (strcmp(argv[i], "&&") == 0) { return FALSE; }
        else if (strcmp(argv[i], "<") == 0) { return FALSE; }
        else if (strcmp(argv[i], ">") == 0) { return FALSE; }
        else if (strcmp(argv[i], ">>") == 0) { return FALSE; }

        i++;
    }
    return FALSE;
}

void imprime_argv(char **argv) {
  int i = 0;
  if (argv[i]) printf("===\n");
  while (argv[i]) {
    printf("%d: '%s'\n", i, argv[i]);
    i++;
  }
}
