#include <dpl/parsetree.hpp>
#include <iostream>
#include <sstream>

using namespace std;
using namespace dpl;

parsetree::parsetree(const token &t) // {{{
  : myToken(t),
    myType(t.Name()),
    myCase("_TOKEN"),
    is_token(true)
{
  myContent.clear();
} // }}}

parsetree::parsetree(const string &type_name, const string &case_name, vector<parsetree*> &content, map<string,size_t> &tags) // {{{
  : myToken(),
    myType(type_name),
    myCase(case_name),
    myContent(content),
    myTags(tags),
    is_token(false)
{
} // }}}

parsetree::parsetree(const parsetree &rhs) // {{{
{ myType = rhs.myType;
  myTags = rhs.myTags;
  myCase = rhs.myCase;
  is_token = rhs.is_token;
  myToken=rhs.myToken;
  myContent.clear();
  for (vector<parsetree*>::const_iterator sub=rhs.myContent.begin(); sub!=rhs.myContent.end(); ++sub)
    myContent.push_back(new parsetree(**sub));
} // }}}

parsetree::~parsetree() // {{{
{ while (myContent.size()>0)
  { delete myContent.back();
    myContent.pop_back();
  }
} // }}}

string parsetree::ToString(bool include_cases) // {{{
{
  if (myType == "_ERROR")
  {
    return myType + ": " + myCase;
  }
  string result = myType;
  if (myCase!="_TOKEN")
    result += "." + myCase;
  if (myCase!="_TOKEN" && myContent.size()>0)
  { result += "( ";
    for (vector<parsetree*>::iterator it=myContent.begin(); it!=myContent.end(); ++it)
    {
      result += (*it)->ToString(include_cases);
      result += " ";
    }
    result += ")";
  }

  return result;
} // }}}

pair<int,int> parsetree::GetPosition() const // {{{
{
  if (is_token)
    return pair<int,int>(myToken.Line(),myToken.Column());
  for (vector<parsetree*>::const_iterator it=myContent.begin(); it!=myContent.end(); ++it)
  { pair<int,int> pos=(*it)->GetPosition();
    if (pos!=pair<int,int>(-1,-1))
      return pos;
  }
  return pair<int,int>(-1,-1);
} // }}}

string parsetree::Position() const // {{{
{ stringstream ss;
  pair<int,int> lc=GetPosition();
  ss << lc.first << ":" << lc.second;
  return ss.str();
} // }}}

const parsetree *parsetree::Child(size_t index) const // {{{
{ if (is_token)
    throw string("Child called on token: " + Type());
  if (index>=myContent.size())
    throw string("Child index out of bounds on : ") + Type() + "." + Case();
  return myContent[index];
} // }}}

const parsetree *parsetree::Child(const string &tag) const // {{{
{ if (is_token)
    throw string("Child called on token: " + Type());
  map<string,size_t>::const_iterator it=myTags.find(tag);
  if (it==myTags.end())
    throw string("Child tag: ") + tag + " not found in: " + Type() + "." + Case();
  return Child(it->second);
} // }}}
