#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
cd /cygdrive/c/Users/gcarneva/Documents/Personal/C-test/Codewars/superlong
 gcc twice-linear.c -o twice-linear.out && ./twice-linear.out
*/
#define MAX_LEVELS 40
typedef unsigned long long twiceLinearInt_t;

static int compare(const void* p1, const void* p2)
{
  int a,b;
  a = *((twiceLinearInt_t*)p1);
  b = *((twiceLinearInt_t*)p2);
  if (a==b)
    return 0;
  if (a<b)
    return -1;
  return 1;
}

struct sortedEntry_t
{
  twiceLinearInt_t* values;
  int first;
  int mallocLength;
  int length;
  struct sortedEntry_t* next;
};

static struct sortedEntry_t* getTail(struct sortedEntry_t* p)
{
  for (;p;p=p->next)
  {
    if (p->next==NULL)
      return p;
  }
  return NULL;
}

static int getLength(struct sortedEntry_t* p)
{
  int result;
  for (result=0;p;p=p->next) result++;
  return result;
}

static void mergeEntries(struct sortedEntry_t* dest, struct sortedEntry_t* src)
{
  struct sortedEntry_t* temp;
  int i,count,newLength,searchSize;
  twiceLinearInt_t destLast,sourceVal;
  void* searchPtr;
  void* sortPtr;
  void *keyPtr;
  int upperLimit;
  if (dest==NULL)
    return;
  if (src==NULL)
    return;
  for (;;)
  {
    sourceVal = src->values[src->first];
    destLast = dest->values[dest->first + dest->length-1];
    if (sourceVal>destLast)
    {
      return;
    }
    else
    {
      keyPtr = (void*)(&sourceVal);
      searchPtr = (void*)(&(dest->values[dest->first]));
      searchSize = dest->length;
      if (bsearch(keyPtr,searchPtr,searchSize,sizeof(twiceLinearInt_t),compare)==NULL)
      {
        newLength = dest->mallocLength+1;
        dest->values = (twiceLinearInt_t*)realloc((void*)(dest->values),newLength*sizeof(twiceLinearInt_t));
        if (dest->values)
        {
          dest->values[newLength-1]=sourceVal;
          dest->mallocLength++;
          dest->length++;
          sortPtr = (void*)(&dest->values[dest->first]);
          qsort(sortPtr,dest->length,sizeof(twiceLinearInt_t),compare);
        }
        else
        {
          printf("ERROR: realloc failed in mergeEntries\n");
          return;
        }
      }
      src->first++;
      src->length--;
    }
  }
}

static void rearangeList(struct sortedEntry_t* p)
{
  struct sortedEntry_t* first;
  if (p==NULL)
    return;
  for (;p->next;p=p->next)
  {
    mergeEntries(p,p->next);
  }
  return;
}

static struct sortedEntry_t* addEntry(struct sortedEntry_t* p, twiceLinearInt_t *ptr, int size)
{
  int i,j;
  struct sortedEntry_t* last,*temp;
  last = getTail(p);
  if (size<0)
    return NULL;
  if (size==0)
    return p;
  temp = (struct sortedEntry_t*)malloc(sizeof(struct sortedEntry_t));
  if (temp)
  {
    temp->values = (twiceLinearInt_t*)malloc(size*sizeof(twiceLinearInt_t));
    if (temp->values==NULL)
    {
      printf("ERROR: second malloc failed in addEntry\n");
      free(temp);
      return NULL;
    }
    temp->mallocLength=size;
    temp->length=size;
    temp->first=0;
    temp->next=NULL;
    for (i=0;i<size;i++)
    {
      temp->values[i]=ptr[i];
    }
    if (last==NULL)
    {
      if (p)
        printf("ERROR: expected null list\n");
      return temp;
    }
    else
    {
      last->next = temp;
      mergeEntries(last,temp);
      return p;
    }
  }
  else
  {
    printf("ERROR: first malloc failed in addEntry\n");
    return NULL;
  }
  return p;
}

static struct sortedEntry_t* clearList(struct sortedEntry_t* p)
{
  struct sortedEntry_t* temp;
  for (;p;)
  {
    temp = p->next;
    free(p);
    p=temp;
  }
}

#define ENABLE_LIST_PRINT
static void printList(struct sortedEntry_t* p)
{
#ifdef ENABLE_LIST_PRINT
  int i;
  int s=0;
  if (p)
    printf("--------------------------------------\n");
  for (;p;p=p->next)
  {
    printf("Lst %i: from %i, %i values, %i locations: ",s++,p->first,p->length,p->mallocLength);
    for (i=p->first;i<p->first+p->length;i++)
    {
      printf("%i,",p->values[i]);
    }
    printf("\n");
  }
  printf("--------------------------------------\n");
#endif
}

static struct sortedEntry_t* buildList(twiceLinearInt_t** pTable, struct sortedEntry_t* p)
{
  int n,len,i,length;
  for (n=0;;n++)
  {
    if (pTable[n]==NULL)
      break;
  }
  len = getLength(p);

  if (n == len)
    return p;
  for (i=len;i<n;i++)
  {
    length = 1ULL << i;
    p = addEntry(p,pTable[i],length);

  }
  return p;
}

static twiceLinearInt_t* buildRow(twiceLinearInt_t** pTable, int index)
{
  int i,count,length;
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
  row = buildRow(pTable,index-1);
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
    printf("ERROR: second malloc failed in buildRow\n");
    return NULL;
  }
  pTable[index]=result;
  return result;
}


static int requiredTableRow(int n,int *pRemainder)
{
  int length,result;
  if (n<0)
  {
    *pRemainder=-1;
    return -1;
  }
  if (n==0)
  {
    *pRemainder=0;
    return 0;
  }
  for (result=0,length=1;;)
  {
    n = n - length;
    result++;
    length = length*2;
    if (n<length)
    {
      *pRemainder=n;
      return result;
    }
  }
  return result;
}

static int addMargin(int index)
{
  switch (index)
  {
    case 0: return 0;
    case 1: return 1;
    case 2: return 2;
    case 3: return 4;
    case 4: return 5;
    case 5: return 7;
    case 6: return 9;
    case 7: return 10;
    case 8: return 12;
    case 9: return 13;
    case 10: return 15;
    case 11: return 17;
    case 12: return 18;
    default:printf("ERROR: too many rows\n");
  }
  return -1;
}

static twiceLinearInt_t findValue(struct sortedEntry_t* p, int position)
{
  int lower, upper;
  for (;p;)
  {
    if (position<p->length)
    {
      return p->values[position+p->first];
    }
    else
    {
      position = position - p->length;
      p = p->next;
    }
  }
  return -1;
}

static struct sortedEntry_t* sortedList=NULL;
static twiceLinearInt_t* repetitionArray=NULL;
static twiceLinearInt_t* table[MAX_LEVELS];
static int repetitionCount = 0;
static twiceLinearInt_t* addRepetition(twiceLinearInt_t* ptr, twiceLinearInt_t value,int *pSize)
{
  if (ptr==NULL)
  {
    ptr = (twiceLinearInt_t*)malloc(sizeof(twiceLinearInt_t));
    if (ptr)
    {
      *ptr=value;
      *pSize=1;
      printf("first\n");
    }
    return ptr;
  }
  else
  {
    ptr = (twiceLinearInt_t*)realloc(ptr,(*pSize+1)*sizeof(twiceLinearInt_t));
    if (ptr)
    {
      ptr[*pSize]=value;
      *pSize++;
      printf("the %i\n",*pSize);
    }
    return ptr;
  }
}

static twiceLinearInt_t* findRepetitions(twiceLinearInt_t** pTable, twiceLinearInt_t* ptr,int *pSize)
{
  int i,j,k,length,searchLength;
  void* searchPtr;
  void* keyPtr;
  twiceLinearInt_t val;
  printf("Repetitions: ");
  for (i=0;pTable[i];i++)
  {
    searchLength = 1ULL<<i;

    for (j=i+1;pTable[j];j++)
    {
      length = 1ULL<<j;

      for (k=0;k<length;k++)
      {
        val = pTable[j][k];
        printf("pTable[%i][%i]==%i\n",j,k,val);
        if (val>pTable[i][searchLength-1])
          break;
        else
        {
          printf("found one \n");
          keyPtr = (void*)&val;
          searchPtr = (void*)pTable[i];
          if (bsearch(keyPtr,searchPtr,searchLength,sizeof(twiceLinearInt_t),compare))
          {
            ptr = addRepetition(ptr,val,pSize);
          }
        }
      }
    }
  }
  printf("\n");
}

static void clearAll()
{
  int i;
  for (i=0;i<MAX_LEVELS;i++)
  {
    if (table[i])
    {
      free(table[i]);
      table[i]=NULL;
    }
  }
  clearList(sortedList);
}

static void _init()
{
  int i;
  int val=1;
  static int firstTime=1;
  if (firstTime)
  {
    firstTime=0;
    for (i=0;i<MAX_LEVELS;i++)
    {
      table[i]=NULL;
    }
    repetitionCount=0;
    repetitionArray=NULL;
    atexit(clearAll);
  }
}

static twiceLinearInt_t findElement(int n,twiceLinearInt_t** pTable, twiceLinearInt_t* ptr, int size)
{
  int i,j,length;
  int result,count;
  twiceLinearInt_t value;
  for (i=0;pTable[i];i++)
  {
    length = 1ULL<<i;
    if (n<length)
    {
      break;
    }
    else
    {
      n = n-length;
    }
  }

  printf("looking at row %i, position %i\n",i,n);
  value = pTable[i][n];

  for (count=0,j=0;j<size;j++)
  {
    if (value>=ptr[j])
      count++;
    else
      break;
  }
  printf("count==%i\n",count);
  return 0;
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
      printf("%i,",pTable[i][j]);
    }
    printf("\n");
  }
}

static twiceLinearInt_t dblLinear(int n)
{
  int pos,i,len,index;
  struct sortedEntry_t* temp;
  _init();
  pos = requiredTableRow(n,&index);
  buildRow(table,pos);

  repetitionArray=findRepetitions(table,repetitionArray,&repetitionCount);

  printf("table[%i][%i]==%d\n",pos,index,table[pos][index]);

  /*
  return findElement(n,table,repetitionArray,repetitionCount);
  */

  return 0;
}





int main(int argc, char* argv[])
{
  int i,L;
  int test[]={10,20,30,50,60};
  L = sizeof(test)/sizeof(int);
  for (i=0;i<L;i++)
  {
    printf("dblLinear(%i)==%llu\n",test[i],dblLinear(test[i]));
    printf("fdsfd\n");
  }


  printTable(table);
/*
  printTable(table);
*/



  return 0;
}
