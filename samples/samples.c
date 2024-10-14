#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char *argv[])
{
  FILE *fp;
  int nfrags, max, i;
  long size, pos;
  char *buffer;
  double val;
  
  if (argc != 4)
  {
    fprintf(stderr, "usage: samples file numberfrags maxfragsize\n");
    return EXIT_FAILURE;
  }
  else
  {
    fp = fopen(argv[1], "r");

    if (fp == NULL)
    {
      fprintf(stderr, "impossível abrir o arquivo %s\n", argv[1]);
      fprintf(stderr, "usage: samples file numberfrags maxfragsize\n");
      return EXIT_FAILURE;
    }

    nfrags = atoi(argv[2]);
    max = atoi(argv[3]);

    if (nfrags < 1 || max < 1)
    {
      fprintf(stderr, "usage: samples file numberfrags maxfragsize\n");
      return EXIT_FAILURE;
    }
  }

  srandom(0);

  fseek(fp, 0L, SEEK_END);
  size = ftell(fp);

  buffer = (char *)malloc(size);

  if (buffer == NULL)
  {
    fprintf(stderr, "impossível alocar memória\n");
    return EXIT_FAILURE;
  }

  fseek(fp, -1L, SEEK_END);
  fread(buffer, 1, 1, fp);

  if (buffer[0] == '\n')
    size--;

  fseek(fp, 0L, SEEK_SET);
  fread(buffer, 1, size, fp);
  printf("\n%s\n\n", buffer);

  for (i = 0; i < nfrags; i++) 
  {
    val = (double) random();

    val = (val / RAND_MAX) * (size - max);

    pos = round(val);

    fseek(fp, pos, SEEK_SET);
    fread(buffer, 1, max, fp);
    buffer[max] = 0;
    printf(">%s<\n", buffer);
  }

  free(buffer);
  fclose(fp);
  return 0;
}