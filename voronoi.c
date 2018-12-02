#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#if 0

x0,y0
x1,y1

a=(y1-y0)/(x1-x0)

(x0+x1)/2
(y0+y1)/2

b = (x0-x1)/(y1-y0)


y=bx+k

(y0+y1)/2 =  (x0-x1)*(x0+x1)/2*(y1-y0)+k


#endif
typedef struct { double x,y; } Point;

struct Line
{
  double angular;
  double offset;
};

static struct Line getStraightLine(Point a, Point b)
{
  struct Line result;
  if (a.x == b.x)
  {
    result.angular = INFINITY;
    result.offset = a.x;
  }
  else
  {
    result.angular = (b.y - a.y)/(b.x - a.x);
    result.offset = a.y - result.angular * a.x;
  }
  return result;
}

static struct Line getMedianLine(Point a, Point b)
{
  struct Line result;
  if (a.y == b.y)
  {
    result.angular = INFINITY;
    result.offset = (a.x + b.x)/2.0;
  }
  else
  {
    result.angular = (a.x - b.x)/(b.y - a.y);
    result.offset = (a.y + b.y)/2.0 - (a.x*a.x - b.x*b.x)/(2.0*(b.y - a.y));
  }
  return result;
}

static Point getIntersection(struct Line l1, struct Line l2)
{
  Point result;
  if (l1.angular == l2.angular)
  {
    result.x = INFINITY;
    result.y = INFINITY;
  }
  else
  {
    if (isinf(l2.angular))
    {
      result.x = l2.offset;
      result.y = l1.angular * result.x + l1.offset;
    }
    else
    {
      if (isinf(l1.angular))
      {
        result.x = l1.offset;
        result.y = l2.angular * result.x + l2.offset;
      }
      else
      {
        result.x = (l2.offset - l1.offset) / (l1.angular - l2.angular);
        result.y = l1.angular * result.x + l1.offset;
      }
    }
  }
  return result;
}

static int areParted(Point a,Point b, struct Line l)
{
  double ya,yb;
  if (isinf(l.angular))
  {
    if ((a.x < l.offset) && (b.x > l.offset))
      return 1;
    if ((a.x > l.offset) && (b.x < l.offset))
      return 1;
    return 0;
  }
  else
  {
    ya = l.angular * a.x + l.offset;
    yb = l.angular * b.x + l.offset;
    if ((ya > a.y) && (yb < b.y))
      return 1;
    if ((ya < a.y) && (yb > b.y))
      return 1;
    return 0;
  }
}

static double getSquareDistance(Point a, Point b)
{
  double p,q;
  p = a.x - b.x;
  q = a.y - b.y;
  return (p*p+q*q);
}

static int samePoint(Point a, Point b)
{
  if ((a.x==b.x)&&(a.y==b.y))
    return 1;
  return 0;
}

static int getVertexBetweenLongerEdges(Point a, Point b, Point c)
{
  double sqDistAB, sqDistAC, sqDistBC;
  sqDistAB = getSquareDistance(a,b);
  sqDistAC = getSquareDistance(a,c);
  sqDistBC = getSquareDistance(b,c);
  if (sqDistAB < sqDistAC)
  {
    if (sqDistBC < sqDistAB)
    {
      /* sqDistBC < sqDistAB < sqDistAC */
      return 0;
    }
    else
    {
      /* sqDistAB < sqDistAC and sqDistAB <= sqDistBC */
      return 2;
    }
  }
  else
  {
    /* sqDistAC <= sqDistAB */
    if (sqDistBC < sqDistAC)
    {
      return 0;
    }
    else
    {
      /* sqDistAC <= sqDistAB and sqDistAC <= sqDistBC */
      return 1;
    }
  }
}

static Point getCircumcenter(Point a, Point b, Point c, double *pSquareRadius)
{
  Point intersection;
  switch (getVertexBetweenLongerEdges(a,b,c))
  {
    case 0:
      intersection = getIntersection(getMedianLine(a,b),getMedianLine(a,c));
      *pSquareRadius = getSquareDistance(a,b);
    break;
    case 1:
      intersection = getIntersection(getMedianLine(b,a),getMedianLine(b,c));
      *pSquareRadius = getSquareDistance(b,a);
    break;
    case 2:
      intersection = getIntersection(getMedianLine(c,a),getMedianLine(c,b));
      *pSquareRadius = getSquareDistance(c,a);
    break;
  }
  if (isinf(intersection.x))
  {
    printf("WARNING: circumcenter with infinite radius\n");
    *pSquareRadius=INFINITY;
  }
  return intersection;
}




#if 0


#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
static size_t getLength(char* s, size_t* pSize)
{
    int i,result;
    for (result=0,i=0;s[i];i++)
    {
        if (s[i]!='-')
            result++;
    }
    *pSize=i;
    return result;
}

char* licenseKeyFormatting(char* S, int K)
{
    int i,j,first,nDashes,count;
    size_t n,size;
    char* result,*temp;
    n = getLength(S,&size);
    first = n % K;
    nDashes = (n / K)- 1;
    if (first)
        nDashes++;
    printf("first==%i,n==%i,size==%i\n",first,n,size);
    result = (char*)malloc((nDashes+n+1)*sizeof(char));
    if (result)
    {
        result[nDashes+n]=0;
        for (i=0,j=0;i<first;)
        {
            if (S[j]!='-')
            {
                if (isalpha(S[j]))
                    result[i]=toupper(S[j]);
                else
                    result[i]=S[j];
                i++;
            }
            j++;
        }
        if (result[i+1])
        {
            if (first)
            {
                printf("Putting the first dash\n");
                result[i]='-';
                i++;
            }
        }

        temp = &result[i];
        for (;S[j];)
        {
            for (i=0;S[j];)
            {
                if ((i>0)&&(i%K==0))
                {

                    temp[i]='-';
                    i++;
                    break;
                }
                if (S[j]!='-')
                {
                    if (isalpha(S[j]))
                        temp[i]=toupper(S[j]);
                    else
                        temp[i]=S[j];
                    i++;
                }
                j++;
            }
            temp = &temp[i];
        }
    }
    printf("strlen(temp)==%i\n",strlen(temp));
    return result;
}
#endif


static int delaunay(int* pT, Point* pArray, int numPoints)
{
  int i;
  Point a,b,c,center;
  double squareRadius;
  a = pArray[pT[0]];
  b = pArray[pT[1]];
  c = pArray[pT[2]];
  center = getCircumcenter(a,b,c,&squareRadius);
  for (i=0;i<numPoints;i++)
  {
    if ((samePoint(pArray[i],a)==0) && (samePoint(pArray[i],b)==0) && (samePoint(pArray[i],c)==0))
    {
      if (getSquareDistance(center,pArray[i])<squareRadius)
      {
        return 0;
      }
    }
  }
  return 1;
}

static unsigned binomial(unsigned n, unsigned k)
{
  unsigned i;
  unsigned result;
  if (k>n)
  {
    printf("ERROR: invalid binomial arguments\n");
    return 0;
  }
  if (k==0)
    return 1;
  for (result=1,i=0;i<k;i++)
  {
    result = result * (n-i);
  }
  for (;k>1;k--)
  {
    result = result / k;
  }
  return result;
}

static int getThirdVertex(int a, int b)
{
  printf("third(%i,%i)\n",a,b);
  switch (a)
  {
    case 0:
      switch (b)
      {
        case 1: return 2;
        case 2: return 1;
        default: return -1;
      }
    break;
    case 1:
      switch (b)
      {
        case 0: return 2;
        case 2: return 0;
        default: return -1;
      }
    break;
    case 2:
      switch (b)
      {
        case 0: return 1;
        case 1: return 0;
        default: return -1;
      }
    break;
    default: return -1;
  }
}

static int redundant(Point a1, Point a2, Point a3, Point b1, Point b2, Point b3)
{
  struct Line edge;
  int i,j,k,l,t1,t2;
  Point first[3];
  Point second[3];
  Point third1, third2;
  first[0] = a1;
  first[1] = a2;
  first[2] = a3;
  second[0] = b1;
  second[1] = b2;
  second[2] = b3;
  for (i=0;i<3;i++)
  {
    for (j=i+1;j<3;j++)
    {
      for (k=0;k<3;k++)
      {
        for (l=k+1;l<3;l++)
        {
          if ((samePoint(first[i],second[k])==1) && (samePoint(first[j],second[l])==1))
          {
            printf("Common edge: (%f,%f)-(%f,%f)\n",first[i].x,first[i].y,first[j].x,first[j].y);
            t1 = getThirdVertex(i,j);
            t2 = getThirdVertex(k,l);
            printf("t1==%i, t2==%i\n",t1,t2);
            third1 = first[t1];
            third2 = second[t2];
            printf("Other vertex: (%f,%f)\n",third1.x,third1.y);
            printf("Other vertex: (%f,%f)\n",third2.x,third2.y);
            edge = getStraightLine(first[i],first[j]);
            printf("Common edge line: %f x + %f\n",edge.angular,edge.offset);
            if (areParted(third1,third2,edge))
              return 0;
            else
              return 1;
          }
        }
      }
    }
  }
  return 0;
}
#define MAX_POINTS 100
static int** generateTriplets(unsigned n, unsigned* pLength)
{
  int i,j,k,count;
  unsigned length;
  int** result;
  if (n>MAX_POINTS)
  {
    printf("ERROR: too many triplets\n");
    *pLength = 0;
    return NULL;
  }
  if (n<3)
  {
    *pLength = 0;
    return NULL;
  }
  length = binomial(n,3);
  *pLength = length;
  result = (int**)calloc(length,sizeof(int*));
  if (result)
  {
    count = 0;
    for (i=0;i<n;i++)
    {
      for (j=i+1;j<n;j++)
      {
        for (k=j+1;k<n;k++)
        {
          result[count]=(int*)malloc(3*sizeof(int));
          if (result[count])
          {
            result[count][0] = i;
            result[count][1] = j;
            result[count][2] = k;
            count++;
          }
          else
          {
            printf("ERROR: malloc failed for %i,%i,%i\n",i,j,k);
            return result;
          }
        }
      }
    }
  }
  return result;
}

static int defrag(int** p, int length)
{
  int i,lastFree,newLength;
  for (i=0,lastFree=-1;i<length;i++)
  {
    if (lastFree<0)
    {
      if (p[i])
      {
        continue;
      }
      else
      {
        lastFree = i;
      }
    }
    else
    {
      if (p[i])
      {
        p[lastFree++]=p[i];
        p[i]=NULL;
        newLength = lastFree;
      }
      else
      {
        continue;
      }
    }
  }
  if (lastFree<0)
    return length;
  return newLength;
}

static void deleteTriplets(int** p, unsigned length)
{
  int i;
  for (i=0;i<length;i++)
  {
    if (p[i])
      free(p[i]);
  }
  free(p);
}

int main(int argc, char* argv[])
{
  int i,n;
  Point a = {2.0,2.0};
  Point b = {5.0,5.0};
  Point c = {9.0,4.0};
  Point d = {6.0,1.0};
  printf("redundant==%i\n",redundant(a,b,c,a,c,d));




  return 0;
}
