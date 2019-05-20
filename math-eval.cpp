#include <iostream>
#include <string>
#include <exception>
#include <deque>


struct Token
{
  struct OperatorStruct
  {
    uint64_t opcode: 2;
    uint64_t unused: 62;
  };

  struct BracketStruct
  {
    uint64_t open: 1;
    uint64_t unused: 63;
  };

  union Payload
  {
    int64_t intValue;
    double flValue;
    OperatorStruct op;
    BracketStruct brkt;
  };

  typedef enum
  {
    eInvalid,
    eIntNumber,
    eFlNumber,
    eBracket,
    eOperator
  } TokenType;
  //-------------
  TokenType type;
  Payload data;
  //-------------
};


void getNumber(const std::string& str, std::string& remainder, Token& number)
{
  size_t after, length;
  length = str.size();
  if (str.find('.')==std::string::npos)
  {
    try
    {
      number.data.intValue = std::stol(str,&after,10);
      number.type = Token::eIntNumber;
    }
    catch (std::exception& e)
    {
      number.type = Token::eInvalid;
      return;
    }
  }
  else
  {
    try
    {
      number.data.flValue = std::stod(str,&after);
      number.type = Token::eFlNumber;
    }
    catch (std::exception& e)
    {
      number.type = Token::eInvalid;
      return;
    }
  }
  remainder = str.substr(after,length-after);
}

bool isNegative(const Token& n)
{
  if ((n.type == Token::eFlNumber)&&(n.data.flValue < 0.0))
    return true;
  if ((n.type == Token::eIntNumber)&&(n.data.intValue < 0))
    return true;
  return false;
}

void getTokenList(std::string str, std::deque<Token>& tList)
{
  long value;
  std::string tokenStr;
  Token newItem, plus, errToken, lastNumber;
  bool lastWasNumber;
  char nextChar;
  plus.type = Token::eOperator;
  plus.data.op.opcode = 0;
  errToken.type = Token::eInvalid;
  for (lastWasNumber=false;str.size()>0;)
  {
    nextChar = str.front();
    str.erase(0,1);
    switch (nextChar)
    {
      case ' ':
        if (tokenStr.size()>0)
        {
          getNumber(tokenStr,tokenStr,newItem);
          if (newItem.type != Token::eInvalid)
          {
            if (lastWasNumber && isNegative(newItem))
            {
              tList.push_back(plus);
            }
            lastWasNumber = true;
          }
          tokenStr.clear();
          tList.push_back(newItem);
        }
      break;
      case '(':
      case ')':
      case '+':
      case '-':
      case '*':
      case '/':
        if (tokenStr.size()>0)
        {
          getNumber(tokenStr,tokenStr,newItem);
          if (newItem.type != Token::eInvalid)
          {
            if (lastWasNumber && isNegative(newItem))
            {
              tList.push_back(plus);
            }
            lastWasNumber = true;
          }
          tokenStr.clear();
          tList.push_back(newItem);
        }
        switch (nextChar)
        {
          case '(':
            newItem.type = Token::eBracket;
            newItem.data.brkt.open = 1;
          break;
          case ')':
            newItem.type = Token::eBracket;
            newItem.data.brkt.open = 0;
          break;
          case '+':
            newItem.type = Token::eOperator;
            newItem.data.op.opcode = 0;
          break;
          case '-':
            if (isdigit(str.front()))
            {
              tokenStr.push_back(nextChar);
              continue;
            }
            else
            {
              newItem.type = Token::eOperator;
              newItem.data.op.opcode = 1;
            }
          break;
          case '*':
            newItem.type = Token::eOperator;
            newItem.data.op.opcode = 2;
          break;
          case '/':
            newItem.type = Token::eOperator;
            newItem.data.op.opcode = 3;
          break;
          default:
            newItem.type = Token::eInvalid;
            std::cout<<"ERROR 1"<<std::endl;
          break;
        };
        lastWasNumber = false;
        tList.push_back(newItem);
      break;
      default:
      {
        tokenStr.push_back(nextChar);
      }
      break;
    }
  }
  if (tokenStr.size()>0)
  {
    getNumber(tokenStr,tokenStr,newItem);
    if (newItem.type != Token::eInvalid)
    {
      if (lastWasNumber && isNegative(newItem))
      {
        tList.push_back(plus);
      }
    }
    tokenStr.clear();
    tList.push_back(newItem);
  }
}

std::ostream& operator<<(std::ostream& obj, const Token& t)
{
  switch (t.type)
  {
    case Token::eInvalid:   obj<<"INVALID"; break;
    case Token::eIntNumber: obj<<t.data.intValue; break;
    case Token::eFlNumber:  obj<<t.data.flValue; break;
    case Token::eBracket:
    {
      if (t.data.brkt.open)
        obj<<"(";
      else
        obj<<")";
    }
    break;
    case Token::eOperator:
      switch (t.data.op.opcode)
      {
        case 0: obj<<"+"; break;
        case 1: obj<<"_"; break;
        case 2: obj<<"*"; break;
        case 3: obj<<"/"; break;
        default: obj<<"ERROR2"; break;
      }
    break;
    default: obj<<"ERROR3"; break;
  }
  return obj;
}

bool higherPrecedence(const Token& first, const Token& second)
{
  if (second.data.op.opcode == 0)
    return true;
  if (second.data.op.opcode == 1)
    return true;
  if (first.data.op.opcode == 0)
    return false;
  if (first.data.op.opcode == 1)
    return false;
  return true;
}


Token executeOperation(const Token& first, const Token& second, const Token& operation)
{
  Token result;
  if (operation.type != Token::eOperator)
  {
    result.type = Token::eInvalid;
    return result;
  }
  if (first.type == Token::eIntNumber)
  {
    if (second.type == Token::eIntNumber)
    {
      result.type = Token::eIntNumber;
      switch (operation.data.op.opcode)
      {
        case 0: result.data.intValue = first.data.intValue + second.data.intValue; return result;
        case 1: result.data.intValue = first.data.intValue - second.data.intValue; return result;
        case 2: result.data.intValue = first.data.intValue * second.data.intValue; return result;
        case 3: result.data.intValue = first.data.intValue / second.data.intValue; return result;
        default:result.type = Token::eInvalid;return result;
      }
    }
    else
    {
      if (second.type == Token::eFlNumber)
      {
        result.type = Token::eFlNumber;
        switch (operation.data.op.opcode)
        {
          case 0: result.data.flValue = (double)first.data.intValue + second.data.flValue; return result;
          case 1: result.data.flValue = (double)first.data.intValue - second.data.flValue; return result;
          case 2: result.data.flValue = (double)first.data.intValue * second.data.flValue; return result;
          case 3: result.data.flValue = (double)first.data.intValue / second.data.flValue; return result;
          default:result.type = Token::eInvalid;return result;
        }
      }
      else
      {
        result.type = Token::eInvalid;
        return result;
      }
    }
  }
  else
  {
    if (first.type == Token::eFlNumber)
    {
      if (second.type == Token::eIntNumber)
      {
        result.type = Token::eFlNumber;
        switch (operation.data.op.opcode)
        {
          case 0: result.data.flValue = first.data.flValue + (double)second.data.intValue; return result;
          case 1: result.data.flValue = first.data.flValue - (double)second.data.intValue; return result;
          case 2: result.data.flValue = first.data.flValue * (double)second.data.intValue; return result;
          case 3: result.data.flValue = first.data.flValue / (double)second.data.intValue; return result;
          default:result.type = Token::eInvalid;return result;
        }
      }
      else
      {
        if (second.type == Token::eFlNumber)
        {
          result.type = Token::eFlNumber;
          switch (operation.data.op.opcode)
          {
            case 0: result.data.flValue = first.data.flValue + second.data.flValue; return result;
            case 1: result.data.flValue = first.data.flValue - second.data.flValue; return result;
            case 2: result.data.flValue = first.data.flValue * second.data.flValue; return result;
            case 3: result.data.flValue = first.data.flValue / second.data.flValue; return result;
            default:result.type = Token::eInvalid;return result;
          }
        }
        else
        {
          result.type = Token::eInvalid;
          return result;
        }
      }
    }
    else
    {
      result.type = Token::eInvalid;
      return result;
    }
  }
}

Token flushDeque(std::deque<Token>& tList)
{
  Token next, operation, temp, first, second;
  for (;tList.size()>0;)
  {
    temp = tList.back();
    tList.pop_back();
    std::cout<<"Output item: "<<temp<<std::endl<<std::flush;
    if (temp.type == Token::eOperator)
    {
      if (tList.size()<2)
      {
        std::cout<<"Output stack underflow"<<std::endl<<std::flush;
        temp.type == Token::eInvalid;
        return temp;
      }
      second = tList.back();
      tList.pop_back();
      first = tList.back();
      tList.pop_back();
      operation = temp;
      std::cout<<"Preparing: "<<first<<", "<<second<<", "<<operation<<std::endl<<std::flush;
      temp = executeOperation(first, second, operation);
      tList.push_back(temp);
    }
    else
    {
      if (tList.size()>0)
      {
        std::cout<<"ERROR44"<<std::endl;
        temp.type == Token::eInvalid;
        return temp;
      }
      return temp;
    }
  }
  return temp;
}

Token executeList(std::deque<Token>& tList, bool openBracket)
{
  Token next, operation, temp;
  std::deque<Token> output, opStack;
  bool keepLooping;
  for (keepLooping=true;(tList.size()>0) && keepLooping;)
  {
    next = tList.front();
    tList.pop_front();
    std::cout<<"Processing token "<<next<<std::endl<<std::flush;
    switch (next.type)
    {
      case Token::eInvalid:
        return next;
      break;
      case Token::eIntNumber:
      case Token::eFlNumber:
        output.push_back(next);
      break;
      case Token::eBracket:
        if (next.data.brkt.open)
        {
          temp = executeList(tList,true);
          output.push_back(temp);
        }
        else
        {
          if (openBracket)
          {
            for (;opStack.size()>0;)
            {
              temp = opStack.back();
              opStack.pop_back();
              output.push_back(temp);
              keepLooping = false;
            }
          }
          else
          {
            temp.type = Token::eInvalid;
            return temp;
          }
        }
      break;
      case Token::eOperator:
        if (opStack.size()>0)
        {
          temp = opStack.back();
          if (higherPrecedence(temp,next))
          {
            opStack.pop_back();
            output.push_back(temp);
          }
        }
        opStack.push_back(next);
      break;
      default:
        std::cout<<"ERROR"<<std::endl<<std::flush;
        temp.type = Token::eInvalid;
        return temp;
    }
  }
  std::cout<<"Processing output stack"<<std::endl<<std::flush;
  for (;opStack.size()>0;)
  {
    temp = opStack.back();
    opStack.pop_back();
    output.push_back(temp);
  }
  return flushDeque(output);
}

int main(int argc, char* argv[])
{
  Token result;
  std::deque<Token> tList;
  std::deque<Token>::iterator it;
  std::string expr = "-7 * -(6 / 3)";
  getTokenList(expr,tList);
  for (it=tList.begin();it!=tList.end();++it)
  {
    std::cout<<*it;
  }
  std::cout<<std::endl;
  result = executeList(tList,false);
  std::cout<<"Result: "<<result<<std::endl;
  return 0;
}
