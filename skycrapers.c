#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if 0
.PHONY: all run_main
all: run_main

skycrapers.out: skycrapers.c
	rm -f skycrapers.out
	gcc skycrapers.c -o skycrapers.out

run_main: skycrapers.out
	./skycrapers.out `date|sha512sum.exe`
#endif

/*----------------------------------------------*/
#define SQUARE_SIZE 7
/*----------------------------------------------*/
#define MASK_WIDTH  3
#define HEIGHT_MASK ((1<<MASK_WIDTH)-1)
#define NUM_SHUFFLES_MASK 0x7F
#define OPERATION_MASK 0x2
#if SQUARE_SIZE>HEIGHT_MASK
#error SQUARE_SIZE exceeds HEIGHT_MASK
#endif
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
    p[i]=strtol(temp,NULL,16);
		/*
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

static void swapValues(int square[SQUARE_SIZE][SQUARE_SIZE], int val1, int val2)
{
	int i,j;
	if (val1>=SQUARE_SIZE)
		return;
	if (val2>=SQUARE_SIZE)
		return;
	if (val1<0)
		return;
	if (val2<0)
		return;
	val1++;
	val2++;
	for (i=0;i<SQUARE_SIZE;i++)
	{
		for (j=0;j<SQUARE_SIZE;j++)
		{
			if (square[i][j]==val1)
			{
				square[i][j]=val2;
			}
			else if (square[i][j]==val2)
			{
				square[i][j]=val1;
			}
		}
	}
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
	operation = (operation >> (2*MASK_WIDTH)) & OPERATION_MASK;

	switch (operation)
	{
		case 0:
			swapColumns(square,index1,index2);
		break;
		case 1:
			swapRows(square,index1,index2);
		break;
		default:
			swapValues(square,index1,index2);
		break;
	}
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

typedef int (*CountFunctionType)(int[SQUARE_SIZE][SQUARE_SIZE],int);


static int getCount(const int array[SQUARE_SIZE], int visible[SQUARE_SIZE])
{
	int i, count, largest;
	for (i=0,count=0,largest=-1;i<SQUARE_SIZE;i++)
	{
		if (array[i]>largest)
		{
			largest = array[i];
			if (visible!=NULL)
				visible[i] = 1;
			count++;
		}
		else
		{
			if (visible!=NULL)
				visible[i] = 0;
		}
	}
	return count;
}

static int* addShuffle(int* pShuffleList, int newLength, int index1, int index2)
{
	if (pShuffleList==NULL)
	{
		if (newLength!=1)
		{
			printf("ERROR: invalid length\n");
			return NULL;
		}
		pShuffleList = (int*)malloc(2*sizeof(int));
		if (pShuffleList==NULL)
		{
			printf("ERROR: malloc failed in addShuffle\n");
			return NULL;
		}
		pShuffleList[0]=index1;
		pShuffleList[1]=index2;
	}
	else
	{
		if (newLength<2)
		{
			printf("ERROR: invalid length (2)\n");
			return NULL;
		}
		pShuffleList = (int*)realloc(pShuffleList,newLength*2*sizeof(int));
		if (pShuffleList==NULL)
		{
			printf("ERROR: realloc failed in addShuffle\n");
			return NULL;
		}
		pShuffleList[(newLength-1)*2]=index1;
		pShuffleList[(newLength-1)*2+1]=index2;
	}
	return pShuffleList;
}


static int increaseCount(int array[SQUARE_SIZE], int reqCount)
{
	int i,j,temp,count;
	int attempt[SQUARE_SIZE];
	int visible[SQUARE_SIZE];
	int swapIndex1, swapIndex2, maxCount;
	maxCount=-1;

	count = getCount(array,visible);
	for (i=0;i<SQUARE_SIZE;++i)
	{
		for (j=i+1;j<SQUARE_SIZE;++j)
		{
			if (visible[i] || visible[j])
			{
				memcpy(&attempt[0],&array[0],SQUARE_SIZE*sizeof(int));
				temp = attempt[i];
				attempt[i] = attempt[j];
				attempt[j] = temp;
				temp = getCount(attempt,NULL);
				if (temp == reqCount)
				{
					printf("swapping positions %i and %i\n",i,j);
					printf("Finished\n");
					temp = array[i];
					array[i] = array[j];
					array[j] = temp;
					return 0;
				}

				if (temp > reqCount)
				{
					printf("too high\n");
					continue;
				}
				else
				{
					if (temp>maxCount)
					{
						printf("found %i\n",temp);
						maxCount = temp;
						swapIndex1 = i;
						swapIndex2 = j;
					}
				}
			}
		}
	}
	printf("swapping positions %i and %i\n",swapIndex1,swapIndex2);
	temp = array[swapIndex1];
	array[swapIndex1] = array[swapIndex2];
	array[swapIndex2] = temp;

	return 1;
}

static int decreaseCount(int array[SQUARE_SIZE], int reqCount)
{
	int i,j,temp, count;
	int attempt[SQUARE_SIZE];
	int visible[SQUARE_SIZE];
	int swapIndex1, swapIndex2, minCount;
	minCount=SQUARE_SIZE+1;

	count = getCount(array,visible);
	for (i=0;i<SQUARE_SIZE;++i)
	{
		for (j=i+1;j<SQUARE_SIZE;++j)
		{
			if (visible[i] || visible[j])
			{
				memcpy(&attempt[0],&array[0],SQUARE_SIZE*sizeof(int));
				temp = attempt[i];
				attempt[i] = attempt[j];
				attempt[j] = temp;
				temp = getCount(attempt,NULL);
				if (temp == reqCount)
				{
					printf("swapping positions %i and %i\n",i,j);
					printf("Finished\n");
					temp = array[i];
					array[i] = array[j];
					array[j] = temp;
					return 0;
				}
				if (temp < reqCount)
				{
					printf("too low\n");
					continue;
				}
				else
				{
					if (temp < minCount)
					{
						minCount = temp;
						swapIndex1 = i;
						swapIndex2 = j;
					}
				}
			}
		}
	}
	printf("swapping positions %i and %i\n",swapIndex1,swapIndex2);
	temp = array[swapIndex1];
	array[swapIndex1] = array[swapIndex2];
	array[swapIndex2] = temp;
	return 1;
}

static void getShuffleList(const int array[SQUARE_SIZE], int reqCount)
{
	int i,j, count, last, temp, numShuffles, delta, topPosition, bestMin, bestMax;
	int i1, i2;
	int visible[SQUARE_SIZE];
	int copy[SQUARE_SIZE];
	int attempt[SQUARE_SIZE];
	int* result;
	int goingUp;
	int iterations;
	memcpy(&copy[0],&array[0],SQUARE_SIZE*sizeof(int));
	for (iterations=0;iterations<SQUARE_SIZE+1;iterations++)
	{
		count = getCount(copy,visible);
		if (count == reqCount)
		{
			printf("good\n");
			return;
		}
		goingUp = (count<reqCount)?1:0;

		if (goingUp)
			temp = increaseCount(copy,reqCount);
		else
			temp = decreaseCount(copy,reqCount);


	} /* main loop */
}


static int shuffleByRequirement(int square[SQUARE_SIZE][SQUARE_SIZE], int viewPoint, int index, int reqCount)
{
	int count;
	static CountFunctionType fn[4]={countFromNorth,countFromEast,countFromSouth,countFromWest};
	if (viewPoint<0)
		return -1;
	if (viewPoint>3)
		return -1;
	if (index<0)
		return -1;
	if (index>=SQUARE_SIZE)
		return -1;
	if (reqCount<1)
		return -1;
	if (reqCount>SQUARE_SIZE)
		return -1;
	count = fn[viewPoint](square,index);
	if (count == reqCount)
		return 1;
	if (count>reqCount)
	{

	}
	else
	{

	}
	return 0;
}


int main(int argc, char* argv[])
{
	int i;
	int s[SQUARE_SIZE][SQUARE_SIZE];
	int line[SQUARE_SIZE]    = {2,5,1,7,3,4,6};
	int smaller[SQUARE_SIZE] = {0,0,0,0,0,0,0};
	int visible[SQUARE_SIZE]  = {0,0,0,0,0,0,0};
  if (argc>1)
    fillRandomData(randomData,argv[1]);
  else
    printf("WARNING: random data not available\n");

	generateSquare(s);
	printSquare(s);

	printf("Count==%i\n",getCount(line,visible));
	printf("line    == ");
	for (i=0;i<SQUARE_SIZE;i++)
	{
		printf("%i ",line[i]);
	}
	printf("\nvisible == ");
	for (i=0;i<SQUARE_SIZE;i++)
	{
		printf("%i ",visible[i]);
	}
	printf("\n");
	getShuffleList(line,5);
  return 0;
}
