#include <stdio.h>
#include <stdlib.h>
/*---------------------------------------------------------------------*/
typedef struct { unsigned *pixels; unsigned width, height; } Image;
typedef struct { unsigned *values; unsigned size; } unsigned_array;
unsigned_array central_pixels(Image,unsigned);
/*---------------------------------------------------------------------*/

static Image getDepthImage(Image src)
{
  Image result;
  size_t totalSize;
  result.width = src.width;
  result.height = src.height;
  totalSize = result.width * result.height;
  result.pixels = (unsigned*)calloc(totalSize,sizeof(unsigned));
  if (result.pixels==NULL)
  {
    result.width=0;
    result.height=0;
    printf("ERROR: calloc\n");
  }
  return result;
}

static void markBoundaries(Image src, Image dest)
{
  int i,j,first,upper,pos;
  upper = src.width;
  for (i=0;i<upper;++i)
  {
    dest.pixels[i]=1;
  }
  first = src.width * (src.height - 1);
  upper = src.width * src.height;
  for (i=first;i<upper;++i)
  {
    dest.pixels[i]=1;
  }
  upper = src.height - 1;
  for (i=1;i<upper;++i)
  {
    pos = i*src.width;
    dest.pixels[pos]=1;
    dest.pixels[pos + src.width - 1] = 1;
  }
}

static void markEdges(Image src, Image dest)
{
  int i,j,first,upper,pos;
  for (i=1;i<src.height-1;++i)
  {
    for (j=1;j<src.width-1;++j)
    {
      pos = i * src.width + j;
      if (dest.pixels[pos]==0)
      {
        if (src.pixels[pos]!=src.pixels[pos+1])
        {
          dest.pixels[pos]=1;
          dest.pixels[pos+1]=1;
        }
        if (src.pixels[pos]!=src.pixels[pos-1])
        {
          dest.pixels[pos]=1;
          dest.pixels[pos-1]=1;
        }
        if (src.pixels[pos]!=src.pixels[pos+src.width])
        {
          dest.pixels[pos]=1;
          dest.pixels[pos+src.width]=1;
        }
        if (src.pixels[pos]!=src.pixels[pos-src.width])
        {
          dest.pixels[pos]=1;
          dest.pixels[pos-src.width]=1;
        }
      }
    }
  }
}


unsigned* addPosition(unsigned *p, unsigned pos, unsigned *pSize)
{
  static unsigned size=0;
  if (p==NULL)
  {
    p=(unsigned*)malloc(sizeof(unsigned));
    if (p==NULL)
    {
      printf("ERROR: malloc 2\n");
      return NULL;
    }
    size = 1;
    *pSize=size;
    p[0] = pos;
    return p;
  }
  else
  {
    size+=1;
    p = (unsigned*)realloc(p,size*sizeof(unsigned));
    if (p==NULL)
    {
      printf("ERROR: malloc 3\n");
      return NULL;
    }
    p[size-1]=pos;
    *pSize=size;
    return p;
  }
}

static unsigned* findDepthOne(Image src, Image dest, unsigned colour, unsigned* pCount)
{
  int i,j,pos;
  unsigned* result=NULL;
  unsigned count = 0;
  for (i=0;i<src.height;++i)
  {
    for (j=0;j<src.width;++j)
    {
      pos = i * src.width + j;
      if (src.pixels[pos]==colour)
      {
        if (dest.pixels[pos]==1)
        {
          result = addPosition(result,pos,&count);
        }
      }
    }
  }
  pCount[0]=count;
  return result;
}


static unsigned* markDepth(Image src, Image dest, unsigned colour, unsigned depth, unsigned* pCount)
{
  int i,j,pos;
  unsigned* result=NULL;
  unsigned count = 0;
  for (i=1;i<src.height-1;++i)
  {
    for (j=1;j<src.width-1;++j)
    {
      pos = i * src.width + j;

      if (dest.pixels[pos]==0)
      {
        if (src.pixels[pos] == colour)
        {
          if ((dest.pixels[pos+1]==depth-1) ||
              (dest.pixels[pos-1]==depth-1) ||
              (dest.pixels[pos+src.width]==depth-1) ||
              (dest.pixels[pos-src.width]==depth-1))
          {
            dest.pixels[pos]=depth;
            result = addPosition(result,pos,&count);
          }
        }
      }
    }
  }
  pCount[0]=count;
  return result;
}


unsigned_array central_pixels(Image image, unsigned colour)
{
  unsigned_array result;
  Image depthImage;
  unsigned i,maxDepth,count;
  unsigned* temp;
  if ((image.width<3) || (image.height<3))
  {
    result.values = (unsigned*)malloc(sizeof(unsigned));
    result.values[0]=1;
    if ((image.width<1) || (image.height<1))
      result.size = 0;
    else
      result.size = 1;
    return result;
  }
  depthImage = getDepthImage(image);
  if (depthImage.pixels==NULL)
  {
    printf("ERROR: malloc\n");
    return result;
  }
  markBoundaries(image,depthImage);
  markEdges(image,depthImage);
  maxDepth = (image.height<image.width)?image.height:image.width;
  if (maxDepth%2)
    maxDepth = 1+maxDepth/2;
  else
    maxDepth = maxDepth/2;
  result.values=findDepthOne(image,depthImage,colour,&result.size);
  for (i=2;i<=maxDepth;++i)
  {
    temp = markDepth(image,depthImage,colour,i,&count);
    if (temp==NULL)
      break;
    else
    {
      if (result.values)
        free(result.values);
      result.size = count;
      result.values = temp;
    }
  }
  if (result.values==NULL)
  {
    result.values=(unsigned*)malloc(sizeof(unsigned));
  }
  return result;
}


int main(int argc, char* argv[])
{
  unsigned image_data[] = {1,1,4,4,4,4,2,2,2,2,
                         1,1,1,1,2,2,2,2,2,2,
                         1,1,1,1,2,2,2,2,2,2,
                         1,1,1,1,1,3,2,2,2,2,
                         1,1,1,1,1,3,3,3,2,2,
                         1,1,1,1,1,1,3,3,3,3};
Image image = { image_data, 10, 6 };
unsigned_array expected = central_pixels(image,3);

  return 0;
}
