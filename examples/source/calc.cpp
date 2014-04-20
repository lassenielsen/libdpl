#include <iostream>
#include <sstream>
#include <dpl/symparser.hpp>
using namespace dpl;
using namespace std;

int a2i(const string &a) // {{{
{ // Might be overkill, but it works
  stringstream ss;
  ss << a;
  int i;
  ss >> i;
  return i;
} // }}}

int calc(const parsetree *tree) // {{{
{ // Do the actual calculation
  if (tree->type_name == "number") return a2i(tree->root.content);
  if (tree->type_name == "eqnm" && tree->case_name == "case1") return a2i(tree->content[0]->root.content);
  if (tree->type_name == "eqnm" && tree->case_name == "case2") return calc(tree->content[1]);
  if (tree->type_name == "eqnm" && tree->case_name == "case3") return calc(tree->content[0]) * calc(tree->content[2]);
  if (tree->type_name == "eqnm" && tree->case_name == "case4") return calc(tree->content[0]) / calc(tree->content[2]);
  if (tree->type_name == "eqnm" && tree->case_name == "case5") return -calc(tree->content[1]);
  if (tree->case_name == "case1") return calc(tree->content[0]) + calc(tree->content[2]);
  if (tree->case_name == "case2") return calc(tree->content[0]) - calc(tree->content[2]);
  if (tree->case_name == "case3") return calc(tree->content[0]);
  cout << "Calc error unexpected case: " << tree->type_name << "." << tree->case_name << endl;
  return 0;
} // }}}

int main(int argc, char **argv) // {{{
{ // Main program
  if (argc<2)
  { // check for argument
    cout << "Syntax: calc \"<expression>\"" << endl;
    return 1;
  }

  string exp = argv[1]; // Copy argument
  SymParser parser; // Define parser
  parser.DefToken("","[ \t\r\n][ \t\r\n]*");
  parser.DefGeneralToken("number", "0123456789");
  parser.DefKeywordToken("(");
  parser.DefKeywordToken(")");
  parser.DefKeywordToken("+");
  parser.DefKeywordToken("-");
  parser.DefKeywordToken("~");
  parser.DefKeywordToken("*");
  parser.DefKeywordToken("/");

  parser.DefType("eqn ::= eqn + eqn | eqn - eqn | eqnm");
  parser.DefType("eqnm ::= number | ( eqn ) | eqnm * eqnm | eqnm / eqnm | ~ eqnm");
  
  parsetree *tree=parser.Parse(exp); // Parse argument
  // Calculate and print result
  cout << parser.Unparse(*tree) << " = " << calc(tree) << endl;
  // Clean up
  delete tree;
  return 0;
} // }}}
