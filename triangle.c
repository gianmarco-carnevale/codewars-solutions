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
  printf("unhandled characters %c and %c\n",a,b);
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

char _triangle(const char *row)
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
  static const char colors[3]={'B','G','R'};
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

static char getInt(char c)
{
  if (c=='R') return 0;
  if (c=='G') return 1;
  if (c=='B') return 2;
  return -1;
}

#define MAX_LENGTH 8
static void printSequence(unsigned int n)
{
  unsigned i,j;
  unsigned p;
  unsigned* power;
  int bracketCount;
  char* buf;
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
  buf = (char*)malloc(MAX_LENGTH*sizeof(char));
  if (buf==NULL)
  {
    printf("ERROR: malloc failed\n");
    return;
  }
  power = (unsigned*)malloc(n*sizeof(unsigned));
  if (power==NULL)
  {
    printf("ERROR: malloc failed\n");
    free(buf);
    return;
  }

  for (i=0,p=1;i<n;i++)
  {
    p = p*3;
    power[i]=p;
  }
  printf("static const int mixOrder=%d;\n",n);
  printf("static char mix%d",n);
  for (i=0;i<n;i++) printf("[3]");
  printf("=");
  for (i=0;i<p;i++)
  {
    printOpenBrackets(i,n,power);
    printfBase3(i,buf,n);
    printf("'%c'",defaultMix(buf,n));
    printClosedBrackets(i,n,power);
  }
  free(buf);
  free(power);
  printf("static char getMix(char* ptr)\n{\n");
  printf("  return mix%d",n);
  for (i=0;i<n;i++)
  {
    printf("[getInt(ptr[%i])]",i);
  }
  printf(";\n}\n");
}

static const int mixOrder=6;
static char mix6[3][3][3][3][3][3];
static char getMix(char* ptr)
{
  return mix6[getInt(ptr[0])][getInt(ptr[1])][getInt(ptr[2])][getInt(ptr[3])][getInt(ptr[4])][getInt(ptr[5])];
}

char triangle(const char* row)
{
  unsigned i;
  unsigned n;
  char* buf;
  char r;
  n = strlen(row);
  if (n<1)
    return 0;
  if (n==1)
    return row[0];
  if (n==2)
    return mix(row[0],row[1]);
  buf = (char*)malloc((n-1)*sizeof(char));
  if (buf==NULL)
    return 0;
  if (n>=mixOrder)
  {
    for (i=0;i<n-mixOrder+1;i++)
    {
      buf[i]=getMix(&row[i]);
    }
    n = n - mixOrder+1;
    if (n==1)
    {
      r=buf[0];
      free(buf);
      return r;
    }
    for (;;)
    {
      if (n>=mixOrder)
      {
        for (i=0;i<n-mixOrder+1;i++)
        {
          buf[i]=getMix(&buf[i]);
        }
        n = n - mixOrder+1;
        if (n==1)
        {
          r=buf[0];
          free(buf);
          return r;
        }
      }
      else
      {
        r = defaultMix(buf,n);
        free(buf);
        return r;
      }
    }
  }
  else
  {
    free(buf);
    r = defaultMix(row,n);
    return r;
  }
}

static int areReversed(char* p1, char* p2, int length)
{
  int i;
  for (i=0;i<length/2;i++)
  {
    if (p1[i]!=p2[length-1-i])
      return 0;
  }
  return 1;
}

static char rotation(char c, int rule)
{
  static char rotationRule[6][3]={{'B','G','R'},{'B','R','G'},{'G','B','R'},
                                  {'G','R','B'},{'R','B','G'},{'R','G','B'}};
  switch (c)
  {
    case 'B':return rotationRule[rule][0];
    case 'G':return rotationRule[rule][1];
    case 'R':return rotationRule[rule][2];
    default: return 0;
  }
  return 0;
}

int equivalent(char* p1, char* p2, int length)
{
  int i,j,count,upper;
  for (i=0;i<6;i++)
  {
    for (j=0,count=0;j<length;j++)
    {
      if (p1[j]==rotation(p2[j],i))
        count++;
      else
        break;
    }

    if (count==length)
      return 1;

    for (j=0,count=0;j<length;j++)
    {
      if (p1[length-j-1]==rotation(p2[j],i))
        count++;
    }
    if (count==length)
      return 1;

  }
  return 0;
}
#define MAX_STRING_LENGTH 20
struct sequence_t
{
  char row[MAX_STRING_LENGTH+1];
  struct sequence_t* next;
};

void deleteList(struct sequence_t* p)
{
  struct sequence_t* temp;
  for (;p;)
  {
    temp = p->next;
    free(p);
    p = temp;
  }
}

struct sequence_t *addSequence(struct sequence_t * p, char* str, int length)
{
  struct sequence_t * temp;
  if (p==NULL)
  {
    p=(struct sequence_t*)malloc(sizeof(struct sequence_t));
    if (p==NULL)
      return NULL;
    p->next=NULL;
    memcpy(p->row,str,length);
    p->row[length]=0;
    return p;
  }
  for (temp=p;;)
  {
    if (temp->next==NULL)
      break;
    else
      temp = temp->next;
  }
  temp->next = (struct sequence_t*)malloc(sizeof(struct sequence_t));
  if (temp->next)
  {
    temp->next->next = NULL;
    memcpy(temp->next->row,str,length);
    temp->next->row[length]=0;
  }
  return p;
}

int alreadyAdded(struct sequence_t * p, char *str, int length)
{
  if (p==NULL)
    return 0;
  for (;p;p=p->next)
  {
    if (equivalent(p->row,str,length))
      return 1;
  }
  return 0;
}

void generate(int N)
{
  int i;
  long power;
  int count;
  char m,l,r,q1,q2,q3,q4;
  char buf[MAX_STRING_LENGTH+1];
  struct sequence_t *seqList = NULL;
  if (N<2)
    return;
  if (N>MAX_STRING_LENGTH)
    return;
  for (i=0,power=1;i<N;i++)
  {
    power = power*3;
  }
  buf[N]=0;
  for (i=0,count=0;i<power;i++)
  {
    printfBase3(i,buf,N);

    if (!alreadyAdded(seqList,buf,N))
    {
      seqList = addSequence(seqList,buf,N);
      m = defaultMix(buf,N);
      q1 = defaultMix(buf,N/4);
      q2 = defaultMix(&buf[N/4],N/4);
      q3 = defaultMix(&buf[N/2],N/4);
      q4 = defaultMix(&buf[(N*3)/4],N/4);
      if (m!=mix(mix(q1,q2),mix(q3,q4)))
        printf("wrong\n");

      count++;
    }
  }
  deleteList(seqList);
  printf("A total of %i sequences\n",count);
}

static char mixPowerOf3(char* ptr, unsigned length)
{
  char temp[3];
  printf("mixPowerOf3(%s,%d)\n",ptr,length);
  if (length==3) return defaultMix(ptr,length);
  length = length/3;
  temp[0] = mixPowerOf3(ptr,length);
  temp[1] = mixPowerOf3(&ptr[length],length);
  temp[2] = mixPowerOf3(&ptr[2*length],length);
  return defaultMix(temp,3);
}

static char mixPowerOf3PlusOne(char* ptr, unsigned length)
{
  printf("mixPowerOf3PlusOne(%s,%d)\n",ptr,length);
  return mix(ptr[0],ptr[length-1]);
}

static char mixDoublePowerOf3(char* ptr, unsigned length)
{
  printf("mixDoublePowerOf3(%s,%d)\n",ptr,length);
  return mix(mixPowerOf3(ptr,length/2),mixPowerOf3(&ptr[length/2],length/2));
}

static const unsigned long pow3[20] = {1,3,9,27,81,243,729,2187,6561,19683,
                                       59049,177147,531441,1594323,4782969,
                                       14348907,43046721,129140163,387420489,
                                       1162261467};

enum eLength_type
{
  ePowerOf3,
  ePowerOf3PlusOne,
  eDoublePowerOf3,
  eGeneric,
  eInvalid
};

static enum eLength_type getLengthType(unsigned long length)
{
  int i;
  for (i=0;i<20;i++)
  {
    if (length<pow3[i])
      break;
  }
  if (i==20)
    return eInvalid;
  if (i==0)
    return eInvalid;
  if (length==pow3[i-1])
    return ePowerOf3;
  if (length == pow3[i-1]+1)
    ePowerOf3PlusOne;
  if (length==2*pow3[i-1])
    return eDoublePowerOf3;
  return eGeneric;
}

static char lengthBasedMix(char* ptr, unsigned length)
{
  unsigned i;
  char *temp;
  char result;
  enum eLength_type t = getLengthType(length);
  printf("length type == %i\n",t);
  switch (t)
  {
    case ePowerOf3:return mixPowerOf3(ptr,length);
    case ePowerOf3PlusOne:return mixPowerOf3PlusOne(ptr,length);
    case eDoublePowerOf3:return mixDoublePowerOf3(ptr,length);
    case eGeneric:
      if (length<4)
        return defaultMix(ptr,length);
      length--;
      temp = (char*)malloc(length*sizeof(char));
      if (temp==NULL)
      {
        printf("ERROR: malloc\n");
        return 0;
      }
      for (i=0;i<length;i++)
      {
        temp[i]=mix(ptr[i],ptr[i+1]);
      }
      result = lengthBasedMix(temp,length);
      free(temp);
      return result;
    break;
    case eInvalid:
      printf("ERROR: sequence too large\n");
      return 0;
    default:return 0;
  }
  return 0;
}

char myTriangle(const char* row)
{
  unsigned n;
  if (row==NULL)
    return 0;
  n = strlen(row);
  if (n<1)
    return 0;
  if (n==1)
    return row[0];
  if (n==2)
    return mix(row[0],row[1]);
  return lengthBasedMix((char*)row,n);
}

int main(int argc, char* argv[])
{
  int i,length;
  int n;

  const char* test[]={"RBRGBRBGGRRRBGBBBGG",
                      "RRBGRGBGBRRGRRBGGBRG",
                      "BBRRGBBGGGRBRGRGBGRGB",
                      "RRGRGBBGRGBRRGBGBRGGBG",
                      "GGRRGBBGRGBBGRRGRGGBRBR",
                      "RGBGGGBBGGRGBGRRRRGBBGRG",
                      "GGBGRGBBGRGRGRGBBGRGGBGBG",
                      "GRRGBGRBBGRGGBGGRGBGRGBGBRGBR"};


  n = sizeof(test)/sizeof(char*);
  for (i=0;i<n;i++)
  {
    length = strlen(test[i]);
    printf("mix(%s)==%c, triangle==%c\n",test[i],defaultMix(test[i],length),myTriangle(test[i]));
  }

#if 0

  char b[7];
  b[mixOrder]=0;
  for (i=0;i<100;i++)
  {
    printfBase3(i,b,mixOrder);
    if (defaultMix(b,mixOrder)==getMix(b))
      printf("ok\n");
  }


#endif



}
