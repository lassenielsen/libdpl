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
    if (post_token.name == "_ERROR") // Tokenizer error {{{
    {
      SetBuffer("");
      string msg= "Tokenizer error: ";
      msg += post_token.content;
      parsetree *result = new parsetree(msg);
      return result;
    } // }}}
    else if (post_token.name == "_EOF") // No more tokens {{{
    {
      bool reduced=true;
      while (reduced)
      {
        reduced = false;
        for (unordered_map<string,SymBnf>::iterator it_type=myTypes.begin();it_type!=myTypes.end() && !reduced;++it_type)
        {
          vector<string> cases=it_type->second.CaseNames();
          for (vector<string>::iterator case_name=cases.begin(); case_name!=cases.end() && !reduced; ++case_name)
            if (it_type->second.Case(*case_name).size() > 0) // do not use empty cases
            {
              vector<parsetree*> buffer;
              buffer.clear();
              vector<string> cpy_case(it_type->second.Case(*case_name));
              if (make_reduction(peephole, *case_name, cpy_case, it_type->second, buffer)) // do reduction if possible
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
        for (unordered_map<string,SymBnf>::iterator it_type=myTypes.begin();it_type!=myTypes.end() && !reduced;++it_type)
        {
          if (it_type->second.Post().find(post_token.name) != it_type->second.Post().end()) // post_token in type.Post()
          {
            vector<string> cases=it_type->second.CaseNames();
            for (vector<string>::iterator case_name=cases.begin(); case_name!=cases.end() && !reduced; ++case_name)
              if (it_type->second.Case(*case_name).size() > 0) // do not use empty cases
              {
                vector<parsetree*> buffer;
                buffer.clear();
                vector<string> case_cpy(it_type->second.Case(*case_name));
                if (make_reduction(peephole, *case_name, case_cpy, it_type->second, buffer)) // do reduction if possible
                  reduced=true;
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
    string error = "Parser error at:";
    for (vector<parsetree*>::iterator it = peephole.begin(); it != peephole.end(); ++it)
    {
      error+= " ";
      error+= (*it)->type_name;
//      error+= ".";
//      error+= it->case_name;
    }
    parsetree *result = new parsetree(error);
    return result;
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
  unordered_map<string,SymBnf>::iterator it=myTypes.find(name);
  return (it != myTypes.end() && !it->second.Empty());
} // }}}

void SymParser::FixNullable() // {{{
{
  bool updated=true;
  while(updated)
  {
    updated=false;
    for (unordered_map<string,SymBnf>::iterator it_type=myTypes.begin(); it_type!=myTypes.end(); ++it_type)
    {
      if (it_type->second.Nullable())
        continue;

      vector<string> cases=it_type->second.CaseNames();
      for (vector<string>::iterator case_name=cases.begin(); case_name!=cases.end(); ++case_name)
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
          it_type->second.SetNullable(true);
          parsetree void_rep(it_type->second.GetName(),*case_name,void_seq);
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
    for (unordered_map<string,SymBnf>::iterator it_type=myTypes.begin(); it_type!=myTypes.end(); ++it_type)
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
    for (unordered_map<string,SymBnf>::iterator it_type=myTypes.begin(); it_type!=myTypes.end(); ++it_type)
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

void SymParser::FixPre() // {{{
{
  bool updated=true;
  while(updated)
  {
    updated=false;
    for (unordered_map<string,SymBnf>::iterator it_type=myTypes.begin(); it_type!=myTypes.end(); ++it_type)
    {
      vector<string> cases=it_type->second.CaseNames();
      for (vector<string>::iterator case_name=cases.begin(); case_name!=cases.end(); ++case_name)
      {
        set<string> preset;
        preset.clear();
        set_include(it_type->second.Pre(),preset);
        for (vector<string>::const_iterator it_arg=it_type->second.Case(*case_name).begin(); it_arg!=it_type->second.Case(*case_name).end(); ++it_arg)
        {
          if (IsType(*it_arg))
          {
            SymBnf &t = GetType(*it_arg);
            if (t.AddPre(preset))
              updated=true;
            if (!t.Nullable())
              preset.clear();
            set_include(t.Last(),preset); // include Last
            preset.insert(*it_arg); // include type
          }
          else
          {
            preset.clear();
            preset.insert(*it_arg);
          }
        }
      }
    }
  }
} // }}}

void SymParser::FixPost() // {{{
{
  bool updated=true;
  while(updated)
  {
    updated=false;
    for (unordered_map<string,SymBnf>::iterator it_type=myTypes.begin(); it_type!=myTypes.end(); ++it_type)
    {
      vector<string> cases=it_type->second.CaseNames();
      for (vector<string>::iterator case_name=cases.begin(); case_name!=cases.end(); ++case_name)
      {
        set<string> postset;
        postset.clear();
        set_include(it_type->second.Post(),postset);
        for (vector<string>::const_reverse_iterator it_arg=it_type->second.Case(*case_name).rbegin(); it_arg!=it_type->second.Case(*case_name).rend(); ++it_arg)
        {
          if (IsType(*it_arg))
          {
            SymBnf &t = GetType(*it_arg);
            if (t.AddPost(postset))
              updated=true;
            if (!t.Nullable())
              postset.clear();
            set_include(t.First(),postset); // include Last
            postset.insert(*it_arg); // include type
          }
          else
          {
            postset.clear();
            postset.insert(*it_arg);
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
  FixPre();
  FixPost();
} // }}}

bool SymParser::make_reduction(vector<parsetree*> &peephole, const string &case_name, vector<string> &case_def, const SymBnf &this_type, vector<parsetree*> &buffer) // {{{
{
  if (case_def.size() ==0) // just check for Pre
  {
    if (peephole.size() == 0 || this_type.HasPre(peephole.back()->type_name))
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
    if (make_reduction(peephole, case_name, case_def, this_type, buffer))
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
    if (make_reduction(peephole, case_name, case_def, this_type, buffer))
    {
      // FIXME: delete buffer.back(); ???
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
{ if (myTypes.find(type_name)!=myTypes.end())
    throw string("SymParser::AddType: Type ") + type_name + " already exists";
  myTypes.insert(pair<string,SymBnf>(type_name,SymBnf(t)));
} // }}}