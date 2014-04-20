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
  for (char ch=source.get();ch != '\n' && ch != '\r'; ch=source.get())
    result += ch;
  for (char ch=source.peek(); ch=='\n' || ch=='\r'; ch=source.peek()) // Flush end of line chars
    source.get();
  return result;
} // }}}

void Parser::LoadFile(const string &filename) // {{{
{
  ifstream fin(filename.c_str());
  while (!fin.eof())
  {
    string line=readline(fin);
    if (line.size()==0 || line[0] == '#') // comment or empty line
      continue;
    else if (line.find("::=") != string::npos) // type definition
    {
      if (!DefType(line)) // Bad BNF
        throw string("Parser::Parser: BNFType error in file ") + filename + ", line: " + line;
    }
    else if (line.find(":=") != string::npos) // token definition
    {
      if (!DefToken(line)) // Bad Token def
        throw string("Parser::Parser: BNFToken error in file ") + filename + ", line: " + line;
    }
    else
      throw string("Parser::Parser: Ignoring line: ") + line + " in file: " + filename;
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
  if (tree.is_token)
  { // add the token
    dest.push_back(tree.root);
  }
  else
  { // add the tokens from the subtrees
    for (vector<parsetree*>::const_iterator it=tree.content.begin(); it!=tree.content.end(); ++it)
      extract_tokens(**it,dest);
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
