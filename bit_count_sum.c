#include <stdio.h>

static long long getBitSum(int n)
{
  int i;
  long long sum;
  for(i=0,sum=0;i<32;i++)
  {
    sum += (n & (1<<i))?1:0;
  }
  return sum;
}

static long long getRawRangeSum(int a, int b)
{
  int i;
  long long sum;
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

static long long getRecursiveRangeSum(int a, int b)
{
  int middle,difference;
  long long result;
  if (b<a)
  {
    printf("ERROR: invalid range\n");
    return -1;
  }
  printf("[%d,%d]\n",a,b);
  difference = b - a;
  if (difference<0x100000)
    return (long long)getRawRangeSum(a,b);
  else
  {
    middle = ((long long)a + (long long)b)/2;
    result = getRecursiveRangeSum(a,middle) + getRecursiveRangeSum(middle,b) - getBitSum(middle);
  }
}






int main(int argc, char* argv[])
{
  int a,b,c,s1, s2,s3,i,j,prev;
  a = 15;
  prev = 0;

  printf("%lli\n",getRecursiveRangeSum(0x1,0x10000000));
  return 0;
}
