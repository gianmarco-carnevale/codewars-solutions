#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct { double x,y; } Point;

struct Line
{
  double angular;
  double offset;
};

static double getTriangleArea(Point a, Point b, Point c)
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

static double getTriangleAreaFromArray(Point* pArray, int* triplet)
{
  Point p,q;
  int a,b,c;
  double result;
  a = triplet[0];
  b = triplet[1];
  c = triplet[2];
  p.x = pArray[a].x - pArray[b].x;
  p.y = pArray[a].y - pArray[b].y;
  q.x = pArray[b].x - pArray[c].x;
  q.y = pArray[b].y - pArray[c].y;
  result = (p.x*q.y - p.y*q.x)/2.0;
  return (result<0.0)?(-result):result;
}

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
    result.offset = a.y - (result.angular * a.x);
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
        result.y = (l1.angular * result.x) + l1.offset;
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
  return (p*p)+(q*q);
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
  if (samePoint(a,b) || samePoint(b,c) || samePoint(a,c))
  {
    printf("WARNING: at least two vertices are the same\n");
    *pSquareRadius=INFINITY;
    intersection.x=INFINITY;
    intersection.y=INFINITY;
    return intersection;
  }
  int v = getVertexBetweenLongerEdges(a,b,c);
  switch (v)
  {
    case 0:
      intersection = getIntersection(getMedianLine(a,b),getMedianLine(a,c));
      *pSquareRadius = getSquareDistance(a,intersection);
    break;
    case 1:
      intersection = getIntersection(getMedianLine(b,a),getMedianLine(b,c));
      *pSquareRadius = getSquareDistance(b,intersection);
    break;
    case 2:
      intersection = getIntersection(getMedianLine(c,a),getMedianLine(c,b));
      *pSquareRadius = getSquareDistance(c,intersection);
    break;
  }
  if (isinf(intersection.x)||isinf(intersection.y))
  {
    printf("WARNING: vertices are aligned and do not form a triangle\n");
    intersection.x=INFINITY;
    intersection.y=INFINITY;
    *pSquareRadius=INFINITY;
  }
  return intersection;
}

static int delaunay(int* pTriplet, Point* pArray, int numPoints)
{
  int i;
  Point a,b,c,center;
  double squareRadius;
  a = pArray[pTriplet[0]];
  b = pArray[pTriplet[1]];
  c = pArray[pTriplet[2]];
  center = getCircumcenter(a,b,c,&squareRadius);
  /*
  printf("********* (%f,%f)(%f,%f)(%f,%f) *********\n",a.x,a.y,b.x,b.y,c.x,c.y);
  printf("Circumcenter: (%f,%f), radius: %f\n",center.x,center.y,sqrt(squareRadius));
  */
  for (i=0;i<numPoints;i++)
  {
    /*
    printf("(%f,%f) ",pArray[i].x,pArray[i].y);
    */
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
            third1 = first[t1];
            third2 = second[t2];
            printf("Other vertex: (%f,%f)\n",third1.x,third1.y);
            printf("Other vertex: (%f,%f)\n",third2.x,third2.y);
            edge = getStraightLine(first[i],first[j]);
            if (areParted(third1,third2,edge))
            {
              printf("Parted, no cancellation\n");
              return 0;
            }
            else
            {
              printf("On the same side, cancel\n");
              return 1;
            }
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

static int** getDelaunayTriangles(Point* pArray, unsigned numPoints, unsigned *pNumTriangles)
{
  unsigned numTriplets,numTriangles;
  int i,j;
  int** pTriplets;
  Point a1,a2,a3,b1,b2,b3;
  pTriplets = generateTriplets(numPoints,&numTriplets);
  if (pTriplets)
  {
    for (i=0;i<numTriplets;i++)
    {
      if (getTriangleAreaFromArray(pArray,pTriplets[i])>0.0)
      {
        if (delaunay(pTriplets[i],pArray,numPoints)==0)
        {
          free(pTriplets[i]);
          pTriplets[i]=NULL;
        }
      }
      else
      {
        free(pTriplets[i]);
        pTriplets[i]=NULL;
      }
    }
    for (i=0;i<numTriplets;i++)
    {
      for (j=i+1;j<numTriplets;j++)
      {
        if (pTriplets[i])
        {
          if (pTriplets[j])
          {
            a1 = pArray[pTriplets[i][0]];
            a2 = pArray[pTriplets[i][1]];
            a3 = pArray[pTriplets[i][2]];
            b1 = pArray[pTriplets[j][0]];
            b2 = pArray[pTriplets[j][1]];
            b3 = pArray[pTriplets[j][2]];
            if (redundant(a1,a2,a3,b1,b2,b3))
            {
              free(pTriplets[j]);
              pTriplets[j]=NULL;
            }
          }
        }
      }
    }
  }
  numTriangles = defrag(pTriplets,numTriplets);
  *pNumTriangles = numTriangles;
  if (numTriangles==0)
  {
    free(pTriplets);
    return NULL;
  }
  pTriplets = (int**)realloc(pTriplets,numTriangles*sizeof(int*));
  return pTriplets;
}

static int shareCommonEdge(int* pTriangle1, int* pTriangle2 /*, int* pv1, int* pv2*/)
{
  int i,j,k,l;
  /*
  printf("(%i,%i,%i) and (%i,%i,%i) ",pTriangle1[0],pTriangle1[1],pTriangle1[2],pTriangle2[0],pTriangle2[1],pTriangle2[2]);
  */
  for (i=0;i<3;i++)
  {
    for (j=i+1;j<3;j++)
    {
      for (k=0;k<3;k++)
      {
        for (l=k+1;l<3;l++)
        {
          if ((pTriangle1[i]==pTriangle2[k]) && (pTriangle1[j]==pTriangle2[l]))
          {
/*
            printf("share an edge\n");
            */
            return 1;
          }
        }
      }
    }
  }
  /*
  printf("do not share an edge\n");
  */
  return 0;
}

static int** getTrianglesByVertex(int** pTriangles, unsigned numTriangles, int vertex, unsigned *pNumVertexTriangles)
{
  int i,j;
  unsigned numVertexTriangles;
  int** result;
  for (i=0,numVertexTriangles=0,result=NULL;i<numTriangles;i++)
  {
    if ((vertex==pTriangles[i][0]) || (vertex==pTriangles[i][1]) || (vertex==pTriangles[i][2]))
    {
      if (result)
      {
        result = (int**)realloc(result,(numVertexTriangles+1)*sizeof(int*));
        result[numVertexTriangles++]=pTriangles[i];
      }
      else
      {
        numVertexTriangles=1;
        result = (int**)malloc(sizeof(int*));
        result[0]=pTriangles[i];
      }
    }
  }
  *pNumVertexTriangles = numVertexTriangles;
  return result;
}

static int hasVertex(int* pTriangle, int vertex)
{
  if (vertex<0)
  {
    printf("ERROR: invalid vertex");
    return 0;
  }
  if (vertex>2)
  {
    printf("ERROR: invalid vertex");
    return 0;
  }
  if (pTriangle[0]==vertex)
    return 1;
  if (pTriangle[1]==vertex)
    return 1;
  if (pTriangle[2]==vertex)
    return 1;
  return 0;
}

static void sortByVertex(int** pTriangles, unsigned numTriangles, int vertex)
{
  int i,j;
  int* temp;
  for (i=0;i<numTriangles-1;i++)
  {
    /*
    printf("checking (%i,%i,%i) with (%i,%i,%i)\n",pTriangles[i][0],pTriangles[i][1],pTriangles[i][2],pTriangles[i+1][0],pTriangles[i+1][1],pTriangles[i+1][2]);
    */
    if (shareCommonEdge(pTriangles[i],pTriangles[i+1]))
    {
      continue;
    }
    else
    {
      for (j=i+2;j<numTriangles;j++)
      {
        if (shareCommonEdge(pTriangles[i],pTriangles[j]))
        {
          /*
          printf("Found (%i,%i,%i) to swap with (%i,%i,%i)\n",pTriangles[j][0],pTriangles[j][1],pTriangles[j][2],pTriangles[i+1][0],pTriangles[i+1][1],pTriangles[i+1][2]);
          */
          temp = pTriangles[i+1];
          pTriangles[i+1] = pTriangles[j];
          pTriangles[j] = temp;
          break;
        }
      }
    }
  }
}

static double voronoiArea(Point* pArray, int** pTriangles, unsigned numTriangles, int vertex)
{
  int i;
  Point a1,a2,a3,b1,b2,b3,c1,c2;
  double sum,sqRadius;
  if (numTriangles<3)
    return -1.0;
  sortByVertex(pTriangles,numTriangles,vertex);
  if (shareCommonEdge(pTriangles[0],pTriangles[numTriangles-1])==0)
    return -1.0;

  for (sum=0,i=1;i<numTriangles;i++)
  {
    a1 = pArray[pTriangles[i-1][0]];
    a2 = pArray[pTriangles[i-1][1]];
    a3 = pArray[pTriangles[i-1][2]];
    b1 = pArray[pTriangles[i][0]];
    b2 = pArray[pTriangles[i][1]];
    b3 = pArray[pTriangles[i][2]];
    c1 = getCircumcenter(a1,a2,a3,&sqRadius);
    c2 = getCircumcenter(b1,b2,b3,&sqRadius);
    sum += getTriangleArea(c1,c2,pArray[vertex]);
  }
  a1 = pArray[pTriangles[numTriangles-1][0]];
  a2 = pArray[pTriangles[numTriangles-1][1]];
  a3 = pArray[pTriangles[numTriangles-1][2]];
  b1 = pArray[pTriangles[0][0]];
  b2 = pArray[pTriangles[0][1]];
  b3 = pArray[pTriangles[0][2]];
  c1 = getCircumcenter(a1,a2,a3,&sqRadius);
  c2 = getCircumcenter(b1,b2,b3,&sqRadius);
  sum += getTriangleArea(c1,c2,pArray[vertex]);
  return sum;
}

int main(int argc, char* argv[])
{
  int i,j;
  unsigned Npoints;
  unsigned numTriangles, numVertexTriangles;
  int ** pTriangles, **pVertexTriangles;
  double r;

  Point a1,a2,a3,b1,b2,b3;
  Point array[] = {
  {0.0, 0.0}, {2.0, 0.0}, {-2.0, 0.0}, {0.0, 2.0}, {0.0, -2.0}
  };
  Npoints = sizeof(array)/sizeof(Point);

  pTriangles = getDelaunayTriangles(array,Npoints,&numTriangles);
  if (pTriangles)
  {

    printf("Got %d triangles\n",numTriangles);
    for (i=0;i<numTriangles;i++)
    {
      printf("(%f,%f)-",array[pTriangles[i][0]].x,array[pTriangles[i][0]].y);
      printf("(%f,%f)-",array[pTriangles[i][1]].x,array[pTriangles[i][1]].y);
      printf("(%f,%f)\n",array[pTriangles[i][2]].x,array[pTriangles[i][2]].y);
    }
#if 1
    for (i=0;i<Npoints;i++)
    {
      pVertexTriangles = getTrianglesByVertex(pTriangles,numTriangles,i,&numVertexTriangles);
      printf("--------------------------------------------------\n");

      printf("Vertex %i: ",i);
      for (j=0;j<numVertexTriangles;j++)
      {
        printf("%i,%i,%i - ",pVertexTriangles[j][0],pVertexTriangles[j][1],pVertexTriangles[j][2]);
      }
      printf(", Voronoi area: %f\n",voronoiArea(array,pVertexTriangles,numVertexTriangles,i));
    }
#endif
  }


  return 0;
}
