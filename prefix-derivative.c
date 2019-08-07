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
/*-------------------------*/
union ExpressionUnion;
union ArgumentUnion;
union ConstantUnion;
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
union ConstantUnion
{
  int intValue;
  float floatValue;
};

struct Constant
{
  int type;
  union ConstantUnion uConstant;
};

struct VariableX
{
  int _not_used;
};

union ArgumentUnion
{
  struct Constant  sConstant;
  struct VariableX _x_not_used;
};

struct Argument
{
  int type;
  union ArgumentUnion uArgument;
};

struct Operation
{
  int opcode;
  int twoArgsRequired;
  struct Expression* arg1;
  struct Expression* arg2;
};

union ExpressionUnion
{
  struct Operation sOperation;
  struct Argument sArgument;
};

struct Expression
{
  int type;
  union ExpressionUnion uExpression;
};

/*----------------------------------------------------------------------------*/
static int getOperationFromString(char* input, struct Operation* pOperation)
{
  if (strcmp(input,"+")==0)
  {
    pOperation->opcode = OPCODE_PLUS;
    pOperation->twoArgsRequired = 1;
    return 0;
  }
  if (strcmp(input,"-")==0)
  {
    pOperation->opcode = OPCODE_MINUS;
    pOperation->twoArgsRequired = 1;
    return 0;
  }
  if (strcmp(input,"*")==0)
  {
    pOperation->opcode = OPCODE_MULTIPLY;
    pOperation->twoArgsRequired = 1;
    return 0;
  }
  if (strcmp(input,"/")==0)
  {
    pOperation->opcode = OPCODE_DIVIDE;
    pOperation->twoArgsRequired = 1;
    return 0;
  }
  if (strcmp(input,"^")==0)
  {
    pOperation->opcode = OPCODE_POWER;
    pOperation->twoArgsRequired = 1;
    return 0;
  }
  if (strcmp(input,"sin")==0)
  {
    pOperation->opcode = OPCODE_SIN;
    pOperation->twoArgsRequired = 0;
    return 0;
  }
  if (strcmp(input,"cos")==0)
  {
    pOperation->opcode = OPCODE_COS;
    pOperation->twoArgsRequired = 0;
    return 0;
  }
  if (strcmp(input,"tan")==0)
  {
    pOperation->opcode = OPCODE_TAN;
    pOperation->twoArgsRequired = 0;
    return 0;
  }
  if (strcmp(input,"ln")==0)
  {
    pOperation->opcode = OPCODE_LN;
    pOperation->twoArgsRequired = 0;
    return 0;
  }
  return -1;
}

static int getSingleArgumentFromString(char* input, struct Expression* pExpr)
{
  int intConstant;
  float floatConstant;
  char* endPtr;
  if (strcmp(input,"x")==0)
  {
    pExpr->type = EXPR_SINGLE;
    pExpr->uExpression.sArgument.type = VARIABLE_X;
    return 0;
  }
  intConstant = strtol(input,&endPtr,10);
  if (endPtr[0]==0)
  {
    pExpr->type = EXPR_SINGLE;
    pExpr->uExpression.sArgument.type = CONSTANT_VALUE;
    pExpr->uExpression.sArgument.uArgument.sConstant.type = CONST_INT;
    pExpr->uExpression.sArgument.uArgument.sConstant.uConstant.intValue = intConstant;
    return 0;
  }
  floatConstant = strtod(input,&endPtr);
  if (endPtr[0]==0)
  {
    pExpr->type = EXPR_SINGLE;
    pExpr->uExpression.sArgument.type = CONSTANT_VALUE;
    pExpr->uExpression.sArgument.uArgument.sConstant.type = CONST_FLOAT;
    pExpr->uExpression.sArgument.uArgument.sConstant.uConstant.intValue = floatConstant;
    return 0;
  }
  printf("ERROR: failed to get constant from %s\n",input);
  return -1;
}


#define ARG_STATE_AWAITING_EXPRESSION      0
#define ARG_STATE_GATHERING_EXPRESSION     1

static char** parseArguments(const char* input)
{
  int state, length, brackets, argId;
  char** result;
  result = (char**)calloc(2,sizeof(char*));
  if (result==NULL)
  {
    printf("ERROR: calloc in parseArguments\n");
    return NULL;
  }
  result[0]=NULL;
  result[1]=NULL;
  for (argId=0,length=0,state=0;;++input)
  switch (state)
  {
    case ARG_STATE_AWAITING_EXPRESSION:
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
              state = ARG_STATE_AWAITING_EXPRESSION;
            }
            else
            {
              ++argId;
            }
          }
        break;
        case '(':
          state=ARG_STATE_GATHERING_EXPRESSION;
          brackets=1;
        default:
          if (result[argId]==NULL)
          {
            result[argId]=(char*)malloc(2*sizeof(char));
            if (result[argId]==NULL)
            {
              printf("ERROR: malloc in parseArguments\n");
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
            printf("ERROR: realloc 1 in parseArguments\n");
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
    case ARG_STATE_GATHERING_EXPRESSION:
      result[argId] = (char*)realloc(result[argId],sizeof(char)*(length+2));
      if (result[argId]==NULL)
      {
        printf("ERROR: realloc 2 in parseArguments\n");
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
              state = ARG_STATE_AWAITING_EXPRESSION;
            }
          }
        break;
        default:
            /* do nothing, gathering the string */
        break;
      }
    break;
    default:
      printf("ERROR: unhandled state %i\n",state);
      return NULL;
    break;
  }
  return NULL;
}

#define EXPR_STATE_AWAITING_FIRST_SYMBOL      0
#define EXPR_STATE_AWAITING_OPERATION         1
#define EXPR_STATE_AWAITING_ARGUMENT          2
#define EXPR_STATE_DONE                       3
#define EXPR_STATE_ERROR                      4


static char** parseExpression(const char* input)
{
  int state, length;
  const int exprLength = 16;
  const int opLength = 4;
  char expressionString[exprLength];
  char operationString[opLength];
  char** result;
  char** args;
  result = (char**)calloc(3,sizeof(char*));
  if (result==NULL)
  {
    printf("ERROR: calloc in parseExpression\n");
    return NULL;
  }
  result[0]=NULL;
  result[1]=NULL;
  result[2]=NULL;
  for (length=0,state=EXPR_STATE_AWAITING_FIRST_SYMBOL;;++input)
  {
    switch (state)
    {
      case EXPR_STATE_AWAITING_FIRST_SYMBOL:
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
                printf("ERROR: malloc 1 in parseExpression\n");
                return NULL;
              }
              strcpy(result[0],expressionString);
              return result;
            }
          break;
          case '(':
            length=0;
            state = EXPR_STATE_AWAITING_OPERATION;
          break;
          default:
            if (length<exprLength)
            {
              expressionString[length++] = input[0];
            }
            else
            {
              printf("first argument too long");
              state = EXPR_STATE_ERROR;
            }
          break;
        }
      break;
      case EXPR_STATE_AWAITING_OPERATION:
        if (input[0] == ' ')
        {
          if (length>0)
          {
            operationString[length]=0;
            result[0]=(char*)malloc((length+1)*sizeof(char));
            if (result[0]==NULL)
            {
              free(result);
              printf("ERROR: malloc 2 in parseExpression\n");
              return NULL;
            }
            strcpy(result[0],operationString);
            length=0;
            state = EXPR_STATE_AWAITING_ARGUMENT;
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
            printf("ERROR: sOperation too long\n");
            state = EXPR_STATE_ERROR;
          }
        }
      break;
      case EXPR_STATE_AWAITING_ARGUMENT:
        args = parseArguments(input);
        if (args==NULL)
        {
		  printf("ERROR: got null arguments\n");
		  return NULL;
        }
        else
        {
          result[1]=args[0];
          result[2]=args[1];
          return result;
        }
      break;
      case EXPR_STATE_ERROR:
        printf("ERROR: parse error\n");
        return NULL;
      default:
        printf("ERROR: unhandled state %i\n",state);
        return NULL;
      break;
    }
  }
  return NULL;
}

static struct Expression* getExpressionFromString(char* input)
{
  struct Expression* result;
  char** pStrings;
  int count,i;
  if (input==NULL)
	return NULL;
  pStrings = parseExpression(input);
  if (pStrings==NULL)
  {
    printf("ERROR: null pointer from parseExpression\n");
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
      if (getOperationFromString(pStrings[0],&result->uExpression.sOperation))
      {
        printf("ERROR: getOperationFromString failed\n");
        free(result);
        result = NULL;
      }
      printf("OpCode: %i\n",result->uExpression.sOperation.opcode);
      printf("args: %s %s\n",pStrings[1],pStrings[2]);
      result->uExpression.sOperation.arg1 = getExpressionFromString(pStrings[1]);
      if (result->uExpression.sOperation.arg1 == NULL)
      {
        printf("ERROR: getExpressionFromString 1 failed\n");
        free(result);
        result = NULL;
      }
      else
      {
        if (result->uExpression.sOperation.twoArgsRequired)
        {
          if (pStrings[2]==NULL)
          {
            printf("ERROR: arg2 required for opcode %i but null string\n",result->uExpression.sOperation.opcode);
            free(result->uExpression.sOperation.arg1);
            free(result);
            result = NULL;
          }
          else
          {
            result->uExpression.sOperation.arg2 = getExpressionFromString(pStrings[2]);
            if (result->uExpression.sOperation.arg2==NULL)
            {
              printf("ERROR: getExpressionFromString 2 failed\n");
              free(result->uExpression.sOperation.arg1);
              free(result);
              result = NULL;
            }
          }
        }
      }
    break;
    default:
	  printf("ERROR: unexpected case!!!\n");
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
    if (pExpr->uExpression.sArgument.type == VARIABLE_X)
      printf(" x ");
    else
    {
      if (pExpr->uExpression.sArgument.type == CONSTANT_VALUE)
      {
        if (pExpr->uExpression.sArgument.uArgument.sConstant.type = CONST_INT)
          printf(" %i ",pExpr->uExpression.sArgument.uArgument.sConstant.uConstant.intValue);
        else
        {
          if (pExpr->uExpression.sArgument.uArgument.sConstant.type = CONST_FLOAT)
          {
            printf(" %f ",pExpr->uExpression.sArgument.uArgument.sConstant.uConstant.floatValue);
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
      switch (pExpr->uExpression.sOperation.opcode)
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
      printExpression(pExpr->uExpression.sOperation.arg1);
      if (pExpr->uExpression.sOperation.twoArgsRequired)
        printExpression(pExpr->uExpression.sOperation.arg2);
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
  result->uExpression.sArgument.type = CONSTANT_VALUE;
  result->uExpression.sArgument.uArgument.sConstant.type = CONST_INT;
  result->uExpression.sArgument.uArgument.sConstant.uConstant.intValue = x;
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
  result->uExpression.sArgument.type = CONSTANT_VALUE;
  result->uExpression.sArgument.uArgument.sConstant.type = CONST_FLOAT;
  result->uExpression.sArgument.uArgument.sConstant.uConstant.floatValue = x;
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
  result->uExpression.sArgument.type = VARIABLE_X;
  return result;
}

static int isConstant(struct Expression* pExpr)
{
  if (pExpr==NULL)
    return 0;
  if (pExpr->type!=EXPR_SINGLE)
    return 0;
  if (pExpr->uExpression.sArgument.type!=CONSTANT_VALUE)
    return 0;
  return 1;
}

static int isZero(struct Expression* pExpr)
{
  /*
  if (!isConstant(pExpr))
    return 0;
  */
  if (pExpr->uExpression.sArgument.uArgument.sConstant.type == CONST_INT)
  {
    if (pExpr->uExpression.sArgument.uArgument.sConstant.uConstant.intValue == 0)
      return 1;
  }
  if (pExpr->uExpression.sArgument.uArgument.sConstant.type == CONST_FLOAT)
  {
    if (pExpr->uExpression.sArgument.uArgument.sConstant.uConstant.floatValue == 0.0)
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
  if (pExpr->uExpression.sArgument.uArgument.sConstant.type == CONST_INT)
  {
    if (pExpr->uExpression.sArgument.uArgument.sConstant.uConstant.intValue == 1)
      return 1;
  }
  if (pExpr->uExpression.sArgument.uArgument.sConstant.type == CONST_FLOAT)
  {
    if (pExpr->uExpression.sArgument.uArgument.sConstant.uConstant.floatValue == 1.0)
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
  if (pExpr->uExpression.sArgument.type!=VARIABLE_X)
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
    result->uExpression.sOperation.arg1 = copyExpression(pExpr->uExpression.sOperation.arg1);
    if (result->uExpression.sOperation.twoArgsRequired)
      result->uExpression.sOperation.arg2 = copyExpression(pExpr->uExpression.sOperation.arg2);
    else
      result->uExpression.sOperation.arg2 = NULL;
  }
  else
  {
    if (pExpr->type == EXPR_SINGLE)
    {
      if (pExpr->uExpression.sArgument.type == VARIABLE_X)
      {
        return createVariableX();
      }
      else
      {
        if (pExpr->uExpression.sArgument.type == CONSTANT_VALUE)
        {
          if (pExpr->uExpression.sArgument.uArgument.sConstant.type == CONST_INT)
          {
            return createIntConstant(pExpr->uExpression.sArgument.uArgument.sConstant.uConstant.intValue);
          }
          else
          {
            if (pExpr->uExpression.sArgument.uArgument.sConstant.type == CONST_FLOAT)
            {
              return createFlConstant(pExpr->uExpression.sArgument.uArgument.sConstant.uConstant.floatValue);
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
    if ((first->uExpression.sArgument.type = CONST_INT)&&(second->uExpression.sArgument.uArgument.sConstant.type = CONST_INT))
    {
      intA = first->uExpression.sArgument.uArgument.sConstant.uConstant.intValue;
      intB = second->uExpression.sArgument.uArgument.sConstant.uConstant.intValue;
      return getIntOperation(intA, intB, opcode);
    }
    if ((first->uExpression.sArgument.type = CONST_INT)&&(second->uExpression.sArgument.uArgument.sConstant.type = CONST_FLOAT))
    {
      floatA = (float)first->uExpression.sArgument.uArgument.sConstant.uConstant.intValue;
      floatB = second->uExpression.sArgument.uArgument.sConstant.uConstant.floatValue;
      return getIntOperation(floatA, floatB, opcode);
    }
    if ((first->uExpression.sArgument.uArgument.sConstant.type = CONST_FLOAT)&&(second->uExpression.sArgument.uArgument.sConstant.type = CONST_INT))
    {
      floatA = first->uExpression.sArgument.uArgument.sConstant.uConstant.floatValue;
      floatB = (float)second->uExpression.sArgument.uArgument.sConstant.uConstant.intValue;
      return getIntOperation(floatA, floatB, opcode);
    }
    if ((first->uExpression.sArgument.uArgument.sConstant.type = CONST_FLOAT)&&(second->uExpression.sArgument.uArgument.sConstant.type = CONST_FLOAT))
    {
      floatA = first->uExpression.sArgument.uArgument.sConstant.uConstant.floatValue;
      floatB = second->uExpression.sArgument.uArgument.sConstant.uConstant.floatValue;
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
        result->uExpression.sOperation.opcode = OPCODE_MULTIPLY;
        result->uExpression.sOperation.twoArgsRequired = 1;
        result->uExpression.sOperation.arg1 = createVariableX();
        result->uExpression.sOperation.arg2 = createIntConstant(2);
        return result;
      break;
      case OPCODE_MULTIPLY:
        result->type = EXPR_OPERATION;
        result->uExpression.sOperation.opcode = OPCODE_POWER;
        result->uExpression.sOperation.twoArgsRequired = 1;
        result->uExpression.sOperation.arg1 = createVariableX();
        result->uExpression.sOperation.arg2 = createIntConstant(2);
        return result;
      break;
      case OPCODE_MINUS:
        result = createIntConstant(0);
        return result;
      break;
      case OPCODE_DIVIDE:
      case OPCODE_POWER:
        result->type = EXPR_OPERATION;
        result->uExpression.sOperation.opcode = opcode;
        result->uExpression.sOperation.twoArgsRequired = 1;
        result->uExpression.sOperation.arg1 = createVariableX();
        result->uExpression.sOperation.arg2 = createVariableX();
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
  if ((isConstant(first)) && isVariableX(second))
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
          result->uExpression.sOperation.opcode = opcode;
          result->uExpression.sOperation.twoArgsRequired = 1;
          result->uExpression.sOperation.arg1 = createVariableX();
          result->uExpression.sOperation.arg2 = createIntConstant(1);
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
    if (pExpr->uExpression.sOperation.twoArgsRequired)
    {
      if ((pExpr->uExpression.sOperation.arg1) && (pExpr->uExpression.sOperation.arg2))
      {
        if ((isConstant(pExpr->uExpression.sOperation.arg1)) && (isConstant(pExpr->uExpression.sOperation.arg2)))
        {
          result->type = EXPR_SINGLE;
          result->uExpression.sArgument.type=CONSTANT_VALUE;
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
