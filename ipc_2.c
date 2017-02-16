#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

/* construct string with program name and arguments */
char **ParseArguments(char *com) {
  const char *space_char = " ";
  char **execute_string = NULL;
  int count = 1;

  com = strtok(com, space_char);
  execute_string = (char **) realloc(execute_string,
                                     count * sizeof(*execute_string));
  execute_string[count - 1] = (char *) malloc(strlen(com) * sizeof(char));
  strcat(execute_string[count - 1], com);

  while ((com = strtok(NULL, space_char)) != NULL) {
    count++;
    execute_string = (char **) realloc(execute_string,
                                       count * sizeof(*execute_string));
    execute_string[count - 1] = (char *) malloc(strlen(com) * sizeof(char));
    strcat(execute_string[count - 1], com);
  }
  count++;
  execute_string = (char **) realloc(execute_string,
                                     count * sizeof(*execute_string));
  execute_string[count - 1] = (char *) malloc(sizeof(char));
  execute_string[count - 1] = NULL;

  return execute_string;
}

int main(int argc, char** argv) {
  /* array with I/O descriptors */
  int pipedes[2];
  int status;
  const char *delim = "|";
  char *com_1;
  char *com_2;
  char **proc_1;
  char **proc_2;
  /* process id */
  pid_t pid;

  if (argc != 2) {
    printf("Wrong amount of arguments!");
    return EXIT_FAILURE;
  }

  com_1 = strtok(argv[1], delim);
  com_2 = strtok(NULL, delim);

  proc_1 = ParseArguments(com_1);
  proc_2 = ParseArguments(com_2);

  pipe(pipedes);
  pid = fork();
  if (pid < 0) {
    perror("fork: ");
    return EXIT_FAILURE;
  }
  if (pid > 0) {
    close(pipedes[0]);
    dup2(pipedes[1], 1);
    close(pipedes[1]);
    execvp(*proc_1, proc_1);
  } else {
    close(pipedes[1]);
    dup2(pipedes[0], 0);
    close(pipedes[0]);
    execvp(*proc_2, proc_2);
  }

  wait(&status);
  return EXIT_SUCCESS;
}
