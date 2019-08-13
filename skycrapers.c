#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define  SQUARE_SIZE  7

struct Square
{
  unsigned int data[SQUARE_SIZE][SQUARE_SIZE];
};



/*

#define  SIZE  7

static int clues[][SIZE * 4] = {
  { 7, 0, 0, 0, 2, 2, 3,
    0, 0, 3, 0, 0, 0, 0,
    3, 0, 3, 0, 0, 5, 0,
    0, 0, 0, 0, 5, 0, 4 },
  { 0, 2, 3, 0, 2, 0, 0,
    5, 0, 4, 5, 0, 4, 0,
    0, 4, 2, 0, 0, 0, 6,
    5, 2, 2, 2, 2, 4, 1 }
};





static int expected[][SIZE][SIZE] = {
  { { 1, 5, 6, 7, 4, 3, 2 },
    { 2, 7, 4, 5, 3, 1, 6 },
    { 3, 4, 5, 6, 7, 2, 1 },
    { 4, 6, 3, 1, 2, 7, 5 },
    { 5, 3, 1, 2, 6, 4, 7 },
    { 6, 2, 7, 3, 1, 5, 4 },
    { 7, 1, 2, 4, 5, 6, 3 } },
  { { 7, 6, 2, 1, 5, 4, 3 },
    { 1, 3, 5, 4, 2, 7, 6 },
    { 6, 5, 4, 7, 3, 2, 1 },
    { 5, 1, 7, 6, 4, 3, 2 },
    { 4, 2, 1, 3, 7, 6, 5 },
    { 3, 7, 6, 2, 1, 5, 4 },
    { 2, 4, 3, 5, 6, 1, 7 } }
};


*/


static int checkArgs(int row, int column, unsigned int value)
{
  if (row<0)
	return -1;
  if (row>=SQUARE_SIZE)
    return -1;
  if (column<0)
	return -1;
  if (column>=SQUARE_SIZE)
    return -1;
  if (value<1)
    return -1;
  if (value>=SQUARE_SIZE)
    return -1;
  return 0;
}

static int isBlocked(struct Square *p, int row, int column, unsigned value)
{
  return (p->data[row][column] | (1<<(value-1)))?1:0;
}

static void blockValue(struct Square *p, int row, int column, unsigned value)
{
  unsigned int mask;
  mask = 1<<(value-1);
  if (p->data[row][column] | mask)
  {
    p->data[row][column] &= (~mask);
  }
}

int setValue(struct Square *p, int row, int column, unsigned value)
{
  int i;
  if (checkArgs(row,column,value)==0)
  {
	if (!isBlocked(p,row,column,value))
	{
	  p->data[row][column] = 1<<(value-1);
	  for (i=0;i<SQUARE_SIZE;++i)
	  {
	    if (i!=row)
	    {
	      blockValue(p,i,column,value);
	    }
	  }
	  for (i=0;i<SQUARE_SIZE;++i)
	  {
	    if (i!=column)
	    {
	      blockValue(p,row,i,value);
	    }
	  }
	  return 0;
	}
	else
	  return -1;
  }
  else
    return -1;
}

static void printSquare(struct Square *p)
{
  int i,j;
  printf("+");
  for (i=0;i<SQUARE_SIZE;++i)
  {
    printf("----");
  }
  printf("+\n");
  for (i=0;i<SQUARE_SIZE;++i)
  {
	if (i>0)
	{
	  printf("|");
	  for (j=0;j<SQUARE_SIZE;++j)
	  {
	    printf("    ");
	  }
	  printf("|\n");
	}
    printf("|");
	for (j=0;j<SQUARE_SIZE;++j)
	{
	  printf(" %02x ",p->data[i][j]);
	}
	printf("|\n");

  }
  printf("+");
  for (i=0;i<SQUARE_SIZE;++i)
  {
    printf("----");
  }
  printf("+\n");
}

void initialize(struct Square *p)
{
  int i,j;
  unsigned initValue = (1 << SQUARE_SIZE)-1;
  for (i=0;i<SQUARE_SIZE;++i)
  {
    for (j=0;j<SQUARE_SIZE;++j)
	{
	  p->data[i][j] = initValue;
	}
  }
}

static unsigned int freedom(unsigned int x)
{
  unsigned int result;
  int i;
  for (result=0,i=0;i<SQUARE_SIZE;++i)
  {
    if (x | (1<<i))
	{
	  ++result;
	}
  }
  return result;
}


int main(int argc, char* argv[])
{
	struct Square s;
	initialize(&s);
	printSquare(&s);
  return 0;
}
