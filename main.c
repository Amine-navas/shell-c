#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/wait.h>

int lsh_launch(char **args)
{
    __pid_t pid, wpid;
    int status;
    pid = fork();
    if (pid == 0)
    {
        if (execvp(args[0], args) == -1)
        {
            perror("lsh");
        }
        exit(EXIT_FAILURE);
    }
    else if (pid < 0)
    {
        perror("lsh");
    }
    else
    {
        do
        {
            wpid = waitpid(pid, &status, WUNTRACED);
            if (wpid == -1)
            {
                perror("lsh");
                break;
            }
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);

char *builtin_str[] = {"cd", "help", "exit"};
int (*builtin_func[])(char **) = {lsh_cd, lsh_help, lsh_exit};

int lsh_num_builtins()
{
    return sizeof(builtin_str) / sizeof(builtin_str[0]);
}

int lsh_cd(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "excepcted argument to \"cd\" \n");
    }
    else
    {
        if (chdir(args[1]) != 0)
        {
            perror("lsh");
        }
    }
    return 1;
}

int lsh_help(char **args)
{
    int i;
    printf("Amine Aymen Senbati lsh\n");
    printf("entre le nom du programme et ses arguments , apres tape entre.\n");
    printf("les cmds suivantes sont built-in : \n");
    for (i = 0; i < lsh_num_builtins(); i++)
    {
        printf(" %s\n", builtin_str[i]);
    }

    printf("tape la cmd man pour plus d'infos sur les autres progs \n");
    return 1;
}

int lsh_exit(char **args)
{
    return 0;
}

int lsh_excecute(char **args)
{
    int i;
    if (args[0] == NULL)
    {
        return 1;
    }

    for (i = 0; i < lsh_num_builtins(); i++)
    {
        if (strcmp(args[0], builtin_str[i]) == 0)
        {
            return (*builtin_func[i])(args);
        }
    }
    return lsh_launch(args);
}

/*


















*/
///////navas///////////////

#define lsh_bufsize 1024
#define lsh_limits " \t\n\a\r"
#define lsh_tor_bufsize 64

char **lsh_split_ligne(char *ligne)
{
    int buffesize = lsh_tor_bufsize, position = 0;
    char **tokens = malloc(buffesize * sizeof(char *));
    char *token;
    if (!tokens)
    {
        fprintf(stderr, "navas mal9itoch\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(ligne, lsh_limits);
    while (token != NULL)
    {
        tokens[position] = token;
        position++;
        if (position >= buffesize)
        {
            buffesize += lsh_tor_bufsize;
            tokens = realloc(tokens, buffesize * sizeof(char *));
            if (!tokens)
            {
                fprintf(stderr, "allocation erreur\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, lsh_limits);
    }
    tokens[position] = NULL;
    return tokens;
}

char *lsh_ligne_reader()
{
    char *ligne = NULL;
    ssize_t bufsize = 0;

    if (getline(&ligne, &bufsize, stdin) == -1)
    {
        if (feof(stdin))
        {
            exit(EXIT_SUCCESS);
        }
        else
        {
            perror("readline");
            exit(EXIT_FAILURE);
        }
    }
    return ligne;
}

void lsh_boucle()
{
    char *ligne;
    char **args;
    int status;

    do
    {
        printf(">> ");
        ligne = lsh_ligne_reader();
        args = lsh_split_ligne(ligne);
        status = lsh_excecute(args);
    } while (status);
}

int main(int args, char **argv)
{
    lsh_boucle();

    return EXIT_SUCCESS;
}
