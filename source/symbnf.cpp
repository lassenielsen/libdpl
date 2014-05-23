#include <dpl/symbnf.hpp>

using namespace std;
using namespace dpl;

SymBnf::SymBnf(const string &bnf_string) // {{{
: Bnf(bnf_string)
, myVoidRep(NULL)
{
} // }}}

SymBnf::SymBnf(const Bnf &bnf) // {{{
: Bnf(bnf)
, myVoidRep(NULL)
{
} // }}}

SymBnf::~SymBnf() // {{{
{ if (myVoidRep!=NULL)
    delete myVoidRep;
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

bool SymBnf::AddPre(const string &token) // {{{
{
  if (myPre.find(token) == myPre.end())
  {
    myPre.insert(token);
    return true;
  }
  return false;
} // }}}

bool SymBnf::AddPost(const string &token) // {{{
{
  if (myPost.find(token) == myPost.end())
  {
    myPost.insert(token);
    return true;
  }
  return false;
} // }}}

bool SymBnf::AddFrame(const string &pre, const string &post) // {{{
{ pair<string,string> frame = pair<string,string>(pre,post);
  bool updated=false;
  if (myFrame.find(frame) == myFrame.end())
  {
    myFrame.insert(frame);
    updated=true;
  }
  if (AddPre(pre))
    updated=true;
  if (AddPost(post))
    updated=true;
  return updated;
} // }}}

void SymBnf::SetVoidRep(parsetree *rep) // {{{
{
  myVoidRep = rep;
} // }}}

