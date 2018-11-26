#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
/*

 gcc twice-linear.c -o twice-linear.out && ./twice-linear.out
*/
#define MAX_LEVELS 40
typedef unsigned long long twiceLinearInt_t;

static int compare(const void* p1, const void* p2)
{
  twiceLinearInt_t a,b;
  a = *((twiceLinearInt_t*)p1);
  b = *((twiceLinearInt_t*)p2);
  if (a==b)
    return 0;
  if (a<b)
    return -1;
  return 1;
}

static twiceLinearInt_t* mergeRows(twiceLinearInt_t* first, int len1, twiceLinearInt_t* second, int len2)
{
  int i;

  twiceLinearInt_t* result = (twiceLinearInt_t*)malloc((len1+len2)*sizeof(twiceLinearInt_t));

  if (result)
  {
    for (i=0;i<len1;i++)
    {
      result[i]=first[i];
    }
    for (i=0;i<len2;i++)
    {
      result[len1+i]=second[i];
    }
    qsort((void*)result,len1+len2,sizeof(twiceLinearInt_t),compare);

    return result;
  }
  else
  {
    printf("ERROR: malloc in mergeRows\n");
    return NULL;
  }
}


static twiceLinearInt_t* buildTable(twiceLinearInt_t** pTable, int index)
{
  int i,length;
  size_t count;
  twiceLinearInt_t *result, *row;
  struct sortedEntry_t* temp;
  if (index<0)
  {
    printf("ERROR: negative index\n");
    return NULL;
  }
  if (index>=MAX_LEVELS)
  {
    printf("ERROR: index exceeding MAX_LEVELS\n");
    return NULL;
  }
  if (pTable[index])
    return pTable[index];
  if (index==0)
  {
    pTable[0]=(twiceLinearInt_t*)malloc(sizeof(twiceLinearInt_t));
    *(pTable[0])=1ULL;
    return pTable[0];
  }
  row = buildTable(pTable,index-1);
  length = 1ULL << index;
  result = (twiceLinearInt_t*)malloc(length*sizeof(twiceLinearInt_t));
  if (result)
  {
    for (i=0,count=0;i<length/2;i++)
    {
      result[count++]=(twiceLinearInt_t)2*row[i]+(twiceLinearInt_t)1;
      result[count++]=(twiceLinearInt_t)3*row[i]+(twiceLinearInt_t)1;
    }
    qsort((void*)result,count,sizeof(twiceLinearInt_t),compare);
  }
  else
  {
    printf("ERROR: second malloc failed in buildTable\n");
    return NULL;
  }
  pTable[index]=result;
  return result;
}

static twiceLinearInt_t* table[MAX_LEVELS];
static twiceLinearInt_t* twiceLinearArray;

static void clearAll()
{
  int i;
  if (twiceLinearArray)
    free(twiceLinearArray);
  for (i=0;i<MAX_LEVELS;i++)
  {
    if (table[i])
    {
      free(table[i]);
      table[i]=NULL;
    }
  }
}

static void _init()
{
  int i,len1,len2;
  twiceLinearInt_t* temp;
  static int firstTime=1;
  if (firstTime)
  {
    firstTime=0;
    twiceLinearArray=NULL;
    for (i=0;i<MAX_LEVELS;i++)
    {
      table[i]=NULL;
    }
    buildTable(table,20);
    for (temp=table[0],len1=1,i=1;table[i];i++)
    {
      len2=(1<<i);
      twiceLinearArray = mergeRows(temp,len1,table[i],len2);
      if (i>1)
        free(temp);
      temp = twiceLinearArray;
      len1 = len1+len2;
    }
    atexit(clearAll);
  }
}

void printTable(twiceLinearInt_t** pTable)
{
  int i,j,length;
  for (i=0;i<MAX_LEVELS;i++)
  {
    if (pTable[i]==NULL)
      return;
    length = 1ULL<<i;
    printf("Row %i: ",i);
    for (j=0;j<length;j++)
    {
      printf("%llu,",pTable[i][j]);
    }
    printf("\n");
  }
}

twiceLinearInt_t dblLinear(int n)
{
  int row,i,offset;
  twiceLinearInt_t* temp,last;
  twiceLinearInt_t retVal;
  static int f=1;
  int repeatedLength = 0;
  _init();
  for (i=0;;)
  {
    for (offset=0;i<=n;i++)
    {
      if ((i>0) & (twiceLinearArray[i]==twiceLinearArray[i-1]))
      {
        offset++;
      }
    }
    if (offset)
      n+=offset;
    else
      break;
  }
  return twiceLinearArray[n+offset];
}


int main(int argc, char* argv[])
{
  int i,L;
  int test[]={10,20,30,50,100,101,102,103,104};
  L = sizeof(test)/sizeof(int);
  for (i=0;i<L;i++)
  {
    printf("dblLinear(%i)==%llu\n",test[i],dblLinear(test[i]));

  }
/*
  printTable(table);

*/







  return 0;
}
