/***************************************************************************
Written by Gianmarco Carnevale



*/
#if 0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <stdarg.h>
#include <math.h>
/* we will define the super long type as
   a doubled liked list of 32-bit integers */
typedef struct _superlong
{
  uint32_t val;
  struct _superlong* next;
  struct _superlong* prev;
} superlong_t;

void superPrint(superlong_t* p)
{
  if (p==NULL)
    printf("<NULL POINTER>");
  else
  {
    printf("%X",p->val);
    p=p->next;
    for (;p;p=p->next)
    {
      printf("%08X",p->val);
    }
  }
}

superlong_t* getTail(superlong_t* p)
{
  if (p==NULL)
    return NULL;
  for (;p->next;p=p->next);
  return p;
}

superlong_t* getHead(superlong_t* p)
{
  if (p==NULL)
    return NULL;
  for (;p->prev;p=p->prev);
  return p;
}

void supercheck(superlong_t* p)
{
  superlong_t* temp;
  if (p==NULL)
  {
    printf("Null Pointer\n");
    return;
  }
  if (getHead(getTail(p))!=p)
    printf("ERROR: malformed list\n");
}

int superIsZero(superlong_t* p)
{
  if (p)
  {
    if ((p->next==NULL)&&(p->prev==NULL))
    {
      if (p->val==0)
        return 1;
      else
        return 0;
    }
    return 0;
  }
  else
  {
    printf("ERROR: nul pointer passed to superIsZero\n");
    return -1;
  }
  return 0;
}


uint32_t multiplicationWithCarry(uint32_t a, uint32_t b, uint32_t* carry)
{
  uint64_t p;
  uint32_t c;
  p = (uint64_t)a * (uint64_t)b;
  if (carry)
    *carry = (uint32_t)((p & 0xFFFFFFFF00000000ULL) >> 32);
  return (uint32_t)p;
}

uint32_t additionWithCarry(uint32_t a, uint32_t b, int* carry)
{
  uint32_t s;
  s = a + b;
  if (carry)
  {
    if (s<a)
      *carry = 1;
    else
      *carry = 0;
  }
  return s;
}

superlong_t* getZero(unsigned nWords)
{
  unsigned i;
  superlong_t* result;
  superlong_t *index, *previous;
  if (nWords > 0xFFFF)
  {
    printf("ERROR: that's too long\n");
    return NULL;
  }
  if (nWords==0)
  {
    printf("WARNING: getZero calld with zero words argument\n");
  }
  for (i=0,previous=NULL,result=NULL;i<nWords;i++)
  {
    index = (superlong_t*)malloc(sizeof(superlong_t));
    if (index)
    {
      if (previous)
      {
        /* if we created at least one element before
        then we connect it to the current one */
        previous->next = index;
      }
      if (i==0)
      {
        /* let's save the first pointer, that will be the result */
        result = index;
      }
      if (i==nWords-1)
      {
        /*we terminate the list, it's the last element */
        index->next=NULL;
      }
      index->prev = previous;
      index->val = 0;
      previous = index;
    }
  }
  return result;
}

superlong_t* getSuperlong(uint32_t n)
{
  superlong_t* result = (superlong_t*)malloc(sizeof(superlong_t));
  if (result)
  {
    result->val = n;
    result->prev = NULL;
    result->next = NULL;
  }
  return result;
}

void superDelete(superlong_t* p)
{
  superlong_t* index;
  for (;p;)
  {
    index = p->next;
    free(p);
    p = index;
  }
}

unsigned getLength(superlong_t* p)
{
  unsigned i;
  for (i=0;p;p=p->next,i++);
  return i;
}

superlong_t* shiftLeftByWords(superlong_t* p, int shift)
{
  superlong_t* result, *index, *previous, *pZero;
  int i;
  if (p)
  {
    if (shift==0)
      return p;
    index = getTail(p);
    /* now index points to the last structure in the list */
    if (shift>0)
    {
      pZero = getZero(shift);
      if (pZero)
      {
        index->next = pZero;
        pZero->prev = index;
      }
      return p;
    }
    else
    {
      /* shift surely is negative now */
      shift = -shift;
      if (shift>=getLength(p))
      {
        superDelete(p);
        return getZero(1);
      }

      for (i=0;i<shift;i++)
      {
        previous = index->prev;
        if (previous)
        {
          free(index);
          previous->next=NULL;
          index = previous;
        }
        else
        {
          /* this is the last element of the list,
             let's not delete it like the others but
             let's zero the value and exit */
          index->val = 0;
          index->next = NULL;
          return p;
        }
      }
    }
    return p;
  }
  else
    return NULL;
}

superlong_t* superCopy(superlong_t*p)
{
  superlong_t* result,*index;
  result = getZero(getLength(p));
  if (result)
  {
    for (index=result;p && index;)
    {
      index->val = p->val;
      index = index->next;
      p = p->next;
    }
  }
  return result;
}

superlong_t* superAdd(superlong_t* a, superlong_t* b)
{
  unsigned n1, n2, temp;
  superlong_t* result, *index1, *index2, *index3;
  uint32_t sum;
  int carry,prevcarry;

  if (a==NULL)
    return NULL;
  if (b==NULL)
    return NULL;

  n1 = getLength(a);
  n2 = getLength(b);

  if (n2 > n1)
  {
    /* we want to keep the first pointer 'a'
      as the pointer to the longer superlong
      so we swap 'a' and 'b' */
    index1 = a;
    a = b;
    b = index1;
    temp = n1;
    n1 = n2;
    n2 = temp;
  }
  /* now a points to the longer list
     so  n1 >= n2 */
  result = getZero(n1);/* large enough to contain the longest */
  if (result)
  {
    /*
      index1 is the pointer to the first number, the longest
      index1 is the pointer to the second number
      index3 is the pointer to the result
    */
    index1 = getTail(a);
    index2 = getTail(b);/* this is the shorter */
    index3 = getTail(result);/* as long as the a list */
    for (prevcarry=0;index2;)
    {
      sum = additionWithCarry(index1->val, index2->val, &carry);
      if (prevcarry)
        sum++;
      index3->val = sum;
      prevcarry=carry;
      index1=index1->prev;
      index2=index2->prev;
      index3=index3->prev;
    }
    /*
    we're out of the loop,
    we processed all the words from the second number
    let's check whether the first number still
    has words to be processed with the carry
    */

    for (prevcarry=carry;index1;)
    {
      if (prevcarry)
      {
        index3->val = additionWithCarry(index1->val, 1, &carry);
      }
      else
      {
        index3->val = index1->val;
        carry = 0;
      }
      prevcarry=carry;
      index1 = index1->prev;
      index3 = index3->prev;
    }
    if (prevcarry)
    {
      index3 = getSuperlong(1);
      index3->next = result;
      result->prev = index3;
      return index3;
    }
    else
      return result;
  }
  return result;
}

superlong_t* Multiply(superlong_t* p, uint32_t n)
{
  uint32_t prevcarry, carry1, carry2, prod;
  superlong_t* result,*index1,*index2;
  unsigned len;
  if (p==NULL)
    return NULL;
  len = getLength(p);
  result = getZero(len);
  if (result)
  {
    index1=getTail(p);
    index2=getTail(result);
    for (prevcarry=0;index1;)
    {
      prod = multiplicationWithCarry(index1->val,n,&carry1);
      index2->val = additionWithCarry(prod,prevcarry,&carry2);
      prevcarry = carry2+carry1;
      index1=index1->prev;
      index2=index2->prev;
    }
    if (prevcarry)
    {
      index2 = getSuperlong(prevcarry);
      index2->next = result;
      result->prev = index2;
      return index2;
    }
    else
    {
      return result;
    }
  }
  return result;
}

superlong_t* superMultiply(superlong_t* a, superlong_t* b)
{
  superlong_t* result, *index;
  superlong_t** sumTable;
  superlong_t** prodTable;
  unsigned prodLength, sumLength;
  unsigned i,j;
  unsigned n1, n2;
  if (a==NULL)
    return NULL;
  if (b==NULL)
    return NULL;

  if (superIsZero(a))
    return getZero(1);
  if (superIsZero(b))
      return getZero(1);

  n1 = getLength(a);
  n2 = getLength(b);
  prodLength = n2;
  prodTable = (superlong_t**)malloc(prodLength*sizeof(superlong_t**));
  if (prodTable)
  {
    index = getTail(b);
    for (i=0;i<prodLength;i++,index=index->prev)
    {
      prodTable[i] = Multiply(a,index->val);
      prodTable[i] = shiftLeftByWords(prodTable[i],i);
    }
    if (prodLength>1)
    {
      sumLength = prodLength - 1;
      sumTable = (superlong_t**)malloc(sumLength*sizeof(superlong_t**));
      if (sumTable)
      {
        sumTable[0] = superAdd(prodTable[0],prodTable[1]);
        for (i=1;i<sumLength;i++)
        {
          sumTable[i] = superAdd(sumTable[i-1],prodTable[i+1]);
        }
        result = sumTable[sumLength-1];
        for (i=0;i<sumLength-1;i++)
          superDelete(sumTable[i]);
        for (i=0;i<prodLength;i++)
          superDelete(prodTable[i]);
        free(prodTable);
        free(sumTable);
        return result;
      }
    }
    else
    {
      result = prodTable[0];
    }
    return result;
  }
  return NULL;
}


static size_t getDecimalDigits(unsigned long long n)
{
  size_t i;
  if (n==0)
    return 1;
  for (i=0;n;i++,n/=10);
  return i;
}

static char* stringTrim(char* s)
{
  int i;
  char* result;
  int n = strlen(s);
  if (n==1)
    return s;
  for (i=0;i<n;i++)
  {
    if (s[i]!=(char)0)
      continue;
    else
      break;
  }
  if (i>0)
  {
    result = (char*)malloc(n*sizeof(char));
    if (result)
    {
      strcpy(result,&s[i]);
      free(s);
    }
    return result;
  }
}

superlong_t* getSuperLongFromVaList(int n,...)
{
  int i;
  superlong_t* result;
  uint32_t value;
  superlong_t* temp1,*temp2;
  va_list vl;
  va_start(vl,n);
  for (i=0,result=NULL;i<n;i++)
  {
    if (i==0)
    {
      value = va_arg(vl,uint32_t);
      result = getSuperlong(value);
      if (result==NULL)
        return NULL;
    }
    else
    {
      value = va_arg(vl,uint32_t);
      temp1 = shiftLeftByWords(result,1);
      temp2 = getSuperlong(value);
      result = superAdd(temp1,temp2);
      superDelete(temp1);
      superDelete(temp2);
    }
  }
  va_end(vl);
  return result;
}

int superCompare(superlong_t* a, superlong_t* b)
{
  unsigned n1, n2;
  int i;
  n1 = getLength(a);
  n2 = getLength(b);
  if (n1<n2)
    return -1;
  if (n1>n2)
    return 1;
  for (;a && b;)
  {
    if (a->val < b-> val)
      return -1;
    if (a->val > b-> val)
        return 1;
    a = a->next;
    b = b->next;
  }
  return 0;
}

superlong_t* superSubtraction(superlong_t* a, superlong_t* b)
{
  superlong_t* index1,*index2,*index3,*result,*padding,*first,*second,*temp;
  int borrow,c;
  uint32_t lastValue;
  unsigned n1, n2;
  if (a==NULL)
    return NULL;
  if (b==NULL)
    return NULL;
  if (superIsZero(b))
    return superCopy(a);
  c = superCompare(a,b);
  if (c<0)
  {
    printf("ERROR: first superlong is less than the second\n");
    return NULL;
  }
  if (c==0)
  {
    return getZero(1);
  }

  n1 = getLength(a);
  n2 = getLength(b);
  /* n1>=n2 for sure */

  result = getZero(n1);
  padding = NULL;
  /* we allocate the result list long as the first number */
  if (n1>n2)
  {
    padding = getZero(n1-n2);
    if (padding)
    {
      second = superCopy(b);
      padding = getTail(padding);
      padding->next = second;
      second->prev = padding;
      second = getHead(second);
    }
    else
    {
      printf("ERROR: failed to allocate padding\n");
      return NULL;
    }
  }
  else
  {
    second = b;
  }

  index1 = getTail(a);
  index2 = getTail(second);
  index3 = getTail(result);
  /*
    index1 is the pointer to the first number
    index2 is the pointer to the second number
    index3 is the pointer to the result

    index1, index2 and index3 are pointers to
    three lists which have the same length
  */

  for (borrow=0;index1 && index2 && index3;index1 = index1->prev,index2 = index2->prev,index3 = index3->prev)
  {
    if (borrow)
    {
      /* there is a borrow from the words processed before */
      if (index1->val < index2->val + 1)
        borrow = 1;
      else
        borrow = 0;
      index3->val = index1->val - (index2->val + 1);
    }
    else
    {
      /* no borrow, normal subtraction */
      if (index1->val < index2->val)
        borrow = 1;
      else
        borrow = 0;
      index3->val = index1->val - index2->val;
    }
  }
  /* now we look for padding words in the result
  and we delete them */
  for (index3 = getHead(result);;)
  {
    if (index3->val == 0)
    {
      temp = index3->next;
      temp->prev = NULL;
      free(index3);
      index3 = temp;
    }
    else
      return index3;
  }
  return result;
}

superlong_t* superLeftShift(superlong_t* p, int nBits)
{
  int i,j,n;
  superlong_t* index, *result;
  uint32_t mask, selectMask, nextcarry, prevcarry,carry;
  if (p==NULL)
    return NULL;
  if (superIsZero(p))
      return getZero(1);
  n = nBits/32;
  if (n)
  {
    p = shiftLeftByWords(p,n);
  }
  nBits = nBits%32;

  if (nBits == 0)
    return p;
  /* now the absolute value of nBits will be less than 32 */
  if (nBits > 0)
  {
    mask =   0xFFFFFFFF << (32 - nBits);
    selectMask = ~(0xFFFFFFFF << nBits);

    index = getTail(p);
    carry = 0;
    for (;index;)
    {
      nextcarry = (index->val & mask) >> (32 - nBits);
      nextcarry = nextcarry & selectMask;
      index->val = index->val << nBits;
      index->val = index->val | carry;
      carry = nextcarry;
      index = index->prev;
    }
    /* now index == null */
    if (carry)
    {
      result = getZero(1);
      result->val = carry;
      result->next = p;
      p->prev = result;
      return result;
    }
    return p;
  }
  else
  {
    mask = ~(0xFFFFFFFF << (-nBits));
    selectMask = ~(0xFFFFFFFF << (32+nBits));
    index = p;
    carry = 0;
    for (;index;)
    {
      nextcarry = index->val & mask;
      nextcarry = nextcarry << (32 + nBits);
      index->val = index->val >> (-nBits);
      index->val = index->val & selectMask;
      index->val = index->val | carry;
      carry = nextcarry;
      index = index->next;
    }

    if (p->val == 0)
    {
      if (p->next)
      {
        result = p->next;
        result->prev = NULL;
        free(p);
        return result;
      }
      return p;
    }
    return p;
  }
}

int getBitSpan(superlong_t* p)
{
  int i;
  int result;
  if (p==NULL)
    return -1;
  result = 32 * (getLength(p) - 1);
  for (i=0;i<32;i++)
  {
    if ((p->val & (0xFFFFFFFF << i))==0)
      break;
  }
  return result + i;
}

superlong_t* superDivision(superlong_t* first, superlong_t* second, superlong_t** pRemaider)
{
  superlong_t* result=NULL;
  unsigned n1, n2, i,n;
  uint32_t attempt;
  superlong_t *dividend,*divisor,*quotient,*_quotient,*partial, *product, *rem,*diff;
  unsigned quotLength;
  uint64_t temp;
  unsigned s1, s2;

  superlong_t* pOne;
  unsigned shift;
  int c;
  if (first==NULL)
    return NULL;
  if (second==NULL)
    return NULL;
  if (superIsZero(second))
  {
    printf("ERROR: division by zero\n");
    return NULL;
  }
  if (superIsZero(first))
  {
    if (pRemaider)
      *pRemaider = getZero(1);
    return getZero(1);
  }

  c = superCompare(first,second);
  if (c<0)
  {
    if (pRemaider)
      *pRemaider = superCopy(first);
    return getZero(1);
  }
  else
  {
    if (c==0)
    {
      if (pRemaider)
        *pRemaider = getZero(1);
      quotient = getZero(1);
      quotient->val = 1;
      return quotient;
    }
    else
    {
      /* first is greater than second */
      dividend = superCopy(first);
      divisor = superCopy(second);
      s1 = getBitSpan(dividend);
      s2 = getBitSpan(divisor);
      n = s1-s2;
      /* surely s1>=s2*/
      divisor = superLeftShift(divisor,n);
      while (superCompare(dividend,divisor)<0)
      {
        /* we shift right the divisor in case
        we made it greater than the dividend with the previous left shift */
        divisor = superLeftShift(divisor,-1);
        n--;
      }

      for (quotient = getZero(1);;)
      {
        if (superCompare(divisor,second)<0)
        {
          /* the divisor has been shifted right so much
          that it became smaller than the original value i.e. second
          it means that we have finished the loop */
          break;
        }
        else
        {
          c = superCompare(dividend,divisor);
          /* we shift the quotient left bcause we are going to
          put its LSB to zero or to one depending on the comparison
          between dividend and divisor */
          quotient = superLeftShift(quotient,1);
          switch (c)
          {
            case -1:
              /* the dividend is smaller than the divisor
              so the result of this partial division is zero,
              already added to the quotient with the previous shift,
              therefore there is nothing more to do */
            break;
            case 0:
            case 1:
              getTail(quotient)->val |= 1;
              diff = superSubtraction(dividend,divisor);
              superDelete(dividend);
              dividend = diff;
              if (pRemaider)
                *pRemaider = dividend;
            break;
            default:
            {
              printf("ERROR: unhandled exception\n");
              return NULL;
            }
            break;
          }
          divisor = superLeftShift(divisor,-1);
        }
      }
    }
  }
  return quotient;
}

double superLog10(superlong_t* p)
{
  double result;
  unsigned n;
  int k;
  if (p==NULL)
    return NAN;
  printf("It's long %d words.\n",getLength(p));
  for (result=0.0; p && isfinite(result * (double)(0x100000000ULL)); p=p->next)
  {

    result = (result * (double)(0x100000000ULL)) + (double)p->val;
  }

  result = log10(result);
  if (p==NULL)
  {
    return result;
  }
  else
  {
    n = getLength(p);/* it must be equal or greater than 1 */

    result += 9.6329598612473982468396446311838 * n;
    return result;
  }
}

char* superConvertToString(superlong_t* num)
{
  unsigned n,i,printLen,j,k;
  char buffer[10];
  char* result;
  char c;
  superlong_t* rem;
  superlong_t* billion,*one;
  superlong_t* division,*p;
  p = superCopy(num);
  if (superIsZero(num))
  {
    /* it means that we are converting a superlong number which
    is equal to zero */
    result = (char*)malloc(2*sizeof(char));
    if (result)
    {
      result[0]='0';
      result[1]=(char)0;
    }
    return result;
  }
  n = (unsigned)superLog10(num)+1;/* 10E4==10000 so 4+1 digits */
  printf("log+1==%d\n",n);
  billion = getSuperlong(1000000000);
  result = (char*)malloc((n+1)*sizeof(char));/* one more character for the string termination */
  if (result && billion)
  {
    result[n]=(char)0;
#if 0
    printf("Allocated %d chars plus null\n",n);
#endif
    for (;;)
    {
      division = superDivision(p,billion,&rem);
      if (superIsZero(division))
        break;

      printLen = sprintf(buffer,"%llu",rem->val);
      if (printLen>9)
        printf("ERROR: more characters than expected!\n");
      n = n - 9;
#if 0
      printf("%llu, it took %i digits, ",rem->val,printLen);
#endif
      memmove(&buffer[9-printLen],&buffer[0],printLen);
      memset(&buffer[0],'0',9-printLen);
      buffer[9]=0;
#if 0
      printf("Padding done with %i zeros\n",9-printLen);
#endif
      memcpy(&result[n],buffer,9);

      superDelete(p);
      superDelete(rem);
      p = division;
    }
    printLen = sprintf(buffer,"%llu",rem->val);
#if 0
    printf("%llu, it took %i digits\n",rem->val,printLen);
#endif
    n = n - printLen;
    if (n!=0)
    {
      printf("ERROR: unexpected final position %i\n",n);
    }
    memcpy(&result[n],buffer,printLen);
    superDelete(billion);
    superDelete(division);
    superDelete(rem);
    superDelete(p);

  }

  return result;
}

superlong_t* superConvertFromString(char* s)
{
  unsigned n;
  int i;
  superlong_t* ten,*result,*prod,*sum,*digit;

  prod = getSuperlong(0);
  n = strlen(s);
  for (i=0;i<n;i++)
  {
    digit = getSuperlong((uint32_t)(s[i]-'0'));
    sum = superAdd(prod,digit);
    superPrint(sum);printf("==");superPrint(prod);printf("+");superPrint(digit);printf("\n");

    superDelete(digit);
    superDelete(prod);
    if (i==n-1)
    {

      return sum;
    }
    ten = getSuperlong(10);
    prod = superMultiply(sum,ten);
    superPrint(prod);printf("==");superPrint(sum);printf("*");superPrint(ten);printf("\n");
    superDelete(ten);
    superDelete(sum);
  }
  superDelete(prod);

  return sum;
}


#define NFACT 350
static superlong_t* tbl[NFACT+1];
static void clearAll()
{
  int i;
  for (i=0;i<NFACT+1;i++)
  {
    if (tbl[i])
      superDelete(tbl[i]);
  }
}

static void _init()
{
  int i;
  static int firstTime=1;
  if (firstTime)
  {
    firstTime=0;
    atexit(clearAll);
    for (i=0;i<NFACT+1;i++)
      tbl[i]=NULL;
    tbl[0]=getSuperlong(1);
    tbl[1]=getSuperlong(1);

    /*
    the principle is that tbl[x]==factorial(x)
    */
  }
}


int searchLast(int n)
{
  int i;
  if (n<2)
    return 1;
  if (n>NFACT)
    n=NFACT;
  for (i=n;i>0;i--)
  {
    if (tbl[i])
      return i;
  }
  return 1;
}

char *factorial(int n)
{
  char* result;
  superlong_t* p, *prod,*previous;
  int i;
  _init();
  if (n<0)
    return NULL;

  if ((n==0)||(n==1))
  {
    result = (char*)malloc(2*sizeof(char));
    if (result)
    {
      result[0]='1';
      result[1]=0;
    }
    return result;
  }

  i=searchLast(n);
  if (i==n)
    return superConvertToString(tbl[i]);

  if (tbl[i])
  {
    previous=tbl[i];
    i++;
  }

  for (;i<=n;i++)
  {

    prod = Multiply(previous,(uint32_t)i);
    previous = prod;
    if (i<=NFACT)
      tbl[i]=prod;
  }

  result = superConvertToString(prod);

  return result;
}



/************************************************************************************/
long long mySqrt(long long n)
{
    return (long long)(sqrt((double)n));
}



struct squareSum
{
  long long value;
  int N;
  long long* factors;
};

int setTrial(long long value, struct squareSum* p)
{
  if (p==NULL)
    return -1;

  if (value<1)
    return -1;

  if (p->factors)
  {
    p->factors = (long long*)realloc(p->factors,(p->N+1)*sizeof(long long));
    if (p->factors)
    {
      p->factors[p->N]=value;
      p->N++;
      return 0;
    }
    return -1;
  }
  else
  {
    p->factors = (long long*)malloc(sizeof(long long));
    if (p->factors)
    {
      p->factors[0]=value;
      p->N=1;
      return 0;
    }
    return -1;
  }
}

struct squareSum* firstAttempt(long long n)
{
  long long prev,value,square;
  int i;
  struct squareSum* res = (struct squareSum*)malloc(sizeof(struct squareSum));
  if (res==NULL)
  {
    printf("ERROR: couldn't allocate structure\n");
    return NULL;
  }
  res->value = n;
  res->N=0;
  res->factors=NULL;
  for (i=0,square=n*n,prev=-1;square>=0;i++)
  {
    if (i==0)
    {
      /* first iteration */
      value = n-1;
      if (setTrial(value,res))
      {
        printf("ERROR: setTrial returned error at the first iteration\n");
        free(res);
        return NULL;
      }

    }
    else
    {
      /* subsequent iterations, we get the square root of the remainder  */
      value = mySqrt(square);
      if (setTrial(value,res))
      {
        printf("ERROR: setTrial returned error at the %i-th iteration\n",i);
        free(res);
        return NULL;
      }

    }
    /* add the value to the trials array */
    square -= value*value;
    prev = value;
    if (square <= 0)
    {
      return res;
    }
  }
}


#define ERROR_NOT_DECREASING  (-1)
#define ERROR_EXCEEDS         (-2)
#define ERROR_LOW             (-3)

static int checkSoLution(long long n, long long* p, int N)
{
  int i;
  n = n*n - p[N-1]*p[N-1];
  for (i=N-2;i>=0;i--)
  {
    if (p[i]<=p[i+1])
      return ERROR_NOT_DECREASING;
    else
      n = n - p[i]*p[i];
  }
  if (n>0)
    return ERROR_LOW;
  if (n<0)
    return ERROR_EXCEEDS;
  return 0;
}

long long* copyTrial(struct squareSum* p)
{
  int i;
  long long* res = (long long*)malloc(p->N*sizeof(long long));
  if (res)
  {
    for (i=0;i<p->N;i++) res[i]=p->factors[i];
  }
  return res;
}

void recalculate(long long n, long long* p, int N, int last)
{
  int i;
  n = n*n;
  for (i=0;i<N;i++)
  {
    if (i<=last)
    {
      n = n - p[i]*p[i];
    }
    else
    {
      p[i]=mySqrt(n);
      n = n - p[i]*p[i];
    }
  }
}

int printSolution(long long* p, int N)
{
  int i;
  int res=0;
  res += printf("[");
  for (i=0;i<N;i++)
  {
    res += printf("%i",p[N-i-1]);
    if (i<N-1)
      res += printf(",");
    else
      res += printf("]");
  }
  printf("\n");
  return res;
}

int searchSolution(struct squareSum* p)
{
  int i;
  int last=p->N-1;
  long long* solution;
  solution = copyTrial(p);
  for (;last>=0;)
  {
    if (checkSoLution(p->value,solution,p->N)==0)
    {
      free(p->factors);
      p->factors = solution;
      return 0;
    }
    last = p->N-1;
    for (;last>=0;last--)
    {
      if (solution[last]>1)
      {

        printf("Decreasing component %i from %i to %i\n",last,solution[last],solution[last]-1);

        solution[last]--;
        break;
      }
    }
    recalculate(p->value,solution,p->N,last);
  }
  return -1;
}




char* decompose(int n)
{
  struct squareSum* p;
  int res,i,nc;
  char* output,*result;
  char buffer[24];
  p = firstAttempt(n);
  if (searchSolution(p)==0)
  {
    res = printSolution(p->factors,p->N);

    output = (char*)malloc((res+1)*sizeof(char));
    result = output;
    if (output)
    {
      output[0]='[';
      output++;
      for (i=0;i<p->N;i++)
      {
        nc = sprintf(buffer,"%i",p->factors[p->N-i-1]);
        memcpy(output,buffer,nc);
        output += nc;
        if (i<p->N-1)
          output[0]=',';
        else
          output[0]=']';
        output++;
      }
    }
    output[0]=0;
    free(p->factors);
    free(p);
    return result;
  }
  output = (char*)malloc(3*sizeof(char));
  output[0]='[';
  output[1]=']';
  output[2]=0;
  return output;
}
#endif

#include <stdio.h>
#include <stdarg.h>
int myfunction(int n,...)
{
    int sum = 0;
    int i;
    int* p =&sum;
    if (n<=0)
      return 0;
    for (i=0;i<n;i++)
        sum+=p[11+2*i];
    return sum;
}


void getOffsets(int a, int b, int c, int d, int e, int f)
{
  int *p[6];
  int i;
  p[0]=&a;
  p[1]=&b;
  p[2]=&c;
  p[3]=&d;
  p[4]=&e;
  p[5]=&f;
  for (i=1;i<6;i++)
  {
    printf("offset[%i]==%lld\n",i-1,(unsigned long long)(p[i]-p[i-1]));
  }
}





int main(int argc, char* argv[])
{

  int m = myfunction(6,71,72,73,74,75,76);


  printf("%i\n",m);

getOffsets(4,2,3,6,7,9);

  return 0;
}
