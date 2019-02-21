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
#define MAX_NUM_REQS (4*SQUARE_SIZE)
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

typedef enum
{
	eFromNorth,
	eFromEast,
	eFromSouth,
	eFromWest
} PointOfView;

static int getViewCount(int s[SQUARE_SIZE][SQUARE_SIZE], PointOfView view, int index)
{
	int i,last,count,value,loopForward,firstBracket;
	if (index<0)
		return 0;
	if (index>=SQUARE_SIZE)
		return 0;
	switch (view)
	{
		case eFromNorth:
			loopForward = 1;
			firstBracket = 0;
		break;
		case eFromEast:
			loopForward = 0;
			firstBracket = 1;
		break;
		case eFromSouth:
			loopForward = 0;
			firstBracket = 0;
		break;
		case eFromWest:
			loopForward = 1;
			firstBracket = 1;
		break;
		default:
		{
			printf("ERROR: invalid point of view %i\n",view);
			return -1;
		}
	}
	if (loopForward)
	{
		for (count=0,last=-1,i=0;i<SQUARE_SIZE;i++)
		{
			value = (firstBracket)?s[index][i]:s[i][index];
			if (value>last)
			{
				last = value;
				count++;
			}
		}
	}
	else
	{
		for (count=0,last=-1,i=SQUARE_SIZE-1;i>=0;i--)
		{
			value = (firstBracket)?s[index][i]:s[i][index];
			if (value>last)
			{
				last = value;
				count++;
			}
		}
	}
	return count;
}

static void swapByView(int square[SQUARE_SIZE][SQUARE_SIZE], PointOfView view, int index1, int index2)
{
	if (index1>=SQUARE_SIZE)
		return;
	if (index2>=SQUARE_SIZE)
		return;
	if (index1<0)
		return;
	if (index2<0)
		return;
	switch (view)
	{
		case eFromNorth:
			swapRows(square,index1,index2);
		break;
		case eFromEast:
			index1 = SQUARE_SIZE - 1 - index1;
			index2 = SQUARE_SIZE - 1 - index2;
			swapColumns(square,index1,index2);
		break;
		case eFromSouth:
			index1 = SQUARE_SIZE - 1 - index1;
			index2 = SQUARE_SIZE - 1 - index2;
			swapRows(square,index1,index2);
		break;
		case eFromWest:
			swapColumns(square,index1,index2);
		break;
		default:
		{
			printf("ERROR: invalid view %i\n",view);
			return;
		}
	}
}

static void getArrayFromSquare(int square[SQUARE_SIZE][SQUARE_SIZE], PointOfView view, int index, int array[SQUARE_SIZE])
{
	int i,last,count,value,loopForward,firstBracket;
	if (index>=SQUARE_SIZE)
		return;
	if (index>=SQUARE_SIZE)
		return;
	switch (view)
	{
		case eFromNorth:
			loopForward = 1;
			firstBracket = 0;
		break;
		case eFromEast:
			loopForward = 0;
			firstBracket = 1;
		break;
		case eFromSouth:
			loopForward = 0;
			firstBracket = 0;
		break;
		case eFromWest:
			loopForward = 1;
			firstBracket = 1;
		break;
		default:
		{
			printf("ERROR: invalid view %i\n",view);
			return;
		}
	}
	if (loopForward)
	{
		for (i=0;i<SQUARE_SIZE;i++)
		{
			array[i] = (firstBracket)?square[index][i]:square[i][index];
		}
	}
	else
	{
		for (i=SQUARE_SIZE-1;i>=0;i--)
		{
			array[SQUARE_SIZE-1-i] = (firstBracket)?square[index][i]:square[i][index];
		}
	}
}


static void printSquare(int square[SQUARE_SIZE][SQUARE_SIZE])
{
	int i,j;
	printf("\n     ");
  for (j=0;j<SQUARE_SIZE;j++)
  {
    printf("%i ",getViewCount(square,eFromNorth,j));
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
    printf(" %i | ",getViewCount(square,eFromWest,i));
    for (j=0;j<SQUARE_SIZE;j++)
    {
      printf("%i ",square[i][j]);
    }
		printf("| %i\n",getViewCount(square,eFromEast,i));
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
    printf("%i ",getViewCount(square,eFromSouth,j));
  }
	printf("\n");
}

struct Requirement
{
	PointOfView pov;
	int pos;
	int count;
};

struct Shuffle
{
	PointOfView pov;
	int pos1;
	int pos2;
	struct Shuffle* next;
};

static struct Shuffle* addShuffle(struct Shuffle* pShuffleList, PointOfView view, int index1, int index2)
{
	struct Shuffle* last , *temp;
	last = (struct Shuffle*)malloc(sizeof(struct Shuffle));
	if (last==NULL)
	{
		printf("ERROR: malloc failed in addShuffle\n");
		return NULL;
	}
	last->pov  = view;
	last->pos1 = index1;
	last->pos2 = index2;
	last->next = NULL;
	if (pShuffleList==NULL)
		return last;
	/* pShuffleList now is surely non null */
	for (temp=pShuffleList;;temp=temp->next)
	{
		if (temp->next == NULL)
		{
			temp->next = last;
			return pShuffleList;
		}
	}
	printf("ERROR: in addShuffle\n");
	return NULL;
}

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

static void adjustCount(int array[SQUARE_SIZE], int reqCount, int* pIndex1, int *pIndex2)
{
	int i,j,temp,count;
	int attempt[SQUARE_SIZE];
	int visible[SQUARE_SIZE];
	int swapIndex1, swapIndex2, swapIndex3, swapIndex4, maxCount, minCount;
	swapIndex1=-1;
	swapIndex2=-1;
	swapIndex3=-1;
	swapIndex4=-1;
	maxCount=-1;
	minCount = SQUARE_SIZE+1;
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
					temp = array[i];
					array[i] = array[j];
					array[j] = temp;

					*pIndex1=i;
					*pIndex2=j;
					return;
				}

				if ((reqCount>count)?(temp > count):(temp < count))
				{
					if ((reqCount>count)?(temp > reqCount):(temp < reqCount))
					{
						printf("Out of limits!\n");
						continue;
					}
					else
					{
						if ((reqCount>count)?(temp > maxCount):(temp < minCount))
						{
							(reqCount>count)?(maxCount = temp):(minCount = temp);
							swapIndex1 = i;
							swapIndex2 = j;
							continue;
						}
					}
				}
				if (temp == count)
				{
					if ((swapIndex3<0) || (swapIndex4<0))
					{
						if (visible[i])
						{
							if ((reqCount>count)?(array[i]>array[j]):(array[i]<array[j]))
							if (array[i]>array[j])
							{
								swapIndex3 = i;
								swapIndex4 = j;
								continue;
							}
						}
					}
				}
			}
		}
	}

	if ((swapIndex1>=0) || (swapIndex2>=0))
	{
		temp = array[swapIndex1];
		array[swapIndex1] = array[swapIndex2];
		array[swapIndex2] = temp;

		*pIndex1=swapIndex1;
		*pIndex2=swapIndex2;
		return;
	}
	if ((swapIndex3>=0) || (swapIndex4>=0))
	{
		temp = array[swapIndex3];
		array[swapIndex3] = array[swapIndex4];
		array[swapIndex4] = temp;

		*pIndex1=swapIndex3;
		*pIndex2=swapIndex4;
		return;
	}
	printf("ERROR: unhandled situation\n");
	return;
}

static struct Shuffle* getShuffleList(int array[SQUARE_SIZE], PointOfView view, int reqCount)
{
	int i,j, count, last, temp, numShuffles, delta, topPosition, bestMin, bestMax;
	int i1, i2;
	int visible[SQUARE_SIZE];
	int copy[SQUARE_SIZE];
	int iterations;
	struct Shuffle* result;

	for (result=NULL,iterations=0;iterations<3*SQUARE_SIZE;iterations++)
	{
		count = getCount(array,NULL);
		if (count == reqCount)
		{
			printf("\n");
			return result;
		}
		adjustCount(array,reqCount,&i1,&i2);
		printf("(%i,%i,%i)",i1,i2,view);
		result = addShuffle(result,view,i1,i2);
	}
	printf("\n");
	printf("ERROR: incomplete shuffle list\n");
	return result;
}

static void printShuffleList(struct Shuffle* list)
{
	for (;list;list=list->next)
	{
		printf("swapping %i and %i\n",list->pos1,list->pos2);
	}
}

static void deleteShuffleList(struct Shuffle* list)
{
	struct Shuffle* temp;
	for (;list;)
	{
		temp = list->next;
		free(list);
		list = temp;
	}
}

static struct Shuffle* getShuffleListByRequirement(int square[SQUARE_SIZE][SQUARE_SIZE], PointOfView view, int index, int reqCount)
{
	int array[SQUARE_SIZE];
	getArrayFromSquare(square,view,index,array);
	return getShuffleList(array,view,reqCount);
}

static void applyShuffleList(int square[SQUARE_SIZE][SQUARE_SIZE], struct Shuffle* list)
{
	for (;list;list=list->next)
	{
		swapByView(square,list->pov,list->pos1,list->pos1);
		printf("{%i,%i,%i}",list->pos1,list->pos1,list->pov);
	}
	printf("\n");
}

static void findSolution(int square[SQUARE_SIZE][SQUARE_SIZE], struct Requirement* pReq, int numReqs)
{
	int i, goodCount,j;
	struct Shuffle* list;
	if (pReq==NULL)
		return;
	if (numReqs<=0)
		return;
	for (j=0;j<10;++j)
	{
		for (goodCount=0,i=0;i<numReqs;++i)
		{
			list = getShuffleListByRequirement(square,pReq[i].pov,pReq[i].pos,pReq[i].count);
			if (list!=NULL)
			{
				applyShuffleList(square,list);
				deleteShuffleList(list);
			}
			else
			{
				++goodCount;
			}
		}
		if (goodCount==numReqs)
		{
			printf("Solution found:\n");
			printSquare(square);
			return;
		}
	}
}

static void initializeSquare(int square[SQUARE_SIZE][SQUARE_SIZE])
{
	int i,j;
	for (i=0;i<SQUARE_SIZE;++i)
	{
		for (j=0;j<SQUARE_SIZE;++j)
		{
			square[i][j] = 1+((i+j)%SQUARE_SIZE);
		}
	}
}


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

int main(int argc, char* argv[])
{
	int i;
	struct Shuffle* list;
	struct Requirement reqsArray[MAX_NUM_REQS] = {{eFromNorth,0,7},
                                                {eFromNorth,4,2},
																							  {eFromNorth,5,2},
																							  {eFromNorth,6,3},
																							  { eFromEast,2,3},
																							  {eFromSouth,0,3},
																							  {eFromSouth,2,3},
																							  {eFromSouth,5,5},
																							  { eFromEast,4,5},
																							  { eFromEast,6,4}};
  int numOfReqs = 10;
	int s[SQUARE_SIZE][SQUARE_SIZE];
	int line[SQUARE_SIZE]    = {1,3,4,2,6,7,5};
	int smaller[SQUARE_SIZE] = {0,0,0,0,0,0,0};
	int visible[SQUARE_SIZE] = {0,0,0,0,0,0,0};

  if (argc>1)
    fillRandomData(randomData,argv[1]);
  else
    printf("WARNING: random data not available\n");

	generateSquare(s);
	printSquare(s);

	findSolution(s,reqsArray,numOfReqs);
	printSquare(s);
  return 0;
}
