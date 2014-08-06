#include <iostream>
#include <sstream>
#include <dpl/slrparser.hpp>
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
  if (tree->case_name == "add") return calc(tree->content[0]) + calc(tree->content[2]);
  if (tree->case_name == "sub") return calc(tree->content[0]) - calc(tree->content[2]);
  if (tree->case_name == "sub1") return calc(tree->content[0]);
  if (tree->case_name == "mul") return calc(tree->content[0]) * calc(tree->content[2]);
  if (tree->case_name == "div") return calc(tree->content[0]) / calc(tree->content[2]);
  if (tree->case_name == "min") return -calc(tree->content[1]);
  if (tree->case_name == "sub2") return calc(tree->content[0]);
  if (tree->case_name == "int") return a2i(tree->content[0]->root.content);
  if (tree->case_name == "par") return calc(tree->content[1]);
  cout << "Calc error unexpected case: " << tree->type_name << "." << tree->case_name << endl;
  return 0;
} // }}}

int main(int argc, char **argv) // {{{
{ // Main program
  try {
    if (argc<2)
    { // check for argument
      cout << "Syntax: calc \"<expression>\"" << endl;
      return 1;
    }

    string exp = argv[1]; // Copy argument
    SlrParser parser("eqn"); // Define parser
    parser.DefToken("","[ \t\r\n][ \t\r\n]*");
    parser.DefGeneralToken("number", "0123456789");
    parser.DefKeywordToken("(");
    parser.DefKeywordToken(")");
    parser.DefKeywordToken("+");
    parser.DefKeywordToken("-");
    parser.DefKeywordToken("~");
    parser.DefKeywordToken("*");
    parser.DefKeywordToken("/");

    parser.DefType("eqn ::= ::add eqn + eqn1 | ::sub eqn - eqn1 | ::sub1 eqn1");
    parser.DefType("eqn1 ::= ::mul eqn1 * eqn2 | ::div eqn1 / eqn2 | ::min ~ eqn2 | ::sub2 eqn2");
    parser.DefType("eqn2 ::= ::int number | ::par ( eqn )");

    parsetree *tree=parser.Parse(exp); // Parse argument
    // Calculate and print result
    cout << parser.Unparse(*tree) << " = " << calc(tree) << endl;
    // Clean up
    delete tree;
  }
  catch (string &msg)
  { cerr << "Error: " << msg << endl;
    return 1;
  }
  return 0;
} // }}}
