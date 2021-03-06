#include <dpl/tokenizer.hpp>
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <rcp/frca.hpp>

using namespace std;
using namespace dpl;
//using namespace pcrepp;

// token methods
token::token(string name, string content, string pre_content, int line, int column) // {{{
: myName(name),
  myContent(content),
  myPrelude(pre_content),
  myLine(line),
  myColumn(column)
{
} // }}}

string token::Position() const // {{{
{ stringstream ss;
  ss << Line() << ":" << Column();
  return ss.str();
} // }}}

// Tokenizer methods
Tokenizer::Tokenizer(string mode) // {{{
: myPos(0),
  myMode(mode),
  myCurrentLine(1),
  myCurrentColumn(1)
{
  myTokenDefs.clear();
  myStar=NULL;
  myDirty=false;
} // }}}

Tokenizer::Tokenizer(const Tokenizer &init) // {{{
: myPos(init.myPos)
, myMode(init.myMode)
, myCurrentLine(init.myCurrentLine)
, myCurrentColumn(init.myCurrentColumn)
, myTokenDefs(init.myTokenDefs)
{
  myStar=NULL;
  myDirty=true;
} // }}}

Tokenizer::~Tokenizer() // {{{
{ if (myStar!=NULL)
    delete myStar;
  myStar = NULL;
} // }}}

void string_replace( string &source, const string &find, const string &replace ) // {{{
{
  size_t j=0;
  while ((j = source.find( find, j )) != string::npos)
  {
    source.replace( j, find.length(), replace );
    j+=replace.size();
  }
} // }}}

void string_escape( string &source, const string &pattern ) // {{{
{
  string_replace(source, pattern, (string)"\\" + pattern);
} // }}}

string keyword_escape(string reg) // {{{
{
  // Escape: \[()+*01 chars
  string_escape(reg,"\\");
  string_escape(reg,"|");
  string_escape(reg,"[");
  string_escape(reg,"(");
  string_escape(reg,")");
  string_escape(reg,"{");
  string_escape(reg,"}");
  string_escape(reg,"+");
  string_escape(reg,"*");
  string_escape(reg,"0");
  string_escape(reg,"1");
  return reg;
} // }}}

string choice_exp(string chars) // {{{
{
  string result = (string)"";
  for (int i = 0; i<chars.size(); ++i)
  { if (i>0)
      result += "+";
    string ch = "";
    ch += chars[i];
    result += keyword_escape(ch);
  }
  return result;
} // }}}

void Tokenizer::DefToken(const string &name, const string &exp, int priority) // {{{
{
  tokendef def;
  def.name = name;
  def.definition = exp;
  def.priority = priority;
  // Look for prior definition
  for (vector<tokendef>::const_iterator it=myTokenDefs.begin(); it!=myTokenDefs.end(); ++it)
    if (it->name==name)
      if (it->definition==exp && it->priority==priority)
        return;
      else
        throw string("Tokenizer::DefToken: Redefinition of token ") + name;
  // Add the new def
  unsigned int index = 0;
  while (index < myTokenDefs.size() && myTokenDefs[index].priority < priority)
    index++;
  myTokenDefs.insert(myTokenDefs.begin()+=index, def);
  myDirty=true;
} // }}}

bool Tokenizer::DefToken(const string &def, int priority) // {{{
{ string name="";
  string exp="";
  size_t pos=0;
  while (def.size()-pos>0 && (def[pos]==' ' || def[pos]=='\t' || def[pos]=='\r' || def[pos]=='\n'))
    ++pos;
  while (def.size()-pos>0 && def[pos]!=' ' && def[pos]!='\t' && def[pos]!='\r' && def[pos]!='\n' && (def.size()-pos<2 || def.substr(pos,2)!=":="))
  { name+=def[pos];
    ++pos;
  }
  while (def.size()-pos>0 && (def[pos]==' ' || def[pos]=='\t' || def[pos]=='\r' || def[pos]=='\n'))
    ++pos;
  if (def.size()-pos<2 || def.substr(pos,2) != ":=") // Not correct format
    return false;
  else
    pos+=2;
  if (def[pos]=='_') // Extract priority
  { if (priority>=0)
      return false;
    string prio;
    ++pos;
    while (def.size()-pos>0 && def[pos]>='0' && def[pos]<='9')
    { prio += def[pos];
      ++pos;
    }
    if (prio=="")
      return false;
    priority = stoi(prio);
  }
    
  while (def.size()-pos>0 && (def[pos]==' ' || def[pos]=='\t' || def[pos]=='\r' || def[pos]=='\n'))
    ++pos;
  if (def[pos] != '"') // Not correct format
    return false;
  else
    ++pos;
  while (def.size()-pos>0 && def[pos]!='"')
  {
    if (def[pos] == '\\' && def.length()-pos>=2)
      exp += def[++pos];
    else
      exp+=def[pos];
    ++pos;
  }
  if (def.size()-pos>0 && def[pos] != '"') // Not correct format
    return false;

  if (priority<0)
    priority=0;
  
  DefToken(name,exp,priority);
  def.substr(pos);
  return true;
} // }}}

void Tokenizer::DefKeywordToken(const string &keyword, int priority) // {{{
{
  DefToken(keyword,keyword_escape(keyword),priority);
} // }}}

void Tokenizer::DefGeneralToken(const string &name, const string &chars, int priority) // {{{
{
  // Make expression
  string exp = (string)"(" + choice_exp(chars) + ")(" + choice_exp(chars) + ")*";
  DefToken(name,exp,priority);
} // }}}

void Tokenizer::DefSpecialToken(const string &name, const string &initchars, const string &chars, int priority) // {{{
{
  // Make expression
  string exp = (string)"(" + choice_exp(initchars) + ")(" + choice_exp(chars) + ")*";
  DefToken(name,exp,priority);
} // }}}

RE *build_fullre(vector<tokendef> &tokens, int start=0, int end=-1) // {{{
{ if (end <0)
    end = tokens.size()-1;
  if (end<start) // no tokens
    return new RE(); // return 0
  else if (start==end)
    return RE::Create(tokens[start].definition);
  else // start < end
  { int mid=(start+end)/2;
    RE *lhs = build_fullre(tokens,start,mid);
    RE *rhs = build_fullre(tokens,mid+1,end);
    return new RE_Sum(lhs,rhs);
  }
} // }}}

int tokenid(vector<tokendef> &tokens, const RV &value, int start=0, int end=-1) // {{{
{ if (end<0)
    end = tokens.size()-1;
  if (end<start) // no tokens
    throw "tokenid: no tokendefs";
  else if (start==end)
    return start;
  else if (typeid(value)==typeid(RV_Inl)) //  go left
    return tokenid(tokens, *((const RV_Inl&)value).GetLeft(), start, (start+end)/2);
  else if (typeid(value)==typeid(RV_Inr)) // go right
    return tokenid(tokens, *((const RV_Inr&)value).GetRight(), (start+end)/2+1,end);
  else
    throw "tokenid: Non inl/inr value";
} // }}}

void decode_token(vector<tokendef> &tokens, const BitCode &code, int &pos, const RE *exp, string &dest_name, string &dest_content, int start=0, int end=-1) // {{{
{ if (end<0)
    end = tokens.size()-1;
  if (end<start) // no tokens
    throw "tokenid: no tokendefs";
  for (;;)
  { if (start==end)
      break;
    else if (code.GetBit(pos++)==BitCode::INL) //  go left
    { end=(start+end)/2;
      exp=&((const RE_Sum*)exp)->GetLeft();
      continue;
    }
    else // go right
    { start=(start+end)/2+1;
      exp=&((const RE_Sum*)exp)->GetRight();
      continue;
    }
  }
  // Store result
  stringstream content;
  exp->Decode(code,pos,content);
  dest_content=content.str();
  dest_name=tokens[start].name;
  //cout << "decode_token returns: [" << dest_name << "," << dest_content << ", " << start << ", " << end << "]" << endl;
  return;
} // }}}

void Tokenizer::SetBuffer(const string &buffer) // {{{
{ // Compress buffer using current tokens
  if (myDirty && myStar!=NULL)
  { delete myStar;
    myStar=NULL;
  }
  if (myStar==NULL)
  { myStar = new RE_Star(build_fullre(myTokenDefs));
    myDirty=false;
  }

//  NFA nfa(*myStar);
//  BCOrder_LL order(*myStar);
//  try
//  { myCompressed=nfa.Thompson(buffer,order);
//  }
//  catch (string s)
//  {
//    cout << "Error: " << s << endl;
//  }

  FRCA *frca = FRCA::Create(myStar);
//  if (myMode=="ll")
//    myCompressed = frca->CompressLL(buffer);
//  else
  myCompressed = frca->CompressGL(buffer);
  delete frca;
  myPos=0;
}; // }}}

token Tokenizer::PopToken() // {{{
{
  if (myStar==NULL)
    return token("_ERROR","RE uninitialized","");
  string token_pre="";
  string token_name="";
  string token_content="";
  int token_line=0;
  int token_column=0;
  while (token_name=="")
  { // while comment tokens, move content to pre_content and find next token
    token_pre+=token_content;
    token_content="";
    // Find next token
    if (myCompressed.GetLength()<=myPos) // no value
    { token_name="_ERROR";
      token_content="EOF";
    }
    else if (myCompressed.GetBit(myPos)==BitCode::NIL) // end of tokens
    { token_name="_EOF";
      token_content="";
      myCompressed=BitCode();
    }
    else if (myCompressed.GetBit(myPos)==BitCode::CONS) // contains token
    { ++myPos; // myCompressed=myCompressed.substr(1);
      // use myPos int pos=0;
      //RV *token_value=myStar->GetSub().Decompress(myCompressed,myPos);
      //int tid=tokenid(myTokenDefs,*token_value); // Find index of tokendef
      //token_name=myTokenDefs[tid].name;
      //token_content=token_value->Flatten();
      decode_token(myTokenDefs,myCompressed,myPos,&myStar->GetSub(),token_name,token_content);
      // Update position
      token_line=myCurrentLine;
      token_column=myCurrentColumn;
      for (string::const_iterator p=token_content.begin(); p!=token_content.end(); ++p)
      { if (*p=='\n')
        { myCurrentColumn=1;
          ++myCurrentLine;
        }
        else
          ++myCurrentColumn;
      }
      //delete token_value;
      // myPos incremented by Decompress
    }
    else
    { token_name="_ERROR";
      token_content=(string)"Bad compression: " + myCompressed.ToString().substr(myPos);
    }
  }
  if (myPos>myCompressed.GetLength())
  { myPos = 0;
    myCompressed=BitCode();
  }
//  else if (myPos >= 4096 || myCompressed.GetLength()<=myPos) // remove chunk from buffer
//  { myCompressed=myCompressed.substr(myPos);
//    myPos=0;
//  }
  // Return found token
  return token(token_name,token_content,token_pre,token_line,token_column);
}; // }}}

void Tokenizer::Tokenize(vector<token> &dest) // {{{
{
  while (!Empty())
  {
    token t = PopToken();
    dest.push_back(t);
  }
} // }}}

string Tokenizer::Serialize(const vector<token> &source) // {{{
{
  string result="";
  for (int index=0; index<source.size(); ++index)
  {
    result += source[index].Prelude();
    result += source[index].Content();
  }
  return result;
} // }}}

bool Tokenizer::Empty() // {{{
{
  return myCompressed.GetLength()<=myPos;
} // }}}

void Tokenizer::Clear() // {{{
{ if (myStar!=NULL)
    delete myStar;
  myStar = NULL;
  myTokenDefs.clear();
  myCompressed=BitCode();
  myPos=0;
  myDirty=false;
} // }}}
