/*------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
/*------------------------------------------------------------------------------------------------------*/
#define TOKEN_INSTRUCTION  1
#define TOKEN_REGISTER     2
#define TOKEN_CONSTANT     3
#define TOKEN_STRING       4

struct Label
{
  char* name;
  int counter;
};

struct Dictionary
{
  int length;
  struct Label* list;
};

/*----------------------------------------------------------------------------*/
static int addLabel(struct Dictionary* pDict, char* labelName, int value)
{
  struct Label* temp;
  char* tempString;
  if (value<0)
  {
    printf("ERROR: negative program counter\n");
    return -1;
  }
  if (pDict==NULL)
  {
    printf("ERROR: null pointer for dictionary\n");
    return -1;
  }
  if (labelName==NULL)
  {
    printf("ERROR: null string for label name\n");
    return -1;
  }
  if (strlen(labelName)==0)
  {
    printf("ERROR: empty string for label name\n");
    return -1;
  }
  if (pDict->list==NULL)
  {
    pDict->length=0;
    temp = (struct Label*)malloc(sizeof(struct Label));
  }
  else
  {
    temp = (struct Label*)realloc(pDict->list,(pDict->length+1)*sizeof(struct Label));
  }
  if (temp==NULL)
  {
    printf("ERROR: allocation (1) in addLabel\n");
    return -1;
  }
  pDict->list = temp;
  tempString = (char*)malloc(sizeof(char)*(1+strlen(labelName)));
  if (tempString==NULL)
  {
	printf("ERROR: allocation 2 in addId\n");
    return -1;
  }
  strcpy(tempString,labelName);
  pDict->list[pDict->length].name = tempString;
  pDict->list[pDict->length].counter = value;
  p->length += 1;
  return 0;
}

/*----------------------------------------------------------------------------*/
static int getProgramCounter(struct Dictionary* pDict, char* labelName)
{
  int i;
  if (pDict==NULL)
  {
    printf("ERROR: null pointer for dictionary\n");
    return -1;
  }
  if (pDict->list==NULL)
  {
    printf("ERROR: null pointer for dictionary\n");
	return -1;
  }
  if (labelName==NULL)
  {
    printf("ERROR: null pointer for label name\n");
    return -1;
  }
  if (strlen(labelName)==0)
  {
    printf("ERROR: empty label name\n");
    return -1;
  }
  for (i=0;i<pDict->length;++i)
  {
    if (strcmp(labelName,pDict->list[i].name)==0)
    {
	  return pDict->list[i].counter;
	}
  }
  return -1;
}

struct Token
{
  int type;
  int value;
  char* name;
};

struct Instruction
{
  int length;
  struct Token* list;
};

static int copyToken(struct Token* dst, struct Token* src)
{
  if (dst==NULL)
	return -1;
  if (src==NULL)
    return -1;
  dst->type = src->type;
  if (src->type!=TOKEN_STRING)
  {
    dst->value = src->value;
    dst->name=NULL;
	return 0;
  }
  dst->value= 0;
  if (src->name==NULL)
    dst->name=NULL;
  dst->name = (char*)malloc((1+strlen(src->name))*sizeof(char));
  if (dst->name==NULL)
    return -1;
  strcpy(dst->name,src->name);
  return 0;
}

static int addToken(struct Instruction* pInstr, struct Token* pToken)
{
  struct pToken* temp;
  if (pToken==NULL)
  {
    printf("ERROR: null token\n");
    return -1;
  }
  if (pInstr==NULL)
  {
    printf("ERROR: null pointer for dictionary\n");
    return -1;
  }
  if (pInstr->list==NULL)
  {
    pInstr->length=0;
    temp = (struct Token*)malloc(sizeof(struct Token));
  }
  else
  {
    temp = (struct Token*)realloc(pInstr->list,(pInstr->length+1)*sizeof(struct Token));
  }
  if (temp==NULL)
  {
    printf("ERROR: allocation (1) in addLabel\n");
    return -1;
  }
  pInstr->list = temp;
  temp = &(pInstr->list[pInstr->length]);
  if (copyToken(temp,pToken))
  {
    printf("ERROR: failed to copy\n");
    return -1;
  }
  p->length += 1;
  return 0;
}

static int getOpcode(char* name)
{
  if (name==NULL)
    return -1;
  if (strcmp(name,"mov")==0)
  {}
  if (strcmp(name,"inc")==0)
  {}
  if (strcmp(name,"mov")==0)
  {}
  if (strcmp(name,"mov")==0)
  {}
  if (strcmp(name,"mov")==0)
  {}
  if (strcmp(name,"mov")==0)
  {}
  if (strcmp(name,"mov")==0)
  {}
}

int parseLine(char* input)
{
  char c;
  int state;
  if (input==NULL)
	return -1;
  for (state=0;;)
  {
    c = input[0];
	switch (state)
	{
	  case 0:
	    if ((c==0)||(c==';'))
		  return 0;
		if (isalpha(c))
		  state=1;
		if ((c=='-')||isdigit(c))
		  state=2;
	  break;
	  case 1:
	  break;
	}
  }
}

