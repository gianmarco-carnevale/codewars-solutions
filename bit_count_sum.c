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


static int getRangeSum(int a, int b)
{
  int i,sum;
  if (b<a)
    return -1;
  for (i=a,sum=0;i<=b;i++)
  {
    sum+=getBitSum(i);
  }
  return sum;
}
/*
   001    01
   010    01
   011    10
   100    01
   101    10
   110    10
   111    11
  1000    01
  1001    10
  1010    10
  1011    11
  1100    10
  1101    11
  1110    11
  1111   100
 10000   001

*/

int _getRangeSum(int a, int b)
{
  int delta, difference;
  if (b<a)
    return -1;
  delta = 1 + a + getBitSum(a);
  difference = b - a;
  switch (difference)
  {
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
      return 0;
    break;
  }
  return 0;
}


int main(int argc, char* argv[])
{
  int a,b,c,s1, s2,s3;
  a = 0x12;
  b = a + 0x20000;

  s1 = getRangeSum(a,b);
  s2 = _getRangeSum(a,b);

  printf("%x\n",s1);
  printf("%x\n",s2);
  return 0;
}
