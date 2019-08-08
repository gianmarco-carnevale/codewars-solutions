/*----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
/*----------------------------------------------------------------------------*/
struct Constant;
struct VariableX;
struct Argument;
struct Operation;
struct Expression;
/*-------------------------*/
union ConstantUnion;
union ArgumentUnion;
union ExpressionUnion;
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

/*--------------------------*/
struct Constant
{
  int type;
  union ConstantUnion uConstant;
};

/*--------------------------*/
struct VariableX
{
  int _not_used;
};

/*--------------------------*/
union ArgumentUnion
{
  struct Constant  sConstant;
  struct VariableX _x_not_used;
};

/*--------------------------*/
struct Argument
{
  int type;
  union ArgumentUnion uArgument;
};

/*--------------------------*/
struct Operation
{
  int opcode;
  int twoArgsRequired;
  struct Expression* arg1;
  struct Expression* arg2;
};

/*--------------------------*/
union ExpressionUnion
{
  struct Operation sOperation;
  struct Argument sArgument;
};

/*--------------------------*/
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

/*----------------------------------------------------------------------------*/
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
    pExpr->uExpression.sArgument.uArgument.sConstant.uConstant.floatValue = floatConstant;
    return 0;
  }
  printf("ERROR: failed to get constant from %s\n",input);
  return -1;
}

/*----------------------------------------------------------------------------*/
#define ARG_STATE_AWAITING_ARGUMENT        0
#define ARG_STATE_GATHERING_EXPRESSION     1
#define ARG_STATE_AWAITING_BRACKET         2
#define ARG_STATE_AWAITING_END             3
#define ARG_STATE_ERROR                    4

static char** parseArguments(const char* input)
{
  int state, length, brackets, argId;
  char** result;
  char* argumentString = NULL;
  result = (char**)calloc(2,sizeof(char*));
  if (result==NULL)
  {
    printf("ERROR: calloc in parseArguments\n");
    return NULL;
  }
  result[0]=NULL;
  result[1]=NULL;
  for (argId=0,length=0,state=ARG_STATE_AWAITING_ARGUMENT;;++input)
  switch (state)
  {
    case ARG_STATE_AWAITING_ARGUMENT:
      switch (input[0])
      {
        case 0:
            printf("ERROR: string ended without closing bracket\n");
            state = ARG_STATE_ERROR;
        break;
        case ')':
        case ' ':
          if (argumentString && (strlen(argumentString)>0))
          {
            result[argId] = argumentString;
            ++argId;
            argumentString = NULL;
            if (input[0]==')')
            {
              state=ARG_STATE_AWAITING_END;
            }
                else
                {
                   if (argId>1)
                     state=ARG_STATE_AWAITING_BRACKET;
                   else
                     state = ARG_STATE_AWAITING_ARGUMENT;
                }
          }
            else
            {
              if (input[0]==')')
                {
                  state=ARG_STATE_AWAITING_END;
                }
            }
        break;
        case '(':
          state=ARG_STATE_GATHERING_EXPRESSION;
          brackets=1;
        default:
          if (argumentString==NULL)
          {
            argumentString=(char*)malloc(2*sizeof(char));
            if (argumentString==NULL)
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
            argumentString = (char*)realloc(argumentString,sizeof(char)*(length+2));
            printf("ERROR: realloc 1 in parseArguments\n");
            if (argumentString==NULL)
            {
              if (result[0])
                free(result[0]);
              free(result);
              return NULL;
            }
          }
          argumentString[length] = input[0];
          argumentString[length+1] = 0;
          ++length;
        break;
      }
    break;
    case ARG_STATE_GATHERING_EXPRESSION:
      argumentString = (char*)realloc(argumentString,sizeof(char)*(length+2));
      if (argumentString==NULL)
      {
        printf("ERROR: realloc 2 in parseArguments\n");
        if (result[0])
          free(result[0]);
        free(result);
        return NULL;
      }
      argumentString[length] = input[0];
      argumentString[length+1] = 0;
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
            if (argId<2)
            {
              result[argId] = argumentString;
              ++argId;
              argumentString = NULL;
              state = ARG_STATE_AWAITING_ARGUMENT;
            }
            else
            {
              state = ARG_STATE_AWAITING_BRACKET;
            }
          }
        break;
        default:
        break;
      }
    break;
    case ARG_STATE_AWAITING_BRACKET:
     switch (input[0])
     {
        case 0:
          printf("ERROR: missing closing bracket\n");
          state = ARG_STATE_ERROR;
        break;
        case ' ':
        break;
        case ')':
          state = ARG_STATE_AWAITING_END;
        break;
        default:
            printf("ERROR: unexpected argument: %s\n",input);
            state = ARG_STATE_ERROR;
        break;
     }
     break;
    case ARG_STATE_AWAITING_END:
     switch (input[0])
     {
        case 0:
          return result;
        break;
        case ' ':
        break;
        default:
            state = ARG_STATE_ERROR;
            printf("ERROR: unexpected argument: %s\n",input);
        break;
     }
     break;
     case ARG_STATE_ERROR:
        printf("ERROR: parsing in parseArguments\n");
        return NULL;
     break;
     default:
      printf("ERROR: unhandled state %i\n",state);
      return NULL;
     break;
  }
  return NULL;
}

/*----------------------------------------------------------------------------*/
#define EXPR_STATE_AWAITING_FIRST_SYMBOL      0
#define EXPR_STATE_AWAITING_OPERATION         1
#define EXPR_STATE_AWAITING_ARGUMENT          2
#define EXPR_STATE_AWAITING_END               3
#define EXPR_STATE_AWAITING_CLOSED            4
#define EXPR_STATE_ERROR                      5

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
              if (input[0])
                state = EXPR_STATE_AWAITING_END;
              else
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
          {
            continue;
          }
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
      case EXPR_STATE_AWAITING_END:
        switch (input[0])
        {
          case 0:
            return result;
          break;
          case ' ':
          break;
          default:
            state = EXPR_STATE_ERROR;
          break;
        }
      break;
      case EXPR_STATE_ERROR:
        printf("ERROR: parse error in parseExpression\n");
        return NULL;
      default:
        printf("ERROR: unhandled state %i in parseExpression\n",state);
        return NULL;
      break;
    }
  }
  return NULL;
}

/*----------------------------------------------------------------------------*/
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
        free(result);
        result = NULL;
      }
      printf("Single expr: %s\n",pStrings[0]);
    break;
    case 2:
    case 3:
      result->type = EXPR_OPERATION;
        result->uExpression.sOperation.arg1;
        result->uExpression.sOperation.arg2;
      if (getOperationFromString(pStrings[0],&result->uExpression.sOperation))
      {
        printf("ERROR: getOperationFromString failed\n");
        free(result);
        result = NULL;
      }
      else
      {
        printf("OpCode: %i\n",result->uExpression.sOperation.opcode);
        printf("args: %s %s\n",pStrings[1],pStrings[2]);
        result->uExpression.sOperation.arg1 = getExpressionFromString(pStrings[1]);
        if (count==3)
            result->uExpression.sOperation.arg2 = getExpressionFromString(pStrings[2]);
        else
            result->uExpression.sOperation.arg2 = NULL;
        if (result->uExpression.sOperation.arg1 == NULL)
        {
          printf("ERROR: getExpressionFromString arg1 is null\n");
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
                printf("ERROR: getExpressionFromString arg2 is null\n");
                free(result->uExpression.sOperation.arg1);
                free(result);
                result = NULL;
              }
            }
          }
          else
          {
            if (pStrings[2])
            {
               printf("ERROR: got one more unexpected argument: %s\n",pStrings[2]);
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

/*----------------------------------------------------------------------------*/
static void printExpression(struct Expression* pExpr)
{
  if (pExpr==NULL)
  {
    printf("ERROR: null pointer in printExpression\n");
    return;
  }
  if (pExpr->type == EXPR_SINGLE)
  {
    if (pExpr->uExpression.sArgument.type == VARIABLE_X)
      printf("x");
    else
    {
      if (pExpr->uExpression.sArgument.type == CONSTANT_VALUE)
      {
        if (pExpr->uExpression.sArgument.uArgument.sConstant.type == CONST_INT)
          printf("%i",pExpr->uExpression.sArgument.uArgument.sConstant.uConstant.intValue);
        else
        {
          if (pExpr->uExpression.sArgument.uArgument.sConstant.type == CONST_FLOAT)
          {
            printf("%f",pExpr->uExpression.sArgument.uArgument.sConstant.uConstant.floatValue);
          }
          else
          {
            printf("ERROR: invalid type %i in printExpression\n");
          }
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
        default:
          printf("ERROR: not an operation %i\n",pExpr->uExpression.sOperation.opcode);
        break;
      }
      printf(" ");
      printExpression(pExpr->uExpression.sOperation.arg1);
      if (pExpr->uExpression.sOperation.twoArgsRequired)
      {
        printf(" ");
        printExpression(pExpr->uExpression.sOperation.arg2);
      }
      printf(")");
    }
    else
    {
      printf("ERROR: not an operation neither a single, %i\n",pExpr->type);
    }
  }
}

/*----------------------------------------------------------------------------*/
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

static struct Expression* createFloatConstant(float x)
{
  struct Expression* result;
  result = (struct Expression*)malloc(sizeof(struct Expression));
  if (result==NULL)
  {
    printf("ERROR: malloc in createFloatConstant\n");
    return NULL;
  }
  result->type = EXPR_SINGLE;
  result->uExpression.sArgument.type = CONSTANT_VALUE;
  result->uExpression.sArgument.uArgument.sConstant.type = CONST_FLOAT;
  result->uExpression.sArgument.uArgument.sConstant.uConstant.floatValue = x;
  return result;
}

/*----------------------------------------------------------------------------*/
static struct Expression* createVariableX()
{
  struct Expression* result;
  result = (struct Expression*)malloc(sizeof(struct Expression));
  if (result==NULL)
  {
    printf("ERROR: malloc in createVariableX\n");
    return NULL;
  }
  result->type = EXPR_SINGLE;
  result->uExpression.sArgument.type = VARIABLE_X;
  return result;
}

/*----------------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------------*/
static int isZero(struct Expression* pExpr)
{
  /*
  this function assumes that the input pointer
  points to a constant structure
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

/*----------------------------------------------------------------------------*/
static int isOne(struct Expression* pExpr)
{
  /*
  this function assumes that the input pointer
  points to a constant structure
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

/*----------------------------------------------------------------------------*/
static int isZeroOrNegative(struct Expression* pExpr)
{
  if (!isConstant(pExpr))
     return 0;
  if (pExpr->uExpression.sArgument.uArgument.sConstant.type == CONST_INT)
  {
    if (pExpr->uExpression.sArgument.uArgument.sConstant.uConstant.intValue <= 0)
      return 1;
  }
  if (pExpr->uExpression.sArgument.uArgument.sConstant.type == CONST_FLOAT)
  {
    if (pExpr->uExpression.sArgument.uArgument.sConstant.uConstant.floatValue <= 0.0)
      return 1;
  }
  return 0;
}

/*----------------------------------------------------------------------------*/
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

/*----------------------------------------------------------------------------*/
static struct Expression* copyExpression(struct Expression* pExpr)
{
  struct Expression* result;
  if (pExpr==NULL)
    return NULL;
  result = (struct Expression*)malloc(sizeof(struct Expression));
  if (result==NULL)
  {
    printf("ERROR: malloc in copyExpression\n");
    return NULL;
  }
  *result = *pExpr;
  if (pExpr->type == EXPR_OPERATION)
  {
    result->uExpression.sOperation.arg1 = copyExpression(pExpr->uExpression.sOperation.arg1);
    result->uExpression.sOperation.arg2 = copyExpression(pExpr->uExpression.sOperation.arg2);
  }
  return result;
}

/*----------------------------------------------------------------------------*/
static int intPower(int a, int b)
{
  int result,i;
  for (i=0,result=1;i<b;++i)
  {
    result = result * a;
  }
  return result;
}

/*----------------------------------------------------------------------------*/
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
        printf("ERROR: division by zero in getIntOperation\n");
        return NULL;
      }
      if (a%b==0)
        return createIntConstant(a/b);
      else
        return createFloatConstant((float)a/(float)b);
    break;
    case OPCODE_POWER:
      if (a==0)
      {
        if (b<0)
        {
          printf("ERROR: zero raised to negative power in getIntOperation\n");
          return NULL;
        }
        if (b==0)
        {
          return createIntConstant(1);
        }
        return createIntConstant(0);
      }
      return createIntConstant(intPower(a,b));
    break;
    default:
      printf("ERROR: unhandled opcode %i in getIntOperation\n",opcode);
      return NULL;
    break;
  }
}

/*----------------------------------------------------------------------------*/
static struct Expression* getFloatOperation(float a, float b, int opcode)
{
  float temp;
  switch (opcode)
  {
    case OPCODE_PLUS:
      return createFloatConstant(a+b);
    break;
    case OPCODE_MINUS:
      return createFloatConstant(a-b);
    break;
    case OPCODE_MULTIPLY:
      return createFloatConstant(a*b);
    break;
    case OPCODE_DIVIDE:
      if (b==0.0)
      {
        printf("ERROR: division by zero in getFloatOperation\n");
        return NULL;
      }
      return createFloatConstant((float)a/(float)b);
    break;
    case OPCODE_POWER:
      temp = (float)pow((double)a,(double)b);
      return createFloatConstant(temp);
    break;
    default:
      printf("ERROR: unhandled opcode %i in getFloatOperation\n",opcode);
      return NULL;
    break;
  }
}
/*----------------------------------------------------------------------------*/
static struct Expression* createOperation(int opcode, struct Expression* arg1, struct Expression* arg2);

static struct Expression* simplify(struct Expression* pExpr)
{
  struct Expression *first, *second, *result;
  int opcode, intA, intB;
  float floatA, floatB;
  /*-----------------------------------------------------------------*/
  if (pExpr==NULL)
  {
    printf("ERROR: null pointer in simplify\n");
    return NULL;
  }
  if (pExpr->type == EXPR_SINGLE)
  {
    return copyExpression(pExpr);
  }
  if (pExpr->type == EXPR_OPERATION)
  {
    opcode = pExpr->uExpression.sOperation.opcode;
    first = simplify(pExpr->uExpression.sOperation.arg1);
    if (pExpr->uExpression.sOperation.twoArgsRequired==0)
    {
      if (pExpr->uExpression.sOperation.opcode == OPCODE_LN)
      {
        if (isZeroOrNegative(first))
        {
           printf("ERROR: logarithm of a non positive number in simplify\n");
           return NULL;
        }
      }
      return createOperation(opcode,first,NULL);
    }
    else
    {
      second = simplify(pExpr->uExpression.sOperation.arg2);
      /*-----------------------------------------------------------------*/
      if (isConstant(first) && isZero(first))
      {
        switch (opcode)
        {
          case OPCODE_PLUS:
            return simplify(second);
          break;
          case OPCODE_MULTIPLY:
            return createIntConstant(0);
          break;
          default:
          break;
        }
      }
      if (isConstant(second) && isZero(second))
      {
        switch (opcode)
        {
          case OPCODE_PLUS:
          case OPCODE_MINUS:
            return simplify(first);
          break;
          case OPCODE_MULTIPLY:
            return createIntConstant(0);
          break;
          case OPCODE_DIVIDE:
            printf("ERROR: division by zero\n");
            return NULL;
          break;
          default:
          break;
        }
      }
      if (isConstant(first) && isOne(first))
      {
        switch (opcode)
        {
          case OPCODE_MULTIPLY:
            return simplify(second);
          break;
          case OPCODE_POWER:
            return createIntConstant(1);
          break;
          default:
          break;
        }
      }
      if (isConstant(second) && isOne(second))
      {
        switch (opcode)
        {
          case OPCODE_MULTIPLY:
          case OPCODE_DIVIDE:
          case OPCODE_POWER:
            return simplify(first);
          break;
          default:
          break;
        }
      }
      /*------------ both expressions are constant values ---------------*/
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
          return getFloatOperation(floatA, floatB, opcode);
        }
        if ((first->uExpression.sArgument.uArgument.sConstant.type = CONST_FLOAT)&&(second->uExpression.sArgument.uArgument.sConstant.type = CONST_INT))
        {
          floatA = first->uExpression.sArgument.uArgument.sConstant.uConstant.floatValue;
          floatB = (float)second->uExpression.sArgument.uArgument.sConstant.uConstant.intValue;
          return getFloatOperation(floatA, floatB, opcode);
        }
        if ((first->uExpression.sArgument.uArgument.sConstant.type = CONST_FLOAT)&&(second->uExpression.sArgument.uArgument.sConstant.type = CONST_FLOAT))
        {
          floatA = first->uExpression.sArgument.uArgument.sConstant.uConstant.floatValue;
          floatB = second->uExpression.sArgument.uArgument.sConstant.uConstant.floatValue;
          return getFloatOperation(floatA, floatB, opcode);
        }
        printf("ERROR: unexpected 1 in symplifyBinaryOperation\n");
        return NULL;
      }
      /*------------ both expressions are x variable ---------------*/
      if ((isVariableX(first)) && (isVariableX(second)))
      {
        switch (opcode)
        {
          case OPCODE_PLUS:
            return createOperation(OPCODE_MULTIPLY,createVariableX(),createIntConstant(2));
          break;
          case OPCODE_MULTIPLY:
            return createOperation(OPCODE_POWER,createVariableX(),createIntConstant(2));
          break;
          case OPCODE_MINUS:
            return createIntConstant(0);
          break;
          default:
            printf("ERROR: unhandled opcode %i in executeBinaryOperation (both x)\n",opcode);
            return NULL;
          break;
        }
      }
      /*------------ generic situation ---------------*/
      return createOperation(opcode,copyExpression(first),copyExpression(second));
    }
  }
  printf("ERROR: invalid type %i in simplify\n",pExpr->type);
  return NULL;
}

static struct Expression* createOperation(int opcode, struct Expression* arg1, struct Expression* arg2)
{
  struct Expression* result;
  int twoArgs;
  if (arg1==NULL)
  {
    printf("ERROR: arg1 is null\n");
    return NULL;
  }
  switch (opcode)
  {
    case OPCODE_PLUS:
    case OPCODE_MINUS:
    case OPCODE_MULTIPLY:
    case OPCODE_DIVIDE:
    case OPCODE_POWER:
      twoArgs=1;
      if (arg2==NULL)
      {
        printf("ERROR: arg2 is null\n");
        return NULL;
      }
    break;
    case OPCODE_SIN:
    case OPCODE_COS:
    case OPCODE_TAN:
    case OPCODE_LN:
      twoArgs=0;
      if (arg2!=NULL)
      {
        printf("ERROR: unexpected second argument\n");
        return NULL;
      }
    break;
    default:
      printf("ERROR: unhandled opcode %i in createOperation\n",opcode);
      return NULL;
    break;
  }
  result = (struct Expression*)malloc(sizeof(struct Expression));
  if (result==NULL)
  {
    printf("ERROR: malloc in createOperation\n");
    return NULL;
  }
  result->type = EXPR_OPERATION;
  result->uExpression.sOperation.opcode = opcode;
  result->uExpression.sOperation.twoArgsRequired = twoArgs;
  result->uExpression.sOperation.arg1 = arg1;
  result->uExpression.sOperation.arg2 = arg2;
  return result;
}

static struct Expression* differentiate(struct Expression* pExpr)
{
  int opcode;
  struct Expression* first, *second;
  if (pExpr==NULL)
    return NULL;
  if (pExpr->type == EXPR_SINGLE)
  {
    if (pExpr->uExpression.sArgument.type == VARIABLE_X)
    {
      return createIntConstant(1);
    }
    if (pExpr->uExpression.sArgument.type == CONSTANT_VALUE)
    {
      return createIntConstant(0);
    }
  }
  if (pExpr->type == EXPR_OPERATION)
  {
    opcode = pExpr->uExpression.sOperation.opcode;
	first =  pExpr->uExpression.sOperation.arg1;
	second = pExpr->uExpression.sOperation.arg2;
    switch (opcode)
    {
      case OPCODE_PLUS:
	  case OPCODE_MINUS:
	    return createOperation(opcode,differentiate(first),differentiate(second));
	  break;
      case OPCODE_MULTIPLY:
	    return createOperation(
		                       OPCODE_PLUS,
							   createOperation(OPCODE_MULTIPLY,   first,           differentiate(second)),
							   createOperation(OPCODE_MULTIPLY,   differentiate(first), second)
							  );
	  break;
      case OPCODE_DIVIDE:
	    return createOperation(
		                       OPCODE_DIVIDE,
							   createOperation(
							                   OPCODE_MINUS,
							                   createOperation(OPCODE_MULTIPLY,differentiate(first),second),
											   createOperation(OPCODE_MULTIPLY,first,differentiate(second))
											   ),
							   createOperation(OPCODE_POWER,second,createIntConstant(2))
							  );
	  break;
      case OPCODE_POWER:
	    return createOperation(
		                       OPCODE_MULTIPLY,
							   createOperation(OPCODE_POWER,first,second),
							   createOperation(
							                   OPCODE_PLUS,
											   createOperation(
											                   OPCODE_MULTIPLY,
															   createOperation(differentiate(first)),
															   createOperation(OPCODE_DIVIDE,second,first)
											                  ),
											   createOperation(
											                   OPCODE_MULTIPLY,
															   differentiate(second),
															   createOperation(OPCODE_LN,first,NULL)
															  )
							                  )
		                      );
      break;
      case OPCODE_SIN:
	    return createOperation(
		                       OPCODE_MULTIPLY,
		                       createOperation(OPCODE_COS,first),
							   differentiate(first)
							   );
	  break;
      case OPCODE_COS:
	    return createOperation(
		                      OPCODE_MULTIPLY,
							  createOperation(
							                  OPCODE_MULTIPLY,
											  createIntConstant(-1),
											  createOperation(OPCODE_SIN,first)
											 ),
							  differentiate(first)
							  );
	  break;
      case OPCODE_TAN:
	    return createOperation(
		                       OPCODE_MULTIPLY,
							   createOperation(
							                   OPCODE_PLUS,
											   createIntConstant(1),
											   createOperation(
											                   OPCODE_POWER,
															   createOperation(OPCODE_TAN,first,NULL),
															   createIntConstant(2)
											                  )
							                  ),
							   differentiate(first)
							  );
	  break;
      case OPCODE_LN:
        return createOperation(OPCODE_DIVIDE,differentiate(first),first);
      break;
      default:
        printf("ERROR: unhandled opcode %i in createOperation\n",opcode);
        return NULL;
      break;
    }
  }
  printf("ERROR: unexpected type %i in differentiate\n",pExpr->type);
  return NULL;
}

int main(int argc, char* argv[])
{
  int n,c;
  char s[]="(sin (+ (^ x 1) (- 5 5)))";
  char** p;
  int i;
  struct Expression* pExp = getExpressionFromString(s);
  printf("---------------------------------\n");
  printExpression(pExp);
  printf("\n---------------------------------\n");
  printExpression(simplify(pExp));
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
