#include <stdio.h>




static int reduceToHalf(int size)
{
  if (size>1)
  {
    return (size%2)?1+size/2:size/2;
  }
  else
    return 0;
}

static int findPosition(int *pNum, int size, int val)
{
  int i;
  int upper = size-1;
  size=reduceToHalf(size);
  for (i=size;;)
  {
/*
    printf("Comparing with input[%i]==%i, size==%i\n",i,pNum[i],size);
*/
    if (val>pNum[i])
    {
      if (size==1)
      {
        return i+1;
      }
      else
      {
        size=reduceToHalf(size);
        i = i + size;
        if (i>upper)
          i=upper;
      }
    }
    else
    {
      if (size==1)
      {
        return i;
      }
      else
      {
        size=reduceToHalf(size);
        i = i - size;
        if (i<0)
          i=0;
      }
    }
  }
  return -1;
}


static int findTargetPosition(int* nums1, int nums1Size, int* nums2, int nums2Size, int targetPosition)
{
  int i,pos,upper;;
  upper = nums2Size-1;
  nums2Size=reduceToHalf(nums2Size);
  for (i=nums2Size;nums2Size>0;)
  {
    pos = i + findPosition(nums1,nums1Size,nums2[i]);
    if (pos==targetPosition)
    {
      printf("find(1, ... %i) + %i == %i\n",nums2[i],i,pos);
      return pos;
    }

    nums2Size=reduceToHalf(nums2Size);
    if (nums2Size==0)
      return -1;

    if (pos>targetPosition)
    {
      i = i - nums2Size;
      if (i<0)
        i=0;
    }
    else
    {
      i = i + nums2Size;
      if (i>upper)
        i=upper;
    }
  }
  return -1;
}

double findMedianSortedArrays(int* nums1, int nums1Size, int* nums2, int nums2Size)
{
  int totalLength,medianPosition1,medianPosition2;
  int targetPosition,first,second;
  totalLength = nums1Size + nums2Size;
  if (totalLength%2)
  {
      medianPosition1 = totalLength/2;
      medianPosition2 = medianPosition1;
  }
  else
  {
      medianPosition2 = totalLength/2;
      medianPosition1 = medianPosition2-1;
  }
  targetPosition = findTargetPosition(nums1,nums1Size,nums2,nums2Size,medianPosition1);
  if (targetPosition<0)
  {
    targetPosition = findTargetPosition(nums2,nums2Size,nums1,nums1Size,medianPosition1);
    if (targetPosition<0)
    {
      return -1.0;
    }
    else
    {
      first = nums2[targetPosition];
    }
  }
  else
  {
    first = nums2[targetPosition];
  }
  if (medianPosition1==medianPosition2)
    return (double)first;
  else
  {
    targetPosition = findTargetPosition(nums1,nums1Size,nums2,nums2Size,medianPosition2);
    if (targetPosition<0)
    {
      targetPosition = findTargetPosition(nums2,nums2Size,nums1,nums1Size,medianPosition2);
      if (targetPosition<0)
      {
        return -1.0;
      }
      else
      {
        second = nums2[targetPosition];
      }
    }
    else
    {
      second = nums2[targetPosition];
    }
    return (double)(first+second)/2.0;
  }
}



int main(int argc, char* argv[])
{
  int a1[]={2,4,6,8,10,12,14,16,18,20};
  int a2[]={3,5,7,9,11,13};
  int n1,n2;
  n1 = sizeof(a1)/sizeof(int);
  n2 = sizeof(a2)/sizeof(int);
  printf("median==%f\n",findMedianSortedArrays(a1,n1,a2,n2));
  return 0;
}
