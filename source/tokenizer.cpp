#include <dpl/tokenizer.hpp>
#include <iostream>
#include <typeinfo>
#include <rcp/frca.hpp>

using namespace std;
using namespace dpl;
//using namespace pcrepp;

// token methods
token::token(string name, string content, string pre_content) // {{{
: name(name),
  content(content),
  pre_content(pre_content)
{
} // }}}

// Tokenizer methods
Tokenizer::Tokenizer(string mode) // {{{
: myPos(0),
  myMode(mode)
{
  myTokenDefs.clear();
  myStar=NULL;
  myDirty=false;
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
  // Add the new def
  unsigned int index = 0;
  while (index < myTokenDefs.size() && myTokenDefs[index].priority < priority)
    index++;
  myTokenDefs.insert(myTokenDefs.begin()+=index, def);
  myDirty=true;
} // }}}

bool Tokenizer::DefToken(string def, int priority) // {{{
{
  string name="";
  string exp="";
  while (def.size()>0 && (def[0]==' ' || def[0]=='\t' || def[0]=='\r' || def[0]=='\n'))
    def=def.substr(1);
  while (def.size()>0 && def[0]!=' ' && def[0]!='\t' && def[0]!='\r' && def[0]!='\n' && def.substr(0,2)!=":=")
  {
    name+=def[0];
    def=def.substr(1);
  }
  while (def.size()>0 && (def[0]==' ' || def[0]=='\t' || def[0]=='\r' || def[0]=='\n'))
    def=def.substr(1);
  if (def.substr(0,2) != ":=") // Not correct format
    return false;
  else
    def=def.substr(2);
  while (def.size()>0 && (def[0]==' ' || def[0]=='\t' || def[0]=='\r' || def[0]=='\n'))
    def=def.substr(1);
  if (def[0] != '"') // Not correct format
    return false;
  else
    def=def.substr(1);
  while (def.size()>0 && def[0]!='"')
  {
    if (def.substr(0,2) == "\\\"")
    {
      exp += "\"";
      def = def.substr(2);
    }
    else
    {
      exp+=def[0];
      def=def.substr(1);
    }
  }
  if (def[0] != '"') // Not correct format
    return false;
  
  DefToken(name,exp,priority);
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
//  delete frca;
  myPos=0;
}; // }}}

token Tokenizer::PopToken() // {{{
{
  if (myStar==NULL)
    return token("_ERROR","RE uninitialized","");
  string token_pre="";
  string token_name="";
  string token_content="";
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
      RV *token_value=myStar->GetSub().Decompress(myCompressed,myPos);
      int tid=tokenid(myTokenDefs,*token_value); // Find index of tokendef
      token_name=myTokenDefs[tid].name;
      token_content=token_value->Flatten();
      delete token_value;
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
  return token(token_name,token_content,token_pre);
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
    result += source[index].pre_content;
    result += source[index].content;
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
