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

static unsigned int getValueFromMask(unsigned int mask)
{
  int i;
  for (i=0;i<SQUARE_SIZE;++i)
  {
    if (mask == (1<<i))
      return i+1;
  }
  return 0;
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
      printf("Value %d not available at row %i column %i\n",value,row,column);
  }
  else
    printf("Bad arguments: %d,%i,%i\n",value,row,column);
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
  fflush(stdout);
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

static int getFreedom(unsigned int mask)
{
  int i, result;
  for (result=0,i=0;i<SQUARE_SIZE;++i)
  {
    if (mask & (1<<i))
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

static unsigned int getValuesToSet(int *countArray, unsigned int* valueArray)
{
  int i, result;
  memset(valueArray,0,SQUARE_SIZE*sizeof(unsigned int));
  for (result=0,i=0;i<SQUARE_SIZE;++i)
  {
    if (countArray[i]==1)
    {
      valueArray[result]=i+1;
      ++result;
    }
  }
  return (unsigned int)result;
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

struct Setting
{
  unsigned int value;
  int row;
  int column;
};


struct SquareScan
{
  int result; /* -1: error, 0: finished, 1: values to be set, 2: trials to be done */
  unsigned int length; /* this field must be zero when the type is equal to 0 or 1 */
  struct Setting* settingList; /* this pointer must be NULL when type is equal to 0 or 1 */
};

static void clearScan(struct SquareScan* p)
{
  if (p->settingList!=NULL)
  {
    free(p->settingList);
  }
  p->settingList=NULL;
  p->length=0;
}

static int addSetting(struct SquareScan* pScan, struct Setting* pSetting)
{
  if (pScan->length==0)
  {
    if (pScan->settingList!=NULL)
    {
      return -1;
    }
    pScan->settingList = (struct Setting*)malloc(sizeof(struct Setting));
    if (pScan->settingList==NULL)
    {
      return -1;
    }
    memcpy(pScan->settingList,pSetting,sizeof(struct Setting));
    pScan->length = 1;
    return 0;
  }
  else
  {
    if (pScan->settingList==NULL)
    {
      return -1;
    }
    pScan->settingList = (struct Setting*)realloc((void*)pScan->settingList,sizeof(struct Setting)*(pScan->length+1));
	
    if (pScan->settingList==NULL)
    {
      return -1;
    }
    memcpy(&(pScan->settingList[pScan->length]),pSetting,sizeof(struct Setting));
    ++pScan->length;
    return 0;
  }
}


static void scanSquare(struct Square *p, struct SquareScan* pScan)
{
  int i,j,k,m,f,value,finalValues,valuesSet, mask;
  int columnCount[SQUARE_SIZE][SQUARE_SIZE];
  int rowCount[SQUARE_SIZE];
  int minFreedom = SQUARE_SIZE;
  int r,c;
  unsigned int valueArray[SQUARE_SIZE];
  struct Square local;
  struct Setting setting;
  int numValuesToSet;
  /*
  printf("--------------------------------\n");
  */
  finalValues=0;
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
          clearScan(pScan);
          pScan->result = -1;
          return;
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
    numValuesToSet = getValuesToSet(rowCount,valueArray);
	/*
    printf("Row %i: ",i);fflush(stdout);
    for (m=0;m<SQUARE_SIZE;++m) printf("%i",valueArray[m]);fflush(stdout);
    printf("\n");fflush(stdout);
	*/
    for (m=0;m<numValuesToSet;++m)
    {
      value = valueArray[m];
      for (k=0;k<SQUARE_SIZE;++k)
      {
        mask = 1<<(value-1);
        if ((p->data[i][k] & mask) && (p->data[i][k]!=mask))
        {
          printf("Found value %i at position [%i][%i]\n",value,i,k);fflush(stdout);
          setting.value = value;
          setting.row = i;
          setting.column = k;
          if (addSetting(pScan,&setting))
          {
            clearScan(pScan);
            pScan->result = -1;
            return;
          }
        }
      }
    }
  }
  if (finalValues==SQUARE_SIZE*SQUARE_SIZE)
  {
    clearScan(pScan);
    pScan->result = 0;
    return;
  }
  for (i=0;i<SQUARE_SIZE;++i)
  {
    getValuesToSet(columnCount[i],valueArray);
	/*
    printf("Column %i: ",i);
    for (m=0;m<SQUARE_SIZE;++m) printf("%i",valueArray[m]);
    printf("\n");
	*/
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
            setting.value = value;
            setting.row = k;
            setting.column = i;
            if (addSetting(pScan,&setting))
            {
              clearScan(pScan);
              pScan->result = -1;
              return;
            }
          }
        }
      }
    }
  }
  if (pScan->length>0)
  {
    pScan->result = 1;
    return;
  }
  pScan->result = 2;
  for (i=0;i<SQUARE_SIZE;++i)
  {
    if (p->data[r][c] & (1<<i))
	{
	  setting.value = i+1;
      setting.row = r;
      setting.column = c;
	  printf("candidate value %i at [%i][%i] with degree %d\n",i+1,r,c,getFreedom(p->data[r][c]));
      if (addSetting(pScan,&setting))
      {
        clearScan(pScan);
        pScan->result = -1;
        return;
      }
	}
  }
}


int recursiveSolve(struct Square* pSquare)
{
  struct SquareScan scan = {-1,0,NULL};
  struct Square local;
  int i, loop;
  for (loop=1;loop;)
  {
    printSquare(pSquare,NULL);
	scanSquare(pSquare,&scan);
	/*
	printf("res==%i\n",scan.result);fflush(stdout);
	*/
    switch (scan.result)
    {
      case -1:
        clearScan(&scan);
        return -1;
      break;
      case 0:
        clearScan(&scan);
		printf("Yay!\n");
        return 0;
      break;
      case 1:
        for (i=0;i<scan.length;++i)
        {
          setValue(pSquare,scan.settingList[i].value,scan.settingList[i].row,scan.settingList[i].column);
        }
        clearScan(&scan);
      break;
      case 2:
		  for (i=0;i<scan.length;++i)
		  {
			memcpy(&local,pSquare,sizeof(struct Square));
			printf("Let's try value %i at [%i][%i] ... ",scan.settingList[i].value,scan.settingList[i].row,scan.settingList[i].column);
			setValue(&local,scan.settingList[i].value,scan.settingList[i].row,scan.settingList[i].column);
			/*printSquare(pSquare,NULL);fflush(stdout);*/
			if (recursiveSolve(&local)==0)
			{
			  printf("yessss!!!\n");
			  memcpy(pSquare,&local,sizeof(struct Square));
			  /*printSquare(pSquare,NULL);*/
			  return 0;
			}
			else
			  printf("no, didn't work, let's try another\n");
			
		  }
		  printf("... I don't know ....\n");
		  return -1;
      break;
      default:
        clearScan(&scan);
        return -1;
      break;
    }
  }
  return -1;
}


int solve(int* clues)
{
  int result;
  struct Square s;
  initialize(&s);
  applyClueArray(clues,&s);
  result = recursiveSolve(&s);
  if (result==0)
  {
    printSquare(&s,clues);
  }
  return result;
}




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
	solve(clues);
    
  return 0;
}
