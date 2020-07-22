#include <dpl/bnf.hpp>
#include <dpl/tokenizer.hpp>
#include <dpl/common.hpp>
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
  tokenizer.DefToken(":->",":->",3);
  tokenizer.DefToken("id","[^:| \t\r\n][^| \t\r\n]*",4);
  tokenizer.DefToken("string",":\"(\\\\[^]+[^\"])*\"",4);
  return true;
} // }}}

Bnf::Bnf(const string &bnf_string) // {{{
{
  //cout << "Bnf::Bnf(\"" << bnf_string << "\")" << endl;
  if (bnf_string=="")
    return; // Create empty Bnf

  Tokenizer bnf_tokenizer("ll");
  init_bnf_tokenizer(bnf_tokenizer);
  bnf_tokenizer.SetBuffer(bnf_string);
  token type_name = bnf_tokenizer.PopToken();
  if (type_name.Name() != "id" || bnf_tokenizer.Empty()) // Unexpected token
    throw string("Bnf constructor: Expected id but found ") + type_name.Name() + " in " + bnf_string;
  else
    myName=type_name.Content();

  token t = bnf_tokenizer.PopToken();
  if (t.Name() != "::=") // Unexpected token
    throw string("Bnf constructor: Expected ::= but found ") + t.Name() + " in " + bnf_string;

  vector<string> c;
  string cname = "";
  int i=0;
  while (!bnf_tokenizer.Empty()) // Parse definitions
  {
    // Update cname
    if (cname=="")
    { stringstream buf;
      buf << "case";
      buf << ++i;
      cname = buf.str();
    }

    t = bnf_tokenizer.PopToken();

    if (t.Name()=="tag")
    { if (c.size()==0)
        cname=t.Content().substr(2);
      else
        myTags[cname][t.Content().substr(2)]=c.size()-1;
    }
    else if (t.Name() == "|" || t.Name() == "_EOF") // End current production
    {
      myCases[cname]=c;
      cname="";
      c.clear();
    }
    else if (t.Name() == ":->")
    { t = bnf_tokenizer.PopToken();
      if (t.Name()=="string")
        mySugar[cname]=stringreplace(t.Content().substr(2,t.Content().size()-3),"\\\"","\"");
      else // Tokenizer error
           // hack: remove all cases
        throw string("Bnf constructor: Expected sugar-string at: ") + t.Content() + " in: " + bnf_string;
    }
    else if (t.Name() == "id")
      c.push_back(t.Content());
    else // Tokenizer error
         // hack: remove all cases
      throw string("Bnf constructor: Bad BNF at: ") + t.Content() + " in: " + bnf_string;
  }
} // }}}

Bnf::~Bnf() // {{{
{
} // }}}

void Bnf::AddCase(const string &name, const vector<string> &def) // {{{
{
  if (myCases.find(name)!=myCases.end())
  { if (myCases.find(name)->second==def)
      return;
    throw string("Adding case with existing name: ") + name;
  }
  myCases[name]=def;
} // }}}

const vector<string> &Bnf::Case(const string &name) const // {{{
{
  std::map<std::string,std::vector<std::string> >::const_iterator it=myCases.find(name);
  if (it==myCases.end())
    return ourVoidCase;
  
  return it->second;
} // }}}

const map<string,size_t> &Bnf::Tags(const string &name) const // {{{
{
  map<string,map<string,size_t>>::const_iterator it=myTags.find(name);
  if (it==myTags.end())
    return ourVoidTags;
  
  return it->second;
} // }}}

const string &Bnf::Sugar(const string &name) const // {{{
{
  map<string,string>::const_iterator it=mySugar.find(name);
  if (it==mySugar.end())
    return ourVoidSugar;
  
  return it->second;
} // }}}

vector<string> Bnf::CaseNames() const // {{{
{
  vector<string> result;
  for (std::map<std::string,std::vector<std::string> >::const_iterator it=myCases.begin(); it!=myCases.end(); ++it)
    result.push_back(it->first);
  
  return result;
} // }}}

const vector<string> Bnf::ourVoidCase;
const map<string,size_t> Bnf::ourVoidTags;
const string Bnf::ourVoidSugar;
