#if 0
expression      ::= factor | expression operator expression
factor          ::= number | identifier | assignment | '(' expression ')'
assignment      ::= identifier '=' expression

operator        ::= '+' | '-' | '*' | '/' | '%'

identifier      ::= letter | '_' { identifier-char }
identifier-char ::= '_' | letter | digit

number          ::= { digit } [ '.' digit { digit } ]

letter          ::= 'a' | 'b' | ... | 'y' | 'z' | 'A' | 'B' | ... | 'Y' | 'Z'
digit           ::= '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9'
#endif



#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct TokenStruct;
struct IdentifierStruct;
struct OperatorStruct;
struct BracketStruct;
struct NumberStruct;

union NumberUnion
{
  int intValue;
  float floatValue;
};

struct IdentifierStruct
{
  int id;
};

struct OperatorStruct
{
  int opcode;
};

struct BracketStruct
{
  int open;
};

struct NumberStruct
{
  int type;
  union NumberUnion uNumber;
};

union TokenUnion
{
  struct IdentifierStruct sIdentifier;
  struct NumberStruct sNumber;
  struct OperatorStruct sOperator;
  struct BracketStruct sBracket;
};


struct IdentifierList
{
  char* name;
  struct NumberStruct sNumber;
};

static struct IdentifierList* iList = NULL;
static unsigned int iLength = 0;
static int addId(char* newName)
{
  
}

static int findId(char* newName)
{
  int i;
  for (i=0;i<iLength;++i)
  {
    if (strcmp(newName,iList[i].name)==0)
      return i;
  }
  return -1;
}

static void clearList()
{
  int i;
  for (i=0;i<iLength;++i)
  {
    if (iList[i])
    {
      if (iList[i].name)
      {
        free(iList[i].name);
      }
      free(iList[i]);
    }
  }
  iList=NULL;
  iLength=0;
}

#define TOKEN_NUMBER       1
#define TOKEN_IDENTIFIER  (TOKEN_NUMBER+1)
#define TOKEN_ASSIGNMENT  (TOKEN_IDENTIFIER+1)
#define TOKEN_OPERATOR    (TOKEN_ASSIGNMENT+1)
#define TOKEN_BRACKET     (TOKEN_OPERATOR+1)

struct Token
{
  int type;
  union TokenUnion uToken;
};

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

static int parseString(char* input)
{
  int state;
  char c;
  char tokenString[64];
  int tokenIndex;
  for (state=0,tokenIndex=0;;)
  {
    c = input[0];
    switch (state)
    {
      case 0:/* awaiting for first token character*/
        if (isalpha(c))
        {
          tokenString[tokenIndex++]=c;
          tokenString[tokenIndex]=0;
          ++input;
          state=1;/* gathering an identifier */
        }
        else
        {
          if (isdigit(c))
          {
            tokenString[tokenIndex++]=c;
            tokenString[tokenIndex]=0;
            ++input;
            state=2;/*gathering a numeric constant*/
          }
          else
          {
            if (c==0)
              state=4;/* end processing  */
            else
              state=3;/* getting symbols */
          }
        }
      break;
      case 1:/* gathering an identifier */
        if (isIdChar(c))
        {
          tokenString[tokenIndex++]=c;
          tokenString[tokenIndex]=0;
          ++input;
        }
        else
        {
          /* process the new identifier */
          tokenIndex=0;
          if (c==0)
            state=4;/* end processing  */
          else
            state=0;/* let's look for the next token */
        }
      break;
      case 2:/*gathering a numeric constant*/
        if (isNumeric(c))
        {
          tokenString[tokenIndex++]=c;
          tokenString[tokenIndex]=0;
          ++input;
        }
        else
        {
          /* process the new number */
          tokenIndex=0;
          if (c==0)
            state=4;/* end processing  */
          else
            state=0;/* let's look for the next token */
        }
      break;
      case 3:
        switch (c)
        {
          case ' ':
			++input;
		  break;
          case '=':
			++input;
		  break;
          case '+':
			++input;
		  break;
          case '-':
			++input;
		  break;
          case '*':
			++input;
		  break;
          case '/':
			++input;
		  break;
          case '%':
			++input;
		  break;
          case '(':
			++input;
		  break;
          case ')':
			++input;
		  break;
          default:
			++input;
		  break;
        }
      break;
      case 4:
		return 0;
      break;
      default:
		printf("ERROR: unhandled parse state %i\n",state);
		return -1;
      break;
    }
  }
  return -1;
}

int main(int argc, char* argv[])
{
  return 0;
}
