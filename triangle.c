#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
   R G B
*/


static int getSteps(char a, char b)
{
  if (a==b)
    return 0;

  if (a=='R')
  {
    if (b=='G') return 1;
    if (b=='B') return 2;
  }
  if(a=='G')
  {
    if (b=='R') return 2;
    if (b=='B') return 1;
  }
  if (a=='B')
  {
    if (b=='G') return 2;
    if (b=='R') return 1;
  }
}

/*
   R G B
*/

static char getNext(char c, int steps)
{
  if (steps<0)
    steps += 3;
  if (steps==0)
  {
    return c;
  }
  if(steps==1)
  {
    if (c=='G') return 'B';
    if (c=='R') return 'G';
    if (c=='B') return 'R';
  }
  if (steps==2)
  {
    if (c=='G') return 'R';
    if (c=='R') return 'B';
    if (c=='B') return 'G';
  }
  printf("ERROR:\n");
  return 0;
}

char triangle(const char *row)
{
  int i,j,n;
  char *res;
  char result;
  n = strlen(row);
  if (n<1)
    return 0;
  if (n==1)
    return *row;
  result = row[0];
  for (i=1;i<n;i++)
  {
    result = getNext(result,-getSteps(row[i-1],row[i]));
  }
  return result;
}



int main(int argc, char* argv[])
{
  char buf[10];
  myFunction(buf,4);
  return 0;

}
