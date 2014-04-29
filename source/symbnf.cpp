#include <dpl/symbnf.hpp>

using namespace std;
using namespace dpl;

SymBnf::SymBnf(const string &bnf_string) // {{{
: Bnf(bnf_string)
, myNullable(false)
{
} // }}}

SymBnf::SymBnf(const Bnf &bnf) // {{{
: Bnf(bnf)
, myNullable(false)
{
} // }}}

SymBnf::~SymBnf() // {{{
{
} // }}}

bool SymBnf::AddFirst(const string &token) // {{{
{
  if (myFirst.find(token) == myFirst.end())
  {
    myFirst.insert(token);
    return true;
  }
  return false;
} // }}}

bool SymBnf::AddFirst(const set<string> &source) // {{{
{
  bool updated = false;
  for (set<string>::iterator it = source.begin(); it != source.end(); ++it)
    if (AddFirst(*it))
      updated=true;
  return updated;
} // }}}

bool SymBnf::AddLast(const string &token) // {{{
{
  if (myLast.find(token) == myLast.end())
  {
    myLast.insert(token);
    return true;
  }
  return false;
} // }}}

bool SymBnf::AddLast(const set<string> &source) // {{{
{
  bool updated=false;
  for (set<string>::iterator it = source.begin(); it != source.end(); ++it)
    if (AddLast(*it))
      updated=true;
  return updated;
} // }}}

bool SymBnf::AddFrame(const string &pre, const string &post) // {{{
{ pair<string,string> frame = pair<string,string>(pre,post);
  if (myFrame.find(frame) == myFrame.end())
  {
    myFrame.insert(frame);
    myPre.insert(pre);
    myPost.insert(post);
    return true;
  }
  return false;
} // }}}

void SymBnf::SetNullable(bool value) // {{{
{
  myNullable = value;
} // }}}

void SymBnf::SetVoidRep(const parsetree &rep) // {{{
{
  myVoidRep = rep;
} // }}}

