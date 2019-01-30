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



/*----------------------------------------------*/
#define MASK_WIDTH  3
#define HEIGHT_MASK (~(0xFFFFFFFF << MASK_WIDTH))
#define SQUARE_SIZE 7
#define NUM_SHUFFLES_MASK 0x7F
/*----------------------------------------------*/
#define RANDOM_LENGTH 16
static int randomData[RANDOM_LENGTH];
static void fillRandomData(int* p, char* hash)
{
  int i;
  char temp[10];
  temp[8]=0;
  for (i=0;i<RANDOM_LENGTH;i++)
  {
    memcpy(temp,&hash[8*i],8);
		/*
    p[i]=strtol(temp,NULL,16);
    printf("%08X\n",p[i]);
		*/
  }
	/*
  printf("%s\n",hash);
	*/
}

static int getRandomValue()
{
  static int offset=0;
  randomData[offset] += rand();
  offset = randomData[offset] % RANDOM_LENGTH;
	if (offset<0)
		offset = -offset;
  return randomData[offset];
}

static void swapColumns(int square[SQUARE_SIZE][SQUARE_SIZE], int col1, int col2)
{
	int i,temp;
	if (col1>=SQUARE_SIZE)
		return;
	if (col2>=SQUARE_SIZE)
		return;
	if (col1<0)
		return;
	if (col2<0)
		return;
	for (i=0;i<SQUARE_SIZE;i++)
	{
		temp = square[i][col1];
		square[i][col1] = square[i][col2];
		square[i][col2] = temp;
	}
}

static void swapRows(int square[SQUARE_SIZE][SQUARE_SIZE], int row1, int row2)
{
	int i,temp;
	if (row1>=SQUARE_SIZE)
		return;
	if (row2>=SQUARE_SIZE)
		return;
	if (row1<0)
		return;
	if (row2<0)
		return;
	for (i=0;i<SQUARE_SIZE;i++)
	{
		temp = square[row1][i];
		square[row1][i] = square[row2][i];
		square[row2][i] = temp;
	}
}

static void singleShuffle(int square[SQUARE_SIZE][SQUARE_SIZE])
{
	int index1, index2, operation;
	operation = getRandomValue();
	index1 = (operation & HEIGHT_MASK) % SQUARE_SIZE;
	index2 = ((operation >> MASK_WIDTH) & HEIGHT_MASK) % SQUARE_SIZE;
	while (index1==index2)
	{
		index1 = (index1 + getRandomValue()) % SQUARE_SIZE;
	}
	operation = (operation >> 8) & 0x1;

	if (operation)
		swapColumns(square,index1,index2);
	else
		swapRows(square,index1,index2);
		/*
	printf("%i, %i, %i\n",operation,index1,index2);
	*/
}

static void generateSquare(int square[SQUARE_SIZE][SQUARE_SIZE])
{
	int i,j;
	int numShuffles=0;
	while (numShuffles==0)
	{
		numShuffles = getRandomValue() & NUM_SHUFFLES_MASK;
	}
	for (i=0;i<SQUARE_SIZE;i++)
	{
		for (j=0;j<SQUARE_SIZE;j++)
		{
			square[i][j]= 1+((i+j)%SQUARE_SIZE);
		}
	}
	for (i=0;i<numShuffles;i++)
	{
		singleShuffle(square);
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


static int countFromEast(int s[SQUARE_SIZE][SQUARE_SIZE],int row)
{
  int i,last,count;
  if (row<0)
    return 0;
  if (row>=SQUARE_SIZE)
    return 0;
  for (count=0,last=-1,i=SQUARE_SIZE-1;i>=0;i--)
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

static int countFromSouth(int s[SQUARE_SIZE][SQUARE_SIZE],int column)
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

static void printSquare(int square[SQUARE_SIZE][SQUARE_SIZE])
{
	int i,j;
	printf("\n     ");
  for (j=0;j<SQUARE_SIZE;j++)
  {
    printf("%i ",countFromNorth(square,j));
  }
	printf("\n     ");
	for (j=0;j<SQUARE_SIZE;j++)
	{
		printf("-");
		if (j<SQUARE_SIZE-1)
			printf("-");
	}
  printf("\n");
  for (i=0;i<SQUARE_SIZE;i++)
  {
    printf(" %i | ",countFromWest(square,i));
    for (j=0;j<SQUARE_SIZE;j++)
    {
      printf("%i ",square[i][j]);
    }
		printf("| %i\n",countFromEast(square,i));
  }
	printf("     ");
	for (j=0;j<SQUARE_SIZE;j++)
	{
		printf("-");
		if (j<SQUARE_SIZE-1)
			printf("-");
	}
	printf("\n     ");
  for (j=0;j<SQUARE_SIZE;j++)
  {
    printf("%i ",countFromSouth(square,j));
  }
	printf("\n");
}

int main(int argc, char* argv[])
{
	int s[SQUARE_SIZE][SQUARE_SIZE];
	if (SQUARE_SIZE>HEIGHT_MASK)
	{
		printf("ERROR: SQUARE_SIZE exceeds HEIGHT_MASK, rebuild the application.\n");
		return -1;
	}

  if (argc>1)
    fillRandomData(randomData,argv[1]);
  else
    printf("WARNING: random data not available\n");

	generateSquare(s);
	printSquare(s);
  return 0;
}
