#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int compare(const void* p1, const void* p2)
{
  int a,b;
  a = *((int*)p1);
  b = *((int*)p2);
  if (a==b)
    return 0;
  if (a<b)
    return -1;
  return 1;
}

struct sortedEntry_t
{
  int* values;
  int first;
  int mallocLength;
  int length;
  struct sortedEntry_t* prev;
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

static void merge(struct sortedEntry_t* dest, struct sortedEntry_t* src)
{
  struct sortedEntry_t* temp;
  int i,last,x,count,newLength,searchSize;
  void* searchPtr;
  void* sortPtr;
  void *keyPtr;
  int upperLimit;
  if (dest==NULL)
    return;
  if (src==NULL)
    return;
  last = dest->values[dest->first + dest->length-1];
  upperLimit = src->first + src->length - 1;
  for (i=src->first;i<upperLimit;i++)
  {
    x = src->values[i];
    keyPtr = (void*)(&x);
    searchPtr = (void*)(&(dest->values[dest->first]));
    searchSize = dest->length;
    newLength = dest->mallocLength+1;
    if (x<=last)
    {
      src->first++;
      src->length--;
      if (bsearch(keyPtr,searchPtr,searchSize,sizeof(int),compare)==NULL)
      {
        dest->values = (int*)realloc((void*)(dest->values),newLength*sizeof(int));
        if (dest->values)
        {
          dest->values[newLength-1]=x;
          dest->mallocLength++;
          dest->length++;
        }
      }
    }
    else
    {
      sortPtr = (void*)(&dest->values[dest->first]);
      qsort(sortPtr,dest->length,sizeof(int),compare);
      return;
    }
  }
}

static struct sortedEntry_t* addEntry(struct sortedEntry_t* p, int *ptr, int size)
{
  int i,j;
  struct sortedEntry_t* last,*temp;
  last = getTail(p);
  temp = (struct sortedEntry_t*)malloc(sizeof(struct sortedEntry_t));
  if (temp)
  {
    temp->values = (int*)malloc(size*sizeof(int));
    if (temp->values==NULL)
    {
      free(temp);
      return NULL;
    }
    temp->mallocLength=size;
    temp->length=size;
    temp->first=0;
    temp->next=NULL;
    memcpy((char*)temp->values,(char*)ptr,size*sizeof(int));
    if (last==NULL)
    {
      temp->prev=NULL;
      return temp;
    }
    else
    {
      last->next = temp;
      temp->prev = last;
      return p;
    }
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

#define MAX_LEVELS 10
static struct sortedEntry_t* sortedList=NULL;
static int* table[MAX_LEVELS];

static void printList(struct sortedEntry_t* p)
{
  int i;
  for (;p;p=p->next)
  {
    for (i=p->first;i<p->length;i++)
    {
      printf("%i,",p->values[i]);
    }
    printf("\n");
  }
}

int* buildRow(int** pTable, struct sortedEntry_t** pp, int index)
{
  int i;
  int* result;
  int count;
  int length;
  int *row;
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
    pTable[0]=(int*)malloc(sizeof(int));
    *(pTable[0])=1;
    *pp = addEntry(*pp,table[0],1);
    printList(*pp);
    return pTable[0];
  }
  row = buildRow(table,pp,index-1);
  length = 1 << index;
  result = (int*)malloc(length*sizeof(int));
  if (result)
  {

    for (i=0,count=0;i<length/2;i++)
    {
      result[count++]=2*row[i]+1;
      result[count++]=3*row[i]+1;
    }
    qsort((void*)result,length,sizeof(int),compare);

    printf("Row %i: ",index);
    for (i=0;i<length;i++) printf("%i,",result[i]);
    printf("\n");
  }
  *pp = addEntry(*pp,result,length);
  temp = getTail(*pp);
  merge(temp->prev,temp);
  table[index]=result;
  printf("Lst: ");
  printList(*pp);
  return result;
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
    atexit(clearAll);
  }
}


static int requiredTableRow(int n)
{
  int length,result;
  if (n<0)
    return -1;
  if (n==0)
    return 0;
  for (result=0,length=1;;)
  {
    n = n - length;
    result++;
    length = length*2;
    if (n<length)
      return result;
  }
  return result;
}

static int addMargin(int index)
{
  if (index<4)
    return index;
  return 2*index-1;
}

static int findValue(struct sortedEntry_t* p, int position)
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

int dblLinear(int n)
{
  int pos,i,len;
  struct sortedEntry_t* temp;
  _init();
  pos = addMargin(requiredTableRow(n));
  buildRow(table,&sortedList,pos);
  len = getLength(sortedList);

  return findValue(sortedList,n);
}


int main(int argc, char* argv[])
{
  printf("%i\n",dblLinear(50));

  return 0;
}
