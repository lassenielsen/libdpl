#include <iostream>
#include <dpl/parser.hpp>

using namespace dpl;
using namespace std;

int main(int argc, char **argv)
{
  Parser parser;
  parser.DefToken("","[ \t\r\n]+");
  parser.DefSpecialToken("s", "\\^$.|*+?%§\"","()[]{}");
  parser.DefType("tree ::= s | tree tree");
  parsed_tree *tree=parser.Parse("+ ? $ $ ^(() |()) \"[]");
  cout << tree->ToString() << endl;
  delete tree;
  return 0;
}

