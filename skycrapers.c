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
  if (value>SQUARE_SIZE)
    return -1;
  return 0;
}

static int isAvailable(struct Square *p, int row, int column, unsigned value)
{
  return (p->data[row][column] & (1<<(value-1)))?1:0;
}

static void blockValue(struct Square *p, int row, int column, unsigned value)
{
  unsigned int mask;
  mask = 1<<(value-1);
  if (p->data[row][column] & mask)
  {
    p->data[row][column] &= (~mask);
  }
}

void setValue(struct Square *p, unsigned int value, int row, int column)
{
  int i;
  if (checkArgs(row,column,value)==0)
  {
    if (isAvailable(p,row,column,value))
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
    }
    else
      printf("Not available\n");
  }
  else
    printf("Bad arguments\n");
}

static void printValue(unsigned int value)
{
  int i;
  for (i=0;i<SQUARE_SIZE;++i)
  {
    if (value == (1<<i))
    {
      printf(" .%i ",i+1);
      return;
    }
  }
  printf(" %02X ",value);
}

static char getClue(int* clueArray, int pos)
{
  if (clueArray==NULL)
    return ' ';
  if (clueArray[pos]>0)
    return ('0' + clueArray[pos]);
  return ' ';
}

static void printSquare(struct Square *p, int* clueArray)
{
  int column, row;
  printf("\n     ");
  for (column=0;column<SQUARE_SIZE;++column)
  {
    printf("%c   ",getClue(clueArray,column));
  }
  printf("\n  +");
  for (column=0;column<SQUARE_SIZE;++column)
  {
    printf("----");
  }
  printf("-+\n");
  for (row=0;row<SQUARE_SIZE;++row)
  {
    if (row>0)
	{
	  printf("  | ");
	  for (column=0;column<SQUARE_SIZE;++column)
	  {
	    printf("    ");
	  }
	  printf("|\n");
	}
	printf("%c |",getClue(clueArray,4*SQUARE_SIZE-1-row));
	for (column=0;column<SQUARE_SIZE;++column)
	{
	  printValue(p->data[row][column]);
	}
	printf(" | %c\n",getClue(clueArray,SQUARE_SIZE+row));
  }
  printf("  +");
  for (column=0;column<SQUARE_SIZE;++column)
  {
    printf("----");
  }
  printf("-+\n");
  printf("     ");
  for (column=0;column<SQUARE_SIZE;++column)
  {
    printf("%c   ",getClue(clueArray,3*SQUARE_SIZE-1-column));
  }
  printf("\n");
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

static void applyClue(struct Square *p, unsigned int clue, int index, int isRow, int reversed)
{
  unsigned n, b, upper, lower;
  int i;
  if (clue==0)
    return;
  if (clue==1)
  {
    if (isRow)
    {
      if (reversed)
      {
        setValue(p,SQUARE_SIZE,index,SQUARE_SIZE-1);
      }
      else
      {
        setValue(p,SQUARE_SIZE,index,0);
      }
    }
    else
    {
      if (reversed)
      {
        setValue(p,SQUARE_SIZE,SQUARE_SIZE-1,index);
      }
      else
      {
        setValue(p,SQUARE_SIZE,0,index);
      }
    }
    return;
  }
  if (clue==SQUARE_SIZE)
  {
    if (reversed)
    {
      for (i=0;i<SQUARE_SIZE;++i)
      {
        if (isRow)
          setValue(p,SQUARE_SIZE-i,index,i);
        else
          setValue(p,SQUARE_SIZE-i,i,index);
      }
    }
    else
    {
      for (i=0;i<SQUARE_SIZE;++i)
      {
        if (isRow)
          setValue(p,i+1,index,i);
        else
          setValue(p,i+1,i,index);
      }
    }
    return;
  }
  for (n=0;n<clue-1;++n)
  {
    lower = SQUARE_SIZE + 2 - clue + n;
    upper = SQUARE_SIZE;
    for (b=lower;b<=upper;++b)
    {
      if (isRow)
      {
        if (reversed)
            blockValue(p,index,SQUARE_SIZE-1-n,b);
        else
            blockValue(p,index,n,b);
      }
      else
      {
        if (reversed)
            blockValue(p,SQUARE_SIZE-1-n,index,b);
        else
            blockValue(p,n,index,b);
      }
    }
  }
}

static int getFreedom(unsigned int x)
{
  int i, result;
  for (result=0,i=0;i<SQUARE_SIZE;++i)
  {
    if (x | (1<<i))
    {
      ++result;
    }
  }
  return result;
}

static void gatherCount(int* countArray, unsigned int mask)
{
  int i;
  for (i=0;i<SQUARE_SIZE;++i)
  {
    if (mask & (1UL<<i))
    {
      ++countArray[i];
    }
  }
}

static void getValuesToSet(int *countArray, unsigned int* valueArray)
{
  int i,j;
  memset(valueArray,0,SQUARE_SIZE*sizeof(unsigned int));
  for (j=0,i=0;i<SQUARE_SIZE;++i)
  {
    if (countArray[i]==1)
    {
      valueArray[j]=i+1;
      ++j;
    }
  }
}

struct Setting
{
  unsigned int value;
  int row;
  int column;
};

static int scanSquare(struct Square *p, struct Setting* pSettings)
{
  int i,j,k,m,f,value,finalValues,valuesSet, mask;
  int columnCount[SQUARE_SIZE][SQUARE_SIZE];
  int rowCount[SQUARE_SIZE];
  int minFreedom = SQUARE_SIZE;
  int r,c;
  unsigned int valueArray[SQUARE_SIZE];
  printf("--------------------------------\n");
  finalValues=0;
  valuesSet=0;
  memset(&columnCount[0][0],0,SQUARE_SIZE*SQUARE_SIZE*sizeof(int));
  for (i=0;i<SQUARE_SIZE;++i)
  {
    memset(&rowCount[0],0,SQUARE_SIZE*sizeof(int));
    for (j=0;j<SQUARE_SIZE;++j)
    {
      f = getFreedom(p->data[i][j]);
      switch (f)
      {
        case 0:
          return -1;
        break;
        case 1:
          ++finalValues;
        break;
        default:
          if (f<minFreedom)
          {
            minFreedom = f;
            r = i;
            c = j;
          }
        break;
      }
      gatherCount(rowCount,p->data[i][j]);
      gatherCount(columnCount[j],p->data[i][j]);
    }
    getValuesToSet(rowCount,valueArray);
    printf("Row %i: ",i);
    for (m=0;m<SQUARE_SIZE;++m) printf("%i",valueArray[m]);
    printf("\n");
    for (m=0;m<SQUARE_SIZE;++m)
    {
      value = valueArray[m];
      if (value>0)
      {
        for (k=0;k<SQUARE_SIZE;++k)
        {
          mask = 1<<(value-1);
          if ((p->data[i][k] & mask) && (p->data[i][k]!=mask))
          {
            printf("Found value %i at position [%i][%i]\n",value,i,k);
            pSettings[valuesSet].value = value;
            pSettings[valuesSet].row = i;
            pSettings[valuesSet].column = k;
            ++valuesSet;
          }
        }
      }
    }
  }
  for (i=0;i<SQUARE_SIZE;++i)
  {
    getValuesToSet(columnCount[i],valueArray);
    printf("Column %i: ",i);
    for (m=0;m<SQUARE_SIZE;++m) printf("%i",valueArray[m]);
    printf("\n");
    for (m=0;m<SQUARE_SIZE;++m)
    {
      value = valueArray[m];
      if (value>0)
      {
        for (k=0;k<SQUARE_SIZE;++k)
        {
          mask = 1<<(value-1);
          if ((p->data[k][i] & mask) && (p->data[k][i]!=mask))
          {
            printf("Found value %i at position [%i][%i]\n",value,k,i);
            pSettings[valuesSet].value = value;
            pSettings[valuesSet].row = k;
            pSettings[valuesSet].column = i;
            ++valuesSet;
          }
        }
      }
    }
  }
  if (finalValues==SQUARE_SIZE*SQUARE_SIZE)
    return 0;
  
  return valuesSet;
}

static int getParams(int position, int* pIndex, int* pIsRow, int* pReversed)
{
  int x, y;
  if (position<0)
    return -1;
  x = position/SQUARE_SIZE;
  if (x>3)
    return -1;
  y = position % SQUARE_SIZE;
  switch (x)
  {
    case 0:
      pIndex[0]   = y;
      pIsRow[0]   = 0;
      pReversed[0] = 0;
    break;        
    case 1:       
      pIndex[0]   = y;
      pIsRow[0]   = 1;
      pReversed[0] = 1;
    break;        
    case 2:       
      pIndex[0]   = SQUARE_SIZE - 1 - y;
      pIsRow[0]   = 0;
      pReversed[0] = 1;
    break;        
    case 3:       
      pIndex[0]   = SQUARE_SIZE - 1 - y;
      pIsRow[0]   = 1;
      pReversed[0] = 0;
    break;
    default:
      return -1;
    break;
  }
  return 0;
}

int applyClueArray(int* array, struct Square* p)
{
  int i, index, isRow, reversed;
  for (i=0;i<4*SQUARE_SIZE;++i)
  {
    if (getParams(i,&index,&isRow,&reversed)==0)
    {
      applyClue(p,array[i],index,isRow,reversed);
    }
    else
      return -1;
  }
  return 0;
}


/*
 sequence of 1 or more specific value settings, each with value, row, column
 sequence of 1 or more trial contexts, each


*/


struct SquareScan
{
  int result; /* -1: error, 0: finished, 1: values to be set, 2: trials to be done */
  unsigned int length; /* this field must be zero when the type is equal to 0 or 1 */
  struct Setting* pSettings; /* this pointer must be NULL when type is equal to 0 or 1 */
};

#define MAX_SETTINGS_NUMBER  (SQUARE_SIZE * SQUARE_SIZE)
static struct Setting localSettingArray[MAX_SETTINGS_NUMBER];

int main(int argc, char* argv[])
{
    struct Square s;
    int count,i;
	/*
    int clues[SQUARE_SIZE * 4] = {7, 0, 0, 0, 2, 2, 3,
                                  0, 0, 3, 0, 0, 0, 0,
                                  3, 0, 3, 0, 0, 5, 0,
                                  0, 0, 0, 0, 5, 0, 4};
								  */
	int clues[SQUARE_SIZE * 4] =  { 0, 2, 3, 0, 2, 0, 0,
    5, 0, 4, 5, 0, 4, 0,
    0, 4, 2, 0, 0, 0, 6,
    5, 2, 2, 2, 2, 4, 1 };
    initialize(&s);
    applyClueArray(clues,&s);
    printSquare(&s,clues);
	while(1)
	{
      count = scanSquare(&s,sett);
      printf("Count==%i\n",count);
      if (count==0)
	  {
	    break;
	  }
	  else
	  {
        for (i=0;i<count;++i)
        {
          setValue(&s,sett[i].value,sett[i].row,sett[i].column);
        }
	  }
    }
    printSquare(&s,clues);
  return 0;
}
