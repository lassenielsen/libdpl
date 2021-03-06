#include <dpl/symparser.hpp>
#include <iostream>

using namespace std;
using namespace dpl;

SymParser::SymParser() // {{{
{
} // }}}

SymParser::SymParser(const string &filename) // {{{
{ LoadFile(filename);
} // }}}

SymParser::~SymParser() // {{{
{
} // }}}

parsetree *SymParser::Parse(const string &buffer) // {{{
{
  // Ensure semantical properties are consistent
  FixAll();

  // Check for conflicts
  // Fixme implement this
  // For each pair of type cases check if they overlap, and if they do check if their types have common elements in both their post set and their pre set.
  // Check if it is terminating

  // Initialize buffer
  SetBuffer(buffer);

  vector<parsetree*> peephole;
  peephole.clear();

  // Do parsing
  while (!Empty())
  {
    // Move token to peephole
    token post_token = PopToken();
    if (post_token.Name() == "_ERROR") // Tokenizer error {{{
    {
      SetBuffer("");
      string msg= "Tokenizer error: ";
      msg += post_token.Content();
      parsetree *result = new parsetree(msg);
      return result;
    } // }}}
    else if (post_token.Name() == "_EOF") // No more tokens {{{
    {
      bool reduced=true;
      while (reduced)
      {
        reduced = false;
        for (map<string,SymBnf>::iterator it_type=myTypes.begin();it_type!=myTypes.end() && !reduced;++it_type)
        {
          vector<string> cases=it_type->second.CaseNames();
          for (vector<string>::iterator case_name=cases.begin(); case_name!=cases.end() && !reduced; ++case_name)
            if (it_type->second.Case(*case_name).size() > 0) // do not use empty cases
            {
              vector<parsetree*> buffer;
              buffer.clear();
              vector<string> cpy_case(it_type->second.Case(*case_name));
              if (make_reduction(peephole, *case_name, cpy_case, it_type->second, buffer, post_token.Name(), false)) // do reduction if possible
                reduced=true;
            }
        }
      }
    } // }}}
    else // default case {{{
    {
      bool reduced=true;
      while (reduced)
      {
        reduced = false;
        for (map<string,SymBnf>::iterator it_type=myTypes.begin();it_type!=myTypes.end() && !reduced;++it_type)
        {
          if (it_type->second.HasPost(post_token.Name())) // Preliminary test
          {
            vector<string> cases=it_type->second.CaseNames();
            for (vector<string>::iterator case_name=cases.begin(); case_name!=cases.end() && !reduced; ++case_name)
            {
              if (it_type->second.Case(*case_name).size() > 0) // do not use empty cases
              {
                vector<parsetree*> buffer;
                buffer.clear();
                vector<string> case_cpy(it_type->second.Case(*case_name));
                if (make_reduction(peephole, *case_name, case_cpy, it_type->second, buffer, post_token.Name(), false)) // do reduction if possible
                  reduced=true;
              }
            }
          }
        }
      }
      // Add new token to peephole
      parsetree *tree = new parsetree(post_token);
      peephole.push_back(tree);
    } // }}}
  }
  SetBuffer("");
  if (peephole.size() != 1)
  {
    string error = "Parser error at: ";
    for (vector<parsetree*>::iterator it = peephole.begin(); it != peephole.end(); ++it)
    {
      error+= " ";
      error+= (*it)->Type();
      if ((*it)->Case() != "_TOKEN")
      { error+= ".";
        error+= (*it)->Case();
      }
      delete *it;
    }
    throw error;
  }

  // If no error, return the found tree
  return peephole.front();
} // }}}

SymBnf &SymParser::GetType(const std::string &name) // {{{
{
  return myTypes[name];
} // }}}

bool SymParser::IsType(const std::string &name) // {{{
{
  map<string,SymBnf>::iterator it=myTypes.find(name);
  return (it != myTypes.end() && !it->second.Empty());
} // }}}

void SymParser::FixNullable() // {{{
{
  bool updated=true;
  while(updated)
  {
    updated=false;
    for (map<string,SymBnf>::iterator it_type=myTypes.begin(); it_type!=myTypes.end(); ++it_type)
    {
      if (it_type->second.Nullable())
        continue;

      vector<string> cases=it_type->second.CaseNames();
      for (vector<string>::iterator case_name=cases.begin(); case_name!=cases.end() && !it_type->second.Nullable(); ++case_name)
      {
        bool nullable_case = true;
        vector<parsetree*> void_seq;
        for (vector<string>::const_iterator it_arg=it_type->second.Case(*case_name).begin(); it_arg!=it_type->second.Case(*case_name).end(); ++it_arg)
        {
          if (!GetType(*it_arg).Nullable())
            nullable_case = false;
          else
            void_seq.push_back(new parsetree(GetType(*it_arg).VoidRep()));
        }
        if (nullable_case)
        {
          map<string,size_t> tags=it_type->second.Tags(*case_name);
          parsetree *void_rep=new parsetree(it_type->second.GetName(),*case_name,void_seq,tags);
          it_type->second.SetVoidRep(void_rep);
          updated = true;
        }
        else
        { // Clean up
          while (void_seq.size()>0)
          { delete void_seq.back();
            void_seq.pop_back();
          }
        }
      }
    }
  }
} // }}}

void SymParser::FixFirst() // {{{
{
  bool updated=true;
  while(updated)
  {
    updated=false;
    for (map<string,SymBnf>::iterator it_type=myTypes.begin(); it_type!=myTypes.end(); ++it_type)
    {
      vector<string> cases=it_type->second.CaseNames();
      for (vector<string>::iterator case_name=cases.begin(); case_name!=cases.end(); ++case_name)
      {
        for (vector<string>::const_iterator it_arg=it_type->second.Case(*case_name).begin(); it_arg!=it_type->second.Case(*case_name).end(); ++it_arg)
        {
          if (IsType(*it_arg))
          {
            if (it_type->second.AddFirst(GetType(*it_arg).First()))
              updated=true;
            if (it_type->second.AddFirst(*it_arg))
              updated=true;
            if (GetType(*it_arg).Nullable()) // If nullable, we can include the next type or token
              continue;
          }
          else
          {
            if (it_type->second.AddFirst(*it_arg))
              updated=true;
          }
          break;
        }
      }
    }
  }
} // }}}

void SymParser::FixLast() // {{{
{
  bool updated=true;
  while(updated)
  {
    updated=false;
    for (map<string,SymBnf>::iterator it_type=myTypes.begin(); it_type!=myTypes.end(); ++it_type)
    {
      vector<string> cases=it_type->second.CaseNames();
      for (vector<string>::iterator case_name=cases.begin(); case_name!=cases.end(); ++case_name)
      {
        for (vector<string>::const_reverse_iterator it_arg=it_type->second.Case(*case_name).rbegin(); it_arg!=it_type->second.Case(*case_name).rend(); ++it_arg)
        {
          if (IsType(*it_arg))
          {
            if (it_type->second.AddLast(GetType(*it_arg).Last()))
              updated=true;
            if (it_type->second.AddLast(*it_arg))
              updated=true;
            if (GetType(*it_arg).Nullable()) // If nullable, we include the previous type or token
              continue;
          }
          else
          {
            if (it_type->second.AddLast(*it_arg))
              updated=true;
          }
          break;
        }
      }
    }
  }
} // }}}

void set_include(const set<string> &source, set<string> &dest) // {{{
{
  for (set<string>::const_iterator it=source.begin(); it!=source.end(); ++it)
    dest.insert(*it);
} // }}}

bool SymParser::FramePostRec(const std::string &pre, SymBnf &t, const std::string &post) // {{{
{ bool updated=false;
  if (pre!="" && post!="" && t.AddFrame(pre,post))
    updated=true;
  else if (pre!="" && post=="" && t.AddPre(pre))
    updated=true;
  else if (pre=="" && post!="" && t.AddPost(post))
    updated=true;
  if (IsType(post) && updated)
  { SymBnf &t_post = GetType(post);
    for (set<string>::const_iterator it_first=t_post.First().begin(); it_first!=t_post.First().end(); ++it_first)
      FramePostRec(pre,t,*it_first);
  }
  return updated;
} // }}}

bool SymParser::FrameAllPosts(const std::string &pre, SymBnf &t, vector<string>::const_iterator post, vector<string>::const_iterator end, const set<string> &follow) // {{{
{
  bool updated=false;
  for (;;++post) // Iterate over posts until break
  {
    if (post==end) // Add posts from follow and break
    { for (set<string>::const_iterator it_follow=follow.begin(); it_follow!=follow.end(); ++it_follow)
        if (FramePostRec(pre,t,*it_follow))
          updated=true;
      if (FramePostRec(pre,t,""))
        updated=true;
      break;
    }
    else if (FramePostRec(pre,t,*post))
      updated=true;
    if (!(IsType(*post) && GetType(*post).Nullable()))
      break;
  }
  return updated;
} // }}}

bool SymParser::FramePreRec(const std::string &pre, SymBnf &t, vector<string>::const_iterator post, vector<string>::const_iterator end, const set<string> &follow) // {{{
{ bool updated=false;
  if (FrameAllPosts(pre,t,post,end,follow))
    updated=true;
  if (IsType(pre) && updated)
  { SymBnf &t_pre = GetType(pre);
    for (set<string>::const_iterator it_last=t_pre.Last().begin(); it_last!=t_pre.Last().end(); ++it_last)
      FramePreRec(*it_last,t,post,end,follow);
  }
  return updated;
} // }}}

bool SymParser::FrameAllPres(vector<string>::const_iterator pre, SymBnf &t, vector<string>::const_iterator post, vector<string>::const_iterator begin, vector<string>::const_iterator end, const set<string> &precede, const set<string> &follow) // {{{
{
  bool updated=false;
  while (true) // Iterate over posts until break
  {
    if (pre==begin) // Add posts from follow and break
    { for (set<string>::const_iterator it_precede=precede.begin(); it_precede!=precede.end(); ++it_precede)
        if (FramePreRec(*it_precede,t,post,end,follow))
          updated=true;
      if (FramePreRec("",t,post,end,follow))
        updated=true;
      break;
    }
    --pre;
    if (FramePreRec(*pre,t,post,end,follow))
      updated=true;
    if (!(IsType(*pre) && GetType(*pre).Nullable()))
      break;
  }
  return updated;
} // }}}

void SymParser::FixFrame() // {{{
{
  bool updated=true;
  while(updated)
  {
    updated=false;
    for (map<string,SymBnf>::iterator it_type=myTypes.begin(); it_type!=myTypes.end(); ++it_type) // Iterate over types
    {
      vector<string> cases=it_type->second.CaseNames();
      for (vector<string>::iterator case_name=cases.begin(); case_name!=cases.end(); ++case_name) // Iterate over cases
      {
        for (vector<string>::const_iterator it_arg=it_type->second.Case(*case_name).begin(); it_arg!=it_type->second.Case(*case_name).end(); ++it_arg) // Iiterate over args in case
        {
          if (IsType(*it_arg))
          {
            SymBnf &t = GetType(*it_arg);
            vector<string>::const_iterator it_pre=it_arg;
//            if (it_pre!=it_type->second.Case(*case_name).begin())
//              --it_pre;
            vector<string>::const_iterator it_post=it_arg;
            ++ it_post;
            if (FrameAllPres(it_pre, t, it_post, it_type->second.Case(*case_name).begin(), it_type->second.Case(*case_name).end(), it_type->second.Pre(), it_type->second.Post()))
              updated=true;
          }
        }
      }
    }
  }
} // }}}

void SymParser::FixAll() // {{{
{
  FixNullable();
  FixFirst();
  FixLast();
  FixFrame();
} // }}}

bool SymParser::make_reduction(vector<parsetree*> &peephole, const string &case_name, vector<string> &case_def, const SymBnf &this_type, vector<parsetree*> &buffer, string post, bool nonempty) // {{{
{
  if (case_def.size() == 0) // just check for Frame or Pre
  {
    if (!nonempty)
      return false;
    bool pre_test=peephole.size() == 0;
    bool post_test=post=="_EOF";
    string pre="_BOF";
    if (!pre_test)
      pre=peephole.back()->Type();

    if ((pre_test && post_test) ||
        (pre_test && !post_test && this_type.HasPost(post)) ||
        (!pre_test && post_test && this_type.HasPre(pre)) ||
        (!pre_test && !post_test && this_type.HasFrame(pre,post)))
    {
      vector<parsetree*> revbuffer(buffer.rbegin(),buffer.rend());
      buffer.clear();
      map<string,size_t> tags=this_type.Tags(case_name);
      parsetree *newtree = new parsetree(this_type.GetName(),case_name,revbuffer,tags);
      peephole.push_back(newtree);
      return true;
    }
    else
      return false;
  }
  //else
  if (peephole.size() == 0)
    return false;
  //else
  string arg = case_def.back();
  case_def.pop_back();
  parsetree *peep = peephole.back();
  peephole.pop_back();
  if (arg == peep->Type()) // Last elements match
  {
    buffer.push_back(peep);
    if (make_reduction(peephole, case_name, case_def, this_type, buffer, post, true))
    {
      buffer.pop_back();
      case_def.push_back(arg);
      return true;
    }
    //else
    buffer.pop_back();
  }
  peephole.push_back(peep);
  if (IsType(arg) && GetType(arg).Nullable())
  {
    buffer.push_back(new parsetree(GetType(arg).VoidRep()));
    if (make_reduction(peephole, case_name, case_def, this_type, buffer, post, nonempty))
    {
      buffer.pop_back();
      case_def.push_back(arg);
      return true;
    }
    delete buffer.back();
    buffer.pop_back();
  }
  case_def.push_back(arg);
  return false;
} // }}}

void SymParser::AddCase(const string &type_name, const string &case_name, const vector<string> &def) // {{{
{ myTypes[type_name].AddCase(case_name,def);
} // }}}

void SymParser::AddType(const string &type_name, const Bnf &t) // {{{
{ map<string,SymBnf>::const_iterator pd=myTypes.find(type_name);
  if (pd!=myTypes.end())
  { if (pd->second.CaseNames()==t.CaseNames())
    { bool identical=true;
      for (vector<string>::const_iterator cn=pd->second.CaseNames().begin(); cn!=pd->second.CaseNames().end(); ++cn)
        if (pd->second.Case(*cn)!=t.Case(*cn))
          identical=false;
      if (identical)
        return;
    }
    throw string("SymParser::AddType: Redefinition of type ") + type_name;
  }
  myTypes.insert(pair<string,SymBnf>(type_name,SymBnf(t)));
} // }}}
