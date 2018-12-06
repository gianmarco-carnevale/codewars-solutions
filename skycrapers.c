#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if 0
#!/usr/bin/sh
.PHONY: all run_main
all: run_main

RANDOM_STRING=`date|sha512sum.exe`

skycrapers.out: skycrapers.c
	rm -f skycrapers.out
	gcc skycrapers.c -o skycrapers.out

run_main: skycrapers.out
	./skycrapers.out $(RANDOM_STRING)
#endif



/*---------------------*/
#define SQUARE_SIZE 7
/*---------------------*/
#define RANDOM_LENGTH 16
static int randomData[RANDOM_LENGTH];
static void fillRandomData(int* p,char* hash)
{
  int i;
  char temp[10];
  temp[8]=0;
  for (i=0;i<RANDOM_LENGTH;i++)
  {
    memcpy(temp,&hash[8*i],8);
    p[i]=strtol(temp,NULL,16);
    printf("%08X\n",p[i]);
  }
  printf("%s\n",hash);
}

static int getRandomValue()
{
  static int offset=0;
  randomData[offset] += rand();
  offset = randomData[offset] % RANDOM_LENGTH;
  return randomData[offset];
}





static void generateRandomRow(int row[SQUARE_SIZE])
{
  int i;
  int length;
  int selection;
  int r;
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
      selection = getRandomValue() % (SQUARE_SIZE-i);
      row[i] = v[selection];
      memmove(&v[selection],&v[selection+1],(SQUARE_SIZE-selection-1)*sizeof(int));
    }
  }
}

static void crossOut(int freedom[SQUARE_SIZE][SQUARE_SIZE],int column)
{
  int i;
  for (i=0;i<SQUARE_SIZE;i++)
    freedom[i][column]=0;
}

static void generateSquare(int s[SQUARE_SIZE][SQUARE_SIZE])
{
  int i,j,k,value;
  int freedom[SQUARE_SIZE][SQUARE_SIZE];
  int tempRow[SQUARE_SIZE];
  for (i=0;i<SQUARE_SIZE;i++)
  {
    for (j=0;j<SQUARE_SIZE;j++)
    {
      freedom[i][j]=1;
    }
  }
  for (i=0;i<SQUARE_SIZE;i++)
  {
    generateRandomRow(tempRow);
    for (j=0;i<SQUARE_SIZE;j++)
    {
      value = tempRow[j];
      for (k=0;k<SQUARE_SIZE;k++)
      {
        if (freedom[i][value-1])
        {
          crossOut(freedom,);
        }
      }
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
  int i,last,count;
  if (row<0)
    return 0;
  if (row>=SQUARE_SIZE)
    return 0;
  for (count=0,last=-1,i=0;i<SQUARE_SIZE;i++)
  {
    if (s[row][i]>last)
    {
      last=s[row][i];
      count++;
    }
  }
  return count;
}

static int countFromWest(int s[SQUARE_SIZE][SQUARE_SIZE],int row)
{
  int i,last,count;
  if (row<0)
    return 0;
  if (row>=SQUARE_SIZE)
    return 0;
  for (count=0,last=-1,i=0;i<SQUARE_SIZE;i++)
  {
    if (s[row][i]>last)
    {
      last=s[row][i];
      count++;
    }
  }
  return count;
}

static int countFromNorth(int s[SQUARE_SIZE][SQUARE_SIZE],int column)
{
  int i,last,count;
  if (column<0)
    return 0;
  if (column>=SQUARE_SIZE)
    return 0;
  for (count=0,last=-1,i=0;i<SQUARE_SIZE;i++)
  {
    if (s[i][column]>last)
    {
      last=s[i][column];
      count++;
    }
  }
  return count;
}

static int countFromSouth(int** s,int column)
{
  int i,last,count;
  if (column<0)
    return 0;
  if (column>=SQUARE_SIZE)
    return 0;
  for (count=0,last=-1,i=SQUARE_SIZE-1;i>=0;i--)
  {
    if (s[i][column]>last)
    {
      last=s[i][column];
      count++;
    }
  }
  return count;
}




int main(int argc, char* argv[])
{
  int i,j,k,keepLooping;
  if (argc>1)
    fillRandomData(randomData,argv[1]);
  else
    printf("WARNING: random data not available\n");

  int s[SQUARE_SIZE][SQUARE_SIZE];

  printf("  ");
  for (j=0;j<SQUARE_SIZE;j++)
  {
    printf("%i ",countFromNorth(s,j));
  }
  printf("\n");
  for (i=0;i<SQUARE_SIZE;i++)
  {
    printf("%i ",countFromWest(s,i));
    for (j=0;j<SQUARE_SIZE;j++)
    {
      printf("%i ",s[i][j]);
    }
    printf("\n");
  }

  return 0;
}
