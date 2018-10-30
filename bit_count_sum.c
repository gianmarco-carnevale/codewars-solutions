#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

const unsigned m1  = 0x55555555;
const unsigned m2  = 0x33333333;
const unsigned m4  = 0x0f0f0f0f;
const unsigned m8  = 0x00ff00ff;
const unsigned m16 = 0x0000ffff;
const unsigned m32 = 0xffffffff;

int popcount64b(unsigned x)
{
    x -= (x >> 1) & m1;
    x = (x & m2) + ((x >> 2) & m2);
    x = (x + (x >> 4)) & m4;
    x += x >>  8;
    x += x >> 16;
    return x & 0x7f;
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
    sum+=popcount64b(i);
  }
  return sum;
}


static const int range256Sum[0x100]=
{0,1,2,4,5,7,9,12,13,15,17,20,22,25,28,32,33,35,37,40,42,45,48,52,54,57,60,64,67,
71,75,80,81,83,85,88,90,93,96,100,102,105,108,112,115,119,123,128,130,133,136,
140,143,147,151,156,159,163,167,172,176,181,186,192,193,195,197,200,202,205,208,
212,214,217,220,224,227,231,235,240,242,245,248,252,255,259,263,268,271,275,279,
284,288,293,298,304,306,309,312,316,319,323,327,332,335,339,343,348,352,357,362,
368,371,375,379,384,388,393,398,404,408,413,418,424,429,435,441,448,449,451,453,
456,458,461,464,468,470,473,476,480,483,487,491,496,498,501,504,508,511,515,519,
524,527,531,535,540,544,549,554,560,562,565,568,572,575,579,583,588,591,595,599,
604,608,613,618,624,627,631,635,640,644,649,654,660,664,669,674,680,685,691,697,
704,706,709,712,716,719,723,727,732,735,739,743,748,752,757,762,768,771,775,779,
784,788,793,798,804,808,813,818,824,829,835,841,848,851,855,859,864,868,873,878,
884,888,893,898,904,909,915,921,928,932,937,942,948,953,959,965,972,977,983,989,
996,1002,1009,1016,1024};




static long long getRange256Sum(int a, int b)
{
  long long result;
  int bit_a = popcount64b(a);
  if (b<a)
    return -1;
  if (b<0x100)
    return range256Sum[b] - range256Sum[a] + bit_a;
  else
  {
    if (a<0x100)
    {
      result = range256Sum[0xff] - range256Sum[a] + bit_a;
      result += getRawRangeSum(0xff,b) - 8; /* popcount64b(0xff) */
      return result;
    }
    else
      return getRawRangeSum(a,b);
  }

}



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


/*

getRangeSum (0,0x400) = getRangeSum(0,0xff) + getRangeSum(0x100,0x1ff) + getRangeSum(0x300,0x3ff) + bitcount(0x400)

getRangeSum(0x100,0x1ff) = (0xff+1) * getRangeSum(0,0xff)
getRangeSum(0x200,0x2ff) = 2*(0xff+1)*getRangeSum(0,0xff)
getRangeSum(0x300,0x3ff) = 3*(0xff+1)*getRangeSum(0,0xff)

-----------------------------------

getRangeSum (0,0x1000) =






*/



static long long getRecursiveRangeSum(int a, int b)
{
  int middle,difference;
  long long result;
  if (b<a)
  {
    printf("ERROR: invalid range\n");
    return -1;
  }

  difference = b - a;
  if (difference<0x100)
  {
    printf("getRange256Sum(%i,%i)\n",a,b);
    return getRange256Sum(a,b);
  }

  else
  {
    middle = ((long long)a + (long long)b)/2;
    result = getRecursiveRangeSum(a,middle) + getRecursiveRangeSum(middle,b) - getBitSum(middle);
  }
}



/*
getBitSum (a | b)= getBitSum(a) + getBitSum(b) - getBitSum(a & b)

*/


int main(int argc, char* argv[])
{
  int a,b,c,s1, s2,s3,i,j,prev;
  a = 80;
  b = 300;
  s1 = getRecursiveRangeSum(1,1000000000);
  s2 = getRange256Sum(1,1);
  printf("%d\n",s1);
  printf("%d\n",s2);
  return 0;
}
