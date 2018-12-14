#include <stdio.h>
/*---------------------------------------------------------------------*/
typedef struct { unsigned *pixels; unsigned width, height; } Image;
typedef struct { unsigned *values; unsigned size; } unsigned_array;
unsigned_array central_pixels(Image,unsigned);
/*---------------------------------------------------------------------*/

struct picture_row
{
  int start;
  int length;
  int* depths;
};

struct color_area
{
  unsigned color;
  int numRows;
  struct picture_row* pRows;
};

static int compare(void* p1, void* p2)
{
  unsigned a,b;
  a = ((struct color_area*)p1)->color;
  b = ((struct color_area*)p2)->color;
  if (a<b) return -1;
  if (a>b) return 1;
  return 0;
}

static struct color_area* addArea(struct color_area* pAreas, struct color_area* pNew)
{

}

static struct color_area* findArea(struct color_area* pAreas, unsigned color)
{
  struct color_area dummy;
  dummy->color = color;
  return bsearch(&dummy);
}

static void addRow(struct color_area* pArea, int startPosition, int rowLength)
{

}

static struct color_area* getColorAreas(Image image, int* pLength)
{
  struct color_area* pResult,*temp;
  int i,j,startPosition,size,numAreas;
  unsigned lastColor,currentColor;
  for (pResult=NULL,numAreas=0,i=0;i<image.height;i++)
  {
    for (j=0;j<image.width;j++)
    {
      currentColor = image.pixels[(i*image.width)+j];
      if ((i==0)&&(j==0))
      {
        lastColor = currentColor;
        startPosition = 0;
        size=1;
      }
      else
      {
        if (lastColor == currentColor)
        {
          size++;
        }
        else
        {
          temp = findArea(pResult,currentColor);
          if (temp==NULL)
          {
            temp = (struct color_area*)malloc(sizeof(struct color_area));
            if (temp==NULL)
            {
              printf("ERROR: malloc failed.\n");
              *pLength=0;
              return NULL;
            }
            temp->color = currentColor;
            pResult = addArea(pResult,temp);
            numAreas++;
          }
          addRow(temp,startPosition,size);
          startPosition=j;
          size=1;
          lastColor = currentColor;
        }
      }
    }
  }
  *pLength = numAreas;
  return pResult;
}


static void getHorizontalDepth(struct color_area* pArea)
{
  int i,j;
  for (i=0;i<pArea->numRows;i++)
  {

  }
}


static unsigned_array getMaxDepthPositions(struct color_area* pArea, unsigned width, unsigned height)
{

}

unsigned_array central_pixels(Image image, unsigned colour)
{
  int numAreas;
  unsigned_array result;
  static struct color_area* pColorAreas=NULL;
  numAreas=0;
  if (pColorAreas==NULL)
  {
    pColorAreas = getColorAreas(image,&numAreas);
    if (pColorAreas==NULL)
    {
      printf("ERROR: failed to find color areas.\n");
      result.values=NULL;
      result.size=0;
      return result;
    }

  }
}


int main(int argc, char* argv[])
{
  return 0;
}
