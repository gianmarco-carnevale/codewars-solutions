#include <stdio.h>



static int findPosition(int *pNum, int size, int val)
{
  int i;
  int upper = size-1;
  size=(size%2)?1+size/2:size/2;
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
        size=(size%2)?1+size/2:size/2;
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
        size=(size%2)?1+size/2:size/2;
        i = i - size;
        if (i<0)
          i=0;
      }
    }
  }
  return -1;
}






double findMedianSortedArrays(int* nums1, int nums1Size, int* nums2, int nums2Size)
{
    int totalLength,medianPosition1, medianPosition2;
    int pos1,pos2;
    int targetPosition;
    int first,second;
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
    pos1 = nums1Size/2;
    pos2 = nums2Size/2;
    for (;;)
    {
      targetPosition = pos2 + findPosition(nums1,nums1Size,nums2[pos2]);
      printf("target2==%i==%i+find()\n",targetPosition,pos2);
      if (targetPosition==medianPosition1)
      {
        printf("target2 is good\n");
        pos1=-1;
        break;
      }
      pos1 += (targetPosition<medianPosition1)?1:-1;

      targetPosition = pos1 + findPosition(nums2,nums2Size,nums1[pos1]);
      printf("target2==%i==%i+find()\n",targetPosition,pos1);
      if (targetPosition==medianPosition1)
      {
        printf("target1 is good\n");
        pos2=-1;
        break;
      }
      pos2 += (targetPosition<medianPosition1)?1:-1;
    }
    if ((pos1<0)&&(pos2<0))
      return -1.0;
    if (pos1<0)
      first = nums2[pos2];
    else
      first = nums1[pos1];
      printf("first==%i\n",first);
    if (medianPosition1==medianPosition2)
      return (double)first;
    printf("medianPosition2 required\n");
    for (;;)
    {
      targetPosition = pos2 + findPosition(nums1,nums1Size,nums2[pos2]);
      printf("target2'==%i==%i+find()\n",targetPosition,pos2);
      if (targetPosition==medianPosition2)
      {
        printf("target2' is good\n");
        pos1=-1;
        break;
      }
      pos1 += (targetPosition<medianPosition2)?1:-1;

      targetPosition = pos1 + findPosition(nums2,nums2Size,nums1[pos1]);
      printf("target1'==%i==%i+find()\n",targetPosition,pos1);
      if (targetPosition==medianPosition2)
      {
        printf("target1' is good\n");
        pos2=-1;
        break;
      }
      pos2 += (targetPosition<medianPosition2)?1:-1;
    }
    if ((pos1<0)&&(pos2<0))
      return -1.0;
    if (pos1<0)
      second = nums2[pos2];
    else
      second = nums1[pos1];
    printf("second==%i\n",second);
    return (double)(first+second)/2.0;

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
