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

typedef struct
{
  int sides[3];
} Triangle;





struct Line
{
  double angular;
  double offset;
};

static struct Line getLine(Point a, Point b)
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

double _triangleArea(Point a, Point b, Point c)
{
  Point d;
  double result;
  d.x = a.x - b.x;
  d.y = a.y - b.y;
  a.x = b.x - c.x;
  a.y = b.y - c.y;
  result = (d.x*a.y - d.y*a.x)/2.0;
  return (result<0.0)?(-result):result;
}

static double triangleArea(Point* pArray, int a, int b, int c)
{
  Point p,q;
  double result;
  p.x = pArray[a]->x - pArray[b]->x;
  p.y = pArray[a]->y - pArray[b]->y;
  q.x = pArray[b]->x - pArray[c]->x;
  q.y = pArray[b]->y - pArray[c]->y;
  result = (p.x*q.y - p.y*q.x)/2.0;
  return (result<0.0)?(-result):result;
}

static int isInside(Point* pArray, Triangle* pT, int pointIndex)
{
  double area,sum;
  area = triangleArea(pArray,pT->sides[0],pT->sides[1],pT->sides[2]);
  sum  = triangleArea(pT->sides[0],pT->sides[1],pointIndex);
  sum += triangleArea(pT->sides[1],pT->sides[2],pointIndex);
  sum += triangleArea(pT->sides[0],pT->sides[2],pointIndex);
  if (area == sum)
    return 1;
  return 0;
}

static Triangle* createNewTriangle(int a, int b ,int c)
{
  Triangle* result;
  result = (Triangle*)malloc(sizeof(Triangle));
  if (result)
  {
    result->sides[0]=a;
    result->sides[1]=b;
    result->sides[2]=c;
  }
  return result;
}

static double getSquareDistance(Point* pArray, int a, int b)
{
  double p,q;
  p = pArray[a]->x - pArray[b]->x;
  q = pArray[a]->y - pArray[b]->y;
  return (p*p+q*q);
}

static int findClosestTriangle(Point* pArray,Triangle** pTrArray, int pointIndex, int *pIsInside)
{
  int i,j,n,closest;
  Triangle* pTriangle;
  double sqDist=INFINITY;
  double temp;
  *pIsInside=0;
  for (i=0,pTriangle=pTrArray[i];pTriangle;i++,pTriangle=pTrArray[i])
  {
    if (isInside(pArray,pTriangle,pointIndex))
    {
      *pIsInside=1;
      return i;
    }
    for (j=0;j<3;j++)
    {
      temp = getSquareDistance(pArray,pointIndex,pTriangle->sides[j]);
      if (temp<sqDist)
      {
        sqDist = temp;
        closest=i;
      }
    }
  }
  return closest;
}

Triangle** joinPointsIntoTriangles(Point* pArray, int numPoints)
{
  Triangle** result;
  /* this pointer will be non-null if there is at least one triangle */
  /* if no triangles can be formed, null pointer shall be returned */
  int i,j,numTriangles,index,closest,isInside;
  if (pArray==NULL)
    return NULL;
  if (numPoints<3)
    return NULL;
  /* so from now on it is guaranteed that numPoints is equal to 3 or greater */
  numTriangles = numPoints-2;
  result = (Triangle**)malloc(numTriangles*sizeof(Triangle*));
  if (result)
  {
    for (i=0;i<numTriangles;i++)
    {
      result[i]=NULL;
    }

    for (index=0,i=0;i<numPoints;i++)
    {
      /* check if there are triangles available */
      if (index>0)
      {
        /* find the closest triangle */
        closest = findClosestTriangle(pArray,result,i,&isInside);
        if (isInside)
        {

        }
        else
        {
          /* now let's choose the common side
             between the existing triangle and the new one which we are about to create */
        }
        /* let's also connect vertex of existing triangles to form new ones */



      }
      else
      {
        /* no triangles, just aligned points which cannot form any triangle
           let's see if a new triangle can be formed now */
        if ((i>2) && (triangleArea(pArray,i,i-1,i-2)>0.0))
        {
          result[index]=createNewTriangle(i,i-1,i-2);
          if (result[index])
          {
            index++;
          }
          else
          {
            printf("ERROR: malloc\n");
            return result;
          }
        }
      }
    }
  }
  return result;
}


int main(int argc, char* argv[])
{
  return 0;
}
