#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

int num_processo;
float proba;
unsigned delay;
char **namedpipe;
int MAX;

void usage(const char *prog)
{
  fprintf(stderr, "Usage : %s nprocess proba tempo\n", prog);
  exit(1);
}

void primeiro_filho()
{
  int count = 0, infd, outfd;

  srand((time_t)getpid());

  while (1)
  {
    if (count > MAX)
    {
      printf("\n");
      fflush(stdout);
      count = -1;
    }

    outfd = open(namedpipe[1], O_WRONLY);

    if (outfd != -1)
    {
      if (write(outfd, &count, sizeof(int)) <= 0)
        fprintf(stderr, "erro escritura [p1]\n");

      close(outfd);
    }
    else
      fprintf(stderr, "[p1] erro abertura arquivo %s\n",namedpipe[1]);

    infd = open(namedpipe[num_processo], O_RDONLY);

    if (infd != -1)
    {
      while (read(infd, &count, sizeof(int)) <= 0);

      close(infd);

      if (count == -1)
      {
        printf("fim processo filho [p1]\n");
        fflush(stdout);
        exit(0);
      }
      count++;
    }
    else
      fprintf(stderr, "[p1] erro abertura arquivo %s\n", namedpipe[num_processo]);

    float num = ((float)rand()) / RAND_MAX;

    if (num < proba)
    {
      printf("[p1] lock on token (val = %d)\n", count);
      fflush(stdout);
      sleep(delay);
      printf("[p1] unlock token\n");
      fflush(stdout);
    }
  }
}

void processo_filho(int i)
{
  int count, infd, outfd;

  srand((time_t)getpid());

  while (1)
  {
    infd = open(namedpipe[i], O_RDONLY);

    if (infd != -1)
    {
      while (read(infd, &count, sizeof(int)) <= 0);

      close(infd);

      if (count != -1)
        count++;
    }
    else
      fprintf(stderr, "[p%d] erro abertura arquivo %s\n", i + 1, namedpipe[i]);

    float num = ((float)rand()) / RAND_MAX;

    if (num < proba && count != -1)
    {
      printf("[p%d] lock on token (val = %d)\n", i + 1, count);
      fflush(stdout);
      sleep(delay);
      printf("[p%d] unlock token\n", i + 1);
      fflush(stdout);
    }

    outfd = open(namedpipe[i + 1], O_WRONLY);

    if (outfd != -1)
    {
      if (write(outfd, &count, sizeof(int)) <= 0)
        fprintf(stderr, "erro escritura [p%d]\n", i + 1);

      close(outfd);
    }
    else
      fprintf(stderr, "[p%d] erro abertura arquivo %s\n", i + 1, namedpipe[i + 1]);

    if (count == -1)
    {
      printf("fim processo filho [p%d]\n", i + 1);
      fflush(stdout);
      exit(0);
    }
  }
}

void ler_argumentos(int argc, char *argv[])
{
  if (argc != 4 && argc != 5)
    usage(argv[0]);

  num_processo = atoi(argv[1]);
  proba = atof(argv[2]);
  delay = atoi(argv[3]);

  if (argc == 5)
    MAX = atoi(argv[4]);
  else
    MAX = __INT_MAX__;

  if (num_processo < 2 || num_processo > 999 ||
      proba <= 0.0 || proba >= 1.0 || delay < 1)
    usage(argv[0]);
}

void criar_named_pipes()
{
  int i;
  char *str;

  printf("\nCriação de %d processos\n\n", num_processo);
  fflush(stdout);

  namedpipe = (char **)malloc((num_processo + 1) * sizeof(char *));

  if (namedpipe == NULL)
  {
    perror("malloc");
    exit(1);
  }

  for (i = 1; i <= num_processo; i++)
  {
    str = (char *)malloc(80);

    if (str == NULL)
    {
      perror("malloc");
      exit(1);
    }

    strcpy(str, "pipe");
    sprintf(str, "%s%d", str, i);
    strcat(str, "to");

    if (i == num_processo)
      strcat(str, "1");
    else
      sprintf(str, "%s%d", str, i + 1);

    namedpipe[i] = str;

    printf("criando o named pipe: %s\n", namedpipe[i]);
    mkfifo(namedpipe[i], 0666);
  }

  printf("\n");
}

void liberar_memoria()
{
  for (int i = 1; i <= num_processo; i++)
    free (namedpipe[i]);

  free (namedpipe);
}

int main(int argc, char *argv[])
{
  int i;
  pid_t pid;

  ler_argumentos(argc,argv);
  criar_named_pipes();

  for (i = 0; i < num_processo; i++)
  {
    pid = fork();

    if (pid == 0)
    {
      if (i == 0)
        primeiro_filho();
      else
        processo_filho(i);
      break;
    }
    else if (pid > 0)
    {
      printf("Criação do processo filho [p%d] : PID %d\n", i + 1, (int)pid);
      fflush(stdout);
    }
    else 
    {
      perror("fork");
      exit(1);
    }
  }

  printf("\n");

  for (int i = 1; i <= num_processo; i++)
    wait(NULL);

  liberar_memoria();
  printf("fim\n");
  return 0;
}
