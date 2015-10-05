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
  if (tree->type_name == "num") return a2i(tree->root.content);
  if (tree->case_name == "add") return calc(tree->content[0]) + calc(tree->content[2]);
  if (tree->case_name == "sub") return calc(tree->content[0]) - calc(tree->content[2]);
  if (tree->case_name == "mul") return calc(tree->content[0]) * calc(tree->content[2]);
  if (tree->case_name == "div") return calc(tree->content[0]) / calc(tree->content[2]);
  if (tree->case_name == "num") return calc(tree->content[0]);
  if (tree->case_name == "lvl") return calc(tree->content[0]);
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
    SlrParser parser("exp","defs/exp_sugar.ttd"); // Define parser
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
