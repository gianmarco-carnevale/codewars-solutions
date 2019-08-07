/*----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
/*----------------------------------------------------------------------------*/
struct Constant;
struct VariableX;
struct Argument;
struct Expression;
struct Operation;
/*---------------*/
union ConstUnion;
union ArgUnion;
union ExprUnion;
/*----------------------------------------------------------------------------*/
#define CONST_INT        1
#define CONST_FLOAT      2
/*--------------------------*/
#define VARIABLE_X       1
#define CONSTANT_VALUE   2
/*--------------------------*/
#define OPCODE_PLUS      1
#define OPCODE_MINUS     2
#define OPCODE_MULTIPLY  3
#define OPCODE_DIVIDE    4
#define OPCODE_POWER     5
#define OPCODE_SIN       6
#define OPCODE_COS       7
#define OPCODE_TAN       8
#define OPCODE_LN        9
/*--------------------------*/
#define EXPR_SINGLE      1
#define EXPR_OPERATION   2
#define EXPR_INVALID     3
/*--------------------------*/
union ConstUnion
{
  int intValue;
  float flValue;
};

struct Constant
{
  int type;
  union ConstUnion uValue;
};

struct VariableX
{
  int _not_used;
};

union ArgUnion
{
  struct Constant  constValue;
  struct VariableX _x_not_used;
};

struct Argument
{
  int type;
  union ArgUnion uArg;
};

struct Operation
{
  int opcode;
  int secondArgRequired;
  struct Expression* pArg1;
  struct Expression* pArg2;
};

union ExpressionUnion
{
  struct Operation operation;
  struct Argument singleArg;
};

struct Expression
{
  int type;
  union ExpressionUnion uExpr;
};

/*----------------------------------------------------------------------------*/
static int getOperationFromString(char* input, struct Operation* pOperation)
{
  if (strcmp(input,"+")==0)
  {
    pOperation->opcode = OPCODE_PLUS;
    pOperation->secondArgRequired = 1;
    return 0;
  }
  if (strcmp(input,"-")==0)
  {
    pOperation->opcode = OPCODE_MINUS;
    pOperation->secondArgRequired = 1;
    return 0;
  }
  if (strcmp(input,"*")==0)
  {
    pOperation->opcode = OPCODE_MULTIPLY;
    pOperation->secondArgRequired = 1;
    return 0;
  }
  if (strcmp(input,"/")==0)
  {
    pOperation->opcode = OPCODE_DIVIDE;
    pOperation->secondArgRequired = 1;
    return 0;
  }
  if (strcmp(input,"^")==0)
  {
    pOperation->opcode = OPCODE_POWER;
    pOperation->secondArgRequired = 1;
    return 0;
  }
  if (strcmp(input,"sin")==0)
  {
    pOperation->opcode = OPCODE_SIN;
    pOperation->secondArgRequired = 0;
    return 0;
  }
  if (strcmp(input,"cos")==0)
  {
    pOperation->opcode = OPCODE_COS;
    pOperation->secondArgRequired = 0;
    return 0;
  }
  if (strcmp(input,"tan")==0)
  {
    pOperation->opcode = OPCODE_TAN;
    pOperation->secondArgRequired = 0;
    return 0;
  }
  if (strcmp(input,"ln")==0)
  {
    pOperation->opcode = OPCODE_LN;
    pOperation->secondArgRequired = 0;
    return 0;
  }
  return -1;
}

static int getSingleArgumentFromString(char* input, struct Expression* pExpr)
{
  int intConstant;
  float flConstant;
  char* endPtr;
  if (strcmp(input,"x")==0)
  {
    pExpr->type = EXPR_SINGLE;
    pExpr->uExpr.singleArg.type = VARIABLE_X;
    return 0;
  }
  intConstant = strtol(input,&endPtr,10);
  if (endPtr[0]==0)
  {
    pExpr->type = EXPR_SINGLE;
    pExpr->uExpr.singleArg.type = CONSTANT_VALUE;
    pExpr->uExpr.singleArg.uArg.constValue.type = CONST_INT;
    pExpr->uExpr.singleArg.uArg.constValue.uValue.intValue = intConstant;
    return 0;
  }
  flConstant = strtod(input,&endPtr);
  if (endPtr[0]==0)
  {
    pExpr->type = EXPR_SINGLE;
    pExpr->uExpr.singleArg.type = CONSTANT_VALUE;
    pExpr->uExpr.singleArg.uArg.constValue.type = CONST_FLOAT;
    pExpr->uExpr.singleArg.uArg.constValue.uValue.intValue = flConstant;
    return 0;
  }
  printf("ERROR: failed to get constant\n");
  return -1;
}

#define STATE_AWAITING_FIRST_SYMBOL      0
#define STATE_AWAITING_OPERATION         1
#define STATE_AWAITING_ARGUMENT          2
#define STATE_AWAITING_CLOSED_BRACKET    3
#define STATE_DONE                       4
#define STATE_ERROR                      5


static char** parseArguments(const char* input)
{
  int state;
  int length=0;
  int brackets;
  int argId;
  char** result;
  result = (char**)calloc(2,sizeof(char*));
  if (result==NULL)
  {
    printf("ERROR: calloc\n");
    return NULL;
  }
  result[0]=NULL;
  result[1]=NULL;
  for (argId=0,length=0,state=0;;++input)
  switch (state)
  {
    case 0:
      switch (input[0])
      {
        case 0:
        case ')':
          return result;
        break;
        case ' ':
          if (result[argId])
          {
            if (argId==1)
            {
              state = 0;
            }
            else
            {
              ++argId;
            }
          }
        break;
        case '(':
          state=1;
          brackets=1;
        default:
          if (result[argId]==NULL)
          {
            result[argId]=(char*)malloc(2*sizeof(char));
            if (result[argId]==NULL)
            {
              if (result[0])
                free(result[0]);
              free(result);
              return NULL;
            }
            length=0;
          }
          else
          {
            result[argId] = (char*)realloc(result[argId],sizeof(char)*(length+2));
            if (result[argId]==NULL)
            {
              if (result[0])
                free(result[0]);
              free(result);
              return NULL;
            }
          }
          result[argId][length] = input[0];
          result[argId][length+1] = 0;
          ++length;
        break;
      }
    break;
    case 1:
      result[argId] = (char*)realloc(result[argId],sizeof(char)*(length+2));
      if (result[argId]==NULL)
      {
        if (result[0])
          free(result[0]);
        free(result);
        return NULL;
      }
      result[argId][length] = input[0];
      result[argId][length+1] = 0;
      ++length;
      switch (input[0])
      {
        case '(':
          ++brackets;
        break;
        case ')':
          --brackets;
          if (brackets==0)
          {
            if (argId==1)
            {
              return result;
            }
            else
            {
              ++argId;
              state = 0;
            }
          }
        break;
        default:
        break;
      }
    break;
    default:
      return NULL;
    break;
  }
  return NULL;
}

static char** parseExpression(const char* input)
{
  int state;
  int length;
  const int exprLength = 16;
  const int opLength = 4;
  char expressionString[exprLength];
  char operationString[opLength];
  char** result;
  char** args;
  result = (char**)calloc(3,sizeof(char*));
  if (result==NULL)
  {
    printf("ERROR: calloc\n");
    return NULL;
  }
  result[0]=NULL;
  result[1]=NULL;
  result[2]=NULL;
  for (length=0,state=STATE_AWAITING_FIRST_SYMBOL;;++input)
  {
    switch (state)
    {
      case STATE_AWAITING_FIRST_SYMBOL:
        switch (input[0])
        {
          case 0:
          case ' ':
            if (length>0)
            {
              expressionString[length]=0;
              result[0]=(char*)malloc((length+1)*sizeof(char));
              if (result[0]==NULL)
              {
                free(result);
                printf("ERROR: malloc\n");
                return NULL;
              }
              strcpy(result[0],expressionString);
              return result;
            }
          break;
          case '(':
            length=0;
            state = STATE_AWAITING_OPERATION;
          break;
          default:
            if (length<exprLength)
            {
              expressionString[length++] = input[0];
            }
            else
            {
              printf("first argument too long");
              state = STATE_ERROR;
            }
          break;
        }
      break;
      case STATE_AWAITING_OPERATION:
        if (input[0] == ' ')
        {
          if (length>0)
          {
            operationString[length]=0;
            result[0]=(char*)malloc((length+1)*sizeof(char));
            if (result[0]==NULL)
            {
              free(result);
              printf("ERROR: malloc\n");
              return NULL;
            }
            strcpy(result[0],operationString);
            length=0;
            state = STATE_AWAITING_ARGUMENT;
          }
          else
            continue;
        }
        else
        {
          if (length<opLength)
          {
            operationString[length++] = input[0];
          }
          else
          {
            printf("operation too long");
            state = STATE_ERROR;
          }
        }
      break;
      case STATE_AWAITING_ARGUMENT:
        args = parseArguments(input);
        if (args==NULL)
          return NULL;
        else
        {
          result[1]=args[0];
          result[2]=args[1];
          return result;
        }
      break;
      case STATE_DONE:
        return result;
      break;
      case STATE_ERROR:
        printf("ERROR: parse error\n");
        return NULL;
      default:
        return NULL;
    }
  }
  return NULL;
}

static struct Expression* getExpressionFromString(char* input)
{
  struct Expression* result;
  char** pStrings;
  int count,i;
  pStrings = parseExpression(input);
  if (pStrings==NULL)
  {
    printf("ERROR: null pointer");
    return NULL;
  }
  printf("****** Receiving: %s ********\n",pStrings[0]);
  printf("                : %s\n",pStrings[1]);
  printf("                : %s\n",pStrings[2]);
  result = (struct Expression*)malloc(sizeof(struct Expression));
  if (result==NULL)
  {
    printf("ERROR: malloc in getExpressionFromString\n");
    return NULL;
  }
  for (count=0,i=0;i<3;++i)
  {
    if (pStrings[i])
      ++count;
  }
  switch (count)
  {
    case 1:
      result->type = EXPR_SINGLE;
      if (getSingleArgumentFromString(pStrings[0],result))
      {
        printf("ERROR: getSingleArgumentFromString failed\n");
        printf("Look: %s\n",pStrings[0]);
        free(result);
        result = NULL;
      }
      printf("Single expr: %s\n",pStrings[0]);
    break;
    case 2:
    case 3:
      result->type = EXPR_OPERATION;
      if (getOperationFromString(pStrings[0],&result->uExpr.operation))
      {
        printf("ERROR: getOperationFromString failed\n");
        free(result);
        result = NULL;
      }
      printf("OpCode: %i\n",result->uExpr.operation.opcode);
      printf("args: %s %s\n",pStrings[1],pStrings[2]);
      result->uExpr.operation.pArg1 = getExpressionFromString(pStrings[1]);
      if (result->uExpr.operation.pArg1 == NULL)
      {
        printf("ERROR: getExpressionFromString 1 failed\n");
        free(result);
        result = NULL;
      }
      else
      {
        if (result->uExpr.operation.secondArgRequired)
        {
          if (pStrings[2]==NULL)
          {
            printf("ERROR: arg2 required for opcode %i but null string\n",result->uExpr.operation.opcode);
            free(result->uExpr.operation.pArg1);
            free(result);
            result = NULL;
          }
          else
          {
            result->uExpr.operation.pArg2 = getExpressionFromString(pStrings[2]);
            if (result->uExpr.operation.pArg2==NULL)
            {
              printf("ERROR: getExpressionFromString 2 failed\n");
              free(result->uExpr.operation.pArg1);
              free(result);
              result = NULL;
            }
          }
        }
      }
    break;
    default:
      free(result);
      result = NULL;
    break;
  }
  for (i=0;i<3;++i)
  {
    if (pStrings[i])
      free(pStrings[i]);
  }
  free(pStrings);
  printf("\n");
  return result;
}

static void printExpression(struct Expression* pExpr)
{
  if (pExpr==NULL)
    printf("ERROR:\n");
  if (pExpr->type == EXPR_SINGLE)
  {
    if (pExpr->uExpr.singleArg.type == VARIABLE_X)
      printf(" x ");
    else
    {
      if (pExpr->uExpr.singleArg.type == CONSTANT_VALUE)
      {
        if (pExpr->uExpr.singleArg.uArg.constValue.type = CONST_INT)
          printf(" %i ",pExpr->uExpr.singleArg.uArg.constValue.uValue.intValue);
        else
        {
          if (pExpr->uExpr.singleArg.uArg.constValue.type = CONST_FLOAT)
          {
            printf(" %f ",pExpr->uExpr.singleArg.uArg.constValue.uValue.flValue);
          }
          else
            printf("ERROR3\n");
        }
      }
    }
  }
  else
  {
    if (pExpr->type == EXPR_OPERATION)
    {
      printf("(");
      switch (pExpr->uExpr.operation.opcode)
      {
        case OPCODE_PLUS:
          printf("+");
        break;
        case OPCODE_MINUS:
          printf("-");
        break;
        case OPCODE_MULTIPLY:
          printf("*");
        break;
        case OPCODE_DIVIDE:
          printf("/");
        break;
        case OPCODE_POWER:
          printf("^");
        break;
        case OPCODE_SIN:
          printf("sin");
        break;
        case OPCODE_COS:
          printf("cos");
        break;
        case OPCODE_TAN:
          printf("tan");
        break;
        case OPCODE_LN:
          printf("ln");
        break;
      }
      printExpression(pExpr->uExpr.operation.pArg1);
      if (pExpr->uExpr.operation.secondArgRequired)
        printExpression(pExpr->uExpr.operation.pArg2);
      printf(")");
    }
    else
    {
      printf("ERROR2\n");
    }
  }
}

static struct Expression* createIntConstant(int x)
{
  struct Expression* result;
  result = (struct Expression*)malloc(sizeof(struct Expression));
  if (result==NULL)
  {
    printf("ERROR: malloc in createIntConstant\n");
    return NULL;
  }
  result->type = EXPR_SINGLE;
  result->uExpr.singleArg.type = CONSTANT_VALUE;
  result->uExpr.singleArg.constValue.type = CONST_INT;
  result->uExpr.singleArg.constValue.uValue.intValue = x;
  return result;
}

static struct Expression* createFlConstant(float x)
{
  struct Expression* result;
  result = (struct Expression*)malloc(sizeof(struct Expression));
  if (result==NULL)
  {
    printf("ERROR: malloc in createIntConstant\n");
    return NULL;
  }
  result->type = EXPR_SINGLE;
  result->uExpr.singleArg.type = CONSTANT_VALUE;
  result->uExpr.singleArg.constValue.type = CONST_FLOAT;
  result->uExpr.singleArg.constValue.uValue.flValue = x;
  return result;
}

static struct Expression* createVariableX()
{
  struct Expression* result;
  result = (struct Expression*)malloc(sizeof(struct Expression));
  if (result==NULL)
  {
    printf("ERROR: malloc in createIntConstant\n");
    return NULL;
  }
  result->type = EXPR_SINGLE;
  result->uExpr.singleArg.type = VARIABLE_X;
  return result;
}

static int isConstant(struct Expression* pExpr)
{
  if (pExpr==NULL)
    return 0;
  if (pExpr->type!=EXPR_SINGLE)
    return 0;
  if (pExpr->uExpr.singleArg.type!=CONSTANT_VALUE)
    return 0;
  return 1;
}

static int isZero(struct Expression* pExpr)
{
  /*
  if (!isConstant(pExpr))
    return 0;
  */
  if (pExpr->uExpr.singleArg.uValue.constValue.type == CONST_INT)
  {
    if (pExpr->uExpr.singleArg.uValue.constValue.uValue.intValue == 0)
      return 1;
  }
  if (pExpr->uExpr.singleArg.uValue.constValue.type == CONST_FLOAT)
  {
    if (pExpr->uExpr.singleArg.uValue.constValue.uValue.flValue == 0.0)
      return 1;
  }
  return 0;
}

static int isOne(struct Expression* pExpr)
{
  /*
  if (!isConstant(pExpr))
    return 0;
  */
  if (pExpr->uExpr.singleArg.uValue.constValue.type == CONST_INT)
  {
    if (pExpr->uExpr.singleArg.uValue.constValue.uValue.intValue == 1)
      return 1;
  }
  if (pExpr->uExpr.singleArg.uValue.constValue.type == CONST_FLOAT)
  {
    if (pExpr->uExpr.singleArg.uValue.constValue.uValue.flValue == 1.0)
      return 1;
  }
  return 0;
}

static int isVariableX(struct Expression* pExpr)
{
  if (pExpr==NULL)
    return 0;
  if (pExpr->type!=EXPR_SINGLE)
    return 0;
  if (pExpr->uExpr.singleArg.type!=VARIABLE_X)
    return 0;
  return 1;
}

static struct Expression* copyExpression(struct Expression* pExpr)
{
  struct Expression* result;
  if (pExpr==NULL)
    return NULL;
  if (pExpr->type == EXPR_OPERATION)
  {
    result = (struct Expression*)malloc(sizeof(struct Expression));
    if (result==NULL)
    {
      printf("ERROR: malloc in executeBinaryOperation\n");
      return NULL;
    }
    *result = *pExpr;
    result->uExpr.operation.pArg1 = copyExpression(pExpr->uExpr.operation.pArg1);
    if (result->uExpr.operation.secondArgRequired)
      result->uExpr.operation.pArg2 = copyExpression(pExpr->uExpr.operation.pArg2);
    else
      result->uExpr.operation.pArg2 = NULL;
  }
  else
  {
    if (pExpr->type == EXPR_SINGLE)
    {
      if (pExpr->uExpr.singleArg.type == VARIABLE_X)
      {
        return createVariableX();
      }
      else
      {
        if (pExpr->uExpr.singleArg.type == CONSTANT_VALUE)
        {
          if (pExpr->uExpr.singleArg.uArg.constValue.type == CONST_INT)
          {
            return createIntConstant(pExpr->uExpr.singleArg.uArg.constValue.intValue);
          }
          else
          {
            if (pExpr->uExpr.singleArg.uArg.constValue.type == CONST_FLOAT)
            {
              return createFlConstant(pExpr->uExpr.singleArg.uArg.constValue.flValue);
            }
            else
            {
              printf("ERROR: wrong constant type");
              return NULL;
            }
          }
        }
        else
        {
          printf("ERROR: argument type");
          return NULL;
        }
      }
    }
    else
    {
      printf("ERROR: wrong expression type");
      return NULL;
    }
  }
  return NULL;
}

static struct Expression* getIntOperation(int a, int b, int opcode)
{
  switch (opcode)
  {
    case OPCODE_PLUS:
      return createIntConstant(a+b);
    break;
    case OPCODE_MINUS:
      return createIntConstant(a-b);
    break;
    case OPCODE_MULTIPLY:
      return createIntConstant(a*b);
    break;
    case OPCODE_DIVIDE:
      if (b==0)
      {
        printf("ERROR: division by zero\n");
        return NULL;
      }
      return createFlConstant((float)a/(float)b);
    break;
    case OPCODE_POWER:
      return createFlConstant(pow(a,b));
    break;
    default:
      printf("ERROR: unhandled opcode %i\n",opcode);
      return NULL;
    break;
  }
}

static struct Expression* getFlOperation(float a, float b, int opcode)
{
  switch (opcode)
  {
    case OPCODE_PLUS:
      return createFlConstant(a+b);
    break;
    case OPCODE_MINUS:
      return createFlConstant(a-b);
    break;
    case OPCODE_MULTIPLY:
      return createFlConstant(a*b);
    break;
    case OPCODE_DIVIDE:
      if (b==0.0)
      {
        printf("ERROR: division by zero\n");
        return NULL;
      }
      return createFlConstant((float)a/(float)b);
    break;
    case OPCODE_POWER:
      return createFlConstant(pow(a,b));
    break;
    default:
      printf("ERROR: unhandled opcode %i\n",opcode);
      return NULL;
    break;
  }
}

static struct Expression* symplifyBinaryOperation(struct Expression* first, struct Expression* second, int opcode)
{
  struct Expression* result, *temp;
  int intA, intB;
  float floatA, floatB;
  if ((isConstant(first)) && (isConstant(second)))
  {
    if ((first->uExpr.singleArg.uValue.type = CONST_INT)&&(second->uExpr.singleArg.uValue.type = CONST_INT))
    {
      intA = first->uExpr.singleArg.uValue.constValue.intValue;
      intB = second->uExpr.singleArg.uValue.constValue.intValue;
      return getIntOperation(intA, intB, opcode);
    }
    if ((first->uExpr.singleArg.uValue.type = CONST_INT)&&(second->uExpr.singleArg.uValue.type = CONST_FLOAT))
    {
      floatA = (float)first->uExpr.singleArg.uValue.constValue.intValue;
      floatB = second->uExpr.singleArg.uValue.constValue.flValue;
      return getIntOperation(floatA, floatB, opcode);
    }
    if ((first->uExpr.singleArg.uValue.type = CONST_FLOAT)&&(second->uExpr.singleArg.uValue.type = CONST_INT))
    {
      floatA = first->uExpr.singleArg.uValue.constValue.flValue;
      floatB = (float)second->uExpr.singleArg.uValue.constValue.intValue;
      return getIntOperation(floatA, floatB, opcode);
    }
    if ((first->uExpr.singleArg.uValue.type = CONST_FLOAT)&&(second->uExpr.singleArg.uValue.type = CONST_FLOAT))
    {
      floatA = first->uExpr.singleArg.uValue.constValue.flValue;
      floatB = second->uExpr.singleArg.uValue.constValue.flValue;
      return getIntOperation(floatA, floatB, opcode);
    }
    return NULL;
  }
  if ((isVariableX(first)) && (isVariableX(second)))
  {
    result = (struct Expression*)malloc(sizeof(struct Expression));
    if (result==NULL)
    {
      printf("ERROR: malloc in executeBinaryOperation\n");
      return NULL;
    }
    switch (opcode)
    {
      case OPCODE_PLUS:
        result->type = EXPR_OPERATION;
        result->uExpr.operation.opcode = OPCODE_MULTIPLY;
        result->uExpr.operation.secondArgRequired = 1;
        result->uExpr.operation.pArg1 = createVariableX();
        result->uExpr.operation.pArg2 = createIntConstant(2);
        return result;
      break;
      case OPCODE_MULTIPLY:
        result->type = EXPR_OPERATION;
        result->uExpr.operation.opcode = OPCODE_POWER;
        result->uExpr.operation.secondArgRequired = 1;
        result->uExpr.operation.pArg1 = createVariableX();
        result->uExpr.operation.pArg2 = createIntConstant(2);
        return result;
      break;
      case OPCODE_MINUS:
        result = createIntConstant(0);
        return result;
      break;
      case OPCODE_DIVIDE:
      case OPCODE_POWER:
        result->type = EXPR_OPERATION;
        result->uExpr.operation.opcode = opcode;
        result->uExpr.operation.secondArgRequired = 1;
        result->uExpr.operation.pArg1 = createVariableX();
        result->uExpr.operation.pArg2 = createVariableX();
        return result;
      break;
      default:
        printf("ERROR: unhandled opcode %i\n",opcode);
        free(result);
        return NULL;
      break;
    }
    return NULL;
  }
  if ((isConstant(first)) && isVariableX(second)))
  {
    temp = first;
    first = second;
    second = temp;
  }
  if ((isVariableX(first)) && (isConstant(second)))
  {
    if (isZero(second))
    {
      switch (opcode)
      {
        case OPCODE_PLUS:
        case OPCODE_MINUS:
          result = createVariableX();
          return result;
        break;
        case OPCODE_MULTIPLY:
          result = createIntConstant(0);
          return result;
        break;
        case OPCODE_DIVIDE:
          printf("ERROR: division by zero\n");
          return NULL;
        break;
        case OPCODE_POWER:
          result = createIntConstant(1);
          return result;
        break;
        default:
          printf("ERROR: unhandled opcode %i\n",opcode);
          return NULL;
        break;
      }
    }
    if (isOne(second))
    {
      switch (opcode)
      {
        case OPCODE_PLUS:
        case OPCODE_MINUS:
          result = (struct Expression*)malloc(sizeof(struct Expression));
          if (result==NULL)
          {
            printf("ERROR: malloc in executeBinaryOperation\n");
            return NULL;
          }
          result->type = EXPR_OPERATION;
          result->uExpr.operation.opcode = opcode;
          result->uExpr.operation.secondArgRequired = 1;
          result->uExpr.operation.pArg1 = createVariableX();
          result->uExpr.operation.pArg2 = createIntConstant(1);
          return result;
        break;
        case OPCODE_MULTIPLY:
        case OPCODE_DIVIDE:
        case OPCODE_POWER:
          result = createVariableX();
          return result;
        break;
        default:
          printf("ERROR: unhandled opcode %i\n",opcode);
          free(result);
          return NULL;
        break;
      }
    }
    return NULL;
  }
  return NULL;
}


static struct Expression* symplify(struct Expression* pExpr)
{
  struct Expression* result;
  struct Expression* arg1, *arg2;
  result = (struct Expression*)malloc(sizeof(struct Expression));
  if (result==NULL)
    return NULL;
  if (pExpr->type == EXPR_SINGLE)
  {
    *result = *pExpr;
    return result;
  }
  if (pExpr->type == EXPR_OPERATION)
  {
    if (pExpr->uExpr.operation.secondArgRequired)
    {
      if ((pExpr->uExpr.operation.pArg1) && (pExpr->uExpr.operation.pArg2))
      {
        if ((isConstant(pExpr->uExpr.operation.pArg1)) && (isConstant(pExpr->uExpr.operation.pArg2)))
        {
          result->type = EXPR_SINGLE;
          result->uExpr.singleArg.type=CONSTANT_VALUE;
        }
      }
      else
      {
        printf("ERROR: expected two expressions\n");
        return NULL;
      }
    }
  }
  return NULL;
}

int main(int argc, char* argv[])
{
  int n,c;
  char s[]="(+ (* x 3) (ln (+ x 44)))";
  char** p;
  int i;
  struct Expression* pExp = getExpressionFromString(s);
  printExpression(pExp);
  printf("\n");
  /*
  p = parseExpression(s);
  for (i=0;i<3;++i)
  {
    if (p[i])
      printf("%s\n",p[i]);
  }*/

  return 0;
}
