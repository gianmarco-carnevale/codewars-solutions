#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SQUARE_SIZE 7

static void generateRandomArray(int* row)
{
  int i;
  int length;
  int selection;
  int v[SQUARE_SIZE];
  for (i=0;i<SQUARE_SIZE;i++)
  {
    v[i]=i+1;
  }
  for (i=0;i<SQUARE_SIZE;i++)
  {
    if (i==SQUARE_SIZE-1)
    {
      row[i]=v[0];
    }
    else
    {
      selection = rand() % (SQUARE_SIZE-i);
      row[i] = v[selection];
      memmove(&v[selection],&v[selection+1],(SQUARE_SIZE-selection-1)*sizeof(int));
    }
  }
}

static int commonValue(int* a, int* b)
{
  int i;
  for (i=0;i<SQUARE_SIZE;i++)
  {
    if (a[i]==b[i])
      return 1;
  }
  return 0;
}


static int countFromEast(int** s,int row)
{
  int i;
}

int main(int argc, char* argv[])
{
  int i,j,k,keepLooping;
  int s[SQUARE_SIZE][SQUARE_SIZE];
  for (i=0;i<SQUARE_SIZE;i++)
  {
    for (j=0;j<SQUARE_SIZE;j++)
    {
      if (i>0)
      {
        generateRandomArray(s[i]);
        for (;commonValue(s[i],s[i-1]);)
        {
          generateRandomArray(s[i]);
        }
      }
      else
        generateRandomArray(s[i]);
    }
  }
  for (i=0;i<SQUARE_SIZE;i++)
  {
    for (j=0;j<SQUARE_SIZE;j++)
    {
      printf("%i ",s[i][j]);
    }
    printf("\n");
  }

  return 0;
}
