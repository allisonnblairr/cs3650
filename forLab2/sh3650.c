/*
 * file:        shell3650.c
 * description: skeleton code for a simple shell
 *
 * Peter Desnoyers, Northeastern Fall 2023
 */

/* <> means don't check the local directory */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

/* "" means check the local directory */
#include "parser.h"

/* you'll need these includes later: */
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <limits.h>

int status = 0;

int cd(int argc, char **argv)
{
    if(argc > 2){
     fprintf(stderr, "cd: wrong number of arguments\n");
     status = 1;
    } else {
    if(argc == 1) {
     chdir(getenv("HOME"));
     status = 0;
    } else {
      chdir(argv[1]);
      status = 0;
       if (chdir(argv[1]) != 0) {
        fprintf(stderr, "cd: %s\n", strerror(errno));
         status = 1;
        }
    }
  }

   return status;
}

int pwd(int argc, char **argv){
   status = 0;
   char cwd[PATH_MAX];
    if(getcwd(cwd, sizeof(cwd)) != NULL) {
     printf("%s\n", cwd);
    } 

  return status;
}

int exit_v2(int argc, char **argv) {
   if(argc > 2) {
     fprintf(stderr, "exit: too many arguments\n");
     status = 1;
    } else if(argc == 1) {
       exit(0);
       status = 0;
    } else {
     exit(atoi(argv[1]));
     status = atoi(argv[1]);
    }

  return status;
}


int ls_command(int argc, char **argv) {
    int pidStatus;
    int pid = fork();

    if(pid < 0) {
    fprintf(stderr, "fork failed\n");
    status = 1;
    exit(1); 
  } else if (pid == 0) {
   // create the child command and implement execvp
   for(int i = 0; i < argc; ++i) {
     if(strcmp(argv[i], "<") == 0) {
       int input = open(argv[i + 1], O_RDONLY);
       if(input < 0) {
        status = 1;
        fprintf(stderr, "non-existing-file: %s\n", strerror(errno));
        exit(EXIT_FAILURE); 
       }
       dup2(input, STDIN_FILENO);
       close(input);
       argv[i] = NULL;
     } else if(strcmp(argv[i], ">") == 0) {
       int output = open(argv[i + 1], O_CREAT|O_TRUNC|O_WRONLY, 0777);
       if(output < 0) {
        close(output);
        argv[i] = NULL;
       }
       dup2(output, STDOUT_FILENO);
       close(output);
       argv[i] = NULL;
      }
   }
    execvp(argv[0], argv);
    status = 0;
    if(execvp(argv[0], argv) != 0) {
    fprintf(stderr,"%s: %s\n", argv[0], strerror(errno));
    status = 1;
    exit(EXIT_FAILURE);
   }
  } else {
   // where parent goes
  do {
     waitpid(pid, &pidStatus, WUNTRACED);
  } while (!WIFEXITED(pidStatus) && WIFSIGNALED(pidStatus));
     status = WEXITSTATUS(pidStatus); 
  }
return status;
}


int echoStatus(int argc, char **argv) {
    char qbuf[16];
    sprintf(qbuf, "%d", status);
    for(int i = 0; i < argc; ++i) {
        if(strcmp(argv[i], "$?") == 0) {
           argv[i] = qbuf;
        }
     }
return status;
} 


// for testing purposes
int testFalse() {
  status = 1;
return status;
}

int main(int argc, char **argv)
{
    signal(SIGINT, SIG_IGN); /* ignore SIGINT=^C */
    bool interactive = isatty(STDIN_FILENO); /* see: man 3 isatty */
    FILE *fp = stdin;

    if (argc == 2) {
        interactive = false;
        fp = fopen(argv[1], "r");
        if (fp == NULL) {
            fprintf(stderr, "%s: %s\n", argv[1], strerror(errno));
            exit(EXIT_FAILURE); /* see: man 3 exit */
        }
    }
    if (argc > 2) {
        fprintf(stderr, "%s: too many arguments\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char line[1024], linebuf[1024];
    const int max_tokens = 32;
    char *tokens[max_tokens];

    /* loop:
     *   if interactive: print prompt
     *   read line, break if end of file
     *   tokenize it
     *   print it out <-- your logic goes here
     */
    while (true) {
        if (interactive) {
            /* print prompt. flush stdout, since normally the tty driver doesn't
             * do this until it sees '\n'
             */
            printf("sh3650> ");
            fflush(stdout);
        }

        /* see: man 3 fgets (fgets returns NULL on end of file)
         */
        if (!fgets(line, sizeof(line), fp))
            break;

        /* read a line, tokenize it, and print it out
         */
        int n_tokens = parse(line, max_tokens, tokens, linebuf, sizeof(linebuf));

        if(n_tokens > 0) {
         if(strcmp(tokens[0], "cd") == 0) {
         cd(n_tokens, tokens);
         } else if (strcmp(tokens[0], "pwd") == 0) {
         pwd(n_tokens, tokens);
         } else if (strcmp(tokens[0], "exit") == 0) {
          exit_v2(n_tokens, tokens);
         } else {
          echoStatus(n_tokens, tokens);
          ls_command(n_tokens, tokens);
          }
        }
    }

    if (interactive)            /* make things pretty */
        printf("\n");           /* try deleting this and then quit with ^D */

return 0;
}

