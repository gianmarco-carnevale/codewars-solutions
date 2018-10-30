#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>




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




static long long getRawRangeSum(int a, long long b)
{
  long long i;
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








/*

getRangeSum (0,0x400) = getRangeSum(0,0xff) + getRangeSum(0x100,0x1ff) + getRangeSum(0x300,0x3ff) + bitcount(0x400)

getRangeSum(0x100,0x1ff) = (0xff+1) * getRangeSum(0,0xff)
getRangeSum(0x200,0x2ff) = 2*(0xff+1)*getRangeSum(0,0xff)
getRangeSum(0x300,0x3ff) = 3*(0xff+1)*getRangeSum(0,0xff)

-----------------------------------

getCumulativeSum(0xABCDEF78) =    getRangeSum(1,           0xA0000000) +
                                  getRangeSum(0xA0000000+1,0xAB000000) +
                                  getRangeSum(0xAB000000+1,0xABC00000) +
                                  getRangeSum(0xABC00000+1,0xABCD0000) +
                                  getRangeSum(0xABCD0000+1,0xABCDE000) +
                                  getRangeSum(0xABCDE000+1,0xABCDEF00) +
                                  getRangeSum(0xABCDEF00+1,0xABCDEF70) +
                                  getRangeSum(0xABCDEF70+1,0xABCDEF78)


getRangeSum(1, 0xA0000000) = getRangeSum(1,            0x10000000) +
                             getRangeSum(0x10000000+1, 0x20000000) +
                             getRangeSum(0x20000000+1, 0x30000000) +
                             getRangeSum(0x30000000+1, 0x40000000) +
                             getRangeSum(0x40000000+1, 0x50000000) + ...

getRangeSum(1,0x10000000) = table
getRangeSum(0x10000000+1,0x20000000) =

getRangeSum(0x40000000+1, 0x50000000) = getBitCount(0x4) * (0x10000000-1) * getRangeSum(1,0x10000000-1) + getBitCount(0x50000000);


getRangeSum(0xABCD0000+1,0xABCDE000) = getBitCount(0xABCD) * 0xE000 * getRangeSum(1,0xE000);


getRangesum(0x100,0x10000) = getRangesum(0x100,0x1000) + getRangesum(0x1000+1,0x10000)

*/

static long long rangeSumTable[32]={
0x1,0x2,0x5,0xd,0x21,0x51,0xc1,0x1c1,0x401,0x901,0x1401,0x2c01,0x6001,
0xd001,0x1c001,0x3c001,0x80001,0x110001,0x240001,0x4c0001,0xa00001,
0x1500001,0x2c00001,0x5c00001,0xc000001,0x19000001,0x34000001,0x6c000001,
0xe0000001,0x1d0000001,0x3c0000001,0x7c0000001};




static long long getCumulativeSum(int n)
{
  int i,count,left,mask;
  long long result;
  left=-1;
  count=0;
  for (result=0,i=31;i>=0;i--)
  {
    mask = (1<<i);
    if (n & mask)
    {
      count++;
      if ((n == mask)||(i==0))
      {
        return rangeSumTable[i];
      }
      else
      {
        n = n & ~(0xFFFFFFFF << i);
        result = rangeSumTable[i]-1 + n + count + getCumulativeSum(n);
        return result;
      }
    }
  }
  return result;
}


int main(int argc, char* argv[])
{
  int a,b,c,s1, s2,s3,i,j,prev;
  /*              98 7654 3210   */
  a = 0x54346a3;/*  0010 1010 0110   */

    printf("%lld\n",getRawRangeSum(0x0,a));
    printf("%lld\n", getCumulativeSum(a));


  return 0;
}
