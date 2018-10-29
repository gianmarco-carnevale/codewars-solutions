#include <stdio.h>

static int getBitSum(int n)
{
  int i,sum;
  for(i=0,sum=0;i<32;i++)
  {
    sum += (n & (1<<i))?1:0;
  }
  return sum;
}


static int getCumulativeSum(int n)
{
  int i,sum;
  for (i=1,sum=0;i<=n;i++)
  {
    sum+=getBitSum(i);
  }
}


static int getRawRangeSum(int a, int b)
{
  int i,sum;
  if (b<a)
  {
    printf("ERROR: invalid range\n");
    return -1;
  }
  for (i=a,sum=0;i<=b;i++)
  {
    sum+=getBitSum(i);
  }
  return sum;
}

static int getRangeSum(int , int );

static int getEvenRangeSum(int a,int b)
{
  int difference,nibble,i,m,partial,ratio;
  difference = b - a;
  for (m=0xF,nibble=0x1;;m<<=4,nibble<<=4)
  {
    if (difference & m)
    {
      break;
    }
  }
  ratio = difference/nibble;
  printf("%x,%x,ratio==%i\n",a,b,ratio);
  for (partial=0,i=0;i<ratio;i++,a+=nibble)
  {
    partial += getRangeSum(a, a+nibble);
    if (i<ratio-1)
      partial -= getBitSum(a+nibble);
    printf("[%x,%x]\n",a, a+nibble);
  }
  return partial;
}


static int getRangeSum(int a, int b)
{
  int delta, difference,i,j,m,nibble,partial;
  if (b<a)
  {
    printf("ERROR: invalid range\n");
    return -1;
  }
  delta = 1 + a + getBitSum(a);
  difference = b - a;
/*
  printf("difference==%i, delta==%i\n",difference,delta);
*/
  switch (difference)
  {
    case 0:
      return getBitSum(a);
    case 0x1:
      return getBitSum(a) + getBitSum(b);
    case 0x10:
      return 0x20 + delta;
    case 0x100:
      return 0x400 + delta;
    case 0x1000:
      return 0x6000 + delta;
    case 0x10000:
      return 0x80000 + delta;
    case 0x100000:
      return 0xa00000 + delta;
    case 0x1000000:
      return 0xc000000 + delta;
    case 0x10000000:
      return 0xe0000000 + delta;

    default:
    {
      return -1;
    }
    break;
  }
  return -1;
}


int main(int argc, char* argv[])
{
  int a,b,c,s1, s2,s3;
  a = 0x123;
  b = a + 0x20000;

  s1 = getRawRangeSum(a,b);
  s2 = getRangeSum(a,b);
  s3 = getEvenRangeSum(a,b);
  printf("----------------\n");
  printf("%x\n",s1);
  printf("%x\n",s2);
  printf("%x\n",s3);
  return 0;
}
