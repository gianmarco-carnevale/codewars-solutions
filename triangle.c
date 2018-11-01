#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char triangle(const char *row)
{
  int i,j,n;
  char *res;
  char result;
  static char _array[22]={0,0,0,0,0,'R',0,0,0,0,0,0,0,0,0,0,'G',0,0,0,0,'B'};
  n = strlen(row);
  result=0;
  if (n<1)
    return 0;
  if (n==1)
    return *row;
  n--;
  res = (char*)malloc(n*sizeof(char));
  if (res)
  {
    printf("%s\n",row);
    for (i=0;i<n;i++)
    {
        _array[0]=row[i];
        res[i]=_array[row[i]^row[i+1]];
    }
    res[i]=0;
    for (i=n-1;i>0;i--)
    {
      printf("%s\n",res);
      for (j=0;j<i;j++)
      {
        _array[0]=res[j];
        res[j]=_array[res[j]^res[j+1]];
      }
      res[j]=0;

    }
    result = res[0];
    free(res);
  }
  return result;
}





/*
       R B R G B R B
        G G B R G G
         G R G B G
          B B R R
           B G R
            R B
             G


     3^5 = 6
     3^6 = 5
     6^5 = 3

     3 ^ 5 nand 5 =   110 nand 101 = 11 = 3

     3^3 = 0

     3 ^ 5 & 5 | 5

     0x356 ^ 0x653 == 0x505      AFA      1010
                                            11
                                           110
     0x356 | 0x653 == 0x757
                       0x757 | 0x888 == 0xFDF      1101



     7 ^ 5 =


     3&5 = 1 + 5
     3&6 = 2 + 3
     5&6 = 4 - 1




52 47 42
6   5  3



*/

int main(int argc, char* argv[])
{
  char c = triangle("BBRGBRBGBRBRBGBRBRB");
  printf("%c\n",c);

  return 0;
}
