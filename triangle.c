#if 0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
static char _array[22]={0,0,0,0,0,'R',0,0,0,0,0,0,0,0,0,0,'G',0,0,0,0,'B'};
char rotate(char a, char b)
{
  _array[0]=a;
  return _array[a^b];
}

char parity(const char *row)
{
    char c=row[0];
    row++;
    while(*row)
    {
      c = rotate(c,*row);
      row++;
    }
    return c;
}

char triangle(const char *row)
{
  int i,j,n;
  char *res;
  char result;

  n = strlen(row);
  result=0;
  if (n<1)
    return 0;
  if (n==1)
    return *row;
  n--;
  res = (char*)malloc(n*sizeof(char));
  if (res)
  {

    for (i=0;i<n;i++)
    {
      res[i]=(res[i]==res[i+1])?res[i]:_array[res[i]^res[i+1]];
    }
    res[i]=0;
    for (i=n-1;i>0;i--)
    {

      for (j=0;j<i;j++)
      {
        res[j]=(res[j]==res[j+1])?res[j]:_array[res[j]^res[j+1]];
      }
      res[j]=0;

    }
    result = res[0];
    free(res);
  }
  return result;
}

/*
           R B R G B R B
       R B R G B R B
         G G B R G G
           g r g b g


        G G B R G G       G G G G
         G G R R B         G G G
          G G B G
           G G R
            G G
             G

  B R B G R B G
   G G R B G R
    G B G R B




     3^5 = 6
     3^6 = 5
     6^5 = 3

     3 ^ 5 nand 5 =   110 nand 101 = 11 = 3

     3^3 = 0

     3 ^ 5 & 5 | 5

     0x356 ^ 0x653 == 0x505      AFA      1010
                                            11
                                           110
     0x356 | 0x653 == 0x757
                       0x757 | 0x888 == 0xFDF      1101



     7 ^ 5 =


     3&5 = 1 + 5
     3&6 = 2 + 3
     5&6 = 4 - 1




52 47 42
6   5  3



*/
#endif

#include <stdio.h>
#include <stdlib.h>


/*

RBG order

*/

char getInt(char c)
{
  switch (c)
  {
    case 'R':return 0;
    case 'B':return 1;
    case 'G':return 2;
    default:return -1;
  }
}

char getParity(const char *row)
{
  int i,n,sum;
  n = strlen(row);
  for (sum=0,i=0;i<n;i++)
  {
    sum += getInt(row[i]);
  }
  return sum % 3;
}

/*

RBG order

*/

char guess(char c, char parity)
{
  if (parity==0)
    return c;

  if ((c=='R') && (parity==1))
    return 'G';

  if ((c=='R') && (parity==2))
    return 'B';

  if ((c=='B') && (parity==1))
    return 'R';

  if ((c=='B') && (parity==2))
    return 'G';

  if ((c=='G') && (parity==1))
    return 'B';

  if ((c=='G') && (parity==2))
    return 'R';
}

char vertex(char g, char np)
{
  if (np==0)
    return g;
  if (np==1)
  {
    if (g=='R')
      return 'B';
    if (g=='B')
      return 'G';
    if (g=='G')
      return 'R';
  }
  if (np==2)
  {
    if (g=='R')
      return 'G';
    if (g=='B')
      return 'R';
    if (g=='G')
      return 'B';
  }
}




char triangle(const char *row)
{
  char p,np,g;
  p = getParity(row);
  np = strlen(row)%3;
  g = guess(row[0],np);
  printf("np==%i, p==%i, g==%c\n",np,p,guess(row[0],p));
  return vertex(g,p);
}

#if 0
char triangle(const char *row)
{
  int i,j,n;
  char *res;
  char result;
  static char _array[22]={0,0,0,0,0,'R',0,0,0,0,0,0,0,0,0,0,'G',0,0,0,0,'B'};
  n = strlen(row);
  result=0;
  if (n<1)
    return 0;
  if (n==1)
    return *row;
  n--;

  res = (char*)malloc(n*sizeof(char));
  if (res)
  {
    for (i=0;i<n;i++)
    {
        _array[0]=row[i];
        res[i]=_array[row[i]^row[i+1]];
    }
    for (i=n-1;i>0;i--)
    {
      for (j=0;j<i;j++)
      {
        _array[0]=res[j];
        res[j]=_array[res[j]^res[j+1]];
      }
    }
    result = res[0];
    free(res);
  }
  return result;
}

#endif




int main(int argc, char* argv[])
{
  char buf[10];
  myFunction(buf,4);
  return 0;

}
