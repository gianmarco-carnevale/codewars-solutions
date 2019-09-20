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

static uint64_t getFilterMask(int column)
{
  switch (column)
  {
    case 0:  return (uint64_t)0xFF000000000000ULL;
    case 1:  return (uint64_t)0x00FF0000000000ULL;
    case 2:  return (uint64_t)0x0000FF00000000ULL;
    case 3:  return (uint64_t)0x000000FF000000ULL;
    case 4:  return (uint64_t)0x00000000FF0000ULL;
    case 5:  return (uint64_t)0x0000000000FF00ULL;
    case 6:  return (uint64_t)0x000000000000FFULL;
    default:
      printf("ERROR: invalid value %i in getFilterMask\n",column);
      return 0;
  }
}
static void printSquare(struct Square *, int* );
static int setValue(struct Square *p, unsigned int value, int row, int column)
{
  int i;
  uint64_t rowMask;
  uint64_t setMask, resetMask;
  setMask   = ((uint64_t)(1 << (value-1))) << SHIFT_AMOUNT_BY_COLUMN(column);
  resetMask  = getValueResetRowMask(column);
  resetMask |= getBitResetMask(value) << SHIFT_AMOUNT_BY_COLUMN(column);
  rowMask = p->rows[row] & getFilterMask(column);
  if (rowMask & setMask)
  {
    /*if (rowMask != setMask)*/
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
  /*printf("ERROR: cannot set value %d at position [%i][%i]\n",value,row,column);*/
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
      default:
        printf("ERROR: unexpected clue %i in setClueByRow(reversed)\n",clue);
        break;
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
      default:
        printf("ERROR: unexpected clue %i in setClueByRow\n",clue);
        break;
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
        default:
        printf("ERROR: unexpected clue %i for column %i in setClueByColumn\n",clue,column);
        break;
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
        default:
        printf("ERROR: unexpected clue %i for column %i in setClueByColumn\n",clue,column);
        break;
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
        default:
        printf("ERROR: unexpected clue %i for column %i in setClueByColumn\n",clue,column);
        break;
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
        default:
        printf("ERROR: unexpected clue %i for column %i in setClueByColumn\n",clue,column);
        break;
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
        default:
        printf("ERROR: unexpected clue %i for column %i in setClueByColumn\n",clue,column);
        break;
      }
    break;
    default:
    printf("ERROR: unexpected clue %i for setClueByColumn\n",clue);
    break;
  }
}


static int setClue1ByRow(struct Square* p, int row, int reverse)
{
  if (reverse)
    return setValue(p,7,row,6);
  else
    return setValue(p,7,row,0);
}

static int setClue1ByColumn(struct Square* p, int column, int reverse)
{
  if (reverse)
    return setValue(p,7,6,column);
  else
    return setValue(p,7,0,column);
}

static int setClue7ByRow(struct Square* p, int row, int reverse)
{
  int i;
  for (i=0;i<7;++i)
  {
    if (reverse)
    {
      if (setValue(p,i+1,row,6-i))
      {
        return -1;
      }
    }
    else
    {
      if (setValue(p,i+1,row,i))
      {
        return -1;
      }
    }
  }
}

static int setClue7ByColumn(struct Square* p, int column, int reverse)
{
  int i;
  for (i=0;i<7;++i)
  {
    if (reverse)
    {
      if (setValue(p,i+1,6-i,column))
      {
        return -1;
      }
    }
    else
    {
      if (setValue(p,i+1,i,column))
      {
        return -1;
      }
    }
  }
}


static void setClue(struct Square* p, int clue, int index, int isRow, int reverse)
{
  switch (clue)
  {
    case 0:
      return;
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
    default:
      printf("ERROR: setting clue %i for %i,%i,%i\n",clue,index,isRow,reverse);
    break;
  }
}



void setClueArray(struct Square* p, int* array)
{
  int i;
  for (i=0;i<7;++i)
  {
    setClue(p,array[i],     i,0,0);
    setClue(p,array[i+7],   i,1,1);
    setClue(p,array[i+14],6-i,0,1);
    setClue(p,array[i+21],6-i,1,0);
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

static void copy(struct Square* dst, const struct Square* src)
{
  int i;
  for (i=0;i<7;++i)
  {
    dst->rows[i] = src->rows[i];
  }
}

unsigned int getCell(const struct Square* p, int r, int c)
{
  uint64_t value;
  unsigned shiftAmount = (6-c)*8;
  value = p->rows[r] & ((uint64_t)0x7FULL << shiftAmount);
  return (int)((value>>shiftAmount)&0x7FULL);
}


static int getCountFromLeft(const struct Square* p, int row)
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
      if (max==7)
        return result;
    }
    value <<= 8;
  }
  return result;
}

static int getCountFromRight(const struct Square* p, int row)
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
      if (max==7)
        return result;
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

static int getCountFromTop(const struct Square* p, int column)
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
      if (max==7)
        return result;
    }
  }
  return result;
}

static int getCountFromBottom(const struct Square* p, int column)
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
      if (max==7)
        return result;
    }
  }
  return result;
}


static int checkClueArray(const struct Square* p, const int *array)
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
static void getArrayFromRow(struct Square* , int , unsigned int *);
static void printSquare(struct Square *p, int* clueArray)
{
  int column, row;
  unsigned int squareRow[7];
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
    getArrayFromRow(p,row,squareRow);
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
      printValue(squareRow[column]);
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

static int getCandidates(unsigned int value, unsigned int *array)
{
  int i, count;
  unsigned int mask;
  switch (value)
  {
    case 0: return 0;
	case 0x01:
    case 0x02:
    case 0x04:
    case 0x08:
    case 0x10:
    case 0x20:
    case 0x40:
	  return 1;
    default:
	  for (mask=0x40,count=0,i=0;i<7;++i,mask>>=1)
      {
        if (value & mask)
        {
          array[count]=7-i;
          ++count;
        }
      }
      return count;
  }
}


static void getArrayFromRow(struct Square* p, int r, unsigned int *array)
{
  int i;
  uint64_t value;
  unsigned int shiftAmount;
  for (i=0;i<7;++i)
  {
    shiftAmount = (6-i)*8;
    value = p->rows[r] & ((uint64_t)0x7FULL << shiftAmount);
    array[i] = (value>>shiftAmount)&0x7FULL;
  }
}
static int getFinalValue(unsigned int );
static int recursiveSolve(struct Square* p, int* clues)
{
  int bestRow, bestColumn, r, c, n, finalValues, k;
  unsigned int minCount;
  struct Square newSquare;
  unsigned int candidates[7]={0,0,0,0,0,0,0};
  unsigned int tempArray[7]={0,0,0,0,0,0,0};
  unsigned int tempRow[7]={0,0,0,0,0,0,0};
  finalValues=0;
  minCount=7;
  bestRow=0;
  bestColumn=0;

  for (r=0;r<7;++r)
  {
    getArrayFromRow(p,r,tempRow);
    for (c=0;c<7;++c)
    {
      n = getCandidates(tempRow[c],tempArray);
      switch (n)
      {
        case 0:
          return -1;
        break;
        case 1:
          if (setValue(p,getFinalValue(tempRow[c]),r,c)==0)
          {
            ++finalValues;
          }
        break;
        default:
          if (n<0)
          {
            printf("n<0!!!\n");
            return -1;
          }
          else
          {
            if (n<minCount)
            {
              minCount=n;
              bestRow=r;
              bestColumn=c;
              for (k=0;k<n;++k)
              {
                candidates[k] = tempArray[k];
              }
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
    if (minCount<7)
	{
	  for (k=0;k<minCount;++k)
      {
        copy(&newSquare,p);
        if ((candidates[k]<1)||(candidates[k]>7))
        {
          printf("Out of range %d,%i,%i, minCount==%i\n,",candidates[k],bestRow,bestColumn,minCount);
          printSquare(p,NULL);
        }
          
        if (setValue(&newSquare,candidates[k],bestRow,bestColumn) == 0)
        {
          if (recursiveSolve(&newSquare,clues) == 0)
          {
            copy(p,&newSquare);
            return 0;
          }
        }
      }
	}
    return -1;
  }
}




static int getFinalValue(unsigned int value)
{
  switch (value)
  {
    case 1: return 1;
    case 2: return 2;
    case 4: return 3;
    case 8: return 4;
    case 16: return 5;
    case 32: return 6;
    case 64: return 7;
    default: return 0;
  }
}

static int** packResult(struct Square* p)
{
  int** result=NULL;
  int i,j;
  if (p)
  {
    result = (int**)malloc(7*sizeof(int*));
    if (result)
    {
      for (i=0;i<7;++i)
      {
        result[i] = (int*)malloc(7*sizeof(int));
        if (result[i])
        {
          for (j=0;j<7;++j)
          {
            result[i][j] = getFinalValue(getCell(p,i,j));
          }
        }
      }
    }
  }
  return result;
}

int** SolvePuzzle(int* clues)
{
    
    struct Square square;
    initialize(&square);
    setClueArray(&square,clues);
    printSquare(&square,clues);
    if (recursiveSolve(&square,clues)==0)
    {
      printSquare(&square,clues);
      return packResult(&square);
    }
    else
    {
      printf("No solution found\n");
    }
    return NULL;
}

int main(int argc, char* argv[])
{
    struct Square s = {0x03102040080402ULL,
                       0x02400810040120ULL,
                       0x04081020400201ULL,
                       0x08200401024010ULL,
                       0x10040102200840ULL,
                       0x20024004011008ULL,
                       0x40010208102004ULL};
    struct Square s1;
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
#if 0

    initialize(&s);
    printSquare(&s,NULL);
    /*
    setValue(&s,1,0,0);
    setValue(&s,5,0,1);
    setValue(&s,6,0,2);
    setValue(&s,7,0,3);
    setValue(&s,4,0,4);
    setValue(&s,3,0,5);
    setValue(&s,2,0,6);

    setValue(&s,2,1,0);
    setValue(&s,7,1,1);
    setValue(&s,4,1,2);
    setValue(&s,5,1,3);
    setValue(&s,3,1,4);
    setValue(&s,1,1,5);
    setValue(&s,6,1,6);

    setValue(&s,3,2,0);
    setValue(&s,4,2,1);
    setValue(&s,5,2,2);
    setValue(&s,6,2,3);
    setValue(&s,7,2,4);
    setValue(&s,2,2,5);
    setValue(&s,1,2,6);
    
    setValue(&s,4,3,0);
    setValue(&s,6,3,1);
    setValue(&s,3,3,2);
    setValue(&s,1,3,3);
    setValue(&s,2,3,4);
    setValue(&s,7,3,5);
    setValue(&s,5,3,6);
    */
    setValue(&s,1,6,3);
    
    printSquare(&s,NULL);
#else
    
    SolvePuzzle(clues);
    
#endif


    
  return 0;
}
