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

/*

mov x, y - copy y (either an integer or the value of a register) into register x.
inc x - increase the content of register x by one.
dec x - decrease the content of register x by one.
add x, y - add the content of the register x with y (either an integer or the value of a register) and stores the result in x (i.e. register[x] += y).
sub x, y - subtract y (either an integer or the value of a register) from the register x and stores the result in x (i.e. register[x] -= y).
mul x, y - same with multiply (i.e. register[x] *= y).
div x, y - same with integer division (i.e. register[x] /= y).
label: - define a label position (label = identifier + ":", an identifier being a string that does not match any other command). Jump commands and call are aimed to these labels positions in the program.
jmp lbl - jumps to to the label lbl.
cmp x, y - compares x (either an integer or the value of a register) and y (either an integer or the value of a register). The result is used in the conditional jumps (jne, je, jge, jg, jle and jl)
jne lbl - jump to the label lbl if the values of the previous cmp command were not equal.
je lbl - jump to the label lbl if the values of the previous cmp command were equal.
jge lbl - jump to the label lbl if x was greater or equal than y in the previous cmp command.
jg lbl - jump to the label lbl if x was greater than y in the previous cmp command.
jle lbl - jump to the label lbl if x was less or equal than y in the previous cmp command.
jl lbl - jump to the label lbl if x was less than y in the previous cmp command.
call lbl - call to the subroutine identified by lbl. When a ret is found in a subroutine, the instruction pointer should return to the instruction next to this call command.
ret - when a ret is found in a subroutine, the instruction pointer should return to the instruction that called the current function.
msg 'Register: ', x - this instruction stores the output of the program. It may contain text strings (delimited by single quotes) and registers. The number of arguments isn't limited and will vary, depending on the program.
end - this instruction indicates that the program ends correctly, so the stored output is returned (if the program terminates without this instruction it should return the default output: see below).
; comment - comments should not be taken in consideration during the execution of the program.
*/


/*-------------------------------------------------------------------------------------------------------------*/
struct Label
{
  char* name;
  int counter;
};

/*----------------------------*/
struct Dictionary
{
  int length;
  struct Label* list;
};

/*----------------------------*/
struct Token
{
  int type;
  int value;
  char* name;
};

/*----------------------------*/
struct Instruction
{
  int length;
  struct Token* list;
};

/*----------------------------*/
struct Program
{
  int length;
  struct Instruction** list;
};

/*----------------------------*/
struct Cpu
{
  int registers[26];
  int comparison;
  int counter;
};

/*----------------------------*/
#define STACK_LENGTH 32
struct CallStack
{
  int position;
  int data[STACK_LENGTH];
};

/*-------------------------------------------------------------------------------------------------------------*/
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
    printf("ERROR: null pointer for label name\n");
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
    printf("ERROR: allocation (2) in addLabel\n");
    return -1;
  }
  strcpy(tempString,labelName);
  pDict->list[pDict->length].name = tempString;
  pDict->list[pDict->length].counter = value;
  pDict->length += 1;
  return 0;
}

/*-------------------------------------------------------------------------------------------------------------*/
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
    printf("ERROR: null list in dictionary\n");
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
  printf("ERROR: failed to find label %s\n",labelName);
  return -1;
}

/*-------------------------------------------------------------------------------------------------------------*/
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
  {
    printf("ERROR: allocation in copyToken\n");
    return -1;
  }
  strcpy(dst->name,src->name);
  return 0;
}

/*-------------------------------------------------------------------------------------------------------------*/
static int addToken(struct Instruction* pInstr, struct Token* pToken)
{
  struct Token* temp;
  if (pToken==NULL)
  {
    printf("ERROR: null token in addTokenn");
    return -1;
  }
  if (pInstr==NULL)
  {
    printf("ERROR: null pointer for instruction in addToken\n");
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
    printf("ERROR: allocation (1) in addToken\n");
    return -1;
  }
  pInstr->list = temp;
  temp = &(pInstr->list[pInstr->length]);
  if (copyToken(temp,pToken))
  {
    printf("ERROR: failed to copy in addToken\n");
    return -1;
  }
  pInstr->length += 1;
  return 0;
}

/*-------------------------------------------------------------------------------------------------------------*/
static int addInstruction(struct Program* pProg, struct Instruction* pInstr)
{
  struct Instruction** temp;
  if (pInstr==NULL)
  {
    printf("ERROR: null pointer for instruction in addInstruction\n");
    return -1;
  }
  if (pProg==NULL)
  {
    printf("ERROR: null pointer for program in addInstruction\n");
    return -1;
  }
  if (pProg->list==NULL)
  {
    pProg->length=0;
    temp = (struct Instruction**)malloc(sizeof(struct Instruction*));
  }
  else
  {
    temp = (struct Instruction**)realloc(pProg->list,(pProg->length+1)*sizeof(struct Instruction*));
  }
  if (temp==NULL)
  {
    printf("ERROR: allocation in addInstruction\n");
    return -1;
  }
  pProg->list = temp;
  pProg->list[pProg->length] = pInstr;
  pProg->length += 1;
  return 0;
}

/*-------------------------------------------------------------------------------------------------------------*/
#define  OPCODE_MOV   0
#define  OPCODE_INC   1
#define  OPCODE_DEC   2
#define  OPCODE_ADD   3
#define  OPCODE_SUB   4
#define  OPCODE_MUL   5
#define  OPCODE_DIV   6
#define  OPCODE_JMP   7
#define  OPCODE_CMP   8
#define  OPCODE_JNE   9
#define  OPCODE_JE    10
#define  OPCODE_JGE   11
#define  OPCODE_JG    12
#define  OPCODE_JLE   13
#define  OPCODE_JL    14
#define  OPCODE_CALL  15
#define  OPCODE_RET   16
#define  OPCODE_MSG   17
#define  OPCODE_END   18

static int getOpcode(char* name)
{
  int i;
#define NUM_OF_KEYWORDS  19
  static const char* keywordList[NUM_OF_KEYWORDS] = {"mov", "inc", "dec", "add",
                                                     "sub", "mul", "div", "jmp",
                                                     "cmp", "jne", "je", "jge",
                                                     "jg", "jle", "jl", "call",
                                                     "ret", "msg", "end"};
  if (name==NULL)
  {
    return -1;
  }
  if (strlen(name)==0)
  {
    return -1;
  }
  for (i=0;i<NUM_OF_KEYWORDS;++i)
  {
    if (strcmp(name,keywordList[i])==0)
    {
      return i;
    }
  }
  return -1;
}

/*-------------------------------------------------------------------------------------------------------------*/
static int getRegId(char* name)
{
  if (name==NULL)
    return -1;
  if (strlen(name)!=1)
    return -1;
  if (!isalpha(name[0]))
    return -1;
  return (tolower(name[0]) - 'a');
}

/*-------------------------------------------------------------------------------------------------------------*/
#define  STATE_LINE_AWAITING_FIRST_CHARACTER   0
#define  STATE_LINE_PUTTING_LABEL_TOGETHER     1
#define  STATE_LINE_PUTTING_NUMBER_TOGETHER    2
#define  STATE_LINE_PUTTING_STRING_TOGETHER    3
#define  STATE_LINE_AWAITING_COLON             4
#define  STATE_LINE_AWAITING_COMMA             5
#define  STATE_LINE_ERROR                      6

static struct Instruction* parseLine(char* input, char* labelName)
{
  char c;
  int state, strIndex, id;
  const int strLength = 64;
  char localString[strLength];
  char* end;
  char localLabel[strLength];
  struct Instruction* result=NULL;
  struct Token token;
  if (input==NULL)
  {
    printf("ERROR: null string pointer in parseLine\n");
    return NULL;
  }
  if (strlen(input)==0)
  {
    printf("ERROR: empty string in parseLine\n");
    return NULL;
  }
  result = (struct Instruction*)malloc(sizeof(struct Instruction));
  if (result==NULL)
  {
    printf("ERROR: malloc failed in parseLine\n");
    return NULL;
  }
  result->length=0;
  result->list=NULL;
  localLabel[0]=0;
  for (strIndex=0,state=STATE_LINE_AWAITING_FIRST_CHARACTER;;)
  {
    c = input[0];
    switch (state)
    {
      /*-------------------------------------------------------------------*/
      case STATE_LINE_AWAITING_FIRST_CHARACTER:
        if (c!=' ')
        {
          if ((c==0)||(c==';'))
          {
            if (strlen(localLabel)>0)
            {
              strcpy(labelName,localLabel);
            }
            return result;
          }
          if (isalpha(c))
          {
            localString[strIndex++]=c;
            ++input;
            state=STATE_LINE_PUTTING_LABEL_TOGETHER;
          }
          else
          {
            if ((c=='-')||isdigit(c))
            {
              localString[strIndex++]=c;
              ++input;
              state=STATE_LINE_PUTTING_NUMBER_TOGETHER;
            }
            else
            {
              if (c=='\'')
              {
                state=STATE_LINE_PUTTING_STRING_TOGETHER;
                strIndex=0;
                ++input;
              }
              else
              {
                state=STATE_LINE_ERROR;
              }
            }
          }
        }
      break;
      /*-------------------------------------------------------------------*/
      case STATE_LINE_PUTTING_LABEL_TOGETHER:
        if ((isalpha(c))||(isdigit(c))||(c=='_'))
        {
          if (strIndex>strLength-1)
          {
            printf("ERROR: string exceeds limits (1)\n");
            state=STATE_LINE_ERROR;
          }
          else
          {
            localString[strIndex++]=c;
            ++input;
          }
        }
        else
        {
          localString[strIndex]=0;
          id = getOpcode(localString);
          if (id>=0)
          {
            strIndex=0;
            localString[0]=0;
            token.type = TOKEN_INSTRUCTION;
            token.value = id;
            addToken(result,&token);
            state = STATE_LINE_AWAITING_FIRST_CHARACTER;
          }
          else
          {
            id = getRegId(localString);
            if (id>=0)
            {
              strIndex=0;
              localString[0]=0;
              token.type = TOKEN_REGISTER;
              token.value = id;
              addToken(result,&token);
              state = STATE_LINE_AWAITING_FIRST_CHARACTER;
            }
            else
            {
              /* it's a label */
              state = STATE_LINE_AWAITING_COLON;
            }
          }
        }
      break;
      /*-------------------------------------------------------------------*/
      case STATE_LINE_PUTTING_NUMBER_TOGETHER:
        if (isdigit(c))
        {
          if (strIndex>strLength-1)
          {
            printf("ERROR: string exceeds limits (2)\n");
            state=STATE_LINE_ERROR;
          }
          else
          {
            localString[strIndex++]=c;
            ++input;
          }
        }
        else
        {
          localString[strIndex]=0;
          token.type = TOKEN_CONSTANT;
          token.value = strtol(localString,&end,10);
          if (end[0]==0)
          {
            localString[0]=0;
            strIndex=0;
            addToken(result,&token);
            state = STATE_LINE_AWAITING_FIRST_CHARACTER;
          }
          else
          {
            state = STATE_LINE_ERROR;
          }
        }
      break;
      /*-------------------------------------------------------------------*/
      case STATE_LINE_PUTTING_STRING_TOGETHER:
        if (c=='\'')
        {
          localString[strIndex]=0;
          strIndex=0;
          token.type = TOKEN_STRING;
          token.value = 0;
          token.name = localString;
          addToken(result,&token);
          localString[0]=0;
          state=STATE_LINE_ERROR;
        }
        if (strIndex>strLength-1)
        {
          printf("ERROR: text exceeds limits\n");
          state=STATE_LINE_ERROR;
        }
        else
        {
          localString[strIndex++]=c;
          ++input;
        }
      break;
      /*-------------------------------------------------------------------*/
      case STATE_LINE_AWAITING_COLON:
        if (c!=' ')
        {
          if (c==':')
          {
            strcpy(localLabel,localString);
            strIndex=0;
            localString[0]=0;
            state = STATE_LINE_AWAITING_FIRST_CHARACTER;
          }
          else
          {
            state = STATE_LINE_ERROR;
          }
        }
        else
        {
          ++input;
        }
      break;
      /*-------------------------------------------------------------------*/
      case STATE_LINE_AWAITING_COMMA:
        if (c!=' ')
        {
          if (c==',')
          {
            ++input;
            state = STATE_LINE_AWAITING_FIRST_CHARACTER;
          }
          else
          {
            if ((c==0)||(c==';'))
            {
              return result;
            }
            else
            {
              state = STATE_LINE_ERROR;
            }
          }
        }
        else
        {
          ++input;
        }
      break;
      /*-------------------------------------------------------------------*/
      case STATE_LINE_ERROR:
      break;
      /*-------------------------------------------------------------------*/
      default:
        state = STATE_LINE_ERROR;
      break;
    }
  }
  return NULL;
}

/*-------------------------------------------------------------------------------------------------------------*/
static int parseProgram(const char* input, struct Program* pProg, struct Dictionary* pDict)
{
  struct Instruction* pInstr;
  char instructionLine[80];
  int index;
  char label[64];
  char c;
  if (input==NULL)
  {
    printf("ERROR: null string pointer in parseProgram\n");
    return -1;
  }
  for (index=0;;++input)
  {
    c=input[0];
    switch (c)
    {
      case 0:
      case '\n':
        instructionLine[index]=0;
        index=0;
        label[0]=0;
        pInstr = parseLine(instructionLine,label);
        if (pInstr==NULL)
        {
          return -1;
        }
        addInstruction(pProg,pInstr);
        if (strlen(label)>0)
        {
          addLabel(pDict,label,pProg->length-1);
        }
        if (c==0)
        {
          return 0;
        }
      break;
      default:
        instructionLine[index++]=c;
        ++input;
      break;
    }
  }
  return -1;
}

/*-------------------------------------------------------------------------------------------------------------*/
static int push(struct CallStack* pStack, int value)
{
  if (pStack->position==STACK_LENGTH-1)
  {
    printf("ERROR: stack is full\n");
    return -1;
  }
  pStack->data[pStack->position++]=value;
  return 0;
}

/*-------------------------------------------------------------------------------------------------------------*/
static int pop(struct CallStack* pStack)
{
  if (pStack->position==0)
  {
    printf("ERROR: stack is empty\n");
    return -1;
  }
  return pStack->data[pStack->position--];
}

/*-------------------------------------------------------------------------------------------------------------*/
static int compare(int a, int b)
{
  if (a==b) return 0;
  if (a<b) return 1;
  return -1;
}

/*-------------------------------------------------------------------------------------------------------------*/
static int unaryOperation(int opcode, struct Token* p, struct Cpu* pCpu)
{
  switch (opcode)
  {
    case OPCODE_INC:
      if (p->type==TOKEN_REGISTER)
      {
        pCpu->registers[p->value] += 1;
        return 0;
      }
    break;
    case OPCODE_DEC:
      if (p->type==TOKEN_REGISTER)
      {
        pCpu->registers[p->value] -= 1;
        return 0;
      }
    break;
    default:break;
  }
  return -1;
}

/*-------------------------------------------------------------------------------------------------------------*/
static int binaryOperation(int opcode, struct Token* p1, struct Token* p2, struct Cpu* pCpu)
{
  switch (opcode)
  {
    case OPCODE_MOV:
      if (p1->type==TOKEN_REGISTER)
      {
        if (p2->type==TOKEN_REGISTER)
        {
          pCpu->registers[p1->value] = pCpu->registers[p2->value];
          return 0;
        }
        else
        {
          if (p2->type==TOKEN_CONSTANT)
          {
            pCpu->registers[p1->value] = p2->value;
            return 0;
          }
        }
      }
    break;
    case OPCODE_ADD:
      if (p1->type==TOKEN_REGISTER)
      {
        if (p2->type==TOKEN_REGISTER)
        {
          pCpu->registers[p1->value] += pCpu->registers[p2->value];
          return 0;
        }
        else
        {
          if (p2->type==TOKEN_CONSTANT)
          {
            pCpu->registers[p1->value] += p2->value;
            return 0;
          }
        }
      }
    break;
    case OPCODE_SUB:
      if (p1->type==TOKEN_REGISTER)
      {
        if (p2->type==TOKEN_REGISTER)
        {
          pCpu->registers[p1->value] -= pCpu->registers[p2->value];
          return 0;
        }
        else
        {
          if (p2->type==TOKEN_CONSTANT)
          {
            pCpu->registers[p1->value] -= p2->value;
            return 0;
          }
        }
      }
    break;
    case OPCODE_MUL:
      if (p1->type==TOKEN_REGISTER)
      {
        if (p2->type==TOKEN_REGISTER)
        {
          pCpu->registers[p1->value] *= pCpu->registers[p2->value];
          return 0;
        }
        else
        {
          if (p2->type==TOKEN_CONSTANT)
          {
            pCpu->registers[p1->value] *= p2->value;
            return 0;
          }
        }
      }
    break;
    case OPCODE_DIV:
      if (p1->type==TOKEN_REGISTER)
      {
        if (p2->type==TOKEN_REGISTER)
        {
          pCpu->registers[p1->value] /= pCpu->registers[p2->value];
          return 0;
        }
        else
        {
          if (p2->type==TOKEN_CONSTANT)
          {
            pCpu->registers[p1->value] /= p2->value;
            return 0;
          }
        }
      }
    break;
    case OPCODE_CMP:
      if (p1->type==TOKEN_REGISTER)
      {
        if (p2->type==TOKEN_REGISTER)
        {
          pCpu->comparison = compare(pCpu->registers[p1->value],pCpu->registers[p2->value]);
          return 0;
        }
        else
        {
          if (p2->type==TOKEN_CONSTANT)
          {
            pCpu->comparison = compare(pCpu->registers[p1->value],p2->value);
            return 0;
          }
        }
      }
      else
      {
        if (p1->type==TOKEN_CONSTANT)
        {
          if (p2->type==TOKEN_REGISTER)
          {
            pCpu->comparison = compare(p1->value,pCpu->registers[p2->value]);
            return 0;
          }
          else
          {
            if (p2->type==TOKEN_CONSTANT)
            {
              pCpu->comparison = compare(p1->value,p2->value);
              return 0;
            }
          }
        }
      }
    break;
    default:break;
  }
  return -1;
}

/*-------------------------------------------------------------------------------------------------------------*/
int branchOperation(int opcode, int value, struct Cpu* pCpu, struct CallStack* pCallStack)
{
  switch (opcode)
  {
    case OPCODE_JNE:
      if (pCpu->comparison)
      {
        pCpu->counter = value;
      }
      else
      {
        ++pCpu->counter;
      }
    break;
    case OPCODE_JE:
      if (pCpu->comparison==0)
      {
        pCpu->counter = value;
      }
      else
      {
        ++pCpu->counter;
      }
    break;
    case OPCODE_JGE:
      if (pCpu->comparison<=0)
      {
        pCpu->counter = value;
      }
      else
      {
        ++pCpu->counter;
      }
    break;
    case OPCODE_JG:
      if (pCpu->comparison<0)
      {
        pCpu->counter = value;
      }
      else
      {
        ++pCpu->counter;
      }
    break;
    case OPCODE_JLE:
      if (pCpu->comparison>=0)
      {
        pCpu->counter = value;
      }
      else
      {
        ++pCpu->counter;
      }
    break;
    case OPCODE_JL:
      if (pCpu->comparison>0)
      {
        pCpu->counter = value;
      }
      else
      {
        ++pCpu->counter;
      }
    break;
    case OPCODE_JMP:
      pCpu->counter = value;
    break;
    case OPCODE_CALL:
      push(pCallStack,pCpu->counter+1);
      pCpu->counter = value;
    break;
    default:break;
  }
  return -1;
}

/*-------------------------------------------------------------------------------------------------------------*/
#define STATE_INSTR_AWAITING_OPCODE      0
#define STATE_INSTR_AWAITING_ARG1        1
#define STATE_INSTR_AWAITING_ARG2        2
#define STATE_INSTR_AWAITING_ARG1_ONLY   3
#define STATE_INSTR_AWAITING_LABEL       4
#define STATE_INSTR_AWAITING_TO_PRINT    5
#define STATE_INSTR_ERROR                6
#define STATE_INSTR_DONE                 7

static int executeInstruction(struct Instruction* pInstr, struct Dictionary* pDict, struct Cpu* pCpu, struct CallStack* pCallStack)
{
  int i;
  struct Token arg1, arg2;
  int opcode;
  int state;
  int value;
  for (state=STATE_INSTR_AWAITING_OPCODE,i=0;i;++i)
  {
    switch (state)
    {
      /*------------------------------------------------------------*/
      case STATE_INSTR_AWAITING_OPCODE:
        if (pInstr->list[i].type==TOKEN_INSTRUCTION)
        {
          opcode = pInstr->list[i].value;
          switch (opcode)
          {
            /*-------------------------------------------------*/
            case OPCODE_MOV:
            case OPCODE_ADD:
            case OPCODE_SUB:
            case OPCODE_MUL:
            case OPCODE_DIV:
            case OPCODE_CMP:
              if (pInstr->length==3)
                state = STATE_INSTR_AWAITING_ARG1;
              else
                state = STATE_INSTR_ERROR;
            break;
            /*-------------------------------------------------*/
            case OPCODE_INC:
            case OPCODE_DEC:
              if (pInstr->length==2)
                state = STATE_INSTR_AWAITING_ARG1_ONLY;
              else
                state = STATE_INSTR_ERROR;
            break;
            /*-------------------------------------------------*/
            case OPCODE_JNE:
            case OPCODE_JE:
            case OPCODE_JGE:
            case OPCODE_JG:
            case OPCODE_JLE:
            case OPCODE_JL:
            case OPCODE_JMP:
            case OPCODE_CALL:
              if (pInstr->length==2)
                state = STATE_INSTR_AWAITING_LABEL;
              else
                state = STATE_INSTR_ERROR;
            break;
            /*-------------------------------------------------*/
            case OPCODE_RET:
                if (pInstr->length==1)
                {
                  value = pop(pCallStack);
                  if (value==-1)
                  {
                    state = STATE_INSTR_ERROR;
                  }
                  else
                  {
                    pCpu->counter = value;
                    state = STATE_INSTR_DONE;
                  }
                }
                else
                  state = STATE_INSTR_ERROR;
            break;
            /*-------------------------------------------------*/
            case OPCODE_MSG:
              state = STATE_INSTR_AWAITING_TO_PRINT;
            break;
            /*-------------------------------------------------*/
            case OPCODE_END:
              if (pInstr->length==1)
                state = STATE_INSTR_DONE;
              else
                state = STATE_INSTR_ERROR;
            break;
            /*-------------------------------------------------*/
            default:
            break;
          }
        }
        else
        {
          state = STATE_INSTR_ERROR;
        }
      break;
      /*------------------------------------------------------------*/
      case STATE_INSTR_AWAITING_ARG1_ONLY:
        arg1 = pInstr->list[i];
        if (unaryOperation(opcode,&arg1,pCpu))
          state = STATE_INSTR_ERROR;
        else
          state = STATE_INSTR_DONE;
      break;
      case STATE_INSTR_AWAITING_ARG1:
        arg1 = pInstr->list[i];
        state = STATE_INSTR_AWAITING_ARG2;
      break;
      case STATE_INSTR_AWAITING_ARG2:
        arg2 = pInstr->list[i];
        if (binaryOperation(opcode,&arg1,&arg2,pCpu))
          state = STATE_INSTR_ERROR;
        else
          state = STATE_INSTR_DONE;
      break;
      case STATE_INSTR_AWAITING_LABEL:
        if (pInstr->list[i].type == TOKEN_STRING)
        {
          value = getProgramCounter(pDict,pInstr->list[i].name);
          if (value>=0)
          {
            if (branchOperation(opcode,value,pCpu,pCallStack))
              state = STATE_INSTR_ERROR;
            else
              state = STATE_INSTR_DONE;
          }
          else
            state = STATE_INSTR_ERROR;
        }
        else
          state = STATE_INSTR_ERROR;
      break;
      case STATE_INSTR_AWAITING_TO_PRINT:
        if (pInstr->list[i].type==TOKEN_STRING)
        {
          printf("%s",pInstr->list[i].name);
        }
        else
        {
          if (pInstr->list[i].type==TOKEN_REGISTER)
          {
            printf("%i",pInstr->list[i].value);
          }
          else
          {
            
          }
        }
      break;
      /*------------------------------------------------------------*/
      case STATE_INSTR_ERROR:
        return -1;
      break;
      /*------------------------------------------------------------*/
      case STATE_INSTR_DONE:
        printf("ERROR: no more arguments expected\n");
        return -1;
      break;
      /*------------------------------------------------------------*/
      default:
        return -1;
      break;
    }
  }
  return -1;
}

/*-------------------------------------------------------------------------------------------------------------*/
static void executeProgram(struct Program* pProg, struct Dictionary* pDict)
{
  struct Cpu cpu = {{0},0,0};
  struct CallStack callStack = {0,{0}};
  struct Instruction* pInstr;
  for (;;)
  {
    pInstr = pProg->list[cpu.counter];
    if (pInstr==NULL)
    {
      return;
    }
    if (executeInstruction(pInstr,pDict,&cpu,&callStack))
    {
      return;
    }
  }
}

/*-------------------------------------------------------------------------------------------------------------*/
int assembler(const char* input)
{
  struct Program program = {0,NULL};
  struct Dictionary dictionary = {0,NULL};
  if (parseProgram(input,&program,&dictionary))
  {
    return -1;
  }
  executeProgram(&program,&dictionary);
  return 0;
}

/*-------------------------------------------------------------------------------------------------------------*/
int main(int argc, char* argv[])
{
  return 0;
}

