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
  if (tree->Type() == "num") return a2i(tree->Token().Content());
  if (tree->Case() == "add") return calc(tree->Child("lhs")) + calc(tree->Child("rhs"));
  if (tree->Case() == "sub") return calc(tree->Child("lhs")) - calc(tree->Child("rhs"));
  if (tree->Case() == "mul") return calc(tree->Child("lhs")) * calc(tree->Child("rhs"));
  if (tree->Case() == "div") return calc(tree->Child("lhs")) / calc(tree->Child("rhs"));
  if (tree->Case() == "neg") return -calc(tree->Child("exp"));
  if (tree->Case() == "num") return a2i(tree->Child("val")->Token().Content());
  if (tree->Case() == "par") return calc(tree->Child("exp"));
  if (tree->Case() == "lvl") return calc(tree->Child("exp"));
  cout << "Calc error unexpected case: " << tree->TypeCase() << endl;
  return 0;
} // }}}

// BNFs {{{
const string bnf_exp = "\n\
# A small ttd (Token- and Type-Definitions) file defining a sequence of pairs\n\
\n\
# TOKENS\n\
    := \"[ \\n\\r\\t][ \\n\\r\\t]*\"\n\
num := \"[0-9][0-9]*\"\n\
id  := \"[a-zA-Z][a-zA-Z0-9]*\"\n\
~   := \"~\"\n\
(   := \"\\\\(\"\n\
)   := \"\\\\)\"\n\
+   := \"\\\\+\"\n\
-   := \"\\\\-\"\n\
*   := \"\\\\*\"\n\
/   := \"/\"\n\
,   := \",\"\n\
\n\
# TYPES\n\
exp ::= ::add exp ::lhs + exp2 ::rhs\n\
      | ::sub exp ::lhs - exp2 ::rhs\n\
      | ::lvl exp2 ::exp\n\
exp2 ::= ::mul exp2 ::lhs * exp3 ::rhs\n\
       | ::div exp2 ::lhs / exp3 ::rhs\n\
       | ::lvl exp3 ::exp\n\
exp3 ::= ::neg - exp3 ::exp :-> :\"( 0 - ::exp )\"\n\
       | ::lvl exp4 ::exp\n\
exp4 ::= ::num num ::val\n\
       | ::cal id ::name ( exps ::args )\n\
       | ::par ( exp ::exp )\n\
exps ::= exps2 |\n\
exps2 ::= exp | exp , exps2\n\
"; // }}}

int main(int argc, char **argv) // {{{
{ // Main program
  try {
    if (argc<2)
    { // check for argument
      cout << "Syntax: calc \"<expression>\"" << endl;
      return 1;
    }

    string exp = argv[1]; // Copy argument
    SlrParser parser("exp"); // Define parser
    parser.Load(bnf_exp);

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
