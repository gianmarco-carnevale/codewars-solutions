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

  return 0;
}

static int mix(char a, char b)
{
  if (a==b)
    return a;

  if (a=='R')
  {
    if (b=='G') return 'B';
    if (b=='B') return 'G';
  }
  if(a=='G')
  {
    if (b=='R') return 'B';
    if (b=='B') return 'R';
  }
  if (a=='B')
  {
    if (b=='G') return 'R';
    if (b=='R') return 'G';
  }
  return 0;
}

char defaultMix(const char *ptr, int length)
{
  int i,j,n;
  char *temp,result;
  n = length-1;
  temp = (char*)malloc(n*sizeof(char));
  if (temp)
  {
    for (i=0;i<n;i++)
    {
      temp[i]=mix(ptr[i],ptr[i+1]);
    }
    for (i=n-1;i>0;i--)
    {
      for (j=0;j<i;j++)
      {
        temp[j]=mix(temp[j],temp[j+1]);
      }
    }
    result = temp[0];
    free(temp);
  }
  return result;
}

char recursiveMix(const char *ptr, int length)
{
  if ((length%2==0) && (length/2>2))
  {
    length = length/2;

    return mix(recursiveMix(ptr,length),recursiveMix(&ptr[length],length));
  }
  else
    return defaultMix(ptr,length);
}

char layerMix(const char *row)
{
  int i,n;
  char *temp;
  int rgbCount1[3]={0,0,0};
  int rgbCount2[3]={0,0,0};
  char result[3]={'R','G','B'};
  n = strlen(row);
  temp = (char*)malloc((n-1)*sizeof(char));
  if (temp)
  {
    for (i=0;i<n;i++)
    {
      if (row[i]=='R') rgbCount1[0]++;
      if (row[i]=='G') rgbCount1[1]++;
      if (row[i]=='B') rgbCount1[2]++;
      if (i>0)
      {
        temp[i-1]=mix(row[i-1],row[i]);
        if (temp[i-1]=='R') rgbCount2[0]++;
        if (temp[i-1]=='G') rgbCount2[1]++;
        if (temp[i-1]=='B') rgbCount2[2]++;
      }
    }
    free(temp);
    for (i=0;i<3;i++)
    {
      /*
      if (rgbCount2[i]>rgbCount1[i])
      {
        printf("%c incremented\n",result[i]);
      }
      if (rgbCount2[i]<rgbCount1[i])
      {
        printf("%c decremented\n",result[i]);
      }
      if (rgbCount2[i]==rgbCount1[i])
      {
        printf("%c stable\n",result[i]);
      }
      */
      printf("%c passing from %i to %i\n",result[i],rgbCount1[i],rgbCount2[i]);
    }
  }
  return 'R';
}


char straightMix(const char *row)
{
  int i;
  char res;
  int length= strlen(row);
  res=row[0];
  for (i=1;i<length;i++)
  {
    res = mix(row[i-1],row[i]);
  }
  return res;
}

char triangle(const char *row)
{
  int length;
  length = strlen(row);
  if (length<1)
    return 0;
  if (length==1)
    return *row;
  if (length==2)
    return mix(row[0],row[1]);
  return recursiveMix(row,length);
}

#if 0

 0 0 0 0    0 1 2 0
  0 0 0      2 0 1
   0 0        1 2
    0          0

0 0 1 2 0
 0 2 0 1
  1 1 2
   1 0
    2


1 2 0 0 0 0 0
 0 1 0 0 0 0
  2 2 0 0 0
   2 1 0 0
    0 2 0
     1 1
      1

1 2      0 0
 0



 0 1 0
  2 2
   2

{{'R','G','B'},{'R','G','B'},{'R','G','B'},{'R','G','B'},{'R','G','B'},{'R','G','B'}}





#endif


static void printfBase3(unsigned int n, char *dest, unsigned size)
{
  int i;
  static const char colors[3]={'R','G','B'};
  for (i=0;n>0;i++)
  {
    dest[0]=colors[n%3];
    dest=&dest[1];
    n=n/3;
  }
  if (i<=size)
    memset(dest,colors[0],size-i);
  else
      printf("ERROR:!!!!\n");
}

static void printOpenBrackets(unsigned index, unsigned int n, unsigned *powPtr)
{
  unsigned i;
  if (index==0)
  {
    for (i=0;i<n;i++) printf("{");
  }
  else
  {
    for (i=0;i<n;i++)
    {
      if ((index % powPtr[i] == 0) && (index>=powPtr[i]))
      {
        printf("{");
      }
    }
  }
}

static void printClosedBrackets(unsigned index, unsigned int n, unsigned *powPtr)
{
  unsigned i;
  if (index==powPtr[n-1]-1)
  {
    for (i=0;i<n;i++) printf("}");
    printf(";\n");
  }
  else
  {
    for (i=0;i<n;i++)
    {
      if (((index+1) % powPtr[i] == 0) && ((index+1)/powPtr[i]))
      {
        printf("}");
      }
    }
    printf(",");
    if (((index+1)>=18) && ((index+1)%18==0))
      printf("\n                                  ");
  }
}



#define MAX_LENGTH 8
static void printSequence(unsigned int n)
{
  unsigned i,j;
  unsigned res;
  unsigned* power;
  int bracketCount;
  char buf[MAX_LENGTH+1];
  if (n==0)
  {
    printf("ERROR: size cannot be zero\n");
    return;
  }
  if (n>MAX_LENGTH)
  {
    printf("ERROR: size is too large\n");
    return;
  }
  power = (unsigned*)malloc(n*sizeof(unsigned));
  if (power==NULL)
  {
    printf("ERROR: malloc failed\n");
    return;
  }
  for (i=0,res=1;i<n;i++)
  {
    res = res*3;
    power[i]=res;
    printf("power[%i]==%i\n",i,res);
  }
  printf("static char mix%d",n);
  for (i=0;i<n;i++) printf("[3]");
  printf("=");
  for (i=0;i<res;i++)
  {
    printOpenBrackets(i,n,power);
    printfBase3(i,buf,n);
    printf("'%c'",defaultMix(buf,n));
    printClosedBrackets(i,n,power);
  }

}






static char mix6[3][3][3][3][3][3]={{{{{{'R','B','G'},{'G','R','B'},{'B','G','R'}},{{'B','G','R'},{'R','B','G'},{'G','R','B'}},
                                  {{'G','R','B'},{'B','G','R'},{'R','B','G'}}},{{{'B','G','R'},{'R','B','G'},{'G','R','B'}},
                                  {{'G','R','B'},{'B','G','R'},{'R','B','G'}},{{'R','B','G'},{'G','R','B'},{'B','G','R'}}},
                                  {{{'G','R','B'},{'B','G','R'},{'R','B','G'}},{{'R','B','G'},{'G','R','B'},{'B','G','R'}},
                                  {{'B','G','R'},{'R','B','G'},{'G','R','B'}}}},{{{{'G','R','B'},{'B','G','R'},{'R','B','G'}},
                                  {{'R','B','G'},{'G','R','B'},{'B','G','R'}},{{'B','G','R'},{'R','B','G'},{'G','R','B'}}},
                                  {{{'R','B','G'},{'G','R','B'},{'B','G','R'}},{{'B','G','R'},{'R','B','G'},{'G','R','B'}},
                                  {{'G','R','B'},{'B','G','R'},{'R','B','G'}}},{{{'B','G','R'},{'R','B','G'},{'G','R','B'}},
                                  {{'G','R','B'},{'B','G','R'},{'R','B','G'}},{{'R','B','G'},{'G','R','B'},{'B','G','R'}}}},
                                  {{{{'B','G','R'},{'R','B','G'},{'G','R','B'}},{{'G','R','B'},{'B','G','R'},{'R','B','G'}},
                                  {{'R','B','G'},{'G','R','B'},{'B','G','R'}}},{{{'G','R','B'},{'B','G','R'},{'R','B','G'}},
                                  {{'R','B','G'},{'G','R','B'},{'B','G','R'}},{{'B','G','R'},{'R','B','G'},{'G','R','B'}}},
                                  {{{'R','B','G'},{'G','R','B'},{'B','G','R'}},{{'B','G','R'},{'R','B','G'},{'G','R','B'}},
                                  {{'G','R','B'},{'B','G','R'},{'R','B','G'}}}}},{{{{{'B','G','R'},{'R','B','G'},{'G','R','B'}},
                                  {{'G','R','B'},{'B','G','R'},{'R','B','G'}},{{'R','B','G'},{'G','R','B'},{'B','G','R'}}},
                                  {{{'G','R','B'},{'B','G','R'},{'R','B','G'}},{{'R','B','G'},{'G','R','B'},{'B','G','R'}},
                                  {{'B','G','R'},{'R','B','G'},{'G','R','B'}}},{{{'R','B','G'},{'G','R','B'},{'B','G','R'}},
                                  {{'B','G','R'},{'R','B','G'},{'G','R','B'}},{{'G','R','B'},{'B','G','R'},{'R','B','G'}}}},
                                  {{{{'R','B','G'},{'G','R','B'},{'B','G','R'}},{{'B','G','R'},{'R','B','G'},{'G','R','B'}},
                                  {{'G','R','B'},{'B','G','R'},{'R','B','G'}}},{{{'B','G','R'},{'R','B','G'},{'G','R','B'}},
                                  {{'G','R','B'},{'B','G','R'},{'R','B','G'}},{{'R','B','G'},{'G','R','B'},{'B','G','R'}}},
                                  {{{'G','R','B'},{'B','G','R'},{'R','B','G'}},{{'R','B','G'},{'G','R','B'},{'B','G','R'}},
                                  {{'B','G','R'},{'R','B','G'},{'G','R','B'}}}},{{{{'G','R','B'},{'B','G','R'},{'R','B','G'}},
                                  {{'R','B','G'},{'G','R','B'},{'B','G','R'}},{{'B','G','R'},{'R','B','G'},{'G','R','B'}}},
                                  {{{'R','B','G'},{'G','R','B'},{'B','G','R'}},{{'B','G','R'},{'R','B','G'},{'G','R','B'}},
                                  {{'G','R','B'},{'B','G','R'},{'R','B','G'}}},{{{'B','G','R'},{'R','B','G'},{'G','R','B'}},
                                  {{'G','R','B'},{'B','G','R'},{'R','B','G'}},{{'R','B','G'},{'G','R','B'},{'B','G','R'}}}}},
                                  {{{{{'G','R','B'},{'B','G','R'},{'R','B','G'}},{{'R','B','G'},{'G','R','B'},{'B','G','R'}},
                                  {{'B','G','R'},{'R','B','G'},{'G','R','B'}}},{{{'R','B','G'},{'G','R','B'},{'B','G','R'}},
                                  {{'B','G','R'},{'R','B','G'},{'G','R','B'}},{{'G','R','B'},{'B','G','R'},{'R','B','G'}}},
                                  {{{'B','G','R'},{'R','B','G'},{'G','R','B'}},{{'G','R','B'},{'B','G','R'},{'R','B','G'}},
                                  {{'R','B','G'},{'G','R','B'},{'B','G','R'}}}},{{{{'B','G','R'},{'R','B','G'},{'G','R','B'}},
                                  {{'G','R','B'},{'B','G','R'},{'R','B','G'}},{{'R','B','G'},{'G','R','B'},{'B','G','R'}}},
                                  {{{'G','R','B'},{'B','G','R'},{'R','B','G'}},{{'R','B','G'},{'G','R','B'},{'B','G','R'}},
                                  {{'B','G','R'},{'R','B','G'},{'G','R','B'}}},{{{'R','B','G'},{'G','R','B'},{'B','G','R'}},
                                  {{'B','G','R'},{'R','B','G'},{'G','R','B'}},{{'G','R','B'},{'B','G','R'},{'R','B','G'}}}},
                                  {{{{'R','B','G'},{'G','R','B'},{'B','G','R'}},{{'B','G','R'},{'R','B','G'},{'G','R','B'}},
                                  {{'G','R','B'},{'B','G','R'},{'R','B','G'}}},{{{'B','G','R'},{'R','B','G'},{'G','R','B'}},
                                  {{'G','R','B'},{'B','G','R'},{'R','B','G'}},{{'R','B','G'},{'G','R','B'},{'B','G','R'}}},
                                  {{{'G','R','B'},{'B','G','R'},{'R','B','G'}},{{'R','B','G'},{'G','R','B'},{'B','G','R'}},
                                  {{'B','G','R'},{'R','B','G'},{'G','R','B'}}}}}};


int main(int argc, char* argv[])
{
  int i,length;
  int n;
  /*
  const char* test[]={"RBRGBRBGGRRRBGBBBGG",
                      "RRBGRGBGBRRGRRBGGBRG",
                      "BBRRGBBGGGRBRGRGBGRGB",
                      "RRGRGBBGRGBRRGBGBRGGBG",
                      "GGRRGBBGRGBBGRRGRGGBRBR",
                      "RGBGGGBBGGRGBGRRRRGBBGRG",
                      "GGBGRGBBGRGRGRGBBGRGGBGBG"};
                      */
                      #if 0
  const char* test[]={"RBGRB","RBGRBB","RBGRBBB","RBGRBBBB","RBGRBBBBB",
                      "RBGRBBBBBB","RBGRBBBBBBB","RBGRBBBBBBBB","RBGRBBBBBBBBB",
                      "RBGRBBBBBBBBBB","RBGRBBBBBBBBBBB","RBGRBBBBBBBBBBBB","RBGRBBBBBBBBBBBBB","RBGRBBBBBBBBBBBBBB"};
  n = sizeof(test)/sizeof(char*);
  for (i=0;i<n;i++)
  {
    length = strlen(test[i]);
    printf("mix(%s)==%c, straight==%c\n",test[i],defaultMix(test[i],length),straightMix(test[i]));
  }
  #endif
  printSequence(6);
  return 0;

}
