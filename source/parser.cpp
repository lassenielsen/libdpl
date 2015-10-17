#include <dpl/parser.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include <unistd.h>

using namespace std;
using namespace dpl;

Parser::Parser() // {{{
: Tokenizer("ll")
{
}; // }}}

string readline(istream &source) // {{{
{
  string result="";
  for (char ch=source.get();ch != '\n' && ch != '\r' && !source.eof(); ch=source.get())
    result += ch;
  for (char ch=source.peek(); (ch=='\n' || ch=='\r') && !source.eof(); ch=source.peek()) // Flush end of line chars
    source.get();
  return result;
} // }}}

string nextline(istream &source) // {{{
{
  string result;
  while ((result.size()==0 || result[0] == '#') && !source.eof())
    result=readline(source);
  return result;
} // }}}

void Parser::LoadFile(const string &filename) // {{{
{
  ifstream fin(filename.c_str());
  Load(fin);
}; // }}}

void Parser::Load(const string &defs) // {{{
{
  stringstream ss;
  ss << defs;
  Load(ss);
}; // }}}

void Parser::Load(istream &fin) // {{{
{
  string line=nextline(fin);
  while (line.size()>0 || !fin.eof())
  {
    string def=line;
    line=nextline(fin);
    while (line.find(":=")==string::npos && !fin.eof())
    { def += "\n" + line;
      line=nextline(fin);
    }
    if (fin.eof() && line.find(":=")==string::npos)
    { def += "\n" + line;
      line ="";
    }

    if (def.find("::=") != string::npos) // type definition
    {
      if (!DefType(def)) // Bad BNF
        throw string("Parser::LoadFile: BNFType error in def: ") + def;
    }
    else if (def.find(":=") != string::npos) // token definition
    {
      if (!DefToken(def)) // Bad Token def
        throw string("Parser::LoadFile: BNFToken error in def: ") + def;
    }
    else
      throw string("Parser::LoadFile: Unknown deftype in def: ") + def;
  }
}; // }}}

Parser::~Parser() // {{{
{
} // }}}

bool Parser::DefType(const string &bnf) // {{{
{
  // Construct new type
  Bnf t(bnf);

  // check if BNF was malformed
  if (t.Empty())
    return false;

  AddType(t.GetName(),t);

  return true;
} // }}}

void extract_tokens(const parsetree &tree, vector<token> &dest) // {{{
{
  if (tree.IsToken())
  { // add the token
    dest.push_back(tree.Token());
  }
  else
  { // add the tokens from the subtrees
    for (size_t i=0; i<tree.Children(); ++i)
      extract_tokens(*tree.Child(i),dest);
  }
  return;
} // }}}

string Parser::Unparse(const parsetree &tree) // {{{
{
  vector<token> tokens;
  tokens.clear();
  extract_tokens(tree,tokens);
  return Serialize(tokens);
} // }}}
