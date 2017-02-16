#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

/* construct string with program name and arguments */
char **ParseArguments(char *com) {
  const char *space_char = " ";
  char **exec_str = NULL;
  int count = 1;

  com = strtok(com, space_char);
  exec_str = (char **) realloc(exec_str, count * sizeof(*exec_str));
  exec_str[count - 1] = (char *) malloc(strlen(com) * sizeof(char));
  strcat(exec_str[count - 1], com);

  while ((com = strtok(NULL, space_char)) != NULL) {
    count++;
    exec_str = (char **) realloc(exec_str, count * sizeof(*exec_str));
    exec_str[count - 1] = (char *) malloc(strlen(com) * sizeof(char));
    strcat(exec_str[count - 1], com);
  }
  count++;
  exec_str = (char **) realloc(exec_str, count * sizeof(*exec_str));
  exec_str[count - 1] = (char *) malloc(sizeof(char));
  exec_str[count - 1] = NULL;

  return exec_str;
}

void FreeArray(char **exec_str) {
  int i;
  int count = 0;

  while (exec_str[count] != NULL)
    count++;

  for (i = 0; i < count; i++) {
    free(exec_str[i]);
  }
  free(exec_str);
}

int main(int argc, char** argv) {
  /* array with I/O descriptors */
  int pipedes[2];
  int status;
  const char *delim = "|";
  char *com_1;
  char *com_2;
  char **exec_str_1;
  char **exec_str_2;
  /* process id */
  pid_t pid;

  if (argc != 2) {
    printf("Wrong amount of arguments!");
    return EXIT_FAILURE;
  }

  pipe(pipedes);
  pid = fork();

  if (pid < 0) {
    perror("fork: ");
    return EXIT_FAILURE;
  }

  com_1 = strtok(argv[1], delim);
  com_2 = strtok(NULL, delim);

  if (pid > 0) {
    close(pipedes[0]);
    dup2(pipedes[1], 1);
    close(pipedes[1]);
    exec_str_1 = ParseArguments(com_1);
    execvp(*exec_str_1, exec_str_1);
    FreeArray(exec_str_1);
  } else {
    close(pipedes[1]);
    dup2(pipedes[0], 0);
    close(pipedes[0]);
    exec_str_2 = ParseArguments(com_2);
    execvp(*exec_str_2, exec_str_2);
    FreeArray(exec_str_2);
  }

  wait(&status);
  return EXIT_SUCCESS;
}
