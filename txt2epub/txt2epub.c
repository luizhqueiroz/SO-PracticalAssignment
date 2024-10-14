#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <fcntl.h>

void processo_filho(char *filename)
{
  char str1[255], str2[255];
  char *args[4];

  args[0] = "/usr/bin/pandoc";

  sprintf(str1, "%s.txt", filename);
  args[1] = str1;

  sprintf(str2, "-o%s.epub", filename);
  args[2] = str2;

  args[3] = NULL;

  for (int i = 0; args[i] != NULL; i++)
    printf("[%s] ", args[i]);

  execv(args[0], args);

  fprintf(stderr, "conversão impossível %s.txt\n", filename);
  exit(EXIT_FAILURE);
}

char *toUpper(char *str)
{
  int i;
  char *ptr = str;
  char *str2 = (char *)malloc(strlen(str + 1));

  for (i = 0; *ptr != 0; ptr++, i++)
    str2[i] = (char)toupper(str[i]);

  str2[i] = 0;
  return str2;
}

int main(int argc, char *argv[])
{
  int i;
  pid_t pid;
  char **args;
  char *ptr;

  printf("\n");

  for (i = 1; i < argc; i++)
  {
    for (ptr = argv[i]; *ptr != 0; ptr++)
      if (*ptr == '.')
      {
        *ptr = 0;
        ptr++;

        if (strcmp(toUpper(ptr), "TXT"))
        {
          fprintf(stderr, "extenção incorreta: %s", ptr);
          exit(EXIT_FAILURE);
        }

        break;
      }

    pid = fork();

    if (pid == 0)
      processo_filho(argv[i]);
    else if (pid > 0)
    {
      printf("[pid%d] converting %s.txt ...\n", (int)pid, argv[i]);
      fflush(stdout);
    }
    else
    {
      perror("fork");
      exit(EXIT_FAILURE);
    }
  }

  printf("\n");

  for (int i = 1; i < argc; i++)
  {
    pid = wait(NULL);
    printf("[pid%d] arquivo epub gerado\n", (int)pid);
  }

  printf("\ncompactando: zip ebooks.zip ");
  args = (char **)malloc((argc + 2) * sizeof(char *));

  args[0] = (char *)malloc(13);
  strcpy(args[0],"/usr/bin/zip");

  args[1] = (char *)malloc(11);
  strcpy(args[1],"ebooks.zip");
  
  for (int i = 2; i <= argc; i++)
  {
    args[i] = (char *)malloc(strlen(argv[i - 1]) + 6);
    sprintf(args[i], "%s.epub", argv[i - 1]);
    printf("%s ", args[i]);
  }

  args[argc + 1] = NULL;
  printf(" ...\n\n");

  pid = fork();

  if (pid == 0)
    execv(args[0], args);
  else if ( pid < 0)
  {
    perror("fork");
    exit(EXIT_FAILURE);
  }

  wait(NULL);

  for (int i = 0; i <= argc; i++) free (args[i]);

  free (args);

  printf("fim\n");

  return 0;  
}