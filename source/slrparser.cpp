#include <dpl/slrparser.hpp>
#include <iostream>

using namespace std;
using namespace dpl;

SlrParser::SlrParser() // {{{
{
} // }}}

SlrParser::SlrParser(const string &filename) // {{{
{ LoadFile(filename);
} // }}}

SlrParser::~SlrParser() // {{{
{
} // }}}

action SlrParser::FindAction(const std::set<node> &state, const std::string &symbol) // {{{
{
  set<node> shift_dest;
  string reduce_type;
  string reduce_case;

  for (set<node>::const_iterator n=state->begin(); n!=state->end(); ++n)
  { map<string,SymBnf>::const_iterator n_bnf=myTypes.find(n->t);
    if (n_bnf!=myTypes.end()) {
      vector<string> &n_case=n_bnf->Case(n->c);
      if (n->p<n_case.size() && n_case[n->p]==post_token) {
        node s=n;
        ++s.p;
        edge_dest.insert(s);
      }
      else if (n->p==n_case.size() && n_bnf->Post.find(post_token.name)!=n_bnf->Post().end()) { // Reduce

      }
    }
  }
} // }}}

parsetree *SlrParser::Parse(const string &buffer) // {{{
{
  // Initialize buffer
  SetBuffer(buffer);

  // Create initial state
  { set<string> init_state;
    SymBnf &init=GetType(myInit);
    vector<string> init_cases=init.CaseNames();
    for (int i=0; i<init_cases.size(); ++i)
    { node n;
      n.t=myInit;
      n.c=init_cases[i];
      n.p=0;
      stack.instert(n);
    }
    EpsilonClosure(init_state);
    states.push_back(init_state);
  }


  /* Build states dynamically
  // Build reachable states, and transitions
  size_t processed=0;
  while (processed<states.size())
  { map<string,set<state>> steps;
    FindSteps(states[processed],steps);
    for (map<string,set<state> >::const_iterator step=steps.begin(); step!=steps.end(); ++step)
    { EpsilonClosure(step->second);
      vector<set<state> >::const_iterator target=states.find(step->second);
      if (target==states.end())
      { states.push_back(step->second);
        transitions[pair<int,string>(processed,step->first)]=states.size()-1;
      }
      else
      { transitions[pair<int,string>(processed,step->first)]=target-states.begin();
      }
    }
    ++processed;
  }
  */

  // Perform parsing
  vector<int> stack;
  stack.push_back(0);
  vector<parsetree*> peephole;
  
  // Do parsing
  while (!Empty())
  {
    // Move token to peephole
    token post_token = PopToken();
    if (post_token.name == "_ERROR") // Tokenizer error {{{
    {
      SetBuffer("");
      string msg= "Tokenizer error: ";
      msg += post_token.content;
      throw(msg);
    } // }}}
    pair<int,action>> edge=FindAction(stack.back(),post_token.name);
    if (edge.second.sr=="SHIFT")
    {
      parsetree *tree = new parsetree(post_token);
      peephole.push_back(tree);
      stack.push_back(edge.first);
    }
    if (edge.second.sr=="REDUCE")
    { vector<string> def=GetType(edge.second.t).Case(edge.second.c);

    }
    else if (post_token.name == "_EOF") // No more tokens {{{
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
              if (make_reduction(peephole, *case_name, cpy_case, it_type->second, buffer, post_token.name, false)) // do reduction if possible
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
          if (it_type->second.HasPost(post_token.name)) // Preliminary test
          {
            vector<string> cases=it_type->second.CaseNames();
            for (vector<string>::iterator case_name=cases.begin(); case_name!=cases.end() && !reduced; ++case_name)
            {
              if (it_type->second.Case(*case_name).size() > 0) // do not use empty cases
              {
                vector<parsetree*> buffer;
                buffer.clear();
                vector<string> case_cpy(it_type->second.Case(*case_name));
                if (make_reduction(peephole, *case_name, case_cpy, it_type->second, buffer, post_token.name, false)) // do reduction if possible
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
      error+= (*it)->type_name;
      if ((*it)->case_name != "_TOKEN")
      { error+= ".";
        error+= (*it)->case_name;
      }
      delete *it;
    }
    throw error;
  }

  // If no error, return the found tree
  return peephole.front();
} // }}}

SymBnf &SlrParser::GetType(const std::string &name) // {{{
{
  return myTypes[name];
} // }}}

bool SlrParser::IsType(const std::string &name) // {{{
{
  map<string,SymBnf>::iterator it=myTypes.find(name);
  return (it != myTypes.end() && !it->second.Empty());
} // }}}

void SlrParser::FixNullable() // {{{
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
          parsetree *void_rep=new parsetree(it_type->second.GetName(),*case_name,void_seq);
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

void SlrParser::FixFirst() // {{{
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

void SlrParser::FixLast() // {{{
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

bool SlrParser::FramePostRec(const std::string &pre, SymBnf &t, const std::string &post) // {{{
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

bool SlrParser::FrameAllPosts(const std::string &pre, SymBnf &t, vector<string>::const_iterator post, vector<string>::const_iterator end, const set<string> &follow) // {{{
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

bool SlrParser::FramePreRec(const std::string &pre, SymBnf &t, vector<string>::const_iterator post, vector<string>::const_iterator end, const set<string> &follow) // {{{
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

bool SlrParser::FrameAllPres(vector<string>::const_iterator pre, SymBnf &t, vector<string>::const_iterator post, vector<string>::const_iterator begin, vector<string>::const_iterator end, const set<string> &precede, const set<string> &follow) // {{{
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

void SlrParser::FixFrame() // {{{
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

void SlrParser::FixAll() // {{{
{
  FixNullable();
  FixFirst();
  FixLast();
  FixFrame();
} // }}}

bool SlrParser::make_reduction(vector<parsetree*> &peephole, const string &case_name, vector<string> &case_def, const SymBnf &this_type, vector<parsetree*> &buffer, string post, bool nonempty) // {{{
{
  if (case_def.size() == 0) // just check for Frame or Pre
  {
    if (!nonempty)
      return false;
    bool pre_test=peephole.size() == 0;
    bool post_test=post=="_EOF";
    string pre="_BOF";
    if (!pre_test)
      pre=peephole.back()->type_name;

    if ((pre_test && post_test) ||
        (pre_test && !post_test && this_type.HasPost(post)) ||
        (!pre_test && post_test && this_type.HasPre(pre)) ||
        (!pre_test && !post_test && this_type.HasFrame(pre,post)))
    {
      vector<parsetree*> revbuffer(buffer.rbegin(),buffer.rend());
      buffer.clear();
      parsetree *newtree = new parsetree(this_type.GetName(),case_name,revbuffer);
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
  if (arg == peep->type_name) // Last elements match
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

void SlrParser::AddCase(const string &type_name, const string &case_name, const vector<string> &def) // {{{
{ myTypes[type_name].AddCase(case_name,def);
} // }}}

void SlrParser::AddType(const string &type_name, const Bnf &t) // {{{
{ if (myTypes.find(type_name)!=myTypes.end())
    throw string("SlrParser::AddType: Type ") + type_name + " already exists";
  myTypes.insert(pair<string,SymBnf>(type_name,SymBnf(t)));
} // }}}
