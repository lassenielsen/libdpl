#include <dpl/bnf.hpp>
#include <dpl/tokenizer.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include <unistd.h>

using namespace std;
using namespace dpl;

inline bool init_bnf_tokenizer(Tokenizer &tokenizer) // {{{
{
  tokenizer.Clear();
  tokenizer.DefToken("","[ \t\r\n][ \t\r\n]*",0); // Whitespace
  tokenizer.DefToken("::=","::=",1);
  tokenizer.DefToken("|","\\|",2);
  tokenizer.DefToken("tag","::[a-zA-Z0-9_]*",3);
  tokenizer.DefToken("id","[^:| \t\r\n][^| \t\r\n]*",4);
  return true;
} // }}}

Bnf::Bnf(const string &bnf_string) // {{{
{
  if (bnf_string=="")
    return; // Create empty Bnf

  Tokenizer bnf_tokenizer("ll");
  init_bnf_tokenizer(bnf_tokenizer);
  bnf_tokenizer.SetBuffer(bnf_string);
  token type_name = bnf_tokenizer.PopToken();
  if (type_name.name != "id" || bnf_tokenizer.Empty()) // Unexpected token
    throw string("Bnf constructor: Bad BNF: ") + bnf_string;
  else
    myName=type_name.content;

  token t = bnf_tokenizer.PopToken();
  if (t.name != "::=") // Unexpected token
    throw string("Bnf constructor: Bad BNF: ") + bnf_string;

  vector<string> c;
  string cname = "";
  int i=0;
  while (!bnf_tokenizer.Empty()) // Parse definitions
  {
    t = bnf_tokenizer.PopToken();

    if (c.size()==0 && t.name=="tag")
    { cname=t.content.substr(2);
    }
    else if (t.name == "|" || t.name == "_EOF") // End current constructor
    {
      if (cname=="")
      { stringstream buf;
        buf << "case";
        buf << ++i;
        cname = buf.str();
      }
      myCases[cname]=c;
      cname="";
      c.clear();
    }
    else if (t.name == "id")
      c.push_back(t.content);
    else // Tokenizer error
         // hack: remove all cases
      throw string("Bnf constructor: Bad BNF at: ") + t.content + " in: " + bnf_string;
  }
} // }}}

Bnf::~Bnf() // {{{
{
} // }}}

void Bnf::AddCase(const string &name, const vector<string> &def) // {{{
{
  if (myCases.find(name)!=myCases.end())
    throw string("Adding case with existing name: ") + name;

  myCases[name]=def;
} // }}}

const vector<string> &Bnf::Case(const string &name) const // {{{
{
  std::map<std::string,std::vector<std::string> >::const_iterator it=myCases.find(name);
  if (it==myCases.end())
    return ourVoid;
  
  return it->second;
} // }}}

vector<string> Bnf::CaseNames() const // {{{
{
  vector<string> result;
  for (std::map<std::string,std::vector<std::string> >::const_iterator it=myCases.begin(); it!=myCases.end(); ++it)
    result.push_back(it->first);
  
  return result;
} // }}}

vector<string> Bnf::ourVoid;
