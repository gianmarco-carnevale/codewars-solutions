#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

struct Square
{
  uint64_t rows[7];
};


/*

#define  SIZE  7

static int clues[][SIZE * 4] = {
  { 7, 0, 0, 0, 2, 2, 3,
    0, 0, 3, 0, 0, 0, 0,
    3, 0, 3, 0, 0, 5, 0,
    0, 0, 0, 0, 5, 0, 4 },
  { 0, 2, 3, 0, 2, 0, 0,
    5, 0, 4, 5, 0, 4, 0,
    0, 4, 2, 0, 0, 0, 6,
    5, 2, 2, 2, 2, 4, 1 }
};





static int expected[][SIZE][SIZE] = {
  { { 1, 5, 6, 7, 4, 3, 2 },
    { 2, 7, 4, 5, 3, 1, 6 },
    { 3, 4, 5, 6, 7, 2, 1 },
    { 4, 6, 3, 1, 2, 7, 5 },
    { 5, 3, 1, 2, 6, 4, 7 },
    { 6, 2, 7, 3, 1, 5, 4 },
    { 7, 1, 2, 4, 5, 6, 3 } },
    
    
    
  { { 7, 6, 2, 1, 5, 4, 3 },
    { 1, 3, 5, 4, 2, 7, 6 },
    { 6, 5, 4, 7, 3, 2, 1 },
    { 5, 1, 7, 6, 4, 3, 2 },
    { 4, 2, 1, 3, 7, 6, 5 },
    { 3, 7, 6, 2, 1, 5, 4 },
    { 2, 4, 3, 5, 6, 1, 7 } }
};

  +-----------------------------+
  | .7  .6  .2  .1  .5  .4  .3  |  
  |                             |
  | .1  .3  .5  .4  .2  .7  .6  |  
  |                             |
  | .6  .5  .4  .7  .3  .2  .1  |  
  |                             |
  | .5  .1  .7  .6  .4  .3  .1  |  
  |                             |
  | .4  .2  .1  .3  .7  .6  .5  |  
  |                             |
  | .3  .7  .6  .2  .1  .5  .4  |  
  |                             |
  | .2  .4  .3  .5  .6  .1  .7  |  
  +-----------------------------+


*/



#define  INIT_MASK                   ((uint64_t)0x7F7F7F7F7F7F7FULL)
#define  SHIFT_AMOUNT_BY_COLUMN(c)   ((6 - c) * 8)




static uint64_t getBitResetRowMask(int value)
{
  switch (value)
  {
    case 1:  return (uint64_t)0x7E7E7E7E7E7E7EULL;
    case 2:  return (uint64_t)0x7D7D7D7D7D7D7DULL;
    case 3:  return (uint64_t)0x7B7B7B7B7B7B7BULL;
    case 4:  return (uint64_t)0x77777777777777ULL;
    case 5:  return (uint64_t)0x6F6F6F6F6F6F6FULL;
    case 6:  return (uint64_t)0x5F5F5F5F5F5F5FULL;
    case 7:  return (uint64_t)0x3F3F3F3F3F3F3FULL;
    default:
      printf("ERROR: invalid value %i in getBitResetRowMask\n",value);
      return 0;
  }
}

static uint64_t getValueResetRowMask(int column)
{
  switch (column)
  {
    case 0:  return (uint64_t)0x007F7F7F7F7F7FULL;
    case 1:  return (uint64_t)0x7F007F7F7F7F7FULL;
    case 2:  return (uint64_t)0x7F7F007F7F7F7FULL;
    case 3:  return (uint64_t)0x7F7F7F007F7F7FULL;
    case 4:  return (uint64_t)0x7F7F7F7F007F7FULL;
    case 5:  return (uint64_t)0x7F7F7F7F7F007FULL;
    case 6:  return (uint64_t)0x7F7F7F7F7F7F00ULL;
    default:
      printf("ERROR: invalid column %i in getValueResetRowMask\n",column);
      return 0;
  }
}

static uint64_t getBitResetMask(int value)
{
  switch (value)
  {
    case 1:  return (uint64_t)0x7E;
    case 2:  return (uint64_t)0x7D;
    case 3:  return (uint64_t)0x7B;
    case 4:  return (uint64_t)0x77;
    case 5:  return (uint64_t)0x6F;
    case 6:  return (uint64_t)0x5F;
    case 7:  return (uint64_t)0x3F;
    default:
      printf("ERROR: invalid value %i in getBitResetMask\n",value);
      return 0;
  }
}

static int setValue(struct Square *p, unsigned int value, int row, int column)
{
  int i;
  uint64_t setMask, resetMask;
  setMask   = ((uint64_t)(1 << (value-1))) << SHIFT_AMOUNT_BY_COLUMN(column);
  resetMask  = getValueResetRowMask(column);
  resetMask |= getBitResetMask(value) << SHIFT_AMOUNT_BY_COLUMN(column);
  if (p->rows[row] & setMask)
  {
    p->rows[row] &= getBitResetRowMask(value);/* block the value along all the row */
    p->rows[row] &= getValueResetRowMask(column);/* wipe out the column value  */
    p->rows[row] |= setMask;/* set the value for that column */
    
    for (i=0;i<7;++i)
    {
      if (i!=row)
      {
        p->rows[i] &= resetMask;
      }
    }
    return 0;
  }
  printf("ERROR: cannot set value %d at position [%i][%i], see row: %llX\n",value,row,column,p->rows[row]);
  return -1;
}

static void setClueByRow(struct Square* p, int clue, int row, int reversed)
{
  if (reversed)
  {
    switch (clue)
    {
      case 2: p->rows[row] &= (uint64_t)0x7F7F7F7F7F5F3FULL; break;
      case 3: p->rows[row] &= (uint64_t)0x7F7F7F7F7F3F1FULL; break;
      case 4: p->rows[row] &= (uint64_t)0x7F7F7F7F3F1F0FULL; break;
      case 5: p->rows[row] &= (uint64_t)0x7F7F7F3F1F0F07ULL; break;
      case 6: p->rows[row] &= (uint64_t)0x7F7F3F1F0F0703ULL; break;
      default:break;
    }
  }
  else
  {
    switch (clue)
    {
      case 2: p->rows[row] &= (uint64_t)0x3F5F7F7F7F7F7FULL; break;
      case 3: p->rows[row] &= (uint64_t)0x1F3F7F7F7F7F7FULL; break;
      case 4: p->rows[row] &= (uint64_t)0x0F1F3F7F7F7F7FULL; break;
      case 5: p->rows[row] &= (uint64_t)0x070F1F3F7F7F7FULL; break;
      case 6: p->rows[row] &= (uint64_t)0x03070F1F3F7F7FULL; break;
      default:break;
    }
  }
}

static void setClueByColumn(struct Square* p, int clue, int column, int reversed)
{
  int* idx;
  int straightIndexes[5] = {0,1,2,3,4};
  int reversedIndexes[5] = {6,5,4,3,2};
  if (reversed)
  {
    idx = &reversedIndexes[0];
  }
  else
  {
    idx = &straightIndexes[0];
  }
  switch (clue)
  {
    case 2:
      switch (column)
      {
        case 0:
          p->rows[idx[0]] &= (uint64_t)0x3F7F7F7F7F7F7FULL;
          p->rows[idx[1]] &= (uint64_t)0x5F7F7F7F7F7F7FULL;
        break;
        case 1:
          p->rows[idx[0]] &= (uint64_t)0x7F3F7F7F7F7F7FULL;
          p->rows[idx[1]] &= (uint64_t)0x7F5F7F7F7F7F7FULL;
        break;
        case 2:
          p->rows[idx[0]] &= (uint64_t)0x7F7F3F7F7F7F7FULL;
          p->rows[idx[1]] &= (uint64_t)0x7F7F5F7F7F7F7FULL;
        break;
        case 3:
          p->rows[idx[0]] &= (uint64_t)0x7F7F7F3F7F7F7FULL;
          p->rows[idx[1]] &= (uint64_t)0x7F7F7F5F7F7F7FULL;
        break;
        case 4:
          p->rows[idx[0]] &= (uint64_t)0x7F7F7F7F3F7F7FULL;
          p->rows[idx[1]] &= (uint64_t)0x7F7F7F7F5F7F7FULL;
        break;
        case 5:
          p->rows[idx[0]] &= (uint64_t)0x7F7F7F7F7F3F7FULL;
          p->rows[idx[1]] &= (uint64_t)0x7F7F7F7F7F5F7FULL;
        break;
        case 6:
          p->rows[idx[0]] &= (uint64_t)0x7F7F7F7F7F7F3FULL;
          p->rows[idx[1]] &= (uint64_t)0x7F7F7F7F7F7F5FULL;
        break;
        default:break;
      }
    break;
    case 3:
      switch (column)
      {
        case 0:
          p->rows[idx[0]] &= (uint64_t)0x1F7F7F7F7F7F7FULL;
          p->rows[idx[1]] &= (uint64_t)0x3F7F7F7F7F7F7FULL;
        break;
        case 1:
          p->rows[idx[0]] &= (uint64_t)0x7F1F7F7F7F7F7FULL;
          p->rows[idx[1]] &= (uint64_t)0x7F3F7F7F7F7F7FULL;
        break;
        case 2:
          p->rows[idx[0]] &= (uint64_t)0x7F7F1F7F7F7F7FULL;
          p->rows[idx[1]] &= (uint64_t)0x7F7F3F7F7F7F7FULL;
        break;
        case 3:
          p->rows[idx[0]] &= (uint64_t)0x7F7F7F1F7F7F7FULL;
          p->rows[idx[1]] &= (uint64_t)0x7F7F7F3F7F7F7FULL;
        break;
        case 4:
          p->rows[idx[0]] &= (uint64_t)0x7F7F7F7F1F7F7FULL;
          p->rows[idx[1]] &= (uint64_t)0x7F7F7F7F3F7F7FULL;
        break;
        case 5:
          p->rows[idx[0]] &= (uint64_t)0x7F7F7F7F7F1F7FULL;
          p->rows[idx[1]] &= (uint64_t)0x7F7F7F7F7F3F7FULL;
        break;
        case 6:
          p->rows[idx[0]] &= (uint64_t)0x7F7F7F7F7F7F1FULL;
          p->rows[idx[1]] &= (uint64_t)0x7F7F7F7F7F7F3FULL;
        break;
        default:break;
      }
    break;
    case 4:
      switch (column)
      {
        case 0:
          p->rows[idx[0]] &= (uint64_t)0x0F7F7F7F7F7F7FULL;
          p->rows[idx[1]] &= (uint64_t)0x1F7F7F7F7F7F7FULL;
          p->rows[idx[2]] &= (uint64_t)0x3F7F7F7F7F7F7FULL;
        break;
        case 1:
          p->rows[idx[0]] &= (uint64_t)0x7F0F7F7F7F7F7FULL;
          p->rows[idx[1]] &= (uint64_t)0x7F1F7F7F7F7F7FULL;
          p->rows[idx[2]] &= (uint64_t)0x7F3F7F7F7F7F7FULL;
        break;
        case 2:
          p->rows[idx[0]] &= (uint64_t)0x7F7F0F7F7F7F7FULL;
          p->rows[idx[1]] &= (uint64_t)0x7F7F1F7F7F7F7FULL;
          p->rows[idx[2]] &= (uint64_t)0x7F7F3F7F7F7F7FULL;
        break;
        case 3:
          p->rows[idx[0]] &= (uint64_t)0x7F7F7F0F7F7F7FULL;
          p->rows[idx[1]] &= (uint64_t)0x7F7F7F1F7F7F7FULL;
          p->rows[idx[2]] &= (uint64_t)0x7F7F7F3F7F7F7FULL;
        break;
        case 4:
          p->rows[idx[0]] &= (uint64_t)0x7F7F7F7F0F7F7FULL;
          p->rows[idx[1]] &= (uint64_t)0x7F7F7F7F1F7F7FULL;
          p->rows[idx[2]] &= (uint64_t)0x7F7F7F7F3F7F7FULL;
        break;
        case 5:
          p->rows[idx[0]] &= (uint64_t)0x7F7F7F7F7F0F7FULL;
          p->rows[idx[1]] &= (uint64_t)0x7F7F7F7F7F1F7FULL;
          p->rows[idx[2]] &= (uint64_t)0x7F7F7F7F7F3F7FULL;
        break;
        case 6:
          p->rows[idx[0]] &= (uint64_t)0x7F7F7F7F7F7F0FULL;
          p->rows[idx[1]] &= (uint64_t)0x7F7F7F7F7F7F1FULL;
          p->rows[idx[2]] &= (uint64_t)0x7F7F7F7F7F7F3FULL;
        break;
        default:break;
      }
    break;
    case 5:
      switch (column)
      {
        case 0:
          p->rows[idx[0]] &= (uint64_t)0x077F7F7F7F7F7FULL;
          p->rows[idx[1]] &= (uint64_t)0x0F7F7F7F7F7F7FULL;
          p->rows[idx[2]] &= (uint64_t)0x1F7F7F7F7F7F7FULL;
          p->rows[idx[3]] &= (uint64_t)0x3F7F7F7F7F7F7FULL;
        break;
        case 1:
          p->rows[idx[0]] &= (uint64_t)0x7F077F7F7F7F7FULL;
          p->rows[idx[1]] &= (uint64_t)0x7F0F7F7F7F7F7FULL;
          p->rows[idx[2]] &= (uint64_t)0x7F1F7F7F7F7F7FULL;
          p->rows[idx[3]] &= (uint64_t)0x7F3F7F7F7F7F7FULL;
        break;
        case 2:
          p->rows[idx[0]] &= (uint64_t)0x7F7F077F7F7F7FULL;
          p->rows[idx[1]] &= (uint64_t)0x7F7F0F7F7F7F7FULL;
          p->rows[idx[2]] &= (uint64_t)0x7F7F1F7F7F7F7FULL;
          p->rows[idx[3]] &= (uint64_t)0x7F7F3F7F7F7F7FULL;
        break;
        case 3:
          p->rows[idx[0]] &= (uint64_t)0x7F7F7F077F7F7FULL;
          p->rows[idx[1]] &= (uint64_t)0x7F7F7F0F7F7F7FULL;
          p->rows[idx[2]] &= (uint64_t)0x7F7F7F1F7F7F7FULL;
          p->rows[idx[3]] &= (uint64_t)0x7F7F7F3F7F7F7FULL;
        break;
        case 4:
          p->rows[idx[0]] &= (uint64_t)0x7F7F7F7F077F7FULL;
          p->rows[idx[1]] &= (uint64_t)0x7F7F7F7F0F7F7FULL;
          p->rows[idx[2]] &= (uint64_t)0x7F7F7F7F1F7F7FULL;
          p->rows[idx[3]] &= (uint64_t)0x7F7F7F7F3F7F7FULL;
        break;
        case 5:
          p->rows[idx[0]] &= (uint64_t)0x7F7F7F7F7F077FULL;
          p->rows[idx[1]] &= (uint64_t)0x7F7F7F7F7F0F7FULL;
          p->rows[idx[2]] &= (uint64_t)0x7F7F7F7F7F1F7FULL;
          p->rows[idx[3]] &= (uint64_t)0x7F7F7F7F7F3F7FULL;
        break;
        case 6:
          p->rows[idx[0]] &= (uint64_t)0x7F7F7F7F7F7F07ULL;
          p->rows[idx[1]] &= (uint64_t)0x7F7F7F7F7F7F0FULL;
          p->rows[idx[2]] &= (uint64_t)0x7F7F7F7F7F7F1FULL;
          p->rows[idx[3]] &= (uint64_t)0x7F7F7F7F7F7F3FULL;
        break;
        default:break;
      }
    break;
    case 6:
      switch (column)
      {
        case 0:
          p->rows[idx[0]] &= (uint64_t)0x037F7F7F7F7F7FULL;
          p->rows[idx[1]] &= (uint64_t)0x077F7F7F7F7F7FULL;
          p->rows[idx[2]] &= (uint64_t)0x0F7F7F7F7F7F7FULL;
          p->rows[idx[3]] &= (uint64_t)0x1F7F7F7F7F7F7FULL;
          p->rows[idx[4]] &= (uint64_t)0x3F7F7F7F7F7F7FULL;
        break;
        case 1:
          p->rows[idx[0]] &= (uint64_t)0x7F037F7F7F7F7FULL;
          p->rows[idx[1]] &= (uint64_t)0x7F077F7F7F7F7FULL;
          p->rows[idx[2]] &= (uint64_t)0x7F0F7F7F7F7F7FULL;
          p->rows[idx[3]] &= (uint64_t)0x7F1F7F7F7F7F7FULL;
          p->rows[idx[4]] &= (uint64_t)0x7F3F7F7F7F7F7FULL;
        break;
        case 2:
          p->rows[idx[0]] &= (uint64_t)0x7F7F037F7F7F7FULL;
          p->rows[idx[1]] &= (uint64_t)0x7F7F077F7F7F7FULL;
          p->rows[idx[2]] &= (uint64_t)0x7F7F0F7F7F7F7FULL;
          p->rows[idx[3]] &= (uint64_t)0x7F7F1F7F7F7F7FULL;
          p->rows[idx[4]] &= (uint64_t)0x7F7F3F7F7F7F7FULL;
        break;
        case 3:
          p->rows[idx[0]] &= (uint64_t)0x7F7F7F037F7F7FULL;
          p->rows[idx[1]] &= (uint64_t)0x7F7F7F077F7F7FULL;
          p->rows[idx[2]] &= (uint64_t)0x7F7F7F0F7F7F7FULL;
          p->rows[idx[3]] &= (uint64_t)0x7F7F7F1F7F7F7FULL;
          p->rows[idx[4]] &= (uint64_t)0x7F7F7F3F7F7F7FULL;
        break;
        case 4:
          p->rows[idx[0]] &= (uint64_t)0x7F7F7F7F037F7FULL;
          p->rows[idx[1]] &= (uint64_t)0x7F7F7F7F077F7FULL;
          p->rows[idx[2]] &= (uint64_t)0x7F7F7F7F0F7F7FULL;
          p->rows[idx[3]] &= (uint64_t)0x7F7F7F7F1F7F7FULL;
          p->rows[idx[4]] &= (uint64_t)0x7F7F7F7F3F7F7FULL;
        break;
        case 5:
          p->rows[idx[0]] &= (uint64_t)0x7F7F7F7F7F037FULL;
          p->rows[idx[1]] &= (uint64_t)0x7F7F7F7F7F077FULL;
          p->rows[idx[2]] &= (uint64_t)0x7F7F7F7F7F0F7FULL;
          p->rows[idx[3]] &= (uint64_t)0x7F7F7F7F7F1F7FULL;
          p->rows[idx[4]] &= (uint64_t)0x7F7F7F7F7F3F7FULL;
        break;
        case 6:
          p->rows[idx[0]] &= (uint64_t)0x7F7F7F7F7F7F03ULL;
          p->rows[idx[1]] &= (uint64_t)0x7F7F7F7F7F7F07ULL;
          p->rows[idx[2]] &= (uint64_t)0x7F7F7F7F7F7F0FULL;
          p->rows[idx[3]] &= (uint64_t)0x7F7F7F7F7F7F1FULL;
          p->rows[idx[4]] &= (uint64_t)0x7F7F7F7F7F7F3FULL;
        break;
        default:break;
      }
    break;
    default:break;
  }
}


static void setClue1ByRow(struct Square* p, int row, int reverse)
{
  if (reverse)
    setValue(p,7,row,6);
  else
    setValue(p,7,row,0);
}

static void setClue1ByColumn(struct Square* p, int column, int reverse)
{
  if (reverse)
    setValue(p,7,6,column);
  else
    setValue(p,7,0,column);
}

static void setClue7ByRow(struct Square* p, int row, int reverse)
{
  int i;
  for (i=0;i<7;++i)
  {
    if (reverse)
    {
      setValue(p,i+1,row,6-i);
    }
    else
    {
      setValue(p,i+1,row,i);
    }
  }
}

static void setClue7ByColumn(struct Square* p, int column, int reverse)
{
  int i;
  for (i=0;i<7;++i)
  {
    if (reverse)
    {
      setValue(p,i+1,6-i,column);
    }
    else
    {
      setValue(p,i+1,i,column);
    }
  }
}


static void setClue(struct Square* p, int clue, int index, int isRow, int reverse)
{
  switch (clue)
  {
    case 1:
      if (isRow)
      {
        setClue1ByRow(p,index,reverse);
      }
      else
      {
        setClue1ByColumn(p,index,reverse);
      }
    break;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
      if (isRow)
      {
        setClueByRow(p,clue,index,reverse);
      }
      else
      {
        setClueByColumn(p,clue,index,reverse);
      }
    break;
    case 7:
      if (isRow)
      {
        setClue7ByRow(p,index,reverse);
      }
      else
      {
        setClue7ByColumn(p,index,reverse);
      }
    break;
    default:break;
  }
}

static void printSquare(struct Square *, int* );

void setClueArray(struct Square* p, int* array)
{
  int i;
  for (i=0;i<7;++i)
  {
    if (array[i]>0)
    {
      setClue(p,array[i],i,0,0);
    }
  }
  for (i=7;i<14;++i)
  {
    if (array[i]>0)
    {
      setClue(p,array[i],i-7,1,1);
    }
  }
  for (i=14;i<21;++i)
  {
    if (array[i]>0)
    {
      setClue(p,array[i],20-i,0,1);
    }
  }
  for (i=21;i<28;++i)
  {
    if (array[i]>0)
    {
      setClue(p,array[i],27-i,1,0);
    }
  }
}


static void initialize(struct Square* p)
{
  int i;
  for (i=0;i<7;++i)
  {
    p->rows[i] = 0x7F7F7F7F7F7F7FULL;
  }
}

static void copy(struct Square* dst, struct Square* src)
{
  int i;
  for (i=0;i<7;++i)
  {
    dst->rows[i] = src->rows[i];
  }
}

unsigned int getCell(struct Square* p, int r, int c)
{
  uint64_t value;
  int shiftAmount = (6-c)*8;
  value = p->rows[r] & (0x7FULL << shiftAmount);
  return (int)(value>>shiftAmount);
}


static int getCountFromLeft(struct Square* p, int row)
{
  uint64_t last, max;
  uint64_t value;
  int i, result;
  value = p->rows[row];
  max=0;
  for (result=0,i=0;i<7;++i)
  {
    last = value & 0xFF000000000000ULL;
    if (last>max)
    {
      max=last;
      ++result;
    }
    value <<= 8;
  }
  return result;
}

static int getCountFromRight(struct Square* p, int row)
{
  uint64_t last, max;
  uint64_t value;
  int i, result;
  value = p->rows[row];
  max=0;
  for (result=0,i=0;i<7;++i)
  {
    last = value & 0xFF;
    if (last>max)
    {
      max=last;
      ++result;
    }
    value >>= 8;
  }
  return result;
}

static uint64_t getColumnSelectionMask(int column)
{
  switch (column)
  {
    case 0: return 0xFF000000000000ULL;
    case 1: return 0x00FF0000000000ULL;
    case 2: return 0x0000FF00000000ULL;
    case 3: return 0x000000FF000000ULL;
    case 4: return 0x00000000FF0000ULL;
    case 5: return 0x0000000000FF00ULL;
    case 6: return 0x000000000000FFULL;
    default: return 0;
  }
}

static int getCountFromTop(struct Square* p, int column)
{
  uint64_t last, max;
  int i, result;
  max=0;
  for (result=0,i=0;i<7;++i)
  {
    last = p->rows[i] & getColumnSelectionMask(column);
    if (last>max)
    {
      max=last;
      ++result;
    }
  }
  return result;
}

static int getCountFromBottom(struct Square* p, int column)
{
  uint64_t last, max;
  int i, result;
  max=0;
  for (result=0,i=6;i>=0;--i)
  {
    last = p->rows[i] & getColumnSelectionMask(column);
    if (last>max)
    {
      max=last;
      ++result;
    }
  }
  return result;
}


static int checkClueArray(struct Square* p, int *array)
{
  int i;
  for (i=0;i<7;++i)
  {
    if (array[i]>0)
    {
      if (getCountFromTop(p,i)!=array[i])
      {
        return -1;
      }
    }
  }
  for (i=7;i<14;++i)
  {
    if (array[i]>0)
    {
      if (getCountFromRight(p,i-7)!=array[i])
      {
        return -1;
      }
    }
  }
  for (i=14;i<21;++i)
  {
    if (array[i]>0)
    {
      if (getCountFromBottom(p,20-i)!=array[i])
      {
        return -1;
      }
    }
  }
  for (i=21;i<28;++i)
  {
    if (array[i]>0)
    {
      if (getCountFromLeft(p,27-i)!=array[i])
      {
        return -1;
      }
    }
  }
  return 0;
}



/************************************************************************************************/


static void printValue(unsigned int value)
{
  int i;
  for (i=0;i<7;++i)
  {
    if (value == (1<<i))
    {
      printf(" .%i ",i+1);
      return;
    }
  }
  printf(" %02X ",value);
}

static char getClue(int* clueArray, int pos)
{
  if (clueArray==NULL)
    return ' ';
  if (clueArray[pos]>0)
    return ('0' + clueArray[pos]);
  return ' ';
}

static void printSquare(struct Square *p, int* clueArray)
{
  int column, row;
  printf("\n     ");
  for (column=0;column<7;++column)
  {
    printf("%c   ",getClue(clueArray,column));
  }
  printf("\n  +");
  for (column=0;column<7;++column)
  {
    printf("----");
  }
  printf("-+\n");
  for (row=0;row<7;++row)
  {
    if (row>0)
    {
      printf("  | ");
      for (column=0;column<7;++column)
      {
        printf("    ");
      }
      printf("|\n");
    }
    printf("%c |",getClue(clueArray,4*7-1-row));
    for (column=0;column<7;++column)
    {
      printValue(getCell(p,row,column));
    }
    printf(" | %c\n",getClue(clueArray,7+row));
  }
  printf("  +");
  for (column=0;column<7;++column)
  {
    printf("----");
  }
  printf("-+\n");
  printf("     ");
  for (column=0;column<7;++column)
  {
    printf("%c   ",getClue(clueArray,3*7-1-column));
  }
  printf("\n");
  fflush(stdout);
}


static int isSingleValue(unsigned int value)
{
  switch (value)
  {
    case 0:
      return -1;
    case 1:
    case 2:
    case 4:
    case 8:
    case 16:
    case 32:
    case 64:
      return 1;
    default:
    break;
  }
  return 0;
}

static int getCandidates(unsigned int value, unsigned int *array)
{
  int i, count;
  for (count=0,i=0;i<7;++i)
  {
    if (value & (1<<i))
    {
      array[count++]=i+1;
    }
  }
  return count;
}


static int recursiveSolve(struct Square* p, int* clues)
{
  int bestRow, bestColumn, r, c, n, finalValues, k;
  unsigned int minCount, candidates[7], tempArray[7];
  struct Square newSquare;
  finalValues=0;
  minCount=7;
  for (r=0;r<7;++r)
  {
    for (c=0;c<7;++c)
    {
	  n = getCandidates(getCell(p,r,c),tempArray);
      switch (n)
      {
        case 0:
          return -1;
        break;
		case 1:
		  ++finalValues;
		break;
		default:
		{
		  if (n<minCount)
		  {
		    minCount=n;
			bestRow=r;
			bestColumn=c;
			memcpy(candidates,tempArray,7*sizeof(unsigned int));
		  }
		}
		break;
      }
    }
  }
  if (finalValues==49)
  {
    if (checkClueArray(p,clues))
	{
	  return -1;
	}
	else
	{
	  return 0;
	}
  }
  else
  {
	for (k=0;k<minCount;++k)
	{
	  copy(&newSquare,p);
	  if (setValue(&newSquare,candidates[k],bestRow,bestColumn) == 0)
	  {
	    if (recursiveSolve(&newSquare,clues) == 0)
	    {
	      copy(p,&newSquare);
		  return 0;
	    }
	  }
	}
	return -1;
  }
}


int** SolvePuzzle(int* clues)
{
    
    struct Square square;
    initialize(&square);
    setClueArray(&square,clues);
    if (recursiveSolve(&square,clues)==0)
	{
	  printSquare(&square,clues);
	}
	return NULL;
}





int main(int argc, char* argv[])
{
    struct Square s = {0x01102040080402ULL,
	                   0x02400810040120ULL,
					   0x04081020400201ULL,
					   0x08200401024010ULL,
					   0x10040102200840ULL,
					   0x20024004011008ULL,
					   0x40010208102004ULL};
    int count,i;
    
    int clues[7 * 4] = {7, 0, 0, 0, 2, 2, 3,
                                  0, 0, 3, 0, 0, 0, 0,
                                  3, 0, 3, 0, 0, 5, 0,
                                  0, 0, 0, 0, 5, 0, 4};

                                  
	/*
	static int expected[][SIZE][SIZE] = {
  { { 1, 5, 6, 7, 4, 3, 2 },
    { 2, 7, 4, 5, 3, 1, 6 },
    { 3, 4, 5, 6, 7, 2, 1 },
    { 4, 6, 3, 1, 2, 7, 5 },
    { 5, 3, 1, 2, 6, 4, 7 },
    { 6, 2, 7, 3, 1, 5, 4 },
    { 7, 1, 2, 4, 5, 6, 3 } },
	
	*/
	SolvePuzzle(clues);



    
  return 0;
}
