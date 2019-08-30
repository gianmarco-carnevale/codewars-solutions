/*------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
/*------------------------------------------------------------------------------------------------------*/
#define TOKEN_NUMBER               1
#define TOKEN_IDENTIFIER          (TOKEN_NUMBER+1)
#define TOKEN_ASSIGNMENT          (TOKEN_IDENTIFIER+1)
#define TOKEN_OPERATOR            (TOKEN_ASSIGNMENT+1)
#define TOKEN_BRACKET             (TOKEN_OPERATOR+1)

#define TOKEN_OPERATOR_PLUS       1
#define TOKEN_OPERATOR_MINUS      (TOKEN_OPERATOR_PLUS+1)
#define TOKEN_OPERATOR_MULTIPLY   (TOKEN_OPERATOR_MINUS+1)
#define TOKEN_OPERATOR_DIVIDE     (TOKEN_OPERATOR_MULTIPLY+1)
#define TOKEN_OPERATOR_REMAINDER  (TOKEN_OPERATOR_DIVIDE+1)
#define TOKEN_OPERATOR_NEGATE     (TOKEN_OPERATOR_REMAINDER+1)

#define TOKEN_NUMBER_INTEGER      1
#define TOKEN_NUMBER_FLOAT        (TOKEN_NUMBER_INTEGER+1)
/*------------------------------------------------------------------------------------------------------*/
union NumberUnion;
union TokenUnion;
struct NumberStruct;
struct IdentifierStruct;
struct OperatorStruct;
struct BracketStruct;
struct Token;
struct IdentifierData;
struct IdentifierList;
struct TokenBlock;
struct TokenStack;
/*--------------------------------------------------------*/
union NumberUnion
{
  long int intValue;
  double floatValue;
};

/*--------------------------------------------------------*/
struct IdentifierStruct
{
  int id;
};

/*--------------------------------------------------------*/
struct OperatorStruct
{
  int opcode;
};

/*--------------------------------------------------------*/
struct BracketStruct
{
  int open;
};

/*--------------------------------------------------------*/
struct NumberStruct
{
  int type;
  union NumberUnion uNumber;
};

/*--------------------------------------------------------*/
union TokenUnion
{
  struct IdentifierStruct sIdentifier;
  struct NumberStruct sNumber;
  struct OperatorStruct sOperator;
  struct BracketStruct sBracket;
};

/*--------------------------------------------------------*/
struct Token
{
  int type;
  union TokenUnion uToken;
};

/*--------------------------------------------------------*/
struct IdentifierData
{
  char* name;
  int numberIsValid;
  struct NumberStruct sNumber;
};

/*--------------------------------------------------------*/
struct IdentifierList
{
  unsigned int length;
  struct IdentifierData* list;
};

/*--------------------------------------------------------*/
struct TokenList
{
  unsigned int length;
  struct Token* list;
};

/*--------------------------------------------------------*/
struct TokenBlock
{
  struct Token t;
  struct TokenBlock* prev;
  struct TokenBlock* next;
};

/*--------------------------------------------------------*/
struct TokenStack
{
  int length;
  struct TokenBlock* front;
  struct TokenBlock* back;
};

/*--------------------------------------------------------*/
static int addId(struct IdentifierList* p, char* newName)
{
  struct IdentifierData* temp;
  char* tempString;
  if (newName==NULL)
  {
    printf("ERROR: null string for identifier name\n");
    return -1;
  }
  if (strlen(newName)==0)
  {
    printf("ERROR: empty string for identifier name\n");
    return -1;
  }
  if (p==NULL)
  {
    printf("ERROR: null pointer for identifier list\n");
    return -1;
  }
  if (p->list==NULL)
  {
    p->length=0;
    temp = (struct IdentifierData*)malloc(sizeof(struct IdentifierData));
  }
  else
  {
    temp = (struct IdentifierData*)realloc(p->list,(p->length+1)*sizeof(struct IdentifierData));
  }
  if (temp==NULL)
  {
    printf("ERROR: allocation 1 in addId\n");
    return -1;
  }
  p->list = temp;
  tempString = (char*)malloc(sizeof(char)*(1+strlen(newName)));
  if (tempString==NULL)
  {
    printf("ERROR: allocation 2 in addId\n");
    return -1;
  }
  strcpy(tempString,newName);
  p->list[p->length].name = tempString;
  p->list[p->length].numberIsValid = 0;
  memset(&(p->list[p->length].sNumber),0,sizeof(struct NumberStruct));
  p->length += 1;
  return p->length-1;
}

/*-----------------------------------------------------------*/
static int findId(struct IdentifierList* p, char* newName)
{
  int i;
  if (p==NULL)
  {
    printf("ERROR: null pointer for identifier list\n");
    return -1;
  }
  if (p->list==NULL)
  {
    return -1;
  }
  if (newName==NULL)
  {
    printf("ERROR: null pointer for new name\n");
    return -1;
  }
  if (strlen(newName)==0)
  {
    printf("ERROR: empty new name\n");
    return -1;
  }
  for (i=0;i<p->length;++i)
  {
    if (strcmp(newName,p->list[i].name)==0)
      return i;
  }
  return -1;
}

/*------------------------------------------------------------------------------------*/
static int putValue(struct IdentifierList* p, int id, struct NumberStruct *pNumber)
{
  if (p==NULL)
  {
    printf("ERROR: null pointer for identifier list\n");
    return -1;
  }
  if (id>=p->length)
  {
    printf("ERROR: id exceeds length\n");
    return -1;
  }
  if (id<0)
  {
    printf("ERROR: negative id\n");
    return -1;
  }
  memcpy(&(p->list[id].sNumber),pNumber,sizeof(struct NumberStruct));
  p->list[id].numberIsValid=1;
  return 0;
}

/*--------------------------------------------------------*/
static void clearIdList(struct IdentifierList* p)
{
  int i;
  if (p==NULL)
    return;
  for (i=0;i<p->length;++i)
  {
    if (p->list[i].name)
    {
      free(p->list[i].name);
    }
  }
  free(p->list);
  p->list=NULL;
  p->length=0;
}

/*------------------------------------------------------------------------------------------------------*/
static int addToken(struct TokenList *pList, const struct Token* pToken)
{
  struct Token* temp;
  if (pList==NULL)
    return -1;
  if (pList->list==NULL)
  {
    pList->length=0;
    pList->list = (struct Token*)malloc(sizeof(struct Token));
    if (pList->list==NULL)
    {
      printf("ERROR: allocation in addToken\n");
      return -1;
    }
  }
  else
  {
    temp = (struct Token*)realloc(pList->list,(pList->length+1)*sizeof(struct Token));
    if (temp==NULL)
    {
      free(pList->list);
      pList->list=NULL;
      printf("ERROR: allocation in addToken 2\n");
      return -1;
    }
    pList->list = temp;
  }
  memcpy(&(pList->list[pList->length]),pToken,sizeof(struct Token));
  ++pList->length;
  return 0;
}

/*------------------------------------------------------------------------------------------------------*/
static void clearTokenList(struct TokenList *pList)
{
  if (pList)
  {
    if (pList->list)
    {
      free(pList->list);
    }
    pList->list=NULL;
    pList->length=0;
  }
}

/*------------------------------------------------------------------------------------------------------*/
static int popListFromFront(struct TokenList *pList, struct Token* pToken)
{
  struct Token* temp;
  int i;
  if (pList->list==NULL)
    return -1;
  if (pList->length==1)
  {
    memcpy(pToken,&(pList->list[0]),sizeof(struct Token));
    clearTokenList(pList);
    return 0;
  }
  temp = (struct Token*)malloc((pList->length-1)*sizeof(struct Token));
  if (temp==NULL)
  {
    return -1;
  }
  for (i=1;i<pList->length;++i)
  {
    memcpy(&temp[i-1],&(pList->list[i]),sizeof(struct Token));
  }
  memcpy(pToken,&(pList->list[0]),sizeof(struct Token));
  free(pList->list);
  pList->list = temp;
  pList->length -= 1;
  return 0;
}

/*------------------------------------------------------------------------------------------------------*/
static int isIdChar(char c)
{
  if (isalpha(c))
    return 1;
  if (c=='_')
    return 1;
  return 0;
}

/*---------------------------------------------------------------------------------------------*/
static int isNumeric(char c)
{
  if (isdigit(c))
    return 1;
  if (c=='.')
    return 1;
  return 0;
}

/*---------------------------------------------------------------------------------------------*/
static int makeNumberFromString(struct IdentifierList* pList, struct Token *pToken, char* s)
{
  char *end;
  float f;
  long l;
  if (strchr(s,'.')==NULL)
  {
    l = strtol(s,&end,10);
    if (end[0]==0)
    {
      pToken->type = TOKEN_NUMBER;
      pToken->uToken.sNumber.type = TOKEN_NUMBER_INTEGER;
      pToken->uToken.sNumber.uNumber.intValue = l;
      return 0;
    }
    return -1;
  }
  else
  {
    f = strtod(s,&end);
    if (end[0]==0)
    {
      pToken->type = TOKEN_NUMBER;
      pToken->uToken.sNumber.type = TOKEN_NUMBER_FLOAT;
      pToken->uToken.sNumber.uNumber.floatValue = f;
      return 0;
    }
    return -1;
  }
  return -1;
}

/*-------------------------------------------------------------------------------------------------*/
static int makeIdentifierFromString(struct IdentifierList* pList, struct Token *pToken, char* s)
{
  int temp;
  temp = findId(pList,s);
  if (temp==-1)
  {
    temp = addId(pList,s);
    if (temp==-1)
      return -1;
    pToken->type = TOKEN_IDENTIFIER;
    pToken->uToken.sIdentifier.id = temp;
    return 0;
  }
  else
  {
    pToken->type = TOKEN_IDENTIFIER;
    pToken->uToken.sIdentifier.id = temp;
    return 0;
  }
}

/*-------------------------------------------------------------------------------------------------*/
static void makeIntegerNumber(struct Token *p, long int n)
{
  p->type = TOKEN_NUMBER;
  p->uToken.sNumber.type = TOKEN_NUMBER_INTEGER;
  p->uToken.sNumber.uNumber.intValue = n;
}

/*-------------------------------------------------------------------------------------------------*/
static void makeFloatNumber(struct Token *p, double n)
{
  p->type = TOKEN_NUMBER;
  p->uToken.sNumber.type = TOKEN_NUMBER_FLOAT;
  p->uToken.sNumber.uNumber.floatValue = n;
}

/*-------------------------------------------------------------------------------------------------*/
static void makeIdentifierFromId(struct Token *p, int id)
{
  p->type = TOKEN_IDENTIFIER;
  p->uToken.sIdentifier.id = id;
}

/*-------------------------------------------------------------------------------------------------*/
static void makeOperator(struct Token *p, int opcode)
{
  p->type = TOKEN_OPERATOR;
  p->uToken.sOperator.opcode = opcode;
}

/*-------------------------------------------------------------------------------------------------*/
static void makeBracket(struct Token *p, int open)
{
  p->type = TOKEN_BRACKET;
  p->uToken.sBracket.open = open;
}

/*-------------------------------------------------------------------------------------------------*/
static void makeAssignment(struct Token *p)
{
  p->type = TOKEN_ASSIGNMENT;
}

/*------------------------------------------------------------------------------------------------------*/
#define STATE_AWAITING_FIRST_CHARACTER      0
#define STATE_PUTTING_TOGETHER_IDENTIFIER   1
#define STATE_PUTTING_TOGETHER_NUMBER       2
#define STATE_PROCESSING_SYMBOLS            3
/*------------------------------------------------------------------------------------------------------*/
static int makeTokenList(char* input, struct TokenList* pTokenList, struct IdentifierList* pIdList)
{
  int state;
  char c;
  const int tokenStringLength=64;
  char tokenString[tokenStringLength];
  int tokenIndex;
  struct Token singleToken;
  int minusIsUnary;
  for (minusIsUnary=1,state=0,tokenIndex=0;;)
  {
    c = input[0];
    switch (state)
    {
      case STATE_AWAITING_FIRST_CHARACTER:
        if (isalpha(c))
        {
          if (tokenIndex>=tokenStringLength)
          {
            printf("ERROR: exceeding maximum token length (1)\n");
            return -1;
          }
          tokenString[tokenIndex++]=c;
          tokenString[tokenIndex]=0;
          ++input;
          state=STATE_PUTTING_TOGETHER_IDENTIFIER;
        }
        else
        {
          if (isdigit(c))
          {
            if (tokenIndex>=tokenStringLength)
            {
              printf("ERROR: exceeding maximum token length (2)\n");
              return -1;
            }
            tokenString[tokenIndex++]=c;
            tokenString[tokenIndex]=0;
            ++input;
            state=STATE_PUTTING_TOGETHER_NUMBER;
          }
          else
          {
            if (c==0)
              return 0;
            else
            {
              if (c==' ')
              {
                ++input;
              }
              else
              {
                state=STATE_PROCESSING_SYMBOLS;
              }
            }
          }
        }
      break;
      case STATE_PUTTING_TOGETHER_IDENTIFIER:
        if (isIdChar(c))
        {
          tokenString[tokenIndex++]=c;
          tokenString[tokenIndex]=0;
          ++input;
        }
        else
        {
          if (makeIdentifierFromString(pIdList,&singleToken,tokenString))
          {
            printf("ERROR: makeIdentifierFromString failed in state %i with token %s\n",state,tokenString);
            fflush(stdout);
            return -1;
          }
          if (addToken(pTokenList,&singleToken))
          {
            printf("ERROR: addToken failed in state %i\n",state);
            fflush(stdout);
            return -1;
          }
          minusIsUnary=0;
          tokenIndex=0;
          if (c==0)
            return 0;
          else
          {
            state=STATE_AWAITING_FIRST_CHARACTER;
          }
        }
      break;
      case STATE_PUTTING_TOGETHER_NUMBER:
        if (isNumeric(c))
        {
          tokenString[tokenIndex++]=c;
          tokenString[tokenIndex]=0;
          ++input;
        }
        else
        {
          if (makeNumberFromString(pIdList,&singleToken,tokenString))
          {
            printf("ERROR: makeNumberFromString failed in state %i with string %s\n",state,tokenString);
            fflush(stdout);
            return -1;
          }
          if (addToken(pTokenList,&singleToken))
          {
            printf("ERROR: addToken failed in state %i\n",state);
            fflush(stdout);
            return -1;
          }
          minusIsUnary=0;
          tokenIndex=0;
          if (c==0)
            return 0;
          else
          {
            state=STATE_AWAITING_FIRST_CHARACTER;
          }
        }
      break;
      case STATE_PROCESSING_SYMBOLS:
        switch (c)
        {
          case ' ':break;
          case '=':
            makeAssignment(&singleToken);
            minusIsUnary=1;
          break;
          case '+':
            makeOperator(&singleToken,TOKEN_OPERATOR_PLUS);
            minusIsUnary=0;
          break;
          case '-':
            if (minusIsUnary)
            {
              makeOperator(&singleToken,TOKEN_OPERATOR_NEGATE);
              minusIsUnary=0;
            }
            else
            {
              makeOperator(&singleToken,TOKEN_OPERATOR_MINUS);
              minusIsUnary=1;
            }
          break;
          case '*':
            makeOperator(&singleToken,TOKEN_OPERATOR_MULTIPLY);
            minusIsUnary=1;
          break;
          case '/':
            makeOperator(&singleToken,TOKEN_OPERATOR_DIVIDE);
            minusIsUnary=1;
          break;
          case '%':
            makeOperator(&singleToken,TOKEN_OPERATOR_REMAINDER);
            minusIsUnary=1;
          break;
          case '(':
            makeBracket(&singleToken,1);
            minusIsUnary=1;
          break;
          case ')':
            makeBracket(&singleToken,0);
            minusIsUnary=0;
          break;
          default:
            printf("ERROR: unhandled symbol %c\n",c);
            return -1;
          break;
        }
        if (addToken(pTokenList,&singleToken))
        {
          printf("ERROR: addToken failed in state %i\n",state);
          fflush(stdout);
          return -1;
        }
        ++input;
        state=STATE_AWAITING_FIRST_CHARACTER;
      break;
      default:
        printf("ERROR: unhandled parser state %i\n",state);
        return -1;
      break;
    }
  }
  return -1;
}

/*------------------------------------------------------------------------------------------------------*/
static void printToken(const struct Token* p, const struct IdentifierList* pIdList)
{
  if (p==NULL)
    return;
  if (pIdList==NULL)
    return;
  switch (p->type)
  {
    case TOKEN_NUMBER:
      switch (p->uToken.sNumber.type)
      {
        case TOKEN_NUMBER_INTEGER:printf("%li",p->uToken.sNumber.uNumber.intValue);break;
        case TOKEN_NUMBER_FLOAT:printf("%f",p->uToken.sNumber.uNumber.floatValue);break;
        default:break;
      }
    break;
    case TOKEN_IDENTIFIER:
      printf("%s",pIdList->list[p->uToken.sIdentifier.id].name);
    break;
    case TOKEN_ASSIGNMENT:
      printf("=");
    break;
    case TOKEN_OPERATOR:
      switch (p->uToken.sOperator.opcode)
      {
        case TOKEN_OPERATOR_PLUS:printf("+");break;
        case TOKEN_OPERATOR_MINUS:printf("-");break;
        case TOKEN_OPERATOR_MULTIPLY:printf("*");break;
        case TOKEN_OPERATOR_DIVIDE:printf("/");break;
        case TOKEN_OPERATOR_REMAINDER:printf("%");break;
        case TOKEN_OPERATOR_NEGATE:printf("~");break;
        default:break;
      }
    break;
    case TOKEN_BRACKET:
      if (p->uToken.sBracket.open)
        printf("(");
      else
        printf(")");
    break;
    default:
      printf("@@@");
    break;
  }
}

/*----------------------------------------------------------------------------------------------------*/
static void printTokenList(const struct TokenList* pTokenList, const struct IdentifierList* pIdList)
{
  int i;
  if (pTokenList==NULL)
    return;
  if (pIdList==NULL)
    return;
  for (i=0;i<pTokenList->length;++i)
  {
    printToken(&(pTokenList->list[i]),pIdList);
  }
  printf("\n");
}

/*---------------------------------------------------------------------------------------------*/
static int pushBack(struct TokenStack* pStack, const struct Token* pToken)
{
  struct TokenBlock* temp;
  if (pStack==NULL)
    return -1;
  if (pToken==NULL)
    return -1;
  temp = (struct TokenBlock*)malloc(sizeof(struct TokenBlock));
  if (temp==NULL)
  {
    printf("ERROR: malloc failed in pushBack\n");
    return -1;
  }
  memcpy(&(temp->t),pToken,sizeof(struct Token));
  temp->prev = NULL;
  temp->next = NULL;
  if (pStack->back==NULL)
  {
    pStack->back = temp;
    pStack->length=1;
  }
  else
  {
    temp->prev = pStack->back;
    pStack->back->next = temp;
    pStack->back = temp;
    ++(pStack->length);
  }
  if (pStack->front==NULL)
  {
    pStack->front = pStack->back;
  }
  return 0;
}

/*---------------------------------------------------------------------------------------------*/
static int popBack(struct TokenStack* pStack, struct Token* pToken)
{
  struct TokenBlock *temp;
  if (pStack==NULL)
    return -1;
  if (pStack->back==NULL)
    return -1;
  if (pToken!=NULL)
  {
    memcpy(pToken,&(pStack->back->t),sizeof(struct Token));
  }
  temp = pStack->back->prev;
  if (temp)
  {
    temp->next = NULL;
  }
  free(pStack->back);
  pStack->back = temp;
  if (pStack->length==1)
  {
    pStack->front = pStack->back;
  }
  pStack->length-=1;
  return 0;
}

/*---------------------------------------------------------------------------------------------*/
static int popFront(struct TokenStack* pStack, struct Token* pToken)
{
  struct TokenBlock *temp;
  if (pStack==NULL)
    return -1;
  if (pStack->front==NULL)
    return -1;
  if (pToken!=NULL)
  {
    memcpy(pToken,&(pStack->front->t),sizeof(struct Token));
  }
  temp = pStack->front->next;
  if (temp)
  {
    temp->prev = NULL;
  }
  free(pStack->front);
  pStack->front = temp;
  if (pStack->length==1)
  {
    pStack->back = pStack->front;
  }
  pStack->length-=1;
  return 0;
}

/*---------------------------------------------------------------------------------------------*/
static void clearStack(struct TokenStack* pStack)
{
  struct TokenBlock *temp1, *temp2;
  if (pStack==NULL)
    return;
  for (temp1=pStack->front;temp1;)
  {
    temp2 = temp1->next;
    free(temp1);
    temp1 = temp2;
  }
  pStack->front=NULL;
  pStack->back=NULL;
  pStack->length=0;
}

/*---------------------------------------------------------------------------------------------*/
static void printStack(const struct TokenStack* pStack, const struct IdentifierList* pIdList)
{
  struct TokenBlock *temp;
  if (pStack==NULL)
    return;
  printf(": ");
  if (pStack->length>0)
  {
    for (temp=pStack->front;temp;temp=temp->next)
    {
      printToken(&temp->t,pIdList);
      printf(", ");
    }
    printf("\n");
  }
}

/*---------------------------------------------------------------------------------*/
static int getPriority(const struct Token* p)
{
  if (p->type==TOKEN_ASSIGNMENT)
    return 0;
  if (p->type==TOKEN_OPERATOR)
  {
    switch (p->uToken.sOperator.opcode)
    {
      case TOKEN_OPERATOR_NEGATE: return 5;
      case TOKEN_OPERATOR_MULTIPLY: return 3;
      case TOKEN_OPERATOR_DIVIDE: return 3;
      case TOKEN_OPERATOR_PLUS: return 2;
      case TOKEN_OPERATOR_MINUS: return 2;
      case TOKEN_OPERATOR_REMAINDER: return 4;
      default: return -1;
    }
  }
  return -1;
}

/*---------------------------------------------------------------------------------*/
static int higherPrecedence(const struct Token* first, const struct Token* second)
{
  if (getPriority(first) >= getPriority(second))
    return 1;
  return 0;
}

/*---------------------------------------------------------------------------------------------------------------------------------------*/
static int numberOperation(const struct NumberStruct* first, const struct NumberStruct* second, int opcode, struct NumberStruct* result)
{
  long int tempValue, tempRemainder;
  switch (opcode)
  {
    case TOKEN_OPERATOR_PLUS:
      if ((first->type == TOKEN_NUMBER_INTEGER) && (second->type == TOKEN_NUMBER_INTEGER))
      {
        result->type = TOKEN_NUMBER_INTEGER;
        result->uNumber.intValue = first->uNumber.intValue + second->uNumber.intValue;
        return 0;
      }
    break;
    case TOKEN_OPERATOR_MINUS:
      if ((first->type == TOKEN_NUMBER_INTEGER) && (second->type == TOKEN_NUMBER_INTEGER))
      {
        result->type = TOKEN_NUMBER_INTEGER;
        result->uNumber.intValue = first->uNumber.intValue - second->uNumber.intValue;
        return 0;
      }
    break;
    case TOKEN_OPERATOR_MULTIPLY:
      if ((first->type == TOKEN_NUMBER_INTEGER) && (second->type == TOKEN_NUMBER_INTEGER))
      {
        result->type = TOKEN_NUMBER_INTEGER;
        result->uNumber.intValue = first->uNumber.intValue * second->uNumber.intValue;
        return 0;
      }
    break;
    case TOKEN_OPERATOR_DIVIDE:
      if ((first->type == TOKEN_NUMBER_INTEGER) && (second->type == TOKEN_NUMBER_INTEGER))
      {
        if (second->uNumber.intValue==0)
        {
          printf("ERROR: division by zero\n");
          return -1;
        }
        tempValue = first->uNumber.intValue / second->uNumber.intValue;
        tempRemainder = first->uNumber.intValue % second->uNumber.intValue;
        if (tempRemainder)
        {
          result->type = TOKEN_NUMBER_FLOAT;
          result->uNumber.floatValue = (double)first->uNumber.intValue / (double)second->uNumber.intValue;
          return 0;
        }
        else
        {
          result->type = TOKEN_NUMBER_INTEGER;
          result->uNumber.intValue = first->uNumber.intValue / second->uNumber.intValue;
          return 0;
        }
      }
    break;
    case TOKEN_OPERATOR_REMAINDER:
      if ((first->type == TOKEN_NUMBER_INTEGER) && (second->type == TOKEN_NUMBER_INTEGER))
      {
        if (second->uNumber.intValue==0)
        {
          printf("ERROR: remainder after division by zero\n");
          return -1;
        }
        result->type = TOKEN_NUMBER_INTEGER;
        result->uNumber.intValue = first->uNumber.intValue % second->uNumber.intValue;
        return 0;
      }
    break;
    case TOKEN_OPERATOR_NEGATE:
      if (second!=NULL)
      {
        printf("ERROR: unexpected second argument for negate\n");
        return -1;
      }
      if (first->type == TOKEN_NUMBER_INTEGER)
      {
        result->type = TOKEN_NUMBER_INTEGER;
        result->uNumber.intValue = -first->uNumber.intValue;
        return 0;
      }
    break;
    default:
      return -1;
    break;
  }
  return -1;
}

/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
static int executeOperation(const struct Token* first, const struct Token* second, const struct Token* operation, struct Token* result, struct IdentifierList* pIdList)
{
  struct NumberStruct temp1, temp2;
  if (operation->type == TOKEN_ASSIGNMENT)
  {
    if (first->type == TOKEN_IDENTIFIER)
    {
      if (second->type == TOKEN_IDENTIFIER)
      {
        if (pIdList->list[second->uToken.sIdentifier.id].numberIsValid)
        {
          result->type = TOKEN_NUMBER;
          result->uToken.sNumber = pIdList->list[second->uToken.sIdentifier.id].sNumber;
          putValue(pIdList,first->uToken.sIdentifier.id,&result->uToken.sNumber);
          return 0;
        }
        else
        {
          printf("ERROR: unknown value for assignment\n");
          return -1;
        }
      }
      else
      {
        if (second->type == TOKEN_NUMBER)
        {
          result->type = TOKEN_NUMBER;
          result->uToken.sNumber = second->uToken.sNumber;
          putValue(pIdList,first->uToken.sIdentifier.id,&result->uToken.sNumber);
          return 0;
        }
        else
        {
          printf("ERROR: expected identifier or number\n");
          return -1;
        }
      }
    }
    else
    {
      printf("ERROR: identifier required for assignment\n");
      return -1;
    }
  }
  else
  {
    if (operation->type == TOKEN_OPERATOR)
    {
      if (first->type == TOKEN_IDENTIFIER)
      {
        if (pIdList->list[first->uToken.sIdentifier.id].numberIsValid)
        {
          temp1 = pIdList->list[first->uToken.sIdentifier.id].sNumber;
        }
        else
        {
          printf("ERROR: identifier has no value assigned yet (1)\n");
          return -1;
        }
      }
      else
      {
        if (first->type == TOKEN_NUMBER)
        {
          temp1 = first->uToken.sNumber;
        }
        else
        {
          printf("ERROR: identifier or value needed\n");
          return -1;
        }
      }
      if (second!=NULL)
      {
        if (second->type == TOKEN_IDENTIFIER)
        {
          if (pIdList->list[second->uToken.sIdentifier.id].numberIsValid)
          {
            temp2 = pIdList->list[second->uToken.sIdentifier.id].sNumber;
          }
          else
          {
            printf("ERROR: identifier has no value assigned yet (2)\n");
            return -1;
          }
        }
        else
        {
          if (second->type == TOKEN_NUMBER)
          {
            temp2 = second->uToken.sNumber;
          }
          else
          {
            printf("ERROR: identifier or value needed\n");
            return -1;
          }
        }
        result->type = TOKEN_NUMBER;
        if (numberOperation(&temp1,&temp2,operation->uToken.sOperator.opcode,&(result->uToken.sNumber)))
        {
          printf("ERROR: in numberOperation");
          return -1;
        }
      }
      else
      {
        if (operation->uToken.sOperator.opcode==TOKEN_OPERATOR_NEGATE)
        {
          result->type = TOKEN_NUMBER;
          if (numberOperation(&temp1,NULL,TOKEN_OPERATOR_NEGATE,&(result->uToken.sNumber)))
          {
            printf("ERROR: negate failed\n");
            return -1;
          }
        }
        else
        {
          printf("ERROR: only negate requires one argument\n");
          return -1;
        }
      }
    }
    else
    {
      printf("ERROR: expected operation or assignment\n");
      return -1;    
    }   
  }
  return 0;
}

/*----------------------------------------------------------------------------------------------------*/
static int executeStack(struct TokenStack* pStack, struct Token* result, struct IdentifierList* pIdList)
{
  struct Token operation, temp, first, second;
  struct TokenStack localStack = {0,NULL,NULL};
  for (;pStack->length>0;)
  {
    popFront(pStack,&temp);
    switch (temp.type)
    {
      case TOKEN_NUMBER:
      case TOKEN_IDENTIFIER:
        pushBack(&localStack,&temp);
      break;
      case TOKEN_OPERATOR:
        if (temp.uToken.sOperator.opcode == TOKEN_OPERATOR_NEGATE)
        {
          if (localStack.length<1)
          {
            printf("ERROR: underflow (1)\n");
            return -1;
          }
          popBack(&localStack,&first);
          operation = temp;
          if (executeOperation(&first, NULL, &operation, &temp, pIdList))
          {
            printf("ERROR: executeOperation in executeStack (1)\n");
            return -1;
          }
          pushBack(&localStack,&temp);
        }
        else
        {
          if (localStack.length<2)
          {
            printf("ERROR: underflow (2)\n");
            return -1;
          }
          popBack(&localStack,&second);
          popBack(&localStack,&first);
          operation = temp;
          if (executeOperation(&first, &second, &operation, &temp, pIdList))
          {
            printf("ERROR: executeOperation in executeStack (2)\n");
            return -1;
          }
          pushBack(&localStack,&temp);
        }
      break;
      case TOKEN_ASSIGNMENT:
        if (localStack.length<2)
        {
          printf("ERROR: underflow (3)\n");
          return -1;
        }
        popBack(&localStack,&second);
        popBack(&localStack,&first);
        operation = temp;
        if (executeOperation(&first, &second, &operation,&temp, pIdList))
        {
          printf("ERROR: executeOperation in flush (3)\n");
          return -1;
        }
        pushBack(&localStack,&temp);
      break;
      default:
       printf("ERROR: unhandled type %i in executeStack\n",temp.type);
       return -1;
      break;
    }
  }
  if (localStack.length!=1)
  {
    printf("ERROR: stack flow\n");
    return -1;
  }
  popBack(&localStack,&temp);
  clearStack(&localStack);
  switch (temp.type)
  {
    case TOKEN_NUMBER:
      *result = temp;
      return 0;
    break;
    case TOKEN_IDENTIFIER:
      if (pIdList->list[temp.uToken.sIdentifier.id].numberIsValid)
      {
        result->type = TOKEN_NUMBER;
        result->uToken.sNumber = pIdList->list[temp.uToken.sIdentifier.id].sNumber;
        return 0;
      }   
    break;
    default:
    break;
  }
  return -1;
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------------*/
static int buildReverseStack(struct TokenList* pList, struct TokenStack* pOutStack, struct IdentifierList* pIdList)
{
  struct Token next, operation, temp;
  int keepLooping;
  struct TokenStack operatorStack = {0,NULL,NULL};
  for (;pList->length>0;)
  {
    if (popListFromFront(pList,&next))
    {
      printf("ERROR: popListFromFront failed in buildReverseStack\n");
      return -1;
    }
    switch (next.type)
    {
      case TOKEN_IDENTIFIER:
      case TOKEN_NUMBER:
        pushBack(pOutStack,&next);
      break;
      case TOKEN_BRACKET:
        if (next.uToken.sBracket.open)
        {
          pushBack(&operatorStack,&next);
        }
        else
        {
          for (keepLooping=1;keepLooping;)
          {
            if (operatorStack.length>0)
            {
              memcpy(&temp,operatorStack.back,sizeof(struct Token));
              if (temp.type!=TOKEN_BRACKET)
              {
                popBack(&operatorStack,NULL);
                pushBack(pOutStack,&temp);
              }
              else
              {
                if (temp.uToken.sBracket.open)
                {
                  popBack(&operatorStack,NULL);
                  keepLooping=0;
                }
              }
            }
          }
        }
      break;
      case TOKEN_ASSIGNMENT:
      case TOKEN_OPERATOR:
        for (keepLooping=1;(operatorStack.length>0)&&keepLooping;)
        {
          memcpy(&temp,operatorStack.back,sizeof(struct Token));
          if (higherPrecedence(&temp,&next))
          {
            popBack(&operatorStack,NULL);
            pushBack(pOutStack,&temp);
          }
		  else
		  {
		    keepLooping=0;
		  }
        }
        pushBack(&operatorStack,&next);
      break;
      default:
        printf("ERROR: unhandled type %i in executeList\n",next.type);
        return -1;
    }
  }
  for (;operatorStack.length>0;)
  {
    popBack(&operatorStack,&temp);
    pushBack(pOutStack,&temp);
  }
  clearStack(&operatorStack);
  return 0;
}

typedef int Type;

/* Status codes */
enum StatusCodes {
    OK_CODE = 0,          /* Success */
    EMPTY_CODE = 1,       /* Input string consists entirely of whitespaces */
    /* 2, 3, ... */       /* Errors */
};

struct IdentifierList iList={0,NULL};
static int getTokenResult(char* input, struct Token* result)
{
  struct TokenList tList = {0,NULL};
  struct TokenStack outputStack = {0,NULL,NULL};
  if (makeTokenList(input,&tList,&iList))
  {
    printf("ERROR: in makeTokenList\n");
    clearTokenList(&tList);
    return -1;
  }
  printf("Tokenlist: ");
  printTokenList(&tList,&iList);
  if (tList.length==0)
  {
    return EMPTY_CODE;
  }
  if (buildReverseStack(&tList, &outputStack, &iList))
  {
    clearStack(&outputStack);
    clearTokenList(&tList);
    return -1;
  }
  printf("Outputstack:");
  printStack(&outputStack, &iList);
  clearTokenList(&tList);
  if (executeStack(&outputStack,result,&iList))
  {
    clearStack(&outputStack);
    return -1;
  }
  clearStack(&outputStack);
  return OK_CODE;
}



/* initInterpreter: initialize the interpreter if necessary and return
   a status code (any value other than OK_CODE is treated as an error) */
int initInterpreter(void)
{
    iList.length=0;
    iList.list=NULL;
    return OK_CODE;
}

/* closeInterpreter: close the interpreter and free memory if necessary */
void closeInterpreter(void)
{
    clearIdList(&iList);
    return;
}

/* evaluate: evaluate the string expression, and return a status code
   (any value other than OK_CODE and EMPTY_CODE is treated as an error).
   The result of evaluating the expression is placed in a variable
   by the pointer 'result' if the function returns OK_CODE. */
int evaluate(char *input, Type *result)
{
    int retValue;
    struct Token finalValue;
    retValue = getTokenResult(input,&finalValue);
    if (retValue == OK_CODE)
    {
      if (finalValue.type==TOKEN_NUMBER)
      {
        if (finalValue.uToken.sNumber.type==TOKEN_NUMBER_INTEGER)
        {
          *result = finalValue.uToken.sNumber.uNumber.intValue;
          return OK_CODE;
        }
        return -1;
      }
      return -1;
    }
    return retValue;
}


void doTest(char* input)
{
  struct Token r;
  printf("Input: %s\n",input);
  getTokenResult(input,&r);
  printToken(&r,&iList);printf("\n");
}

int main(int argc, char* argv[])
{
  doTest("25 + 4 * 7 - 17*1/1 + 20  / 10 * 5 -7");

  return 0;
}
