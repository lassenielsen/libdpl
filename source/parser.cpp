#include <dpl/parser.hpp>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;
using namespace dpl;

// bnf_case methods
bnf_case::bnf_case(const string &name) // {{{
 : name(name)
{
} // }}}

static Tokenizer _BNFTokenizer;
int _INIT_BNFTokenizer()
{
  _BNFTokenizer.DefToken("","[ \t\r\n][ \t\r\n]*",0); // Whitespace
  _BNFTokenizer.DefToken("::=","::=",1);
  _BNFTokenizer.DefToken("|","\\|",2);
  _BNFTokenizer.DefToken("id","[^| \t\r\n][^| \t\r\n]*",3);
  return 0;
}
static int x = _INIT_BNFTokenizer();
// bnf_type methods
bnf_type::bnf_type(const string &bnf_string) // {{{
{
  myNullable = false;
  myFirst.clear();
  myLast.clear();
  myPre.clear();
  myPost.clear();
  
  _BNFTokenizer.SetBuffer(bnf_string);
  token type_name = _BNFTokenizer.PopToken();
  if (type_name.name != "id" || _BNFTokenizer.Empty()) // Unexpected token
  {
    myName="";
    //cerr << "bnf_type constructor: Bad BNF: " << bnf_string << endl;
    return;
  }
  else
    myName=type_name.content;

  token t = _BNFTokenizer.PopToken();
  if (t.name != "::=") // Unexpected token
  {
    //cerr << "bnf_type constructor: Bad BNF: " << bnf_string << endl;
    return;
  }

  bnf_case c;
  c.args.clear();
  int i=0;
  while (!_BNFTokenizer.Empty()) // Parse definitions
  {
    t = _BNFTokenizer.PopToken();
    if (t.name == "|" || t.name == "_EOF") // End current constructor
    {
      stringstream buf;
      buf << "case";
      buf << ++i;
      c.name = buf.str();
      myCases.push_back(c);
      c.args.clear();
    }
    else if (t.name == "id")
      c.args.push_back(t.content);
    else // Tokenizer error
         // hack: remove all cases
    {
      myCases.clear();
      cerr << "bnf_type constructor: Bad BNF: " << bnf_string << endl;
      break;
    }
  }
} // }}}

bnf_type::~bnf_type() // {{{
{

} // }}}

const string &bnf_type::Name() // {{{
{
  return myName;
} // }}}

void bnf_type::SetName(const string &name) // {{{
{
  myName = name;
} // }}}

vector<bnf_case> &bnf_type::Cases() // {{{
{
  return myCases;
} // }}}

bool bnf_type::Empty() // {{{
{
  return myCases.size() == 0;
} // }}}

void bnf_type::DefCase(const bnf_case &c) // {{{
{
  myCases.push_back(c);
} // }}}

bool bnf_type::Nullable() // {{{
{
  return myNullable;
} // }}}

void bnf_type::SetNullable(bool value) // {{{
{
  myNullable = value;
} // }}}

parsed_tree bnf_type::VoidRep() // {{{
{
  return myVoidRep;
} // }}}

void bnf_type::SetVoidRep(const parsed_tree &rep) // {{{
{
  myVoidRep = rep;
} // }}}

const set<string> &bnf_type::First() // {{{
{
  return myFirst;
} // }}}

const set<string> &bnf_type::Last() // {{{
{
  return myLast;
} // }}}

const set<string> &bnf_type::Pre() // {{{
{
  return myPre;
} // }}}

const set<string> &bnf_type::Post() // {{{
{
  return myPost;
} // }}}

bool bnf_type::HasFirst(const string &token) // {{{
{
  return (myFirst.find(token) != myFirst.end());
} // }}}

bool bnf_type::HasLast(const string &token) // {{{
{
  return (myLast.find(token) != myLast.end());
} // }}}

bool bnf_type::HasPre(const string &token) // {{{
{
  return (myPre.find(token) != myPre.end());
} // }}}

bool bnf_type::HasPost(const string &token) // {{{
{
  return (myPost.find(token) != myPost.end());
} // }}}

bool bnf_type::AddFirst(const string &token) // {{{
{
  if (myFirst.find(token) == myFirst.end())
  {
    myFirst.insert(token);
    return true;
  }
  return false;
} // }}}

bool bnf_type::AddFirst(const set<string> &source) // {{{
{
  bool updated = false;
  for (set<string>::iterator it = source.begin(); it != source.end(); ++it)
    if (AddFirst(*it))
      updated=true;
  return updated;
} // }}}

bool bnf_type::AddLast(const string &token) // {{{
{
  if (myLast.find(token) == myLast.end())
  {
    myLast.insert(token);
    return true;
  }
  return false;
} // }}}

bool bnf_type::AddLast(const set<string> &source) // {{{
{
  bool updated=false;
  for (set<string>::iterator it = source.begin(); it != source.end(); ++it)
    if (AddLast(*it))
      updated=true;
  return updated;
} // }}}

bool bnf_type::AddPre(const string &token) // {{{
{
  if (myPre.find(token) == myPre.end())
  {
    myPre.insert(token);
    return true;
  }
  return false;
} // }}}

bool bnf_type::AddPre(const set<string> &source) // {{{
{
  bool updated=false;
  for (set<string>::iterator it = source.begin(); it != source.end(); ++it)
    if (AddPre(*it))
      return updated=true;
  return updated;
} // }}}

bool bnf_type::AddPost(const string &token) // {{{
{
  if (myPost.find(token) == myPost.end())
  {
    myPost.insert(token);
    return true;
  }
  return false;
} // }}}

bool bnf_type::AddPost(const set<string> &source) // {{{
{
  bool updated=false;
  for (set<string>::iterator it = source.begin(); it != source.end(); ++it)
    if (AddPost(*it))
      updated=true;
  return updated;
} // }}}

// parsed_tree methods
parsed_tree::parsed_tree(const token &t) // {{{
  : root(t),
    type_name(t.name),
    case_name("_TOKEN"),
    is_token(true)
{
  content.clear();
} // }}}

parsed_tree::parsed_tree(const string &type_name, const string &case_name, vector<parsed_tree*> &content) // {{{
  : root(),
    type_name(type_name),
    case_name(case_name),
    content(content),
    is_token(false)
{
  content.clear();
} // }}}

parsed_tree::parsed_tree(const string &error) // {{{
  : type_name("_ERROR"),
    case_name(error),
    is_token(false)
{
  content.clear();
} // }}}

parsed_tree::parsed_tree(const parsed_tree &rhs) // {{{
{ type_name = rhs.type_name;
  case_name = rhs.case_name;
  is_token = rhs.is_token;
  content.clear();
  for (vector<parsed_tree*>::const_iterator sub=rhs.content.begin(); sub!=rhs.content.end(); ++sub)
    content.push_back(new parsed_tree(**sub));
} // }}}

string parsed_tree::ToString(bool include_cases) // {{{
{
  if (type_name == "_ERROR")
  {
    return type_name + ": " + case_name;
  }
  string result = type_name + " ";
  for (vector<parsed_tree*>::iterator it=content.begin(); it!=content.end(); ++it)
  {
    if ((*it)->case_name != "_TOKEN")
      result += "(";
    result += (*it)->ToString(include_cases);
    if ((*it)->case_name != "_TOKEN")
      result += ")";
  }
  return result;
} // }}}

parsed_tree::~parsed_tree() // {{{
{ while (content.size()>0)
  { delete content.back();
    content.pop_back();
  }
} // }}}

// Parser methods
Parser::Parser() // {{{
: Tokenizer("ll")
{
  myTypes.clear();
}; // }}}

string readline(istream &source) // {{{
{
  string result="";
  for (char ch=source.get();ch != '\n' && ch != '\r'; ch=source.get())
    result += ch;
  for (char ch=source.peek(); ch=='\n' || ch=='\r'; ch=source.peek()) // Flush end of line chars
    source.get();
  return result;
} // }}}

Parser::Parser(const string &filename) // {{{
{
  myTypes.clear();
  ifstream fin(filename.c_str());
  while (!fin.eof())
  {
    string line=readline(fin);
    if (line.size()==0 || line[0] == '#') // comment or empty line
      continue;
    else if (line.find("::=") != string::npos) // type definition
    {
      if (!DefType(line)) // Bad BNF
        cout << "Bad BNF: " << line << endl;
    }
    else if (line.find(":=") != string::npos) // token definition
    {
      if (!DefToken(line)) // Bad Token def
        cout << "Bad token def: " << line << endl;
    }
    else
      cout << "Ignoring line: " << line << endl;
  }
}; // }}}

Parser::~Parser() // {{{
{
} // }}}

void Parser::DefCase(const string &type_name, // {{{
                     const string &case_name,
                     const vector<string> &def)
{
  bnf_case c(case_name);
  GetType(type_name).DefCase(c);
} // }}}

bool Parser::DefType(const string &bnf) // {{{
{
  // Construct new type
  bnf_type t(bnf);

  // check if BNF was malformed
  if (t.Empty())
    return false;

  // Check for any previous definition
  map<string,bnf_type>::iterator def = myTypes.find(t.Name());
  if (def!=myTypes.end()) // There was a previous definition
    if (def->second.Empty()) // Remove empty definition
      myTypes.erase(def); // Abort because of nonempty previous definition
    else
      return false;

  // Insert new type
  myTypes[t.Name()]=t;
  return true;
} // }}}

bnf_type &Parser::GetType(const std::string &name) // {{{
{
  bnf_type &t = myTypes[name];
  if (t.Name() != name) // Fix name of newly constructed bnf_type
    t.SetName(name);
  return t;
} // }}}

bool Parser::IsType(const std::string &name) // {{{
{
  map<string,bnf_type>::iterator it=myTypes.find(name);
  return (it != myTypes.end() && !it->second.Empty());
} // }}}

void Parser::FixNullable() // {{{
{
  bool updated=true;
  while(updated)
  {
    updated=false;
    for (map<string,bnf_type>::iterator it_type=myTypes.begin(); it_type!=myTypes.end(); ++it_type)
    {
      if (it_type->second.Nullable())
        continue;

      for (vector<bnf_case>::iterator it_case=it_type->second.Cases().begin(); it_case!=it_type->second.Cases().end(); ++it_case)
      {
        bool nullable_case = true;
        vector<parsed_tree*> void_seq;
        void_seq.clear();
        for (vector<string>::iterator it_arg=it_case->args.begin(); it_arg!=it_case->args.end(); ++it_arg)
        {
          if (!GetType(*it_arg).Nullable())
            nullable_case = false;
          else
            void_seq.push_back(new parsed_tree(GetType(*it_arg).VoidRep()));
        }
        if (nullable_case)
        {
          it_type->second.SetNullable(true);
          parsed_tree void_rep(it_type->second.Name(),it_case->name,void_seq);
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

void Parser::FixFirst() // {{{
{
  bool updated=true;
  while(updated)
  {
    updated=false;
    for (map<string,bnf_type>::iterator it_type=myTypes.begin(); it_type!=myTypes.end(); ++it_type)
    {
      for (vector<bnf_case>::iterator it_case=it_type->second.Cases().begin(); it_case!=it_type->second.Cases().end(); ++it_case)
      {
        for (vector<string>::iterator it_arg=it_case->args.begin(); it_arg!=it_case->args.end(); ++it_arg)
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

void Parser::FixLast() // {{{
{
  bool updated=true;
  while(updated)
  {
    updated=false;
    for (map<string,bnf_type>::iterator it_type=myTypes.begin(); it_type!=myTypes.end(); ++it_type)
    {
      for (vector<bnf_case>::iterator it_case=it_type->second.Cases().begin(); it_case!=it_type->second.Cases().end(); ++it_case)
      {
        for (vector<string>::reverse_iterator it_arg=it_case->args.rbegin(); it_arg!=it_case->args.rend(); ++it_arg)
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

void Parser::FixPre() // {{{
{
  bool updated=true;
  while(updated)
  {
    updated=false;
    for (map<string,bnf_type>::iterator it_type=myTypes.begin(); it_type!=myTypes.end(); ++it_type)
    {
      for (vector<bnf_case>::iterator it_case=it_type->second.Cases().begin(); it_case!=it_type->second.Cases().end(); ++it_case)
      {
        set<string> preset;
        preset.clear();
        set_include(it_type->second.Pre(),preset);
        for (vector<string>::iterator it_arg=it_case->args.begin(); it_arg!=it_case->args.end(); ++it_arg)
        {
          if (IsType(*it_arg))
          {
            bnf_type &t = GetType(*it_arg);
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

void Parser::FixPost() // {{{
{
  bool updated=true;
  while(updated)
  {
    updated=false;
    for (map<string,bnf_type>::iterator it_type=myTypes.begin(); it_type!=myTypes.end(); ++it_type)
    {
      for (vector<bnf_case>::iterator it_case=it_type->second.Cases().begin(); it_case!=it_type->second.Cases().end(); ++it_case)
      {
        set<string> postset;
        postset.clear();
        set_include(it_type->second.Post(),postset);
        for (vector<string>::reverse_iterator it_arg=it_case->args.rbegin(); it_arg!=it_case->args.rend(); ++it_arg)
        {
          if (IsType(*it_arg))
          {
            bnf_type &t = GetType(*it_arg);
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

void Parser::FixAll() // {{{
{
  FixNullable();
  FixFirst();
  FixLast();
  FixPre();
  FixPost();
} // }}}

bool Parser::make_reduction(vector<parsed_tree*> &peephole, bnf_case this_case, bnf_type &this_type, vector<parsed_tree*> &buffer) // {{{
{
  if (this_case.args.size() ==0) // just check for Pre
  {
    if (peephole.size() == 0 || this_type.Pre().find(peephole.back()->type_name) != this_type.Pre().end())
    {
      vector<parsed_tree*> revbuffer(buffer.rbegin(),buffer.rend());
      buffer.clear();
      parsed_tree *newtree = new parsed_tree(this_type.Name(),this_case.name,revbuffer);
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
  string arg = this_case.args.back();
  this_case.args.pop_back();
  parsed_tree *peep = peephole.back();
  peephole.pop_back();
  if (arg == peep->type_name) // Last elements match
  {
    buffer.push_back(peep);
    if (make_reduction(peephole,this_case, this_type, buffer))
    {
      buffer.pop_back();
      this_case.args.push_back(arg);
      return true;
    }
    //else
    buffer.pop_back();
  }
  peephole.push_back(peep);
  if (IsType(arg) && GetType(arg).Nullable())
  {
    buffer.push_back(new parsed_tree(GetType(arg).VoidRep()));
    if (make_reduction(peephole, this_case, this_type, buffer))
    {
      // FIXME: delete buffer.back(); ???
      buffer.pop_back();
      this_case.args.push_back(arg);
      return true;
    }
    delete buffer.back();
    buffer.pop_back();
  }
  this_case.args.push_back(arg);
  return false;
} // }}}

parsed_tree *Parser::Parse(const string &buffer) // {{{
{
  // Ensure semantical properties are consistent
  FixAll();

  // Check for conflicts
  // Fixme implement this
  // For each pair of type cases check if they overlap, and if they do check if their types have common elements in both their post set and their pre set.
  // Check if it is terminating

  // Initialize buffer
  SetBuffer(buffer);

  vector<parsed_tree*> peephole;
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
      parsed_tree *result = new parsed_tree(msg);
      return result;
    } // }}}
    else if (post_token.name == "_EOF") // No more tokens {{{
    {
      bool reduced=true;
      while (reduced)
      {
        reduced = false;
        for (map<string,bnf_type>::iterator it_type=myTypes.begin();it_type!=myTypes.end() && !reduced;++it_type)
        {
          for (vector<bnf_case>::iterator it_case=it_type->second.Cases().begin();it_case!=it_type->second.Cases().end() && !reduced;++it_case)
            if (it_case->args.size() > 0) // do not use empty cases
            {
              vector<parsed_tree*> buffer;
              buffer.clear();
              if (make_reduction(peephole, *it_case, it_type->second, buffer)) // do reduction if possible
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
        for (map<string,bnf_type>::iterator it_type=myTypes.begin();it_type!=myTypes.end() && !reduced;++it_type)
        {
          if (it_type->second.Post().find(post_token.name) != it_type->second.Post().end()) // post_token in type.Post()
            for (vector<bnf_case>::iterator it_case=it_type->second.Cases().begin();it_case!=it_type->second.Cases().end() && !reduced;++it_case)
              if (it_case->args.size() > 0) // do not use empty cases
              {
                vector<parsed_tree*> buffer;
                buffer.clear();
                if (make_reduction(peephole, *it_case, it_type->second, buffer)) // do reduction if possible
                  reduced=true;
              }
        }
      }
      // Add new token to peephole
      parsed_tree *tree = new parsed_tree(post_token);
      peephole.push_back(tree);
    } // }}}
  }
  SetBuffer("");
  if (peephole.size() != 1)
  {
    string error = "Parser error at:";
    for (vector<parsed_tree*>::iterator it = peephole.begin(); it != peephole.end(); ++it)
    {
      error+= " ";
      error+= (*it)->type_name;
//      error+= ".";
//      error+= it->case_name;
    }
    parsed_tree *result = new parsed_tree(error);
    return result;
  }

  // If no error, return the found tree
  return peephole.front();
} // }}}

void extract_tokens(const parsed_tree &tree, vector<token> &dest) // {{{
{
  if (tree.is_token)
  { // add the token
    dest.push_back(tree.root);
  }
  else
  { // add the tokens from the subtrees
    for (vector<parsed_tree*>::const_iterator it=tree.content.begin(); it!=tree.content.end(); ++it)
      extract_tokens(**it,dest);
  }
  return;
} // }}}

string Parser::Unparse(const parsed_tree &tree) // {{{
{
  vector<token> tokens;
  tokens.clear();
  extract_tokens(tree,tokens);
  return Serialize(tokens);
} // }}}
