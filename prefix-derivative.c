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
/*----------------------------------------------*/
#define VARIABLE_X       1
#define CONSTANT_VALUE   2
/*----------------------------------------------*/
#define OPCODE_PLUS      1
#define OPCODE_MINUS     2
#define OPCODE_MULTIPLY  3
#define OPCODE_DIVIDE    4
#define OPCODE_POWER     5
#define OPCODE_SIN       6
#define OPCODE_COS       7
#define OPCODE_TAN       8
#define OPCODE_EXP       9
#define OPCODE_LN        10
/*----------------------------------------------*/
#define EXPR_SINGLE      1
#define EXPR_OPERATION   2
#define EXPR_INVALID     3
/*----------------------------------------------*/
union ConstantUnion
{
  int intValue;
  float floatValue;
};

/*----------------------------------------------*/
struct Constant
{
  int type;
  union ConstantUnion uConstant;
};

/*----------------------------------------------*/
struct VariableX
{
  int _not_used;
};

/*----------------------------------------------*/
union ArgumentUnion
{
  struct Constant  sConstant;
  struct VariableX _x_not_used;
};

/*----------------------------------------------*/
struct Argument
{
  int type;
  union ArgumentUnion uArgument;
};

/*----------------------------------------------*/
struct Operation
{
  int opcode;
  int twoArgsRequired;
  struct Expression* arg1;
  struct Expression* arg2;
};

/*----------------------------------------------*/
union ExpressionUnion
{
  struct Operation sOperation;
  struct Argument sArgument;
};

/*----------------------------------------------*/
struct Expression
{
  int type;
  union ExpressionUnion uExpression;
};

/*----------------------------------------------*/

static int mallocCalls = 0;
static void* Malloc(size_t size)
{
  void* result = malloc(size);
  if (result)
    ++mallocCalls;
  return result;
}

static void* Calloc(size_t num, size_t size)
{
  void* result = calloc(num,size);
  if (result)
    ++mallocCalls;
  return result;
}

static void Free(void* p)
{
  --mallocCalls;
  free(p);
}


static int getOperationFromString(char* input, struct Operation* pOperation);
static int getSingleArgumentFromString(char* input, struct Expression* pExpr);
static char** parseArguments(const char* input);
static char** parseExpression(const char* input);
static struct Expression* getExpressionFromString(const char* input);
static size_t getExpressionLength(struct Expression* pExpr);
static char* printExpression(struct Expression* pExpr);
static struct Expression* createIntConstant(int x);
static struct Expression* createFloatConstant(float x);
static struct Expression* createVariableX();
static int isConstant(struct Expression* pExpr);
static int isZeroOrNegative(struct Expression* pExpr);
static int isVariableX(struct Expression* pExpr);
static struct Expression* copyExpression(struct Expression* pExpr);
static int compareExpressions(struct Expression* first, struct Expression* second);
static struct Expression* getIntOperation(int a, int b, int opcode);
static struct Expression* getFloatOperation(float a, float b, int opcode);
static void deleteExpression(struct Expression* pExpr);
static struct Expression* simplify(struct Expression* pExpr);
static struct Expression* createOperation(int opcode, struct Expression* arg1, struct Expression* arg2);
static struct Expression* differentiate(struct Expression* pExpr);
/*--------------------------------------------------------------------------------*/
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
  if (strcmp(input,"exp")==0)
  {
    pOperation->opcode = OPCODE_EXP;
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
  result = (char**)Calloc(2,sizeof(char*));
  if (result==NULL)
  {
    printf("ERROR: Calloc in parseArguments\n");
    return NULL;
  }
  result[0]=NULL;
  result[1]=NULL;
  for (argId=0,length=0,state=ARG_STATE_AWAITING_ARGUMENT;;++input)
  {
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
              argumentString=(char*)Malloc(2*sizeof(char));
              if (argumentString==NULL)
              {
                printf("ERROR: Malloc in parseArguments\n");
                state = ARG_STATE_ERROR;
              }
              else
              {
                argumentString[0] = input[0];
                argumentString[1] = 0;
                length=1;
              }
            }
            else
            {
              argumentString = (char*)realloc(argumentString,sizeof(char)*(length+2));
              if (argumentString==NULL)
              {
                printf("ERROR: realloc 1 in parseArguments\n");
                state = ARG_STATE_ERROR;
              }
              else
              {
                argumentString[length] = input[0];
                argumentString[length+1] = 0;
                ++length;
              }
            }
          break;
        }
      break;
      case ARG_STATE_GATHERING_EXPRESSION:
        if (input[0]==0)
          state = ARG_STATE_ERROR;
        else
        {
           argumentString = (char*)realloc(argumentString,sizeof(char)*(length+2));
           if (argumentString==NULL)
           {
             printf("ERROR: realloc 2 in parseArguments\n");
             state = ARG_STATE_ERROR;
           }
           else
           {
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
           }
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
            if (argumentString)
              Free(argumentString);
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
          if (result)
          {
            if (result[0])
              Free(result[0]);
            if (result[1])
              Free(result[1]);
            Free(result);
          }
          if (argumentString)
            Free(argumentString);
          return NULL;
       break;
       default:
        printf("ERROR: unhandled state %i\n",state);
        return NULL;
       break;
    }
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
  result = (char**)Calloc(3,sizeof(char*));
  if (result==NULL)
  {
    printf("ERROR: Calloc in parseExpression\n");
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
              result[0]=(char*)Malloc((length+1)*sizeof(char));
              if (result[0]==NULL)
              {
                Free(result);
                printf("ERROR: Malloc 1 in parseExpression\n");
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
              printf("ERROR: argument too long\n");
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
            result[0]=(char*)Malloc((length+1)*sizeof(char));
            if (result[0]==NULL)
            {
              Free(result);
              printf("ERROR: Malloc 2 in parseExpression\n");
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
          if (input[0]==0)
            state = EXPR_STATE_ERROR;
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
        }
      break;
      case EXPR_STATE_AWAITING_ARGUMENT:
        args = parseArguments(input);
        if (args==NULL)
        {
          printf("ERROR: got null arguments\n");
          state = EXPR_STATE_ERROR;
        }
        else
        {
          result[1]=args[0];
          result[2]=args[1];
          Free(args);
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
        if (result)
        {
          if (result[0])
            Free(result[0]);
          if (result[1])
            Free(result[1]);
          if (result[2])
            Free(result[2]);
          Free(result);
        }
        return NULL;
      default:
        printf("ERROR: unhandled state %i in parseExpression\n",state);
        if (result)
        {
          if (result[0])
            Free(result[0]);
          if (result[1])
            Free(result[1]);
          if (result[2])
            Free(result[2]);
          Free(result);
        }
        return NULL;
      break;
    }
  }
  return NULL;
}

/*----------------------------------------------------------------------------*/
static struct Expression* getExpressionFromString(const char* input)
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
  result = (struct Expression*)Malloc(sizeof(struct Expression));
  if (result==NULL)
  {
    printf("ERROR: Malloc in getExpressionFromString\n");
    for (i=0;i<3;++i)
    {
      if (pStrings[i])
        Free(pStrings[i]);
    }
    Free(pStrings);
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
        deleteExpression(result);
        result = NULL;
      }
    break;
    case 2:
    case 3:
      result->type = EXPR_OPERATION;
      result->uExpression.sOperation.arg1 = NULL;
      result->uExpression.sOperation.arg2 = NULL;
      if (getOperationFromString(pStrings[0],&result->uExpression.sOperation))
      {
        printf("ERROR: getOperationFromString failed\n");
        deleteExpression(result);
        result = NULL;
      }
      else
      {
        result->uExpression.sOperation.arg1 = getExpressionFromString(pStrings[1]);
        if (count==3)
        {
           result->uExpression.sOperation.arg2 = getExpressionFromString(pStrings[2]);
        }
        else
            result->uExpression.sOperation.arg2 = NULL;
        if (result->uExpression.sOperation.arg1 == NULL)
        {
          printf("ERROR: getExpressionFromString arg1 is null\n");
          deleteExpression(result);
          result = NULL;
        }
        else
        {
          if (result->uExpression.sOperation.twoArgsRequired)
          {
            if (result->uExpression.sOperation.arg2==NULL)
            {
              printf("ERROR: arg2 required for opcode %i but null string\n",result->uExpression.sOperation.opcode);
              deleteExpression(result);
              result = NULL;
            }
            else
            {
              if (result->uExpression.sOperation.arg2==NULL)
              {
                printf("ERROR: getExpressionFromString arg2 is null\n");
                deleteExpression(result);
                result = NULL;
              }
            }
          }
          else
          {
            if (pStrings[2])
            {
               printf("ERROR: got one more unexpected argument: %s\n",pStrings[2]);
               deleteExpression(result);
               result = NULL;
            }
          }
        }
      }
    break;
    default:
      printf("ERROR: unexpected case!!!\n");
      deleteExpression(result);
      result = NULL;
    break;
  }
  for (i=0;i<3;++i)
  {
    if (pStrings[i])
      Free(pStrings[i]);
  }
  Free(pStrings);
  return result;
}

/*----------------------------------------------------------------------------*/
static size_t getExpressionLength(struct Expression* pExpr)
{
  size_t result;
  if (pExpr==NULL)
    return 0;
  if (pExpr->type == EXPR_SINGLE)
  {
    if (pExpr->uExpression.sArgument.type == VARIABLE_X)
    {
      return 2;
    }
    if (pExpr->uExpression.sArgument.type == CONSTANT_VALUE)
    {
      return 12;
    }
  }
  if (pExpr->type == EXPR_OPERATION)
  {
    result = 8;
    result += getExpressionLength(pExpr->uExpression.sOperation.arg1);
    result += getExpressionLength(pExpr->uExpression.sOperation.arg2);
    return result;
  }
  return 0;
}

static char* printExpression(struct Expression* pExpr)
{
  char *result, *temp, *arg;
  int n;
  result = (char*)Malloc(getExpressionLength(pExpr)+1);
  if (result==NULL)
    return NULL;
  result[0]=0;
  if (pExpr==NULL)
  {
    printf("ERROR: null pointer in printExpression\n");
    return result;
  }
  temp = result;
  if (pExpr->type == EXPR_SINGLE)
  {
    if (pExpr->uExpression.sArgument.type == VARIABLE_X)
    {
      n = sprintf(temp,"x");
      temp += n;
    }
    else
    {
      if (pExpr->uExpression.sArgument.type == CONSTANT_VALUE)
      {
        if (pExpr->uExpression.sArgument.uArgument.sConstant.type == CONST_INT)
        {
          n = sprintf(temp,"%i",pExpr->uExpression.sArgument.uArgument.sConstant.uConstant.intValue);
          temp += n;
        }
        else
        {
          if (pExpr->uExpression.sArgument.uArgument.sConstant.type == CONST_FLOAT)
          {
            n = sprintf(temp,"%1.1f",pExpr->uExpression.sArgument.uArgument.sConstant.uConstant.floatValue);
            temp += n;
          }
          else
          {
            printf("ERROR: invalid type %i in printExpression\n",pExpr->uExpression.sArgument.uArgument.sConstant.type);
          }
        }
      }
    }
  }
  else
  {
    if (pExpr->type == EXPR_OPERATION)
    {
      n = sprintf(temp,"(");
      temp += n;
      switch (pExpr->uExpression.sOperation.opcode)
      {
        case OPCODE_PLUS:
          n = sprintf(temp,"+");
          temp += n;
        break;
        case OPCODE_MINUS:
          n = sprintf(temp,"-");
          temp += n;
        break;
        case OPCODE_MULTIPLY:
          n = sprintf(temp,"*");
          temp += n;
        break;
        case OPCODE_DIVIDE:
          n = sprintf(temp,"/");
          temp += n;
        break;
        case OPCODE_POWER:
          n = sprintf(temp,"^");
          temp += n;
        break;
        case OPCODE_SIN:
          n = sprintf(temp,"sin");
          temp += n;
        break;
        case OPCODE_COS:
          n = sprintf(temp,"cos");
          temp += n;
        break;
        case OPCODE_TAN:
          n = sprintf(temp,"tan");
          temp += n;
        break;
        case OPCODE_EXP:
          n = sprintf(temp,"exp");
          temp += n;
        break;
        case OPCODE_LN:
          n = sprintf(temp,"ln");
          temp += n;
        break;
        default:
          printf("ERROR: not an operation %i\n",pExpr->uExpression.sOperation.opcode);
        break;
      }
      n = sprintf(temp," ");
      temp += n;
      arg = printExpression(pExpr->uExpression.sOperation.arg1);
      n = strlen(arg);
      memcpy(temp,arg,n);
      Free(arg);
      temp += n;
      if (pExpr->uExpression.sOperation.twoArgsRequired)
      {
        n = sprintf(temp," ");
        temp += n;
        arg = printExpression(pExpr->uExpression.sOperation.arg2);
        n = strlen(arg);
        memcpy(temp,arg,n);
        Free(arg);
        temp += n;
      }
      n = sprintf(temp,")");
      temp += n;
    }
    else
    {
      printf("ERROR: not an operation neither a single, %i\n",pExpr->type);
    }
  }
  return result;
}

/*----------------------------------------------------------------------------*/
static struct Expression* createIntConstant(int x)
{
  struct Expression* result;
  result = (struct Expression*)Malloc(sizeof(struct Expression));
  if (result==NULL)
  {
    printf("ERROR: Malloc in createIntConstant\n");
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
  result = (struct Expression*)Malloc(sizeof(struct Expression));
  if (result==NULL)
  {
    printf("ERROR: Malloc in createFloatConstant\n");
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
  result = (struct Expression*)Malloc(sizeof(struct Expression));
  if (result==NULL)
  {
    printf("ERROR: Malloc in createVariableX\n");
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
  result = (struct Expression*)Malloc(sizeof(struct Expression));
  if (result==NULL)
  {
    printf("ERROR: Malloc in copyExpression\n");
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

static int compareExpressions(struct Expression* first, struct Expression* second)
{
  if (first==NULL)
  {
    if (second==NULL)
      return 0;
    else
      return -1;
  }
  if (second==NULL)
    return -1;
  if (first->type != second->type)
    return -1;
  if (first->type == EXPR_SINGLE)
  {
    if (isVariableX(first) && isVariableX(second))
      return 0;
    if (first->uExpression.sArgument.uArgument.sConstant.type != second->uExpression.sArgument.uArgument.sConstant.type)
        return 1;
    if (first->uExpression.sArgument.uArgument.sConstant.type == CONST_INT)
    {
        if (first->uExpression.sArgument.uArgument.sConstant.uConstant.intValue == second->uExpression.sArgument.uArgument.sConstant.uConstant.intValue)
          return 0;
    }
    if (first->uExpression.sArgument.uArgument.sConstant.type == CONST_FLOAT)
    {
        if (first->uExpression.sArgument.uArgument.sConstant.uConstant.floatValue == second->uExpression.sArgument.uArgument.sConstant.uConstant.floatValue)
          return 0;
    }
    return -1;
  }
  else
  {
    if (first->type == EXPR_OPERATION)
    {
      if (first->uExpression.sOperation.opcode != second->uExpression.sOperation.opcode)
        return -1;
      if (first->uExpression.sOperation.twoArgsRequired != second->uExpression.sOperation.twoArgsRequired)
        return -1;
      if (compareExpressions(first->uExpression.sOperation.arg1,second->uExpression.sOperation.arg1))
        return -1;
      if (compareExpressions(first->uExpression.sOperation.arg2,second->uExpression.sOperation.arg2))
        return -1;
      return 0;
    }
  }
  return -1;
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
static void deleteExpression(struct Expression* pExpr)
{
  if (pExpr==NULL)
    return;
  if (pExpr->type == EXPR_OPERATION)
  {
    deleteExpression(pExpr->uExpression.sOperation.arg1);
    deleteExpression(pExpr->uExpression.sOperation.arg2);
  }
  Free(pExpr);
}

/*----------------------------------------------------------------------------*/
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
    if (isConstant(pExpr))
    {
      if (pExpr->uExpression.sArgument.uArgument.sConstant.type == CONST_FLOAT)
      {
        floatA = pExpr->uExpression.sArgument.uArgument.sConstant.uConstant.floatValue;
        intA = (int)floatA;
        if ((float)intA == floatA)
        {
          return createIntConstant(intA);
        }
      }
    }
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
        if (isConstant(first) && isOne(first))
        {
          deleteExpression(first);
          return createIntConstant(0);
        }
      }
      if (pExpr->uExpression.sOperation.opcode == OPCODE_EXP)
      {
        if (isConstant(first) && isZero(first))
        {
          deleteExpression(first);
          return createIntConstant(1);
        }
      }
      return createOperation(opcode,first,NULL);
    }
    else
    {
      second = simplify(pExpr->uExpression.sOperation.arg2);
      /*-----------------------------------------------------------------*/
      if (isConstant(first) && isZero(first) && !isConstant(second))
      {
        switch (opcode)
        {
          case OPCODE_PLUS:
            result = copyExpression(second);
            deleteExpression(first);
            deleteExpression(second);
            return result;
          break;
          case OPCODE_MULTIPLY:
            deleteExpression(first);
            deleteExpression(second);
            return createIntConstant(0);
          break;
          default:
          break;
        }
      }
      if (!isConstant(first) && isConstant(second) && isZero(second))
      {
        switch (opcode)
        {
          case OPCODE_PLUS:
          case OPCODE_MINUS:
            result = copyExpression(first);
            deleteExpression(first);
            deleteExpression(second);
            return result;
          break;
          case OPCODE_MULTIPLY:
            deleteExpression(first);
            deleteExpression(second);
            return createIntConstant(0);
          break;
          case OPCODE_POWER:
            deleteExpression(first);
            deleteExpression(second);
            return createIntConstant(1);
          break;
          case OPCODE_DIVIDE:
            printf("ERROR: division by zero\n");
            return NULL;
          break;
          default:
          break;
        }
      }
      if (isConstant(first) && isOne(first) && !isConstant(second))
      {
        switch (opcode)
        {
          case OPCODE_MULTIPLY:
            result = copyExpression(second);
            deleteExpression(first);
            deleteExpression(second);
            return result;
          break;
          case OPCODE_POWER:
            deleteExpression(first);
            deleteExpression(second);
            return createIntConstant(1);
          break;
          default:
          break;
        }
      }
      if (!isConstant(first) && isConstant(second) && isOne(second))
      {
        switch (opcode)
        {
          case OPCODE_MULTIPLY:
          case OPCODE_DIVIDE:
          case OPCODE_POWER:
            result = copyExpression(first);
            deleteExpression(first);
            deleteExpression(second);
            return result;
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
          deleteExpression(first);
          deleteExpression(second);
          return getIntOperation(intA, intB, opcode);
        }
        if ((first->uExpression.sArgument.type = CONST_INT)&&(second->uExpression.sArgument.uArgument.sConstant.type = CONST_FLOAT))
        {
          floatA = (float)first->uExpression.sArgument.uArgument.sConstant.uConstant.intValue;
          floatB = second->uExpression.sArgument.uArgument.sConstant.uConstant.floatValue;
          deleteExpression(first);
          deleteExpression(second);
          return getFloatOperation(floatA, floatB, opcode);
        }
        if ((first->uExpression.sArgument.uArgument.sConstant.type = CONST_FLOAT)&&(second->uExpression.sArgument.uArgument.sConstant.type = CONST_INT))
        {
          floatA = first->uExpression.sArgument.uArgument.sConstant.uConstant.floatValue;
          floatB = (float)second->uExpression.sArgument.uArgument.sConstant.uConstant.intValue;
          deleteExpression(first);
          deleteExpression(second);
          return getFloatOperation(floatA, floatB, opcode);
        }
        if ((first->uExpression.sArgument.uArgument.sConstant.type = CONST_FLOAT)&&(second->uExpression.sArgument.uArgument.sConstant.type = CONST_FLOAT))
        {
          floatA = first->uExpression.sArgument.uArgument.sConstant.uConstant.floatValue;
          floatB = second->uExpression.sArgument.uArgument.sConstant.uConstant.floatValue;
          deleteExpression(first);
          deleteExpression(second);
          return getFloatOperation(floatA, floatB, opcode);
        }
        printf("ERROR: unexpected 1 in symplifyBinaryOperation\n");
        return NULL;
      }
      if (compareExpressions(first,second)==0)
      {
        switch (opcode)
        {
          case OPCODE_PLUS:
            result = createOperation(OPCODE_MULTIPLY,copyExpression(first),createIntConstant(2));
            deleteExpression(first);
            deleteExpression(second);
            return result;
          break;
          case OPCODE_MULTIPLY:
            result = createOperation(OPCODE_POWER,copyExpression(first),createIntConstant(2));
            deleteExpression(first);
            deleteExpression(second);
            return result;
          break;
          case OPCODE_MINUS:
            deleteExpression(first);
            deleteExpression(second);
            return createIntConstant(0);
          break;
          default:
            printf("ERROR: unhandled opcode %i in executeBinaryOperation (both x)\n",opcode);
            return NULL;
          break;
        }
      }
      /*------------ generic situation ---------------*/
      result = createOperation(opcode,copyExpression(first),copyExpression(second));
      deleteExpression(first);
      deleteExpression(second);
      return result;
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
    case OPCODE_EXP:
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
  result = (struct Expression*)Malloc(sizeof(struct Expression));
  if (result==NULL)
  {
    printf("ERROR: Malloc in createOperation\n");
    return NULL;
  }
  result->type = EXPR_OPERATION;
  result->uExpression.sOperation.opcode = opcode;
  result->uExpression.sOperation.twoArgsRequired = twoArgs;
  result->uExpression.sOperation.arg1 = arg1;
  result->uExpression.sOperation.arg2 = arg2;
  return result;
}

static struct Expression* createPowerDerivative(struct Expression* base, struct Expression* exponent, struct Expression* pExpr)
{
  struct Expression* result;
  if (isConstant(base) && !isConstant(exponent))
  {
    return createOperation(
                           OPCODE_MULTIPLY,
                           createOperation(
                                           OPCODE_MULTIPLY,
                                           createOperation(
                                                          OPCODE_LN,
                                                          copyExpression(base),
                                                          NULL
                                                          ),
                                           copyExpression(pExpr)
                                          ),
                           differentiate(pExpr)
                           );
  }
  else
  {
    if (!isConstant(base) && isConstant(exponent))
    {
      if (exponent->uExpression.sArgument.uArgument.sConstant.type == CONST_INT)
	  {

		result = createOperation(
                               OPCODE_MULTIPLY,
                               createOperation(
                                               OPCODE_MULTIPLY,
                                               copyExpression(exponent),
                                               createOperation(
                                                               OPCODE_POWER,
                                                               copyExpression(base),
                                                               createIntConstant(exponent->uExpression.sArgument.uArgument.sConstant.uConstant.intValue-1)
                                                               )
                                              ),
                              differentiate(base)
                           );
		return result;
	  }
	  else
        return createOperation(
                               OPCODE_MULTIPLY,
                               createOperation(
                                               OPCODE_MULTIPLY,
                                               copyExpression(exponent),
                                               createOperation(
                                                               OPCODE_POWER,
                                                               copyExpression(base),
                                                               createIntConstant(exponent->uExpression.sArgument.uArgument.sConstant.uConstant.floatValue-1.0)
                                                               )
                                              ),
                              differentiate(base)
                           );
	  
    }
    else
    {
        printf("NOT YET READY!!!\n");
        return NULL;
    }
  }
}

static struct Expression* createMultiplyDerivative(struct Expression* first, struct Expression* second)
{
  if (isConstant(first))
  {
    return createOperation(OPCODE_MULTIPLY,copyExpression(first),differentiate(second));
  }
  if (isConstant(second))
  {
    return createOperation(OPCODE_MULTIPLY,differentiate(first),copyExpression(second));
  }
  return createOperation(
                       OPCODE_PLUS,
                       createOperation(OPCODE_MULTIPLY, copyExpression(first), differentiate(second)),
                       createOperation(OPCODE_MULTIPLY, differentiate(first),  copyExpression(second))
                       );
}

static struct Expression* differentiate(struct Expression* pExpr)
{
  int opcode;
  struct Expression* first, *second, *result, *final;
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
    first  = pExpr->uExpression.sOperation.arg1;
    second = pExpr->uExpression.sOperation.arg2;
    switch (opcode)
    {
      case OPCODE_PLUS:
      case OPCODE_MINUS:
        result = createOperation(opcode,differentiate(first),differentiate(second));
      break;
      case OPCODE_MULTIPLY:
        result = createMultiplyDerivative(first,second);
      break;
      case OPCODE_DIVIDE:
        result = createOperation(
                                OPCODE_DIVIDE,
                                createOperation(
                                                OPCODE_MINUS,
                                                createOperation(OPCODE_MULTIPLY,differentiate(first),copyExpression(second)),
                                                createOperation(OPCODE_MULTIPLY,copyExpression(first),differentiate(second))
                                               ),
                                createOperation(OPCODE_MULTIPLY,copyExpression(second),copyExpression(second))
                                );
      break;
      case OPCODE_POWER:
        result = createPowerDerivative(first,second,pExpr);
      break;
      case OPCODE_SIN:
        result = createOperation(
                               OPCODE_MULTIPLY,
                               differentiate(first),
                               createOperation(OPCODE_COS,copyExpression(first),NULL)
                               );
      break;
      case OPCODE_COS:
        result = createOperation(
                              OPCODE_MULTIPLY,
                              differentiate(first),
                              createOperation(
                                              OPCODE_MULTIPLY,
                                              createIntConstant(-1),
                                              createOperation(OPCODE_SIN,copyExpression(first),NULL)
                                             )
                              );
      break;
      case OPCODE_TAN:
        result = createOperation(
                               OPCODE_MULTIPLY,
                               createOperation(
                                               OPCODE_PLUS,
                                               createIntConstant(1),
                                               createOperation(
                                                               OPCODE_POWER,
                                                               createOperation(OPCODE_TAN,copyExpression(first),NULL),
                                                               createIntConstant(2)
                                                              )
                                              ),
                               differentiate(first)
                              );
      break;
      case OPCODE_EXP:
        result = createOperation(OPCODE_MULTIPLY,differentiate(first),copyExpression(pExpr));
      break;
      case OPCODE_LN:
        result = createOperation(OPCODE_DIVIDE,differentiate(first),copyExpression(first));
      break;
      default:
        printf("ERROR: unhandled opcode %i in createOperation\n",opcode);
        return NULL;
      break;
    }
    final = simplify(result);
    /*
    printf("simplify[ %s ] == %s\n",printExpression(result),printExpression(final));
    */
    deleteExpression(result);
    return final;
  }
  printf("ERROR: unexpected type %i in differentiate\n",pExpr->type);
  return NULL;
}

/*---------------------------------------------------------------------------------------------------------*/
static char buffer[1024];
char* diff(char* input)
{
  struct Expression *pExpr, *differentiated;
  char* result;
  pExpr = getExpressionFromString(input);
  differentiated = differentiate(pExpr);
  deleteExpression(pExpr);
  result = printExpression(differentiated);
  deleteExpression(differentiated);
  memset(buffer,0,1024);
  strcpy(buffer,result);
  Free(result);
  return &buffer[0];
}

/*-------------------------------------------------------------------------------------------------------*/
#if 0
Test (test_cases, sample_tests_1) {

  const char *s = diff ("(tan x)");
  cr_assert (!strcmp (s, "(+ 1 (^ (tan x) 2))") || !strcmp (s, "(^ (cos x) -2)") || !strcmp (s, "(/ 1 (^ (cos x) 2))"), "diff (tan x) incorrect");
}

Test (test_cases, sample_tests_2) {
  diffShouldBe ("(+ x (+ x x))", "3");
  diffShouldBe ("(- (+ x x) x)", "1");
  diffShouldBe ("(* 2 (+ x 2))", "2");
  diffShouldBe ("(/ 2 (+ 1 x))", "(/ -2 (^ (+ 1 x) 2))");
  diffShouldBe ("(cos (+ x 1))", "(* -1 (sin (+ x 1)))");
  diffShouldBe ("(sin (+ x 1))", "(cos (+ x 1))");
  diffShouldBe ("(sin (* 2 x))", "(* 2 (cos (* 2 x)))");
  diffShouldBe ("(exp (* 2 x))", "(* 2 (exp (* 2 x)))");
  const char *s = diff("(tan (* 2 x))");
  cr_assert (!strcmp (s, "(* 2 (+ 1 (^ (tan (* 2 x)) 2)))") || !strcmp (s, "(* 2 (^ (cos (* 2 x)) -2))") || !strcmp (s, "(/ 2 (^ (cos (* 2 x)) 2))"), "diff (tan (* 2 x)) incorrect");
}

Test (test_cases, second_derivatives) {
  diff2ShouldBe ("(sin x)", "(* -1 (sin x))");
  diff2ShouldBe ("(exp x)", "(exp x)");
  const char *s = diff (diff ("(^ x 3)"));
  cr_assert (!strcmp (s, "(* 3 (* 2 x))") || !strcmp (s, "(* 6 x)"), "diff (diff (^ x 3)) incorrect");
}
#endif


void diffShouldBe (char *input, char *expected)
{
  char *result = diff(input);
  
  if (strcmp(expected,result))
    printf ("diff (%s) should be %s, got %s\n", input, expected, result);
}

void diff2ShouldBe (char *input, char *expected)
{
  char *result = diff (diff (input));
  if (strcmp(expected,result))
    printf ("diff (diff (%s)) should be %s, got %s\n", input, expected, result);
}


int main(int argc, char* argv[])
{
#if 1

  diffShouldBe ("5", "0");
  diffShouldBe ("x", "1");
  diffShouldBe ("(+ x x)", "2");
  diffShouldBe ("(- x x)", "0");
  
  diffShouldBe ("(* x 2)", "2");
  diffShouldBe ("(/ x 2)", "0.5");
  diffShouldBe ("(^ x 2)", "(* 2 x)");
  
  
  diffShouldBe ("(cos x)", "(* -1 (sin x))");
  diffShouldBe ("(sin x)", "(cos x)");
  diffShouldBe ("(exp x)", "(exp x)");
  diffShouldBe ("(ln x)", "(/ 1 x)");
  diffShouldBe ("(+ x (+ x x))", "3");
  diffShouldBe ("(- (+ x x) x)", "1");
  diffShouldBe ("(* 2 (+ x 2))", "2");
  diffShouldBe ("(/ 2 (+ 1 x))", "(/ -2 (^ (+ 1 x) 2))");
  diffShouldBe ("(cos (+ x 1))", "(* -1 (sin (+ x 1)))");
  diffShouldBe ("(sin (+ x 1))", "(cos (+ x 1))");
  diffShouldBe ("(sin (* 2 x))", "(* 2 (cos (* 2 x)))");
  diffShouldBe ("(exp (* 2 x))", "(* 2 (exp (* 2 x)))");
  
#endif



  

#if 0
#define N 3
  char* s[N]={"(* x 2)","(/ x 2)","(^ x 2)"};
  int i;
  for (i=0;i<N;++i)
    printf("d/dx[ %s ] == %s\n",s[i],diff(s[i]));
  
#endif
  printf("\nmallocCalls == %i\n",mallocCalls);
  return 0;
}
