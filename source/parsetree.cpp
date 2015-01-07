#include <dpl/parsetree.hpp>
#include <iostream>

using namespace std;
using namespace dpl;

parsetree::parsetree(const token &t) // {{{
  : root(t),
    type_name(t.name),
    case_name("_TOKEN"),
    is_token(true)
{
  content.clear();
} // }}}

parsetree::parsetree(const string &type_name, const string &case_name, vector<parsetree*> &content) // {{{
  : root(),
    type_name(type_name),
    case_name(case_name),
    content(content),
    is_token(false)
{
} // }}}

parsetree::parsetree(const string &error) // {{{
  : type_name("_ERROR"),
    case_name(error),
    is_token(false)
{
  content.clear();
} // }}}

parsetree::parsetree(const parsetree &rhs) // {{{
{ type_name = rhs.type_name;
  case_name = rhs.case_name;
  is_token = rhs.is_token;
  root=rhs.root;
  content.clear();
  for (vector<parsetree*>::const_iterator sub=rhs.content.begin(); sub!=rhs.content.end(); ++sub)
    content.push_back(new parsetree(**sub));
} // }}}

parsetree::~parsetree() // {{{
{ while (content.size()>0)
  { delete content.back();
    content.pop_back();
  }
} // }}}

string parsetree::ToString(bool include_cases) // {{{
{
  if (type_name == "_ERROR")
  {
    return type_name + ": " + case_name;
  }
  string result = type_name;
  if (case_name!="_TOKEN")
    result += "." + case_name;
  if (case_name!="_TOKEN" && content.size()>0)
  { result += "( ";
    for (vector<parsetree*>::iterator it=content.begin(); it!=content.end(); ++it)
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
    return pair<int,int>(root.line,root.column);
  for (vector<parsetree*>::const_iterator it=content.begin(); it!=content.end(); ++it)
  { pair<int,int> pos=(*it)->GetPosition();
    if (pos!=pair<int,int>(-1,-1))
      return pos;
  }
  return pair<int,int>(-1,-1);
} // }}}

