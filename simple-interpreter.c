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
static int addId(struct IdentifierList* p, char* newName)
{
  struct IdentifierData* temp;
  char* tempString;
  if (newName==NULL)
    return -1;
  if (strlen(newName)==0)
    return -1;
  if (p==NULL)
    return -1;
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
  return 0;
}

/*--------------------------------------------------------*/
static int findId(struct IdentifierList* p, char* newName)
{
  int i;
  if (p==NULL)
    return -1;
  if (p->list==NULL)
    return -1;
  if (newName==NULL)
    return -1;
  if (strlen(newName)==0)
    return -1;
  for (i=0;i<p->length;++i)
  {
    if (strcmp(newName,p->list[i].name)==0)
      return i;
  }
  return -1;
}

/*--------------------------------------------------------*/
static int putValue(struct IdentifierList* p, int id, struct NumberStruct *pNumber)
{
  if (p==NULL)
    return -1;
  if (id>=p->length)
    return -1;
  if (id<0)
    return -1;
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
static int popFront(struct TokenList *pList, struct Token* pToken)
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

static int isNumeric(char c)
{
  if (isdigit(c))
    return 1;
  if (c=='.')
    return 1;
  return 0;
}

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

static void makeIntegerNumber(struct Token *p, long int n)
{
  p->type = TOKEN_NUMBER;
  p->uToken.sNumber.type = TOKEN_NUMBER_INTEGER;
  p->uToken.sNumber.uNumber.intValue = n;
}

static void makeFloatNumber(struct Token *p, double n)
{
  p->type = TOKEN_NUMBER;
  p->uToken.sNumber.type = TOKEN_NUMBER_FLOAT;
  p->uToken.sNumber.uNumber.floatValue = n;
}

static void makeIdentifierFromId(struct Token *p, int id)
{
  p->type = TOKEN_IDENTIFIER;
  p->uToken.sIdentifier.id = id;
}

static void makeOperator(struct Token *p, int opcode)
{
  p->type = TOKEN_OPERATOR;
  p->uToken.sOperator.opcode = opcode;
}

static void makeBracket(struct Token *p, int open)
{
  p->type = TOKEN_BRACKET;
  p->uToken.sBracket.open = open;
}

static void makeAssignment(struct Token *p)
{
  p->type = TOKEN_ASSIGNMENT;
}
/*------------------------------------------------------------------------------------------------------*/
#define STATE_AWAITING_FIRST_CHARACTER      0
#define STATE_PUTTING_TOGETHER_IDENTIFIER   1
#define STATE_PUTTING_TOGETHER_NUMBER       2
#define STATE_PROCESSING_SYMBOLS            3

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
    /*
    if (c)
    {printf("Processing character %c in state %i\n",c,state);fflush(stdout);}
    else
    {printf("Reached the end of string in state %i\n",state);fflush(stdout);}
    */
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

struct TokenBlock
{
  struct Token t;
  struct TokenBlock* prev;
  struct TokenBlock* next;
};

struct TokenStack
{
  int length;
  struct TokenBlock* front;
  struct TokenBlock* back;
};

static int push(struct TokenStack* pStack, const struct Token* pToken)
{
  struct TokenBlock* temp;
  if (pStack==NULL)
    return -1;
  if (pToken==NULL)
    return -1;
  temp = (struct TokenBlock*)malloc(sizeof(struct TokenBlock));
  if (temp==NULL)
  {
    printf("ERROR: malloc failed in push\n");
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

static int pop(struct TokenStack* pStack, struct Token* pToken)
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

static int getPriority(const struct Token* p)
{
  if (p->type==TOKEN_ASSIGNMENT)
    return 0;
  if (p->type==TOKEN_OPERATOR)
  {
    switch (p->uToken.sOperator.opcode)
    {
      case TOKEN_OPERATOR_NEGATE: return 4;
      case TOKEN_OPERATOR_MULTIPLY: return 3;
      case TOKEN_OPERATOR_DIVIDE: return 3;
      case TOKEN_OPERATOR_PLUS: return 2;
      case TOKEN_OPERATOR_MINUS: return 2;
      case TOKEN_OPERATOR_REMAINDER: return 1;
      default: return -1;
    }
  }
  return -1;
}


static int higherPrecedence(const struct Token* first, const struct Token* second)
{
  if (getPriority(first)>=getPriority(second))
    return 1;
  return 0;
}

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

static int executeOperation(const struct Token* first, const struct Token* second, const struct Token* operation, struct Token* result, const struct IdentifierList* pIdList)
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
          pIdList->list[first->uToken.sIdentifier.id].sNumber = pIdList->list[second->uToken.sIdentifier.id].sNumber;
          pIdList->list[first->uToken.sIdentifier.id].numberIsValid=1;
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
          pIdList->list[first->uToken.sIdentifier.id].sNumber = second->uToken.sNumber;
          pIdList->list[first->uToken.sIdentifier.id].numberIsValid=1;
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
          printf("ERROR: identifier has no value assigned yet\n");
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
            printf("ERROR: identifier has no value assigned yet\n");
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


static int flushStack(struct TokenStack* pStack, struct Token* result, struct IdentifierList* pIdList)
{
  struct Token operation, temp, first, second;
  printf("flushStack hadling %i items\n",pStack->length);
  for (;pStack->length>0;)
  {
    pop(pStack,&temp);
    printf("flushStack processing ");printToken(&temp,pIdList);printf("\n");fflush(stdout);
    if (temp.type == TOKEN_OPERATOR)
    {
      if (temp.uToken.sOperator.opcode == TOKEN_OPERATOR_NEGATE)
      {
        if (pStack->length<1)
        {
          printf("ERROR: underflow\n");
          return -1;
        }
        pop(pStack,&first);
        operation = temp;
        if (executeOperation(&first, NULL, &operation, &temp, pIdList))
        {
          printf("ERROR: executeOperation in flush\n");
          return -1;
        }
        push(pStack,&temp);
      }
      else
      {
        if (pStack->length<2)
        {
          printf("ERROR: underflow\n");
          return -1;
        }
        pop(pStack,&second);
        pop(pStack,&first);
        operation = temp;
        if (executeOperation(&first, &second, &operation, &temp, pIdList))
        {
          printf("ERROR: executeOperation in flush\n");
          return -1;
        }
        push(pStack,&temp);
      }
    }
    else
    {
      if (temp.type == TOKEN_ASSIGNMENT)
      {
        if (pStack->length<2)
        {
          printf("ERROR: underflow\n");
          return -1;
        }
        pop(pStack,&second);
        pop(pStack,&first);
        operation = temp;
        if (executeOperation(&first, &second, &operation,&temp, pIdList))
        {
          printf("ERROR: executeOperation in flush\n");
          return -1;
        }
        push(pStack,&temp);
      }
    }
  }
  *result = temp;
  printf("flushStack returns ");
  printToken(result,pIdList);
  printf("\n");
  return 0;
}


static int executeList(struct TokenList* pList, struct Token* result, struct IdentifierList* pIdList, int openBracket)
{
  struct Token next, operation, temp;
  struct TokenStack output = {0,NULL,NULL};
  struct TokenStack opStack = {0,NULL,NULL};
  int keepLooping;
  for (keepLooping=1;(pList->length>0) && keepLooping;)
  {
    if (popFront(pList,&next))
    {
      printf("ERROR: popFront failed in executeList\n");
      return -1;
    }
    printf("Next==");
    printToken(&next,pIdList);
    printf("\n");
    fflush(stdout);
    switch (next.type)
    {
      case TOKEN_IDENTIFIER:
      case TOKEN_NUMBER:
        push(&output,&next);
      break;
      case TOKEN_BRACKET:
        if (next.uToken.sBracket.open)
        {
          if (executeList(pList,&temp,pIdList,1))
          {
            printf("ERROR: executeList recursion failed\n");
            return -1;
          }
          push(&output,&temp);
        }
        else
        {
          if (openBracket)
          {
            for (;opStack.length>0;)
            {
              pop(&opStack,&temp);
              push(&output,&temp);
              keepLooping = 0;
            }
          }
          else
          {
            printf("ERROR: bracket mismatch\n");
            return -1;
          }
        }
      break;
      case TOKEN_ASSIGNMENT:
      case TOKEN_OPERATOR:
        if (opStack.length>0)
        {
          memcpy(&temp,opStack.back,sizeof(struct Token));
          if (higherPrecedence(&temp,&next))
          {
            pop(&opStack,NULL);
            push(&output,&temp);
          }
        }
        push(&opStack,&next);
      break;
      default:
        printf("ERROR: unhandled type %i in executeList\n",next.type);
        return -1;
    }
  }
  for (;opStack.length>0;)
  {
    pop(&opStack,&temp);
    push(&output,&temp);
  }
  if (flushStack(&output,result,pIdList))
  {
    return -1;
  }
  return 0;
}

static int evaluate(char* input, struct Token* result)
{
  struct TokenList tList = {0,NULL};
  static struct IdentifierList iList = {0,NULL};
  if ((input==NULL)&&(result==NULL))
  {
    clearIdList(&iList);
    return 0;
  }
  if (makeTokenList(input,&tList,&iList))
  {
    printf("ERROR: in makeTokenList\n");
    clearTokenList(&tList);
    return -1;
  }
  printTokenList(&tList,&iList);
  if (executeList(&tList, result, &iList, 0))
  {
    clearTokenList(&tList);
    return -1;
  }
  clearTokenList(&tList);
  return 0;
}


int main(int argc, char* argv[])
{
  char input[] = "5+6*7";
  struct Token r;
  if (evaluate(input,&r)==0)
  {
    printf("%li\n",r.uToken.sNumber.uNumber.intValue);
  }
  else
  {
    printf("ERROR: evaluate\n");
  }
  return 0;
}
